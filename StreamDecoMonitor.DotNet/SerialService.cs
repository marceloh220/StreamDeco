using System.IO.Ports;
using System.Management;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Channels;

namespace StreamDecoMonitor;

internal sealed record SerialAdapter(string Name, string VendorId, string[] ProductIds, string[] NameMarkers)
{
    public bool Matches(string description, string deviceInstanceId)
    {
        if (NameMarkers.Any(marker => description.Contains(marker, StringComparison.OrdinalIgnoreCase)))
            return true;

        var hardwareId = Regex.Match(deviceInstanceId, @"VID_([0-9A-F]{4})&PID_([0-9A-F]{4})",
            RegexOptions.IgnoreCase);
        return hardwareId.Success &&
            hardwareId.Groups[1].Value.Equals(VendorId, StringComparison.OrdinalIgnoreCase) &&
            ProductIds.Contains(hardwareId.Groups[2].Value, StringComparer.OrdinalIgnoreCase);
    }
}

internal static class SerialAdapters
{
    public static readonly IReadOnlyList<SerialAdapter> Known =
    [
        new("WCH CH34x/CH91xx", "1A86", ["7523"], ["CH340", "CH341", "CH910"]),
        new("Silicon Labs CP210x", "10C4", ["EA60"], ["CP210"])
    ];

    public static bool IsCompatible(string description, string deviceInstanceId) =>
        Known.Any(adapter => adapter.Matches(description, deviceInstanceId));
}

internal sealed record SerialDevice(string Port, string Description, string DeviceInstanceId = "")
{
    public bool Compatible => SerialAdapters.IsCompatible(Description, DeviceInstanceId);
    public override string ToString() => $"{Port} - {Description}{(Compatible ? " [compativel]" : "")}";
}

internal sealed record SerialDiagnostics(string Port = "--", bool Connected = false, bool Running = false,
    int ConnectAttempts = 0, int SendFailures = 0, string LastEvent = "no serial device",
    string LastEventTime = "--:--:--", string LastError = "none", string LastPayload = "",
    string LastPayloadTime = "--:--:--");

internal interface ISerialConnection : IDisposable
{
    void Open();
    void Write(string payload);
}

internal sealed class SerialConnection(string name) : ISerialConnection
{
    private readonly SerialPort port = new(name, 115200, Parity.None, 8, StopBits.One)
    { ReadTimeout = 1000, WriteTimeout = 1000, Handshake = Handshake.None,
        DtrEnable = true, RtsEnable = true, Encoding = Encoding.UTF8 };

    public void Open() => port.Open();
    public void Write(string payload)
    {
        port.Write(payload);
        port.BaseStream.Flush();
    }
    public void Dispose() => port.Dispose();
}

internal sealed class SerialService : IAsyncDisposable
{
    private readonly Channel<string> payloads = Channel.CreateBounded<string>(new BoundedChannelOptions(1)
    { FullMode = BoundedChannelFullMode.DropOldest, SingleReader = true, SingleWriter = true });
    private readonly CancellationTokenSource stop = new();
    private readonly Func<string, ISerialConnection> createConnection;
    private readonly Task worker;
    private SerialDiagnostics snapshot = new();
    private string requestedPort = "";
    private int revision;

    public SerialDiagnostics Diagnostics => Volatile.Read(ref snapshot);

    public SerialService(Func<string, ISerialConnection>? createConnection = null)
    {
        this.createConnection = createConnection ?? (port => new SerialConnection(port));
        worker = Task.Run(RunAsync);
    }
    public void Publish(Metrics metrics) => payloads.Writer.TryWrite(metrics.ToSerialPayload());
    public void Connect(string port)
    {
        Volatile.Write(ref requestedPort, port);
        Interlocked.Increment(ref revision);
    }

