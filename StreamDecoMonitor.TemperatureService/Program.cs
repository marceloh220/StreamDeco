using Microsoft.Extensions.Hosting.WindowsServices;
using StreamDecoMonitor.TemperatureService;

var builder = Host.CreateApplicationBuilder(args);
builder.Services.AddWindowsService(options =>
{
    options.ServiceName = TemperatureWorker.ServiceName;
});
builder.Services.AddHostedService<TemperatureWorker>();

await builder.Build().RunAsync();
