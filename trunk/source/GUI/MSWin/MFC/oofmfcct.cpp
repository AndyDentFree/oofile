// oofmfcct.cpp : implementation file
//

#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofmfcct.h"
#include "oofmfcx.h"
#include "oofview.h"
#include "oofmfchr.h"
#include "oofmfcpd.h"
#include "oofres.h"
#include "oofmfcic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _MSC_VER>=1010
	// want to suppress these warnings for ver 4.1 but not other compilers or warnings
	// eg: 0x800 for earlier version 1.5
	#pragma warning( disable : 4237 )  // bool, true & false
	#pragma warning( disable : 4800 )  // bool conversion to int
	#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
#endif

struct DropStruct {
	CWnd* commandTarget;
	long int commandID;
};

/////////////////////////////////////////////////////////////////////////////
// COOFOleDropTarget

COOFOleDropTarget::COOFOleDropTarget() {}

COOFOleDropTarget::~COOFOleDropTarget() {}     

// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
DROPEFFECT COOFOleDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject*
				pDataObject, DWORD dwKeyState, CPoint point)
{
	long commandID;
	GetDropData(pDataObject, NULL, &commandID);

	if (pWnd->SendMessage(WM_OOFILE_ACCEPTDROP, (WPARAM)commandID) == 0)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;

/*    // if the control key is held down, return a drop effect COPY 
    if ((dwKeyState&MK_CONTROL) == MK_CONTROL)
        return DROPEFFECT_COPY;
    // Otherwise return a drop effect of MOVE
    else
        return DROPEFFECT_MOVE;    
*/
}

// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
void COOFOleDropTarget::OnDragLeave(CWnd* pWnd)
{
    // Call base class implementation
    COleDropTarget::OnDragLeave(pWnd);
}

// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
DROPEFFECT COOFOleDropTarget::OnDragOver(CWnd* pWnd, COleDataObject*
				pDataObject, DWORD dwKeyState, CPoint point)
{
/*	if ((dwKeyState&MK_CONTROL) == MK_CONTROL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_MOVE;  // move source
*/
	long commandID;
	GetDropData(pDataObject, NULL, &commandID);

	if (pWnd->SendMessage(WM_OOFILE_ACCEPTDROP, (WPARAM)commandID) == 0)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;
}

// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
BOOL COOFOleDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
				DROPEFFECT dropEffect, CPoint point)
{
	CWnd* commandTarget;
	long commandID;

	GetDropData(pDataObject, &commandTarget, &commandID);

	ASSERT(pWnd);
	commandTarget->SendMessage(WM_COMMAND, MAKELONG(commandID, BN_CLICKED), (LPARAM)pWnd->m_hWnd);
	return TRUE;
}

void COOFOleDropTarget::GetDropData(COleDataObject* pDataObject, CWnd** ppCommandTarget,
									long* pCommandID)
{
	HGLOBAL hGlobal;
	struct DropStruct* dropStruct;

	// get data from COleDataObject
	hGlobal = pDataObject->GetGlobalData(CF_TEXT);
	// get pointer to data
	dropStruct = (struct DropStruct*)GlobalLock(hGlobal);    
	ASSERT(dropStruct != NULL);

	if (ppCommandTarget != NULL)
		*ppCommandTarget = dropStruct->commandTarget;

	if (pCommandID != NULL)
		*pCommandID = dropStruct->commandID;

	// Unlock memory
	GlobalUnlock(hGlobal);
}

/////////////////////////////////////////////////////////////////////////////


// -------------------------------------------------------
// O O F _ m i x E d i t A t t a c h m e n t
// -------------------------------------------------------
// using this base instead of the following code frees us 
// to support a more general model of multiple attachments
// in future, without changing lots of editing classes

OOF_mixEditAttachment::OOF_mixEditAttachment() :
	mAdapter(0)
{}


void
OOF_mixEditAttachment::AttachedOnMessage(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (mAdapter)
		mAdapter->OnMessage(message, wParam, lParam) ;
}


void
OOF_mixEditAttachment::AttachedOnKillFocus(CWnd* pNewWnd) 
{
	if (mAdapter)
		mAdapter->OnKillFocus(pNewWnd);
}


/////////////////////////////////////////////////////////////////////////////
// COOFEdit

LRESULT
COOFEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	AttachedOnMessage(message, wParam, lParam);

	return CEdit::WindowProc(message, wParam, lParam);
}

BEGIN_MESSAGE_MAP(COOFEdit, CEdit)
	//{{AFX_MSG_MAP(COOFEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFEdit message handlers

void COOFEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	AttachedOnKillFocus(pNewWnd);
}



/////////////////////////////////////////////////////////////////////////////
// COOFComboBox::COOFSubWnd

WNDPROC COOFComboBox::COOFSubEdit::pfnSuper = NULL;

WNDPROC* COOFComboBox::COOFSubEdit::GetSuperWndProcAddr()
{
    return &pfnSuper;
}

/////////////////////////////////////////////////////////////////////////////
// COOFComboBox

BEGIN_MESSAGE_MAP(COOFComboBox, CComboBox)
	//{{AFX_MSG_MAP(COOFComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFComboBox message handlers

void COOFComboBox::OnSelChange() 
{
	AttachedOnMessage(CBN_SELCHANGE, 0, 0);
}


void COOFComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	AttachedOnKillFocus(pNewWnd);
}

/////////////////////////////////////////////////////////////////////////////
// COOFCheckBox

LRESULT
COOFCheckBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	AttachedOnMessage(message, wParam, lParam);
	return CButton::WindowProc(message, wParam, lParam);
}

BEGIN_MESSAGE_MAP(COOFCheckBox, CButton)
	//{{AFX_MSG_MAP(COOFCheckBox)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFCheckBox message handlers

void COOFCheckBox::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	AttachedOnKillFocus(pNewWnd);
}

/////////////////////////////////////////////////////////////////////////////
// COOFListBox


COOFListBox::COOFListBox() :
	OOF_NavigableList(this),
	mSingleClickMessage(0),
	mDoubleClickMessage(0),
	mCommandID(-1),
	mCommandTarget(NULL),
	mView(NULL),
	mNumID(0),
	mArrayID(NULL),
	mDropID(0),
	mItemCaching(TRUE)
{
	mItemCache = new dbItemCache();
}

#pragma warning( default : 4355 )

COOFListBox::~COOFListBox()
{
	if (mArrayID != NULL)
		delete[] mArrayID;

	delete mItemCache;
}

dbGUI*
COOFListBox::getGUI() const
{
	return mView != NULL ? mView->table()->getGUI() : NULL;
}

const char*
COOFListBox::GetItemString(UINT nIndex)
{
	if (isItemCachingEnabled())
		return mItemCache->getString(nIndex, 0, this);
	else
		return mItemCache->getStringFromView(*mView, nIndex, 0);
}

void
COOFListBox::initializeSpace(unsigned int count)
{
	BeginWaitCursor();
	// initialize the initial memory allocated for listbox; we don't need
	// any space for item strings
	VERIFY(InitStorage(count,0) != LB_ERRSPACE);

	if (count > ProgressDlgThreshold) {
#ifdef OOFMFC_STATIC
		// set the current resource handle into OOFile resource DLL
		// (DLL version of OOFMFC has linked resources with it, so there
		// is no need to set the resource handle)
		SetOOFileResources setRes;
#endif
		// progress dialog will be displayed
		COOFProgressDlg* progressDlg = new COOFProgressDlg(NULL);
		// create and initialize progress dialog
		CString s;
		VERIFY(s.LoadString(IDS_INITLISTBOX));
		VERIFY(progressDlg->Create(count, s));
		// fill list control with empty items
		FillItems(count, progressDlg);
		// finish progress dialog
		progressDlg->DestroyWindow();
	}
	else {
		FillItems(count, NULL);
	}

	EndWaitCursor();
}

