// COPYRIGHT 1995 A.D. Software, All rights reserved

// MFC Integration layer of OOFILE database
#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofres.h"

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#include "oofmfcx.h"  // private interface, includes public
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#include "oofmfchr.h"

// static data
dbGUIFactoryMFC  theMFCFactory;		// Singleton	
const unsigned long kRecordLockedByOtherUser = 'Lock';

#ifdef OOFMFC_STATIC
HINSTANCE dbGUIFactoryMFC::mResourceDLL = NULL;
static char szResourceDLL[] = "OOFMFCRe.DLL";
#endif

#ifdef OOFMFC_STATIC
// -------------------------------------------------------
//           S e t O O F i l e R e s o u r c e s
// -------------------------------------------------------
SetOOFileResources::SetOOFileResources()
{
	mResourceHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(dbGUIFactoryMFC::getResourceHandle());
}

SetOOFileResources::~SetOOFileResources()
{
	ASSERT(mResourceHandle != NULL);
	AfxSetResourceHandle(mResourceHandle);
}
#endif


// -------------------------------------------------------
//                        d b G U I
// -------------------------------------------------------
bool 
dbGUI::msgForOOFILE(
	const MessageT	inMessage,
	void* ioParam)
{
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() 
		<< "dbGUI::msgForOOFILE(" 
		<< PPMessage(inMessage) << ", "
		<< hex << ioParam << ")\ttable: "
		<< mTable->tableName() << '\t' << (void*)mTable
		<< endl;
	dbConnect::endDebugWrite();
#endif

	bool ret = true; // we've handled the command
	switch (inMessage) {
	
	case IDOK:			// msg_OK
		if (leaveRecord())	// implies save
			mClosing = true;

		break;

	case IDCANCEL:		// msg_Cancel
		mTable->unlockRecord();
		mClosing = true;
		break;

	case ID_APP_EXIT:	// cmd_Quit
		ret = !leaveRecord();	// 0 / 1 - allow / disallow exit
		break;

	case ID_FILE_CLOSE:	// cmd_Close
		break;

	case cmd_dbNew:
		newRecord();
		break;

	case cmd_dbClone:
		cloneRecord();
		break;

	case cmd_dbFirst:
		goFirstRecord();
		break;

	case cmd_dbPrev:
		goPrevRecord();
		break;

	case cmd_dbNext:
		goNextRecord();
		break;

	case cmd_dbLast:
		goLastRecord();
		break;

	case cmd_dbEdit:	// want to go to a new record
		{
			dbAbstractBrowseHelper* handlesList = GetDisplayTableHandler(ioParam);
			if (handlesList)
				goHighlightedRecord(handlesList);
			else
				ret = false;	// ***** we don't have a handler for this display table
		}
		break;

	case cmd_dbSave:
		saveRecord();
		break;

	case cmd_dbRevert:
		revertRecord();
		break;

	case cmd_dbDel:
		if (ConfirmDelete()) 
			deleteRecord();
		return true;
/*
AD 990425 keep the following complex implementation as it is richer than the Mac one
and some time in future we will implement like this (see 
		{
			dbAbstractBrowseHelper* browseHelper = GetDisplayTableHandler(ioParam);

#ifdef OOFMFC_STATIC
		// set the current resource handle into OOFile resource DLL
		// (DLL version of OOFMFC has linked resources with it, so there
		// is no need to set the resource handle)
		SetOOFileResources setRes;
#endif
			if (browseHelper != NULL) {
				dbSelection selection(browseHelper->highlightedSelection());
				switch (selection.count()) {
				case 0:
					break;
				case 1:
					// can't use dbTable::isEmpty() for there is always a record
					if (!mTable->isNewRecord() && ConfirmDelete(IDS_DELETERECORD)) {
						// deleteRecord is called instead of deleteSelection from two reasons:
						// first, it is slightly more effective, second, deleteRecord must
						// be present and working in case there is no browser in a user dialog
						// (else branch of the main if)
						deleteRecord();
						browseHelper->updateBrowserSelection();
					}
					break;
				default:
					if (ConfirmDelete(IDS_DELETESELECTION)) {
						browseHelper->deleteSelection(selection);
						browseHelper->highlightNothing();
						gotoCurrentOrNewRec();
						browseHelper->updateBrowserSelection();
					}
					break;
				}
			}
			else {
				// there is no browser - just delete the current record
				if (!mTable->isNewRecord() && ConfirmDelete(IDS_DELETERECORD)) {
					deleteRecord();
				}
			}

			return true;
		}  // cmd_dbDel
*/
		
	case cmd_dbBye:	// user's close or leave button wants to know if it can go
		if (!leaveRecord())
			ret = false;	// as it is a test, we return false if can't leave
							// the command won't be handled further up the hierarchy so will remain false
		break;

	case cmd_dbLockedOnEntry: 
		oofYNC("Someone else is editing this record - you will not be able to make any changes");
		break;

	default:
		ret = false;
	}

	return ret;
}


