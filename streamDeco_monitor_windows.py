'''
From Windows:

Install the following Python modules:
    pip.exe install pyserial psutil pythonnet

Make sure 'OpenHardwareMonitorLib.dll' is in the same folder as this Python script or the exe program

Run exe program as administrator,
or run py script in console with administratives permissions

If you don't trust in AMERICA... your comunist ¬_¬ ... create your own exe
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
boardCOM   = "CH340" # USB/TTL on StreamDeco Board, check your controler

class xPU:
    load = ''
    temp = ''
    freq = ''
    
class xRAM:
    used = ''
    max  = '' 

class Date:
    sec   = ''
    min   = ''
    hour  = ''
    day   = ''
    month = ''
    year  = ''


def sendData(cpu:xPU, gpu:xPU, mem:xRAM, disk:xRAM, date:Date):
    conectionPort = ''
    try:
        pcPorts = list(serial.tools.list_ports.comports())
        for ports in pcPorts:
            if boardCOM in ports.description:
                conectionPort = ports.name
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
        print(e)

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
    mem = xRAM()
    mem.used  = str(psutil.virtual_memory().used / (1024 ** 2))
    mem.max   = str(psutil.virtual_memory().total / (1024 ** 2))
    return mem

def readDiskDrive(letterDrive = 'c'):
    disk = xRAM()
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
    mem = xRAM()
    disk = xRAM()
    date = Date()
    sensors = initOpenHardwareMonitor()
    while(1):
        cpu, gpu = readOpenHardwareMonitor(sensors)
        mem  = readMem()
        disk = readDiskDrive('c')
        date = readDate()
        sendData(cpu, gpu, mem, disk, date)
        time.sleep(updateTime)


if __name__ == '__main__':
    pcPorts = list(serial.tools.list_ports.comports())
    for ports in pcPorts:
        print(ports)
    main()
