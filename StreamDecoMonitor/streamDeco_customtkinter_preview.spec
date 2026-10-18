# -*- mode: python ; coding: utf-8 -*-

from PyInstaller.utils.hooks import collect_all

customtkinter_datas, customtkinter_binaries, customtkinter_hiddenimports = collect_all('customtkinter')
pythonnet_datas, pythonnet_binaries, pythonnet_hiddenimports = collect_all('pythonnet')

a = Analysis(
    ['streamDeco_customtkinter_preview.py'],
    pathex=[],
    binaries=[
        ('lib/LibreHardwareMonitorLib.dll', 'lib'),
        ('lib/HidSharp.dll', 'lib'),
    ] + customtkinter_binaries + pythonnet_binaries,
    datas=[
        ('assets', 'assets'),
        ('modules', 'modules'),
    ] + customtkinter_datas + pythonnet_datas,
    hiddenimports=[
        'customtkinter',
        'pystray',
        'PIL',
        'serial.tools.list_ports',
        'psutil',
        'GPUtil',
        'clr',
    ] + customtkinter_hiddenimports + pythonnet_hiddenimports,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='streamDeco_customtkinter_preview',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='assets/streamDeco.ico',
)
