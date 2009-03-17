// stSetCursor
// changes the cursor as specified in the constructor
// ©1995 A.D. Software, all rights reserved
// freely licensed for any use
// contact dent@highway1.com.au  or http://www.highway1.com.au/adsoftware/

// TO DO - add option of the CAnimCursor classes from the PowerPlant archive


// defaults to using QD calls to get watch and arrow cursors

#ifndef H_stSetCursor
#define H_stSetCursor

#include "oofplat.h"

#ifndef _Windows

/**
	Stack class to change cursor for a simple scope.
	Includes nesting count so won't flicker as exit methods within that scope
	which may instantiate their own stSetCursors.
	Default cursor is hourglass.
	\ingroup oofMacSpecific oofGUI
*/
class stSetCursor {
public:
	stSetCursor(CursHandle inAdoptFromCurs=0, CursHandle inToCurs=0); 
	~stSetCursor();
	
private:
	void SetInitialCursor(CursHandle=0) const;
	CursHandle	mFromCursor;	///< owned & will destroy
	CursHandle	mToCursor;		///< APPLICATION'S RESPONSIBILITY TO CLEANUP
	
	static long	sSetCursorCount;
};

#else
// NOT YET IMPLEMENTED
#endif

#endif
