
# The binaries of the Open Hardware Monitor have been made available 
# by the Open Hardware Monitor Project
# https://openhardwaremonitor.org/
#
# under the Mozilla Public License 2.0 (MPL)
# https://www.mozilla.org/en-US/MPL/2.0/
#
#
# This script is distributed under MIT License
# 
# Copyright © 2024 Marcelo H Moraes
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# BE CAREFUL DON'T RUN ANY UNKNOWN EXECUTABLES ON YOUR COMPUTER!
# MAKE YOUR OWN AFTER READING AND UNDERSTANDING THE SCRIPT.
#
#  ADDITIONAL TERMS, DON'T RUN THIS PROGRAM IF YOU DON'T AGREE WITH THIS
#
#  1 - I DO NOT DISTRIBUTE EXECUTABLES AND I AM NOT RESPONSIBLE FOR ANY DAMAGE CAUSED TO 
#      YOUR COMPUTER BY ANY EXECUTABLES THAT MAY COME WITH THIS REPOSITORY!
#  
#  2 - I AM NOT RESPONSIBLE FOR ANY DECISION MADE BY THE PRESIDENT, OR DICTATOR, OF ANY COUNTRY!
#
#  3 - I'M NOT GOING TO SAY ANYTHING ABOUT ANY COUNTRY OR ABOUT THE PRESIDENT, OR DICTATOR, OF ANY COUNTY!
#
#  4 - ALSO I'M NOT GOING TO SAY ANYTHING ABOUT WHAT HAPPEN ON ANY COUNTRY BY DECISION MADE BY THE PRESIDENT, 
#      OR DICTATOR, OF ANY COUNTRY!
#  
#  5 - YOU ARE FREE TO DO THE SAME IF YOU WANT
#
# From Windows:
#
# Install the following Python modules:
#     pip.exe install pyserial psutil pythonnet pyinstaller pstray pillow
#
# Make sure that 'OpenHardwareMonitorLib.dll', 'LibreHardwareMonitorLib.dll' and 'HidSharp.dll' is in the same folder 
# as this Python script or the exe program
#
# Run py script in console with administrative permissions
#     pythonw.exe .\streamDeco_monitor_windows.py
#
# Create your own exe with commander
#     pyinstaller.exe --icon=streamDeco.ico .\streamDeco_monitor_windows.pyw
#
# The COM device is searched until find ESP32 USB chip
#

updateTime = 1       # monitor update interval
boardCOM   = 'CH340' # USB/TTL on ESP32 8048s43c Board, check your controller
debugCode  = False   # Show information in terminal, only to terminal execution
monitorFull = False  # If true enable all sensors in monitor, else only CPU and GPU sensors are enabled

class MetricCPU:
    def __init__(self) -> None:
        self.load:str = ''
        self.temperature:str = ''
        self.frequency:str = ''
        
    def parse(self, sensor) -> None:
        if str(sensor.SensorType) == 'Load' and sensor.Name == 'CPU Total':
            self.load = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Temperature' and sensor.Name == 'CPU Package':
            self.temperature = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Clock' and sensor.Name == 'CPU Core #1':
            self.frequency = str(int(sensor.Value))
            
    def decode(self, last:bool = False) -> str:
        _data = f'{self.load}, {self.temperature}, {self.frequency}'
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data
        
            
class MetricGPU(MetricCPU):
    def parse(self, sensor) -> None:
        if str(sensor.SensorType) == 'Load' and sensor.Name == 'GPU Core':
            self.load = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Temperature' and sensor.Name == 'GPU Core':
            self.temperature = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Clock' and sensor.Name == 'GPU Core':
            self.frequency = str(int(sensor.Value))


class MetricRAM:
    def __init__(self) -> None:
        from psutil import virtual_memory, disk_usage
        self.used:str = ''
        self.max:str = ''
        self.memory:callable = virtual_memory
        self.disk:callable = disk_usage
    
    def read(self) -> None:
        self.used  = str(self.memory().used / (1024 ** 2))
        self.max   = str(self.memory().total / (1024 ** 2))
        
    def decode(self, last:bool = False) -> str:
        _data = f'{self.used}, {self.max}'
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data


class MetricDISK(MetricRAM):
    def read(self, letterDrive:str) -> None:
        drive = self.disk(f'{letterDrive}:\\') # Drive letter with double \\
        self.used = str(drive.used / (1024.0 ** 3))
        self.max  = str(drive.total / (1024.0 ** 3))


class MetricDate:
    def __init__(self) -> None:
        from datetime import datetime
        self.sec:str = ''
        self.min:str = ''
        self.hour:str = ''
        self.day:str = ''
        self.month:str = ''
        self.year:str = ''
        self.metric:object = datetime
    
    def read(self) -> None:
        now = self.metric.now()
        self.sec = str(now.second)
        self.min = str(now.minute)
        self.hour = str(now.hour)
        self.day = str(now.day)
        self.month = str(now.month)
        self.year = str(now.year)

    def decode(self, last = False) -> str:
        _data = f'{self.sec}, {self.min}, {self.hour},'
        _data = _data + f'{self.day}, {self.month}, {self.year}'
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data


