// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofClrbl.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFCLRBL
	#include "oofClrbl.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// ---------------------------------------------------------------------------------
//		 oofColorable()
// ---------------------------------------------------------------------------------

oofColorable::oofColorable()
		: mDrawStyle(0)
{
}


// ---------------------------------------------------------------------------------
//		 oofColorable() copy ctor
// ---------------------------------------------------------------------------------

oofColorable::oofColorable(const oofColorable& rhs)
{
	if (rhs.drawStyle())
		mDrawStyle = new oofDrawStyle(rhs.drawStyle());
	else
		mDrawStyle = 0;
}


oofColorable::~oofColorable()
{
	delete mDrawStyle;
}


// ---------------------------------------------------------------------------------
//		 oofColorable(const oofDrawStyle& inPattern)
// ---------------------------------------------------------------------------------

oofColorable::oofColorable(const oofDrawStyle& inDrawStyle)
{
	mDrawStyle = new oofDrawStyle(inDrawStyle);
}

