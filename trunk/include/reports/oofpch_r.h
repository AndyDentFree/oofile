// OOFILE Precompiled Header files
// oofpch_r.h - Report files
// Copyright 1998 AD Software

// For Visual C++, we need to have a single header included at the top
// of every file, that's precompiled, and then set the USE Precompiled Header
// setting in the C/C++ settings panel, Precompiled Header options

#ifndef H_OOFPCH_R
#define H_OOFPCH_R

#include "oofpch_g.h"  // graphs, gives us core as well
#ifdef _Macintosh 
	#ifndef OOF_REP_NO_GUI
		#include "oofrepMac.h"
	#endif
#elif defined _Windows
	#include "oofrpWin.h"
#endif

#endif
