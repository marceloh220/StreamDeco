using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shell;
using System.Windows.Shapes;
using System.Windows.Threading;
using Forms = System.Windows.Forms;

namespace StreamDecoMonitor;

public sealed class MainWindow : Window
{
    private const int WmGetMinMaxInfo = 0x0024;
    private const uint MonitorDefaultToNearest = 2;
    private static readonly string[] Modes = ["MONITOR", "PERFORMANCE", "CLOCK", "DIAGNOSTICS"];
    private readonly SerialService serial = new();
    private readonly MetricsCollector collector;
    private readonly DispatcherTimer refresh = new() { Interval = TimeSpan.FromMilliseconds(500) };
    private readonly DispatcherTimer noticeTimer = new() { Interval = TimeSpan.FromMilliseconds(3200) };
    private readonly Grid main = new();
    private readonly CircularGauge cpuGauge = new(), gpuGauge = new();
    private readonly StatBar cpuTemp = new("0 C"), cpuFreq = new("0 MHz");
    private readonly StatBar gpuTemp = new("0 C"), gpuFreq = new("0 MHz");
    private readonly StatBar ram = new("RAM: 0/0 MB"), disk = new("C: 0/0 GB");
    private readonly TextBlock clock = Theme.Label("00:00:00", 52, true);
    private readonly TextBlock date = Theme.Label("01/01/2026", 30, true);
    private readonly TextBlock status = Theme.Label("MODE: MONITOR", 14, true);
    private readonly TextBlock diagnosticSerial = DiagnosticText();
    private readonly TextBlock diagnosticMetrics = DiagnosticText();
    private readonly TextBlock diagnosticPayload = DiagnosticText();
    private readonly TextBlock sideDiagnostic = DiagnosticText();
    private readonly Rectangle connectionIcon = new() { Width = 64, Height = 64 };
    private readonly Button pauseButton;
    private readonly Button serialButton;
    private readonly FrameworkElement cpuSection, gpuSection, memorySection, clockSection, diagnosticSection;
    private readonly StackPanel clockBody = new();
    private readonly Forms.NotifyIcon tray;
    private readonly System.Drawing.Icon trayIcon;
    private readonly bool smokeTest;
    private Window? portPicker;
    private Metrics? displayed;
    private int mode;
    private bool paused, exiting, discovering, noticeActive;

