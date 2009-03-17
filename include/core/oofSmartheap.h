/**
	\file oofSmartheap.h Debugging code to use the MicroQuill SmartHeap files.
	Use by #define OOF_MEM_DEBUG_LAST_INCLUDE "oofSmartheap.h"
	\todo define OOF_MEM_DEBUG_REPORT_LEAKS & OOF_MEM_DEBUG_FORGET_LEAKS for SmartHeap.
	\see oofDebugNew.h
*/

#ifdef MEM_DEBUG
	#include "smrtheap.hpp"
#endif

// now do some defines needed for checking to occur later

#define OOF_MEM_DEBUG_FORCE_POOL_CHECK assert(MemPoolCheck(MemDefaultPool));
\\ need SH equivalents
\\#define OOF_MEM_DEBUG_REPORT_LEAKS DebugNewReportLeaks();
\\#define OOF_MEM_DEBUG_FORGET_LEAKS DebugNewForgetLeaks();
