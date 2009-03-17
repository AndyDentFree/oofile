// COPYRIGHT 1997 A.D. Software, All rights reserved

// MFC Integration layer of OOFILE database
#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing


#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFMFCX
	#include "oofmfcx.h"  // private interface, includes public
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




// -------------------------------------------------------
//      d b G U I A d a p t C W n d
// -------------------------------------------------------
dbGUIAdaptCWnd::dbGUIAdaptCWnd(CWnd* inWnd) :
	mWnd(inWnd),
	mAttachableWnd(dynamic_cast<OOF_mixEditAttachment*>(inWnd))
{
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI"
#endif	
}


dbGUIAdaptCWnd::dbGUIAdaptCWnd(CWnd* inWnd, OOF_mixEditAttachment* inAttach) :
	mWnd(inWnd),
	mAttachableWnd(inAttach)
{}


dbGUIAdapter* 
dbGUIAdaptCWnd::clone() const
{
	return new dbGUIAdaptCWnd(*this);
}


bool 
dbGUIAdaptCWnd::adaptsControl(void* inControl) const
{
	return (inControl==mWnd);
}


void 
dbGUIAdaptCWnd::readOnly(bool isReadOnly)
{
		mWnd->EnableWindow(!isReadOnly);
}


void 
dbGUIAdaptCWnd::highlightContents()
{
		mWnd->SetFocus();
		// ES - this message selects all the text in the edit box,
		// every other control ignores it
		mWnd->SendMessage(EM_SETSEL, 0, -1);
}


void 
dbGUIAdaptCWnd::setFieldLink(dbFieldLink* inLink)
{
	if(mAttachableWnd)
		mAttachableWnd->addAttachment(this);  // very limited implementation of attachments, just for this context
/*
2000-01-20 no longer applicable now that report editors use link classes
	else
		ASSERT(inLink->alwaysReadOnly());  // eg: CStatic
*/
	mFieldLink = inLink;
	inLink->tellGUIcareAboutLocking();
}


oofString 
dbGUIAdaptCWnd::getString() const
{
	CString str;
	mWnd->GetWindowText(str);
	const char* cstr = str;
	return oofString(cstr);
}


void 
dbGUIAdaptCWnd::setString(const oofString& inString)
{
	mWnd->SetWindowText(inString);
}


void
dbGUIAdaptCWnd::OnMessage(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) {
	case WM_KEYDOWN:
		// Ignore Tab or Shift-Tab & navigation keys
		if ((wParam != VK_TAB) &&
			(wParam != VK_LEFT) &&
			(wParam != VK_RIGHT) &&
			(wParam != VK_UP) &&
			(wParam != VK_DOWN) &&
			(wParam != VK_HOME) &&
			(wParam != VK_END) &&
			(wParam != VK_PRIOR) &&
			(wParam != VK_INSERT) &&
			(wParam != VK_NEXT) &&
			(wParam != VK_SHIFT) &&
			(wParam != VK_CONTROL))
		{
			mFieldLink->maybeDirty();
		}
		break;
		
	case WM_KILLFOCUS:
		break;
		
	case WM_CUT:
	case WM_PASTE:
	case WM_CLEAR:
		mFieldLink->maybeDirty();
		break;
		
	default:
		break;
	}
}



void
dbGUIAdaptCWnd::OnKillFocus(CWnd* pNewWnd) 
{
	if(mFieldLink) {
		if (pNewWnd) {
			dbHelper* ourLinkBoss = mFieldLink->owner();
			assert(ourLinkBoss);
			dbAbstractEditHelper* editHelper = dynamic_cast<dbAbstractEditHelper*>(ourLinkBoss);
			if(editHelper) {  // field links may be owned by any other kind of helper
				dbFieldLink* newLink = editHelper->getLink(pNewWnd);
				if (newLink)  // may be transit into un-owned field
					newLink->maybeChangeTargetLink();
				else
					mFieldLink->leaveLink();  // at least say goodbye to current one
			}  // standard use of helper
			// else (eg: report editing) field link may work differently
		}
		else {
				mFieldLink->leaveLink();
		}
	}
}


