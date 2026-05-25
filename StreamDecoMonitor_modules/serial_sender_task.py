from __future__ import annotations

import threading
from queue import Empty, Full, Queue

from .stream_monitor import StreamMonitor
from .report import report


class SerialSenderTask:
    def __init__(self, boardCOM: str, queue_serial_sender: Queue[str]) -> None:
        self._stream_monitor = StreamMonitor(boardCOM=boardCOM)
        self._stop_event = threading.Event()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._queue_serial_sender = queue_serial_sender

    def start(self) -> None:
        if self._thread.is_alive():
            return
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._stop_event.set()
        try:
            self._queue_serial_sender.put_nowait("")
        except Full:
            pass
        if self._thread.is_alive():
            self._thread.join(timeout=1.5)


    def _run(self) -> None:
        while not self._stop_event.is_set():
            try:
                payload = self._queue_serial_sender.get(timeout=0.5)
            except Empty:
                continue

            if not payload:
                continue

            self._stream_monitor.send(payload)
            report("SerialSenderTask", "INFO", f"Sent payload: {payload}")
