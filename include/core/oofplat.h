#ifndef H_OOFPLAT
#define H_OOFPLAT

// COPYRIGHT 1994 A.D. Software, All rights reserved

// minimal root public layer of OOFILE database
// -------------- P L A T F O R M ---------------------
// NOT YET IMPLEMENTED - tests for OS2

/**
	\file oofplat.h
	Define consistent platform identifiers _Macintosh _Windows _Unix.
	Most work is to cope with the variety of definitions between compilers Visual C++ v1.52, v4
	and Borland and Symantec on Windows.
	\ingroup oofUtils
*/

#ifndef _Windows
	#if defined(_WIN32)
		#define _Win32
		#define _Windows
	#elif defined(WIN32)
		#define _Win32
		#define _Windows
	#elif defined(__WIN32__)
		#define _Win32
		#define _Windows
	#elif defined(__Win32__)
		#define _Win32
		#define _Windows
	#elif defined(_WINDOWS)
		#define _Windows
	#elif defined(__INTEL__) && defined(__MWERKS__)
	// Metrowerks CodeWarrior doesn't build anything other than Win32 on INTEL, no DOS
		#define _Windows
		#define _Win32
	#endif
#else
	#if defined __Win32__ || defined _WIN32
		#ifndef _Win32
			#define _Win32
		#endif
	#endif
#endif

#ifndef _MSDOS
	#ifdef _Windows
		#define _MSDOS
	#elif defined(MSDOS)
		#define _MSDOS
	#elif defined(__MSDOS__)
		#define _MSDOS
	#endif
#endif

#ifdef _Windows
	#ifndef STRICT
// some Windows headers define STRICT. In Visual C++ at least having it defined
// affects how static member signatures are mangled, so we define it up front 	
		#define STRICT
	#endif
#endif


// if not a DOS machine by now, may be Mac or Unix
// cope with Metrowerks and Symantec (and MPW?)
#ifndef _MSDOS
	#ifndef _Macintosh
		#ifdef macintosh
			#define _Macintosh
		#endif
	#endif
	#ifndef _Macintosh
		#define _Unix
	#endif
#endif

#ifdef _Windows
	#if _MSC_VER>=1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
		#pragma warning( disable : 4237 )  // bool, true & false
		#pragma warning( disable : 4800 )  // bool conversion to int
		#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
	#endif
	#ifdef __MWERKS__
	// avoid conflicts with the min and max macros in MFC
	// which have already been defined by the precompiled header
		#undef min
		#undef max
	#endif
#endif

#endif
