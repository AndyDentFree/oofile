# Microsoft Developer Studio Project File - Name="ooftst02" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ooftst02 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ooftst02.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ooftst02.mak" CFG="ooftst02 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ooftst02 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ooftst02 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ooftst02 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ooftst02\ooftst02"
# PROP BASE Intermediate_Dir ".\ooftst02\ooftst02"
# PROP BASE Target_Dir ".\ooftst02"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFTst\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFTst\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\ooftst02"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "..\source\core" /I "..\source\ctree" /I "..\source\dbase" /I "..\source\rep_char" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_char" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib lib\debug\OOFileLib.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\samples\Debug/ooftst02.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Debug\OOFileLibD.lib /nologo /subsystem:console /debug /machine:I386 /out:".\samples\Debug\ooftst02.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ooftst02 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ooftst02\ooftst02"
# PROP BASE Intermediate_Dir ".\ooftst02\ooftst02"
# PROP BASE Target_Dir ".\ooftst02"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFTst\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFTst\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ".\ooftst02"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "..\source\core" /I "..\source\ctree" /I "..\source\dbase" /I "..\source\rep_char" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /O2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_char" /D "WIN32" /D "_NDEBUG" /D "_CONSOLE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib lib\debug\OOFileLib.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\samples\Debug/ooftst02.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Release\OOFileLib.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:".\samples\Release\ooftst02.exe"
# SUBTRACT LINK32 /pdb:none /debug

!ENDIF 

# Begin Target

# Name "ooftst02 - Win32 Debug"
# Name "ooftst02 - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\samples\ooftst02.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\samples\ooftst02.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
