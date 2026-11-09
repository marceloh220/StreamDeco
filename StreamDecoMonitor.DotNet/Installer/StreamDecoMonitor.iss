#define AppVersion "1.0.0"
#define AppExeName "StreamDecoMonitor.DotNet.exe"

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
Name: "autostart"; Description: "Iniciar com o Windows com privilegios administrativos"; GroupDescription: "Inicializacao:"

[Files]
Source: "..\publish\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\StreamDeco Monitor"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"
Name: "{autodesktop}\StreamDeco Monitor"; Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon

[UninstallRun]
Filename: "{sys}\schtasks.exe"; Parameters: "/Delete /F /TN ""StreamDeco Monitor"""; Flags: runhidden waituntilterminated

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  Parameters: String;
begin
  if CurStep = ssPostInstall then
  begin
    if WizardIsTaskSelected('autostart') then
      Parameters := '/Create /F /SC ONLOGON /RL HIGHEST /IT /RU "' +
        ExpandConstant('{username}') + '" /TN "StreamDeco Monitor" /TR "\"' +
        ExpandConstant('{app}\{#AppExeName}') + '\""'
    else
      Parameters := '/Delete /F /TN "StreamDeco Monitor"';
    if not Exec(ExpandConstant('{sys}\schtasks.exe'), Parameters, '', SW_HIDE,
      ewWaitUntilTerminated, ResultCode) then
      ResultCode := -1;
    if WizardIsTaskSelected('autostart') and (ResultCode <> 0) then
      MsgBox('Nao foi possivel configurar a inicializacao com o Windows. Codigo: ' +
        IntToStr(ResultCode), mbError, MB_OK);
  end;
end;