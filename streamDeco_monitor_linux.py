#!/usr/bin/env python3

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
# From Linux:
#
# Install the following Python modules:
#     pip.exe install pyserial psutil pyamdgpuinfo pystray pillow
#
# Or using package manager (Arch linux)
#     yay -S python-{pyserial,psutil,pyamdgpuinfo,pystray,pillow}
#
#
# The USB device is searched until find ESP32 USB chip
#

updateTime = 1       # monitor update interval
boardCOM   = 'USB Serial' # USB/TTL on ESP32 8048s43c Board, check your controller
debugCode  = True   # Show information in terminal, only to terminal execution

import psutil
import pyamdgpuinfo
from datetime import datetime
from time import sleep

class MetricCPU:
    def __init__(self) -> None:
        self.load:str = ''
        self.temperature:str = ''
        self.frequency:str = ''
        
    def read(self) -> None:
        self.load  = str(psutil.cpu_percent())
        self.temperature = str(psutil.sensors_temperatures()["coretemp"][0].current)
        self.frequency = str(psutil.cpu_freq().current)
            
    def decode(self, last:bool = False) -> str:
        _data = f'{self.load}, {self.temperature}, {self.frequency}'
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data
        
            
class MetricGPU(MetricCPU):
    def read(self) -> None:
        _gpu = pyamdgpuinfo.get_gpu(0)
        self.load  = str(_gpu.query_load())
        self.temperature = str(_gpu.query_temperature())
        self.frequency = str(_gpu.query_sclk()/1000/1000)


class MetricRAM:
    def __init__(self) -> None:
        self.used:str = ''
        self.max:str = ''
    
    def read(self) -> None:
        self.used  = str(psutil.virtual_memory().used / (1024 ** 2))
        self.max   = str(psutil.virtual_memory().total / (1024 ** 2))
        
    def decode(self, last:bool = False) -> str:
        _data = f'{self.used}, {self.max}'
        if last == True:
            _data = _data + '/'
        else:
            _data = _data + ','
        return _data


class MetricDISK(MetricRAM):
    def read(self, letterDrive:str) -> None:
        drive = psutil.disk_usage(letterDrive)
        self.used = str(drive.used / (1024.0 ** 3))
        self.max  = str(drive.total / (1024.0 ** 3))


class MetricDate:
    def __init__(self) -> None:
        self.sec:str = ''
        self.min:str = ''
        self.hour:str = ''
        self.day:str = ''
        self.month:str = ''
        self.year:str = ''
    
    def read(self) -> None:
        now = datetime.now()
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
        self._port:str = 'FAIL'
        self._name:str = boardName
        self._debug:bool = debug
        self._port:callable = self._initPortConnection()
        while self._port == 'FAIL':
            self._port = self._initPortConnection()
            if self._debug == True:
                print('No device found. Please, connect SteamDeco on USB to use monitor functions.')
            sleep(10)

    def send(self, data:str) -> None:
        from serial import Serial
        try:
            connection = Serial(f'/dev/{self._port}', 115200)
            connection.write(data.encode())
            if self._debug == True:
                print('Data written', data.encode())
            connection.close() 
        except Exception as e:
            if self._debug == True:
                print('No device found. Please, connect SteamDeco on USB to use monitor functions.')
                print(e)
            sleep(10)

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
    import timeit
    streamMonitor = StreamMonitor(boardName = boardCOM, debug=debugCode)
    cpu = MetricCPU()
    gpu = MetricGPU()
    mem = MetricRAM()
    disk = MetricDISK()
    date = MetricDate()
    while(1):
        cpu.read()
        gpu.read()
        mem.read()
        disk.read('/')
        date.read()
        data = cpu.decode() + gpu.decode()
        data = data + mem.decode() + disk.decode()
        data = data + date.decode(last = True)
        streamMonitor.send(data)
        sleep(updateTime)


if __name__ == '__main__':
    import pystray
    import PIL.Image
    
    task = Task(monitor)
    iconTray = PIL.Image.open('/usr/share/icons/streamdeco-monitor/streamDeco.ico')
    tray = pystray.Icon(name = 'StreamDeco', title = 'StreamDeco', icon = iconTray, 
                        menu = pystray.Menu(pystray.MenuItem('Sair', task.close))
                       )
    task.start()
    tray.run()