void 
dbGUI::goHighlightedRecord(dbAbstractBrowseHelper* inBrowser)
{
	assert(mTable);
	long recNo = inBrowser->highlightedRecNo();
	unsigned long currRec = mTable->recordNumber();

	if (recNo < 0) {
		inBrowser->highlightRecNo(currRec);
		recNo = currRec;
	}

	assert(recNo >= 0);

	if ((unsigned long)recNo != currRec || mTable->isNewRecord()) {
		if(leaveRecord()) {
			mTable->gotoRecord(recNo);
			changedSelection(inBrowser);	// inBrowser skipped - already knows!
		}
		else if (mTable->isNewRecord())
			inBrowser->highlightNothing();
		else {
			// user cancelled selection -
			// clear the cancelled selection (list controls with multiple
			// selection would leave it selected)
			inBrowser->highlightRecNo(recNo, FALSE);
			// highlight original rec
			inBrowser->highlightRecNo(currRec);
		}
	}
}


void
dbGUI::changedSelection(oofReceiver* dontNeedToTell)
{
	bool someoneElseLocked = false;
	try {
		mTable->beginTransaction();
		mTable->lockRecord();
	}
	catch (oofE_RecordLocked&) {
//		oofYNC("Someone else is editing this record - you will not be able to make any changes");
		someoneElseLocked = true;
	}
	if (someoneElseLocked)  { 
		mTable->abortTransaction();
		dbEditHelper::postFrameworkMessage(cmd_dbLockedOnEntry);  // feel safer doing this outside catch block
		broadcast(OOFmsg_ChangeSelection, kRecordLockedByOtherUser, dontNeedToTell);
	}
	else 
		broadcast(OOFmsg_ChangeSelection, 0, dontNeedToTell);
}


bool
dbGUI::isOOFILEcommandEnabled(long /*commandID*/, Boolean& /*outEnabled*/) const  // use in findCommandStatus in PowerPlant apps
{
//  NOT YET IMPLEMENTED	
	return true;
}


bool 
dbGUI::isOOFILEmsg(const MessageT	inMessage)
{
// constants in oofpp.h and oofmfc.h as vary depending on framework
	bool ret = (inMessage >= kFirstOOFILEcmd && inMessage <= kLastOOFILEcmd);
	return ret;
}


// -------------------------------------------------------
//             d b G U I F a c t o r y M F C
// -------------------------------------------------------
dbGUIFactoryMFC::dbGUIFactoryMFC() : dbGUIFactory()
{
#ifdef OOFMFC_STATIC
	// load resources when OOFMFC is a static library
	// DLL version of OOFMFC has resources linked with it
	ASSERT(mResourceDLL == NULL);	// just one MFC GUI factory can be defined
	mResourceDLL = ::LoadLibrary(szResourceDLL);
	ASSERT(mResourceDLL != NULL);
#endif
}

dbGUIFactoryMFC::~dbGUIFactoryMFC()
{
#ifdef OOFMFC_STATIC
	VERIFY(::FreeLibrary(mResourceDLL));
#endif
}

dbGUI*  
dbGUIFactoryMFC::concreteMakeGUI(dbTable* inTable) 
{
	return new dbGUI(inTable);
}


// -------------------------------------------------------
//        d b B r o w s e H e l p e r
// -------------------------------------------------------

dbBrowseHelper::dbBrowseHelper(dbView* adoptedView) :
	dbAbstractBrowseHelper(adoptedView->table()),
	mBrowsingView(adoptedView),
	mBrowserDisplay(0)
{
	// for now headings are handled by control and we always
	// suppose to have no heading
	adoptedView->suppressHeadings();

	if (adoptedView->willSuppressHeadings())
		mHeadingRows = 0;
	else
		mHeadingRows = 1;	// default, for now only allow one row
}


dbBrowseHelper::~dbBrowseHelper() 
{
	delete mBrowsingView;
	delete mBrowserDisplay;
}


bool 
dbBrowseHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret = true;
	switch (msg) {

	case OOFmsg_DeleteRecord : {	// senderDefined is the record number in the selection
		dbConnect::raise("OOFmsg_DeleteRecord sent to dbBrowseHelper");  // not sure if we cope yet???
		dbTable* ourTable = mBrowsingView->table();
		assert(ourTable);
		ourTable->dropRecordFromSelection(senderDefined);	// assumes ourTable and sender have same sort order
		assert(mBrowserDisplay);
		mBrowserDisplay->deleteRow(senderDefined);
		break;
	}

	case OOFmsg_GetDisplayTableHandler :
		assert(mBrowserDisplay);
		ret = mBrowserDisplay->displayUsesControl((void*)senderDefined);
		break;	
		
	default :
		ret = dbAbstractBrowseHelper::receiveMsg(msg, senderDefined);
	};
	return ret;
}


void 
dbBrowseHelper::adoptView(dbView* theView)
{
	if (mBrowsingView)
		if (mBrowserDisplay)
			mBrowserDisplay->changeViewTo(theView);
	delete mBrowsingView;
	mBrowsingView = theView;
}


