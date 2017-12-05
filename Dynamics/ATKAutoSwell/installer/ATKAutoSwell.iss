[Setup]
AppName=ATKAutoSwell
AppVersion=2.0.0


DefaultDirName={pf}\ATKAutoSwell
DefaultGroupName=ATKAutoSwell
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=ATKAutoSwell Installer
LicenseFile=license.rtf
SetupLogging=yes

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst2_32"; Description: "32-bit VST2 Plugin (.dll)"; Types: full custom;
Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)"; Types: full custom; Check: Is64BitInstallMode;
Name: "vst3_32"; Description: "32-bit VST3 Plugin (.vst3)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
Name: "aax_32"; Description: "32-bit AAX Plugin (.aaxplugin)"; Types: full custom;
Name: "aax_64"; Description: "64-bit AAX Plugin (.aaxplugin)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Files]
Source: "..\Builds\VisualStudio2017\Win32\Release\VST\ATKAutoSwell.dll"; DestDir: {code:GetVST2Dir_32}; Components:vst2_32; Flags: ignoreversion;
Source: "..\Builds\VisualStudio2017_x64\x64\Release\VST\ATKAutoSwell_x64.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\Builds\VisualStudio2017\Win32\Release\VST3\ATKAutoSwell.vst3"; DestDir: "{cf}\VST3\"; Check: not Is64BitInstallMode; Components:vst3_32; Flags: ignoreversion;
Source: "..\Builds\VisualStudio2017\Win32\Release\VST3\ATKAutoSwell.vst3"; DestDir: "{cf32}\VST3\"; Check: Is64BitInstallMode; Components:vst3_32; Flags: ignoreversion;
Source: "..\Builds\VisualStudio2017_x64\x64\Release\VST3\ATKAutoSwell_x64.vst3"; DestDir: "{cf64}\VST3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion;

Source: "..\Builds\VisualStudio2017\Win32\Release\aax\ATKAutoSwell.aaxplugin\*.*"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\ATKAutoSwell.aaxplugin\"; Components:aax_32; Flags: ignoreversion recursesubdirs;
Source: "..\Builds\VisualStudio2017_x64\x64\Release\aax\ATKAutoSwell_x64.aaxplugin\*.*"; DestDir: "{cf}\Avid\Audio\Plug-Ins\ATKAutoSwell_x64.aaxplugin\"; Components:aax_64; Flags: ignoreversion recursesubdirs;

Source: "..\manual\ATKAutoSwell_manual.pdf"; DestDir: "{app}"
Source: "changelog.txt"; DestDir: "{app}"
Source: "readmewin.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
Name: "{group}\ATKAutoSwell"; Filename: "{app}\ATKAutoSwell.exe"
Name: "{group}\User guide"; Filename: "{app}\ATKAutoSwell_manual.pdf"
Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
Name: "{group}\Uninstall ATKAutoSwell"; Filename: "{app}\unins000.exe"

[Code]
var
  OkToCopyLog : Boolean;
  VST2DirPage_32: TInputDirWizardPage;
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

    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\WOW6432NODE\VST,VSTPluginsPath|{pf32}\Steinberg\VSTPlugins}\');
  end else begin
    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\');
  end;
end;

function GetVST2Dir_32(Param: String): String;
begin
  Result := VST2DirPage_32.Values[0]
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
