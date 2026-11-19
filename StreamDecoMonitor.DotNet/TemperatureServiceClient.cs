using System.Globalization;
using System.IO.Pipes;

namespace StreamDecoMonitor;

internal sealed class TemperatureServiceClient
{
    private const string PipeName = "StreamDeco.Temperature";
    private DateTime nextQuery;

    public (double? Cpu, double? Gpu) Temperatures { get; private set; }

    public void Refresh()
    {
        var now = DateTime.UtcNow;
        if (now < nextQuery) return;
        nextQuery = now.AddSeconds(2);

        try
        {
            using var client = new NamedPipeClientStream(
                ".", PipeName, PipeDirection.In, PipeOptions.None);
            client.Connect(200);
            using var reader = new StreamReader(client);
            var payload = reader.ReadLine();
            if (string.IsNullOrWhiteSpace(payload)) return;
            var fields = payload.Split(',');
            if (fields.Length != 2) return;
            Temperatures = (Parse(fields[0]), Parse(fields[1]));
        }
        catch (IOException)
        {
            Temperatures = (null, null);
            nextQuery = now.AddSeconds(10);
        }
        catch (TimeoutException)
        {
            Temperatures = (null, null);
            nextQuery = now.AddSeconds(10);
        }
    }

    private static double? Parse(string value) =>
        double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out var result)
            ? result
            : null;
}