void
COOFListBox::FillItems(unsigned int count, COOFProgressDlg* progressDlg)
{
	if (progressDlg != NULL) {
		for (unsigned int i=0; i<count; i++) {
			int retVal = AddString("");
			ASSERT(retVal != LB_ERR);
			ASSERT(retVal != LB_ERRSPACE);
			// update progress bar position
			if (i%100 == 0)
				progressDlg->setPos(i);
		}
		progressDlg->setPos(count);
	}
	else {
		for (unsigned int i=0; i<count; i++) {
			int retVal = AddString("");
			ASSERT(retVal != LB_ERR);
			ASSERT(retVal != LB_ERRSPACE);
		}
	}
}

void
COOFListBox::adjustNumItems()
{
	// current number of items in listbox
	int itemCount = GetCount();
	// difference between number of items in table and current number of items in list control
	int nItemDiff = mView->source()->count() - itemCount;

	if (nItemDiff > 0) {
		// items have been inserted
		// append new empty items at the end of the current listbox of items
		for (int i=0; i<nItemDiff; i++) {
			int retVal = AddString("");
			ASSERT(retVal != LB_ERR);
			ASSERT(retVal != LB_ERRSPACE);
		}
	}
	else if (nItemDiff < 0) {
		// items have been deleted
		// number of items to delete
		nItemDiff = abs(nItemDiff);
		// delete items from end of the current list of items
		for (int i=0; i<nItemDiff; i++) {
			// index of item is zero-based
			VERIFY(DeleteString(itemCount-i-1) != LB_ERR);
		}
	}

	// dirty the list box's cache
	mItemCache->setDirtyFlag();
}

void
COOFListBox::changeViewTo(dbView* theView)
{
	mView = theView;
	if (isItemCachingEnabled())
		mItemCache->setNumFields(mView);
	RedrawWindow();
}

void
COOFListBox::enableItemCaching(BOOL bEnable)
{
	if (!bEnable && isItemCachingEnabled())
		mItemCache->deleteItemCache();

	if (bEnable && mView != NULL)
		mItemCache->setNumFields(mView);

	mItemCaching = bEnable;
}

void COOFListBox::enableDragDrop(long int commandID, CWnd* commandTarget)
{
	ASSERT(commandID != -1);
	ASSERT(commandTarget != NULL);

	mCommandID		= commandID;
	mCommandTarget	= commandTarget;
}

void COOFListBox::acceptDrop(long ID)
{
	long tmp = ID;
	acceptDrop(1, &tmp);
}

void COOFListBox::acceptDrop(unsigned int numID, long* arrayID)
{
// copies of this routine exist in COOFTreeCtrl, COOFListCtrl and COOFListBox and we need to use a mixin
// may be called multiple times and hence need to preserve old data
	ASSERT(numID);
	if (mNumID) {  // has existing values
		const unsigned int newNum = numID + mNumID;
		long* newArray = new long[newNum];
		ASSERT(newArray);
		if (mNumID==1) {  // old value in mDropID
			newArray[0] = mDropID;
		}
		else {  // array to copy
			memcpy(newArray, mArrayID, mNumID * sizeof(long));
			delete[] mArrayID;
		}  // 1 or many old values
		memcpy(&newArray[mNumID], arrayID, numID * sizeof(long));  // append incoming data
		mArrayID = newArray;
		mNumID = newNum;
// don't need to register a second time		
	}  // preserved old value(s)
	
	else {
		if (numID==1) {
			const long onlyID = arrayID[0];
			if (onlyID==0)  {  // probably default case from single param
				mDropID = mNumID = 0;	// not really any data, just the default
			}
			else {
				mDropID = onlyID;
				mNumID = 1;
			}
		} 
		else
			mNumID = numID;
	
		if (mNumID > 1) {
			mArrayID = new long[mNumID];
			ASSERT(mArrayID != NULL);
			memcpy(mArrayID, arrayID, mNumID * sizeof(long));
		}
	
		// register the COleDropTarget object with the OLE dll's
		BOOL success = mDropTarget.Register(this);
	
		if (!success)
			MessageBox("Ole Register Drop Target Failed");     
	} // first time called
}

int COOFListBox::GetCountPerPage() const
{
	CRect clientRect;
	GetClientRect(&clientRect);

	CClientDC dc((CWnd*)this);

	TEXTMETRIC tm;
	memset(&tm, '\0', sizeof(tm));

	dc.GetTextMetrics(&tm);

	return clientRect.Height() / tm.tmHeight;
}

BOOL COOFListBox::EnsureVisible(int nIndex)
{
	if (GetCount() <= 0)
		return FALSE;

	const int topIndex = GetTopIndex();
	const int numVisibleItems = GetCountPerPage();

	ASSERT(topIndex != LB_ERR);

	if (nIndex > topIndex && nIndex < topIndex + numVisibleItems)
		return TRUE;
	else
		return SetTopIndex(nIndex < topIndex ? nIndex : nIndex-numVisibleItems) != LB_ERR;
}

BEGIN_MESSAGE_MAP(COOFListBox, CListBox)
	//{{AFX_MSG_MAP(COOFListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MEASUREITEM()
	ON_WM_TIMER()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OOFILE_ACCEPTDROP, OnOOFileAcceptDrop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFListBox message handlers

void COOFListBox::OnSelChange() 
{
		if (mSingleClickMessage!=OOFmsg_None)
			dbEditHelper::postFrameworkMessage(mSingleClickMessage, (WPARAM)m_hWnd);
}

void COOFListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	dbGUI* gui = getGUI();

	if (gui) {
		switch (nChar) {
		case VK_DELETE:
//			gui->msgForOOFILE(cmd_dbDel, this);
				::MessageBeep(0);
		break;
		case VK_INSERT:
//			gui->msgForOOFILE(cmd_dbNew);
			::MessageBeep(0);
			break;
		default:
			CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
	else  // no GUI 
		CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COOFListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsTypingChar(nChar)) {
		AddChar(nChar);
	}

	CListBox::OnChar(nChar, nRepCnt, nFlags);
}

void COOFListBox::OnDblClk() 
{
	if (mDoubleClickMessage != OOFmsg_None) {
		const int singleSel = GetCurSel();
		if (singleSel!=LB_ERR)
			dbEditHelper::postFrameworkMessage(mDoubleClickMessage, (WPARAM)m_hWnd);
	}
}

void COOFListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// if no items in list box yet, indicate focus for specified rectangle
	if (lpDrawItemStruct->itemID == -1) {
		VERIFY(::DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT) &lpDrawItemStruct->rcItem));
		return;
	}

	// get item text from index reffered by itemID
	CString sItemString(GetItemString(lpDrawItemStruct->itemID));

	COLORREF crBack;
	COLORREF crText;

	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		// set background and text colors for selected item
		crBack = ::GetSysColor(COLOR_HIGHLIGHT);
		crText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else {
		// set background and text colors for unselected item
		crBack = ::GetSysColor(COLOR_WINDOW);
		crText = ::GetSysColor(COLOR_WINDOWTEXT);
	}

	// fill item rectangle with background color
	HBRUSH hbrBack = ::CreateSolidBrush(crBack);
	ASSERT(hbrBack != NULL);

	VERIFY(::FillRect(lpDrawItemStruct->hDC, (LPRECT) &lpDrawItemStruct->rcItem, hbrBack));
	VERIFY(::DeleteObject(hbrBack));

	// set current background and text colors
	VERIFY(::SetBkColor(lpDrawItemStruct->hDC, crBack) != CLR_INVALID);
	VERIFY(::SetTextColor(lpDrawItemStruct->hDC, crText) != CLR_INVALID);
	VERIFY(
		::TextOut(
			lpDrawItemStruct->hDC,
			lpDrawItemStruct->rcItem.left + LeftMargin,
			lpDrawItemStruct->rcItem.top + TopMargin,
			sItemString,
			sItemString.GetLength())
	);

	// if enabled item has the input focus, call
	// DrawFocusRect to set or clear the focus rectangle
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		VERIFY(::DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT) &lpDrawItemStruct->rcItem));
}

void COOFListBox::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	TEXTMETRIC tm;
	// get and use height of system font for drawing
	CWnd* pOwnerWnd = GetParentOwner();
	CDC* pDC = pOwnerWnd->GetDC();		// get device context
	pDC->GetTextMetrics(&tm);			// get text information
	lpMeasureItemStruct->itemHeight = tm.tmHeight;	// required to set height
	pOwnerWnd->ReleaseDC(pDC);			// release device context
	
	CListBox::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void COOFListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDown(nFlags, point);

	// trivial case - drag&drop is not permitted
	if (mCommandID == -1)
		return;

	// create global memory for sharing dragdrop data
	HGLOBAL hgData = GlobalAlloc(GPTR, sizeof(struct DropStruct));
	ASSERT(hgData!=NULL);

	struct DropStruct* dropStruct = (struct DropStruct*)GlobalLock(hgData);
	ASSERT(dropStruct != NULL);

	dropStruct->commandID = mCommandID;
	dropStruct->commandTarget = mCommandTarget;

	// cache the data, and initiate DragDrop
	mOleDataSource.CacheGlobalData(CF_TEXT, hgData);           

	DROPEFFECT dropEffect = mOleDataSource.DoDragDrop(DROPEFFECT_COPY, NULL);

	// We need to send WM_LBUTTONUP to control or else the selection rectangle 
	// will "follow" the mouse (like when you hold the left mouse down and 
	// scroll through a regular listbox). WM_LBUTTONUP was sent to window that 
	// recieved the drag/drop, not the one that initiated it, so we simulate
	// an WM_LBUTTONUP to the initiating window.
	LPARAM lparam;

	// "Pack" lparam with x and y coordinates where lbuttondown originated
	lparam=point.y; 
	lparam=lparam<<16;
	lparam &= point.x;                         

	SendMessage(WM_LBUTTONUP,0,lparam);

	// Clear the Data Source's cache
	mOleDataSource.Empty();        
}

LONG COOFListBox::OnOOFileAcceptDrop(UINT wParam, LONG lParam)
{
	switch (mNumID) {
	case 0:			// accept everything
		return 0;
	case 1:			// should cover most cases
		return mDropID != (long)wParam;
	default:
		// look up the ID in arrayID
		for (unsigned int i=0; i<mNumID; i++) {
			if (mArrayID[i] == (long)wParam)
				return 0;
		}
		return 1;
	}
}


void 
COOFListBox::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==mNavListTimerID) {
// react to timer by searching for string
		unsigned long zeroBasedRowNum;
		const int numItems = GetCount();
		if (FindRowforString(mView, zeroBasedRowNum)) {
//				if (GetCount() > 0) {
//					SelItemRange(true, zeroBasedRowNum, zeroBasedRowNum);
//				}
			EnsureVisible(zeroBasedRowNum);
		}
		else  // didn't find so scroll to end
				EnsureVisible(numItems -1);		
		ResetTimer();
	}
}


/////////////////////////////////////////////////////////////////////////////
// COOFListCtrl

// disable Warning C4355: "'this' used in base member initializer list."
#pragma warning( disable : 4355 )

COOFListCtrl::COOFListCtrl() :
	OOF_NavigableList(this),
	mSingleClickMessage(0),
	mDoubleClickMessage(0),
	mCommandID(-1),
	mCommandTarget(NULL),
	mView(NULL),
	mNumID(0),
	mArrayID(NULL),
	mDropID(0),
	mItemCaching(TRUE),
	mHighlight(HIGHLIGHT_NORMAL)
{
#ifdef _DEBUG
	mNotificationDisabled = false;
#endif
	mItemCache = new dbItemCache();
}

#pragma warning( default : 4355 )

COOFListCtrl::~COOFListCtrl()
{
	if (mArrayID != NULL)
		delete[] mArrayID;

	delete mItemCache;
}

dbGUI*
COOFListCtrl::getGUI() const
{
	return mView != NULL ? mView->table()->getGUI() : NULL;
}

UINT
COOFListCtrl::getColCount() const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	ASSERT(pHeader != NULL);
	return pHeader->GetItemCount();
}

/**
	Load a CListCtrl with OOFILE data.
	Displays COOFProgressDlg if list exceeds ProgressDlgThreshold.
	\todo make this limit user configurable and maybe based on machine performance.
*/
void
COOFListCtrl::initializeSpace(unsigned long count)
{
	BeginWaitCursor();
	SetItemCount(count);

	if (count > ProgressDlgThreshold) {
#ifdef OOFMFC_STATIC
		// set the current resource handle into OOFile resource DLL
		// (DLL version of OOFMFC has linked resources with it, so there
		// is no need to set the resource handle)
		SetOOFileResources setRes;
#endif
		// progress dialog will be displayed
		COOFProgressDlg* progressDlg = new COOFProgressDlg(NULL);
		// create and initialize progress dialog
		CString s;
		VERIFY(s.LoadString(IDS_INITLISTCTRL));
		VERIFY(progressDlg->Create(count, s));
		// fill list control with empty items
		FillItems(count, progressDlg);
		// finish progress dialog
		progressDlg->DestroyWindow();
	}
	else {
		FillItems(count, NULL);
	}

	EndWaitCursor();
}

void
COOFListCtrl::FillItems(unsigned long count, COOFProgressDlg* progressDlg)
{
	LV_ITEM item;
	ZeroMemory(&item, sizeof(LV_ITEM));

	item.mask		= LVIF_TEXT;
	item.pszText	= LPSTR_TEXTCALLBACK;

	if (progressDlg != NULL) {
		for (unsigned long i=0; i<count; i++) {
			item.iItem = i;
			VERIFY(InsertItem(&item) > -1);
			// update progress bar position
			if (i%100 == 0)
				progressDlg->setPos(i);
		}
		progressDlg->setPos(count);
	}
	else {
		for (unsigned long i=0; i<count; i++) {
			item.iItem = i;
			VERIFY(InsertItem(&item) > -1);
		}
	}
}

void
COOFListCtrl::redraw()
{
	int firstVisible = GetTopIndex();
	int lastVisible  = firstVisible + GetCountPerPage();

	RedrawItems(firstVisible, lastVisible);
}

