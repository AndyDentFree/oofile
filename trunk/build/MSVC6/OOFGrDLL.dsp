# Microsoft Developer Studio Project File - Name="OOFGraphDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OOFGraphDLL - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrDLL.mak" CFG="OOFGraphDLL - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFGraphDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OOFGraphDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OOFGrDLL", TLAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFGraphDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\OOFGraph"
# PROP BASE Intermediate_Dir ".\OOFGraph"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFGrDll\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrDll\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOFGRAPH_CREATE_DLL" /D "OOFILE_USE_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Release\OOFile.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Bin\Release\OOFGraph.dll" /implib:".\Bin\Release\OOFGraph.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OOFGraphDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\OOFGrap0"
# PROP BASE Intermediate_Dir ".\OOFGrap0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFGrDll\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrDll\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOFGRAPH_CREATE_DLL" /D "OOFILE_USE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Debug\OOFileD.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Bin\Debug\OOFGraphD.dll" /implib:".\Bin\Debug\OOFGraphD.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "OOFGraphDLL - Win32 Release"
# Name "OOFGraphDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\graphs\oofClrbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofcolor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofdraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofDStyl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGeos.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGrDta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGrphs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofPlBit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofRect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSrSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSting.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofTrMgr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\source\graphs\oofClrbl.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofcolor.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofdraw.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofDStyl.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGeos.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGrDta.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGrphs.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofGrWin.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofPlBit.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofRect.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSrSet.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSting.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofTrMgr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
