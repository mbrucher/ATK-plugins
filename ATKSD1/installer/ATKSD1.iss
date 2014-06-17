[Setup]
AppName=ATKSD1
AppVersion=1.0.0
DefaultDirName={pf}\ATKSD1
DefaultGroupName=ATKSD1
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=ATKSD1 Installer
LicenseFile=license.rtf
SetupLogging=yes

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Files]
Source: "..\build-win\vst2\x64\bin\ATKSD1.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\manual\ATKSD1_manual.pdf"; DestDir: "{app}"
Source: "changelog.txt"; DestDir: "{app}"
Source: "readmewin.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
Name: "{group}\ATKSD1"; Filename: "{app}\ATKSD1.exe"
Name: "{group}\User guide"; Filename: "{app}\ATKSD1_manual.pdf"
Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
;Name: "{group}\readme"; Filename: "{app}\readme.rtf"
Name: "{group}\Uninstall ATKSD1"; Filename: "{app}\unins000.exe"

;[Dirs] 
;Name: {cf}\Digidesign\DAE\Plugins\

[Code]
var
  OkToCopyLog : Boolean;
  VST2DirPage_64: TInputDirWizardPage;

procedure InitializeWizard;
begin
  if IsWin64 then begin
    VST2DirPage_64 := CreateInputDirPage(wpSelectDir,
    'Confirm 64-Bit VST2 Plugin Directory', '',
    'Select the folder in which setup should install the 64-bit VST2 Plugin, then click Next.',
    False, '');
    VST2DirPage_64.Add('');
    VST2DirPage_64.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\');
  end;
end;

function GetVST2Dir_64(Param: String): String;
begin
  Result := VST2DirPage_64.Values[0]
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    OkToCopyLog := True;
end;

procedure DeinitializeSetup();
begin
  if OkToCopyLog then
    FileCopy (ExpandConstant ('{log}'), ExpandConstant ('{app}\InstallationLogFile.log'), FALSE);
  RestartReplace (ExpandConstant ('{log}'), '');
end;

[UninstallDelete]
Type: files; Name: "{app}\InstallationLogFile.log"