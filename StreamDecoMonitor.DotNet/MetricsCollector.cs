using System.Diagnostics;
using System.Globalization;
using System.Management;
using System.Runtime.InteropServices;
using LibreHardwareMonitor.Hardware;

namespace StreamDecoMonitor;

internal sealed class MetricsCollector : IAsyncDisposable
{
    private readonly CancellationTokenSource stop = new();
    private readonly bool debug;
    private readonly Action<Metrics> publish;
    private Task? worker;
    private Metrics? latest;
    private string status = "Inicializando sensores...";
    private ulong previousIdle;
    private ulong previousTotal;
    private double fallbackFrequency;

    public Metrics? Latest => Volatile.Read(ref latest);
    public string Status => Volatile.Read(ref status);

    public MetricsCollector(Action<Metrics> publish, bool debug)
    {
        this.publish = publish;
        this.debug = debug;
    }

    public void Start() => worker ??= Task.Run(RunAsync);

    private async Task RunAsync()
    {
        var computer = new Computer { IsCpuEnabled = true, IsGpuEnabled = true,
            IsMemoryEnabled = debug, IsMotherboardEnabled = debug, IsControllerEnabled = debug,
            IsNetworkEnabled = debug, IsStorageEnabled = debug };
        bool hardwareAvailable = false;
        try
        {
            try
            {
                computer.Open();
                hardwareAvailable = true;
            }
            catch (Exception exception) { Log.Write($"Hardware initialization: {exception}"); }
            try
            {
                using var search = new ManagementObjectSearcher("SELECT CurrentClockSpeed FROM Win32_Processor");
                using var results = search.Get();
                foreach (ManagementObject processor in results)
                {
                    using (processor) fallbackFrequency = Convert.ToDouble(processor["CurrentClockSpeed"]);
                    break;
                }
            }
            catch (Exception exception) { Log.Write($"CPU frequency fallback: {exception.Message}"); }

            using var timer = new PeriodicTimer(TimeSpan.FromSeconds(1));
            do
            {
                try
                {
                    var sample = Read(computer, hardwareAvailable);
                    Volatile.Write(ref latest, sample);
                    publish(sample);
                }
                catch (Exception exception)
                {
                    Volatile.Write(ref status, $"Falha na coleta: {exception.Message}");
                    Log.Write(Status);
                }
            } while (await timer.WaitForNextTickAsync(stop.Token));
        }
        catch (OperationCanceledException) when (stop.IsCancellationRequested) { }
        finally
        {
            try { computer.Close(); }
            catch (Exception exception) { Log.Write($"Hardware close: {exception.Message}"); }
        }
    }

