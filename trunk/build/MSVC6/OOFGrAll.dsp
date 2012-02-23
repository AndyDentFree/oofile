# Microsoft Developer Studio Project File - Name="OOFGraphLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOFGraphLib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrAll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFGrAll.mak" CFG="OOFGraphLib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFGraphLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFGraphLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# PROP Output_Dir "..\Temp\vc6\OOFGrAllLib\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrAllLib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "\faircom\ctree\include" /I "\faircom\ctree\custom" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Release\OOFGrAllLib.lib"

!ELSEIF  "$(CFG)" == "OOFGraphLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFGrAllLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFGrAllLib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "." /I "\faircom\ctree\include" /I "\faircom\ctree\custom" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFGrAllLibD.lib"

!ENDIF 

# Begin Target

# Name "OOFGraphLib - Win32 Release"
# Name "OOFGraphLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\core\oof1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofarray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofClrbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofcolor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\dbase\oofdbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofdraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofDStyl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofexcep.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooffiles.cpp
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

SOURCE=..\..\source\rep_char\oofhtml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooflist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofPlBit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofquery.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofRect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofrw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofsize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSrSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSting.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofTrMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofwords.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\source\core\oof0.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof1.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof2.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof3.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof4.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofarray.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofbool.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofClrbl.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofcolor.h
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctree.h
# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctrex.h
# End Source File
# Begin Source File

SOURCE=..\..\source\dbase\oofdbase.h
# End Source File
# Begin Source File

SOURCE=..\..\source\dbase\oofdbasx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofdraw.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofDStyl.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofexcep.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooffiles.h
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

SOURCE=..\..\source\rep_char\oofhtml.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofile.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofios.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooflist.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofmacro.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofPlBit.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofquery.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofram.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframp.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframpx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrecs.h
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofRect.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrel.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrelx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofrw.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofsize.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSrSet.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofSting.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofstr.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooftest.h
# End Source File
# Begin Source File

SOURCE=..\..\source\graphs\oofTrMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofview.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofwords.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