void
COOFListCtrl::adjustNumItems()
{
	// current number of items in list control
	int itemCount = GetItemCount();
	// difference between number of items in table and current number of items in list control
	int nItemDiff = mView->source()->count() - itemCount;

	if (nItemDiff > 0) {
		// items have been inserted
		// item descriptor
		LV_ITEM item;
		ZeroMemory(&item, sizeof(LV_ITEM));
		// fill item descriptor
		item.mask		= LVIF_TEXT;
		item.pszText	= LPSTR_TEXTCALLBACK;
		// append new empty items at the end of the current list of items
		for (int i=0; i<nItemDiff; i++) {
			// item index
			item.iItem = itemCount + i;
			// append new item
			VERIFY(InsertItem(&item) > -1);
		}
	}
	else if (nItemDiff < 0) {
		// items have been deleted
		// number of items to delete
		nItemDiff = abs(nItemDiff);
		// delete items from end of the current list of items
		for (int i=0; i<nItemDiff; i++) {
			// index of item iz zero-based
			VERIFY(DeleteItem(itemCount-i-1));
		}
	}

	// dirty the list control's cache
	mItemCache->setDirtyFlag();
}

void COOFListCtrl::enableDragDrop(long int commandID, CWnd* commandTarget)
{
	ASSERT(commandID != -1);
	ASSERT(commandTarget != NULL);

	mCommandID		= commandID;
	mCommandTarget	= commandTarget;
}

void COOFListCtrl::acceptDrop(long ID)
{
	long tmp = ID;
	acceptDrop(1, &tmp);
}

void COOFListCtrl::acceptDrop(unsigned int numID, long* arrayID)
{
// copies of this routine exist in COOFTreeCtrl, COOFListCtrl and COOFListBox and we need to use a mixin
// may be called multiple times and hence need to preserve old data
	ASSERT(numID);
	if (mNumID) {  // has existing values
		const unsigned int newNum = numID + mNumID;
		long* newArray = new long[newNum];
		ASSERT(newArray);
		if (mNumID==1) {  // old value in mDropID
			newArray[0] = mDropID;
		}
		else {  // array to copy
			memcpy(newArray, mArrayID, mNumID * sizeof(long));
			delete[] mArrayID;
		}  // 1 or many old values
		memcpy(&newArray[mNumID], arrayID, numID * sizeof(long));  // append incoming data
		mArrayID = newArray;
		mNumID = newNum;
// don't need to register a second time		
	}  // preserved old value(s)
	
	else {
		if (numID==1) {
			const long onlyID = arrayID[0];
			if (onlyID==0)  {  // probably default case from single param
				mDropID = mNumID = 0;	// not really any data, just the default
			}
			else {
				mDropID = onlyID;
				mNumID = 1;
			}
		} 
		else
			mNumID = numID;
	
		if (mNumID > 1) {
			mArrayID = new long[mNumID];
			ASSERT(mArrayID != NULL);
			memcpy(mArrayID, arrayID, mNumID * sizeof(long));
		}
	
		// register the COleDropTarget object with the OLE dll's
		BOOL success = mDropTarget.Register(this);
	
		if (!success)
			MessageBox("Ole Register Drop Target Failed");     
	} // first time called
}

void
COOFListCtrl::changeViewTo(dbView* theView)
{
	mView = theView;
	if (isItemCachingEnabled())
		mItemCache->setNumFields(mView);
	redraw();
}

void
COOFListCtrl::enableItemCaching(BOOL bEnable)
{
	if (!bEnable && isItemCachingEnabled())
		mItemCache->deleteItemCache();

	if (bEnable && mView != NULL)
		mItemCache->setNumFields(mView);

	mItemCaching = bEnable;
}

void COOFListCtrl::disableOOFileNotification()
{
	mStoredSingleClickMessage = getSingleClickMessage();
	mStoredDoubleClickMessage = getDoubleClickMessage();

	ASSERT(!mNotificationDisabled);
	setSingleClickMessage(0);
	setDoubleClickMessage(0);

#ifdef _DEBUG
	mNotificationDisabled = true;
#endif
}

void COOFListCtrl::enableOOFileNotification()
{
	ASSERT(mNotificationDisabled);
	setSingleClickMessage(mStoredSingleClickMessage);
	setDoubleClickMessage(mStoredDoubleClickMessage);

#ifdef _DEBUG
	mNotificationDisabled = false;
#endif
}

// selItemRange - Selects/Deselect a range of items
// Returns		- The number of new items selected
// bSelect		- TRUE to select, FALSE to deselect
// nFirstItem	- index of first item to select
// nLastItem	- index of last item to select
int COOFListCtrl::selItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
{
	// make sure nFirstItem and nLastItem are valid
	if (nFirstItem >= GetItemCount() || nLastItem >= GetItemCount())
		return 0;

	int nItemsSelected = 0;
	int nFlags = bSelect ? 0 : LVNI_SELECTED;
	int nItem = nFirstItem - 1;

	while ((nItem = GetNextItem(nItem, nFlags)) >= 0 && nItem <= nLastItem) {
		nItemsSelected++;
		SetItemState(nItem, bSelect ? LVIS_SELECTED : 0, LVIS_SELECTED);
	}

	return nItemsSelected;
}

void COOFListCtrl::removeFocus()
{
	int nFocused = GetNextItem(-1, LVNI_ALL|LVNI_FOCUSED);

	if (nFocused != -1 )
		SetItemState(nFocused, 0, LVIS_FOCUSED);
}

void COOFListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight = (lvi.state & LVIS_DROPHILITED) ||
		((lvi.state & LVIS_SELECTED) && ((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS)));

	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol(rcBounds);

	CString sLabel = getString(nItem, 0);

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	CRect rcHighlight;
	CRect rcWnd;
	int nExt;

	switch (mHighlight)	{
	case HIGHLIGHT_NORMAL:
		nExt = pDC->GetOutputTextExtent(sLabel).cx + offset;
		rcHighlight = rcLabel;
		if (rcLabel.left + nExt < rcLabel.right)
			rcHighlight.right = rcLabel.left + nExt;
		break;

	case HIGHLIGHT_ALLCOLUMNS:
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		break;

	case HIGHLIGHT_ROW:
		GetClientRect(&rcWnd);
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		rcHighlight.right = rcWnd.right;
		break;

	default:
		rcHighlight = rcLabel;
	}

	// Draw the background color
	if (bHighlight) {
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_WINDOW)));	

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// Draw state icon
	if (lvi.state & LVIS_STATEIMAGEMASK) {
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
		pImageList = GetImageList(LVSIL_STATE);

		if (pImageList)
			pImageList->Draw(pDC, nImage, CPoint(rcCol.left, rcCol.top), ILD_TRANSPARENT);
	}

	// Draw normal and overlay icon
	pImageList = GetImageList(LVSIL_SMALL);

	if (pImageList) {
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC, lvi.iImage,
			CPoint(rcIcon.left, rcIcon.top),
			(bHighlight ? ILD_BLEND50 : 0) | ILD_TRANSPARENT | nOvlImageMask);
	}

	// Draw item label - Column 0
	rcLabel.left += offset/2;
	rcLabel.right -= offset;

	pDC->DrawText(sLabel,-1,rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	// Highlight only first column
	if (mHighlight == HIGHLIGHT_NORMAL) {
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	}

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);

	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++) {
		rcCol.left = rcCol.right;
		rcCol.right += lvc.cx;

		// Draw the background if needed
		if (mHighlight == HIGHLIGHT_NORMAL)
			pDC->FillRect(rcCol, &CBrush(::GetSysColor(COLOR_WINDOW)));

		sLabel = getString(nItem, nColumn);
		if (sLabel.GetLength() == 0)
			continue;

		// Get the text justification	
		UINT nJustify = DT_LEFT;
		switch (lvc.fmt & LVCFMT_JUSTIFYMASK) {
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;

		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;

		default:
			break;
		}

		rcLabel = rcCol;
		rcLabel.left += offset;
		rcLabel.right -= offset;

		pDC->DrawText(sLabel, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
	}

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);

	// Restore dc
	pDC->RestoreDC(nSavedDC);
}

