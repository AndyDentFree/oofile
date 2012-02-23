# Microsoft Developer Studio Project File - Name="OOFileGUI_MFC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OOFileGUI_MFC - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFMFC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFMFC.mak" CFG="OOFileGUI_MFC - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFileGUI_MFC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OOFileGUI_MFC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OOFMFC", ASBAAAAA"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFileGUI_MFC - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFMFC\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFC\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "OOFILE_USE_DLL" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 .\Bin\Release\OOFile.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Bin\Release\OOFMFC.dll" /implib:".\Bin\Release\OOFMFC.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFMFC\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFC\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "OOFILE_USE_DLL" /D "_AFX_NO_DAO_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 .\Bin\Debug\OOFileD.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Bin\Debug\OOFMFCD.dll" /implib:".\Bin\Debug\OOFMFCD.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "OOFileGUI_MFC - Win32 Release"
# Name "OOFileGUI_MFC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\GUI\oofgui.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\OOFileGUI_MFC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\OOFileGUI_MFC.rc
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcct.cpp
# ADD CPP /I "DLLStuff"
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfchr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfclk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcpd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\MSWin\MFC\Oofmfcyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\oofync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\MSWin\MFC\TreeCtrlEx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfc.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcct.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfchr.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcic.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcpd.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfcx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofync.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\DLLStuff\res\OOFileGUI_MFC.rc2
# End Source File
# End Group
# End Target
# End Project
