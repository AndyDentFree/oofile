// CControlEnablingEditField
// enables and disables the attached LControl when the field is empty
// ©1995 A.D. Software, all rights reserved
// freely licensed for any use
// contact dent@highway1.com.au  or http://www.highway1.com.au/adsoftware/

#include "CControlEnablingEditField.h"
#include "CControlTimeout.h"

#include <LControl.h>

CControlEnablingEditField*	CControlEnablingEditField::CreateControlEnablingEditFieldStream(LStream *inStream)
{
	return (new CControlEnablingEditField(inStream));
}


CControlEnablingEditField::CControlEnablingEditField() : 
	mEnablingTarget(NULL), 
	mEnableWhenEmpty(false),
	mTimeoutTicks(0),
	mTimer(0)
{}


CControlEnablingEditField::CControlEnablingEditField(
								const CControlEnablingEditField	&inOriginal) :
	LEditField(inOriginal),
	mEnablingTarget(inOriginal.mEnablingTarget), mEnableWhenEmpty(inOriginal.mEnableWhenEmpty),
	mTimeoutTicks(0),
	mTimer(0)
{}
								
								
CControlEnablingEditField::CControlEnablingEditField(
								const SPaneInfo		&inPaneInfo,
								Str255				inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								Boolean				inHasBox,
								Boolean				inHasWordWrap,
								KeyFilterFunc		inKeyFilter,
								LCommander			*inSuper) :
	LEditField(inPaneInfo, inString, inTextTraitsID, inMaxChars, inHasBox, inHasWordWrap, inKeyFilter, inSuper),
	mEnablingTarget(NULL),
	mEnableWhenEmpty(false),
	mTimeoutTicks(0),
	mTimer(0)
{}
								
								
CControlEnablingEditField::CControlEnablingEditField(
								const SPaneInfo		&inPaneInfo,
								Str255				inString,
								ResIDT				inTextTraitsID,
								SInt16				inMaxChars,
								UInt8				inAttributes,
								KeyFilterFunc		inKeyFilter,
								LCommander			*inSuper) :
	LEditField(inPaneInfo, inString, inTextTraitsID, inMaxChars, inAttributes, inKeyFilter, inSuper),
	mEnablingTarget(NULL), 
	mEnableWhenEmpty(false),
	mTimeoutTicks(0),
	mTimer(0)
{}
								
								
CControlEnablingEditField::CControlEnablingEditField(
								LStream				*inStream) :
	LEditField(inStream), 
	mEnablingTarget(NULL), 
	mEnableWhenEmpty(false),
	mTimeoutTicks(0),
	mTimer(0)
{}
								

CControlEnablingEditField::~CControlEnablingEditField()
{
	delete mTimer;
}
								

void
CControlEnablingEditField::UserChangedText()
{
	if (mEnablingTarget) {
		Boolean isEmpty = ((**mTextEditH).teLength == 0);
		if (isEmpty)
			if (mEnableWhenEmpty)
				mEnablingTarget->Enable();
			else
				mEnablingTarget->Disable();
		else
			if (mEnableWhenEmpty)
				mEnablingTarget->Disable();
			else
				mEnablingTarget->Enable();
	}
	if (mTimer)
		mTimer->Enable();
	else {
		if (mTimeoutTicks) {
			mTimer = new CControlTimeout(mEnablingTarget, mTimeoutTicks);
			mTimer->Enable();
		}
	}
}


LControl*	CControlEnablingEditField::enablesControl()
{
	return mEnablingTarget;
}


void		CControlEnablingEditField::enablesControl(LControl* inCtl, unsigned long ticksTimeout)
{
	mEnablingTarget = inCtl;
	mTimeoutTicks = ticksTimeout;
}


Boolean		CControlEnablingEditField::enablesWhenEmpty()
{
	return mEnableWhenEmpty;
}


void		CControlEnablingEditField::enableWhenEmpty()
{
	mEnableWhenEmpty = true;
}


void		CControlEnablingEditField::disableWhenEmpty()
{
	mEnableWhenEmpty = false;
}

