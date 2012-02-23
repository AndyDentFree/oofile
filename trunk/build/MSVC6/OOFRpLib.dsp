# Microsoft Developer Studio Project File - Name="OOFReportLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOFReportLib - Win32 oldstreams S RTL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFRpLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFRpLib.mak" CFG="OOFReportLib - Win32 oldstreams S RTL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFReportLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFReportLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFReportLib - Win32 Debug with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFReportLib - Win32 Release with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFReportLib - Win32 oldstreams SD RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFReportLib - Win32 oldstreams S RTL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFRpLib\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpLib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Release\OOFReportLib.lib"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFRpLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpLib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFReportLibD.lib"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFRepor"
# PROP BASE Intermediate_Dir "OOFRepor"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFRpLib\DebugS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpLib\DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFReportLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFReportLibSD.lib"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFRepo0"
# PROP BASE Intermediate_Dir "OOFRepo0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFRpLib\ReleaseS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpLib\ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFReportLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFReportLibS.lib"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFRepor"
# PROP BASE Intermediate_Dir "OOFRepor"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFRepor"
# PROP Intermediate_Dir "OOFRepor"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /Yu"oofpch_r.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin\mfc" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFReportLibSD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFReportLibSDO.lib"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFRepo0"
# PROP BASE Intermediate_Dir "OOFRepo0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFRepo0"
# PROP Intermediate_Dir "OOFRepo0"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin\mfc" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_REPORTS" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin\mfc" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOF_READ_REPORTS" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFReportLibSDO.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFReportLibSO.lib"

!ENDIF 

# Begin Target

# Name "OOFReportLib - Win32 Release"
# Name "OOFReportLib - Win32 Debug"
# Name "OOFReportLib - Win32 Debug with static RTL"
# Name "OOFReportLib - Win32 Release with static RTL"
# Name "OOFReportLib - Win32 oldstreams SD RTL"
# Name "OOFReportLib - Win32 oldstreams S RTL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\utils\oofadCSV.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadHTM.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadorn.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadRTF.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadTSV.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadTxt.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadWin.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadXML.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofpch_r.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

# ADD CPP /Yc"oofpch_r.h"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# ADD BASE CPP /Yc"oofpch_r.h"
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep1.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep2.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep3.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofriXML.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpCSV.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpHTM.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpRTF.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpTSV.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrptxt.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\MSWin\Oofrpwin.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# SUBTRACT CPP /Fr /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpXML.cpp

!IF  "$(CFG)" == "OOFReportLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams SD RTL"

# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ELSEIF  "$(CFG)" == "OOFReportLib - Win32 oldstreams S RTL"

# ADD BASE CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"
# ADD CPP /I "\oofile\source\gui" /D "OOFMFC_STATIC"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\source\utils\oofadorn.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofreprn.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpHTM.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpRTF.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrptxt.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\MSWin\Oofrpwin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