void COOFListCtrl::RepaintSelectedItems()
{
	CRect rcBounds, rcLabel;

	// Invalidate focused item so it can repaint 
	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if (nItem != -1) {
		GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcBounds.left = rcLabel.left;

		InvalidateRect(rcBounds, FALSE);
	}

	// Invalidate selected items depending on LVS_SHOWSELALWAYS
	if (!(GetStyle() & LVS_SHOWSELALWAYS)) {
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcBounds.left = rcLabel.left;

			InvalidateRect(rcBounds, FALSE);
		}
	}

	UpdateWindow();
}

COOFListCtrl::EHighlight COOFListCtrl::setHighlightType(EHighlight newHighlight)
{
	EHighlight oldHighlight = mHighlight;

	if (newHighlight <= HIGHLIGHT_ROW) {
		mHighlight = newHighlight;

		// set up the correct window style
		LONG wndStyle = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);

		wndStyle &= ~LVS_TYPEMASK;
		wndStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, wndStyle);

		Invalidate();
	}

	return oldHighlight;
}

char* COOFListCtrl::getString(int row, int subItem) const
{
	if (isItemCachingEnabled())
		return mItemCache->getString(row, subItem, this);
	else
		return mItemCache->getStringFromView(*mView, row, subItem);
}

BEGIN_MESSAGE_MAP(COOFListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(COOFListCtrl)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OOFILE_CHANGESEL, OnOOFileChangeSel)
	ON_MESSAGE(WM_OOFILE_ACCEPTDROP, OnOOFileAcceptDrop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFListCtrl message handlers

void COOFListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uOldState == 0 && pNMListView->uNewState & LVIS_FOCUSED &&
		mSingleClickMessage != 0)
	{	// can't call gui->msgForOOFILE(mSingleClickMessage, this) directly, because
		// possible confirmation dialog would change the message order (OnItemChanged()
		// would be called twice for item focus change, thus displaying the dialog twice
		// as well)
		// IRRELEVANT COMMENT ANYWAY - it is entirely within the user's rights to have
		// a singleClickMessage which is NOT an OOFILE message.
		PostMessage(WM_OOFILE_CHANGESEL);
	}

	*pResult = 0;
}

LONG COOFListCtrl::OnOOFileChangeSel(UINT wParam, LONG lParam)
{
	if (mSingleClickMessage!=OOFmsg_None)
		dbEditHelper::postFrameworkMessage(mSingleClickMessage, (WPARAM)m_hWnd);
	return 0;
}

LONG COOFListCtrl::OnOOFileAcceptDrop(UINT wParam, LONG lParam)
{
	switch (mNumID) {
	case 0:			// accept everything
		return 0;
	case 1:			// should cover most cases
		return mDropID != (long)wParam;
	default:
		// look up the ID in arrayID
		for (unsigned int i=0; i<mNumID; i++) {
			if (mArrayID[i] == (long)wParam)
				return 0;
		}
		return 1;
	}
}

void COOFListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	dbGUI* gui = getGUI();

	if (gui) {
		switch (nChar) {
		case VK_DELETE:
//			gui->msgForOOFILE(cmd_dbDel, this);
				::MessageBeep(0);
		break;
		case VK_INSERT:
//			gui->msgForOOFILE(cmd_dbNew);
			::MessageBeep(0);
			break;
		default:
			CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
	else  // no GUI 
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COOFListCtrl::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (mDoubleClickMessage != OOFmsg_None) {
		const int singleSel = GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
		if (singleSel!=-1)
			dbEditHelper::postFrameworkMessage(mDoubleClickMessage, (WPARAM)m_hWnd);
	}
	*pResult = 0;
}

void COOFListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_ITEM* pItem = &((LV_DISPINFO*)pNMHDR)->item;

	if (pItem->mask & LVIF_TEXT)
		pItem->pszText = getString(pItem->iItem, pItem->iSubItem);

	*pResult = 0;
}

void COOFListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);

	// check if we are losing focus to label edit box
	if (pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;

	// repaint items that should change appearance
	if ((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}

void COOFListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);

	// check if we are getting focus from label edit box
	if (pOldWnd!=NULL && pOldWnd->GetParent()==this)	
		return;

	// repaint items that should change appearance
	if ((GetStyle() & LVS_TYPEMASK)==LVS_REPORT)	
		RepaintSelectedItems();
}

void COOFListCtrl::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if (mHighlight == HIGHLIGHT_ROW && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT) {
		CRect rcBounds;
		GetItemRect(0, rcBounds, LVIR_BOUNDS);

		CRect rcClient;
		GetClientRect(&rcClient);

		if (rcBounds.right < rcClient.right)
		{
			CPaintDC dc(this);
			CRect rcClip;
			dc.GetClipBox(rcClip);
			rcClip.left = min(rcBounds.right-1, rcClip.left);
			rcClip.right = rcClient.right;
			InvalidateRect(rcClip, FALSE);
		}
	}

	CListCtrl::OnPaint();
}

void COOFListCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (mCommandID != -1) {
		// create global memory for sharing dragdrop data
		HGLOBAL hgData = GlobalAlloc(GPTR, sizeof(struct DropStruct));
		ASSERT(hgData!=NULL);

		struct DropStruct* dropStruct = (struct DropStruct*)GlobalLock(hgData);
		ASSERT(dropStruct != NULL);

		dropStruct->commandID = mCommandID;
		dropStruct->commandTarget = mCommandTarget;

		// cache the data, and initiate DragDrop
		mOleDataSource.CacheGlobalData(CF_TEXT, hgData);           

		DROPEFFECT dropEffect = mOleDataSource.DoDragDrop(DROPEFFECT_COPY, NULL);

		// Clear the Data Source's cache
		mOleDataSource.Empty();        
	}

	*pResult = 0;
}


void 
COOFListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsTypingChar(nChar))
		AddChar(nChar);
	else
		CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}


void 
COOFListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==mNavListTimerID) {
// react to timer by searching for string
		unsigned long zeroBasedRowNum;
		const int numItems = GetItemCount();
		if (FindRowforString(mView, zeroBasedRowNum)) {
				if (numItems > 0) {
//					SetItemState(zeroBasedRowNum, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
					EnsureVisible(zeroBasedRowNum, FALSE);
				}
		}
		else  // didn't find so scroll to end
				EnsureVisible(numItems -1, FALSE);		
		ResetTimer();
	}
}


/////////////////////////////////////////////////////////////////////////////
// COOFTreeCtrl

// disable Warning C4355: "'this' used in base member initializer list."
#pragma warning( disable : 4355 )

