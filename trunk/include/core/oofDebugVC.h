/**
	\file oofDebugVC.h Debugging code to use the Visual C++ Debug runtime library.
	Use by #define OOF_MEM_DEBUG_LAST_INCLUDE "oofDebugVC.h"
	\see oofSmartheap.h oofDebugNew.h
*/

#ifndef _CRTDBG_MAP_ALLOC
#error you must define _CRTDBG_MAP_ALLOC to use this
// expected that defined in oof0.h
#endif

#define OOF_MEM_DEBUG_FORCE_POOL_CHECK _CrtCheckMemory();
#define OOF_MEM_DEBUG_REPORT_LEAKS _CrtDumpMemoryLeaks();
//#define OOF_MEM_DEBUG_FORGET_LEAKS dunno what to do for this

#include <CRTDBG.H>
