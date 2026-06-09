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
import customtkinter as ctk
from modules.report import *
from modules.monitor_preview import MonitorPreview
from modules.system_tray import SystemTrayController
from modules.serial_sender_task import SerialSenderTask
from modules.single_instance_mutex import SingleInstanceMutex
from modules.system_metrics_provider import SystemMetricsProvider
from modules.serial_port_picker import close_serial_port_picker_if_open, open_serial_port_picker
from modules.serial_reconnect import connect_serial_device as connect_serial_device_impl, reconnect_device as reconnect_device_impl
from modules.ui_button_actions import btn_bot_action as btn_bot_action_impl, btn_mid_action as btn_mid_action_impl, btn_top_action as btn_top_action_impl


def serial_diagnostics_snapshot(sender: SerialSenderTask | None) -> dict[str, str | int | bool]:
    """
    Returns a stable diagnostics snapshot for the UI diagnostics panel.
    """
    if sender is None:
        return {
            "port": "--",
            "connected": False,
            "running": False,
            "connect_attempts": 0,
            "send_failures": 0,
            "last_event": "no serial device",
            "last_event_time": "--:--:--",
            "last_error": "none",
            "last_payload": "",
            "last_payload_time": "--:--:--",
        }
    return sender.diagnostics()


def search_com_port() -> str:
    from serial.tools.list_ports import comports
    for port in comports():
        if "CH340" in port.description:
            return port.name
        if "CP210x" in port.description:
            return port.name
    report("StreamDecoCustomTkinterPreview", "ERROR", "No compatible COM port found. Please check your connections.")
    return ""


def list_serial_ports() -> list[dict[str, str | bool]]:
    """
    Returns all available serial ports with basic metadata and compatibility hint.
    """
    from serial.tools.list_ports import comports
    ports: list[dict[str, str | bool]] = []
    for port in comports():
        description = str(port.description or "")
        ports.append(
            {
                "name": str(port.name),
                "description": description,
                "compatible": ("CH340" in description) or ("CP210x" in description),
            }
        )
        report("StreamDecoCustomTkinterPreview", "DEBUG", f"Detected serial port: {port.name} - {description}")
    return ports


def has_admin_rights() -> bool:
    """
    Returns True when the current process is running with administrator rights on Windows.
    """
    try:
        return bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        return False