// -------------------------------------------------------
//      d b G U I A d a p t C C o m b o 
// -------------------------------------------------------
dbGUIAdaptCCombo::dbGUIAdaptCCombo(CWnd* inWnd) :
	dbGUIAdaptCWnd(inWnd)
{}


dbGUIAdapter* 
dbGUIAdaptCCombo::clone() const
{
	return new dbGUIAdaptCCombo(*this);
}


void
dbGUIAdaptCCombo::OnMessage(UINT message, WPARAM, LPARAM) 
{
	switch (message) {
	case WM_LBUTTONUP:
	case WM_KEYDOWN:
		mFieldLink->maybeChangeTargetLink();	// only maybe if click again in the same field
		mFieldLink->maybeDirty();
		break;

	case WM_KILLFOCUS:
		mFieldLink->leaveLink();
		break;

	case CBN_SELCHANGE:
		mFieldLink->maybeDirty();
		break;

	default:
		break;
	}
}



// -------------------------------------------------------
//    d b G U I A d a p t C E d i t
// -------------------------------------------------------
dbGUIAdaptCEdit::dbGUIAdaptCEdit(CWnd* inWnd) :
	dbGUIAdaptCWnd(inWnd)
{}


dbGUIAdapter* 
dbGUIAdaptCEdit::clone() const
{
	return new dbGUIAdaptCEdit(*this);
}


void 
dbGUIAdaptCEdit::readOnly(bool isReadOnly)
{
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI"
#endif	
	CEdit* theEdit = dynamic_cast<CEdit*>(mWnd);
	ASSERT(theEdit);  // don't expect to fail - should only use this with CEdit or subclasses
	VERIFY(theEdit->SetReadOnly(isReadOnly));
/*
old logic - may need to use if use VC4 as not certain of its RTTI???
		if(mLinkTo->IsKindOf( RUNTIME_CLASS( CEdit ) ) )
		{
			CEdit* pEdit = (CEdit*) mLinkTo ;  // safe downcast
			pEdit->SetReadOnly(isReadOnly);
		}

*/
}



// -------------------------------------------------------
//    d b G U I A d a p t C B u t to n
// -------------------------------------------------------
dbGUIAdaptCButton::dbGUIAdaptCButton(CWnd* inWnd) :
	dbGUIAdaptCWnd(inWnd)
{}


dbGUIAdapter* 
dbGUIAdaptCButton::clone() const
{
	return new dbGUIAdaptCButton(*this);
}


void
dbGUIAdaptCButton::OnMessage(UINT message, WPARAM, LPARAM) 
{
	switch (message) {
	case WM_KILLFOCUS:
		mFieldLink->leaveLink();
		break;

	default:
		break;
	}
}


oofString 
dbGUIAdaptCButton::getString() const
{
	const int theValue = getValue();
	oofString ret;
	ret.convertNumber(theValue);
	return ret;
}


void 
dbGUIAdaptCButton::setString(const oofString& inString)
{
	const long theValue = inString.asLong();
	setValue(theValue);
}


bool 
dbGUIAdaptCButton::equalsString(const oofString& inString) const
{
	const long theValue = getValue();
	const long inValue = inString.asLong();
	const bool ret = (theValue==inValue);
	return ret;
}


// hopefully this adapter will be used mainly with field links that directly use
// getValue, setValue and equalsValue (eg: dbBoolFieldLink)
// the string versions above are just for generic use
int 
dbGUIAdaptCButton::getValue() const
{
	const int ret = ((CButton*)mWnd)->GetCheck() ;  // safe downcast
	return ret;
}


void 
dbGUIAdaptCButton::setValue(int inValue)
{
	((CButton*)mWnd)->SetCheck(inValue);  // safe downcast
}

#endif  // !_Macintosh for doxygen
