from __future__ import annotations
import platform
from datetime import datetime
from queue import Empty, Full, Queue
import threading
from typing import Any
import psutil

from .libre_hardware_monitor import LibreHardwareMonitor
from .report import report, get_debug_level


class SystemMetricsProvider:
    """
    Provides system metrics such as CPU load, GPU load, RAM usage, disk usage, and date/time information.
    This class runs in a separate thread to continuously read system metrics at a specified interval and
    places the metrics in a queue for consumption by other parts of the application. It also decodes the metrics
    into a string format suitable for sending to an external device via a serial connection.
    Attributes:
    - queue_metrics (Queue[dict[str, float]]): A queue for placing the latest system metrics as a dictionary.
    - queue_serial_sender (Queue[str]): A queue for placing the decoded metrics string to be sent to the serial device.
    - update_interval_seconds (float): The interval in seconds at which to read and update the system metrics.
    - _stop_event (threading.Event): An event to signal the thread to stop running.
    - _thread (threading.Thread): The thread that runs the metrics reading loop.
    - cpu_load, cpu_temp, cpu_freq: Attributes to store the latest CPU load, temperature, and frequency.
    - gpu_load, gpu_temp, gpu_freq: Attributes to store the latest GPU load, temperature, and frequency.
    - ram_used, ram_total: Attributes to store the latest RAM usage in MB and total RAM in MB.
    - disk_used, disk_total: Attributes to store the latest disk usage in GB and total disk space in GB.
    - date_sec, date_min, date_hour, date_week, date_day, date_month, date_year: 
        Attributes to store the latest date and time information.
    """

    def __init__(self, queue_metrics: Queue[dict[str, Any]], queue_serial_sender: Queue[str], 
                 update_interval_seconds: float = 1.0) -> None:
        """
        Initializes the SystemMetricsProvider with the specified queues for metrics and serial sender.
        It detects the platform and initializes the LibreHardwareMonitor for reading hardware metrics.
        For GPU monitoring on Linux, it first tries to use pyamdgpuinfo and falls back to GPUtil 
        if pyamdgpuinfo is not available or fails to initialize.
        Args:
            queue_metrics (Queue[dict[str, float]]): A queue for placing the latest system
                metrics as a dictionary.
            queue_serial_sender (Queue[str]): A queue for placing the decoded metrics string to be sent
                to the serial device.
            update_interval_seconds (float): The interval in seconds at which to read and update the system metrics.
        This constructor sets up the necessary attributes and initializes the hardware monitoring components.
        """
        self._platform = platform.system().lower()
        self._disk_root = "C:\\" if self._platform == "windows" else "/"
        monitorAll = get_debug_level() == "DEBUG"
        # Initialize the LibreHardwareMonitor to read hardware metrics.
        # The monitorAll flag can be set to True for more detailed monitoring, 
        # which may include additional sensors and components.
        self.monitor = LibreHardwareMonitor(monitorAll=monitorAll)
        self.queue_serial_sender = queue_serial_sender # A queue for placing the decoded metrics string to be sent to the serial device.
        self.queue_metrics = queue_metrics # A queue for placing the latest system metrics as a dictionary.
        self.update_interval_seconds = update_interval_seconds
        self._stop_event = threading.Event() # An event to signal the thread to stop running.
        self._thread = threading.Thread(target=self._run, daemon=True) # The thread that runs the metrics reading loop.
        self.cpu_load, self.cpu_temp, self.cpu_freq = 0.0, 0.0, 0.0
        self.gpu_load, self.gpu_temp, self.gpu_freq = 0.0, 0.0, 0.0
        self.ram_used, self.ram_total = 0.0, 0.0
        self.disk_used, self.disk_total = 0.0, 0.0
        self.date_sec, self.date_min, self.date_hour = 0.0, 0.0, 0.0
        self.date_week, self.date_day, self.date_month, self.date_year = 0.0, 0.0, 0.0, 0.0
        self._gpu_mode = "none"
        self._gpu_handle: Any = None
        if self._platform == "linux":
            try:
                import pyamdgpuinfo  # type: ignore
                if pyamdgpuinfo.detect_gpus() > 0:
                    self._gpu_mode = "pyamdgpuinfo"
                    self._gpu_handle = pyamdgpuinfo.get_gpu(0)
                    report("SystemMetricsProvider", "INFO", 
                           "pyamdgpuinfo detected and initialized for GPU monitoring.")
                    return
            except Exception:
                report("SystemMetricsProvider", "ERROR", "Failed to initialize pyamdgpuinfo for GPU monitoring.")
                pass
        try:
            import GPUtil  # type: ignore
            self._gpu_mode = "gputil"
            self._gpu_handle = GPUtil
            report("SystemMetricsProvider", "INFO", "GPUtil detected and initialized for GPU monitoring.")
        except Exception:
            report("SystemMetricsProvider", "ERROR", "Failed to initialize GPUtil for GPU monitoring.")
            self._gpu_mode = "none"

    def start(self) -> None:
        """
        Starts the background thread that continuously reads system metrics at the specified interval.
        If the thread is already running, it will not start another one.
        This method should be called to begin the metrics monitoring process after initializing the 
        SystemMetricsProvider.
        """
        if self._thread.is_alive():
            return
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        """
        Signals the background thread to stop and waits for it to finish. This method should be called
        when the application is exiting or when you want to stop monitoring the system metrics. It ensures
        that the background thread is properly terminated before the application exits.
        """
        self._stop_event.set()
        if self._thread.is_alive():
            self._thread.join(timeout=self.update_interval_seconds + 0.5)

    @staticmethod
    def _safe_float(value: Any, default: float = 0.0) -> float:
        """
        Safely converts a value to float, returning a default value if conversion fails.
        Args:
            value (Any): The value to convert to float.
            default (float): The default value to return if conversion fails.
        Returns:
            float: The converted float value or the default value if conversion fails.
        This method attempts to convert the input value to a float. 
        If the value is None, an empty string, or if any exception occurs during conversion, 
        it returns the specified default value. This ensures that the metrics reading process 
        can continue without interruption even if some values are missing or cannot be converted to float.
        """
        try:
            if value is None:
                return default
            if isinstance(value, str) and not value.strip():
                return default
            return float(value)
        except Exception:
            report("SystemMetricsProvider", "ERROR", f"Failed to convert value to float: {value}")
            return default

    def _read_cpu_gpu_metrics(self) -> tuple[float, float, float, float, float, float]:
        """
        Reads the CPU and GPU metrics, including load, temperature, and frequency.
        Returns:
            tuple[float, float, float, float, float, float]: A tuple containing the CPU load,
            CPU temperature, CPU frequency, GPU load, GPU temperature, and GPU frequency.
            This method first retrieves the CPU load, temperature, and frequency using psutil as a fallback.
        """
        cpu_load = float(psutil.cpu_percent(interval=None))
        cpu_freq_obj = psutil.cpu_freq()
        cpu_freq = float(cpu_freq_obj.current if cpu_freq_obj else 0.0)
        cpu_temp = self._cpu_temperature_c()
        gpu_load, gpu_temp, gpu_freq = self._gpu_metrics()
        try:
            monitor_info = self.monitor.read()
            cpu_data = monitor_info[0] if len(monitor_info) > 0 else None
            gpu_data = monitor_info[1] if len(monitor_info) > 1 else None
            if cpu_data is not None:
                cpu_load = self._safe_float(getattr(cpu_data, "load", None), cpu_load)
                cpu_temp = self._safe_float(getattr(cpu_data, "temperature", None), cpu_temp)
                cpu_freq = self._safe_float(getattr(cpu_data, "frequency", None), cpu_freq)
            if gpu_data is not None:
                gpu_load = self._safe_float(getattr(gpu_data, "load", None), gpu_load)
                gpu_temp = self._safe_float(getattr(gpu_data, "temperature", None), gpu_temp)
                gpu_freq = self._safe_float(getattr(gpu_data, "frequency", None), gpu_freq)
        except Exception:
            report("SystemMetricsProvider", "ERROR", 
                    "Failed to read metrics from LibreHardwareMonitor. Using fallback values.")
            pass
        return cpu_load, cpu_temp, cpu_freq, gpu_load, gpu_temp, gpu_freq

    def _read_memory_metrics(self) -> tuple[float, float, float, float]:
        """
        Reads the memory and disk usage metrics.
        Returns:
            tuple[float, float, float, float]: A tuple containing the RAM used, RAM total,
            disk used, and disk total in megabytes and gigabytes respectively.
        Note: This method retrieves the RAM usage and total RAM using psutil's virtual_memory function,
              and retrieves the disk usage and total disk space using psutil's disk_usage function for the
              specified disk root. The RAM values are converted to megabytes (MB) and the disk values are
              converted to gigabytes (GB) for easier readability and consistency with common units used in 
              system monitoring.
        """
        vm = psutil.virtual_memory()
        ram_used = float(vm.used / (1024 ** 2))
        ram_total = float(vm.total / (1024 ** 2))
        disk = psutil.disk_usage(self._disk_root)
        disk_used = float(disk.used / (1024 ** 3))
        disk_total = float(disk.total / (1024 ** 3))
        return ram_used, ram_total, disk_used, disk_total

    def _read_date_metrics(self) -> tuple[float, float, float, float, float, float, float]:
        """
        Reads the current date and time metrics.
        Returns:
            tuple[float, float, float, float, float, float, float]: A tuple containing the current second,
            minute, hour, weekday, day, month, and year as floats.
        """
        now = datetime.now()
        week = now.weekday()
        return (
            float(now.second),
            float(now.minute),
            float(now.hour),
            float(0 if week == 6 else week + 1),
            float(now.day),
            float(now.month),
            float(now.year),
        )

    def _sync_attributes(
        self,
        cpu_load: float,  cpu_temp: float, cpu_freq: float,
        gpu_load: float,  gpu_temp: float, gpu_freq: float,
        ram_used: float,  ram_total: float,
        disk_used: float, disk_total: float,
        date_values: tuple[float, float, float, float, float, float, float],
    ) -> None:
        """
        Synchronizes the instance attributes with the latest metrics values.
        Args:
            cpu_load (float): The latest CPU load percentage.
            cpu_temp (float): The latest CPU temperature in Celsius.
            cpu_freq (float): The latest CPU frequency in MHz.
            gpu_load (float): The latest GPU load percentage.
            gpu_temp (float): The latest GPU temperature in Celsius.
            gpu_freq (float): The latest GPU frequency in MHz.
            ram_used (float): The latest RAM usage in megabytes.
            ram_total (float): The total RAM in megabytes.
            disk_used (float): The latest disk usage in gigabytes.
            disk_total (float): The total disk space in gigabytes.
            date_values (tuple[float, float, float, float, float, float, float]):
                A tuple containing the current second, minute, hour, weekday, day, month, and year as floats.
        This method updates the instance attributes with the latest metrics values. 
        It is called after reading the metrics to ensure that the attributes reflect the most recent data. 
        This allows other parts of the application to access the latest metrics through the instance attributes 
        if needed, in addition to consuming them directly from the method return values.
        """
        self.cpu_load, self.cpu_temp, self.cpu_freq = cpu_load, cpu_temp, cpu_freq
        self.gpu_load, self.gpu_temp, self.gpu_freq = gpu_load, gpu_temp, gpu_freq
        self.ram_used, self.ram_total = ram_used, ram_total
        self.disk_used, self.disk_total = disk_used, disk_total
        (
            self.date_sec, self.date_min, self.date_hour,
            self.date_week, self.date_day, self.date_month, self.date_year,
        ) = date_values

    def _cpu_temperature_c(self) -> float:
        """
        Reads the CPU temperature in Celsius using psutil's sensors_temperatures function.
        Returns:
            float: The current CPU temperature in Celsius. If the temperature cannot be read, returns 0.0.
        Note: This method attempts to read the CPU temperature using psutil's sensors_temperatures
              function. It checks for common temperature sensor keys such as "coretemp", "k10temp", "cpu_thermal",
              and "acpitz". If it finds any of these keys with valid temperature entries, it returns the current
              temperature. If no valid temperature data is found or if an error occurs while reading the temperatures,
              it returns 0.0 as a fallback value.
        """
        sensors_temperatures = getattr(psutil, "sensors_temperatures", None)
        if sensors_temperatures is None:
            return 0.0
        try:
            temps = sensors_temperatures(fahrenheit=False)
        except Exception:
            report("SystemMetricsProvider", "ERROR", "Failed to read CPU temperatures using psutil.sensors_temperatures().")
            return 0.0
        if not temps:
            return 0.0
        preferred_keys = ("coretemp", "k10temp", "cpu_thermal", "acpitz")
        for key in preferred_keys:
            if key in temps and temps[key]:
                current = temps[key][0].current
                return float(current or 0.0)
        for entries in temps.values():
            if entries:
                current = entries[0].current
                return float(current or 0.0)
        return 0.0

    def _gpu_metrics(self) -> tuple[float, float, float]:
        """
        Reads the GPU metrics (load, temperature, frequency) using either pyamdgpuinfo or 
        GPUtil based on the initialized mode.
        Returns:
            tuple[float, float, float]: A tuple containing the GPU load, temperature, and frequency.
                 If the metrics cannot be read, returns (0.0, 0.0, 0.0).
         Note: This method checks the initialized GPU monitoring mode and attempts to read the GPU metrics accordingly.
               If pyamdgpuinfo is being used, it calls the appropriate methods to get the load, temperature, 
               and frequency. If GPUtil is being used, it retrieves the list of GPUs and reads the metrics from 
               the first GPU in the list. If any errors occur during the reading of GPU metrics, 
               it logs an error message and returns (0.0, 0.0, 0.0) as fallback values.
        """
        if self._gpu_mode == "pyamdgpuinfo" and self._gpu_handle is not None:
            try:
                load = float(self._gpu_handle.query_load())
                temp = float(self._gpu_handle.query_temperature())
                freq = float(self._gpu_handle.query_sclk() / 1000 / 1000)
                return load, temp, freq
            except Exception:
                report("SystemMetricsProvider", "ERROR", "Failed to read GPU metrics using pyamdgpuinfo.")
                return 0.0, 0.0, 0.0
        if self._gpu_mode == "gputil" and self._gpu_handle is not None:
            try:
                gpus = self._gpu_handle.getGPUs()
                if gpus:
                    gpu = gpus[0]
                    load = float(gpu.load * 100.0)
                    temp = float(getattr(gpu, "temperature", 0.0) or 0.0)
                    freq = float(getattr(gpu, "memoryClock", 0.0) or 0.0)
                    return load, temp, freq
            except Exception:
                report("SystemMetricsProvider", "ERROR", "Failed to read GPU metrics using GPUtil.")
                return 0.0, 0.0, 0.0
        return 0.0, 0.0, 0.0
    
    def _queue_reader_payload(self, payload: dict[str, float]) -> None:
        """
        Attempts to enqueue the latest metrics payload into the queue_metrics. If the queue is full, it will drop
        the oldest payload to make room for the new one. If it fails to enqueue after dropping the oldest payload, it logs an error message.
        Args:
            payload (dict[str, float]): The latest system metrics payload to be enqueued.
        This method tries to place the latest metrics payload into the queue_metrics using put_nowait.
        If the queue is full, it catches the Full exception and attempts to remove the oldest payload from the queue
        using get_nowait. If the queue is unexpectedly empty at this point, it logs an
        error message. After attempting to drop the oldest payload, it tries again to enqueue the new payload.
        If it still fails to enqueue due to the queue being full, it logs another error message
        indicating that the new payload could not be enqueued even after dropping the oldest one.
        """
        try:
            self.queue_metrics.put_nowait(payload)
        except Full:
            try:
                report("SystemMetricsProvider", "WARNING", "Queue metrics is full, dropping oldest payload to enqueue new one.")
                _ = self.queue_metrics.get_nowait()
            except Empty:
                report("SystemMetricsProvider", "ERROR", "Failed to dequeue metrics oldest payload, queue is unexpectedly empty.")
                pass
            try:
                self.queue_metrics.put_nowait(payload)
            except Full:
                report("SystemMetricsProvider", "ERROR", "Failed to enqueue metrics payload after dropping oldest one, queue is still full.")
                pass

    def _queue_serial_payload(self, payload: str) -> None:
        """
        Attempts to enqueue the decoded metrics payload into the queue_serial_sender.
        If the queue is full, it will drop the oldest payload to make room for the new one.
        If it fails to enqueue after dropping the oldest payload, it logs an error message.
        Args:
            payload (str): The decoded metrics string to be enqueued for sending to the serial device.
        This method tries to place the decoded metrics payload into the queue_serial_sender using put_nowait.
        If the queue is full, it catches the Full exception and attempts to remove the oldest payload
        from the queue using get_nowait. If the queue is unexpectedly empty at this point, it logs an
        error message. After attempting to drop the oldest payload, it tries again to enqueue the new payload.
        If it still fails to enqueue due to the queue being full, it logs another error message
        indicating that the new payload could not be enqueued even after dropping the oldest one.
        """
        try:
            self.queue_serial_sender.put_nowait(payload)
        except Full:
            try:
                report("SystemMetricsProvider", "WARNING", "Queue serial sender is full, dropping oldest payload to enqueue new one.")
                _ = self.queue_serial_sender.get_nowait()
            except Empty:
                report("SystemMetricsProvider", "ERROR", "Failed to dequeue serial sender oldest payload, queue is unexpectedly empty.")
                pass
            try:
                self.queue_serial_sender.put_nowait(payload)
            except Full:
                report("SystemMetricsProvider", "ERROR", "Failed to enqueue serial sender payload after dropping oldest one, queue is still full.")
                pass

    def _run(self) -> None:
        """
        The main loop that continuously reads system metrics and enqueues the payloads until the stop event is set.
        This method is intended to be run in a separate thread. It continuously reads the system metrics
        using the read method, decodes the metrics into a string format using the decode method, and then enqueues
        both the raw metrics payload and the decoded string payload into their respective queues for consumption by other
        parts of the application. The loop runs until the _stop_event is set, at which point it will exit and the thread will terminate.
        """
        while not self._stop_event.is_set():
            metrics_payload = self.read()
            serial_payload = self.decode()
            self._queue_reader_payload(metrics_payload)
            self._queue_serial_payload(serial_payload)
            if self._stop_event.wait(self.update_interval_seconds):
                break

    def read(self) -> dict[str, float]:
        """
        Reads the current system metrics, including CPU, GPU, RAM, and disk usage, as well as the current date and time.
        This method collects the metrics by calling the respective private methods for each type of metric and then
        synchronizes the collected values with the instance attributes.
        Returns:
            dict[str, float]: A dictionary containing the current system metrics.
        """
        cpu_load, cpu_temp, cpu_freq, gpu_load, gpu_temp, gpu_freq = self._read_cpu_gpu_metrics()
        ram_used, ram_total, disk_used, disk_total = self._read_memory_metrics()
        date_values = self._read_date_metrics()
        self._sync_attributes(
            cpu_load, cpu_temp, cpu_freq,
            gpu_load, gpu_temp, gpu_freq,
            ram_used, ram_total,
            disk_used, disk_total,
            date_values,
        )
        return {
            "cpu_load": self.cpu_load, "cpu_temp": self.cpu_temp, "cpu_freq": self.cpu_freq,
            "gpu_load": self.gpu_load, "gpu_temp": self.gpu_temp, "gpu_freq": self.gpu_freq,
            "ram_used": self.ram_used, "ram_total": self.ram_total,
            "disk_used": self.disk_used, "disk_total": self.disk_total,
        }

    def decode(self) -> str:
        """
        Decodes the current system metrics into a string format suitable for sending to an external device.
        The format of the decoded string is a comma-separated list of the metrics values followed by a slash ("/").
        Returns:
            str: The decoded metrics string.
        """
        fields = [
            int(self.cpu_load), int(self.cpu_temp), int(self.cpu_freq),
            int(self.gpu_load), int(self.gpu_temp), int(self.gpu_freq),
            int(self.ram_used), int(self.ram_total),
            int(self.disk_used), int(self.disk_total),
            int(self.date_sec), int(self.date_min), int(self.date_hour),
            int(self.date_week), int(self.date_day), int(self.date_month), int(self.date_year),
        ]
        return ", ".join(str(field) for field in fields) + "/"
