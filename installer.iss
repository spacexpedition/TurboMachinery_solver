[Setup]
AppName=Turbomachinery Solver
AppVersion=1.0
AppPublisher=TurbomachinerySolver
DefaultDirName={autopf}\TurbomachinerySolver
DefaultGroupName=Turbomachinery Solver
UninstallDisplayIcon={app}\Turbomachines_GUI.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.\installer
OutputBaseFilename=TurbomachinerySolver_Setup
WizardStyle=modern
WizardImageFile=.\Frontend\assets\setup_wizard_art.bmp
DisableWelcomePage=no
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=yes
; Require Windows 10 minimum
MinVersion=10.0

[Files]
; The GUI executable and all Qt DLLs (deployed by windeployqt)
Source: ".\ReleaseBuild\bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; The self-contained Python backend executable (built by PyInstaller)
; This means the user does NOT need Python installed at all.
Source: ".\Backend\dist\backend_server.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Turbomachinery Solver"; Filename: "{app}\Turbomachines_GUI.exe"; IconFilename: "{app}\Turbomachines_GUI.exe"
Name: "{autodesktop}\Turbomachinery Solver"; Filename: "{app}\Turbomachines_GUI.exe"; IconFilename: "{app}\Turbomachines_GUI.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked
