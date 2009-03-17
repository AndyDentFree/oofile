// stSetCursor
// changes the cursor as specified in the constructor
// ©1995 A.D. Software, all rights reserved
// freely licensed for any use
// contact dent@highway1.com.au  or http://www.highway1.com.au/adsoftware/

#include "stSetCursor.h"
#include <UDrawingState.h>

// static variables
long stSetCursor::sSetCursorCount;


stSetCursor::stSetCursor(CursHandle inAdoptFromCurs, CursHandle inToCurs) : 
						mFromCursor(inAdoptFromCurs),
						mToCursor(inToCurs)
{
	SetInitialCursor(inAdoptFromCurs);
	sSetCursorCount++;	// allow nested calls
}


stSetCursor::~stSetCursor()
{
	sSetCursorCount--;
	if (sSetCursorCount==0) {	// only release if last one nested!
		if (mToCursor) 
			::SetCursor(*mToCursor);  // app's responsibility to delete!
		else
			::SetCursor(&UQDGlobals::GetQDGlobals()->arrow);
		if (mFromCursor)
			ReleaseResource((Handle)mFromCursor);
	}
}


void stSetCursor::SetInitialCursor(CursHandle inFromCurs) const
{
	if (inFromCurs==NULL) 
		inFromCurs = ::GetCursor(watchCursor);
	if (inFromCurs)
		::SetCursor(*inFromCurs);
}


