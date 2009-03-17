#ifndef H_OOFmswin
#define H_OOFmswin

// COPYRIGHT 1994 A.D. Software, All rights reserved

/**
	\file oofmswin.h 
	MS Windows compatability layer of OOFILE database.
*/

// NOTE inline definitions included at end of this header file

#include "oof4.h"

/**
	Persistent field subclass for storing a Windows colour value in a database.
	\ingroup oofUtils
*/
class dbWin3RGB : public dbUlong  {
public:
	dbWin3RGB() {};
	dbWin3RGB(const OOF_IndexOptions);
	dbWin3RGB(const char *fieldName);
	dbWin3RGB(const char *fieldName, const OOF_IndexOptions);
 	dbWin3RGB(FieldIsStandaloneT k) : dbUlong(k) {}; 
// use default copy ctor
	virtual ~dbWin3RGB() {};
	
// data access
	dbWin3RGB& operator=(unsigned long);
	dbWin3RGB& operator=(const dbWin3RGB&);  // value copy
};

// -------------------------------------------------------
//                   d b W i n 3 R G B
// -------------------------------------------------------
inline dbWin3RGB& 
dbWin3RGB::operator=(const dbWin3RGB& rhs)
{
	*this = ((unsigned long)rhs);
	return *this;
}


inline ostream& 
operator<<(ostream& os, const dbWin3RGB& fld)
{
	fld.extract(os);
	return os;
}



#endif

