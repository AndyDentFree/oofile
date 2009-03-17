// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditWin.cpp
// report-writer layer of OOFILE database - editing classes

// default content editing window and base class

#include "stdafx.h" 

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#ifndef H_OOFREPEDITWIN
	#include "oofrepeditWin.h"
#endif
#ifndef H_OOFREPEDITX
	#include "oofrepeditx.h"
#endif

#include "oofrepedit.cpp"  // include cpp file as it lacks stdafx.h at top
// and because of MFC links can't be built into report library

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
		CButton* inFirst, CButton* inLast,
		CButton* inUp, CButton* inDown, CButton* inLeft, CButton* inRight,
		CButton* inPrevItem, CButton* inNextItem
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
	

/**
	override OOF_mixRepAbstractCellEditor to avoid EnterLink	
	because we won't have our dialog fields initialised and so
	not have set our adapter this early
*/
void 
OOF_mixRepCellEditor::FinishCreationByLinkingDataModel(oofRepEditingEnv* inEnv, oofRepEditLink* inLink)  // final after ctor and FinishCreateSelf
{
	assert(!mEnv);  // this is a construction step, not resetting model!
	mEnv = inEnv;  // may utterly ignore if can't move to other items
	mCurrentLink = inLink;
}


void 
OOF_mixRepCellEditor::InitEditControls(
		dbGUIAdapter* adoptedContentAdapter, 
		CButton* inOK, CButton* inCancel
)
{
	mOKButton = inOK;
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
		mGoPrevItemButton->EnableWindow(FALSE);  // NOT YET IMPLEMENTED

	if (mGoNextItemButton)
		mGoNextItemButton->EnableWindow(FALSE);  // NOT YET IMPLEMENTED
}

#endif  // !_Macintosh for doxygen
