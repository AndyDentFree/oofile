#ifndef H_OOF0
#define H_OOF0

// COPYRIGHT 1994-1999 A.D. Software, All rights reserved

// minimal root public layer of OOFILE database

/**
	\file oof0.h
	Declare macros used by most of OOFILE.
*/

#ifndef H_OOFPLAT
	#include "oofplat.h"
#endif

// for debug mode, use VC debug RTL
// see Debugging Applications - chap 15
#if defined _MSC_VER && !defined __MWERKS__
	#ifdef OOF_DEBUG
		#define _CRTDBG_MAP_ALLOC	
		#ifndef	_DEBUG
			#define _DEBUG
		#endif
	#endif
#endif


/**
	Version number of core OOFILE package including database.
	\ingroup oofMacros
*/
#define		OOFVERS_CORE	0x01400000	// Version 1.4.0
/* index to version defines 
Note that you may lack some files depending on your package

file:        VERSION IDENTIFIER
====         ==================
oof0.h	    OOFVERS_CORE core version
oofrep.h     OOFVERS_REP report writer
oofGrphs.h   OOFVERS_GRAPH graphing engine
oofgui.h     OOFVERS_GUI  generic GUI classes used by PP, MFC and other framework integrations
oofpp.h      OOFVERS_PP  PowerPlant framework (Metrowerks CodeWarrior, on Mac)
oofmfc.h     OOFVERS_MFC MFC GUI framework

*/


// -------------- D L L   E X P O R T   M A C R O S ---------------------

/**
	\defgroup oofExportMacros DLL Export Macros
	OOF..._EXPORT macros are used to export/import classes or functions to/from DLL <br>
	if DLL is created, the OOF..._CREATE_DLL must be set on <br>
	if DLL is used, the OOF..._USE_DLL must be set on <br>
	if DLL is not used (so static library OOF...LIB.LIB is linked or created) both macros <br>
	    (OOF..._CREATE_DLL and OOF..._USE_DLL) must be set off; OOF..._EXPORT will
	    be expanded to an empty string <br>
	on non-Windows platforms, the OOF..._EXPORT macros will be expanded to an empty string <br>
	\par
	'OOF...' prefix stands for OOFILE, OOFGRAPH or OOFREP
	\todo check if need to define these for MacOS/X or other Unix dynamic linking 
	\ingroup oofMacros oofWinSpecific
*/


/**
	\def OOFILE_CREATE_DLL
	OOFILE database & general classes will be linked in a DLL.
	Leave undefined to assume static linking.
	\ingroup oofExportMacros
*/


/**
	\def OOFREP_CREATE_DLL
	Report-writer classes will be linked in a DLL.
	Leave undefined to assume static linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFGRAPH_CREATE_DLL
	Graph classes will be linked in a DLL.
	Leave undefined to assume static linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFMFC_STATIC
	MFC classes will NOT be linked in a DLL.
	Leave undefined to assume DLL linking (common MFC default).
	\todo define a OOFMFC_CREATE_DLL to follow same convention as others.
	\ingroup oofExportMacros
*/

/**
	\def OOFILE_EXPORT
	Prefix for OOFILE database & general classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFILE_EXPORT
	Prefix for OOFILE database & general classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFREP_EXPORT
	Prefix for report writer classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFGRAPH_EXPORT
	Prefix for graph classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/

/**
	\def OOFGUI_EXPORT
	Prefix for MFC GUI classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/

/**
	\def EXPATPP_EXPORT
	Prefix for expatpp classes to enable optional DLL linking.
	\ingroup oofExportMacros
*/



