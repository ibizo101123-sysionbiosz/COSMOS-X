#define PluginDir "..\build\COSMOSX_artefacts\Release\VST3\ASTORIAS.vst3"

[Setup]
AppId={{A83B21D4-77E9-46BC-8A6A-4F03D2C0B913}
AppName=ASTORIAS
AppVersion=0.1.0
DefaultDirName={autopf}
DisableProgramGroupPage=yes
OutputDir=..\dist
OutputBaseFilename=ASTORIAS_Installer
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin

[Files]
Source: "{#PluginDir}\*"; DestDir: "{commoncf}\VST3\ASTORIAS.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion