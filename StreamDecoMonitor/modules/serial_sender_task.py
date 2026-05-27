from __future__ import annotations

import threading
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
        if run_task and queue_serial_sender is not None:
            self._stop_event = threading.Event()
            self._thread = threading.Thread(target=self._run, daemon=True)
            self._update_interval_seconds = update_interval_seconds
            self._queue_serial_sender = queue_serial_sender
    
    def set_port(self, port: str) -> None:
        """
        Sets the COM port for the serial device.
        Args:
            port (str): The COM port to set for the serial device.
        """
        self._port = port
    
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
        try:
            self._queue_serial_sender.put_nowait("")
        except Full:
            report("SerialSenderTask", "WARNING", "Failed to send stop signal, queue is full.")
            pass
        if self._thread is not None and self._thread.is_alive():
            self._thread.join(timeout=1.5)

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
        if self._port == "":
            report("SerialSenderTask", "ERROR", "No COM port found. Cannot send data.")
            return
        try:
            connection = Serial(self._port, 115200, timeout=1)
            connection.write(data.encode())
            connection.close()
            report("SerialSenderTask", "INFO", f"Successfully sent data to {self._port}: {data}")
        except Exception as e:
            report("SerialSenderTask", "ERROR", f"Failed to send data: {e}")

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
        while not self._stop_event.is_set():
            try:
                payload = self._queue_serial_sender.get(timeout=0.5)
            except Empty:
                report("SerialSenderTask", "WARNING", "No payload to send, queue is empty.")
                continue

            if not payload:
                report("SerialSenderTask", "WARNING", "Received empty payload, skipping.")
                continue

            self._transmit(payload)
            report("SerialSenderTask", "INFO", f"Sent payload: {payload}")
