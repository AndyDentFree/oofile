# Microsoft Developer Studio Project File - Name="OOFGraphLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOFGraphLib - Win32 oldstreams S RTL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrLib.mak" CFG="OOFGraphLib - Win32 oldstreams S RTL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFGraphLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 Debug with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 Release with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 oldstreams SD RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 oldstreams S RTL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/", aaaaaaaa"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFGraphLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFGrLib\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrLib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Release\OOFGraphLib.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFGrLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrLib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibD.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Debug with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFGraph"
# PROP BASE Intermediate_Dir "OOFGraph"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFGrLib\DebugS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrLib\DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibSD.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Release with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFGrap0"
# PROP BASE Intermediate_Dir "OOFGrap0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFGrLib\ReleaseS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrLib\ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "\oofile\source\core" /I "\OOFile\source\utils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFGraphLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFGraphLibS.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 oldstreams SD RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFGraph"
# PROP BASE Intermediate_Dir "OOFGraph"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFGraph"
# PROP Intermediate_Dir "OOFGraph"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D OOF_USE_ANSI_STREAMS=0 /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibSD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibSDO.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 oldstreams S RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFGrap0"
# PROP BASE Intermediate_Dir "OOFGrap0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFGrap0"
# PROP Intermediate_Dir "OOFGrap0"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D OOF_USE_ANSI_STREAMS=0 /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob2 /I "\oofile\source\core" /I "\OOFile\source\utils" /I "\oofile\source\graphs" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D OOF_USE_ANSI_STREAMS=0 /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFGraphLibSDO.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFGraphLibSO.lib"

!ENDIF 

# Begin Target

# Name "OOFGraphLib - Win32 Release"
# Name "OOFGraphLib - Win32 Debug"
# Name "OOFGraphLib - Win32 Debug with static RTL"
# Name "OOFGraphLib - Win32 Release with static RTL"
# Name "OOFGraphLib - Win32 oldstreams SD RTL"
# Name "OOFGraphLib - Win32 oldstreams S RTL"
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

SOURCE=..\..\source\graphs\oofpch_g.cpp

!IF  "$(CFG)" == "OOFGraphLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Debug with static RTL"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 oldstreams SD RTL"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 oldstreams S RTL"

# SUBTRACT BASE CPP /YX /Yc
# SUBTRACT CPP /YX /Yc

!ENDIF 

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
