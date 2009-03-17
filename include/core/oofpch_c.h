// OOFILE Precompiled Header files
// oofpch_c.h - Core files
// Copyright 1998 AD Software

/**
	\file oofpch_c.h 
	precompilation header.
	For Visual C++, we need to have a single header included at the top
	of every file, that's precompiled, and then set the USE Precompiled Header
	setting in the C/C++ settings panel, Precompiled Header options
*/

#ifndef H_OOFPCH_C
#define H_OOFPCH_C

#include "oofplat.h"

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

#include "oof0.h"  // reacts to OOF_NO_STDLIB so need regardless
	#ifndef OOF_NO_STDLIB
		#include "oof4.h"
		#include "oofexcep.h"
		#include "oofquery.h"
		#include "oofrel.h"
		#include "ooffiles.h"
	#endif

#endif
