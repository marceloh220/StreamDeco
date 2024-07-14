'''

The binaries of the Open Hardware Monitor have been made available 
by the Open Hardware Monitor Project
https://openhardwaremonitor.org/

under the Mozilla Public License 2.0 (MPL)
https://www.mozilla.org/en-US/MPL/2.0/



This script is distribuited under MIT License

Copyright © 2024 Marcelo H Moraes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

BE CAREFUL DON'T RUN ANY EXECUTABLES ON YOUR COMPUTER!
MAKE YOUR OWN AFTER READING AND UNDERSTANDING THE SCRIPT.

I DO NOT DISTRIBUTE EXECUTABLES AND I AM NOT RESPONSIBLE FOR ANY DAMAGE CAUSED TO 
YOUR COMPUTER BY ANY EXECUTABLES THAT MAY COME WITH THIS REPOSITORY!

From Windows:

Install the following Python modules:
    pip.exe install pyserial psutil pythonnet pyinstaller

Make sure 'OpenHardwareMonitorLib.dll' is in the same folder as this Python script or the exe program

Run py script in console with administratives permissions

Create your own exe with commander
    pyinstaller.exe -F .\streamDeco_monitor_windows.py

The COM device is searched until find ESP32 USB chip, in my case the chip is named as CH340
'''

import serial
import serial.tools.list_ports
import os
import time
import clr
import psutil
import datetime

updateTime = 2       # monitor update interval
boardCOM   = "CH340" # USB/TTL on ESP32 8048s43c Board, check your controler

class xPU:
    load = ''
    temp = ''
    freq = ''
    
class xMEM:
    used = ''
    max  = '' 

class Date:
    sec   = ''
    min   = ''
    hour  = ''
    day   = ''
    month = ''
    year  = ''


def sendData(cpu:xPU, gpu:xPU, mem:xMEM, disk:xMEM, date:Date, conectionPort:str):
    try:
        connection = serial.Serial(conectionPort, 115200)
        data = cpu.load + ',' + cpu.temp + ',' + cpu.freq + ',' 
        data = data + gpu.load + ',' + gpu.temp + ',' + gpu.freq + ',' 
        data = data + mem.used + ',' + mem.max + ',' 
        data = data + disk.used + ',' +  disk.max + ',' 
        data = data + date.sec + ',' + date.min + ',' + date.hour + ',' 
        data = data + date.day + ',' + date.month + ',' + date.year + '/'
        connection.write(data.encode())
        print("Data written", data.encode())
        connection.close  
    except Exception as e:
        print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
        time.sleep(10)

def initPortConnection():
    conectionPort:str
    try:
        pcPorts = list(serial.tools.list_ports.comports())
        for ports in pcPorts:
            if boardCOM in ports.description:
                conectionPort = ports.name
                print("Using device  USB-SERIAL " + ports.description)
                return conectionPort
    except Exception as e:
        print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
        time.sleep(10)
    return "FAIL"

def initOpenHardwareMonitor():
    OpenHardwareMonitorFile = 'OpenHardwareMonitorLib'
    clr.AddReference(OpenHardwareMonitorFile)
    from OpenHardwareMonitor import Hardware
    OpenHardwareMonitorHandle = Hardware.Computer()
    OpenHardwareMonitorHandle.CPUEnabled = True
    OpenHardwareMonitorHandle.RAMEnabled = True
    OpenHardwareMonitorHandle.GPUEnabled = True
    OpenHardwareMonitorHandle.Open()
    return OpenHardwareMonitorHandle

def readOpenHardwareMonitor(handle, showAllSensors = False):
    cpu = xPU()
    gpu = xPU()
    for hardware in handle.Hardware:
        hardware.Update()
        for sensor in hardware.Sensors:
            if sensor.Value is not None:
                if str(sensor.SensorType) == 'Load' and sensor.Name == 'CPU Total':
                    cpu.load = str(int(sensor.Value))
                if str(sensor.SensorType) == 'Temperature' and sensor.Name == 'CPU Package':
                    cpu.temp = str(int(sensor.Value))
                if str(sensor.SensorType) == 'Clock' and sensor.Name == 'CPU Core #1':
                    cpu.freq = str(int(sensor.Value))
                if str(sensor.SensorType) == 'Load' and sensor.Name == 'GPU Core':
                    gpu.load = str(int(sensor.Value))
                if str(sensor.SensorType) == 'Temperature' and sensor.Name == 'GPU Core':
                    gpu.temp = str(int(sensor.Value))
                if str(sensor.SensorType) == 'Clock' and sensor.Name == 'GPU Core':
                    gpu.freq = str(int(sensor.Value))
                if showAllSensors:
                    print(str(sensor.SensorType) + " : " + sensor.Name + " : " + str(sensor.Value))
        for subHardware in hardware.SubHardware:
            subHardware.Update()
            for subsensor in subHardware.Sensors:
                if subsensor.Value is not None:
                    if str(subsensor.SensorType) == 'Load' and subsensor.Name == 'CPU Total':
                        cpu.load = str(int(subsensor.Value))
                    if str(subsensor.SensorType) == 'Temperature' and subsensor.Name == 'CPU Package':
                        cpu.temp = str(int(subsensor.Value))
                    if str(subsensor.SensorType) == 'Clock' and subsensor.Name == 'CPU Core #1':
                        cpu.freq = str(int(subsensor.Value))
                    if str(subsensor.SensorType) == 'Load' and subsensor.Name == 'GPU Core':
                        gpu.load = str(int(subsensor.Value))
                    if str(subsensor.SensorType) == 'Temperature' and subsensor.Name == 'GPU Core':
                        gpu.temp = str(int(subsensor.Value))
                    if str(subsensor.SensorType) == 'Clock' and subsensor.Name == 'GPU Core':
                        gpu.freq = str(int(subsensor.Value))
                    if showAllSensors:
                        print(str(subsensor.SensorType) + " : " + subsensor.Name + " : " + str(subsensor.Value))
    return cpu, gpu

def readMem():
    mem = xMEM()
    mem.used  = str(psutil.virtual_memory().used / (1024 ** 2))
    mem.max   = str(psutil.virtual_memory().total / (1024 ** 2))
    return mem

def readDiskDrive(letterDrive = 'c'):
    disk = xMEM()
    drive = psutil.disk_usage(letterDrive + ':\\') # Drive letter with double \\
    disk.used = str(drive.used / (1024.0 ** 3))
    disk.max  = str(drive.total / (1024.0 ** 3))
    return disk

def readDate():
    date = Date()
    now = datetime.datetime.now()
    date.sec = str(now.second)
    date.min = str(now.minute)
    date.hour = str(now.hour)
    date.day = str(now.day)
    date.month = str(now.month)
    date.year = str(now.year)
    return date
    
def main():
    cpu = xPU()
    gpu = xPU()
    mem = xMEM()
    disk = xMEM()
    date = Date()
    sensors = initOpenHardwareMonitor()
    conectionPort = initPortConnection()
    while conectionPort == "FAIL":
        conectionPort = initPortConnection()
        print("No device found. Please, connect SteamDeco on USB to use monitor functions.")
        time.sleep(10)
    while(1):
        cpu, gpu = readOpenHardwareMonitor(sensors)
        mem  = readMem()
        disk = readDiskDrive('c')
        date = readDate()
        sendData(cpu, gpu, mem, disk, date, conectionPort)
        time.sleep(updateTime)


if __name__ == '__main__':
    main()
