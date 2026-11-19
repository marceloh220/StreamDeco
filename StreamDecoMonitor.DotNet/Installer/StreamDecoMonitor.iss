#define AppVersion "1.0.0"
#define AppExeName "StreamDecoMonitor.DotNet.exe"
#define ServiceName "StreamDecoTemperatureService"
#define ServiceExeName "StreamDecoMonitor.TemperatureService.exe"

[Setup]
AppId={{BAE60C4E-4A9A-46B9-BA63-0B81D0C95035}
AppName=StreamDeco Monitor
AppVersion={#AppVersion}
AppPublisher=StreamDeco
DefaultDirName={autopf}\StreamDeco Monitor
DefaultGroupName=StreamDeco Monitor
UninstallDisplayIcon={app}\{#AppExeName}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
OutputDir=..\..\artifacts
OutputBaseFilename=StreamDecoMonitor-Setup
SetupIconFile=..\..\StreamDecoMonitor\assets\streamDeco.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
DisableProgramGroupPage=yes

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Tasks]
Name: "desktopicon"; Description: "Criar atalho na area de trabalho"; GroupDescription: "Atalhos:"; Flags: unchecked
Name: "autostart"; Description: "Iniciar com o Windows"; GroupDescription: "Inicializacao:"

[Files]
Source: "..\publish\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\service-publish\*"; DestDir: "{app}\service"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\StreamDeco Monitor"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"
Name: "{autodesktop}\StreamDeco Monitor"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{userstartup}\StreamDeco Monitor"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: autostart

[UninstallRun]
Filename: "{sys}\schtasks.exe"; Parameters: "/Delete /F /TN ""StreamDeco Monitor"""; Flags: runhidden waituntilterminated
Filename: "{sys}\sc.exe"; Parameters: "stop {#ServiceName}"; Flags: runhidden waituntilterminated
Filename: "{sys}\sc.exe"; Parameters: "delete {#ServiceName}"; Flags: runhidden waituntilterminated

[Run]
Filename: "{sys}\sc.exe"; Parameters: "stop {#ServiceName}"; Flags: runhidden waituntilterminated
Filename: "{sys}\sc.exe"; Parameters: "delete {#ServiceName}"; Flags: runhidden waituntilterminated
Filename: "{sys}\sc.exe"; Parameters: "create {#ServiceName} binPath= ""{app}\service\{#ServiceExeName}"" start= auto DisplayName= ""StreamDeco Temperature Service"""; Flags: runhidden waituntilterminated
Filename: "{sys}\sc.exe"; Parameters: "start {#ServiceName}"; Flags: runhidden waituntilterminated