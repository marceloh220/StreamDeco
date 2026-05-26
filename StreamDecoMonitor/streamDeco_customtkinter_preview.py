"""
CustomTkinter preview window inspired by the StreamDeco monitor layout.

Requirements:
    pip install customtkinter
    pip install pystray pillow

In Windows, you may also need to install the appropriate USB-to-serial drivers for your board 
(e.g., CH340 or CP210x) to enable serial communication.

For Windows users, you can create an executable using PyInstaller with the following command:
    pyinstaller streamDeco_customtkinter_preview.spec
The executable bundles the app files and LibreHardwareMonitor DLL automatically.


Run:
    python streamDeco_customtkinter_preview.py
"""

from queue import Queue
import ctypes
import sys

from modules.report import *
from modules.monitor_preview import MonitorPreview
from modules.system_tray import SystemTrayController
from modules.serial_sender_task import SerialSenderTask
from modules.system_metrics_provider import SystemMetricsProvider

_single_instance_mutex = None


def _acquire_single_instance_lock() -> bool:
    if sys.platform != "win32":
        return True

    global _single_instance_mutex
    kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
    mutex_name = "Global\\StreamDecoCustomTkinterPreview"
    _single_instance_mutex = kernel32.CreateMutexW(None, False, mutex_name)
    if not _single_instance_mutex:
        report("StreamDecoCustomTkinterPreview", "ERROR", "Failed to create single-instance lock.")
        return False

    already_running = ctypes.get_last_error() == 183
    if already_running:
        report("StreamDecoCustomTkinterPreview", "INFO", "Another instance is already running. Exiting.")
        kernel32.CloseHandle(_single_instance_mutex)
        _single_instance_mutex = None
        return False

    return True


def _release_single_instance_lock() -> None:
    if sys.platform != "win32":
        return
    global _single_instance_mutex
    if _single_instance_mutex:
        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel32.CloseHandle(_single_instance_mutex)
        _single_instance_mutex = None


def search_com_port() -> str:
    from serial.tools.list_ports import comports
    for port in comports():
        if "CH340" in port.description:
            return port.name
        if "CP210x" in port.description:
            return port.name
    report("StreamDecoCustomTkinterPreview", "ERROR", "No compatible COM port found. Please check your connections.")
    return ""


if __name__ == "__main__":

    if not _acquire_single_instance_lock():
        raise SystemExit(0)

    #report.set_debug_level("INFO")  # Set to INFO or DEBUG for more detailed logs

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
        _release_single_instance_lock()
        app.destroy()

    app = MonitorPreview(queue_metrics, stop_tasks)
    tray = SystemTrayController(app, stop_tasks)
    
    def btn_top_action():
        global app
        app.cycle_view_mode()
        report("StreamDecoCustomTkinterPreview", "INFO", "Top button clicked! View mode cycled.")

    def btn_mid_action():
        global app
        app.toggle_metrics_pause()
        report("StreamDecoCustomTkinterPreview", "INFO", "Mid button clicked! Metrics pause toggled.")

    def btn_bot_action():
        global app
        app.go_home_mode()
        report("StreamDecoCustomTkinterPreview", "INFO", "Bot button clicked! Home mode activated.")
    
    app.btn_top_command(btn_top_action)
    app.btn_mid_command(btn_mid_action)
    app.btn_bot_command(btn_bot_action)
    app.protocol("WM_DELETE_WINDOW", tray.minimize_to_tray)
    metrics.start()
    if serial_sender is not None:
        serial_sender.start()
    tray.start()
    app.mainloop()
