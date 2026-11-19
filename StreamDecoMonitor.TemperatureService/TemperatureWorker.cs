using System.IO.Pipes;
using System.Security.AccessControl;
using System.Security.Principal;
using LibreHardwareMonitor.Hardware;

namespace StreamDecoMonitor.TemperatureService;

internal sealed class TemperatureWorker(ILogger<TemperatureWorker> logger) : BackgroundService
{
    public const string ServiceName = "StreamDecoTemperatureService";
    public const string PipeName = "StreamDeco.Temperature";

    private readonly object gate = new();
    private readonly Computer computer = new()
    {
        IsCpuEnabled = true,
        IsGpuEnabled = false
    };
    private TemperatureSnapshot snapshot = new();

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        try
        {
            computer.Open();
            logger.LogInformation("CPU temperature sensors initialized.");
        }
        catch (Exception exception)
        {
            logger.LogCritical(exception, "Unable to initialize hardware sensors.");
            throw;
        }

        var serveClients = ServeClientsAsync(stoppingToken);
        try
        {
            using var timer = new PeriodicTimer(TimeSpan.FromSeconds(1));
            do
            {
                UpdateTemperatures();
            } while (await timer.WaitForNextTickAsync(stoppingToken));
        }
        catch (OperationCanceledException) when (stoppingToken.IsCancellationRequested) { }
        await serveClients;
    }

    private void UpdateTemperatures()
    {
        double? cpu = null;
        double? gpu = null;
        try
        {
            foreach (var hardware in computer.Hardware)
            {
                ReadHardware(hardware);
            }
        }
        catch (Exception exception)
        {
            logger.LogWarning(exception, "Unable to update hardware temperatures.");
            return;
        }

        lock (gate)
        {
            snapshot = new TemperatureSnapshot(cpu, gpu);
        }

        void ReadHardware(IHardware hardware)
        {
            hardware.Update();
            foreach (var sensor in hardware.Sensors)
            {
                if (sensor.Value is not float value || !float.IsFinite(value))
                    continue;

                if (sensor.SensorType == SensorType.Temperature &&
                    (sensor.Name == "CPU Package" || (cpu is null && sensor.Name.Contains("CPU", StringComparison.Ordinal))))
                {
                    cpu = cpu is null || sensor.Name == "CPU Package" ? value : cpu;
                }
                if (sensor.SensorType == SensorType.Temperature &&
                    (sensor.Name == "GPU Core" || (gpu is null && sensor.Name.Contains("GPU", StringComparison.Ordinal))))
                {
                    gpu = gpu is null || sensor.Name == "GPU Core" ? value : gpu;
                }
            }
            foreach (var child in hardware.SubHardware)
            {
                ReadHardware(child);
            }
        }
    }

    private async Task ServeClientsAsync(CancellationToken stoppingToken)
    {
        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await using var server = NamedPipeServerStreamAcl.Create(
                    PipeName,
                    PipeDirection.Out,
                    NamedPipeServerStream.MaxAllowedServerInstances,
                    PipeTransmissionMode.Byte,
                    PipeOptions.Asynchronous,
                    0,
                    0,
                    CreatePipeSecurity());
                await server.WaitForConnectionAsync(stoppingToken);
                await using var writer = new StreamWriter(server) { AutoFlush = true };
                TemperatureSnapshot current;
                lock (gate)
                {
                    current = snapshot;
                }
                await writer.WriteLineAsync(
                    $"{Format(current.Cpu)},{Format(current.Gpu)}");
            }
            catch (OperationCanceledException) when (stoppingToken.IsCancellationRequested) { }
            catch (IOException) { }
            catch (Exception exception)
            {
                logger.LogError(exception, "Unable to serve a temperature client.");
                try
                {
                    await Task.Delay(TimeSpan.FromSeconds(1), stoppingToken);
                }
                catch (OperationCanceledException) when (stoppingToken.IsCancellationRequested) { }
            }
        }
    }

    private static string Format(double? value) =>
        value?.ToString(System.Globalization.CultureInfo.InvariantCulture) ?? "";

    private static PipeSecurity CreatePipeSecurity()
    {
        var security = new PipeSecurity();
        security.AddAccessRule(new PipeAccessRule(
            new SecurityIdentifier(WellKnownSidType.LocalSystemSid, null),
            PipeAccessRights.FullControl,
            AccessControlType.Allow));
        security.AddAccessRule(new PipeAccessRule(
            new SecurityIdentifier(WellKnownSidType.BuiltinAdministratorsSid, null),
            PipeAccessRights.FullControl,
            AccessControlType.Allow));
        security.AddAccessRule(new PipeAccessRule(
            new SecurityIdentifier(WellKnownSidType.AuthenticatedUserSid, null),
            PipeAccessRights.Read,
            AccessControlType.Allow));
        return security;
    }

    public override async Task StopAsync(CancellationToken cancellationToken)
    {
        try
        {
            computer.Close();
        }
        catch (Exception exception)
        {
            logger.LogWarning(exception, "Unable to close hardware sensors.");
        }
        await base.StopAsync(cancellationToken);
    }

    private readonly record struct TemperatureSnapshot(double? Cpu, double? Gpu);
}