void 
dbBrowseHelper::browseViewWithListBox(COOFListBox* inLB)
{
// NOT YET IMPLEMENTED - SUPPORT HEADINGS
#ifdef OOF_DEBUG
	if (mBrowserDisplay)
		dbConnect::raise("dbBrowseHelper::BrowseViewWithListBox replacing current browser display");
#endif
	CheckControlStyle(inLB);
	delete mBrowserDisplay;
	mBrowserDisplay = new dbListBoxDisplay(inLB, mBrowsingView);
	dbTable* tbl = mBrowsingView->table();
	dbRelRefBase* tableRel = tbl->controllingRelationship();
	if (tableRel) {
		OOF_RelMN* MNlink = tableRel->getMNlink();
		if (MNlink)	// is MN relationship
			MNlink->makeRelMaintainer();	// auto-link MN relationship
	}
	LoadBrowseData();
	if (tbl->count())
		highlightRecNo(tbl->recordNumber());
}


void 
dbBrowseHelper::browseViewWithListCtrl(COOFListCtrl* inLBCtrl)
{
#ifdef OOF_DEBUG
	if (mBrowserDisplay)
		dbConnect::raise("dbBrowseHelper::BrowseViewWithListCtrl replacing current browser display");
#endif
	CheckControlStyle(inLBCtrl);
	delete mBrowserDisplay;
	mBrowserDisplay = new dbListCtrlDisplay(inLBCtrl, mBrowsingView);
	dbTable* tbl = mBrowsingView->table();
	dbRelRefBase* tableRel = tbl->controllingRelationship();
	if (tableRel) {
		OOF_RelMN* MNlink = tableRel->getMNlink();
		if (MNlink)	// is MN relationship
			MNlink->makeRelMaintainer();	// auto-link MN relationship
	}
	LoadBrowseData();

	RECT clientRect;
	inLBCtrl->GetClientRect(&clientRect);
	const LONG tableWidth = clientRect.right - clientRect.left;
	oofColSizer* sizer = mBrowsingView->colSizer();
	sizer->calculateWidths(tableWidth);	// divides up widths according to field defaults or user rules
	
	const unsigned short numCols = mBrowsingView->count();
	for (unsigned short i=0; i<numCols; i++) {
		unsigned long dataColWidth = sizer->width(i);
		inLBCtrl->SetColumnWidth(i, dataColWidth);	  
	}
	
	if (tbl->count())
		highlightRecNo(tbl->recordNumber());
}


void
dbBrowseHelper::CheckControlStyle(COOFListBox* inLB)
{
	if (inLB == NULL)
		return;

	DWORD wndStyle = inLB->GetStyle();

	if ((wndStyle & LBS_MULTICOLUMN)) {
		TRACE("Removing LBS_MULTICOLUMN flag from ListBox (not supported)\n");
		inLB->ModifyStyle(LBS_MULTICOLUMN, 0);
	}
	if ((wndStyle & LBS_SORT)) {
		TRACE("Removing sort flags from ListBox (not supported)\n");
		inLB->ModifyStyle(LBS_SORT|LBS_STANDARD, 0);
	}
	if (!(wndStyle & LBS_OWNERDRAWFIXED)) {
		// listbox virtualization is realized through owner-drawn flag
		TRACE("ListBox must have LBS_OWNERDRAWFIXED flag set on - please change the resource file\n");
		inLB->ModifyStyle(0, LBS_OWNERDRAWFIXED);
	}
}


void
dbBrowseHelper::CheckControlStyle(COOFListCtrl* inLBCtrl)
{
	if (inLBCtrl == NULL)
		return;

	DWORD wndStyle = inLBCtrl->GetStyle();

//	if (!(wndStyle & LVS_REPORT)) {
//		TRACE("Changing ListControl style to LVS_REPORT (only report style is supported for CListCtrl)\n");
//		inLBCtrl->ModifyStyle(LVS_ICON|LVS_LIST|LVS_SMALLICON, LVS_REPORT);
//	}
	if (!(wndStyle & LVS_SHOWSELALWAYS)) {
		TRACE("Changing ListControl style to LVS_SHOWSELALWAYS\n");
		inLBCtrl->ModifyStyle(0, LVS_SHOWSELALWAYS);
	}
	if ((wndStyle & LVS_SORTASCENDING) || (wndStyle & LVS_SORTDESCENDING)) {
		TRACE("Removing sort flags from ListControl (not supported)\n");
		inLBCtrl->ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING, 0);
	}
	if (wndStyle & LVS_EDITLABELS) {
		TRACE("Removing LVS_EDITLABELS flag from ListControl (not supported)\n");
		inLBCtrl->ModifyStyle(LVS_EDITLABELS, 0);
	}
//	inLBCtrl->ModifyStyleEx(0, LVS_EX_FULLROWSELECT);
	inLBCtrl->setHighlightType(COOFListCtrl::HIGHLIGHT_ALLCOLUMNS);
}

void
dbBrowseHelper::CheckControlStyle(COOFTreeCtrl* inTreeCtrl)
{
	if (inTreeCtrl == NULL)
		return;

	DWORD wndStyle = inTreeCtrl->GetStyle();
}


unsigned long 
dbBrowseHelper::highlightedRecNo() const
{
	assert(mBrowserDisplay);
	unsigned long clickedRow = mBrowserDisplay->highlightedRow() - mHeadingRows;
	return clickedRow;
}


