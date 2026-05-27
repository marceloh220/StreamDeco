import sys
from typing import Callable, Any
from pathlib import Path
from .monitor_preview import MonitorPreview
from .report import report

class SystemTrayController:
    """
    Manages the system tray icon and its interactions for the StreamDeco Monitor application.
    This class uses the pystray library to create a system tray icon with a context menu
    that allows the user to restore the application window or exit the application. It also handles
    minimizing the application to the system tray when the user minimizes the window.
    Attributes:
        _app (MonitorPreview): The main application window that this system tray controller manages.
        _on_exit (Callable[[], None]): A callback function that is called when the user chooses 
                                       to exit the application from the system tray menu.
        _on_reconnect (Callable[[], None] | None): An optional callback function that is called when the user chooses 
                                       to reconnect the serial device from the system tray menu.
                                       If None, the reconnect option will not be shown in the menu.
        _icon (Any): The pystray Icon object representing the system tray icon. This
                     is initialized in the constructor and may be None if the system tray 
                     functionality is unavailable.
        _is_running (bool): A flag indicating whether the system tray icon is currently running.
        _can_use_tray (bool): A flag indicating whether the system tray functionality is available and can be used. This is determined during initialization based on whether the pystray library was successfully imported and initialized.
    """
    def __init__(self, app: MonitorPreview, on_exit: Callable[[], None], on_reconnect: Callable[[], None] | None = None) -> None:
        self._app = app          # The main application window that this system tray controller manages.
        self._on_exit = on_exit  # A callback function that is called when the user chooses to exit the application from the system tray menu.
        self._on_reconnect = on_reconnect
        self._icon: Any = None   # The pystray Icon object representing the system tray icon.
        self._is_running = False
        self._can_use_tray = False
        try:
            import pystray
            from PIL import Image
            if getattr(sys, "frozen", False):
                icon_path = Path(getattr(sys, "_MEIPASS")) / "assets" / "streamDeco.ico"
            else:
                icon_path = Path(__file__).resolve().parents[1] / "assets" / "streamDeco.ico"
            if not icon_path.exists():
                report("StreamDecoCustomTkinterPreview", "WARNING", f"Icon file not found at {icon_path}. System tray icon will be unavailable.")
                sys.exit(1)
            icon_image = Image.open(icon_path) if icon_path.exists() else None
            menu_items = [
                pystray.MenuItem("Restaurar", self._restore_window, default=True),
            ]
            if self._on_reconnect is not None:
                menu_items.append(pystray.MenuItem("Reconectar dispositivo", self._reconnect_device))
            menu_items.append(pystray.MenuItem("Sair", self._quit_application))
            menu = pystray.Menu(*menu_items)
            self._icon = pystray.Icon(name="StreamDecoPreview", title="StreamDeco Preview", icon=icon_image, menu=menu)
            self._can_use_tray = True
        except Exception as exc:
            report("StreamDecoCustomTkinterPreview", "WARNING", f"System tray unavailable: {exc}")

    def start(self) -> None:
        """
        Starts the system tray icon if it is available and not already running.
        This method will run the icon in a detached thread, 
        allowing the main application to continue running without blocking.
        """
        if not self._can_use_tray or self._icon is None or self._is_running:
            return
        self._icon.run_detached()
        self._is_running = True

    def stop(self) -> None:
        """
        Stops the system tray icon if it is currently running.
        This method will attempt to stop the icon and set the running flag to False.
        If the icon is not available or not running, it will simply return without doing anything.
        """
        if self._icon is None or not self._is_running:
            return
        try:
            self._icon.stop()
        except Exception:
            pass
        self._is_running = False

    def minimize_to_tray(self) -> None:
        """
        Minimizes the application window to the system tray. If the system tray functionality is not available,
        it will instead call the quit application method to exit the application. This method is intended to
        be called when the user minimizes the application window, allowing it to continue running in the background with an icon in the system tray.
        If the system tray is available, it will hide the main application window and show the system tray icon.
        If the system tray is not available, it will call the quit application method to exit the application.
        """
        if not self._can_use_tray:
            self._quit_application(None)
            return
        self._app.withdraw()

    def _restore_window(self, icon: Any, item: Any) -> None:
        """
        Restores the application window from the system tray. 
        This method is intended to be called when the user selects the "Restaurar" option from the system tray menu. 
        It will deiconify the main application window, bring it to the front, and set focus to it. 
        The icon and menu item parameters are not used in this method but are required by the pystray library 
        for menu item callbacks.
        """
        del icon, item
        report("StreamDecoCustomTkinterPreview", "INFO", "Window restored from system tray.")

        def _restore() -> None:
            """
            Restores the application window from the system tray.
            This method is intended to be called when the user selects the "Restaurar" option from the system tray menu.
            It will deiconify the main application window, bring it to the front, and set focus to it.
            """
            self._app.deiconify()
            self._app.lift()
            self._app.focus_force()

        self._app.after(0, _restore)

    def _quit_application(self, icon: Any, item: Any = None) -> None:
        """
        Quits the application from the system tray.
        This method is intended to be called when the user selects the "Sair" option from the system tray menu.
        It will stop the system tray icon if it is running and then call the application's exit handler.
        """
        del item
        if icon is not None:
            try:
                icon.stop()
            except Exception:
                pass

        def _quit() -> None:
            """
            Quits the application by calling the exit handler provided during initialization.
            This method is intended to be called when the user selects the "Sair" option from the system tray menu. 
            It will call the exit handler to perform any necessary cleanup and then destroy the main application window.
            """
            self._on_exit()

        self._app.after(0, _quit) # Schedule the quit function to run in the main thread after the current event loop iteration

    def _reconnect_device(self, icon: Any, item: Any) -> None:
        """
        Triggers a serial reconnection flow from the system tray menu.
        """
        del icon, item
        if self._on_reconnect is None:
            return

        def _reconnect() -> None:
            if self._on_reconnect is not None:
                self._on_reconnect()

        self._app.after(0, _reconnect)