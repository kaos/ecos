# Microsoft Developer Studio Project File - Name="cdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=cdl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cdl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cdl.mak" CFG="cdl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cdl - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "cdl - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "cdl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "V:\ide\Release\cdl"
# PROP Intermediate_Dir "V:\ide\Release\cdl\build"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "cdl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cdl___Win32_Debug"
# PROP BASE Intermediate_Dir "cdl___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "V:\ide\debug\cdl"
# PROP Intermediate_Dir "V:\ide\debug\cdl\build"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "cdl - Win32 Release"
# Name "cdl - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\base.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\build.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\cdl.hxx
# End Source File
# Begin Source File

SOURCE=.\cdl.sh
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\cdlcore.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\cdlmisc.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\changelog

!IF  "$(CFG)" == "cdl - Win32 Release"

# Begin Custom Build - Performing Custom Build Step for libCDL
IntDir=V:\ide\Release\cdl\build
OutDir=V:\ide\Release\cdl
InputPath=..\..\..\..\..\ecc\cdltools\current\host\libcdl\changelog

BuildCmds= \
	cd ..\..\standalone\win32 \
	sh -x cdl.sh $(IntDir) $(OutDir) \
	

"$(OutDir)\lib\cdl.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\lib\cyginfra.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "cdl - Win32 Debug"

# Begin Custom Build - Performing Custom Build Step for libCDL
IntDir=V:\ide\debug\cdl\build
OutDir=V:\ide\debug\cdl
InputPath=..\..\..\..\..\ecc\cdltools\current\host\libcdl\changelog

BuildCmds= \
	cd ..\..\standalone\win32 \
	sh -x cdl.sh $(IntDir) $(OutDir) --enable-debug \
	

"$(OutDir)\lib\cdl.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OutDir)\lib\cyginfra.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\component.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\config.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\conflict.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\database.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\dialog.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\expr.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\hcdl.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\interface.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\interp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\option.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\package.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\parse.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\property.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\refer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\value.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\ecc\cdltools\current\host\libcdl\wizard.cxx
# End Source File
# End Target
# End Project
