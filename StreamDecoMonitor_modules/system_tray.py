import sys
from typing import Callable, Any
from pathlib import Path
from .monitor_preview import MonitorPreview
from .report import report

class SystemTrayController:
    def __init__(self, app: MonitorPreview, on_exit: Callable[[], None]) -> None:
        self._app = app
        self._on_exit = on_exit
        self._icon: Any = None
        self._is_running = False
        self._can_use_tray = False
        try:
            import pystray
            from PIL import Image
            icon_path = Path(__file__).resolve().parent/"StreamDeco.ico"
            if not icon_path.exists():
                report("StreamDecoCustomTkinterPreview", "WARNING", f"Icon file not found at {icon_path}. System tray icon will be unavailable.")
                sys.exit(1)
            icon_image = Image.open(icon_path) if icon_path.exists() else None
            menu = pystray.Menu(
                pystray.MenuItem("Restaurar", self._restore_window, default=True),
                pystray.MenuItem("Sair", self._quit_application),
            )
            self._icon = pystray.Icon(name="StreamDecoPreview", title="StreamDeco Preview", icon=icon_image, menu=menu)
            self._can_use_tray = True
        except Exception as exc:
            report("StreamDecoCustomTkinterPreview", "WARNING", f"System tray unavailable: {exc}")

    def start(self) -> None:
        if not self._can_use_tray or self._icon is None or self._is_running:
            return
        self._icon.run_detached()
        self._is_running = True

    def stop(self) -> None:
        if self._icon is None or not self._is_running:
            return
        try:
            self._icon.stop()
        except Exception:
            pass
        self._is_running = False

    def minimize_to_tray(self) -> None:
        if not self._can_use_tray:
            self._quit_application(None)
            return
        self._app.withdraw()

    def _restore_window(self, icon: Any, item: Any) -> None:
        del icon, item
        report("StreamDecoCustomTkinterPreview", "INFO", "Window restored from system tray.")

        def _restore() -> None:
            self._app.deiconify()
            self._app.lift()
            self._app.focus_force()

        self._app.after(0, _restore)

    def _quit_application(self, icon: Any, item: Any = None) -> None:
        del item
        if icon is not None:
            try:
                icon.stop()
            except Exception:
                pass

        def _quit() -> None:
            self._on_exit()

        self._app.after(0, _quit)