void	
dbBrowseHelper::appendSelection(const dbSelection& inSel)
{
	dbTable* tbl = mBrowsingView->table();
	if (tbl->getGUI()->recordAvailable()) {
	#ifdef OOF_DEBUG_LOG_NAME
		dbConnect::startDebugWrite();
		dbConnect::debugLog() << "dbBrowseHelper::appendSelection" << endl;
		dbTable* viewTable = mBrowsingView->table();
		if (viewTable)
			viewTable->describeState(dbConnect::debugLog());
		dbConnect::endDebugWrite();
	#endif
	
		tbl->appendSelection(inSel);
	
	#ifdef OOF_DEBUG_LOG_NAME
		dbConnect::startDebugWrite();
		dbConnect::debugLog() << "dbBrowseHelper::appendSelection after append" << endl;
		if (viewTable)
			viewTable->describeState(dbConnect::debugLog());
		dbConnect::endDebugWrite();
	#endif
	
		refreshBrowser();
	}
	else
		::MessageBeep(MB_OK);
}


void
dbBrowseHelper::removeSelection(const dbSelection& inSel)
{
	dbTable* tbl = mBrowsingView->table();
	if (tbl->getGUI()->recordAvailable()) {
	#ifdef OOF_DEBUG_LOG_NAME
		dbConnect::startDebugWrite();
		dbConnect::debugLog() << "dbBrowseHelper::removeSelection" << endl;
		dbTable* viewTable = mBrowsingView->table();
		if (viewTable)
			viewTable->describeState(dbConnect::debugLog());
		dbConnect::endDebugWrite();
	#endif
	
		tbl->removeSelection(inSel);
	
	#ifdef OOF_DEBUG_LOG_NAME
		dbConnect::startDebugWrite();
		dbConnect::debugLog() << "dbBrowseHelper::removeSelection after remove" << endl;
		if (viewTable)
			viewTable->describeState(dbConnect::debugLog());
		dbConnect::endDebugWrite();
	#endif
	
		refreshBrowser();
	}
	else
		::MessageBeep(MB_OK);
}



void 
dbBrowseHelper::updateBrowserSelection() const
{
	highlightNothing();
	highlightRecNo(mBrowsingView->table()->recordNumber());
}


void	
dbBrowseHelper::clearHighlightedSelection()
{
	removeSelection(highlightedSelection());
}

void
dbBrowseHelper::clearSelection()
{
	dbTable* tbl = mBrowsingView->table();
	if (tbl->getGUI()->recordAvailable()) {
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() << "dbBrowseHelper::clearSelection" << endl;
	dbTable* viewTable = mBrowsingView->table();
	if (viewTable)
		viewTable->describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif

		tbl->clearSelection();
	
	#ifdef OOF_DEBUG_LOG_NAME
		dbConnect::startDebugWrite();
		dbConnect::debugLog() << "dbBrowseHelper::clearSelection after clearing" << endl;
		if (viewTable)
			viewTable->describeState(dbConnect::debugLog());
		dbConnect::endDebugWrite();
	#endif
	
		refreshBrowser();
	}
	else
		::MessageBeep(MB_OK);
}


dbSelection	
dbBrowseHelper::currentSelection() const
{
	return mBrowsingView->table()->currentSelection();
}


dbSelection 
dbBrowseHelper::highlightedSelection() const
{
	dbTable* tbl = mBrowsingView->table();
	dbSelection ret = tbl->makeEmptySelection();

	unsigned long numRecsAvailable = tbl->count();
	long highlightedRow = mBrowserDisplay->highlightedRow();
	while ((highlightedRow > -1) && ((unsigned long)highlightedRow <= numRecsAvailable)) {	// found a row
		unsigned long recNo = highlightedRow - mHeadingRows;
		ret += tbl->oidOfRecord(recNo);
		highlightedRow = mBrowserDisplay->nextHighlightedRow(highlightedRow);
	}
	return ret;
}


oidT 
dbBrowseHelper::highlightedOID() const
{
// much more efficient than highlightedSelection if we just want one record
// make it return the OID of the FIRST record selected, even if multiple selected
	dbTable* tbl = mBrowsingView->table();
	oidT ret = 0;

	long highlightedRow = mBrowserDisplay->highlightedRow();
	unsigned long numRecsAvailable = tbl->count();
	if ((highlightedRow > -1) && ((unsigned long)highlightedRow <= numRecsAvailable)) {	// found a row
		unsigned long recNo = highlightedRow - mHeadingRows;
		ret = tbl->oidOfRecord(recNo);
	}
	return ret;
}


void 
dbBrowseHelper::gotoHighlightedRecord() const
{
	assert(mBrowserDisplay);
	assert(highlightedSelection().count()==1);
	unsigned long clickedRow = mBrowserDisplay->highlightedRow() - mHeadingRows;
	mBrowsingView->table()->gotoRecord(clickedRow);
}


void
dbBrowseHelper::highlightRecNo(unsigned long inRecNo, bool bSelect) const
{
OOF_DEBUG_LOG(ostrstream() << "dbBrowseHelper::highlightRecNo\t" << inRecNo << "\ton table\t" << hex << (void*) mBrowsingView->table());

	assert(mBrowserDisplay);
	mBrowserDisplay->highlightRow(inRecNo+mHeadingRows, bSelect);
}


void
dbBrowseHelper::highlightNothing() const
{
	mBrowserDisplay->highlightNothing();
}


void
dbBrowseHelper::LoadBrowseData()
{
	if (mBrowserDisplay) 
		mBrowserDisplay->loadData();
}


