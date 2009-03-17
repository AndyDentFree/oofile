// Copyright 1998 A.D. Software
// Part of the OOFILE MFC integration library. 

// CWndRedir.h

#ifndef _H_CWndRedir
#define _H_CWndRedir

class CWnd;

/**
	Redirection for cross-platform work.
	Provides methods to support transparent substitution of a shadow class in
	calls that take CWnd*, and shadow objects to look like pointers.
	\par
	This is not really needed for classical
	MFC work but is required for our PowerPlant->MFC migration kit and has had to be 
	included in the MFC integration to avoid compiling libraries with different settings
	depending on the presence of the PP2MFC layer or not.	
	\ingroup oofGUI
*/
class	CWndRedir
{
public:
	CWndRedir(CWnd* inWnd=0) : mWnd(inWnd) {};
	virtual ~CWndRedir() {};

	operator CWnd*()	{ return mWnd; };
	CWnd*				Wnd() const  { return mWnd; };	
	
protected:
	CWnd*	mWnd;
};

#endif