    public static IReadOnlyList<SerialDevice> ListPorts()
    {
        var names = SerialPort.GetPortNames();
        var descriptions = new Dictionary<string, (string Name, string DeviceInstanceId)>(StringComparer.OrdinalIgnoreCase);
        try
        {
            using var search = new ManagementObjectSearcher(
                "SELECT Name, PNPDeviceID FROM Win32_PnPEntity WHERE PNPClass = 'Ports'");
            using var results = search.Get();
            foreach (ManagementObject device in results)
            {
                using (device)
                {
                    string name = Convert.ToString(device["Name"]) ?? "";
                    string deviceInstanceId = Convert.ToString(device["PNPDeviceID"]) ?? "";
                    var match = Regex.Match(name, @"\((COM\d+)\)\s*$", RegexOptions.IgnoreCase);
                    if (match.Success)
                    {
                        string port = match.Groups[1].Value;
                        if (names.Contains(port, StringComparer.OrdinalIgnoreCase))
                            descriptions[port] = (name, deviceInstanceId);
                    }
                }
            }
        }
        catch (Exception exception) { Log.Write($"PnP COM descriptions: {exception.Message}"); }
        try
        {
            using var search = new ManagementObjectSearcher(
                "SELECT DeviceID, Name, PNPDeviceID FROM Win32_SerialPort");
            using var results = search.Get();
            foreach (ManagementObject device in results)
            {
                using (device)
                {
                    string port = Convert.ToString(device["DeviceID"]) ?? "";
                    if (!descriptions.ContainsKey(port))
                        descriptions[port] = (Convert.ToString(device["Name"]) ?? "",
                            Convert.ToString(device["PNPDeviceID"]) ?? "");
                }
            }
        }
        catch (Exception exception) { Log.Write($"COM enumeration: {exception.Message}"); }
        return names.OrderBy(name => name, StringComparer.OrdinalIgnoreCase)
            .Select(name => descriptions.TryGetValue(name, out var device)
                ? new SerialDevice(name, device.Name, device.DeviceInstanceId)
                : new SerialDevice(name, "Serial port")).ToArray();
    }

    private async Task RunAsync()
    {
        ISerialConnection? connection = null;
        int activeRevision = -1;
        DateTime retryAfter = DateTime.MinValue;
        try
        {
            await foreach (var queued in payloads.Reader.ReadAllAsync(stop.Token))
            {
                string payload = queued;
                while (payloads.Reader.TryRead(out var newer)) payload = newer;
                if (activeRevision != Volatile.Read(ref revision))
                {
                    CloseConnection();
                    activeRevision = Volatile.Read(ref revision);
                    var port = Volatile.Read(ref requestedPort);
                    Update(Diagnostics with { Port = string.IsNullOrEmpty(port) ? "--" : port,
                        Running = !string.IsNullOrEmpty(port), Connected = false }, "port changed");
                    retryAfter = DateTime.MinValue;
                }
                if (!Diagnostics.Running || stop.IsCancellationRequested) continue;
                if (connection is null)
                {
                    if (DateTime.UtcNow < retryAfter) continue;
                    Update(Diagnostics with { ConnectAttempts = Diagnostics.ConnectAttempts + 1 }, "connecting");
                    try
                    {
                        connection = createConnection(Diagnostics.Port);
                        connection.Open();
                        Update(Diagnostics with { Connected = true, LastError = "none" }, "connected");
                    }
                    catch (Exception exception)
                    {
                        CloseConnection();
                        Update(Diagnostics with { Connected = false, LastError = exception.Message }, "connect failed");
                        retryAfter = DateTime.UtcNow.AddSeconds(1);
                        continue;
                    }
                }
                try
                {
                    connection.Write(payload);
                    Update(Diagnostics with { LastPayload = payload, LastPayloadTime = DateTime.Now.ToString("HH:mm:ss") }, "payload sent");
                }
                catch (Exception exception)
                {
                    CloseConnection();
                    Update(Diagnostics with { Connected = false, SendFailures = Diagnostics.SendFailures + 1,
                        LastError = exception.Message }, "send failed");
                    retryAfter = DateTime.UtcNow.AddSeconds(1);
                }
            }
        }
        catch (OperationCanceledException) when (stop.IsCancellationRequested) { }
        finally
        {
            CloseConnection();
            Update(Diagnostics with { Connected = false, Running = false }, "worker stopped");
        }

        void CloseConnection()
        {
            try { connection?.Dispose(); }
            catch (Exception exception) { Log.Write($"Serial close: {exception.Message}"); }
            connection = null;
        }
    }

    private void Update(SerialDiagnostics state, string message)
    {
        Volatile.Write(ref snapshot, state with { LastEvent = message, LastEventTime = DateTime.Now.ToString("HH:mm:ss") });
        if (message != "payload sent") Log.Write($"Serial {state.Port}: {message}; {state.LastError}");
    }

    public async ValueTask DisposeAsync()
    {
        stop.Cancel();
        payloads.Writer.TryComplete();
        await worker;
        stop.Dispose();
    }
}