/**
	\file oofDebugNew.h Debugging code to use the Metrowerks DebugNew files.
	Use by #define OOF_MEM_DEBUG_LAST_INCLUDE "oofDebugNew.h"
	\note contrary to comments in debugnew.cp, the use of DebugNew is documented
	in the Targeting MacOS PDF or HTML docs, and NOT in a separate DebugNew.doc file.
	\see oofSmartheap.h DebugNew.cp
*/

#define DEBUG_NEW 2   //enable leak detection
#include <DebugNew.h>
#define new NEW  // use DebugNew's NEW macro

#define OOF_MEM_DEBUG_FORCE_POOL_CHECK DebugNewValidateAllBlocks();
#define OOF_MEM_DEBUG_REPORT_LEAKS DebugNewReportLeaks();
#define OOF_MEM_DEBUG_FORGET_LEAKS DebugNewForgetLeaks();
