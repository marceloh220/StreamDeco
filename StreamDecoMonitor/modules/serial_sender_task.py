from __future__ import annotations

import threading
import time
from datetime import datetime
from queue import Empty, Full, Queue
from serial import Serial

from .report import report


class SerialSenderTask:
    """
    Handles sending data to a serial device in a separate thread.
    """

    def __init__(self, boardCOM: str, queue_serial_sender: Queue[str] | None = None, 
                 run_task: bool = False, update_interval_seconds: float = 1.0) -> None:
        """
        Initializes the SerialSenderTask with the specified COM port and queue for sending data.
        Args:
            boardCOM (str): The COM port of the serial device.
            queue_serial_sender (Queue[str]): The queue for sending data to the serial device.
            run_detach (bool): Whether to run the thread as a detached thread.
        This constructor sets up the StreamMonitor for the specified COM port and initializes
        the threading components for running the serial sending task in the background.
        """
        self._port: str = boardCOM
        self._stop_event: threading.Event | None = None
        self._thread: threading.Thread | None = None
        self._connection: Serial | None = None
        self._update_interval_seconds = update_interval_seconds
        self._connect_retry_seconds = max(0.5, update_interval_seconds)
        self._next_connect_attempt_ts = 0.0
        self._status_lock = threading.Lock()
        self._connect_attempts = 0
        self._send_failures = 0
        self._last_error = "none"
        self._last_event = "idle"
        self._last_event_time = self._timestamp_now()
        self._last_payload = ""
        self._last_payload_time = "--:--:--"
        if run_task and queue_serial_sender is not None:
            self._stop_event = threading.Event()
            self._thread = threading.Thread(target=self._run, daemon=True)
            self._queue_serial_sender = queue_serial_sender

    @staticmethod
    def _timestamp_now() -> str:
        """
        Returns a compact local timestamp for logs and diagnostics.
        """
        return datetime.now().strftime("%H:%M:%S")

    def _record_event(self, event: str, error: str | None = None) -> None:
        """
        Updates the diagnostic state exposed to the UI.
        """
        with self._status_lock:
            self._last_event = event
            self._last_event_time = self._timestamp_now()
            if error is not None:
                self._last_error = error

    def diagnostics(self) -> dict[str, str | int | bool]:
        """
        Returns a snapshot of the current serial task state.
        """
        with self._status_lock:
            connected = self._connection is not None and getattr(self._connection, "is_open", False)
            return {
                "port": self._port or "--",
                "connected": connected,
                "running": self.is_running(),
                "connect_attempts": self._connect_attempts,
                "send_failures": self._send_failures,
                "last_event": self._last_event,
                "last_event_time": self._last_event_time,
                "last_error": self._last_error,
                "last_payload": self._last_payload,
                "last_payload_time": self._last_payload_time,
            }

    def is_running(self) -> bool:
        """
        Returns whether the sender thread is running.
        """
        return self._thread is not None and self._thread.is_alive()
    
    def set_port(self, port: str) -> None:
        """
        Sets the COM port for the serial device.
        Args:
            port (str): The COM port to set for the serial device.
        """
        self._port = port
        self._next_connect_attempt_ts = 0.0
        self._close_connection()
        self._record_event(f"port changed to {port}")
    
    def set_queue(self, queue_serial_sender: Queue[str]) -> None:
        """
        Sets the queue for sending data to the serial device.
        Args:
            queue_serial_sender (Queue[str]): The queue to set for sending data to the serial device.
        """
        self._queue_serial_sender = queue_serial_sender
    
    def task(self, run_task: bool) -> None:
        """
        Enables or disables the SerialSenderTask based on the run_task parameter.
        Args:
            run_task (bool): If True, the task will be enabled and ready to start. If False, the task will be disabled.
        """
        if run_task:
            if self._stop_event is None:
                self._stop_event = threading.Event()
            if self._thread is None:
                self._thread = threading.Thread(target=self._run, daemon=True)
        else:
            self._stop_event = None
            self._thread = None

    def start(self) -> None:
        """
        Starts the SerialSenderTask in a separate thread.
        If the task is already running, it will log a warning and return.
        """
        if self._thread is None:
            report("SerialSenderTask", "WARNING", "Thread is not initialized. Cannot start the task.")
            return
        if self._thread.is_alive():
            report("SerialSenderTask", "WARNING", "SerialSenderTask is already running.")
            return
        if self._stop_event is None:
            self._stop_event = threading.Event()
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        """
        Stops the SerialSenderTask and waits for the thread to finish.
        If the task is not running, it will log a warning and return.
        """
        if self._stop_event is None:
            report("SerialSenderTask", "WARNING", "Stop event is not initialized. Cannot stop the task.")
            return
        self._stop_event.set()
        self._record_event("stopping")
        try:
            self._queue_serial_sender.put_nowait("")
        except Full:
            report("SerialSenderTask", "WARNING", "Failed to send stop signal, queue is full.")
            pass
        if self._thread is not None and self._thread.is_alive():
            self._thread.join(timeout=1.5)
        self._close_connection()

    def _close_connection(self) -> None:
        """
        Closes the serial connection if it is open.
        """
        if self._connection is None:
            return
        try:
            self._connection.close()
        except Exception:
            pass
        self._connection = None
        self._record_event("disconnected")

    def _ensure_connection(self) -> bool:
        """
        Ensures there is an open serial connection.
        Returns:
            bool: True if connection is available, False otherwise.
        """
        if self._connection is not None and getattr(self._connection, "is_open", False):
            return True
        now = time.monotonic()
        if now < self._next_connect_attempt_ts:
            return False
        if self._port == "":
            self._record_event("port unavailable", "No COM port found")
            report("SerialSenderTask", "ERROR", "No COM port found. Cannot send data.")
            self._next_connect_attempt_ts = now + self._connect_retry_seconds
            return False
        try:
            self._connect_attempts += 1
            self._connection = Serial(self._port, 115200, timeout=1, write_timeout=1)
            self._next_connect_attempt_ts = 0.0
            self._record_event(f"connected on {self._port}", "none")
            report(
                "SerialSenderTask",
                "INFO",
                f"[{self._timestamp_now()}] Connect attempt #{self._connect_attempts} succeeded on {self._port}.",
            )
            return True
        except Exception as e:
            error_text = str(e)
            self._record_event("connect failed", error_text)
            report(
                "SerialSenderTask",
                "ERROR",
                f"[{self._timestamp_now()}] Connect attempt #{self._connect_attempts} failed on {self._port}: {error_text}. Retrying in {self._connect_retry_seconds:.1f}s.",
            )
            self._connection = None
            self._next_connect_attempt_ts = now + self._connect_retry_seconds
            return False

    def _transmit(self, data: str) -> None:
        """
        Sends a string of data to the external device via the specified COM port.
        Args:
            data (str): The string of data to send to the external device.
        Note:
            If the COM port is not found (i.e., self._port is an empty string), it logs an error message
            using the report function and does not attempt to send data.
            If there is an error during transmission (e.g., the COM port is unavailable, permission issues, etc.),
            it catches the exception and logs an error message with the details of the failure.
        """
        if not self._ensure_connection():
            return
        try:
            assert self._connection is not None
            self._connection.write(data.encode())
            self._connection.flush()
            with self._status_lock:
                self._last_payload = data
                self._last_payload_time = self._timestamp_now()
            self._record_event("payload sent")
        except Exception as e:
            self._send_failures += 1
            error_text = str(e)
            self._record_event("send failed", error_text)
            report(
                "SerialSenderTask",
                "ERROR",
                f"[{self._timestamp_now()}] Send failure #{self._send_failures} on {self._port}: {error_text}",
            )
            self._close_connection()

    def send(self, data: str) -> None:
        """
        Public method to send data to the serial device.
        This method can be called from other parts of the application to send data without needing to
        directly interact with the threading or queue management.
        Args:
            data (str): The string of data to send to the external device.
        """
        self._transmit(data)

    def _run(self) -> None:
        """
        The main loop for the SerialSenderTask that continuously checks for new data in the queue
        and sends it to the serial device.
        This method runs in a separate thread and will keep running until the stop event is set.
        """
        if self._stop_event is None:
            report("SerialSenderTask", "ERROR", "Stop event is not initialized. Cannot run the task.")
            return
        self._record_event("worker running")
        while not self._stop_event.is_set():
            try:
                payload = self._queue_serial_sender.get(timeout=max(0.1, self._update_interval_seconds))
            except Empty:
                continue

            # Keep only the newest payload to avoid serial backlog when producer is faster.
            while True:
                try:
                    payload = self._queue_serial_sender.get_nowait()
                except Empty:
                    break

            if not payload:
                continue

            self._transmit(payload)
        self._record_event("worker stopped")
        self._close_connection()
