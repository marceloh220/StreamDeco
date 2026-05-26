# -*- mode: python ; coding: utf-8 -*-

a = Analysis(
    ['streamDeco_customtkinter_preview.py'],
    pathex=[],
    binaries=[
        ('lib/LibreHardwareMonitorLib.dll', 'lib'),
        ('lib/HidSharp.dll', 'lib'),
    ],
    datas=[
        ('assets', 'assets'),
        ('modules', 'modules'),
    ],
    hiddenimports=[
        'customtkinter',
        'pystray',
        'PIL',
        'pyserial',
        'psutil',
    ],
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
