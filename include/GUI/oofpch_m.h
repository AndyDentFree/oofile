// OOFILE Precompiled Header files
// oofpch_m.h - MFC GUI files
// Copyright 1998 AD Software

// For Visual C++, we need to have a single header included at the top
// of every file, that's precompiled, and then set the USE Precompiled Header
// setting in the C/C++ settings panel, Precompiled Header options

#ifndef H_OOFPCH_M
#define H_OOFPCH_M

#include "oofplat.h"
#ifdef _Windows
	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
	
	#include <afxwin.h>         // MFC core and standard components
	#include <afxext.h>         // MFC extensions
	
	#ifndef _AFX_NO_OLE_SUPPORT
	#include <afxole.h>         // MFC OLE classes
	#include <afxodlgs.h>       // MFC OLE dialog classes
	#include <afxdisp.h>        // MFC OLE automation classes
	#endif // _AFX_NO_OLE_SUPPORT
	
	
	#ifndef _AFX_NO_DB_SUPPORT
	#include <afxdb.h>			// MFC ODBC database classes
	#endif // _AFX_NO_DB_SUPPORT
	
	#ifndef _AFX_NO_DAO_SUPPORT
	#include <afxdao.h>			// MFC DAO database classes
	#endif // _AFX_NO_DAO_SUPPORT
	
	#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include <afxcmn.h>			// MFC support for Windows Common Controls
	#endif // _AFX_NO_AFXCMN_SUPPORT
	
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
	
	#include "oofpch_c.h"  // core OOFILE definitions
	#include "oofmfcx.h"
	#include "oofmfcct.h"
	#include "oofmfchr.h"
	#include "oofmfcpd.h"
	#include "oofmfcic.h"

#endif // _Windows
#endif