#ifdef _Windows
	#if defined OOFILE_CREATE_DLL && (defined OOFGRAPH_CREATE_DLL || defined OOFREP_CREATE_DLL) ||\
		defined OOFGRAPH_CREATE_DLL && (defined OOFILE_CREATE_DLL || defined OOFREP_CREATE_DLL) ||\
		defined OOFREP_CREATE_DLL && (defined OOFILE_CREATE_DLL || defined OOFGRAPH_CREATE_DLL)
		#error Only one macro OOF..._CREATE_DLL can be defined
	#endif

	#if defined OOFILE_CREATE_DLL
		#define OOFILE_EXPORT _declspec(dllexport)
	#elif defined OOFILE_USE_DLL
		#define OOFILE_EXPORT _declspec(dllimport)
	#else
		#define OOFILE_EXPORT
	#endif

	#if defined OOFGRAPH_CREATE_DLL
		#define OOFGRAPH_EXPORT _declspec(dllexport)
	#elif defined OOFGRAPH_USE_DLL
		#define OOFGRAPH_EXPORT _declspec(dllimport)
	#else
		#define OOFGRAPH_EXPORT
	#endif

	#if defined OOFREP_CREATE_DLL
		#define OOFREP_EXPORT _declspec(dllexport)
	#elif defined OOFREP_USE_DLL
		#define OOFREP_EXPORT _declspec(dllimport)
	#else
		#define OOFREP_EXPORT
	#endif

	#ifndef EXPATPP_EXPORT
		#if defined OOFILE_CREATE_DLL || defined OOFGRAPH_CREATE_DLL || defined OOFREP_CREATE_DLL
			#define EXPATPP_EXPORT	  _declspec(dllexport)
		#elif defined OOFILE_USE_DLL || defined OOFGRAPH_USE_DLL || defined OOFREP_USE_DLL 
			#define EXPATPP_EXPORT _declspec(dllimport)
		#else
			#define EXPATPP_EXPORT
		#endif
	#endif

// OOFMFC_STATIC allows us to remove the OOFGUI_EXPORT export declaration
// when static OOFMFC library is created or used
	#ifdef OOFMFC_STATIC
		#define OOFGUI_EXPORT
	#else
		#define OOFGUI_EXPORT AFX_EXT_CLASS
	#endif

#else
	#define OOFILE_EXPORT
	#define OOFGRAPH_EXPORT
	#define OOFREP_EXPORT
	#define OOFGUI_EXPORT
#endif  // _Windows



// -------------- B Y T E   F L I P P I N G ---------------------

/**
	\def OOF_INTEL_BYTE_ORDER
	define a constant we can use to determine if byte flipping needed.
	auto-defines if your makefile hasn't done so already, where possible.
	\warning doesn't detect Linux or Solaris/x86 so MUST define in makefile.
	\ingroup oofMacros
*/
#ifndef OOF_INTEL_BYTE_ORDER
	#if defined(_MSDOS)
// implying x86
		#define OOF_INTEL_BYTE_ORDER 1
	#else
		#define OOF_INTEL_BYTE_ORDER 0
	#endif
#endif

// SOLARIS and OOF_GCC are constants defined in Makefiles
// as these compilers seem to lack platform-specific constants

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_Windows) && !defined(_WINDOWS_) && !defined(_AFXDLL)
#  include <windows.h>
// disable warning that occurs when bool, true & false are used. GDH
// NOTE these need including after Windows and AFX includes
// as somewhere MS enable all warnings again
	#if _MSC_VER>=1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
		#pragma warning( disable : 4237 )  // bool, true & false
		#pragma warning( disable : 4800 )  // bool conversion to int
		#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
	#endif
#endif

#ifdef _AFXDLL	
	#include <afxwin.h>         // MFC core and standard components
#endif // _AFXDLL
	
#ifdef OOF_DEBUG_LOG_NAME
	#define OOF_DEBUG_LOG(x) dbConnect::logComment(x)
#else
	#define OOF_DEBUG_LOG(x)
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif


// -------------- E X C E P T I O N S ---------------------
/**
	\def OOF_EXCEPTIONS_AVAILABLE
	Indicates compiler supports exceptions.
	Can define in makefile or leave it to OOFILE to work out for Win and Mac compilers.
	\ingroup oofMacros
*/

/**
	\def OOF_SUPPRESS_EXCEPTIONS
	Option to prevent using exceptions in OOFILE regardless of compiler or its settings.
	\ingroup oofMacros
*/

/**
	\def OOF_USING_EXCEPTIONS
	Indicates OOFILE is using exceptions. 
	\ingroup oofMacros
*/
// test if exceptions are available
#ifdef __MWERKS__
	#if __option(exceptions)
		#define OOF_EXCEPTIONS_AVAILABLE
	#endif
#elif defined(_MSC_VER) || defined(__BCPLUSPLUS__)
		#ifdef _CPPUNWIND
			#define OOF_EXCEPTIONS_AVAILABLE
		#endif
#endif

// if user hasn't suppressed exceptions and they are worked out as available
// in the tests above
#ifndef OOF_SUPPRESS_EXCEPTIONS
	#ifdef OOF_EXCEPTIONS_AVAILABLE
		#define OOF_USING_EXCEPTIONS
	#endif
#endif


// -------------- N O S T D L I B ---------------------
/**
	\def OOF_NO_STDLIB
	Define to suppress use of standard library strings and streams.
	Use if parts of OOFILE being used in embedded environments such as QuickTime components. 
	\ingroup oofMacros
*/

