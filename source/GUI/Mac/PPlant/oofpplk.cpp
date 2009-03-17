// COPYRIGHT 1997 A.D. Software, All rights reserved

// PowerPlant Integration layer of OOFILE database
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFPowerPlantX
	#include "oofppx.h"  // private interface, includes public
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef OOFYNC_H
	#include "oofync.h"
#endif

// include PowerPlant definitions
#ifndef _H_LCaption
	#include <LCaption.h>
#endif
#ifndef _H_LListBox
	#include <LListBox.h>
#endif
#ifdef OOF_PP17_TEXTEDIT
	#ifndef _H_LEditField
		#include <LEditField.h>
	#endif
	#ifndef _H_LTextEdit
		#include <LTextEdit.h>
	#endif
#endif
#ifdef OOF_PP18_TEXTEDIT
	#ifndef _H_LEditText
		#include <LEditText.h>
	#endif
	#ifndef _H_LTextEditView
		#include <LTextEditView.h>
	#endif
#endif
#ifndef _H_UMemoryMgr
	#include <UMemoryMgr.h>
#endif
#ifndef _H_PP_Messages
	#include <PP_Messages.h>
#endif
#ifndef _H_PP_KeyCodes
	#include <PP_KeyCodes.h>
#endif
#ifndef _H_LPane
	#include <LPane.h>
#endif
#ifndef _H_LDragAndDrop
	#include <LDragAndDrop.h>
#endif
#ifndef _H_LStdControl
	#include <LStdControl.h>
#endif
#ifndef _H_LPopupButton
	#include <LPopupButton.h>
#endif
#ifndef _H_LMenuController
	#include <LMenuController.h>
#endif

// -------------------------------------------------------
//      d b G U I A d a p t L C o n t r o l
// -------------------------------------------------------

dbGUIAdaptLControl::dbGUIAdaptLControl(LControl*  inControl) :
	LAttachment(msg_Click),
	mControl(inControl)
{
}


void 
dbGUIAdaptLControl::setFieldLink(dbFieldLink* inLink)
{
	const bool firstSettingOfLink = (mFieldLink==NULL);
	dbGUIAdapter::setFieldLink(inLink);
	if (firstSettingOfLink) {
		mControl->AddAttachment(this, nil, false /* not owned by control*/);
		inLink->tellGUIcareAboutLocking();  // assume we are an editable field
	}
}


int 
dbGUIAdaptLControl::getValue() const
{
	assert(mControl);
	const SInt32 ret = mControl->GetValue();
	return ret;
}


void 
dbGUIAdaptLControl::setValue(int inValue)
{
	assert(mControl);
	mControl->SetValue(inValue);
}





dbGUIAdapter* 
dbGUIAdaptLControl::clone() const
{
	return new dbGUIAdaptLControl(*this);
}


void 
dbGUIAdaptLControl::readOnly(bool becomeReadOnly)
{
	if (becomeReadOnly)
		mControl->Disable();
	else
		mControl->Enable();
}


void	
dbGUIAdaptLControl::ExecuteSelf(MessageT inMessage, void *ioParam)
{
	Assert_(inMessage==msg_Click);
	if (mControl->IsEnabled())	// see note in dbEditLinkDirtier::ExecuteSelf
		mFieldLink->maybeChangeTargetLink();	// only maybe if click again in the same field
	if (mFieldLink->isReadOnly())
		mExecuteHost = false;	// cancel host action
	else {
		mExecuteHost = true;
		mFieldLink->maybeDirty();
	}
}


bool 
dbGUIAdaptLControl::adaptsControl(void* inControl) const
{
	return (inControl==mControl);
}


// -------------------------------------------------------
//       d b G U I A d a p t E d i t P a n e
// -------------------------------------------------------
dbGUIAdaptEditPane::dbGUIAdaptEditPane(LPane* inPane, LCommander* inCommander) :
	mPane(inPane),
	mCommander(inCommander)
{
}


void
dbGUIAdaptEditPane::highlightContents()
{
	if (mCommander)
		LCommander::SwitchTarget(mCommander);
}


void 
dbGUIAdaptEditPane::readOnly(bool becomeReadOnly)
{
	if (becomeReadOnly)
		mPane->Disable();
	else
		mPane->Enable();
}