void 
dbBrowseHelper::refreshBrowser()
{
OOF_DEBUG_LOG(ostrstream() << "dbBrowseHelper::refreshBrowser on table\t" << hex << (void*) mBrowsingView->table());
	if (mBrowserDisplay) 
		mBrowserDisplay->refresh();
}


dbView*
dbBrowseHelper::view() const
{
	return mBrowsingView;
}


// -------------------------------------------------------
//                 d b E d i t H e l p e r
// -------------------------------------------------------

dbEditHelper::dbEditHelper(dbTable* tbl) :
	dbAbstractEditHelper(tbl)
{
}


bool 
dbEditHelper::SendFrameworkMessage(OOFmsgT msg, void* /*ioParam*/)
{
	postFrameworkMessage(msg);
	return true;
}


bool
dbEditHelper::SendImmediateFrameworkMessage(OOFmsgT msg, void* lParam)
{
// virtually the same as postFrameworkMessage except for using SendMessage
	CWnd* actWnd = CWnd::GetFocus();
	bool ret = false;
	CWnd* pSafeParent;
	WPARAM wParam = msg;
	if (actWnd) {  // probably something like a button
		pSafeParent = actWnd->GetSafeOwner();
// WARNING if this triggers something like a change in current window you are likely to crash
// eg: if triggered by a double-click in a list		
	}
	else {  // probably called from something like LeaveRecord with a callback message
		pSafeParent = AfxGetMainWnd();
		ASSERT(pSafeParent);
	}
	ret = pSafeParent->SendMessage(WM_COMMAND, wParam, (long)lParam) != 0;
	return ret;
}


bool
dbEditHelper::postFrameworkMessage(UINT msg, LPARAM lParam)
{
// only window which has focus will receive the message;
// if there is no window with focus, message will be discarded
	CWnd* actWnd = CWnd::GetFocus();
	bool ret = false;
	WPARAM wParam = msg;
	CWnd* pSafeParent;
	if (actWnd) {  // probably something like a button
		pSafeParent = actWnd->GetSafeOwner();
	}
	else {  // probably called from something like LeaveRecord with a callback message
		pSafeParent = AfxGetMainWnd();
		ASSERT(pSafeParent);
	}
	ret = pSafeParent->PostMessage(WM_COMMAND, wParam, lParam) != 0;
	return ret;
/*
// this is the version which sends the message to the application
// main window and to all its child controls; main window or any
// child control may break the message routing chain by returning
// nonzero value from its message handler

	CWnd* actWnd = AfxGetMainWnd();

	if (actWnd != NULL && actWnd->SendMessage(msg, wParam, lParam) == 0)
		return SendMessageToDescendantsStoppingAtFirst(actWnd->GetSafeHwnd(), msg, wParam, lParam);
	else
		return 0;
*/
}


// functionality taken from CWnd::SendMessageToDescendants()
LRESULT
dbEditHelper::SendMessageToDescendantsStoppingAtFirst(
					HWND hWnd,
					UINT message,
					WPARAM wParam, LPARAM lParam,
					BOOL bDeep,
					BOOL bOnlyPerm)
{
	LRESULT retVal = 0;
	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (HWND hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		// if bOnlyPerm is TRUE, don't send to non-permanent windows
		if (bOnlyPerm)
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
			if (pWnd != NULL)
			{
				// call window proc directly since it is a C++ window
				retVal = AfxCallWndProc(pWnd, pWnd->m_hWnd, message, wParam, lParam);
				if (retVal)
					break;
			}
		}
		else
		{
			// send message with Windows SendMessage API
			retVal = ::SendMessage(hWndChild, message, wParam, lParam);
			if (retVal)
				break;
		}
		if (bDeep && ::GetTopWindow(hWndChild) != NULL)
		{
			// send to child windows after parent
			retVal = SendMessageToDescendantsStoppingAtFirst(
							hWndChild, message,
							wParam, lParam,
							bDeep, bOnlyPerm);
			if (retVal)
				break;
		}
	}
	return retVal;
}


bool 
dbEditHelper::legalCmdMsg(UINT msg)
{
	bool ret = ((msg >= 0x8000) && (msg <= 0xDFFF));  // MFC Internals p658
	return ret;
}


void*
dbEditHelper::currentGUItarget() const
{
	void* ret  = CWnd::GetFocus();
	return ret;
}


// -------------------------------------------------------
//            d b L i s t B o x D i s p l a y
// -------------------------------------------------------
dbListBoxDisplay::dbListBoxDisplay(COOFListBox* theLB, dbView* theView) :
	mDisplay(theLB),
	mView(theView),
	mLoadedData(false)
{
	if (mView && mDisplay) {
		SetColWidths();
		mDisplay->changeViewTo(mView);
	}
}

void
dbListBoxDisplay::setDoubleClickMessage(MessageT msg)
{
	assert(mDisplay);
// ES
//	mDisplay->SetDoubleClickMessage(msg);
}

void 
dbListBoxDisplay::refresh()
{
	loadData();
}

void
dbListBoxDisplay::loadData()
{
	if (!mLoadedData) {
		// loadData may be called to reread existing data,
		// so mLoadedData may be set to false
		DeleteAllData();
		mDisplay->initializeSpace(mView->source()->count());
		mLoadedData = true;
	}
	else {
		// adds or removes new or deleted items
		mDisplay->adjustNumItems();
		mDisplay->Invalidate();
	}
}

