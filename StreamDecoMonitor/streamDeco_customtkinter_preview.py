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

from modules.report import *
from modules.monitor_preview import MonitorPreview
from modules.system_tray import SystemTrayController
from modules.serial_sender_task import SerialSenderTask
from modules.single_instance_mutex import SingleInstanceMutex
from modules.system_metrics_provider import SystemMetricsProvider
from modules.sound_notification import play_reconnect_failure_sound


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

    def reconnect_device() -> None:
        """Callback to manually reconnect the serial device from the system tray"""
        report("StreamDecoCustomTkinterPreview", "INFO", 
               "Manual reconnect requested from system tray.")
        app.show_status_notice("SERIAL: reconectando...", 
                               severity="info", duration_ms=1500)

        if serial_sender_state["task"] is not None:
            # Stop the existing serial sender task before attempting to reconnect
            serial_sender_state["task"].stop()
            serial_sender_state["task"] = None

        new_port = search_com_port() # Search for a compatible COM port again in case it was reconnected
        if not new_port:
            # If no compatible COM port is found, show an error notice and play a sound notification
            report("StreamDecoCustomTkinterPreview", "ERROR", 
                   "Reconnect failed: no compatible COM port found.")
            play_reconnect_failure_sound()
            app.show_status_notice("SERIAL: falha ao reconectar", severity="error")
            return

        # New SerialSenderTask to reconnect
        new_sender = SerialSenderTask(
            boardCOM=new_port,
            queue_serial_sender=queue_serial_sender,
            run_task=True,
            update_interval_seconds=1.0,
        )

        # Start the new serial sender task and update the state
        new_sender.start()
        serial_sender_state["task"] = new_sender
        report("StreamDecoCustomTkinterPreview", "INFO", 
               f"Reconnect successful on {new_port}.")
        app.show_status_notice(f"SERIAL: conectado em {new_port}", severity="success")

    # App gui
    app = MonitorPreview(queue_metrics, stop_tasks)
    
    # System tray controller to manage minimizing to tray and exiting from the tray menu
    tray = SystemTrayController(app, stop_tasks, reconnect_device)
    
    def btn_top_action():
        """Callback for the top button in the GUI to cycle view modes"""
        global app
        app.cycle_view_mode()
        report("StreamDecoCustomTkinterPreview", "INFO", 
               "Top button clicked! View mode cycled.")

    def btn_mid_action():
        """Callback for the middle button in the GUI to toggle metrics pause/play"""
        global app
        app.toggle_metrics_pause()
        report("StreamDecoCustomTkinterPreview", "INFO", 
               "Mid button clicked! Metrics pause toggled.")

    def btn_bot_action():
        """Callback for the bottom button in the GUI to activate home mode
        (placeholder for future features)"""
        global app
        app.go_home_mode()
        report("StreamDecoCustomTkinterPreview", "INFO", 
               "Bot button clicked! Home mode activated.")
    
    app.btn_top_command(btn_top_action) # Connect the top button to its callback
    app.btn_mid_command(btn_mid_action) # Connect the middle button to its callback
    app.btn_bot_command(btn_bot_action) # Connect the bottom button to its callback

    # Ensure the window close button also triggers the stop_tasks callback to cleanly exit
    app.protocol("WM_DELETE_WINDOW", tray.minimize_to_tray)

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
