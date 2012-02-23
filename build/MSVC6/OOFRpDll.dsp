# Microsoft Developer Studio Project File - Name="OOFRpDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OOFRpDll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOFRpDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOFRpDll.mak" CFG="OOFRpDll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOFRpDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OOFRpDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OOFRpDll", BPAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOFRpDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\OOFRpDll"
# PROP BASE Intermediate_Dir ".\OOFRpDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp\vc6\OOFRpDll\Release"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpDll\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin\mfc" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OOFREP_CREATE_DLL" /D "OOFILE_USE_DLL" /D "OOFGRAPH_USE_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Release\OOFile.lib .\Bin\Release\OOFGraph.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Bin\Release\OOFRep.dll" /implib:".\Bin\Release\OOFRep.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OOFRpDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\OOFRpDl0"
# PROP BASE Intermediate_Dir ".\OOFRpDl0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp\vc6\OOFRpDll\Debug"
# PROP Intermediate_Dir "..\Temp\vc6\OOFRpDll\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "\oofile\source\reports\mswin" /I "\oofile\source\core" /I "\oofile\source\utils" /I "\oofile\source\graphs" /I "\oofile\source\reports" /I "\oofile\source\reports\mswin\mfc" /I "\oofile\expatpp\src_pp" /I "\oofile\expatpp\expat\xmlparse" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "OOFREP_CREATE_DLL" /D "OOFILE_USE_DLL" /D "OOFGRAPH_USE_DLL" /D "OOF_READ_REPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\Bin\Debug\OOFileD.lib .\Bin\Debug\OOFGraphD.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Bin\Debug\OOFRepD.dll" /implib:".\Bin\Debug\OOFRepD.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "OOFRpDll - Win32 Release"
# Name "OOFRpDll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\source\utils\oofadCSV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadHTM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadorn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadRTF.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadTSV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadTxt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\utils\oofadWin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofpch_r.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofriXML.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpCSV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpHTM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpRTF.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpTSV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrptxt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\MSWin\Oofrpwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpXML.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\source\utils\oofadorn.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofpch_r.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrep.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrepedit.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrepeditor.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrepeditx.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrepprint.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofreprn.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofriXML.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpCSV.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpHTM.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpRTF.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpTSV.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrptxt.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\MSWin\Oofrpwin.h
# End Source File
# Begin Source File

SOURCE=..\..\source\reports\oofrpXML.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