/*
void 
dbListBoxDisplay::loadData()
// VERY, VERY coarse first attempt just to get a demo working
{
	mLoadedData = true;

	unsigned long recNo = mView->source()->recordNumber();
	int numRows = mView->source()->count();
//	int curSel = mDisplay->GetCurSel();	// might be different from recNo
//
//	if (curSel == LB_ERR)	// no selection
//		curSel = 0;
//
	DeleteAllData();

	mView->source()->start();  
	for (int i=0; i<numRows; i++) {
		LoadRow(i, false);
		mView->source()->next();
	}

	mView->source()->gotoRecord(recNo);
	// return value is not tested because listbox might be empty
//	mDisplay->SetCurSel(curSel);
}


void 
dbListBoxDisplay::LoadRow(const int row, bool insertTheRow)
{
	CString item;
	mView->start();
	for (int col = 0; mView->more(); col++) {
		dbField  *theField = (dbField  *) (mView->item(col)); 		  // safe downcast
		char* str = theField->copyAsChars();  // good test of allocator, in this loop!
		if (col > 0)
			item += CString(',');
		item += str;
		delete[] str;
		mView->next();
	}

	if (insertTheRow) 
		mDisplay->InsertString(row, item);
	else
		mDisplay->AddString(item);

// ES
//	ListHandle lh = mDisplay->GetMacListH();
//	if (insertTheRow) 
//		::LAddRow(1, row, lh);
//	mView->start();
//	Cell theCell;
//	theCell.v = row;
//	for (int col = 0; mView->more(); col++) {
//		dbField  *theField = (dbField  *) (mView->item(col)); 		  // safe downcast
//		char* str = theField->copyAsChars();  // good test of allocator, in this loop!
//		theCell.h = col;
//		::LSetCell(str, strlen(str), theCell, lh);
//		delete str;
//		mView->next();
//	}
}
*/

bool
dbListBoxDisplay::MultipleSelectionEnabled() const
{
	DWORD wndStyle = mDisplay->GetStyle();
	return (wndStyle & LBS_EXTENDEDSEL) || (wndStyle & LBS_MULTIPLESEL);
}

void 
dbListBoxDisplay::DeleteAllData()
{
	mDisplay->ResetContent();
}

void 
dbListBoxDisplay::changeViewTo(dbView* theView)
{
	mView = theView;
	mDisplay->changeViewTo(theView);
	SetColWidths();
	if (mLoadedData)  // don't load yet if hadn't loaded other view
		loadData();
}

void
dbListBoxDisplay::highlightRow(unsigned long zeroBasedRowNum, bool bSelect)
{
	if (MultipleSelectionEnabled())
		mDisplay->SetSel(zeroBasedRowNum, bSelect);
	else if (bSelect)
		mDisplay->SetCurSel(zeroBasedRowNum);
	else
		mDisplay->SetCurSel(-1);
}

void
dbListBoxDisplay::highlightNothing()
{
	if (MultipleSelectionEnabled())
		mDisplay->SetSel(-1, FALSE);
	else
		mDisplay->SetCurSel(-1);
}

void
dbListBoxDisplay::deleteRow(unsigned long zeroBasedRowNum)
{
	VERIFY(mDisplay->DeleteString(zeroBasedRowNum) != LB_ERR);
}

bool
dbListBoxDisplay::displayUsesControl(void* inControl) const
{
	return (mDisplay==inControl);
}

// Returns the highlighted row in the list box. If there are multiple
// items selected, the index of the first one is returned. The function
// returns -1 in case of an error or when there is no item selected.
long
dbListBoxDisplay::highlightedRow() const
{
	if (MultipleSelectionEnabled()) {
		int indFirst;
		int retVal = mDisplay->GetSelItems(1, &indFirst);

		return retVal == 0 || retVal == LB_ERR ? -1 : indFirst;
	}
	else
		return mDisplay->GetCurSel();
}

// Function returns the next item selected after <afterRow> index in a
// list box. Works for list boxes with both single and multiple selection
// enabled. Returns -1 if there is no item selected after <afterRow> index
// or if there is no item selected.
long
dbListBoxDisplay::nextHighlightedRow(long afterRow) const
{
	if (MultipleSelectionEnabled()) {
		int numSel = mDisplay->GetSelCount();

		// handle the state when there is no item selected or an error has occurred
		if (numSel < 1)
			return -1;

		int retVal = -1;
		LPINT buf = new int[numSel];

		VERIFY(mDisplay->GetSelItems(numSel, buf) != LB_ERR);

		for (int i = 0; i < numSel; i++) {
			if (buf[i] > (int)afterRow) {
				retVal = buf[i];
				break;
			}
		}

		delete[] buf;
		return retVal;
	}
	else {
		int curSel = mDisplay->GetCurSel();
		return curSel != LB_ERR && curSel > afterRow ? curSel : -1;
	}
}