    public MainWindow(bool debug, bool smokeTest)
    {
        this.smokeTest = smokeTest;
        Title = "StreamDeco Monitor - .NET";
        WindowStyle = WindowStyle.None;
        ResizeMode = ResizeMode.CanResize;
        WindowChrome.SetWindowChrome(this, new WindowChrome
        {
            CaptionHeight = 44,
            ResizeBorderThickness = new Thickness(6),
            GlassFrameThickness = new Thickness(0),
            UseAeroCaptionButtons = false
        });
        Width = 1024;
        Height = 640;
        MinWidth = 960;
        MinHeight = 640;
        WindowStartupLocation = WindowStartupLocation.CenterScreen;
        Background = Theme.Brush("#f3a45f");
        FontFamily = new FontFamily("Segoe UI");
        Icon = LoadImage("streamDeco.ico");

        var content = new Grid { Margin = new Thickness(20) };
        content.ColumnDefinitions.Add(new ColumnDefinition());
        content.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(170) });
        var surface = new Border { Background = Theme.Panel, CornerRadius = new CornerRadius(14),
            Margin = new Thickness(0, 0, 20, 0), Child = main };
        content.Children.Add(surface);
        for (int index = 0; index < 2; index++)
        {
            main.ColumnDefinitions.Add(new ColumnDefinition());
            main.RowDefinitions.Add(new RowDefinition());
        }
        cpuSection = BuildProcessor("CPU", "processor_22_simp.png", cpuGauge, cpuTemp, cpuFreq);
        gpuSection = BuildProcessor("GPU", "gpu_22_simp.png", gpuGauge, gpuTemp, gpuFreq);
        var memoryBody = new StackPanel();
        memoryBody.Children.Add(ram);
        memoryBody.Children.Add(disk);
        memorySection = BuildSection("MEM", "ram_22_simp.png", memoryBody);
        clockBody.Children.Add(clock);
        clockBody.Children.Add(date);
        clockSection = BuildSection("CLOCK", "clock_22_simp.png", clockBody);
        var diagnosticBody = new StackPanel();
        AddDiagnostic(diagnosticBody, "SERIAL DEBUG", diagnosticSerial);
        AddDiagnostic(diagnosticBody, "METRICS (TEXT)", diagnosticMetrics);
        AddDiagnostic(diagnosticBody, "SERIAL PAYLOAD", diagnosticPayload);
        var scroll = new ScrollViewer { Content = diagnosticBody, VerticalScrollBarVisibility = ScrollBarVisibility.Auto };
        diagnosticSection = BuildSection("DIAGNOSTICS", "diagnostic_22_simp.png", scroll);
        foreach (var section in new[] { cpuSection, memorySection, gpuSection, clockSection, diagnosticSection })
            main.Children.Add(section);

        var side = new StackPanel();
        Grid.SetColumn(side, 1);
        content.Children.Add(side);
        side.Children.Add(SideButton("\uE76C", "Alternar visualizacao", (_, _) => CycleMode()));
        pauseButton = SideButton("\uE769", "Pausar / retomar widgets", (_, _) => TogglePause());
        side.Children.Add(pauseButton);
        var home = SideButton("\uE80F", "Monitor", (_, _) => { mode = 0; ApplyMode(); });
        home.Background = Theme.Brush("#304d5e");
        home.Foreground = Theme.Text;
        side.Children.Add(home);
        status.Foreground = Theme.Brush("#25313c");
        status.TextAlignment = TextAlignment.Center;
        status.Margin = new Thickness(0, 8, 0, 8);
        side.Children.Add(status);
        connectionIcon.OpacityMask = new ImageBrush(LoadImage("streamDeco.ico"));
        connectionIcon.Fill = Theme.Brush("#e15454");
        serialButton = new Button { Content = connectionIcon, Background = Brushes.Transparent,
            Width = 84, Height = 84, ToolTip = "Reconectar dispositivo" };
        serialButton.Click += async (_, _) => await ReconnectAsync();
        AutomationProperties.SetName(serialButton, "Reconectar dispositivo serial");
        side.Children.Add(serialButton);
        if (debug)
        {
            sideDiagnostic.FontSize = 11;
            sideDiagnostic.Margin = new Thickness(8);
            side.Children.Add(new Border { Background = Theme.Brush("#20303c"),
                CornerRadius = new CornerRadius(8), Child = sideDiagnostic });
            serialButton.Visibility = Visibility.Collapsed;
        }
        var context = new ContextMenu();
        var choosePort = new MenuItem { Header = "Selecionar porta serial..." };
        choosePort.Click += (_, _) => OpenPortPicker();
        context.Items.Add(choosePort);
        var reconnect = new MenuItem { Header = "Reconectar dispositivo" };
        reconnect.Click += async (_, _) => await ReconnectAsync();
        context.Items.Add(reconnect);
        content.ContextMenu = context;
        var windowRoot = new Grid();
        windowRoot.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
        windowRoot.RowDefinitions.Add(new RowDefinition());
        var titleBar = BuildTitleBar(out var maximizeButton);
        windowRoot.Children.Add(titleBar);
        Grid.SetRow(content, 1);
        windowRoot.Children.Add(content);
        Content = windowRoot;
        SourceInitialized += (_, _) =>
        {
            var source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
            source?.AddHook(HandleWindowMessage);
        };

        collector = new MetricsCollector(serial.Publish, debug);
        trayIcon = new System.Drawing.Icon(Asset("streamDeco.ico"));
        tray = new Forms.NotifyIcon { Icon = trayIcon, Text = "StreamDeco Monitor", Visible = !smokeTest };
        var trayMenu = new Forms.ContextMenuStrip();
        AddTrayMenu(trayMenu, "Mostrar / minimizar", () => { if (IsVisible) MinimizeToTray(); else Restore(); });
        AddTrayMenu(trayMenu, "Restaurar", Restore);
        AddTrayMenu(trayMenu, "Reconectar dispositivo", () => _ = ReconnectAsync());
        AddTrayMenu(trayMenu, "Selecionar porta serial...", () => { Restore(); OpenPortPicker(); });
        AddTrayMenu(trayMenu, "Sair", () => _ = ExitAsync());
        tray.ContextMenuStrip = trayMenu;
        tray.DoubleClick += (_, _) => Dispatcher.BeginInvoke(Restore);
        Closing += OnClosing;
        StateChanged += (_, _) =>
        {
            if (WindowState == WindowState.Minimized) MinimizeToTray();
            maximizeButton.Content = WindowState == WindowState.Maximized ? "\uE923" : "\uE922";
        };
        refresh.Tick += (_, _) => Refresh();
        noticeTimer.Tick += (_, _) => { noticeTimer.Stop(); noticeActive = false; UpdateModeStatus(); };
        Loaded += async (_, _) =>
        {
            collector.Start();
            refresh.Start();
            if (smokeTest) await RunSmokeTestAsync();
            else await ReconnectAsync(false);
        };
        ApplyMode();
    }

    private static string Asset(string name) => System.IO.Path.Combine(AppContext.BaseDirectory, "assets", name);

    private static IntPtr HandleWindowMessage(IntPtr hwnd, int message, IntPtr wParam, IntPtr lParam, ref bool handled)
    {
        if (message != WmGetMinMaxInfo) return IntPtr.Zero;

        IntPtr monitor = MonitorFromWindow(hwnd, MonitorDefaultToNearest);
        if (monitor == IntPtr.Zero) return IntPtr.Zero;
        var monitorInfo = new NativeMonitorInfo { Size = Marshal.SizeOf<NativeMonitorInfo>() };
        if (!GetMonitorInfo(monitor, ref monitorInfo)) return IntPtr.Zero;

        var minMaxInfo = Marshal.PtrToStructure<NativeMinMaxInfo>(lParam);
        minMaxInfo.MaxPosition = new NativePoint
        {
            X = monitorInfo.WorkArea.Left - monitorInfo.MonitorArea.Left,
            Y = monitorInfo.WorkArea.Top - monitorInfo.MonitorArea.Top
        };
        minMaxInfo.MaxSize = new NativePoint
        {
            X = monitorInfo.WorkArea.Right - monitorInfo.WorkArea.Left,
            Y = monitorInfo.WorkArea.Bottom - monitorInfo.WorkArea.Top
        };
        Marshal.StructureToPtr(minMaxInfo, lParam, false);
        handled = true;
        return IntPtr.Zero;
    }

    [DllImport("user32.dll")]
    private static extern IntPtr MonitorFromWindow(IntPtr hwnd, uint flags);

    [DllImport("user32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool GetMonitorInfo(IntPtr monitor, ref NativeMonitorInfo monitorInfo);

    [StructLayout(LayoutKind.Sequential)]
    private struct NativePoint
    {
        public int X;
        public int Y;
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct NativeMinMaxInfo
    {
        public NativePoint Reserved;
        public NativePoint MaxSize;
        public NativePoint MaxPosition;
        public NativePoint MinTrackSize;
        public NativePoint MaxTrackSize;
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct NativeRect
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    private struct NativeMonitorInfo
    {
        public int Size;
        public NativeRect MonitorArea;
        public NativeRect WorkArea;
        public uint Flags;
    }

    private Grid BuildTitleBar(out Button maximizeButton)
    {
        var titleBar = new Grid { Height = 44, Background = Background };
        titleBar.ColumnDefinitions.Add(new ColumnDefinition());
        titleBar.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
        var identity = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            VerticalAlignment = VerticalAlignment.Center,
            Margin = new Thickness(16, 0, 0, 0)
        };
        identity.Children.Add(new Rectangle
        {
            Width = 20,
            Height = 20,
            Fill = Brushes.White,
            OpacityMask = new ImageBrush(LoadImage("streamDeco.ico")),
            Margin = new Thickness(0, 0, 10, 0)
        });
        identity.Children.Add(Theme.Label(Title, 15, true));
        titleBar.Children.Add(identity);

        var actions = new StackPanel { Orientation = Orientation.Horizontal };
        var minimizeButton = WindowButton("\uE921", "Minimizar", "#0c5ba0");
        maximizeButton = WindowButton("\uE922", "Maximizar", "#0c5ba0");
        var closeButton = WindowButton("\uE8BB", "Fechar", "#e81123");
        minimizeButton.Click += (_, _) => WindowState = WindowState.Minimized;
        maximizeButton.Click += (_, _) =>
            WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
        closeButton.Click += (_, _) => Close();
        actions.Children.Add(minimizeButton);
        actions.Children.Add(maximizeButton);
        actions.Children.Add(closeButton);
        Grid.SetColumn(actions, 1);
        titleBar.Children.Add(actions);
        return titleBar;
    }

    private static Button WindowButton(string glyph, string tooltip, string hoverColor)
    {
        var button = new Button
        {
            Content = glyph,
            FontFamily = new FontFamily("Segoe Fluent Icons"),
            FontSize = 12,
            Foreground = Theme.Brush("#25313c"),
            Background = Brushes.Transparent,
            Width = 46,
            Height = 44,
            Padding = new Thickness(0),
            ToolTip = tooltip
        };
        button.MouseEnter += (_, _) =>
        {
            button.Background = Theme.Brush(hoverColor);
            button.Foreground = Brushes.White;
        };
        button.MouseLeave += (_, _) =>
        {
            button.Background = Brushes.Transparent;
            button.Foreground = Theme.Brush("#25313c");
        };
        WindowChrome.SetIsHitTestVisibleInChrome(button, true);
        AutomationProperties.SetName(button, tooltip);
        return button;
    }

    private static BitmapImage LoadImage(string name)
    {
        var image = new BitmapImage();
        image.BeginInit();
        image.CacheOption = BitmapCacheOption.OnLoad;
        image.UriSource = new Uri(Asset(name));
        image.EndInit();
        image.Freeze();
        return image;
    }

    private static TextBlock DiagnosticText() => new()
    { FontFamily = new FontFamily("Consolas"), FontSize = 13, Foreground = Theme.Text, TextWrapping = TextWrapping.Wrap };

    private static FrameworkElement BuildSection(string title, string image, FrameworkElement body)
    {
        var section = new Grid { Margin = new Thickness(20, 18, 20, 18) };
        section.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
        section.RowDefinitions.Add(new RowDefinition());
        var header = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(0, 0, 0, 12) };
        header.Children.Add(new Rectangle { Width = 22, Height = 22, Fill = Brushes.White,
            OpacityMask = new ImageBrush(LoadImage(image)), Margin = new Thickness(0, 0, 10, 0) });
        header.Children.Add(Theme.Label(title, 30, true));
        section.Children.Add(header);
        Grid.SetRow(body, 1);
        body.VerticalAlignment = VerticalAlignment.Top;
        if (body is ScrollViewer) body.VerticalAlignment = VerticalAlignment.Stretch;
        section.Children.Add(body);
        return section;
    }

    private static FrameworkElement BuildProcessor(string title, string image, CircularGauge gauge, StatBar temp, StatBar freq)
    {
        var body = new Grid();
        body.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(126) });
        body.ColumnDefinitions.Add(new ColumnDefinition());
        gauge.HorizontalAlignment = HorizontalAlignment.Left;
        body.Children.Add(gauge);
        var stats = new StackPanel();
        stats.Children.Add(temp);
        stats.Children.Add(freq);
        Grid.SetColumn(stats, 1);
        body.Children.Add(stats);
        return BuildSection(title, image, body);
    }

    private static void AddDiagnostic(Panel parent, string title, TextBlock text)
    {
        var heading = Theme.Label(title, 16, true);
        heading.Margin = new Thickness(0, 8, 0, 6);
        parent.Children.Add(heading);
        parent.Children.Add(text);
    }

    private static Button SideButton(string glyph, string tooltip, System.Windows.RoutedEventHandler handler)
    {
        var button = new Button { Content = glyph, FontFamily = new FontFamily("Segoe Fluent Icons"), FontSize = 38,
            Width = 150, Height = 100, Margin = new Thickness(0, 6, 0, 8), ToolTip = tooltip };
        button.Click += handler;
        AutomationProperties.SetName(button, tooltip);
        return button;
    }

    private void AddTrayMenu(Forms.ContextMenuStrip menu, string title, Action action) =>
        menu.Items.Add(title, null, (_, _) => Dispatcher.BeginInvoke(action));

    private void CycleMode()
    {
        mode = (mode + 1) % Modes.Length;
        ApplyMode();
    }

    private void TogglePause()
    {
        paused = !paused;
        pauseButton.Content = paused ? "\uE768" : "\uE769";
        UpdateModeStatus();
    }

    private void ApplyMode()
    {
        foreach (FrameworkElement section in main.Children)
        {
            section.Visibility = Visibility.Collapsed;
            Grid.SetRowSpan(section, 1);
            Grid.SetColumnSpan(section, 1);
        }
        switch (mode)
        {
            case 0:
                Place(cpuSection, 0, 0);
                Place(memorySection, 0, 1);
                Place(gpuSection, 1, 0);
                Place(clockSection, 1, 1);
                break;
            case 1:
                Place(cpuSection, 0, 0);
                Place(memorySection, 0, 1);
                Place(gpuSection, 1, 0, 2);
                break;
            case 2: Place(clockSection, 0, 0, 2, 2); break;
            case 3: Place(diagnosticSection, 0, 0, 2, 2); break;
        }
        clockBody.HorizontalAlignment = mode == 2 ? HorizontalAlignment.Center : HorizontalAlignment.Left;
        clockBody.VerticalAlignment = mode == 2 ? VerticalAlignment.Center : VerticalAlignment.Top;
        clock.TextAlignment = date.TextAlignment = mode == 2 ? TextAlignment.Center : TextAlignment.Left;
        UpdateModeStatus();
    }

    private static void Place(FrameworkElement section, int row, int column, int columns = 1, int rows = 1)
    {
        Grid.SetRow(section, row);
        Grid.SetColumn(section, column);
        Grid.SetColumnSpan(section, columns);
        Grid.SetRowSpan(section, rows);
        section.Visibility = Visibility.Visible;
    }

    private void UpdateModeStatus()
    {
        if (!noticeActive)
        {
            status.Text = paused ? "MODE: PAUSED" : $"MODE: {Modes[mode]}";
            status.Foreground = Theme.Brush("#25313c");
        }
    }

    private void Notice(string message, bool error = false)
    {
        noticeActive = true;
        status.Text = message;
        status.Foreground = Theme.Brush(error ? "#8a1f2f" : "#25313c");
        noticeTimer.Stop();
        noticeTimer.Start();
    }

    private void Refresh()
    {
        if (exiting) return;
        if (!paused && collector.Latest is Metrics metrics)
        {
            displayed = metrics;
            cpuGauge.SetValue(metrics.CpuLoad);
            gpuGauge.SetValue(metrics.GpuLoad);
            cpuTemp.SetValue(metrics.CpuTemp, 120, $"{(int)metrics.CpuTemp} C");
            cpuFreq.SetValue(metrics.CpuFreq, 6000, $"{(int)metrics.CpuFreq} MHz");
            gpuTemp.SetValue(metrics.GpuTemp, 120, $"{(int)metrics.GpuTemp} C");
            gpuFreq.SetValue(metrics.GpuFreq, 4000, $"{(int)metrics.GpuFreq} MHz");
            ram.SetValue(metrics.RamUsed, metrics.RamTotal, $"RAM: {(int)metrics.RamUsed}/{(int)metrics.RamTotal} MB");
            disk.SetValue(metrics.DiskUsed, metrics.DiskTotal, $"C: {(int)metrics.DiskUsed}/{(int)metrics.DiskTotal} GB");
            diagnosticMetrics.Text = metrics.ToPlainText();
        }
        clock.Text = DateTime.Now.ToString("HH:mm:ss");
        date.Text = DateTime.Now.ToString("dd/MM/yyyy");
        var state = serial.Diagnostics;
        string serialState = state.Connected ? "connected" : state.Running ? "waiting" : "stopped";
        diagnosticSerial.Text = $"status: {serialState}\nevent: {state.LastEvent} @ {state.LastEventTime}\n" +
            $"port: {state.Port}\nattempts: {state.ConnectAttempts}  failures: {state.SendFailures}\n" +
            $"last error: {state.LastError}\nsensors: {collector.Status}";
        sideDiagnostic.Text = $"SERIAL\n{serialState}\nport: {state.Port}\nattempts: {state.ConnectAttempts}\n" +
            $"failures: {state.SendFailures}\n{state.LastError}";
        diagnosticPayload.Text = $"last sent @ {state.LastPayloadTime}\n" +
            (state.LastPayload.Length == 0 ? "(no payload yet)" : state.LastPayload);
        connectionIcon.Fill = Theme.Brush(state.Connected ? "#4ad673" : "#e15454");
        serialButton.ToolTip = $"{state.Port}: {serialState} - Reconectar dispositivo";
    }

    private async Task ReconnectAsync(bool notify = true)
    {
        if (discovering || exiting || smokeTest) return;
        discovering = true;
        bool openPortPicker = false;
        if (notify) Notice("SERIAL: reconectando...");
        try
        {
            var ports = await Task.Run(SerialService.ListPorts);
            if (exiting) return;
            var compatible = ports.FirstOrDefault(port => port.Compatible);
            if (compatible is null)
            {
                openPortPicker = true;
                if (notify) System.Media.SystemSounds.Exclamation.Play();
                Notice("SERIAL: dispositivo compativel nao localizado", true);
            }
            else
            {
                portPicker?.Close();
                serial.Connect(compatible.Port);
                Notice($"SERIAL: conectando {compatible.Port}");
            }
        }
        catch (Exception exception)
        {
            openPortPicker = true;
            Notice("SERIAL: falha na busca", true);
            Log.Write(exception.ToString());
        }
        finally { discovering = false; }
        if (exiting || !openPortPicker) return;
        Restore();
        OpenPortPicker();
    }

    private void OpenPortPicker()
    {
        if (exiting || smokeTest) return;
        if (portPicker is not null) { portPicker.Activate(); return; }
        var picker = new Window { Title = "Porta serial", Owner = this, Width = 580, Height = 260,
            ResizeMode = ResizeMode.NoResize, WindowStartupLocation = WindowStartupLocation.CenterOwner,
            Background = Theme.Panel, Foreground = Theme.Text };
        portPicker = picker;
        var body = new StackPanel { Margin = new Thickness(20) };
        using var identity = WindowsIdentity.GetCurrent();
        bool admin = new WindowsPrincipal(identity).IsInRole(WindowsBuiltInRole.Administrator);
        body.Children.Add(Theme.Label(admin ? "Administrador: sim" : "Administrador: nao", 14));
        var selection = new ComboBox { Margin = new Thickness(0, 16, 0, 16), Height = 32 };
        body.Children.Add(selection);
        var message = Theme.Label("Buscando portas...", 14);
        message.TextWrapping = TextWrapping.Wrap;
        body.Children.Add(message);
        var actions = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(0, 16, 0, 0) };
        var reload = new Button { Content = "Atualizar", Margin = new Thickness(0, 0, 12, 0) };
        var connect = new Button { Content = "Conectar", IsEnabled = false };
        actions.Children.Add(reload);
        actions.Children.Add(connect);
        body.Children.Add(actions);
        picker.Content = body;
        picker.Closed += (_, _) => portPicker = null;
        selection.SelectionChanged += (_, _) => connect.IsEnabled = selection.SelectedItem is SerialDevice;
        reload.Click += async (_, _) => await LoadPortsAsync();
        picker.Loaded += async (_, _) => await LoadPortsAsync();
        connect.Click += (_, _) =>
        {
            if (selection.SelectedItem is not SerialDevice device) return;
            serial.Connect(device.Port);
            Notice($"SERIAL: conectando {device.Port}");
            picker.Close();
        };
        picker.Show();

        async Task LoadPortsAsync()
        {
            reload.IsEnabled = connect.IsEnabled = false;
            try
            {
                var ports = await Task.Run(SerialService.ListPorts);
                if (portPicker != picker) return;
                selection.ItemsSource = ports;
                selection.SelectedItem = ports.FirstOrDefault(port => port.Port == serial.Diagnostics.Port)
                    ?? ports.FirstOrDefault(port => port.Compatible) ?? ports.FirstOrDefault();
                var compatible = ports.FirstOrDefault(port => port.Compatible);
                message.Text = compatible is null
                    ? $"Dispositivo compativel nao localizado. {(ports.Count == 0 ? "Nenhuma porta serial encontrada." : "Selecione uma porta manualmente.")}"
                    : $"Dispositivo compativel localizado: {compatible}.";
            }
            catch (Exception exception) { message.Text = exception.Message; }
            finally { reload.IsEnabled = true; connect.IsEnabled = selection.SelectedItem is SerialDevice; }
        }
    }

    private void OnClosing(object? sender, CancelEventArgs args)
    {
        if (exiting) return;
        args.Cancel = true;
        MinimizeToTray();
    }

    private void MinimizeToTray()
    {
        portPicker?.Close();
        Hide();
    }

    private void Restore()
    {
        if (exiting) return;
        portPicker?.Close();
        Show();
        WindowState = WindowState.Normal;
        Activate();
    }

    private async Task ExitAsync()
    {
        if (exiting) return;
        exiting = true;
        portPicker?.Close();
        refresh.Stop();
        noticeTimer.Stop();
        tray.Visible = false;
        tray.ContextMenuStrip?.Dispose();
        tray.Dispose();
        trayIcon.Dispose();
        try { await collector.DisposeAsync(); }
        catch (Exception exception) { Log.Write(exception.ToString()); }
        try { await serial.DisposeAsync(); }
        catch (Exception exception) { Log.Write(exception.ToString()); }
        Close();
        Application.Current.Shutdown();
    }

    private async Task RunSmokeTestAsync()
    {
        string output = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "StreamDecoMonitor", "smoke-test");
        Directory.CreateDirectory(output);
        try
        {
            var deadline = Stopwatch.StartNew();
            while (collector.Latest is null && deadline.Elapsed < TimeSpan.FromSeconds(20)) await Task.Delay(100);
            if (collector.Latest is null) throw new InvalidOperationException($"No metrics collected: {collector.Status}");
            Refresh();
            var frozen = displayed;
            TogglePause();
            await Task.Delay(2200);
            Refresh();
            if (!ReferenceEquals(displayed, frozen) || collector.Latest!.Timestamp <= frozen!.Timestamp)
                throw new InvalidOperationException("Pause must freeze widgets without stopping collection.");
            TogglePause();
            Refresh();
            foreach (var dimensions in new[] { (960d, 640d), (1280d, 800d) })
            {
                Width = dimensions.Item1;
                Height = dimensions.Item2;
                for (mode = 0; mode < Modes.Length; mode++)
                {
                    ApplyMode();
                    UpdateLayout();
                    await Dispatcher.InvokeAsync(() => { }, DispatcherPriority.ContextIdle);
                    var visual = (FrameworkElement)Content;
                    var bitmap = new RenderTargetBitmap((int)visual.ActualWidth, (int)visual.ActualHeight, 96, 96, PixelFormats.Pbgra32);
                    var drawing = new DrawingVisual();
                    using (var context = drawing.RenderOpen())
                    {
                        context.DrawRectangle(Background, null, new Rect(0, 0, visual.ActualWidth, visual.ActualHeight));
                        context.DrawRectangle(new VisualBrush(visual), null, new Rect(0, 0, visual.ActualWidth, visual.ActualHeight));
                    }
                    bitmap.Render(drawing);
                    var encoder = new PngBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(bitmap));
                    using var stream = File.Create(System.IO.Path.Combine(output, $"{Modes[mode]}-{(int)Width}.png"));
                    encoder.Save(stream);
                    int expected = mode == 0 ? 4 : mode == 1 ? 3 : 1;
                    if (main.Children.Cast<FrameworkElement>().Count(section => section.Visibility == Visibility.Visible) != expected)
                        throw new InvalidOperationException("Unexpected view widgets.");
                }
            }
            mode = 0;
            ApplyMode();
            MinimizeToTray();
            if (IsVisible) throw new InvalidOperationException("Window did not hide.");
            Restore();
            if (!IsVisible) throw new InvalidOperationException("Window did not restore.");
            if (serial.Diagnostics.ConnectAttempts != 0) throw new InvalidOperationException("Smoke test must not open serial ports.");
            File.WriteAllText(System.IO.Path.Combine(output, "result.txt"),
                $"PASS: live metrics, pause/resume, four modes at two sizes, hide/restore, no serial writes.\n{collector.Latest!.ToPlainText()}\n{collector.Status}");
        }
        catch (Exception exception)
        {
            File.WriteAllText(System.IO.Path.Combine(output, "result.txt"), $"FAIL: {exception}");
            Environment.ExitCode = 1;
        }
        finally { await ExitAsync(); }
    }
}