
# The binaries of the Open Hardware Monitor have been made available 
# by the Open Hardware Monitor Project
# https://openhardwaremonitor.org/
#
# under the Mozilla Public License 2.0 (MPL)
# https://www.mozilla.org/en-US/MPL/2.0/
#
#
# This script is distribuited under MIT License
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
# BE CAREFUL DON'T RUN ANY EXECUTABLES ON YOUR COMPUTER!
# MAKE YOUR OWN AFTER READING AND UNDERSTANDING THE SCRIPT.
#
# I DO NOT DISTRIBUTE EXECUTABLES AND I AM NOT RESPONSIBLE FOR ANY DAMAGE CAUSED TO 
# YOUR COMPUTER BY ANY EXECUTABLES THAT MAY COME WITH THIS REPOSITORY!
#
# From Windows:
#
# Install the following Python modules:
#     pip.exe install pyserial psutil pythonnet pyinstaller pstray pillow
#
# Make sure 'OpenHardwareMonitorLib.dll' is in the same folder as this Python script or the exe program
#
# Run py script in console with administratives permissions
#     pythonw.exe .\streamDeco_monitor_windows.py
#
# Create your own exe with commander
#     pyinstaller.exe --icon=streamDeco.ico .\streamDeco_monitor_windows.pyw
#
# The COM device is searched until find ESP32 USB chip, in my case the chip is named as CH340


updateTime = 1       # monitor update interval
boardCOM   = "CH340" # USB/TTL on ESP32 8048s43c Board, check your controler
debugCode  = False   # Show informations in terminal, only to terminal execution

class MetricCPU:
    def __init__(self) -> None:
        self.load = ''
        self.temperature = ''
        self.frequency = ''
        
    def parse(self, sensor) -> None:
        if str(sensor.SensorType) == 'Load' and sensor.Name == 'CPU Total':
            self.load = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Temperature' and sensor.Name == 'CPU Package':
            self.temperature = str(int(sensor.Value))
        if str(sensor.SensorType) == 'Clock' and sensor.Name == 'CPU Core #1':
            self.frequency = str(int(sensor.Value))
            
    def decode(self, last = False):
        _data = self.load + ',' + self.temperature + ',' + self.frequency
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
        import psutil
        self.used = ''
        self.max = ''
        self.metric = psutil.virtual_memory
    
    def read(self) -> None:
        self.used  = str(self.metric().used / (1024 ** 2))
        self.max   = str(self.metric().total / (1024 ** 2))
        
    def decode(self, last = False):
        _data = self.used + ',' + self.max
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data
      
        
class MetricDISK(MetricRAM):
    def __init__(self) -> None:
        import psutil
        self.used = ''
        self.max = ''
        self.metric = psutil.disk_usage
    
    def read(self, letterDrive:str) -> None:
        drive = self.metric(letterDrive + ':\\') # Drive letter with double \\
        self.used = str(drive.used / (1024.0 ** 3))
        self.max  = str(drive.total / (1024.0 ** 3))


class MetricDate:
    def __init__(self) -> None:
        import datetime
        self.sec = ''
        self.min = ''
        self.hour = ''
        self.day = ''
        self.month = ''
        self.year = ''
        self.metric = datetime.datetime
    
    def read(self) -> None:
        now = self.metric.now()
        self.sec = str(now.second)
        self.min = str(now.minute)
        self.hour = str(now.hour)
        self.day = str(now.day)
        self.month = str(now.month)
        self.year = str(now.year)
    
    def decode(self, last = False):
        _data = self.sec  + ',' + self.min   + ',' + self.hour + ','
        _data = _data + self.day  + ',' + self.month + ',' + self.year
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data


class StreamMonitor:
    def __init__(self, boardName:str, debug = False) -> None:
        from time import sleep
        import serial
        self._sleep = sleep
        self._port = 'FAIL'
        self._name = boardName
        self._debug = debug
        self._port = self._initPortConnection()
        while self._port == 'FAIL':
            self._port = self._initPortConnection()
            if self._debug == True:
                print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
            self._sleep(10)
               
    def send(self, data:str) -> None:
        import serial
        try:
            connection = serial.Serial(self._port, 115200)
            connection.write(data.encode())
            if self._debug == True:
                print("Data written", data.encode())
            connection.close() 
        except Exception as e:
            if self._debug == True:
                print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
                print(e)
            self._sleep(10)

    def _initPortConnection(self):
        import serial.tools.list_ports
        conectionPort:str
        try:
            pcPorts = list(serial.tools.list_ports.comports())
            for ports in pcPorts:
                if self._name in ports.description:
                    conectionPort = ports.name
                    if self._debug == True:
                        print("Using device " + ports.description)
        except Exception as e:
            conectionPort = 'FAIL'
            if self._debug == True:
                print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
                print(e)
        return conectionPort