void 
dbListBoxDisplay::SetColWidths()
{
// ES
/*
// logic copied from LListBox::ResizeFrameBy
// which unfortunately we can't just call as it has a test to see if the cells are 
// already at the default width
	assert(mDisplay);
	assert(mView);
	ListHandle lh = mDisplay->GetMacListH();
	Rect	displayRect = (**lh).rView;
	Point	cellSize = (**lh).cellSize;
	cellSize.h = (displayRect.right - displayRect.left) / mView->count();
	::LCellSize(cellSize, lh);
*/
}

// -------------------------------------------------------
//             d b L i s t B o x C t r l
// -------------------------------------------------------

dbListCtrlDisplay::dbListCtrlDisplay(COOFListCtrl* theLBCtrl, dbView* theView) :
		mDisplay(theLBCtrl),
		mView(theView),
		mLoadedData(false)
{
	if (mView && mDisplay) {
		SetColWidths();
		mDisplay->changeViewTo(mView);
	}
}

void
dbListCtrlDisplay::setDoubleClickMessage(MessageT msg)
{}

void 
dbListCtrlDisplay::refresh()
{
	loadData();
}

void 
dbListCtrlDisplay::loadData()
// first approach to virtualization
{
	if (!mLoadedData) {
		// loadData may be called to reread existing data,
		// so mLoadedData may be set to false
		DeleteAllData();
		CheckColumns();
		mDisplay->initializeSpace(mView->source()->count());
		mLoadedData = true;
	}
	else {
		// adds or removes new or deleted items
		mDisplay->adjustNumItems();
		mDisplay->redraw();
	}
}

/*
void 
dbListCtrlDisplay::loadData()
// VERY, VERY coarse first attempt just to get a demo working
{
	mLoadedData = true;

	unsigned long recNo = mView->source()->recordNumber();
	int numRows = mView->source()->count();
//	int curSel = highlightedRow();	// might be different from recNo
//
//	if (curSel == -1)	// no selection
//		curSel = 0;

	DeleteAllData();
	CheckColumns();

	mView->source()->start();
	for (int i=0; i<numRows; i++) {
		LoadRow(i, false);
		mView->source()->next();
	}

	mView->source()->gotoRecord(recNo);
//	mDisplay->SetItemState(curSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void 
dbListCtrlDisplay::LoadRow(const int row, bool insertTheRow)
{
	mView->start();

	LV_ITEM item;

	item.mask		= LVIF_TEXT;
	item.iItem		= row;
	item.iSubItem	= 0;
	item.pszText	= "";

	int ind = mDisplay->InsertItem(&item);
	ASSERT(ind != -1);

	for (int col = 0; mView->more(); col++) {
		dbField* theField = (dbField*) (mView->item(col)); 		  // safe downcast

		char* itemText	= theField->copyAsChars();  // good test of allocator, in this loop!
		VERIFY(mDisplay->SetItemText(ind, col, itemText));

		delete[] itemText;
		mView->next();
	}
}
*/

void 
dbListCtrlDisplay::DeleteAllData()
{
	VERIFY(mDisplay->DeleteAllItems());
}

void 
dbListCtrlDisplay::changeViewTo(dbView* theView)
{
	mView = theView;
	mDisplay->changeViewTo(theView);
	SetColWidths();
	if (mLoadedData)  // don't load yet if hadn't loaded other view
		loadData();
}

