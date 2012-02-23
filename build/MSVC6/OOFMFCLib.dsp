# Microsoft Developer Studio Project File - Name="OOFileGUI_MFC_StaticLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFMFCLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFMFCLib.mak" CFG="OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Release\OOFMFCLib.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibD.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileG0"
# PROP BASE Intermediate_Dir "OOFileG0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\ReleaseS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFMFC_Static.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFMFCLibS.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileGU"
# PROP BASE Intermediate_Dir "OOFileGU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\DebugS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibSD.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileGU"
# PROP BASE Intermediate_Dir "OOFileGU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\ReleaseS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFMFCLibS.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFMFCLibS.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileG0"
# PROP BASE Intermediate_Dir "OOFileG0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\DebugS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibSD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibSD.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileGU"
# PROP BASE Intermediate_Dir "OOFileGU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\DebugSDO"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\DebugSDO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibSD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFMFCLibSDO.lib"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileG0"
# PROP BASE Intermediate_Dir "OOFileG0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFMFCLib\DebugSO"
# PROP Intermediate_Dir "..\Temp\vc6\OOFMFCLib\DebugSo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /I "." /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I "\oofile\source\gui\mswin\mfc\dllstuff" /I "\oofile\source\gui" /I "\oofile\source\gui\mswin\mfc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFMFC_STATIC" /D "_AFX_NO_DAO_SUPPORT" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFMFCLibS.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFMFCLibSO.lib"

!ENDIF 

# Begin Target

# Name "OOFileGUI_MFC_StaticLib - Win32 Release"
# Name "OOFileGUI_MFC_StaticLib - Win32 Debug"
# Name "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"
# Name "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"
# Name "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"
# Name "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"
# Name "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"
# Name "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\GUI\oofgui.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfc.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcct.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfchr.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcic.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\Oofmfclk.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\Gui\MSWin\Mfc\oofmfcpd.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# ADD CPP /YX"stdafx.h"
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# ADD BASE CPP /YX"stdafx.h"
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

# ADD BASE CPP /YX
# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\MSWin\MFC\Oofmfcyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\MSWin\MFC\oofpch_m.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\oofync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GUI\MSWin\MFC\TreeCtrlEx.cpp

!IF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static MFC"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static MFC"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream SD RTL"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFileGUI_MFC_StaticLib - Win32 oldstream S RTL"

!ENDIF 

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
# End Group
# End Target
# End Project
