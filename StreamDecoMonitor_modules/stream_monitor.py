from __future__ import annotations
from .report import report

class StreamMonitor:
    
    def __init__(self, boardCOM: str) -> None:
        self._port: str = boardCOM

    def send(self, data: str) -> None:
        from serial import Serial
        if self._port == "":
            report("StreamMonitor", "ERROR", "No COM port found. Cannot send data.")
            return
        try:
            connection = Serial(self._port, 115200, timeout=1)
            connection.write(data.encode())
            connection.close()
        except Exception as e:
            report("StreamMonitor", "ERROR", f"Failed to send data: {e}")
