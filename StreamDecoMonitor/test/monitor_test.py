#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pathlib import Path
import sys

sys.path.append(str(Path(__file__).resolve().parents[1]))

import modules.libre_hardware_monitor as lhm
import modules.report as report

if __name__ == "__main__":
    report.set_debug_level("DEBUG")
    monitor = lhm.LibreHardwareMonitor(monitorAll=True)
    cpu, gpu = monitor.read()
    print(f"CPU Load: {cpu.load}\nCPU Temp: {cpu.temperature}\nCPU Freq: {cpu.frequency}")
    print(f"GPU Load: {gpu.load}\nGPU Temp: {gpu.temperature}\nGPU Freq: {gpu.frequency}")
    print(monitor.decode(last=True))