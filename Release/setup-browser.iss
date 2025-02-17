;汉化：MonKeyDu 
;由 Inno Setup 脚本向导 生成的脚本,有关创建 INNO SETUP 脚本文件的详细信息，请参阅文档！!

#define MyAppName "红警浏览器"
#define MyAppVersion "1.0.0-1.3.3"
#define MyAppExeName "启动浏览器.exe"

[Setup]
;注意：AppId 的值唯一标识此应用程序。请勿在安装程序中对其他应用程序使用相同的 AppId 值。
;（若要生成新的 GUID，请单击“工具”|”在 IDE 中生成 GUID）。
AppId={{9D985F12-F821-46B8-8D59-F33AFD85E930}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
; "ArchitecturesAllowed=x64compatible" 指定安装程序无法运行
;在 Arm 上的 x64 和 Windows 11 以外的任何东西上.
ArchitecturesAllowed=x64compatible
; "ArchitecturesInstallIn64BitMode=x64compatible" 请求
;在 x64 或 Arm 上的 Windows 11 上以“ 64 位模式”进行安装，,
;这意味着它应该使用本机 64 位 Program Files 目录和
;注册表的 64 位视图.
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes
; 取消下列注释行，在非管理员安装模式下运行(仅为当前用户安装.)
;PrivilegesRequired=lowest
OutputDir=C:\Users\huhu\Desktop
OutputBaseFilename="{#MyAppName}v{#MyAppVersion}安装程序"
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "chs"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "res\*"; DestDir: "{app}\res"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "自定义图片\使用说明.txt"; DestDir: "{app}\自定义图片"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "*.dll"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "config.json"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "QtWebEngineProcess.exe"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "启动.exe"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "使用说明.txt"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
; 注意:  在任何共享系统文件上不要使用 "Flags: ignoreversion"

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