void	
dbGUIAdaptEditPane::ExecuteSelf(MessageT inMessage, void *ioParam)
{
	const bool selfIsReadOnly = IsReadOnly();
// WARNING must default this to true as it is called for messages we don't handle, eg: during menu enabling
// and if we don't reset to true (after swallowing a field change) then a side effect is disabling the menus!	
// This is a general problem with any LAttachment which can be called with a variety of messages.
	mExecuteHost = true;
	if (inMessage==msg_KeyPress) {
		const EventRecord*	inKeyEvent = (const EventRecord*) ioParam;	// safe downcast - see LCommander::ProcessKeyPress
		UInt16	theChar = inKeyEvent->message & charCodeMask;
		
			// Ignore Tab or Shift-Tab & navigation keys
		
		if ((theChar != char_Tab) &&
			(theChar != char_LeftArrow) &&
			(theChar != char_RightArrow) &&
			(theChar != char_UpArrow) &&
			(theChar != char_DownArrow) &&
			(theChar != char_Home) &&
			(theChar != char_End) &&
			(theChar != char_PageUp) &&
			(theChar != char_PageDown)
		) 
		{
			if (selfIsReadOnly)
				mExecuteHost = false;	// cancel message
			else
				editingOccurred();
		}
	}
	else {
		if ((inMessage==msg_Click) || (inMessage==msg_TabSelect)) {
			if (mPane->IsEnabled())	// LTabGroup::RotateTarget iterates all disabled fields to get 1st enabled
													// so when leaving a panel and it gets hidden, ALL links get called!
													// which is SLOW!!! as all force updates of derived displays etc.
				mFieldLink->maybeChangeTargetLink();	// only maybe if click again in the same field
		}
		else {
			if(
				inMessage==cmd_ActionCut ||
				inMessage==cmd_ActionPaste ||
				inMessage==cmd_ActionClear ||
				inMessage==cmd_ActionTyping ||
				inMessage==cmd_Cut ||
				inMessage==cmd_Paste ||
				inMessage==cmd_Clear
			) 
			{
				if (selfIsReadOnly) 
					mExecuteHost = false;	// cancel message
				else
					editingOccurred();
			}
		} // click
	} // not a keypress
}


void 
dbGUIAdaptEditPane::setFieldLink(dbFieldLink* inLink)
{
	const bool firstSettingOfLink = (mFieldLink==NULL);   // allow for pointing adapter at different fields
	dbGUIAdapter::setFieldLink(inLink);
	if (firstSettingOfLink) {
		mPane->AddAttachment(this, nil, false /* not owned by edit pane*/);
		inLink->tellGUIcareAboutLocking();  // assume we are an editable field
	}
	const dbField* editingField = mFieldLink->field();
	if (editingField && editingField->fieldType()==charField)
		setTextEntryLimit(editingField->fieldStorageLen());
}


oofString 
dbGUIAdaptEditPane::getString() const
{
	Str255 str;
	mPane->GetDescriptor(str);
	oofString ret;
	ret.setStr255(str);
	return ret;
}


void 
dbGUIAdaptEditPane::setString(const oofString& inString)
{
	Str255 str;
	inString.getStr255(str);
	mPane->SetDescriptor(str);
}


bool 
dbGUIAdaptEditPane::equalsString(const oofString& inString) const
{
	Str255 str;
	mPane->GetDescriptor(str);
	const bool ret = (inString==str);
	return ret;
}


bool 
dbGUIAdaptEditPane::adaptsControl(void* inControl) const
{
	return (inControl==mPane);
}


// -------------------------------------------------------
//   d b G U I A d a p t L a r g e E d i t P a n e
// -------------------------------------------------------
dbGUIAdaptLargeEditPane::dbGUIAdaptLargeEditPane(LPane* inPane,LCommander* editCommander) :
	dbGUIAdaptEditPane(inPane, editCommander)
{}


oofString 
dbGUIAdaptLargeEditPane::getString() const
{
	Handle h = GetTextHandle();	// safe downcast
	StHandleLocker playSafe(h);
	unsigned long hLen = ::GetHandleSize(h);
	const oofString ret(*h, hLen);
	return ret;
}


void 
dbGUIAdaptLargeEditPane::setString(const oofString& inString)
{
	SetTextPtr(inString);  // safe downcast
}


bool 
dbGUIAdaptLargeEditPane::equalsString(const oofString& inString) const
{
// repeat the Handle oriented stuff here from getString to avoid extra copying
// of an oofString which we would get by using base equalsString method
	Handle h = GetTextHandle();	// safe downcast
	StHandleLocker playSafe(h);
	unsigned long hLen = ::GetHandleSize(h);
	const oofString currString(*h, hLen);
	const bool ret = (inString==currString);
	return ret;
}



#ifdef OOF_PP17_TEXTEDIT

// -------------------------------------------------------
//      d b G U I A d a p t L T e x t E d i t
// -------------------------------------------------------

dbGUIAdaptLTextEdit::dbGUIAdaptLTextEdit(LTextEdit* inControl) :
	dbGUIAdaptLargeEditPane(inControl, inControl)
{
}


dbGUIAdapter* 
dbGUIAdaptLTextEdit::clone() const
{
	return new dbGUIAdaptLTextEdit(*this);
}


Handle 
dbGUIAdaptLTextEdit::GetTextHandle() const
{
	return ((LTextEdit*) mPane)->GetTextHandle();  // safe downcast
}


void  
dbGUIAdaptLTextEdit::SetTextPtr(const oofString& inString) const
{
	((LTextEdit*) mPane)->SetTextPtr(inString.charsForOldInterfacesNotWriting(), inString.length());  // safe downcast
}


