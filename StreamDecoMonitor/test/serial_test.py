#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pathlib import Path
import sys

sys.path.append(str(Path(__file__).resolve().parents[1]))

import modules.serial_sender_task as sst
import modules.report as report

def search_com_port() -> str:
    from serial.tools.list_ports import comports
    for port in comports():
        if "CH340" in port.description:
            return port.name
        if "CP210x" in port.description:
            return port.name
    report.report("Serial Test", "ERROR", "No compatible COM port found. Please check your connections.")
    return ""

if __name__ == "__main__":
    report.set_debug_level("DEBUG")
    boardCOM = search_com_port()
    report.report("Serial Test", "INFO", f"Using COM port: {boardCOM}")
    serial_sender = sst.SerialSenderTask(boardCOM, run_task=False)
    serial_sender.send("Test message from SerialSenderTask test script.")