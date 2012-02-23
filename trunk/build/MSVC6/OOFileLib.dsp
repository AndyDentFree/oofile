# Microsoft Developer Studio Project File - Name="OOFileLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOFileLib - Win32 Free Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFileLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFileLib.mak" CFG="OOFileLib - Win32 Free Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFileLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Demo" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Debug with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Release with static RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 oldstream SD RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 oldstream S RTL" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 DebugMem" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Personal Only Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 PersonalOnly Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Free Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OOFileLib - Win32 Free Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\OOFileLib\Release"
# PROP BASE Intermediate_Dir ".\OOFileLib\Release"
# PROP BASE Target_Dir ".\OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFileLib\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\Release"
# PROP Target_Dir ".\OOFileLib"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Release\OOFileLib.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\OOFileLib\Debug"
# PROP BASE Intermediate_Dir ".\OOFileLib\Debug"
# PROP BASE Target_Dir ".\OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFileLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\Debug"
# PROP Target_Dir ".\OOFileLib"
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileLib\Demo"
# PROP BASE Intermediate_Dir "OOFileLib\Demo"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFileLib\Demo"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\Demo"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_Demo" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 .\ctree\lib\dll\ctreestd.lib /nologo /out:".\Lib\Release\OOFileLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Demo\OOFDemo.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\Debug with static RTL"
# PROP BASE Intermediate_Dir "OOFileLib\Debug with static RTL"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFileLib\DebugS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\DebugS"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 .\ctree\lib\dll\ctreestd.lib /nologo /out:".\Bin\Debug\OOFileLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibSD.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileLib\Release with static RTL"
# PROP BASE Intermediate_Dir "OOFileLib\Release with static RTL"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFileLib\ReleaseS"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\ReleaseS"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 .\ctree\lib\dll\ctreestd.lib /nologo /out:".\Bin\Release\OOFileLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFileLibS.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\oldstream SD RTL"
# PROP BASE Intermediate_Dir "OOFileLib\oldstream SD RTL"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFileLib\DebugSO"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\DebugSO"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"oofpch_c.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D OOF_USE_ANSI_STREAMS=0 /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 .\ctree\lib\lib\ctreestdS.lib /nologo /out:".\Bin\Debug\OOFileLibSD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibSDO.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\oldstream S RTL"
# PROP BASE Intermediate_Dir "OOFileLib\oldstream S RTL"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFileLib\oldstream S RTL"
# PROP Intermediate_Dir "OOFileLib\oldstream S RTL"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D OOF_USE_ANSI_STREAMS=0 /D "OOF_Debug" /Yu"oofpch_c.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D OOF_USE_ANSI_STREAMS=0 /D "OOF_Debug" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 .\ctree\lib\lib\ctreestdS.lib /nologo /out:".\Bin\Debug\OOFileLibSDO.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFileLibSO.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\DebugMem"
# PROP BASE Intermediate_Dir "OOFileLib\DebugMem"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFileLib\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFileLib\Debug"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibDM.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\Personal Only Debug"
# PROP BASE Intermediate_Dir "OOFileLib\Personal Only Debug"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFileLib\PersonalOnlyDebug"
# PROP Intermediate_Dir "OOFileLib\PersonalOnlyDebug"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\rep_chars" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileLib\PersonalOnly Release"
# PROP BASE Intermediate_Dir "OOFileLib\PersonalOnly Release"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "OOFileLib\PersonalOnlyRelease"
# PROP Intermediate_Dir "OOFileLib\PersonalOnlyRelease"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\rep_chars" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFileLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFileLib.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OOFileLib\Free Debug"
# PROP BASE Intermediate_Dir "OOFileLib\Free Debug"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OOFileLib\FreeDebug"
# PROP Intermediate_Dir "OOFileLib\FreeDebug"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\rep_chars" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /D "OOF_DEBUG" /Yu"oofpch_c.h" /FD /D OOF_MEM_DEBUG_LAST_INCLUDE="\"oofDebugVC.h\"" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"
# ADD LIB32 /nologo /out:".\Bin\Debug\OOFileLibD.lib"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OOFileLib\Free Release"
# PROP BASE Intermediate_Dir "OOFileLib\Free Release"
# PROP BASE Target_Dir "OOFileLib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "OOFileLib\FreeRelease"
# PROP Intermediate_Dir "OOFileLib\FreeRelease"
# PROP Target_Dir "OOFileLib"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I ".." /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\rep_chars" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOF_READ_XML" /Yu"oofpch_c.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\Bin\Release\OOFileLib.lib"
# ADD LIB32 /nologo /out:".\Bin\Release\OOFileLib.lib"

!ENDIF 

# Begin Target

# Name "OOFileLib - Win32 Release"
# Name "OOFileLib - Win32 Debug"
# Name "OOFileLib - Win32 Demo"
# Name "OOFileLib - Win32 Debug with static RTL"
# Name "OOFileLib - Win32 Release with static RTL"
# Name "OOFileLib - Win32 oldstream SD RTL"
# Name "OOFileLib - Win32 oldstream S RTL"
# Name "OOFileLib - Win32 DebugMem"
# Name "OOFileLib - Win32 Personal Only Debug"
# Name "OOFileLib - Win32 PersonalOnly Release"
# Name "OOFileLib - Win32 Free Debug"
# Name "OOFileLib - Win32 Free Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\core\dbUser.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof1.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof2.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof3.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof4.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof5.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofarray.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre1.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre2.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre3.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofctre4.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\ctree\oofCTRes.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\dbase\oofdbase.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofexcep.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooffiles.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofhtml.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooflist.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofmsg.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofpch_c.cpp
# ADD CPP /Yc"oofpch_c.h"
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofquery.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofram.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframp.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec1.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec2.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec3.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec4.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrel.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofrw.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofsize.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofstr.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofview.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofwords.cpp

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofxml.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofxmldb.cpp
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

SOURCE=..\..\source\core\oofexcep.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooffiles.h
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

SOURCE=..\..\source\core\oofstr.h
# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooftest.h
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
# Begin Group "ctree libraries"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=..\..\..\oofile\msvc\vc6\ctree\lib\dll\ctreestd.lib

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\oofile\msvc\vc6\ctree\lib\lib\ctreestdS.lib

!IF  "$(CFG)" == "OOFileLib - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Debug with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Release with static RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream SD RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 oldstream S RTL"

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 DebugMem"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Personal Only Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 PersonalOnly Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OOFileLib - Win32 Free Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
