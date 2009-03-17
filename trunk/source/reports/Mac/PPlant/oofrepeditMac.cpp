// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditMac.cpp
// report-writer layer of OOFILE database - editing classes

// default content editing window and base class

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREPEDITMAC
	#include "oofrepeditMac.h"
#endif
#ifndef H_OOFREPEDITX
	#include "oofrepeditx.h"
#endif

#ifndef _H_LPane
	#include <LPane.h>
#endif
#ifndef _H_LControl
	#include <LControl.h>
#endif

// -------------------------------------------------------
//     O O F _ C l i c k F o r w a r d e r
// -------------------------------------------------------
OOF_ClickForwarder::OOF_ClickForwarder(oofRepEditingEnv* inEnv) :
	LAttachment(msg_Click),
	mNotifies(inEnv)
{
}



void
OOF_ClickForwarder::ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam)
{
	assert(mNotifies);
	// see LPane::Click
	SMouseDownEvent* inMouseDown = (SMouseDownEvent*) ioParam;
	mNotifies->singleClick(inMouseDown->whereLocal.h, inMouseDown->whereLocal.v);
}							


void	
OOF_ClickForwarder::SetOwnerHost(LAttachable *inOwnerHost)
{
	LAttachment::SetOwnerHost(inOwnerHost);
	if (!inOwnerHost)
		delete this;  // self deleting attachment when no longer attached!
}
	

// -------------------------------------------------------
//     o o f R e p E d i t i n g E n v i r o n m e n t
// -------------------------------------------------------
/**
The default interactor will be used by most users but it is possible
that in future they will have report objects that create their own
interactors.

Here we create a listener that will pass simple clicks to the interactor
so <B>WARNING</B> we are implicitly limiting even user overridden interactors.

So, if you are using fancier interactors you will need to override 
startMonitoringWindowInteractions as well.

In your override, create an LMouseTracker (see LMouseTracker.cp) if
you want to respond to mouse movement within the pane.
You will probably also need to subclass CHandlePicture to override its
MouseWithin method. Alternatively copy LMouseTracker and have it despatch
to some other method.
*/

void
oofRepEditingEnv::startMonitoringWindowInteractions(LPane* inPane)
{
	LAttachment* selfDeletingAttachment = new OOF_ClickForwarder(this);
	inPane->AddAttachment(selfDeletingAttachment);
}



// -------------------------------------------------------
//     O O F _ m i x R e p C e l l E d i t o r
// -------------------------------------------------------

OOF_mixRepCellEditor::OOF_mixRepCellEditor() :
	mOKButton(0),
	mCancelButton(0),  // may also just be Close button
	mGoFirstButton(0),
	mGoLastButton(0),
	mGoUpButton(0),
	mGoDownButton(0),
	mGoLeftButton(0),
	mGoRightCellButton(0),
	mGoPrevItemButton(0),
	mGoNextItemButton(0)
{}



void 
OOF_mixRepCellEditor::InitMovementControls(
		LControl* inFirst, LControl* inLast,
		LControl* inUp, LControl* inDown, LControl* inLeft, LControl* inRight,
		LControl* inPrevItem, LControl* inNextItem
)
{
	mGoFirstButton = inFirst;
	mGoLastButton = inLast;
	mGoUpButton = inUp;
	mGoDownButton = inDown;
	mGoLeftButton = inLeft;
	mGoRightCellButton = inRight;
	mGoPrevItemButton = inPrevItem;
	mGoNextItemButton = inNextItem;
}
	

void 
OOF_mixRepCellEditor::InitEditControls(
		dbGUIAdapter* adoptedContentAdapter, 
		LControl* inOK, LControl* inCancel
)
{
	mOKButton - inOK;
	mCancelButton = inCancel;
	mAdapter = adoptedContentAdapter;
}

	
// optional behavioural overrides
/**
We are initialised with a cell position and maintain it ourselves
so can query the link and enable buttons to suit

If you don't want your buttons disabled, don't pass them into the 
InitMovementControls.
*/
void 
OOF_mixRepCellEditor::UpdateMovementButtons()  
{
/*
	short newCol;
	long newRow ;
	mCurrentLink->getCurrentCoords(newCol, newRow);
	
	if (mGoFirstButton) {
		if ((newCol>0) || (newRow>0))
			mGoFirstButton->Enable();
		else
			mGoFirstButton->Disable();
	}
	
	if (mGoLeftButton) {
		if (newCol>0) 
			mGoLeftButton->Enable();
		else
			mGoLeftButton->Disable();
	}
*/
	if (mGoPrevItemButton)
		mGoPrevItemButton->Disable();  // NOT YET IMPLEMENTED

	if (mGoNextItemButton)
		mGoNextItemButton->Disable();  // NOT YET IMPLEMENTED
}
