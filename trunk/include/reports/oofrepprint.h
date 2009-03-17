/**
	\file oofrepprint.h
	COPYRIGHT 2001 A.D. Software, All rights reserved
 
	Common file to include printing headers for GUI platforms.
*/
#ifndef H_OOFREPPRINT
#define H_OOFREPPRINT

	#include "oofplat.h"
	
	#if defined(_Macintosh)
		#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
			#include "oofMacCarbonPrint.h"
		#else
			#include "oofRepMacPrint.h"
		#endif
	#elif defined(_Windows)
		#include "oofRepMFC.h"  // GUI preview header includes print functions
	#endif
	
#endif  // H_OOFREPPRINT