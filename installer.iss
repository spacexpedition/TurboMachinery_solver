[Setup]
AppName=Turbomachinery Solver
AppVersion=1.0
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

; This specifies we are grabbing files from the release folder
[Files]
Source: ".\ReleaseBuild\bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Turbomachinery Solver"; Filename: "{app}\Turbomachines_GUI.exe"; IconFilename: "{app}\Turbomachines_GUI.exe"
Name: "{autodesktop}\Turbomachinery Solver"; Filename: "{app}\Turbomachines_GUI.exe"; IconFilename: "{app}\Turbomachines_GUI.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked
