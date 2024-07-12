# Windows:

# Install the following Python modules:
# pip install pyserial
# pip install psutil
# pip install pythonnet
#
# or pip install -r requirements.txt
# 
# Make sure 'OpenHardwareMonitorLib.dll' is in the same folder as this Python script!
#
# Change the COM port under connection (line 31) to match that of the COM port the device is on!

import serial
import serial.tools.list_ports
import os
import time
import clr
import psutil
import datetime

updateTime = 2 # number of seconds between each update

# We are only using a few of these, but I left them here so you can experiment

hardwaretypes = ['Mainboard','SuperIO','CPU','RAM','GpuNvidia','GpuAti','TBalancer','Heatmaster','HDD']
sensortypes = ['Voltage','Clock','Temperature','Load','Fan','Flow','Control','Level','Factor','Power','Data','SmallData']


class CPU:
    load = ''
    temp = ''
    freq = ''

class GPU:
    load = ''
    temp = ''
    freq = ''
    
class Spaces:
    used = ''
    max  = '' 

class Date:
    sec   = ''
    min   = ''
    hour  = ''
    day   = ''
    month = ''
    year  = ''


def sendData(cpu:CPU, gpu:GPU, mem:Spaces, disk:Spaces, date:Date):
    conectionPort = ''
    try:
        pc_ports = list(serial.tools.list_ports.comports())
        for ports in pc_ports:
            if "CH340" in ports.description:
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

def readMonitor(handle, showAllSensors = False):
    cpu = CPU()
    gpu = GPU()
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


def main():
    cpu = CPU()
    gpu = GPU()
    mem = Spaces()
    disk = Spaces()
    date = Date()
    file = 'OpenHardwareMonitorLib'
    clr.AddReference(file)
    from OpenHardwareMonitor import Hardware
    OpenHardwareMonitorHandle = Hardware.Computer()
    OpenHardwareMonitorHandle.MainboardEnabled = True
    OpenHardwareMonitorHandle.CPUEnabled = True
    OpenHardwareMonitorHandle.RAMEnabled = True
    OpenHardwareMonitorHandle.GPUEnabled = True
    OpenHardwareMonitorHandle.Open()
    while(1):
        cpu, gpu = readMonitor(OpenHardwareMonitorHandle)
        obj_Disk = psutil.disk_usage('c:\\') # Drive letter with double \\
        mem.used  = str(psutil.virtual_memory().used / (1024 ** 2))
        mem.max   = str(psutil.virtual_memory().total / (1024 ** 2))
        disk.used = str(obj_Disk.used / (1024.0 ** 3))
        disk.max  = str(obj_Disk.total / (1024.0 ** 3))
        now = datetime.datetime.now()
        date.sec = str(now.second)
        date.min = str(now.minute)
        date.hour = str(now.hour)
        date.day = str(now.day)
        date.month = str(now.month)
        date.year = str(now.year)
        sendData(cpu, gpu, mem, disk, date)
        time.sleep(updateTime)

if __name__ == '__main__':
    main()
