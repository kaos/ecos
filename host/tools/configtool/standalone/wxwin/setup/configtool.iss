; Inno Setup Script
; Created with ScriptMaker Version 1.3.22
; 14 February 2001 at 10:38

[Setup]
    MinVersion=4.0,4.0
    AppName=eCos Configuration Tool 2.11
    AppId=eCos Configuration Tool
    CreateUninstallRegKey=1
    UsePreviousAppDir=1
    UsePreviousGroup=1
    AppVersion=2.11
    AppVerName=eCos Configuration Tool 2.11
    AppCopyright=Copyright © Red Hat Inc., 2001-2002
    BackColor=$FF0000
    BackColor2=$000000
    BackColorDirection=toptobottom
    WindowShowCaption=1
    WindowStartMaximized=1
    WindowVisible=1
    WindowResizable=1
    UninstallLogMode=Append
    DirExistsWarning=auto
    UninstallFilesDir={app}
    DisableDirPage=0
    DisableStartupPrompt=0
    CreateAppDir=1
    DisableProgramGroupPage=0
    AlwaysCreateUninstallIcon=1
    Uninstallable=1
    DefaultDirName=c:\Program Files\Red Hat\eCos Configuration Tool
    DefaultGroupName=Red Hat eCos Configuration Tool 2.11
    LicenseFile=v:\deliver\configtool\license.txt
    InfoBeforeFile=v:\deliver\configtool\readme.txt
    MessagesFile=compiler:default.isl
    SourceDir=v:\deliver\configtool
    OutputDir=v:\deliver

[Dirs]
  Name: {app}\.
  Name: {app}\manual
  Name: {app}\manual\pix

[Files]
  Source: v:\deliver\configtool\CHANGES.txt; DestDir: {app}\
  Source: v:\deliver\configtool\configtool.bin; DestDir: {app}\
  Source: v:\deliver\configtool\configtool.exe; DestDir: {app}\
  Source: v:\deliver\configtool\license.txt; DestDir: {app}\
  Source: v:\deliver\configtool\manual\botclear.gif; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\cygnus.css; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\ecos.gif; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\pix\addfromfolder.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\addplatform.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\admin.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\arrow.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\bash.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\botclear.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\build-tools-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\build-tools2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\build3D.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\build3D2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\buildoptions.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\BuildPackages.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\ch-properties-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\ch_initialscreen.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\comprepos.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\comprepos1.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\comprepos2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\config-f1.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\configitemlabels.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\configwin.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Conflicts.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\conflictwin.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\connection.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\CorrectFailingRules2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\defineproject.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\docs.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\ecos.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\ecosprojselect.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\find-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\html-help.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image22.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image23.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image24.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image25.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image26.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image27.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image28.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image29.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image30.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image31.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image32.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image33.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image34.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image35.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image36.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image37.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image38.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image39.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image40.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image41.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image42.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image43.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image44.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image45.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image46.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image47.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image47new.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image48.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image49.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image50.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image50new.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image51.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image52.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image53.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image54.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image54new.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image55.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image56.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image57.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image58.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image59.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\Image60.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\install3D.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\install3D2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\memorywin.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\memregions.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\memreloc.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\mlt-region-general.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\mlt-section-general.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\mlt-section-relocation.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\mlt-view.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\modifyplatform.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\new-configtool-conflict-options.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\new-configtool-f1.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\new-configtool-run-options.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\open-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\options.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\packagesdialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\properties-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\properties-dialog2.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\properties-dialog3.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\propwin.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\regprops.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\repo3D.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\repo3D1.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\repos-relocate.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\resolve-conflicts.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\run-tests.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\save-as-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\targetlist.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\templates.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\ToolsOptions.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\toolsplatforms.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\user-tools-dialog.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\pix\viewmenu.gif; DestDir: {app}\manual/pix
  Source: v:\deliver\configtool\manual\user-guides.1.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.10.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.11.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.12.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.13.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.14.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.15.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.16.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.17.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.2.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.3.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.4.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.5.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.6.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.7.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.8.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.9.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.a.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.b.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.c.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.d.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.e.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.f.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\manual\user-guides.html; DestDir: {app}\manual
  Source: v:\deliver\configtool\README.txt; DestDir: {app}\
  Source: v:\deliver\configtool\TODO.txt; DestDir: {app}\

    Source: c:\winnt\system32\tcl82.dll; DestDir: {sys}\; DestName: tcl82.dll; CopyMode: onlyifdoesntexist; Flags: uninsneveruninstall
    Source: c:\winnt\system32\msvcirt.dll; DestDir: {sys}\; DestName: msvcirt.dll; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall restartreplace
    Source: c:\winnt\system32\msvcp60.dll; DestDir: {sys}\; DestName: msvcp60.dll; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall restartreplace
    Source: c:\winnt\system32\msvcrt.dll; DestDir: {sys}\; DestName: msvcrt.dll; CopyMode: alwaysskipifsameorolder; Flags: uninsneveruninstall restartreplace

[Icons]
    Name: {group}\Configuration Tool; Filename: {app}\configtool.exe; WorkingDir: {app}; IconFilename: {app}\configtool.exe; IconIndex: 0
    Name: {group}\ReadMe; Filename: {app}\README.txt; WorkingDir: {app}; IconIndex: 0; Flags: useapppaths
    Name: {group}\Changes; Filename: {app}\CHANGES.txt; WorkingDir: {app}; IconIndex: 0; Flags: useapppaths
    Name: {group}\License; Filename: {app}\license.txt; WorkingDir: {app}; IconIndex: 0; Flags: useapppaths
    Name: {group}\Configuration Tool Manual; Filename: {app}\manual\user-guides.4.html; WorkingDir: {app}; IconIndex: 0; Flags: useapppaths
    Name: {userdesktop}\eCos Config Tool; Filename: {app}\configtool.exe; WorkingDir: {app}; IconFilename: {app}\configtool.exe; IconIndex: 0

[INI]

[Registry]

[UninstallDelete]

[InstallDelete]

[Run]

[UninstallRun]


; ==============================================
; The lines below are used by ScriptMaker
; They are not required by Inno Setup
; DO NOT DELETE THEM or you may be unable to reload the script

;[ScriptSetup]
;VerNum=2.11
;InnoVer=1.3
;AddVerTo=AppVerName
;SetupFilename=setup.exe
;OutputFolder=v:\setup
;CopyrightText=Copyright © Red Hat Inc., 2001-2002

