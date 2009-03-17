// OOFILE c-tree Plus backend implementation

// COPYRIGHT 1994 A.D. Software, All rights reserved

#include "oofmswin.h"


// -------------------------------------------------------
//                   d b W i n 3 R G B
// -------------------------------------------------------
dbWin3RGB::dbWin3RGB(const OOF_IndexOptions indexOptions) :
	dbUlong(indexOptions)
{}


dbWin3RGB::dbWin3RGB(const char *fieldName)  :
	dbUlong(fieldName)
{}


dbWin3RGB::dbWin3RGB(const char *fieldName, const OOF_IndexOptions indexOptions)  :
	dbUlong(fieldName, indexOptions)
{}


dbWin3RGB& 
dbWin3RGB::operator=(unsigned long n)
{
	if (isStandalone())
		(unsigned long) mBackend = n;
	else {
		mTable->ensureRecordLoaded();
		unsigned long *writeTo = (unsigned long *)mBackend->getFieldWriteDest(mFieldNumber);
		if (writeTo)
			*writeTo = n;	
		mBackend->markDirty();
	}
	return *this;
}



