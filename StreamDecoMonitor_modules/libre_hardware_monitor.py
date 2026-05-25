import os
from .report import report


class MetricBase:

    def __init__(self) -> None:
        self.load: str = ""
        self.temperature: str = ""
        self.frequency: str = ""

    def parse(self, sensor) -> None:
        pass

    def decode(self, last: bool = False) -> str:
        _data = f"{self.load}, {self.temperature}, {self.frequency}"
        if last is True:
            _data = _data + "/"
        else:
            _data = _data + ","
        return _data


class MetricCPU(MetricBase):
    
    def parse(self, sensor) -> None:
        if str(sensor.SensorType) == "Load" and sensor.Name == "CPU Total":
            self.load = sensor.Value
            report("MetricCPU", "INFO", f"CPU Load updated: {self.load}")
        if str(sensor.SensorType) == "Temperature" and sensor.Name == "CPU Package":
            self.temperature = sensor.Value
            report("MetricCPU", "INFO", f"CPU Temperature updated: {self.temperature}")
        if str(sensor.SensorType) == "Clock" and sensor.Name == "CPU Core #1":
            self.frequency = sensor.Value
            report("MetricCPU", "INFO", f"CPU Frequency updated: {self.frequency}")


class MetricGPU(MetricBase):
    def __init__(self) -> None:
        super().__init__()
        self.d3d_load: str = ""
    def parse(self, sensor) -> None:
        if str(sensor.SensorType) == "Load" and sensor.Name == "D3D 3D":
            self.d3d_load = sensor.Value
            report("MetricGPU", "INFO", f"GPU D3D Load updated: {self.d3d_load}")
        if str(sensor.SensorType) == "Load" and sensor.Name == "GPU Core":
            self.load = sensor.Value
            report("MetricGPU", "INFO", f"GPU Load updated: {self.load}")
        if str(sensor.SensorType) == "Temperature" and sensor.Name == "GPU Core":
            self.temperature = sensor.Value
            report("MetricGPU", "INFO", f"GPU Temperature updated: {self.temperature}")
        if str(sensor.SensorType) == "Clock" and sensor.Name == "GPU Core":
            self.frequency = sensor.Value
            report("MetricGPU", "INFO", f"GPU Frequency updated: {self.frequency}")


class LibreHardwareMonitor:

    def __init__(self, monitorAll: bool = False) -> None:
        import clr

        fileLib = "LibreHardwareMonitorLib"
        if not os.path.isfile(fileLib + ".dll"):
            report("LibreHardwareMonitor", "ERROR",
                f"{fileLib}.dll not found. Please, make sure that {fileLib}.dll is in the same folder as this script or the exe program."
            )
        clr.AddReference(fileLib)  # pyright: ignore[reportAttributeAccessIssue]
        from LibreHardwareMonitor import Hardware  # pyright: ignore[reportMissingImports]

        self._handle = Hardware.Computer()
        self._handle.IsCpuEnabled = True
        self._handle.IsGpuEnabled = True
        if monitorAll is True:
            report("LibreHardwareMonitor", "INFO", "Enabling all hardware monitoring features.")
            self._handle.IsMemoryEnabled = True
            self._handle.IsMotherboardEnabled = True
            self._handle.IsControllerEnabled = True
            self._handle.IsNetworkEnabled = True
            self._handle.IsStorageEnabled = True
        self._handle.Open()
        self._cpu = MetricCPU()
        self._gpu = MetricGPU()

    def read(self) -> tuple[MetricCPU, MetricGPU]:
        self._cpu.load = ""
        self._cpu.temperature = ""
        self._cpu.frequency = ""
        self._gpu.load = ""
        self._gpu.d3d_load = ""
        self._gpu.temperature = ""
        self._gpu.frequency = ""
        for hardware in self._handle.Hardware:
            hardware.Update()
            for sensor in hardware.Sensors:
                if sensor.Value is not None:
                    self._cpu.parse(sensor)
                    self._gpu.parse(sensor)
                    report("LibreHardwareMonitor", "DEBUG", f"Sensor read: {str(sensor.SensorType)} - {sensor.Name} : {sensor.Value}")
            for subHardware in hardware.SubHardware:
                subHardware.Update()
                for subsensor in subHardware.Sensors:
                    if subsensor.Value is not None:
                        self._cpu.parse(subsensor)
                        self._gpu.parse(subsensor)
                        report("LibreHardwareMonitor", "DEBUG", f"Sensor read: {str(subsensor.SensorType)} - {subsensor.Name} : {subsensor.Value}")
        if self._gpu.load == "":
            self._gpu.load = self._gpu.d3d_load
        if self._gpu.temperature == "" and self._gpu.frequency == "":
            self._gpu.temperature = self._cpu.temperature
            self._gpu.frequency = self._cpu.frequency
            report("LibreHardwareMonitor", "WARNING", "GPU metrics not found. May be using integrated graphics.")
        return (self._cpu, self._gpu)

    def decode(self, last: bool = False) -> str:
        return self._cpu.decode() + self._gpu.decode(last)
