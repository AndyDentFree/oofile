// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditMac.h
// report-writer layer of OOFILE database - editing classes

#ifndef H_OOFREPEDITMAC
#define H_OOFREPEDITMAC

#ifndef H_OOFREPEDIT
	#include "oofrepedit.h"
#endif

#include <LAttachment.h>

class LControl;

/**
	The idea of the mixin base is to define a common set of controls
	and their behaviour so an application can subclass with just a few
	overrides, supporting with whatever dialog class and layout they 
	fancy. This is necessary because application code may have dialogs based
	on LGADialog or other base classes.
	
	Any buttons not initialised will not be implemented so the entire 
	movement approach can be ignored if desired.
	
	Most of the behaviour is cross platform in OOF_mixRepAbstractCellEditor
	thanks to the cross-platform GUI interface.
	\ingroup oofRepEdit oofMacSpecific
*/
class OOF_mixRepCellEditor : public OOF_mixRepAbstractCellEditor {
protected:  // only used by subclasses
	OOF_mixRepCellEditor();
	
	virtual void InitMovementControls(  // can still pass NULL to non-default params
		LControl* inFirst, LControl* inLast,
		LControl* inUp, LControl* inDown, LControl* inLeft, LControl* inRight,
		LControl* inPrevItem=NULL, LControl* inNextItem=NULL
	);
	
	virtual void InitEditControls(
		dbGUIAdapter* adoptedContentAdapter, 
		LControl* inOK=NULL, LControl* inCancel=NULL
	);

// optional behavioural overrides
	virtual void UpdateMovementButtons();  // provides simple disabling
	
	
// data storage - links to user controls
	LControl*		mOKButton;
	LControl*		mCancelButton;  // may also just be Close button
	LControl*		mGoFirstButton;
	LControl*		mGoLastButton;
	LControl*		mGoUpButton;
	LControl*		mGoDownButton;
	LControl*		mGoLeftButton;
	LControl*		mGoRightCellButton;
	LControl*		mGoPrevItemButton;
	LControl*		mGoNextItemButton;
};


/**
	Forward clicks from preview window to an oofRepEditingEnv subclass to respond.
	Created by oofRepEditingEnv::startMonitoringWindowInteractions.	
	\todo see if this is appropriate class to add mouse-over cursor changes.
	\ingroup oofRepEdit
*/
class OOF_ClickForwarder : public LAttachment {
public:
	OOF_ClickForwarder(oofRepEditingEnv*);
	
	virtual void	SetOwnerHost(LAttachable *inOwnerHost);
	
protected:
	virtual void	ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam);

	oofRepEditingEnv*	mNotifies;		
};


#endif  // H_OOFREPEDITMAC

