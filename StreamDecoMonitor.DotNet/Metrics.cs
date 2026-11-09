using System.Globalization;

namespace StreamDecoMonitor;

public sealed record Metrics(
    double CpuLoad, double CpuTemp, double CpuFreq,
    double GpuLoad, double GpuTemp, double GpuFreq,
    double RamUsed, double RamTotal, double DiskUsed, double DiskTotal,
    DateTime Timestamp)
{
    public string ToSerialPayload()
    {
        double[] fields = [CpuLoad, CpuTemp, CpuFreq, GpuLoad, GpuTemp, GpuFreq,
            RamUsed, RamTotal, DiskUsed, DiskTotal, Timestamp.Second, Timestamp.Minute,
            Timestamp.Hour, (int)Timestamp.DayOfWeek, Timestamp.Day, Timestamp.Month, Timestamp.Year];
        return string.Join(", ", fields.Select(value =>
            ((int)(double.IsFinite(value) ? value : 0)).ToString(CultureInfo.InvariantCulture))) + "/";
    }

    public string ToPlainText() =>
        $"cpu: {(int)CpuLoad}%  temp: {(int)CpuTemp}C  freq: {(int)CpuFreq}MHz\n" +
        $"gpu: {(int)GpuLoad}%  temp: {(int)GpuTemp}C  freq: {(int)GpuFreq}MHz\n" +
        $"ram: {(int)RamUsed}/{(int)RamTotal} MB\ndisk: {(int)DiskUsed}/{(int)DiskTotal} GB";
}