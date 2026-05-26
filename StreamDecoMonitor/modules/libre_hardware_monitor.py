import os
import sys
from pathlib import Path
from .report import report


def _resolve_dll_path(file_name: str) -> str:
    """
    Resolves the path to the specified DLL file, checking multiple potential locations.
    The search order is as follows:
        1. If running from a PyInstaller bundle, check the bundle root and a 'lib' subdirectory 
           within the bundle.
        2. Check a 'lib' subdirectory next to the script.
        3. Check the same directory as the script.
        4. Check the current working directory.
    This function ensures that the LibreHardwareMonitorLib.dll can be found whether
    the app is run as a script or as a bundled executable.
    Args:
        file_name: The name of the DLL file to find.
    Returns:
        The full path to the DLL file if found, or the original file name if not found
        (which will likely lead to a runtime error when the app tries to load the DLL).
    """
    module_dir = Path(__file__).resolve().parent
    app_root = module_dir.parent
    search_roots: list[Path] = []
    bundle_root = getattr(sys, "_MEIPASS", None)
    if bundle_root:
        bundle_path = Path(bundle_root)
        search_roots.extend([bundle_path, bundle_path / "lib"])
    search_roots.extend([
        app_root / "lib",
        app_root,
        module_dir,
        Path.cwd(),
    ])

    for root in search_roots:
        candidate = root / file_name
        if candidate.is_file():
            return str(candidate)
    return file_name


class MetricBase:
    """
    Base class for hardware metrics. Each specific hardware type (CPU, GPU, etc.) 
    can extend this class to implement its own parsing logic for the sensors it cares about.
    The base class provides a common interface for decoding the metrics into a string format 
    that can be sent to the serial port.
    """

    def __init__(self) -> None:
        """
        Initializes the MetricBase instance with default values for load, temperature, and frequency.
        """
        self.load: str = ""
        self.temperature: str = ""
        self.frequency: str = ""

    def parse(self, sensor) -> None:
        """
        Parses the sensor data and updates the metric values.
        This method should be overridden by subclasses to handle specific sensor types.
        """
        pass

    def decode(self, last: bool = False) -> str:
        """
        Decodes the metric values into a string format suitable for sending to the serial port.
        Args:
            last: A boolean indicating whether this is the last metric in the sequence.
        Returns:
            A string representation of the metric values.
        """
        _data = f"{self.load}, {self.temperature}, {self.frequency}"
        if last is True:
            _data = _data + "/"
        else:
            _data = _data + ","
        return _data


class MetricCPU(MetricBase):
    """
    MetricCPU is a subclass of MetricBase that specifically handles CPU metrics.
    It overrides the parse method to update CPU-specific sensor values.
    """

    def parse(self, sensor) -> None:
        """
        Parses the sensor data and updates the CPU metric values.
        Args:
            sensor: The sensor object containing the data to be parsed.
        """
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
    """
    MetricGPU is a subclass of MetricBase that specifically handles GPU metrics.
    It overrides the parse method to update GPU-specific sensor values, 
    including a special case for D3D load.
    """

    def __init__(self) -> None:
        """
        Initializes the MetricGPU instance and adds an additional attribute for D3D load.
        """
        super().__init__()
        self.d3d_load: str = ""
    def parse(self, sensor) -> None:
        """
        Parses the sensor data and updates the GPU metric values.
        Args:
            sensor: The sensor object containing the data to be parsed.
        """
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
    """
    LibreHardwareMonitor is a class that provides an interface to the LibreHardwareMonitor library.
    It allows monitoring of various hardware metrics such as CPU and GPU usage, temperature, and frequency.
    """

    def __init__(self, monitorAll: bool = False) -> None:
        """Initializes the LibreHardwareMonitor instance and sets up the hardware monitoring.
        Args:            
            monitorAll (bool): A boolean indicating whether to enable monitoring of all 
            hardware features (CPU, GPU, Memory, Motherboard, Controller, Network, Storage).
        
        This constructor attempts to load the LibreHardwareMonitorLib.dll and initialize the hardware monitoring.
        It checks for the presence of the DLL in multiple locations to ensure compatibility with both script 
        and bundled executable environments. 
        If the DLL is not found, it logs an error and raises a FileNotFoundError.
        Once the DLL is loaded, it creates an instance of the Computer class from the 
        LibreHardwareMonitor library, enables monitoring for CPU and GPU by default, 
        and optionally enables monitoring for all hardware features if the monitorAll parameter is set to True.
        """

        import clr

        fileLib = "LibreHardwareMonitorLib.dll"
        fileLibPath = _resolve_dll_path(fileLib)
        if not os.path.isfile(fileLibPath):
            report("LibreHardwareMonitor", "ERROR",
                f"{fileLib} not found. Please, make sure the DLL is bundled with the executable or placed next to the script."
            )
            raise FileNotFoundError(fileLib)
        clr.AddReference(fileLibPath)  # pyright: ignore[reportAttributeAccessIssue]
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
        """
        Reads the current hardware metrics for CPU and GPU.
        This method updates the internal state of the CPU and GPU metric objects by iterating through all
        hardware components and their sensors, parsing the relevant sensor data for CPU and GPU metrics.
        It handles cases where GPU metrics may not be available (e.g., when using integrated graphics)
        by falling back to CPU metrics for temperature and frequency if GPU-specific metrics are not found.
        Returns:
            A tuple containing the updated MetricCPU and MetricGPU objects with the latest hardware metrics.
        """
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
        """
        Decodes the current hardware metrics into a string format suitable for sending to the serial port.
        This method calls the decode method of both the CPU and GPU metric objects to get their string
        representations and concatenates them together, separated by a comma.
        Args:
            last (bool): If True, indicates that this is the last set of metrics to decode.
        Returns:
            A string containing the decoded CPU and GPU metrics.
        """
        return self._cpu.decode() + self._gpu.decode(last)