// -------------------------------------------------------
//      d b G U I A d a p t L E d i t F i e l d
// -------------------------------------------------------

dbGUIAdaptLEditField::dbGUIAdaptLEditField(LEditField* inControl) :
	dbGUIAdaptEditPane(inControl, inControl)
{
}


dbGUIAdapter* 
dbGUIAdaptLEditField::clone() const
{
	return new dbGUIAdaptLEditField(*this);
}


void 
dbGUIAdaptLEditField::setTextEntryLimit(unsigned long inMaxChars)
{
	((LEditField*) mPane)->SetMaxChars(inMaxChars);  // safe downcast
}

#endif

#ifdef OOF_PP18_TEXTEDIT

// -------------------------------------------------------
//      d b G U I A d a p t L E d i t T e x t
// -------------------------------------------------------

dbGUIAdaptLEditText::dbGUIAdaptLEditText(LEditText* inControl) :
	dbGUIAdaptLargeEditPane(inControl, inControl)
{
}


dbGUIAdapter* 
dbGUIAdaptLEditText::clone() const
{
	return new dbGUIAdaptLEditText(*this);
}


Handle 
dbGUIAdaptLEditText::GetTextHandle() const
{
// LEditText doesn't have GetTextHandle like other edit classes
	TEHandle TEh = ((LEditText*) mPane)->GetMacTEH();  // safe downcast
	return (Handle) ::TEGetText(TEh);
}


void  
dbGUIAdaptLEditText::SetTextPtr(const oofString& inString) const
{
	((LEditText*) mPane)->SetText(inString.charsForOldInterfacesNotWriting(), inString.length());  // safe downcast
}


void 
dbGUIAdaptLEditText::setTextEntryLimit(unsigned long inMaxChars)
{
	((LEditText*) mPane)->SetMaxChars(inMaxChars);  // safe downcast
}


// -------------------------------------------------------
//    o o f G U I A d a p t L T e x t E d i t V i e w
// -------------------------------------------------------

dbGUIAdaptLTextEditView::dbGUIAdaptLTextEditView(LTextEditView* inControl) :
	dbGUIAdaptLargeEditPane(inControl, inControl)
{
}


dbGUIAdapter* 
dbGUIAdaptLTextEditView::clone() const
{
	return new dbGUIAdaptLTextEditView(*this);
}


Handle 
dbGUIAdaptLTextEditView::GetTextHandle() const
{
	return ((LTextEditView*) mPane)->GetTextHandle();  // safe downcast
}


void  
dbGUIAdaptLTextEditView::SetTextPtr(const oofString& inString) const
{
	((LTextEditView*) mPane)->SetTextPtr(inString.charsForOldInterfacesNotWriting(), inString.length());  // safe downcast
}


#endif


// -------------------------------------------------------
//      d b G U I A d a p t L C a p t i o n
// -------------------------------------------------------

dbGUIAdaptLCaption::dbGUIAdaptLCaption(LCaption* inControl) :
	dbGUIAdaptEditPane(inControl) // no Commander to pass to parent
{
}


dbGUIAdapter* 
dbGUIAdaptLCaption::clone() const
{
	return new dbGUIAdaptLCaption(*this);
}


void 
dbGUIAdaptLCaption::setFieldLink(dbFieldLink* inLink) 
{ 
	dbGUIAdaptEditor::setFieldLink(inLink); 
// bypass parent
};


// -------------------------------------------------------
//      d b G U I A  d a p t L S t d P o p u p M e n u
// -------------------------------------------------------
dbGUIAdaptLStdPopupMenu::dbGUIAdaptLStdPopupMenu(LStdPopupMenu* inMenu) :
	dbGUIAdaptLControl(inMenu)
{
	mIsValuePicker = true;
}


/**
	\todo implement to support older PowerPlant users
*/
void 
dbGUIAdaptLStdPopupMenu::setValueList(dbView&)
{
	assert(!"dbGUIAdaptLStdPopupMenu::setValueList not yet implemented");
}


// -------------------------------------------------------
//      d b G U I A  d a p t  L P o p u p B u t t o n
// -------------------------------------------------------
dbGUIAdaptLPopupButton::dbGUIAdaptLPopupButton(LPopupButton* inMenu) :
	dbGUIAdaptLControl(inMenu),
	mMenuController(inMenu)
{
	mIsValuePicker = true;
}


/**
	Clear the menu and reset from dbView.
	\param inView view we just use first field from to get contents
	\todo work out way to handle multiple fields from view, maybe just space them out?
*/
void 
dbGUIAdaptLPopupButton::setValueList(dbView& inView)
{
	mMenuController->DeleteAllMenuItems();
	dbTable* viewTable = inView.table();
	assert(viewTable);  // report writer may cope with views without tables but we don't
	for(viewTable->start(); viewTable->more(); viewTable->next()) {
		Str255 menuBuf;
		inView.field(0).copyString().getStr255(menuBuf);
		mMenuController->InsertMenuItem(menuBuf, SHRT_MAX);
	}
}
