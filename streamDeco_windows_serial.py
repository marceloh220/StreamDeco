from typing import Callable

class StreamMonitor:
    def __init__(self, boardName:str, debug = False) -> None:
        from time import sleep
        self._sleep:Callable = sleep
        self._port:str = "Fail"
        self._name:str = boardName
        self._debug:bool = debug
        self._port = self._initPortConnection()
        while self._port == "Fail":
            self._port = self._initPortConnection()
            if self._debug == True:
                print('No device found. Please, connect SteamDeco on USB to use monitor functions.')
            self._sleep(10)

    def send(self, data:str) -> None:
        from serial import Serial
        try:
            connection = Serial(self._port, 115200)
            connection.write(data.encode())
            if self._debug == True:
                print('Data written', data.encode())
            connection.close() 
        except Exception as e:
            if self._debug == True:
                print('No device found. Please, connect SteamDeco on USB to use monitor functions.')
                print(e)
            self._sleep(10)

    def _initPortConnection(self) -> str:
        from serial.tools.list_ports import comports
        connectionPort:str = ''
        try:
            pcPorts = list(comports())
            for ports in pcPorts:
                if self._name in ports.description:
                    connectionPort = ports.name
                    if self._debug == True:
                        print('Using device ' + ports.description)
        except Exception as e:
            connectionPort = 'FAIL'
            if self._debug == True:
                print('No device found. Please, connect SteamDeco on USB to use monitor functions.')
                print(e)
        return connectionPort