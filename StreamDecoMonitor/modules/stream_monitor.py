from __future__ import annotations
from .report import report

class StreamMonitor:
    """
    A class responsible for sending hardware metrics data to an external device (e.g., an Arduino) 
    via a serial COM port.
    Attributes:
        _port (str): The COM port to which the external device is connected. 
            This is determined by the search_com_port function.
    Methods:
        __init__(self, boardCOM: str) -> None: Initializes the StreamMonitor with 
            the specified COM port.
        send(self, data: str) -> None: Sends a string of data to the external
            device via the specified COM port. If the COM port is not found or if there is an error
            during transmission, it logs an error message using the report function.
    """
    
    def __init__(self, boardCOM: str) -> None:
        """
        Initializes the StreamMonitor with the specified COM port.
        Args:
            boardCOM (str): The COM port to which the external device is connected. 
                This is determined by the search_com_port function.
        """
        self._port: str = boardCOM

    def send(self, data: str) -> None:
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
