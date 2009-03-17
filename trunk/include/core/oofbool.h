#ifndef H_OOFbool
#define H_OOFbool

#ifndef H_OOFPLAT
	#include "oofplat.h"
#endif

/**
	\file oofbool.h 
	Attempts to define 'bool' regardless of compiler and whether it implements true bools.
*/

/**
	\def OOF_HAS_BOOL
	Indicates compiler implemements true c++ 'bool' data type.
	\ingroup oofMacros
*/

#ifdef _Windows
	#if _MSC_VER>=1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
		#pragma warning( disable : 4237 )  // bool, true & false
		#pragma warning( disable : 4800 )  // bool conversion to int
		#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
	#endif
#endif

//    F A K E   B O O L E A N S
// this is for all compilers lacking the new bool data type
#ifdef OOF_GCC
	#define OOF_HAS_BOOL
#else
	#ifdef __MWERKS__
		#if  __MWERKS__ > 0x0900
			// as of CW10, compiler option to use new bool type		
			#if __option(bool)
				#define OOF_HAS_BOOL
			#endif	
		#endif
	#elif defined(__BCPLUSPLUS__)
		#if __BCPLUSPLUS__>0x340
		// after 4.52
			#define OOF_HAS_BOOL
		#endif
	#endif
#endif

#if defined(_MSC_VER) && !defined(__MWERKS__) && _MSC_VER >= 1020  // check for Visual C++ 4.2 or higher
		#undef OOF_HAS_BOOL
// include file from MS that defines it their way
		#include <yvals.h>
#else
// undefine these for everyone else, as some compilers that support
// bool still have headers that #define true and false
	#undef false
	#undef true
	#ifndef OOF_HAS_BOOL
		typedef int bool;
		const bool false = 0;
		const bool true = 1;
	#endif

#endif

#endif
