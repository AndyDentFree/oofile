// COPYRIGHT 1995 A.D. Software, All rights reserved
// MFC Integration layer of OOFILE database
#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofync.h"
#ifndef H_OOFIOS
	#include "oofios.h"
#endif

#include <assert.h>

retTypeT
oofYNC::msgBox(bool playBeep)
{
// old setDefaultButton logic mapping 1-based buttons
// to Windows standards
	switch (mDefaultButton) {
	case 0:  // Mac standard allows no default button so we init this by default
	case 1:
		mDefaultButton = MB_DEFBUTTON1;
		break;
	case 2:
		mDefaultButton = MB_DEFBUTTON2;
		break;
	case 3:
		mDefaultButton = MB_DEFBUTTON3;
		break;
	default:
		TRACE("oofYNC::msgBox - default button must be between 1 - 3\n");
		ASSERT(0);
		break;
	}

	if (mType == MB_USERBUTTONS) {
		ASSERT(mButton1 != NULL);
	
		if (mButton2 == NULL) {
			ASSERT(mButton3 == NULL);
			mType = MB_OK;
		}
		else if (mButton3 == NULL) {
			mType = MB_YESNO;
		}
		else
			mType = MB_YESNOCANCEL;
	}

	const char* title;

	if (mTitle != NULL)
		title = mTitle;
	else if (sDefaultTitle != NULL)
		title = sDefaultTitle;
	else
		title = "";

	mDisplayed = true;
	if (playBeep){
		UINT beepType;
		switch (mIcon) {
		case MB_ICONSTOP:
			beepType= MB_ICONHAND;
			break;

		case MB_ICONQUESTION:
			beepType= MB_ICONQUESTION;
			break;

		case MB_ICONEXCLAMATION:
			beepType= MB_ICONEXCLAMATION;
			break;

		case MB_ICONINFORMATION:
			beepType= MB_ICONASTERISK;
			break;

		default :
			switch(mType) {
			case MB_OK:
				beepType= MB_OK;
				break;

			case MB_YESNO:
			case MB_YESNOCANCEL:
			case MB_OKCANCEL:
				beepType= MB_ICONQUESTION;
				break;

			case MB_USERBUTTONS:
				beepType= MB_ICONASTERISK;
				break;

			case MB_RETRYCANCEL:
			case MB_ABORTRETRYIGNORE:	
				beepType= MB_ICONHAND;
				break;
			
			default:
				beepType= MB_OK;
			}
		}
	} // has beep
	return ::MessageBox(NULL, mMsg, title, mType|mIcon|mDefaultButton|MB_TASKMODAL);
}

#endif  // !_Macintosh for doxygen
