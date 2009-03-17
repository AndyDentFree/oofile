// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFCLRBL
#define H_OOFCLRBL
// =================================================================================
//	oofClrbl.h
// =================================================================================

#include "oofDStyl.h"

/**
	Base for anything that can be drawn in color.
	Used primarily in graphs.
*/
class OOFGRAPH_EXPORT oofColorable {
public:
		oofColorable();
		oofColorable(const oofColorable&);
		oofColorable(const oofDrawStyle& inPattern);
		~oofColorable();
	
	void setDrawStyle(	const oofColor& fg = oofColor::Black,
					const oofPattern& pat = oofPattern::gray,
					const oofPattern& mono = oofPattern::gray,
					const oofColor& bg = oofColor::White);
	void setDrawStyle( const oofDrawStyle&);
	const oofDrawStyle*	drawStyle() const;
	
protected:
	oofDrawStyle* mDrawStyle;  // OPTIONAL!  owned
};

// include inline definitions

// ---------------------------------------------------------------------------------
//		 setDrawStyle
// ---------------------------------------------------------------------------------

inline void
oofColorable::setDrawStyle(	const oofColor& fg, const oofPattern& pat,
						const oofPattern& mono, const oofColor& bg)
{
	delete mDrawStyle;
	mDrawStyle = new oofDrawStyle(fg,pat,mono,bg);
}

inline void
oofColorable::setDrawStyle(const oofDrawStyle& inDrawStyle)
{
	delete mDrawStyle;
	mDrawStyle = new oofDrawStyle(inDrawStyle);
}


// ---------------------------------------------------------------------------------
//		 drawStyle
// ---------------------------------------------------------------------------------

inline const oofDrawStyle*
oofColorable::drawStyle() const
{
	return mDrawStyle;
}
#endif
