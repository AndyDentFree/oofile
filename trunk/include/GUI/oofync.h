#ifndef OOFYNC_H
#define OOFYNC_H

#include "oofstr.h"	

#ifdef _Windows
	typedef UINT msgTypeT;
	typedef UINT retTypeT;
	typedef UINT iconTypeT;
	
#if _MSC_VER==1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
	#pragma warning( disable : 4237 )  // bool, true & false
	#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
#endif

enum { MB_USERBUTTONS = 6 };

#else
// define constants and types for other platforms
enum msgTypeT{	MB_OK				= 0,
				MB_OKCANCEL			= 1,
				MB_ABORTRETRYIGNORE	= 2,
				MB_YESNOCANCEL		= 3,
				MB_YESNO			= 4,
				MB_RETRYCANCEL		= 5,
				MB_USERBUTTONS		= 6
			};

enum retTypeT{	IDOK				= 1,
				IDCANCEL			= 2,
				IDABORT				= 3,
				IDRETRY				= 4,
				IDIGNORE			= 5,
				IDYES				= 6,
				IDNO				= 7
			};
				
enum iconTypeT{	MB_NULLICON			= 0,
				MB_NOICON			= 8,
				MB_ICONSTOP			= 16,
				MB_ICONQUESTION		= 32,
				MB_ICONEXCLAMATION	= 48,
				MB_ICONINFORMATION	= 64
			   };
#endif  // not _Windows


/**
	Cross-platform class providing MessageBox functionality.
	Up to three buttons, optional beep on message & user-settable icon.
	Message display occurs on destruction so can use ctor alone as a functor or 
	named instance to set extra syle members, calling msgBox to force
	immediate display.
*/
class OOFGUI_EXPORT oofYNC {
public:
	oofYNC();	///< default ctor allows user to set everything manually
	oofYNC(const char* msg, msgTypeT type = MB_OK);
#ifndef OOF_NO_STDLIB
	oofYNC(ostream& os, msgTypeT type = MB_OK);
#endif
	oofYNC(const char* msg, const char* b1, const char* b2=0, const char* b3=0);
	~oofYNC();
	
	virtual void setMsg(const char* msg);
	virtual void setType(msgTypeT);
	virtual void setTitle(const char*);
	static	void setDefaultTitle(const char*);
	virtual void setDefaultButton(int);
	virtual void setIcon(iconTypeT);
	
	virtual retTypeT msgBox(bool playBeep=true);
	
protected:
	oofString		mMsg;
	msgTypeT		mType;
	const char*		mTitle;
	int				mDefaultButton;
	iconTypeT		mIcon;
	const char*		mButton1;
	const char*		mButton2;
	const char*		mButton3;
	bool			mDisplayed;
	static const char* sDefaultTitle;
};


// -------------------------------------------------------
//                   o o f Y N C
// -------------------------------------------------------

inline void
oofYNC::setMsg(const char* msg)
{
	mMsg = msg;
}


inline void
oofYNC::setType(msgTypeT inType)
{
	mType = inType;
}


inline void
oofYNC::setTitle(const char* inTitle)
{
	mTitle = inTitle;
}


inline void
oofYNC::setDefaultTitle(const char* inDefaultTitle)
{
	sDefaultTitle = inDefaultTitle;
}

inline void
oofYNC::setDefaultButton(int n)
{
	mDefaultButton = n;
}


inline void
oofYNC::setIcon(iconTypeT inIcon)
{
	mIcon = inIcon;
}


#endif	// OOFYNC_H

