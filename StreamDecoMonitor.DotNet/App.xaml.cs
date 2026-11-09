using System.Windows;

namespace StreamDecoMonitor;

public partial class App : Application
{
    private Mutex? instanceMutex;
    private bool ownsMutex;

    protected override void OnStartup(StartupEventArgs args)
    {
        base.OnStartup(args);
        bool smokeTest = args.Args.Contains("--smoke-test");
        string mutexName = smokeTest ? "StreamDecoMonitor.DotNet.SmokeTest" : "Global\\StreamDecoCustomTkinterPreview";
        instanceMutex = new Mutex(true, mutexName, out ownsMutex);
        if (!ownsMutex)
        {
            Shutdown();
            return;
        }
        MainWindow = new MainWindow(args.Args.Contains("--debug"), smokeTest);
        MainWindow.Show();
    }

    protected override void OnExit(ExitEventArgs args)
    {
        if (ownsMutex) instanceMutex?.ReleaseMutex();
        instanceMutex?.Dispose();
        base.OnExit(args);
    }
}