class StreamMonitor:
    def __init__(self, boardName:str, debug = False) -> None:
        from time import sleep
        self._sleep:callable = sleep
        self._port:str = 'FAIL'
        self._name:str = boardName
        self._debug:bool = debug
        self._port:callable = self._initPortConnection()
        while self._port == 'FAIL':
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


class LibreHardwareMonitor:
    def __init__(self, debug = False, monitorAll = False) -> None:
        import clr
        fileLib = 'LibreHardwareMonitorLib'
        clr.AddReference(fileLib)
        from LibreHardwareMonitor import Hardware
        self._handle = Hardware.Computer()
        self._handle.IsCpuEnabled = True
        self._handle.IsGpuEnabled = True
        if monitorAll == True:
            self._handle.IsMemoryEnabled = True
            self._handle.IsMotherboardEnabled = True
            self._handle.IsControllerEnabled = True
            self._handle.IsNetworkEnabled = True
            self._handle.IsStorageEnabled = True
        self._handle.Open()
        self._cpu = MetricCPU()
        self._gpu = MetricGPU()
        self._debug = debug

    def read(self) -> list[MetricCPU, MetricGPU]:
        for hardware in self._handle.Hardware:
            hardware.Update()
            for sensor in hardware.Sensors:
                if sensor.Value is not None:
                    self._cpu.parse(sensor)
                    self._gpu.parse(sensor)
                    if self._debug == True:
                        data = f'{str(sensor.SensorType)} : '
                        data = data + f'{sensor.Name} : ' 
                        data = data + str(sensor.Value)
                        print(data)
            for subHardware in hardware.SubHardware:
                subHardware.Update()
                for subsensor in subHardware.Sensors:
                    if subsensor.Value is not None:
                        self._cpu.parse(subsensor)
                        self._gpu.parse(subsensor)
                        if self._debug == True:
                            data = f'{str(subsensor.SensorType)} : '
                            data = data + f'{subsensor.Name} : '
                            data = data + str(subsensor.Value)
                            print(data)
        return self._cpu, self._gpu
    
    def decode(self, last = False) -> str:
        return self._cpu.decode() + self._gpu.decode(last)


class OpenHardwareMonitor(LibreHardwareMonitor):
    def __init__(self, debug = False, monitorAll = False) -> None:
        import clr
        fileLib = 'OpenHardwareMonitorLib'
        clr.AddReference(fileLib)
        from OpenHardwareMonitor import Hardware
        self._handle = Hardware.Computer()
        self._handle.CPUEnabled = True
        self._handle.GPUEnabled = True
        if monitorAll == True:
            self._handle.MainboardEnabled = True
            self._handle.RAMEnabled = True
            self._handle.HDDEnabled = True
        self._handle.Open()
        self._cpu = MetricCPU()
        self._gpu = MetricGPU()
        self._debug = debug


class Task():
    def __init__(self, target:callable) -> None:
        import sys
        import multiprocessing
        if sys.platform.startswith('win'):
            multiprocessing.freeze_support()
        self._task:object = multiprocessing.Process(target = target)

    def start(self) -> None:
        self._task.start()

    def close(self, tray) -> None:
        tray.stop()
        self._task.terminate()


def monitor() -> None:
    global debugCode
    global monitorFull
    from time import sleep
    import timeit
    hardwareMonitor = LibreHardwareMonitor(debug=debugCode, monitorAll=monitorFull) # this work better on my processor
    streamMonitor = StreamMonitor(boardName = boardCOM, debug=debugCode)
    cpu = MetricCPU()
    gpu = MetricGPU()
    mem = MetricRAM()
    disk = MetricDISK()
    date = MetricDate()
    while(1):
        if debugCode:
            print('Hardware read:\t\t', timeit.timeit(hardwareMonitor.read, number=1))
        else:
            hardwareMonitor.read()
        mem.read()
        disk.read('c')
        date.read()
        data = hardwareMonitor.decode()
        data = data + mem.decode() + disk.decode()
        data = data + date.decode(last = True)
        streamMonitor.send(data)
        sleep(updateTime)


if __name__ == '__main__':
    import pystray
    import PIL.Image
    
    task = Task(monitor)
    iconTray = PIL.Image.open('streamDeco.ico')
    tray = pystray.Icon(name = 'StreamDeco', title = 'StreamDeco', icon = iconTray, 
                        menu = pystray.Menu(
                                            pystray.MenuItem('Sair', task.close)
                               )
                       )
    task.start()
    tray.run_detached()
