#!/usr/bin/env python

"""
This script provides a preview of the StreamDeco Monitor using CustomTkinter. It displays system metrics such as CPU usage, GPU usage, RAM usage, and more in a customizable window. The application also supports serial communication with an external board to send the collected metrics.
Requirements:
    pip install customtkinter
    pip install pystray pillow
In Windows, you may also need to install the appropriate USB-to-serial drivers for your board
(e.g., CH340 or CP210x) to enable serial communication.
For Windows users, you can create an executable using PyInstaller with the following command:
    pyinstaller streamDeco_customtkinter_preview.spec
The executable bundles the app files and LibreHardwareMonitor DLL automatically.
Run:
    python install_streamDecoMonitor.py
"""
import os
from modules.report import report


def create_exe():
    import subprocess
    import sys

    if sys.platform.startswith("win"):
        spec_file = "streamDeco_customtkinter_preview.spec"
        if not os.path.exists(spec_file):
            report("install_streamDecoMonitor", "ERROR", f"Spec file '{spec_file}' not found. Please ensure it is in the current directory.")
            return
        command = ["pyinstaller", spec_file]
        try:
            subprocess.run(command, check=True)
            report("install_streamDecoMonitor", "INFO", "Executable created successfully in the 'dist' directory.")
        except subprocess.CalledProcessError as e:
            report("install_streamDecoMonitor", "ERROR", f"Error creating executable: {e}")
    else:
        report("install_streamDecoMonitor", "ERROR", "Executable creation is only supported on Windows.")


if __name__ == "__main__":
    create_exe()