COOFTreeCtrl::COOFTreeCtrl() :
	OOF_NavigableList(this),
	mSingleClickMessage(0),
	mDoubleClickMessage(0),
	mCommandID(-1),
	mCommandTarget(NULL),
	mImgList(NULL),
	mIgnoreChangedSelection(FALSE),
	mNumID(0),
	mArrayID(NULL),
	mDropID(0)
{
}

#pragma warning( default : 4355 )

COOFTreeCtrl::~COOFTreeCtrl()
{
	if (mImgList != NULL)
		delete mImgList;

	if (mArrayID != NULL)
		delete[] mArrayID;
}

void
COOFTreeCtrl::initializeSpace()
{
	BeginWaitCursor();

	ASSERT(mHierLevels != NULL);
	dbTable* table = viewAtLevel(0)->table();

	unsigned long savedRecNo = table->recordNumber();

	{	// store current selection in table
		stSaveSelection saveSelection(table);

		unsigned long count = table->count();

		if (count > ProgressDlgThreshold) {
#ifdef OOFMFC_STATIC
			// set the current resource handle into OOFile resource DLL
			// (DLL version of OOFMFC has linked resources with it, so there
			// is no need to set the resource handle)
			SetOOFileResources setRes;
#endif
			// progress dialog will be displayed
			COOFProgressDlg* progressDlg = new COOFProgressDlg(NULL);
			// create and initialize progress dialog
			CString s;
			VERIFY(s.LoadString(IDS_INITTREECTRL));
			VERIFY(progressDlg->Create(count, s));
			// fill tree control with empty items
			FillItems(count, table, progressDlg);
			// finish progress dialog
			progressDlg->DestroyWindow();
		}
		else {
			FillItems(count, table, NULL);
		}
	}	// stored selection is restored here

	table->gotoRecord(savedRecNo);

	EndWaitCursor();
}

void
COOFTreeCtrl::FillItems(unsigned long count, dbTable* table, COOFProgressDlg* progressDlg)
{
	TV_INSERTSTRUCT	insertDesc;
	ZeroMemory(&insertDesc, sizeof(TV_INSERTSTRUCT));

	insertDesc.hParent		= TVI_ROOT;
	insertDesc.hInsertAfter	= TVI_LAST;

	insertDesc.item.mask		= TVIF_TEXT|TVIF_CHILDREN|TVIF_PARAM;
	insertDesc.item.pszText		= LPSTR_TEXTCALLBACK;
	insertDesc.item.cChildren	= mHierLevels->count() > 0 ? 1 : 0;

	if (progressDlg != NULL) {
		for (unsigned long i=0; i<count; i++) {
			insertDesc.item.lParam = table->oidOfRecord(i);
			VERIFY(InsertItem(&insertDesc) != NULL);
			// update progress bar position
			if (i%100 == 0)
				progressDlg->setPos(i);
		}
		progressDlg->setPos(count);
	}
	else {
		for (unsigned long i=0; i<count; i++) {
			insertDesc.item.lParam = table->oidOfRecord(i);
			VERIFY(InsertItem(&insertDesc) != NULL);
		}
	}
}

void
COOFTreeCtrl::adjustNumItems()
{
}

BOOL
COOFTreeCtrl::SetNewImageList(UINT bitmapID)
{
	if (mImgList != NULL)
		delete mImgList;

	mImgList = new CImageList();

	if (mImgList->Create(bitmapID, 16, 0, 0xFFFFFFFF)) {
		SetImageList(mImgList, TVSIL_NORMAL);
		return TRUE;
	}
	else
		return FALSE;
}

void
COOFTreeCtrl::highlightRecNo(unsigned long recNo, BOOL /*bSelect*/)
{
	HTREEITEM item = GetRootItem();

	if (item == NULL)
		return;

	for (unsigned int i=0; i<recNo; i++) {
		item = GetNextSiblingItem(item);
		ASSERT(item != NULL);
	}

	ignoreChangedSelection();
	SelectItem(item);
	ignoreChangedSelection(FALSE);
}

unsigned long
COOFTreeCtrl::highlightedRecNo() const
{
	HTREEITEM item = GetFirstSelectedItem();

	if (item == NULL)
		return 0;

	HTREEITEM topLevel;

	do {
		topLevel = item;
		item = GetParentItem(item);
	} while (item != NULL);

	item = GetRootItem();

	int ind;
	for (ind=0; topLevel != item; ind++) {
		item = GetNextSiblingItem(item);
		ASSERT(item != NULL);
	}

	return ind;
}

oidT
COOFTreeCtrl::highlightedOID() const
{
	HTREEITEM hItem = GetFirstSelectedItem();
	return hItem != NULL ? GetItemOID(hItem) : 0;
}

dbGUI*
COOFTreeCtrl::getGUI() const
{
	dbView* topView = level(0)->view();
	return topView != NULL ? topView->table()->getGUI() : NULL;
}

dbView*
COOFTreeCtrl::viewAtLevel(unsigned short inLevel) const
{
	dbHierLevel* theLevel = level(inLevel);
	assert(theLevel);
	dbView* ret = theLevel->view();
	return ret;
}

unsigned short
COOFTreeCtrl::GetIndentLevel(HTREEITEM item) const
{
	unsigned short indentLevel = 0;
	HTREEITEM parentItem = GetParentItem(item);

	while (parentItem != NULL) {
		indentLevel++;
		parentItem = GetParentItem(parentItem);
	}

	return indentLevel;
}

unsigned short
COOFTreeCtrl::highlightedIndentLevel() const
{
	return GetIndentLevel(GetFirstSelectedItem());
}

dbViewOutlineItem
COOFTreeCtrl::getItemDesc(HTREEITEM item) const
{
	if (item != NULL)
		return dbViewOutlineItem(item, GetIndentLevel(item), GetItemOID(item));
	else
		return dbViewOutlineItem::nullItem();
}

unsigned long
COOFTreeCtrl::InsertSubItems(unsigned short indentLevel, oidT oid, HTREEITEM hTreeItem)
{
	dbHierLevel* superLevel = level(indentLevel);
	dbTable* superTable = superLevel->view()->table();
	dbSelection* savedTopSelection = new dbSelection(superTable->currentSelection());
	unsigned long savedRecNo = superTable->recordNumber();
	unsigned long itemIndex = 0;

	assert(superLevel);

	dbTable* addingTable = superLevel->selectNestedItemsFor(oid);
	unsigned short nestedIndentLevel = indentLevel+1;

	if (addingTable) {	// we are not yet at bottom!
		unsigned long numToInsert = addingTable->count();
		dbView* nestedView = viewAtLevel(nestedIndentLevel);
		dbTable* nestedTable = nestedView->table();
		stSaveSelection saveNestedSel(nestedTable);

		if (nestedTable != addingTable) // only == if used related table in view
			nestedTable->setSelection(addingTable->currentSelection());

		nestedTable->start();

		TV_INSERTSTRUCT insertDesc;
		ZeroMemory(&insertDesc, sizeof(TV_INSERTSTRUCT));

		insertDesc.hParent		= hTreeItem;
		insertDesc.hInsertAfter	= TVI_LAST;

		insertDesc.item.mask		= TVIF_TEXT|TVIF_CHILDREN|TVIF_PARAM;
		insertDesc.item.pszText		= LPSTR_TEXTCALLBACK;
		insertDesc.item.cChildren	= (unsigned long)nestedIndentLevel+1 < mHierLevels->count() ? 1 : 0;

		for (itemIndex = 0; itemIndex < numToInsert; itemIndex++) {
			insertDesc.item.lParam = nestedTable->oidOfRecord(itemIndex);
			VERIFY(InsertItem(&insertDesc) != NULL);
			nestedTable->next();
		}	// add loop
	} // bottom of hierarchy

	if (savedTopSelection && superTable) {
		superTable->setSelection(*savedTopSelection);
		superTable->gotoRecord(savedRecNo);
		delete savedTopSelection;
	}

	return itemIndex;
}

