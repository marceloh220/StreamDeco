"""
CustomTkinter preview window inspired by the StreamDeco monitor layout.

Requirements:
    pip install customtkinter
    pip install pystray pillow

Run:
    python streamDeco_customtkinter_preview.py
"""

from queue import Queue

from StreamDecoMonitor_modules.monitor_preview import MonitorPreview
from StreamDecoMonitor_modules.serial_sender_task import SerialSenderTask
from StreamDecoMonitor_modules.system_metrics_provider import SystemMetricsProvider
import StreamDecoMonitor_modules.report as report
from StreamDecoMonitor_modules.system_tray import SystemTrayController


def search_com_port() -> str:
    from serial.tools.list_ports import comports
    for port in comports():
        if "CH340" in port.description:
            return port.name
        if "CP210x" in port.description:
            return port.name
    report.report("StreamDecoCustomTkinterPreview", "ERROR", "No compatible COM port found. Please check your connections.")
    return ""


if __name__ == "__main__":

    #report.set_debug_level("DEBUG")  # Set to INFO or DEBUG for more detailed logs

    queue_metrics: Queue[dict[str, float]] = Queue(maxsize=6)
    queue_serial_sender: Queue[str] = Queue(maxsize=6)

    metrics = SystemMetricsProvider(queue_metrics, queue_serial_sender)

    boardCOM = search_com_port()
    serial_sender = SerialSenderTask(boardCOM, queue_serial_sender) if boardCOM else None
    
    app: MonitorPreview
    tray: SystemTrayController
    is_exiting = {"value": False}

    def stop_tasks() -> None:
        if is_exiting["value"]:
            return
        is_exiting["value"] = True
        tray.stop()
        metrics.stop()
        if serial_sender is not None:
            serial_sender.stop()
        app.destroy()

    app = MonitorPreview(queue_metrics, stop_tasks)
    tray = SystemTrayController(app, stop_tasks)
    
    def btn_top_action():
        global app
        app.cycle_view_mode()
        report.report("StreamDecoCustomTkinterPreview", "INFO", "Top button clicked! View mode cycled.")

    def btn_mid_action():
        global app
        app.toggle_metrics_pause()
        report.report("StreamDecoCustomTkinterPreview", "INFO", "Mid button clicked! Metrics pause toggled.")

    def btn_bot_action():
        global app
        app.go_home_mode()
        report.report("StreamDecoCustomTkinterPreview", "INFO", "Bot button clicked! Home mode activated.")
    
    app.btn_top_command(btn_top_action)
    app.btn_mid_command(btn_mid_action)
    app.btn_bot_command(btn_bot_action)
    app.protocol("WM_DELETE_WINDOW", tray.minimize_to_tray)
    metrics.start()
    if serial_sender is not None:
        serial_sender.start()
    tray.start()
    app.mainloop()
