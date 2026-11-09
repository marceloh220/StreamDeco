using System.Globalization;
using System.Collections.Concurrent;
using StreamDecoMonitor;

var ch340 = new SerialDevice("COM5", "USB-SERIAL CH340 (COM5)", @"USB\VID_1A86&PID_7523\DEVICE");
var cp210x = new SerialDevice("COM6", "USB Serial Device (COM6)", @"USB\VID_10C4&PID_EA60\DEVICE");
var cp210xByName = new SerialDevice("COM7", "CP2107 USB to UART Bridge (COM7)");
var unknownAdapter = new SerialDevice("COM8", "USB Serial Device (COM8)", @"USB\VID_1A86&PID_FFFF\DEVICE");
if (!ch340.Compatible || !cp210x.Compatible || !cp210xByName.Compatible || unknownAdapter.Compatible)
    throw new Exception("CH340 and CP210x devices must be detected by name or USB vendor ID.");
Console.WriteLine("PASS: CH340 and CP210x port compatibility detection.");

CultureInfo.CurrentCulture = CultureInfo.GetCultureInfo("pt-BR");
var metrics = new Metrics(12.9, 45.8, 3599.9, 78.1, 60.9, 1700.9,
    8192.9, 16384.9, 120.9, 500.9, new DateTime(2026, 11, 8, 14, 5, 9));
const string expected = "12, 45, 3599, 78, 60, 1700, 8192, 16384, 120, 500, 9, 5, 14, 0, 8, 11, 2026/";
if (metrics.ToSerialPayload() != expected)
    throw new Exception($"Payload mismatch: {metrics.ToSerialPayload()}");
for (int weekday = 0; weekday < 7; weekday++)
{
    var sample = metrics with { Timestamp = metrics.Timestamp.AddDays(weekday) };
    var fields = sample.ToSerialPayload().TrimEnd('/').Split(", ");
    if (fields.Length != 17 || fields[13] != weekday.ToString())
        throw new Exception("Field count or weekday mismatch.");
}
if ((metrics with { CpuLoad = -12.9 }).ToSerialPayload().Split(", ")[0] != "-12")
    throw new Exception("Values must truncate toward zero, like Python int().");
Console.WriteLine("PASS: exact payload, 17 fields, pt-BR culture, Sunday=0, all weekdays, truncation, no newline.");

var connections = new ConcurrentBag<FakeConnection>();
int factoryCalls = 0;
await using (var serial = new SerialService(port =>
{
    var connection = new FakeConnection(port, Interlocked.Increment(ref factoryCalls) == 1);
    connections.Add(connection);
    return connection;
}))
{
    serial.Connect("TEST-A");
    await PumpUntilAsync(() => serial.Diagnostics.LastPayload == expected, () => serial.Publish(metrics));
    if (serial.Diagnostics.ConnectAttempts != 2 || !serial.Diagnostics.Connected || serial.Diagnostics.LastError != "none")
        throw new Exception("Connection failure must retry and clear the error after reconnect.");
    var active = connections.Single(connection => connection.IsOpen);
    active.FailNextWrite = true;
    await PumpUntilAsync(() => serial.Diagnostics.SendFailures == 1, () => serial.Publish(metrics));
    await PumpUntilAsync(() => serial.Diagnostics.Connected, () => serial.Publish(metrics));
    if (!active.Disposed) throw new Exception("Failed connection must be disposed.");
    serial.Connect("TEST-B");
    var newest = metrics with { CpuLoad = 99 };
    await PumpUntilAsync(() => serial.Diagnostics.Port == "TEST-B" && serial.Diagnostics.LastPayload == newest.ToSerialPayload(),
        () => serial.Publish(newest));
    if (connections.Any(connection => connection.Payloads.Any(payload => !payload.EndsWith('/') || payload.Contains('\n'))))
        throw new Exception("Transport must receive exact unframed payloads.");
}
if (connections.Any(connection => !connection.Disposed)) throw new Exception("Shutdown must close every connection.");
Console.WriteLine("PASS: connect failure/retry, write failure/recovery, port switch, last payload diagnostics, clean shutdown; no real COM used.");

var enteredWrite = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
using var releaseWrite = new ManualResetEventSlim(false);
var slow = new FakeConnection("TEST-QUEUE", false)
{
    BeforeWrite = () => { enteredWrite.TrySetResult(); releaseWrite.Wait(TimeSpan.FromSeconds(5)); }
};
await using (var serial = new SerialService(_ => slow))
{
    serial.Connect("TEST-QUEUE");
    serial.Publish(metrics);
    await enteredWrite.Task.WaitAsync(TimeSpan.FromSeconds(5));
    var newest = metrics with { CpuLoad = 99 };
    try
    {
        for (int value = 20; value < 100; value++) serial.Publish(metrics with { CpuLoad = value });
    }
    finally { releaseWrite.Set(); }
    await PumpUntilAsync(() => serial.Diagnostics.LastPayload == newest.ToSerialPayload(), () => { });
    if (slow.Payloads.Count != 2) throw new Exception("Queue must discard intermediate stale samples.");
}
Console.WriteLine("PASS: bounded queue transmits only newest sample after a blocked write.");

static async Task PumpUntilAsync(Func<bool> condition, Action publish)
{
    var deadline = System.Diagnostics.Stopwatch.StartNew();
    while (!condition())
    {
        if (deadline.Elapsed > TimeSpan.FromSeconds(8)) throw new Exception("Serial check timed out.");
        publish();
        await Task.Delay(100);
    }
}

sealed class FakeConnection(string port, bool failOpen) : ISerialConnection
{
    public ConcurrentQueue<string> Payloads { get; } = new();
    public bool IsOpen { get; private set; }
    public bool Disposed { get; private set; }
    public bool FailNextWrite { get; set; }
    public Action? BeforeWrite { get; init; }
    public void Open()
    {
        if (failOpen) throw new IOException($"Simulated open failure on {port}");
        IsOpen = true;
    }
    public void Write(string payload)
    {
        BeforeWrite?.Invoke();
        if (FailNextWrite)
        {
            FailNextWrite = false;
            throw new IOException("Simulated cable disconnect");
        }
        Payloads.Enqueue(payload);
    }
    public void Dispose() { Disposed = true; IsOpen = false; }
}