class LibreHardwareMonitor:
    def __init__(self, debug = False) -> None:
        import clr
        fileLib = 'LibreHardwareMonitorLib'
        clr.AddReference(fileLib)
        from LibreHardwareMonitor import Hardware
        self._handle = Hardware.Computer()
        self._handle.IsCpuEnabled = True
        self._handle.IsGpuEnabled = True
        #self._handle.IsMemoryEnabled = True
        #self._handle.IsMotherboardEnabled = True
        #self._handle.IsControllerEnabled = True
        #self._handle.IsNetworkEnabled = True
        #self._handle.IsStorageEnabled = True
        self._handle.Open()
        self._cpu = MetricCPU()
        self._gpu = MetricGPU()
        self._debug = debug

    def read(self):
        for hardware in self._handle.Hardware:
            hardware.Update()
            for sensor in hardware.Sensors:
                if sensor.Value is not None:
                    self._cpu.parse(sensor)
                    self._gpu.parse(sensor)
                    if self._debug == True:
                        data = str(sensor.SensorType) + " : "
                        data = data + sensor.Name + " : " 
                        data = data + str(sensor.Value)
                        print(data)
            for subHardware in hardware.SubHardware:
                subHardware.Update()
                for subsensor in subHardware.Sensors:
                    if subsensor.Value is not None:
                        self._cpu.parse(subsensor)
                        self._gpu.parse(subsensor)
                        if self._debug == True:
                            data = str(subsensor.SensorType) + " : "
                            data = data + subsensor.Name + " : "
                            data = data + str(subsensor.Value)
                            print(data)
        return self._cpu, self._gpu


class OpenHardwareMonitor:
    def __init__(self, debug = False) -> None:
        import clr
        fileLib = 'OpenHardwareMonitorLib'
        clr.AddReference(fileLib)
        from OpenHardwareMonitor import Hardware
        self.handle = Hardware.Computer()
        self.handle.CPUEnabled = True
        self.handle.GPUEnabled = True
        #self.handle.MainboardEnabled = True
        #self.handle.RAMEnabled = True
        #self.handle.HDDEnabled = True
        self._handle.Open()
        self._cpu = MetricCPU()
        self._gpu = MetricGPU()
        self._debug = debug

    def read(self):
        for hardware in self._handle.Hardware:
            hardware.Update()
            for sensor in hardware.Sensors:
                if sensor.Value is not None:
                    self._cpu.parse(sensor)
                    self._gpu.parse(sensor)
                    if self._debug == True:
                        data = str(sensor.SensorType) + " : "
                        data = data + sensor.Name + " : "
                        data = data + str(sensor.Value)
                        print(data)
            for subHardware in hardware.SubHardware:
                subHardware.Update()
                for subsensor in subHardware.Sensors:
                    if subsensor.Value is not None:
                        self._cpu.parse(subsensor)
                        self._gpu.parse(subsensor)
                        if self._debug == True:
                            data = str(subsensor.SensorType) + " : " 
                            data = data + subsensor.Name + " : "
                            data = data + str(subsensor.Value)
                            print(data)
        return self._cpu, self._gpu


class Task:
    def __init__(self, target:callable) -> None:
        import multiprocessing
        if sys.platform.startswith('win'):
            multiprocessing.freeze_support()
        self.task = multiprocessing.Process(target = target)

    def start(self) -> None:
        self.task.start()

    def close(self, tray) -> None:
        tray.stop()
        self.task.terminate()


def monitor() -> None:
    from time import sleep
    hardwareMonitor = LibreHardwareMonitor(debug=debugCode) # this work better on my processor
    streamMonitor = StreamMonitor(boardName = boardCOM, debug=debugCode)
    cpu = MetricCPU()
    gpu = MetricGPU()
    mem = MetricRAM()
    disk = MetricDISK()
    date = MetricDate()
    while(1):
        cpu, gpu = hardwareMonitor.read()
        mem.read()
        disk.read('c')
        date.read()
        data = cpu.decode() + gpu.decode()
        data = data + mem.decode() + disk.decode()
        data = data + date.decode(last = True)
        streamMonitor.send(data)
        sleep(updateTime)


if __name__ == "__main__":
    import sys
    import pystray
    import PIL.Image
    
    task = Task(monitor)
    iconTray = PIL.Image.open("streamDeco.ico")
    tray = pystray.Icon(name = "StreamDeco", title = "StreamDeco", icon = iconTray, 
                        menu = pystray.Menu(
                                            pystray.MenuItem("Sair", task.close)
                               )
                       )
    task.start()
    tray.run_detached()
