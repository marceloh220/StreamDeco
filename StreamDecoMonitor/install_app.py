#!/usr/bin/env python
"""
Installs StreamDeco Monitor on Windows: copies the executable, creates a Start Menu
shortcut and registers autostart at login.

Usage:
    python install_app.py              # build (if needed) and install
    python install_app.py --uninstall  # remove everything
"""
import os
import shutil
import subprocess
import sys

APP_NAME = "StreamDeco Monitor"
EXE_NAME = "streamDeco_customtkinter_preview.exe"
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SPEC_FILE = os.path.join(BASE_DIR, "streamDeco_customtkinter_preview.spec")
BUILT_EXE = os.path.join(BASE_DIR, "dist", EXE_NAME)
ICON_FILE = os.path.join(BASE_DIR, "assets", "streamDeco.ico")
VENV_PYTHON = os.path.join(BASE_DIR, ".venv", "Scripts", "python.exe")
PYTHON = VENV_PYTHON if os.path.isfile(VENV_PYTHON) else sys.executable
REQUIRED_PACKAGES = ["customtkinter", "pystray", "pillow", "pyserial", "psutil", "pythonnet", "GPUtil"]

INSTALL_DIR = os.path.join(os.environ.get("LOCALAPPDATA", ""), "Programs", "StreamDecoMonitor")
APPDATA = os.environ.get("APPDATA", "")
START_MENU_LNK = os.path.join(APPDATA, "Microsoft", "Windows", "Start Menu", "Programs", f"{APP_NAME}.lnk")
STARTUP_LNK = os.path.join(APPDATA, "Microsoft", "Windows", "Start Menu", "Programs", "Startup", f"{APP_NAME}.lnk")


def build_exe():
    print("Installing application dependencies...")
    subprocess.run([PYTHON, "-m", "pip", "install", *REQUIRED_PACKAGES], check=True, cwd=BASE_DIR)
    print("Building executable with PyInstaller...")
    subprocess.run(
        [PYTHON, "-m", "PyInstaller", "--clean", "--noconfirm", SPEC_FILE],
        check=True,
        cwd=BASE_DIR,
    )


def create_shortcut(lnk_path, target, icon):
    os.makedirs(os.path.dirname(lnk_path), exist_ok=True)
    ps = (
        "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('{lnk}');"
        "$s.TargetPath='{target}';$s.WorkingDirectory='{wd}';$s.IconLocation='{icon}';"
        "$s.Description='{name}';$s.Save()"
    ).format(
        lnk=lnk_path.replace("'", "''"),
        target=target.replace("'", "''"),
        wd=os.path.dirname(target).replace("'", "''"),
        icon=icon.replace("'", "''"),
        name=APP_NAME,
    )
    subprocess.run(["powershell", "-NoProfile", "-Command", ps], check=True)


def install():
    build_exe()
    os.makedirs(INSTALL_DIR, exist_ok=True)
    target = os.path.join(INSTALL_DIR, EXE_NAME)
    icon = os.path.join(INSTALL_DIR, "streamDeco.ico")
    shutil.copy2(BUILT_EXE, target)
    if os.path.exists(ICON_FILE):
        shutil.copy2(ICON_FILE, icon)
    else:
        icon = target
    create_shortcut(START_MENU_LNK, target, icon)
    create_shortcut(STARTUP_LNK, target, icon)
    print(f"Installed to {INSTALL_DIR}")
    print("Start Menu shortcut and autostart created.")


def uninstall():
    subprocess.run(["taskkill", "/F", "/IM", EXE_NAME], capture_output=True)
    for lnk in (START_MENU_LNK, STARTUP_LNK):
        if os.path.exists(lnk):
            os.remove(lnk)
    shutil.rmtree(INSTALL_DIR, ignore_errors=True)
    print("Uninstalled.")


if __name__ == "__main__":
    if not sys.platform.startswith("win"):
        sys.exit("This installer only supports Windows.")
    if "--uninstall" in sys.argv:
        uninstall()
    else:
        install()