#ifdef OOF_NO_STDLIB
	#undef OOF_USE_ANSI_STREAMS
	#define OOF_USE_ANSI_STREAMS 0
	#undef OOF_HAS_ANSI_STRINGS
	#define OOF_HAS_ANSI_STRINGS 0
#endif  // OOF_NO_STDLIB


// -------------- A N S I   S T R E A M S ---------------------

/**
	\def OOF_USE_ANSI_STREAMS
	
	\ingroup oofMacros
*/

/**
	\def OOF_USE_ANSI_STREAMS
	Will compile using older or standard iostreams.
	\ingroup oofMacros
*/

#if !defined OOF_USE_ANSI_STREAMS && !defined OOF_NO_STDLIB
	#if defined(__MWERKS__)
	// Metrowerks on Mac Pro 1  or any Windows, given bug in Windows Pro 1 plugin returns 0x1101
		#if defined(_Windows) || __MWERKS__ >= 0x1800
			#define OOF_USE_ANSI_STREAMS 1
		#else
			#define OOF_USE_ANSI_STREAMS 0
		#endif
	#elif defined(_MSC_VER)
	// check for Visual C++ 4.2 or higher
		#if _MSC_VER >= 1020
			#define OOF_USE_ANSI_STREAMS 1
		#else
			#define OOF_USE_ANSI_STREAMS 0
		#endif
	#elif defined(__BCPLUSPLUS__)
	// check for Borland C++ Builder 4.0 or higher 
      // Added by David Merryweather 10.08.1999
		#if __BCPLUSPLUS__ >= 0x0540
			#define OOF_USE_ANSI_STREAMS 1
		#else
			#define OOF_USE_ANSI_STREAMS 0
		#endif
	#else
	// assume Unix compilers will define in the Makefile
	// if the particular version supports streams, otherwise default
	// to them not being supported
		#define OOF_USE_ANSI_STREAMS 0
	#endif
#endif


// -------------- A N S I   S T R I N G S ---------------------

/**
	\def OOF_HAS_ANSI_STRINGS
	Controls if oofString and others include interfaces for std::string.
	\ingroup oofMacros
*/
#ifndef OOF_HAS_ANSI_STRINGS
	#if OOF_USE_ANSI_STREAMS
		#define OOF_HAS_ANSI_STRINGS 1
// we need them to cope with the str() function alone!		
	#else
		#define OOF_HAS_ANSI_STRINGS 0
	#endif
#else
	#if OOF_USE_ANSI_STREAMS && !OOF_HAS_ANSI_STRINGS
		#pragma error "ANSI Streams require use of ANSI Strings, please don't define OOF_HAS_ANSI_STRINGS as 0"
	#endif
#endif
#if !OOF_USE_ANSI_STREAMS
// define new stream names so can use throughout OOFILE
// this is safer than using old names, as some compilers or includes
// may define typedefs for istringstream etc.
	class istrstream;
	class ostrstream;
	typedef istrstream istringstream;
	typedef ostrstream ostringstream;
	typedef ostrstream stringstream;
#endif

/**
	\def MacLiteral
	Portable constand used to hide use of Pascal strings in some interfaces.
	\ingroup oofMacros
*/
#ifdef _Macintosh
	#define MacLiteral(x) "\p"x
// use string concatenation to make into a Pascal literal with \p prefix
#else
	#define MacLiteral(x) x
#endif



// -------------- X M L   M A C R O S ---------------------
/**
	\def OOF_READ_XML
	Define to build the core database with ability to parse XML schema and import database from XML.
	\ingroup oofMacros
*/

/**
	\def OOF_READ_REPORTS()
	Define to build the report writer with ability to parse XML layouts.
	If defined forces definition of OOF_READ_XML
	\ingroup oofMacros
*/
#ifdef OOF_READ_REPORTS
// report-writer uses XML to read in report documents
	#ifndef OOF_READ_XML
		#define OOF_READ_XML
		// so define the macro enabling XML database parsing in oofXML.cpp
	#endif
#endif



// -------------- R T T I   M A C R O S ---------------------
/**
	\def OOF_RTTI_ENABLED
	Indicates compiler is building with RTTI.
	\ingroup oofMacros
*/
#if defined(__MWERKS__)
	#if __option(RTTI)
		#define OOF_RTTI_ENABLED
	#endif
#elif defined(_MSC_VER)
	#ifdef _CPPRTTI
		#define OOF_RTTI_ENABLED
	#endif
#endif

#endif

