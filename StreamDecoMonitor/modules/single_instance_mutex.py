import sys
import ctypes
from .report import report

class SingleInstanceMutex():
    """Class to manage a single instance mutex for the application."""
    def __init__(self, app_name: str):
        self._single_instance_mutex = None
        self._app_name = app_name


    def acquire(self) -> bool:
        """
        Acquires a mutex to ensure only one instance of the application is running on Windows.
        Return:
            bool: True if the lock was acquired successfully
            False if another instance is already running.
        """
        if sys.platform != "win32":
            return True

        kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        mutex_name = f"Global\\{self._app_name}"
        self._single_instance_mutex = kernel32.CreateMutexW(None, False, mutex_name)
        if not self._single_instance_mutex:
            report(self._app_name, "ERROR", "Failed to create single-instance lock.")
            return False

        already_running = ctypes.get_last_error() == 183
        if already_running:
            report(self._app_name, "INFO", "Another instance is already running. Exiting.")
            kernel32.CloseHandle(self._single_instance_mutex)
            self._single_instance_mutex = None
            return False

        return True


    def release(self) -> None:
        """
        Releases the single-instance mutex on Windows.
        Note: Does nothing on non-Windows platforms.
            Should be called when the application is exiting to clean up resources.
        """
        if sys.platform != "win32":
            return
        if self._single_instance_mutex:
            kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
            kernel32.CloseHandle(self._single_instance_mutex)
            self._single_instance_mutex = None
 