void
dbListCtrlDisplay::highlightRow(unsigned long zeroBasedRowNum, bool bSelect)
{
	assert(mDisplay != NULL);

	if (mDisplay->GetItemCount() > 0) {
		if (bSelect) {
			mDisplay->SetItemState(zeroBasedRowNum, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
			mDisplay->EnsureVisible(zeroBasedRowNum, FALSE);
		}
		else
			mDisplay->SetItemState(zeroBasedRowNum, 0, LVIS_SELECTED);
	}
}

void
dbListCtrlDisplay::highlightNothing()
{
	mDisplay->selItemRange(FALSE, 0, mDisplay->GetItemCount()-1);

	// Remove focus, if there is any. This is necessary when deleting items in
	// a list control with multiple selection enabled. If foucus wasn't removed
	// it would move through lots of items, causing multiple OnItemChanged calls.
	mDisplay->removeFocus();
}

void
dbListCtrlDisplay::deleteRow(unsigned long zeroBasedRowNum)
{
	VERIFY(mDisplay->DeleteItem(zeroBasedRowNum));
}

bool
dbListCtrlDisplay::displayUsesControl(void* inControl) const
{
	return (mDisplay==inControl);
}

long 
dbListCtrlDisplay::highlightedRow() const
{
	return mDisplay->GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
}

// Function returns the next item selected after <afterRow> index in a list
// control. Returns -1 if there is no item selected after <afterRow> index
// or if there is no item selected.
long
dbListCtrlDisplay::nextHighlightedRow(long afterRow) const
{
	return mDisplay->GetNextItem((int)afterRow, LVNI_ALL|LVNI_SELECTED);
}

void 
dbListCtrlDisplay::SetColWidths()
{
	CheckColumns();
}

void
dbListCtrlDisplay::CheckColumns()
{
	unsigned int displayColCount = mDisplay->getColCount();
	unsigned int viewColCount = mView->count();

	if (viewColCount > displayColCount) {
		while (mDisplay->DeleteColumn(0))
			;
		for (unsigned int i=0; i<viewColCount; i++)
			VERIFY(mDisplay->InsertColumn(i, mView->getHeading(i), LVCFMT_LEFT, 100, i) != -1);
	}
}


// -------------------------------------------------------
//       d e b u g g i n g    t o o l s
// -------------------------------------------------------

ostream& operator<<(ostream& os, PPMessage& inMessage)
{
	os << hex << inMessage << dec;  // output just the raw message, if we don't have a lookup
	
// ES
/*
	switch(inMessage.msg) {
	case (cmd_Nothing) :
		os << "cmd_Nothing";
		break;
	case (cmd_About) :
		os << "cmd_About";
		break;

	// File Menu
	case (cmd_New) :
		os << "cmd_New";
		break;
	case (cmd_Open) :
		os << "cmd_Open";
		break;
	case (cmd_Close) :
		os << "cmd_Close";
		break;
	case (cmd_Save) :
		os << "cmd_Save";
		break;
	case (cmd_SaveAs) :
		os << "cmd_SaveAs";
		break;
	case (cmd_SaveCopyAs) :
		os << "cmd_SaveCopyAs";
		break;
	case (cmd_Revert) :
		os << "cmd_Revert";
		break;
	case (cmd_PageSetup) :
		os << "cmd_PageSetup";
		break;
	case (cmd_Print) :
		os << "cmd_Print";
		break;
	case (cmd_PrintOne) :
		os << "cmd_PrintOne";
		break;
	case (cmd_Quit) :
		os << "cmd_Quit";
		break;

	// Edit Menu
	case (cmd_Undo) :
		os << "cmd_Undo";
		break;
	case (cmd_Cut) :
		os << "cmd_Cut";
		break;
	case (cmd_Copy) :
		os << "cmd_Copy";
		break;
	case (cmd_Paste) :
		os << "cmd_Paste";
		break;
	case (cmd_Clear) :
		os << "cmd_Clear";
		break;
	case (cmd_SelectAll) :
		os << "cmd_SelectAll";
		break;
	case (cmd_ShowClipboard) :
		os << "cmd_ShowClipboard";
		break;
		
	// Misc
	case (msg_TabSelect) :
		os << "msg_TabSelect";
		break;
	case (msg_BroadcasterDied) :
		os << "msg_BroadcasterDied";
		break;
	case (msg_ControlClicked) :
		os << "msg_ControlClicked";
		break;
	case (msg_ThumbDragged) :
		os << "msg_ThumbDragged";
		break;
	case (cmd_FontMenu) :
		os << "cmd_FontMenu";
		break;
	case (cmd_SizeMenu) :
		os << "cmd_SizeMenu";
		break;
	case (cmd_StyleMenu) :
		os << "cmd_StyleMenu";
		break;

	// Size menu commands
	case (cmd_FontLarger) :
		os << "cmd_FontLarger";
		break;
	case (cmd_FontSmaller) :
		os << "cmd_FontSmaller";
		break;
	case (cmd_FontOther) :
		os << "cmd_FontOther";
		break;

	// Style menu commands
	case (cmd_Plain) :
		os << "cmd_Plain";
		break;
	case (cmd_Bold) :
		os << "cmd_Bold";
		break;
	case (cmd_Italic) :
		os << "cmd_Italic";
		break;
	case (cmd_Underline) :
		os << "cmd_Underline";
		break;
	case (cmd_Outline) :
		os << "cmd_Outline";
		break;
	case (cmd_Shadow) :
		os << "cmd_Shadow";
		break;
	case (cmd_Condense) :
		os << "cmd_Condense";
		break;
	case (cmd_Extend) :
		os << "cmd_Extend";
		break;

	// Text justification
	case (cmd_JustifyDefault) :
		os << "cmd_JustifyDefault";
		break;
	case (cmd_JustifyLeft) :
		os << "cmd_JustifyLeft";
		break;
	case (cmd_JustifyCenter) :
		os << "cmd_JustifyCenter";
		break;
	case (cmd_JustifyRight) :
		os << "cmd_JustifyRight";
		break;
	case (cmd_JustifyFull) :
		os << "cmd_JustifyFull";
		break;

	// Miscellaneous Messages
	case (msg_GrowZone) :
		os << "msg_GrowZone";
		break;
	case (msg_EventHandlerNote) :
		os << "msg_EventHandlerNote";
		break;

	// Attachment Messages
	case (msg_Event) :
		os << "msg_Event";
		break;
	case (msg_DrawOrPrint) :
		os << "msg_DrawOrPrint";
		break;
	case (msg_Click) :
		os << "msg_Click";
		break;
	case (msg_AdjustCursor) :
		os << "msg_AdjustCursor";
		break;
	case (msg_KeyPress) :
		os << "msg_KeyPress";
		break;
	case (msg_CommandStatus) :
		os << "msg_CommandStatus";
		break;
	case (msg_PostAction) :
		os << "msg_PostAction";
		break;

	case (msg_OK) :
		os << "msg_OK";
		break;
	case (msg_Cancel) :
		os << "msg_Cancel";
		break;

	default : {
		char	charMsg[5];
		memcpy(charMsg, &inMessage, 4);
		charMsg[4] = '\0';
		os << charMsg;
	}
	};
*/
	return os;
}

#endif  // !_Macintosh for doxygen