    private Metrics Read(Computer computer, bool hardwareAvailable)
    {
        double cpuLoad = ReadCpuLoad(), cpuTemp = 0, cpuFreq = fallbackFrequency;
        double gpuLoad = 0, gpuTemp = 0, gpuFreq = 0;
        double? sensorCpuTemp = null, sensorGpuTemp = null, sensorGpuFreq = null;
        double? sensorCpuLoad = null, sensorCpuFreq = null, sensorGpuLoad = null, d3d = null;
        string? hardwareError = null;
        if (hardwareAvailable)
        {
            try
            {
                foreach (var hardware in computer.Hardware)
                    ReadHardware(hardware);
            }
            catch (Exception exception)
            {
                hardwareError = exception.Message;
                Log.Write($"Hardware read: {exception.Message}");
            }
        }
        cpuLoad = sensorCpuLoad ?? cpuLoad;
        cpuTemp = sensorCpuTemp ?? cpuTemp;
        cpuFreq = sensorCpuFreq ?? cpuFreq;
        var gpuFallback = ReadNvidiaFallback();
        gpuLoad = sensorGpuLoad ?? d3d ?? gpuFallback.Load;
        gpuTemp = sensorGpuTemp ?? gpuFallback.Temp;
        gpuFreq = sensorGpuFreq ?? gpuFallback.Freq;
        if (sensorGpuTemp is null && sensorGpuFreq is null && (sensorGpuLoad is not null || d3d is not null))
        {
            gpuTemp = cpuTemp;
            gpuFreq = cpuFreq;
        }

        var memory = new MemoryStatus { Length = (uint)Marshal.SizeOf<MemoryStatus>() };
        if (!GlobalMemoryStatusEx(ref memory))
            throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
        var drive = new DriveInfo("C:\\");
        Volatile.Write(ref status, hardwareError is not null ? $"Sensores: {hardwareError}" :
            sensorCpuTemp is null ? "Temperatura CPU indisponivel; verifique permissoes de administrador." :
            "Sensores ativos");
        return new Metrics(cpuLoad, cpuTemp, cpuFreq, gpuLoad, gpuTemp, gpuFreq,
            (memory.TotalPhysical - memory.AvailablePhysical) / 1048576d, memory.TotalPhysical / 1048576d,
            (drive.TotalSize - drive.TotalFreeSpace) / 1073741824d, drive.TotalSize / 1073741824d, DateTime.Now);

        void ReadHardware(IHardware hardware)
        {
            hardware.Update();
            foreach (var sensor in hardware.Sensors)
            {
                if (sensor.Value is not float value || !float.IsFinite(value)) continue;
                if (debug) Log.Write($"{hardware.Name}: {sensor.SensorType} {sensor.Name} = {value}");
                switch (sensor.SensorType, sensor.Name)
                {
                    case (SensorType.Load, "CPU Total"): sensorCpuLoad = value; break;
                    case (SensorType.Temperature, "CPU Package"): sensorCpuTemp = value; break;
                    case (SensorType.Clock, "CPU Core #1"): sensorCpuFreq = value; break;
                    case (SensorType.Load, "GPU Core"): sensorGpuLoad = value; break;
                    case (SensorType.Load, "D3D 3D"): d3d = value; break;
                    case (SensorType.Temperature, "GPU Core"): sensorGpuTemp = value; break;
                    case (SensorType.Clock, "GPU Core"): sensorGpuFreq = value; break;
                }
            }
            foreach (var child in hardware.SubHardware) ReadHardware(child);
        }
    }

    private double ReadCpuLoad()
    {
        if (!GetSystemTimes(out var idle, out var kernel, out var user)) return 0;
        ulong total = kernel + user;
        ulong elapsed = total - previousTotal;
        double load = previousTotal == 0 || elapsed == 0 ? 0 :
            Math.Clamp(100d * (1d - (double)(idle - previousIdle) / elapsed), 0, 100);
        previousIdle = idle;
        previousTotal = total;
        return load;
    }

    private static (double Load, double Temp, double Freq) ReadNvidiaFallback()
    {
        try
        {
            var start = new ProcessStartInfo("nvidia-smi",
                "--query-gpu=utilization.gpu,temperature.gpu,clocks.mem --format=csv,noheader,nounits")
            { UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
            using var process = Process.Start(start);
            if (process is null) return default;
            var output = process.StandardOutput.ReadToEndAsync();
            var error = process.StandardError.ReadToEndAsync();
            if (!process.WaitForExit(800))
            {
                process.Kill(true);
                process.WaitForExit();
                return default;
            }
            if (process.ExitCode != 0) return default;
            var fields = output.GetAwaiter().GetResult().Split('\n')[0].Split(',');
            return fields.Length >= 3 ? (Parse(fields[0]), Parse(fields[1]), Parse(fields[2])) : default;
        }
        catch (Exception) { return default; }

        static double Parse(string text) => double.TryParse(text.Trim(), NumberStyles.Float,
            CultureInfo.InvariantCulture, out var value) ? value : 0;
    }

    public async ValueTask DisposeAsync()
    {
        stop.Cancel();
        if (worker is not null) await worker;
        stop.Dispose();
    }

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool GlobalMemoryStatusEx(ref MemoryStatus buffer);

    [DllImport("kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool GetSystemTimes(out ulong idle, out ulong kernel, out ulong user);

    [StructLayout(LayoutKind.Sequential)]
    private struct MemoryStatus
    {
        public uint Length, MemoryLoad;
        public ulong TotalPhysical, AvailablePhysical, TotalPageFile, AvailablePageFile;
        public ulong TotalVirtual, AvailableVirtual, AvailableExtendedVirtual;
    }
}