void
COOFTreeCtrl::LoadRow(dbView* view, char* buf, int bufLen)
{
//Andy 980501 - never seems to be called, see OnGetDispInfo instead
	view->start();

	for (int col = 0; view->more(); col++) {
		dbField* theField = (dbField*)(view->item(col)); 		  // safe downcast
		oofString str(theField->copyString());
		if (col > 0)
			strcat(buf, ", ");
		strcat(buf, str);
		view->next();
	}
}

void COOFTreeCtrl::enableDragDrop(long int commandID, CWnd* commandTarget)
{
	ASSERT(commandID != -1);
	ASSERT(commandTarget != NULL);

	mCommandID		= commandID;
	mCommandTarget	= commandTarget;
}

void COOFTreeCtrl::acceptDrop(long ID)
{
	long tmp = ID;
	acceptDrop(1, &tmp);
}

void 
COOFTreeCtrl::acceptDrop(unsigned int numID, long* arrayID)
{
// copies of this routine exist in COOFTreeCtrl, COOFListCtrl and COOFListBox and we need to use a mixin
// may be called multiple times and hence need to preserve old data
	ASSERT(numID);
	if (mNumID) {  // has existing values
		const unsigned int newNum = numID + mNumID;
		long* newArray = new long[newNum];
		ASSERT(newArray);
		if (mNumID==1) {  // old value in mDropID
			newArray[0] = mDropID;
		}
		else {  // array to copy
			memcpy(newArray, mArrayID, mNumID * sizeof(long));
			delete[] mArrayID;
		}  // 1 or many old values
		memcpy(&newArray[mNumID], arrayID, numID * sizeof(long));  // append incoming data
		mArrayID = newArray;
		mNumID = newNum;
// don't need to register a second time		
	}  // preserved old value(s)
	
	else {
		if (numID==1) {
			const long onlyID = arrayID[0];
			if (onlyID==0)  {  // probably default case from single param
				mDropID = mNumID = 0;	// not really any data, just the default
			}
			else {
				mDropID = onlyID;
				mNumID = 1;
			}
		} 
		else
			mNumID = numID;
	
		if (mNumID > 1) {
			mArrayID = new long[mNumID];
			ASSERT(mArrayID != NULL);
			memcpy(mArrayID, arrayID, mNumID * sizeof(long));
		}
	
		// register the COleDropTarget object with the OLE dll's
		BOOL success = mDropTarget.Register(this);
	
		if (!success)
			MessageBox("Ole Register Drop Target Failed");     
	} // first time called
}

dbHierLevel*
COOFTreeCtrl::level(unsigned short inLevel) const
{
	return (dbHierLevel*)mHierLevels->value(inLevel);
}

void
COOFTreeCtrl::setHierLevels(OOF_ExpandableLongArray* hierLevels)
{
	mHierLevels = hierLevels;
}

BOOL
COOFTreeCtrl::SetDefaultImageList()
{
/*
#ifdef OOFMFC_STATIC
	// set the current resource handle into OOFile resource DLL
	// (DLL version of OOFMFC has linked resources with it, so there
	// is no need to set the resource handle)
	SetOOFileResources setRes;
#endif
	return SetNewImageList(IDB_TREEICONS);
	*/
	SetImageList(NULL, TVSIL_NORMAL);	// for now, no icons
	return true;
}

bool
COOFTreeCtrl::isLeafNode(HTREEITEM item) const
{
	return item != NULL ? GetIndentLevel(item)+1 == (unsigned short)mHierLevels->count() : false;
}

BEGIN_MESSAGE_MAP(COOFTreeCtrl, CTreeCtrlEx)
	//{{AFX_MSG_MAP(COOFTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeyDownReflect)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OOFILE_ACCEPTDROP, OnOOFileAcceptDrop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFTreeCtrl message handlers

void COOFTreeCtrl::OnKeyDownReflect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	const char nChar = pTVKeyDown->wVKey;
	const UINT nRepCnt = 1;
	const UINT nFlags =  pTVKeyDown->flags;

	dbGUI* gui = getGUI();

	if (gui) {
		switch (nChar) {
		case VK_DELETE:
//			gui->msgForOOFILE(cmd_dbDel, this);
				::MessageBeep(0);
		break;
		case VK_INSERT:
//			gui->msgForOOFILE(cmd_dbNew);
			::MessageBeep(0);
			break;
		default:
			CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
	else  // no GUI 
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	*pResult = 0;
}

void COOFTreeCtrl::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM& itemNew = pNMTreeView->itemNew;

	if (!mIgnoreChangedSelection &&
		mSingleClickMessage!=OOFmsg_None &&
		itemNew.state & TVIS_SELECTED) // &&
//		isLeafNode(itemNew.hItem))
	{
			dbEditHelper::postFrameworkMessage(mSingleClickMessage, (WPARAM)m_hWnd);
	}
	*pResult = 0;
}

void COOFTreeCtrl::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (mDoubleClickMessage != OOFmsg_None && isLeafNode(GetSelectedItem()))
		dbEditHelper::postFrameworkMessage(mDoubleClickMessage, (WPARAM)m_hWnd);
	*pResult = 0;
}

void COOFTreeCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_ITEM* pItem = &((TV_DISPINFO*)pNMHDR)->item;

	static char buf[BufSize];

	if (pItem->mask & TVIF_TEXT) {
		int indentLevel = GetIndentLevel(pItem->hItem);
		dbView*  view = viewAtLevel(indentLevel);
		dbTable* table = view->table();

		unsigned long savedRecNo = table->recordNumber();

		{	// store current selection in table
			stSaveSelection saveSelection(table);

			oidT oid = GetItemOID(pItem->hItem);
			// current item
			table->selectJustOID(oid);
			table->start();
			// text description
			const dbField& firstField = view->field(0);
			oofString itemText(firstField.copyString());
			const unsigned int numFields = view->count();
			const unsigned short kSepSpaces = 2;
			unsigned long accumulatedWidth = firstField.fieldUsualWrapWidth();
			for (unsigned int i=1; i<numFields; i++) {
// doesn't work - can't use tabstop style				itemText += '\t';  // use tabs - rely on list being tab-stopped
				const dbField& theField = view->field(i);
				accumulatedWidth += kSepSpaces;  
				itemText.padTo(accumulatedWidth);  // includes kSepSpaces allowance for min spaces between fields
				itemText += theField.copyString();
				accumulatedWidth += theField.fieldUsualWrapWidth(); // not the most efficient, recalc this each time! 
				// NOT YET IMPLEMENTED - cache these wrap lengths
			}
			ASSERT(BufSize > itemText.length());
			strcpy(buf, itemText);
			pItem->pszText = buf;
		}	// stored selection is restored here

		table->gotoRecord(savedRecNo);
	}

	*pResult = 0;
}

void COOFTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (pNMTreeView->action == TVE_EXPAND) {
		// tree item handler
		HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
		// selected item's OID
		oidT oid = GetItemOID(hTreeItem);
		// ... and indent level
		unsigned short indentLevel = GetIndentLevel(hTreeItem);
		// insert subitems
		if (InsertSubItems(indentLevel, oid, hTreeItem) == 0) {
			TV_ITEM itemDesc;
			ZeroMemory(&itemDesc, sizeof(TV_ITEM));

			itemDesc.hItem = hTreeItem;
			itemDesc.mask = TVIF_CHILDREN;
			itemDesc.cChildren = 0;

			VERIFY(SetItem(&itemDesc));
		}
	}
	else if (pNMTreeView->action == TVE_COLLAPSE) {
		// collapse the selected node and clean up used memory
		VERIFY(Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE|TVE_COLLAPSERESET));
	}

	*pResult = 0;
}

LONG COOFTreeCtrl::OnOOFileAcceptDrop(UINT wParam, LONG lParam)
{
	switch (mNumID) {
	case 0:			// accept everything
		return 0;
	case 1:			// should cover most cases
		return mDropID != (long)wParam;
	default:
		// look up the ID in arrayID
		for (unsigned int i=0; i<mNumID; i++) {
			if (mArrayID[i] == (long)wParam)
				return 0;
		}
		return 1;
	}
}

void COOFTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (mCommandID != -1) {
		// create global memory for sharing dragdrop data
		HGLOBAL hgData = GlobalAlloc(GPTR, sizeof(struct DropStruct));
		ASSERT(hgData!=NULL);
	
		struct DropStruct* dropStruct = (struct DropStruct*)GlobalLock(hgData);
		ASSERT(dropStruct != NULL);
	
		dropStruct->commandID = mCommandID;
		dropStruct->commandTarget = mCommandTarget;
	
		// cache the data, and initiate DragDrop
		mOleDataSource.CacheGlobalData(CF_TEXT, hgData);
	
		DROPEFFECT dropEffect = mOleDataSource.DoDragDrop(DROPEFFECT_COPY, NULL);
	
		// Clear the Data Source's cache
		mOleDataSource.Empty();
	}

	*pResult = 0;
}


void 
COOFTreeCtrl::OnTimer(UINT nIDEvent)
{

	if (nIDEvent==mNavListTimerID) {
// react to timer by searching for string
		dbView* theView = viewAtLevel(0);
		if (theView) {
			oidT theOID;
			if (FindOIDforString(theView, theOID)) {
	//  scan the list to find the OID
				HTREEITEM hItem = GetRootItem();
				while ( hItem) {
					HTREEITEM hLastItem = hItem;
					hItem = GetNextVisibleItem( hItem );
					if (hItem) {
						if (GetItemData(hItem)==theOID) {
							EnsureVisible(hItem);
							break;	// processed item!
						}
					} // got an item
					else {  // select last
						EnsureVisible(hLastItem);
					}
				} // loop items
			} // got OID
		}  // view to get OID in 
		ResetTimer();
	}
}

void 
COOFTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsTypingChar(nChar))
		AddChar(nChar);
	else
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
// OOF_NavigableList mixin for list classes to react to keypresses

OOF_NavigableList::OOF_NavigableList(CWnd* ourSubclass) :
	mWndSubclass(ourSubclass),
	mTimerActive(false),
	mNavListTimerID(1)	// HACK JUST USE ONE FOR NOW
{
// NOT YET IMPLEMENTED - way to get a timer ID
}


OOF_NavigableList::~OOF_NavigableList()
{
	ResetTimer();
}


void 
OOF_NavigableList::AddChar(char inCh)
{
	if (!mTimerActive) {
		if (mWndSubclass->SetTimer(mNavListTimerID, 500 /*milliseconds - half a second*/, NULL))
			mTimerActive = true;
		else	{
			mWndSubclass->MessageBox(_T("Not enough timers available for this window."),
					_T("OOF_NavigableList::AddChar"), MB_ICONEXCLAMATION | MB_OK);
			return;
		}  // failed to set timer
	} // no timer active
	// if get here have active timer
	mAccumulatedString += inCh;
}


BOOL 
OOF_NavigableList::IsTypingChar(char inCh) const
{
	BOOL ret = (inCh>=' ') && (inCh<='~');
	return ret;	
}


void 
OOF_NavigableList::ResetTimer()
{
	mAccumulatedString = 0;
	if (mTimerActive) {
		mWndSubclass->KillTimer(mNavListTimerID);
		mTimerActive = false;
	}
}


BOOL
OOF_NavigableList::FindOIDforString(dbView* inView, oidT& outOID)
{
	BOOL foundIt = false;
	dbTable* viewTable = inView->table();
	if (viewTable) {  // legal to have views with no table - they behave as if 1 record
		if (mAccumulatedString.length()) {
			const dbField* searchField = &inView->defaultFindField();	
			dbQueryLiteral* rhsLiteral = 0;
			dbQueryField* lhsField = new dbQueryField(searchField);	
			assert(lhsField);
			if (searchField->fieldType() == charField || searchField->fieldType() == compoundField) {
				rhsLiteral = new dbQueryLiteralStr(mAccumulatedString);
				dbQueryClause* theSearch = new dbQueryBinary(
					lhsField, 
					dbQueryClause::greaterThanOrEqual, 	/* want to get next cell after if no match */
					rhsLiteral
				);
				stSaveSelection saveView(viewTable);
				viewTable->search(theSearch);	// in hier tables, always destroying the context
				if (viewTable->count()) {
					viewTable->start();	// goto the first found record
					outOID = viewTable->currentOID();
					foundIt = true;
				}
				else {
					foundIt = false;
					outOID = 0;
				}
				delete theSearch;
			} // is a char field
			// NOT YET IMPLEMENTED - cope with others, see CQueryEngine::performSearch
		} // have data to find
	}
	else {
		// if no table, view behaves as if just one record
		outOID = viewTable->currentOID();
		foundIt = true;	
// NOT YET IMPLEMENTED - test the cells on solitary row and set foundIt & outRow=0 if not found
	}
	return foundIt;
}



BOOL
OOF_NavigableList::FindRowforString(dbView* inView, unsigned long& zeroBasedOutRow)
{
	bool foundIt = false;
	zeroBasedOutRow = 0;
	dbTable* viewTable = inView->table();
	if (viewTable) {
		if (mAccumulatedString.length()) {
			const dbField* searchField = &inView->defaultFindField();	
			dbQueryLiteral* rhsLiteral = 0;
			dbQueryField* lhsField = new dbQueryField(searchField);	
			assert(lhsField);
			if (searchField->fieldType() == charField || searchField->fieldType() == compoundField) {
				rhsLiteral = new dbQueryLiteralStr(mAccumulatedString);
				dbQueryClause* theSearch = new dbQueryBinary(
					lhsField, 
					dbQueryClause::greaterThanOrEqual, 	/* want to get next cell after if no match */
					rhsLiteral
				);
				zeroBasedOutRow = viewTable->recordNumber(theSearch, &foundIt);	// expect 0 if not found
				delete theSearch;
			} // is a char field
			// NOT YET IMPLEMENTED - cope with others, see CQueryEngine::performSearch
		} // have data to find
	}
	else {
		zeroBasedOutRow = 0;	// if no table, view behaves as if just one record
		foundIt = true;	
// NOT YET IMPLEMENTED - test the cells on solitary row and set foundIt & zeroBasedOutRow=0 if not found
	}
	return foundIt;
}

#endif   // !_Macintosh for doxygen
