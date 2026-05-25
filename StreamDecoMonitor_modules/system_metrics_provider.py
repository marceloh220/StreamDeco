from __future__ import annotations
import platform
from datetime import datetime
from queue import Empty, Full, Queue
import threading
from typing import Any
import psutil

from .libre_hardware_monitor import LibreHardwareMonitor
from StreamDecoMonitor_modules.report import report, get_debug_level


class SystemMetricsProvider:

    def __init__(self, queue_metrics: Queue[dict[str, Any]], queue_serial_sender: Queue[str]) -> None:
        self._platform = platform.system().lower()
        self._disk_root = "C:\\" if self._platform == "windows" else "/"
        monitorAll = get_debug_level() == "DEBUG"
        self.monitor = LibreHardwareMonitor(monitorAll=monitorAll)
        self.queue_serial_sender = queue_serial_sender
        self.queue_metrics = queue_metrics
        self.update_interval_seconds = 1.0
        self._stop_event = threading.Event()
        self._thread = threading.Thread(target=self._run, daemon=True)
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
                    report("SystemMetricsProvider", "INFO", "pyamdgpuinfo detected and initialized for GPU monitoring.")
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
        if self._thread.is_alive():
            return
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._stop_event.set()
        if self._thread.is_alive():
            self._thread.join(timeout=self.update_interval_seconds + 0.5)

    @staticmethod
    def _safe_float(value: Any, default: float = 0.0) -> float:
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
            report("SystemMetricsProvider", "ERROR", "Failed to read metrics from LibreHardwareMonitor. Using fallback values.")
            pass
        return cpu_load, cpu_temp, cpu_freq, gpu_load, gpu_temp, gpu_freq

    def _read_memory_metrics(self) -> tuple[float, float, float, float]:
        vm = psutil.virtual_memory()
        ram_used = float(vm.used / (1024 ** 2))
        ram_total = float(vm.total / (1024 ** 2))
        disk = psutil.disk_usage(self._disk_root)
        disk_used = float(disk.used / (1024 ** 3))
        disk_total = float(disk.total / (1024 ** 3))
        return ram_used, ram_total, disk_used, disk_total

    def _read_date_metrics(self) -> tuple[float, float, float, float, float, float, float]:
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
        self.cpu_load, self.cpu_temp, self.cpu_freq = cpu_load, cpu_temp, cpu_freq
        self.gpu_load, self.gpu_temp, self.gpu_freq = gpu_load, gpu_temp, gpu_freq
        self.ram_used, self.ram_total = ram_used, ram_total
        self.disk_used, self.disk_total = disk_used, disk_total
        (
            self.date_sec, self.date_min, self.date_hour,
            self.date_week, self.date_day, self.date_month, self.date_year,
        ) = date_values

    def _cpu_temperature_c(self) -> float:
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
        while not self._stop_event.is_set():
            metrics_payload = self.read()
            serial_payload = self.decode()
            self._queue_reader_payload(metrics_payload)
            self._queue_serial_payload(serial_payload)
            if self._stop_event.wait(self.update_interval_seconds):
                break

    def read(self) -> dict[str, float]:
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
        fields = [
            int(self.cpu_load), int(self.cpu_temp), int(self.cpu_freq),
            int(self.gpu_load), int(self.gpu_temp), int(self.gpu_freq),
            int(self.ram_used), int(self.ram_total),
            int(self.disk_used), int(self.disk_total),
            int(self.date_sec), int(self.date_min), int(self.date_hour),
            int(self.date_week), int(self.date_day), int(self.date_month), int(self.date_year),
        ]
        return ", ".join(str(field) for field in fields) + "/"
