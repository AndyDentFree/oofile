# Microsoft Developer Studio Project File - Name="OODll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OODll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFDll.mak" CFG="OODll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OODll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OODll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OOFDll", MBAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OODll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OODll\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OODll\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFILE_CREATE_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\OODll\Release\OODll.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\ctree\lib\dll\ctreestd.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Bin\Release\OOFile.dll" /implib:".\Bin\Release\OOFile.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OODll\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OODll\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\faircom\ctree\include" /I "\oofile\source\core" /I "\oofile\source\ctree" /I "\oofile\source\dbase" /I "\oofile\source\rep_chars" /I "\oofile\source\utils" /I ".\ctree\include\dll" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OOFILE_CREATE_DLL" /D "OOF_READ_XML" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\ctree\lib\dll\ctreestd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Bin\Debug\OOFileD.dll" /implib:".\Bin\Debug\OOFileD.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "OODll - Win32 Release"
# Name "OODll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\core\platform\MSWin\DllMain.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof1.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof2.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof3.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof4.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oof5.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofarray.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\dbase\oofdbase.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofexcep.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooffiles.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofhtml.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooflist.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofmsg.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofquery.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofram.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\ooframp.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec1.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec2.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec3.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrec4.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofrel.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\rep_char\oofrw.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofsize.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofstr.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofview.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofwords.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\source\core\oofxml.cpp

!IF  "$(CFG)" == "OODll - Win32 Release"

# ADD CPP /D "OOF_READ_XML"

!ELSEIF  "$(CFG)" == "OODll - Win32 Debug"

!ENDIF 

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
# End Target
# End Project
