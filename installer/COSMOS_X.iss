#define PluginDir "..\\build\\COSMOSX_artefacts\\Release\\VST3\\COSMOS X.vst3"
[Setup]
AppId={{B7C5A0F3-9A2C-4D7C-9D0A-CC0A50D5A311}
AppName=COSMOS X
AppVersion=0.1.0
DefaultDirName={autopf}
DisableProgramGroupPage=yes
OutputDir=..\\dist
OutputBaseFilename=COSMOS_X_Installer
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
[Files]
Source: "{#PluginDir}\\*"; DestDir: "{commoncf}\\VST3\\COSMOS X.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion
[Code]
function InitializeSetup(): Boolean;
begin
 Result := DirExists(ExpandConstant('{#PluginDir}'));
 if not Result then MsgBox('The COSMOS X VST3 files were not found. The build may have failed.', mbError, MB_OK);
end;