if __name__ == "__main__":

    # Mutex to ensure only one instance of the app is running on Windows
    mutex = SingleInstanceMutex("StreamDecoCustomTkinterPreview")

    if not mutex.acquire():
        # Exit if another instance is already running (Windows) or if mutex acquisition failed
        raise SystemExit(0)

    #report.set_debug_level("INFO")  # Set to INFO or DEBUG for more detailed logs

    # Use queues to communicate between threads/tasks and the main GUI thread
    # queue_metrics will carry the latest system metrics data for the GUI to display
    # queue_serial_sender will carry system metrics through the serial connection
    queue_metrics: Queue[dict[str, float]] = Queue(maxsize=6)
    queue_serial_sender: Queue[str] = Queue(maxsize=6)

    # Feed queus with system metrics
    metrics = SystemMetricsProvider(queue_metrics, queue_serial_sender)

    # Search for a compatible COM port before starting the app
    # If not found, the SerialSenderTask will be skipped
    boardCOM = search_com_port()

    # Only create the SerialSenderTask if a compatible COM port was found
    # This interface send metrics data to the StreamDeco board via serial communication
    serial_sender = SerialSenderTask(
        boardCOM=boardCOM,
        queue_serial_sender=queue_serial_sender, 
        run_task=True,
        update_interval_seconds=1.0
        ) if boardCOM else None

    # State holders for the serial sender task and the main app/tray instances
    # Used by callbacks to control the lifecycle of these components (start/stop/reconnect)
    serial_sender_state = {"task": serial_sender}

    # Main gui app interface inspired by the StreamDeco monitor layout, built with CustomTkinter
    app: MonitorPreview

    # System tray controller to manage minimizing to tray
    tray: SystemTrayController

    # Flag to indicate if the app is exiting
    # Prevent multiple stop calls from different sources (window close, tray exit, etc.)
    is_exiting = {"value": False}
    app_is_admin = has_admin_rights()

    def stop_tasks() -> None:
        """Callback to stop all running tasks and exit the application"""
        if is_exiting["value"]:
            # Prevent multiple calls to stop_tasks if the app is already in the process of exiting
            return
        is_exiting["value"] = True

        # Stop the system tray and metrics collector tasks
        tray.stop()
        metrics.stop()

        if serial_sender_state["task"] is not None:
            # Stop the serial sender task if it is running
            serial_sender_state["task"].stop()
        mutex.release() # Release the mutex when exiting
        app.destroy()   # Destroy the main window to exit the app

    def connect_serial_device(new_port: str, source: str) -> bool:
        return connect_serial_device_impl(
            app=app,
            serial_sender_state=serial_sender_state,
            queue_serial_sender=queue_serial_sender,
            new_port=new_port,
            source=source,
        )

    def reconnect_device() -> None:
        reconnect_device_impl(
            app=app,
            search_com_port=search_com_port,
            connect_serial_device_fn=connect_serial_device,
        )

    port_picker_state: dict[str, ctk.CTkToplevel | None] = {"window": None}

    def open_serial_port_picker_modal() -> None:
        open_serial_port_picker(
            app=app,
            app_is_admin=app_is_admin,
            port_picker_state=port_picker_state,
            serial_sender_state=serial_sender_state,
            list_serial_ports=list_serial_ports,
            connect_serial_device=connect_serial_device,
        )

    # App gui
    app = MonitorPreview(queue_metrics, stop_tasks)
    app.set_serial_debug_mode(get_debug_level() == "DEBUG")
    app.set_diagnostics_provider(lambda: serial_diagnostics_snapshot(serial_sender_state["task"]))
    #app.serial_status_icon_command(open_serial_port_picker_modal)
    app.serial_status_icon_command(reconnect_device)

    # System tray controller to manage minimizing to tray and exiting from the tray menu
    def on_before_restore_tray() -> None:
        close_serial_port_picker_if_open(
            app=app,
            port_picker_state=port_picker_state,
            notice_context="restaurar",
        )

    def on_before_exit_tray() -> None:
        close_serial_port_picker_if_open(
            app=app,
            port_picker_state=port_picker_state,
            notice_context="sair",
        )

    tray = SystemTrayController(
        app,
        stop_tasks,
        reconnect_device,
        on_before_restore=on_before_restore_tray,
        on_before_exit=on_before_exit_tray,
    )

    def minimize_to_tray_safe() -> None:
        """
        Closes transient dialogs before minimizing to tray.
        """
        close_serial_port_picker_if_open(
            app=app,
            port_picker_state=port_picker_state,
            notice_context="minimizar",
        )
        tray.minimize_to_tray()

    app.btn_top_command(lambda: btn_top_action_impl(app)) # Connect the top button to its callback
    app.btn_mid_command(lambda: btn_mid_action_impl(app)) # Connect the middle button to its callback
    app.btn_bot_command(lambda: btn_bot_action_impl(app)) # Connect the bottom button to its callback

    # Ensure the window close button also triggers the stop_tasks callback to cleanly exit
    app.protocol("WM_DELETE_WINDOW", minimize_to_tray_safe)

    # Start metrics collector task
    metrics.start()

    # Start the serial sender task if it was created 
    # (i.e., if a compatible COM port was found)
    if serial_sender_state["task"] is not None:
        serial_sender_state["task"].start()

    # Start the system tray icon and main GUI loop
    tray.start()

    # Start the main GUI loop (blocking call)
    app.mainloop()
