// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditWin.h
// report-writer layer of OOFILE database - editing classes

#ifndef H_OOFREPEDITWIN
#define H_OOFREPEDITWIN

#ifndef H_OOFREPEDIT
	#include "oofrepedit.h"
#endif

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

/**
	Define common controls for editor.
	The idea of the mixin base is to define a common set of controls
	and their behaviour so an application can subclass with just a few
	overrides, supporting with whatever dialog class and layout they 
	fancy. This is necessary because application code may have dialogs based
	on LGADialog or other base classes.
	
	Any buttons not initialised will not be implemented so the entire 
	movement approach can be ignored if desired.
	
	Most of the behaviour is cross platform in OOF_mixRepAbstractCellEditor
	thanks to the cross-platform GUI interface.
	\ingroup oofRepEdit oofWinSpecific
*/
class OOF_mixRepCellEditor : public OOF_mixRepAbstractCellEditor {
protected:  // only used by subclasses
	OOF_mixRepCellEditor();
	virtual void FinishCreationByLinkingDataModel(oofRepEditingEnv*, oofRepEditLink*);  // final after ctor and FinishCreateSelf
	
	virtual void InitMovementControls(  // can still pass NULL to non-default params
		CButton* inFirst, CButton* inLast,
		CButton* inUp, CButton* inDown, CButton* inLeft, CButton* inRight,
		CButton* inPrevItem=NULL, CButton* inNextItem=NULL
	);
	
	virtual void InitEditControls(
		dbGUIAdapter* adoptedContentAdapter, 
		CButton* inOK=NULL, CButton* inCancel=NULL
	);
	
// optional behavioural overrides
	virtual void UpdateMovementButtons();  // provides simple disabling
	
	
// data storage - links to user controls
	CButton*		mOKButton;
	CButton*		mCancelButton;  // may also just be Close button
	CButton*		mGoFirstButton;
	CButton*		mGoLastButton;
	CButton*		mGoUpButton;
	CButton*		mGoDownButton;
	CButton*		mGoLeftButton;
	CButton*		mGoRightCellButton;
	CButton*		mGoPrevItemButton;
	CButton*		mGoNextItemButton;
};

#endif  // !_Macintosh for doxygen
#endif  // H_OOFREPEDITMAC

