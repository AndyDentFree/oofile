// CControlEnablingEditField
// ©1995 A.D. Software, all rights reserved
// freely licensed for any use
// contact dent@highway1.com.au  or http://www.highway1.com.au/adsoftware/

#pragma once

#include <LEditField.h>

class LControl;
class CControlTimeout;

/**
	enables and disables the attached LControl when the field is empty.
     INSTRUCTIONS
	in FinishCreateSelf()
1)
	create the control with a cast like:
		mFindField = (CControlEnablingEditField *)FindPaneByID('Edit');

	instead of the normal
		mFindField = (LEditField *)FindPaneByID('Edit');

2)
	tell the field which control it enables, eg:
		mFindField->enablesControl(mFindButton);

3)	
	if you want to reverse the sense of when the control should be enabled/disabled
		mFindField->disableWhenEmpty()

*/
#pragma once

#include <LEditField.h>

class LControl;
class CControlTimeout;

class CControlEnablingEditField : public LEditField {
public:
	enum { class_ID = 'ened' };
	static CControlEnablingEditField*	CreateControlEnablingEditFieldStream(LStream *inStream);

						CControlEnablingEditField();
						CControlEnablingEditField(
								const CControlEnablingEditField	&inOriginal);
						CControlEnablingEditField(
								const SPaneInfo		&inPaneInfo,
								Str255				inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								Boolean				inHasBox,
								Boolean				inHasWordWrap,
								KeyFilterFunc		inKeyFilter,
								LCommander			*inSuper);
						CControlEnablingEditField(
								const SPaneInfo		&inPaneInfo,
								Str255				inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								UInt8				inAttributes,
								KeyFilterFunc		inKeyFilter,
								LCommander			*inSuper);
						CControlEnablingEditField(
								LStream				*inStream);
	virtual				~CControlEnablingEditField();
	
	
	virtual void		UserChangedText();  // the main reason for our existence!
	
	LControl*	enablesControl();
	void		enablesControl(LControl*, unsigned long ticksTimeout=0);
	
	Boolean		enablesWhenEmpty();
	void		enableWhenEmpty();
	void		disableWhenEmpty();  // default case
				
private:
	LControl*	mEnablingTarget;
	Boolean		mEnableWhenEmpty;
	unsigned long mTimeoutTicks;
	CControlTimeout*	mTimer;	// owned
};
