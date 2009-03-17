#ifndef H_OOFIOS
#define H_OOFIOS

// COPYRIGHT 1997 A.D. Software, All rights reserved

/**
	\file oofios.h 
	Tries to hide the different platforms and version issues with standard IO.
	evolution from early ARM days to current ANSI standard, and provide a single file to include
	although different compilers may have odd sets of IO headers.
*/

#ifndef OOF_NO_STDLIB
	
	#ifndef H_OOF0
		#include "oof0.h"
	#endif
	
	// OOFILE streams stuff, hidden in here rather than in the public interface
	// so Mac users can create precompiled headers
	#if OOF_USE_ANSI_STREAMS
	
	// -------------- A N S I   S T R E A M S ---------------------
		#ifdef __MWERKS__
			#if __MWERKS__ >= 0x2400 || MSIPL_USING_NAMESPACE
		// only Metrowerks CodeWarrior has required use of new ANSI libraries
		// before implementing namespaces!
				using namespace std;
			#endif
		#else
			using namespace std;
		#endif
		#include <iosfwd>	// some of the typedefs, eg: ostream, only in here
		#include <iostream>
		#include <fstream>
		#include <sstream>
	
	// define old stream name to a safe new version that we can use
	// as it's been commonly used in application programs, eg: to pass
	// into dbConnect::raise and oofYNC
	
	// WARNING if we used ostringstream here it wouldn't work as we
	// can't read its contents
		typedef stringstream ostrstream;
	
	#else
	
	
	// -------------- O L D    S T R E A M S ---------------------
	
	// different libs have fstream including iostream & vice-versa so need both
		#include <iostream.h>
		#include <fstream.h>
		#ifdef __MWERKS__
	// allow for Metrowerks MSL libs which dropped strstream.h in contrast to Visual C++
	// which has retained it
			#include <strstream.h>
		#else
			#ifdef _MSDOS
				#include <strstrea.h>
				// yes this is really necessary - the long name fails under NT as the file is 
				// really called strstrea.h
			#else
				#include <strstream.h>
			#endif
			#ifndef OOF_GCC
				#ifndef SOLARIS
					#include <io.h>
				#endif
			#endif
			#ifdef macintosh
				#include <unix.h>
			#endif
			#include <fcntl.h>
			#ifndef _MSDOS
				#ifndef _UNISTD
					#include <unistd.h>
				#endif
			#endif
		#endif
	#endif
	// ANSI streams
	
#endif  // OOF_NO_STDLIB
#endif
	
	
