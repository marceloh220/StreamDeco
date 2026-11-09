using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace StreamDecoMonitor;

internal static class Theme
{
    public static SolidColorBrush Brush(string color) =>
        new((Color)ColorConverter.ConvertFromString(color));
    public static readonly SolidColorBrush Panel = Brush("#2a414f");
    public static readonly SolidColorBrush Text = Brush("#e7effa");
    public static readonly SolidColorBrush Track = Brush("#496171");
    public static readonly SolidColorBrush Fill = Brush("#f0f3fb");

    public static TextBlock Label(string text, double size = 17, bool bold = false) => new()
    {
        Text = text, FontSize = size, Foreground = Text,
        FontWeight = bold ? FontWeights.Bold : FontWeights.Normal,
        FontFamily = new FontFamily("Segoe UI"), TextWrapping = TextWrapping.Wrap
    };
}

internal sealed class CircularGauge : FrameworkElement
{
    public double Value { get; private set; }
    public CircularGauge()
    {
        Width = 110;
        Height = 110;
    }

    public void SetValue(double value)
    {
        Value = double.IsFinite(value) ? Math.Clamp(value, 0, 100) : 0;
        InvalidateVisual();
    }

    protected override void OnRender(DrawingContext drawing)
    {
        base.OnRender(drawing);
        var center = new Point(55, 55);
        drawing.DrawEllipse(null, new Pen(Theme.Track, 12), center, 47, 47);
        if (Value > 0)
        {
            double angle = Math.Min(Value / 100d, 0.99972) * 2 * Math.PI;
            var end = new Point(55 + 47 * Math.Sin(angle), 55 - 47 * Math.Cos(angle));
            var geometry = new StreamGeometry();
            using (var context = geometry.Open())
            {
                context.BeginFigure(new Point(55, 8), false, false);
                context.ArcTo(end, new Size(47, 47), 0, angle > Math.PI, SweepDirection.Clockwise, true, false);
            }
            drawing.DrawGeometry(null, new Pen(Theme.Fill, 12), geometry);
        }
        var text = new FormattedText($"{(int)Value}%", CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
            new Typeface(new FontFamily("Segoe UI"), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal),
            18, Theme.Text, VisualTreeHelper.GetDpi(this).PixelsPerDip);
        drawing.DrawText(text, new Point(55 - text.Width / 2, 55 - text.Height / 2));
    }
}

internal sealed class StatBar : StackPanel
{
    private readonly TextBlock label;
    private readonly Border fill;
    private readonly Grid rail;
    private double fraction;
    public string DisplayText => label.Text;

    public StatBar(string initial)
    {
        Margin = new Thickness(0, 0, 0, 12);
        label = Theme.Label(initial, 17, true);
        label.Margin = new Thickness(0, 0, 0, 6);
        Children.Add(label);
        rail = new Grid { Height = 15, ClipToBounds = true };
        rail.Children.Add(new Border { Background = Theme.Track, CornerRadius = new CornerRadius(7) });
        fill = new Border { Background = Theme.Fill, CornerRadius = new CornerRadius(7),
            HorizontalAlignment = HorizontalAlignment.Left, Width = 0 };
        rail.Children.Add(fill);
        rail.SizeChanged += (_, _) => fill.Width = rail.ActualWidth * fraction;
        Children.Add(rail);
    }

    public void SetValue(double value, double maximum, string text)
    {
        fraction = Math.Clamp(value / Math.Max(1, maximum), 0, 1);
        label.Text = text;
        fill.Width = rail.ActualWidth * fraction;
    }
}