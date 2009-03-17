// COPYRIGHT 1995 A.D. Software, All rights reserved

// PowerPlant Integration layer of OOFILE database
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#include "oofppx.h"  // private interface, includes public
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



// static data
dbGUIFactoryPP  thePowerPlantFactory;		// Singleton	
const unsigned long kRecordLockedByOtherUser = 'Lock';

// -------------------------------------------------------
//             d b G U I F a c t o r y P P
// -------------------------------------------------------
dbGUI*  
dbGUIFactoryPP::concreteMakeGUI(dbTable* inTable) 
{
	return new dbGUI(inTable);
}


// -------------------------------------------------------
//                        d b G U I
// -------------------------------------------------------
bool 
dbGUI::msgForOOFILE(const MessageT inMessage, void* ioParam)
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
	
	case msg_OK:
		if (leaveRecord()) {	// implies save
			mClosing = true;
			LCommander* defComm = LCommander::GetTarget();
			assert(defComm /* default commander should not be nil */);
			defComm->ProcessCommand(cmd_Close, ioParam);
		}
		break;

	case msg_Cancel:
		mTable->unlockRecord();
		mClosing = true;
		LCommander* defComm = LCommander::GetDefaultCommander();
		assert(defComm /* default commander should not be nil */);
		defComm->ProcessCommand(cmd_Close, ioParam);
		break;

	case cmd_Quit:	// assume auto-save or YNCancel would be desired, same as Close
		if (leaveRecord())
			ret = false;	// let it fall through and be processed by the window
		// else we swallowed it as we couldn't leave
		break;

	case cmd_Close:
		if (mClosing) {
			mClosing = false;
			ret = false;	// fall through for window to process
		}
		else {
			if (leaveRecord())
				ret = false;	// let it fall through and be processed by the window
			// else we swallowed it as we couldn't leave
		}
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

	case cmd_dbEdit :	// want to go to a new record
		dbAbstractBrowseHelper* handlesList = GetDisplayTableHandler(ioParam);
		if (handlesList)
			goHighlightedRecord(handlesList);
		else
			ret = false;	// ***** we don't have a handler for this display table
		break;

	case cmd_Save:
		saveRecord();
		break;

	case cmd_Revert:
		revertRecord();
		break;

	case cmd_dbDel:
		if (ConfirmDelete()) 
			deleteRecord();
		return true;
		
		
	case cmd_dbBye:	// user's close or leave button wants to know if it can go
		if (!leaveRecord())
			ret = false;	// as it is a test, we return false if can't leave
							// the command won't be handled further up the hierarchy so will remain false
		break;
		
	default:
		ret = false;
	}

/*  from browser
	switch (inMessage) {
	case cmd_dbNew:
		assert(0);  // not yet complete
		// scope for us to react afterwards
		return true;

	case cmd_dbDoubleClick:
	case cmd_dbEdit:
		unsigned long clickedRow = gotoHighlightedRecord();
//		Rebroadcast(cmd_dbEdit, clickedRow);	// maybe not optimal, but will work for now
		// scope for us to react afterwards
		return true;

	case cmd_dbDel:
		DelRecord();
		return true;

	default:
		return false;
	}
*/
	return ret;
}

void 
dbGUI::goHighlightedRecord(dbAbstractBrowseHelper* inBrowser)
{
	assert(mTable);
	unsigned long recNo = inBrowser->highlightedRecNo();
	unsigned long currRec = mTable->recordNumber();
	if (recNo != currRec) {
		if(leaveRecord()) {
			mTable->gotoRecord(recNo);
			changedSelection(inBrowser);	// inBrowser skipped - already knows!
		}
		else
			inBrowser->highlightRecNo(currRec);	// ie: user cancelled leave, so must highlight original rec
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
		oofYNC("Someone else is editing this record - you will not be able to make any changes");
		someoneElseLocked = true;
	}
	if (someoneElseLocked) {
		mTable->abortTransaction();
		broadcast(OOFmsg_ChangeSelection, kRecordLockedByOtherUser, dontNeedToTell);
	}
	else 
		broadcast(OOFmsg_ChangeSelection, 0, dontNeedToTell);
}


bool
dbGUI::isOOFILEcommandEnabled(long commandID, Boolean& outEnabled) const  // use in findCommandStatus in PowerPlant apps
{
  	bool ret=true;
	outEnabled = false;
	switch (commandID) {

// you can always choose these as they present a
// Save Changes YNCancel? dialog if record dirty
		case cmd_dbNew:
		case cmd_dbClone:
			outEnabled = true;
			break;

// OOFILE should support a richer test if can delete records
// but currently you would have to add that in user code
		case cmd_dbDel:
			outEnabled = true;
			break;

		case cmd_Save:
			outEnabled = mTable->isDirty();
			break;

		case cmd_Revert:
			outEnabled = mTable->isDirty() && !mTable->isNewRecord();
			break;

// leaving record by navigation similarly triggers Save Changes
// dialog as appropriate, so only disable to indicate location
// in selection
		case cmd_dbFirst:
		case cmd_dbPrev:
			outEnabled = !mTable->atFirst();
			break;

		case cmd_dbNext:
		case cmd_dbLast:
			outEnabled = !mTable->atLast();
			break;

		default:
			ret = false;
	}
	return ret;  // was handled
}


bool 
dbGUI::isOOFILEmsg(const MessageT	inMessage)
{
// constants in oofpp.h and oofmfc.h as vary depending on framework
	bool ret = (inMessage >= kFirstOOFILEcmd && inMessage <= kLastOOFILEcmd);
	return ret;
}


// -------------------------------------------------------
//      d b B r o w s e H e l p e r
// -------------------------------------------------------

dbBrowseHelper::dbBrowseHelper(dbView* adoptedView) :
	dbAbstractBrowseHelper(adoptedView->table()), 
	mBrowsingView(adoptedView),
	mBrowserDisplay(0)
{
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
dbBrowseHelper::browseViewWithListBox(LListBox* inLB)
{
// NOT YET IMPLEMENTED - SUPPORT HEADINGS
#ifdef OOF_DEBUG
	if (mBrowserDisplay)
		dbConnect::raise("dbBrowseHelper::BrowseViewWithListBox replacing current browser display");
#endif
	delete mBrowserDisplay;
	mBrowserDisplay = new dbListBoxDisplay(inLB, mBrowsingView);
	dbTable* tbl = mBrowsingView->table();
	LoadBrowseData();
	if (tbl->count())
		highlightRecNo(tbl->recordNumber());
}


void 
dbBrowseHelper::browseViewWithTable(LTextTableView* inTable)
{
#ifdef OOF_DEBUG
	if (mBrowserDisplay)
		dbConnect::raise("dbBrowseHelper::BrowseViewWithTable replacing current browser display");
#endif
	delete mBrowserDisplay;
	if (mHeadingRows)
		inTable->SetNumFrozenRows(mHeadingRows);
	mBrowserDisplay = new dbTableViewDisplay(inTable, mBrowsingView, mHeadingRows);
	dbTable* tbl = mBrowsingView->table();
	dbRelRefBase* tableRel = tbl->controllingRelationship();
	if (tableRel) {
		OOF_RelMN* MNlink = tableRel->getMNlink();
		if (MNlink)	// is MN relationship
			MNlink->makeRelMaintainer();	// auto-link MN relationship
	}
	LoadBrowseData();
	
// set col widths after loading establishes number of columns
	SDimension16 tableSize;
	inTable->GetFrameSize(tableSize);
	oofColSizer* sizer = mBrowsingView->colSizer();
	sizer->calculateWidths(tableSize.width);	// divides up widths according to field defaults or user rules
	
	unsigned short numCols = mBrowsingView->count();
	for (unsigned short i=0; i<numCols; i++) {
		unsigned long dataColWidth = sizer->width(i);
		unsigned short col = i+1;	// tables are 1-based
		inTable->SetColWidth(dataColWidth, col, col);	// from, to col No's
	}
		
// highlight the current record in the table
	if (tbl->count())
		highlightRecNo(tbl->recordNumber());
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
		SysBeep(1);	// just beep if we haven't got it locked
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
		SysBeep(1);
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
		SysBeep(1);
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
	while ((highlightedRow <= numRecsAvailable) && (highlightedRow > -1)) {	// found a row
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
	if ((highlightedRow <= numRecsAvailable) && (highlightedRow > -1)) {	// found a row
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


bool 
dbBrowseHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret = true;
	dbTable* ourTable = mBrowsingView->table();
	assert(ourTable);

	switch (msg) {
	case OOFmsg_DeleteRecord : {	// senderDefined is the record number in the selection
		dbConnect::raise("OOFmsg_DeleteRecord sent to dbBrowseHelper"); // not sure if we cope yet???
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



// -------------------------------------------------------
//                 d b E d i t H e l p e r
// -------------------------------------------------------

dbEditHelper::dbEditHelper(dbTable* tbl) :
							dbAbstractEditHelper(tbl)
{
}


void*
dbEditHelper::currentGUItarget() const
{
	void* ret = 0;
	LCommander* targetMayBePane = LCommander::GetTarget();
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI"
#endif	
	LPane* targetIsPane = dynamic_cast<LPane*>(targetMayBePane);
	if (targetIsPane)
		ret = targetIsPane;  // if LPane better to use dynamic cast to adjust pointer value
	else
		ret = targetMayBePane;
	return ret;
}


bool 
dbEditHelper::SendFrameworkMessage(OOFmsgT inMsg, void* ioParam)
{
	void* messageParam = this;
	if (ioParam)
		messageParam = ioParam;

	LCommander* defComm = LCommander::GetTarget();
	assert(defComm /* default commander should not be nil */);
	bool ret = defComm->ProcessCommand(inMsg, messageParam);
	return ret;
}


// -------------------------------------------------------
//            d b L i s t B o x D i s p l a y
// -------------------------------------------------------
dbListBoxDisplay::dbListBoxDisplay(LListBox* theLB, dbView* theView) : 
		mDisplay(theLB),
		mView(theView),
		mLoadedData(false)
{
	if (mView && theLB)
		SetColWidths();

}


void
dbListBoxDisplay::setDoubleClickMessage(MessageT msg)
{
	assert(mDisplay);
	mDisplay->SetDoubleClickMessage(msg);
}


void 
dbListBoxDisplay::refresh()
{
	loadData();
}


void 
dbListBoxDisplay::loadData()
// VERY, VERY coarse first attempt just to get a demo working
{
	mDisplay->Hide();
	mLoadedData = true;
	DeleteAllData();
	ListHandle lh = mDisplay->GetMacListH();
	::LAddColumn(mView->count(), 0, lh);
	int numRows = mView->source()->count();
	::LAddRow(numRows, 0, lh);

	mView->source()->start();  
	for (int i=0; i<numRows; i++) {
		LoadRow(i, false);
		mView->source()->next();
	}
	mView->source()->start();  
	mDisplay->SetValue(0);
	mDisplay->Show();
}


void 
dbListBoxDisplay::LoadRow(const int row, bool insertTheRow)
// again, some very inefficient code to get the demo working
{
	ListHandle lh = mDisplay->GetMacListH();
	if (insertTheRow) 
		::LAddRow(1, row, lh);
	mView->start();
	Cell theCell;
	theCell.v = row;
	for (int col = 0; mView->more(); col++) {
		dbField  *theField = (dbField  *) (mView->item(col)); 		  // safe downcast
		oofString str = theField->copyString();  
		theCell.h = col;
		::LSetCell(str, str.length(), theCell, lh);
		mView->next();
	}
}


void 
dbListBoxDisplay::DeleteAllData()
{
	ListHandle lh = mDisplay->GetMacListH();
	::LDelColumn(0, 0, lh);  // all cols in one go
	::LDelRow(0, 0, lh);  // all rows in one go
}


void 
dbListBoxDisplay::changeViewTo(dbView* theView)
{
	mView = theView;
	SetColWidths();
	if (mLoadedData)  // don't load yet if hadn't loaded other view
		loadData();
}


long 
dbListBoxDisplay::highlightedRow() const
{
	return mDisplay->GetValue();
}


void 
dbListBoxDisplay::SetColWidths()
{
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
}



// -------------------------------------------------------
//             d b T a b l e V i e w D i s p l a y 
// -------------------------------------------------------
dbTableViewDisplay::dbTableViewDisplay(LTextTableView* inTable, dbView* inView, unsigned short inHeadingRows) : 
		dbTableDisplay(inHeadingRows),
		mDisplay(inTable),
		mView(inView),
		mLoadedData(false),
		mStorage(0)
{
//	if (mView)
//		subscribeTo(mView->table());
//	if (mView && theLB)
//		SetColWidths();

}


void
dbTableViewDisplay::setDoubleClickMessage(MessageT msg)
{
	assert(mDisplay);
	mDisplay->SetDoubleClickMessage(msg);
}



bool
dbTableViewDisplay::displayUsesControl(void* inControl) const
{
	return (mDisplay==inControl);
}


void 
dbTableViewDisplay::refresh()
{
	unsigned long numRowsExpected = mView->source()->count() + mHeadingRows;
	TableIndexT numRows, numCols;	
	mDisplay->GetTableSize(numRows, numCols);
	long numDiff = numRowsExpected - numRows;
	if (numDiff!=0)
		if (numDiff>0)
			mDisplay->InsertRows(numDiff, ULONG_MAX, 0, 0, false);	// don't refresh row
		else
			mDisplay->RemoveRows(-numDiff, numRows+numDiff, false);	// don't refresh row
			// delete last ABS(numDiff) rows, inset the starting point
	mDisplay->Refresh();	// refresh all in case records resorted
}


void 
dbTableViewDisplay::loadData()
{
	mLoadedData = true;
	if (mStorage==0) // never loaded before
	{
		mStorage = new dbViewAsTableStorage(mDisplay, mView, mHeadingRows);
		assert(mStorage);
		mDisplay->SetTableStorage(mStorage);
	}
}


void 
dbTableViewDisplay::changeViewTo(dbView* inView)
{
//	if (mView)
//		unsubscribe(mView->source()->table());
		
	mView = inView;
	delete mStorage;
	mStorage = 0;
//	subscribeTo(mView->source()->table());
//	if (mLoadedData) 
{ // don't load yet if hadn't loaded other view
		loadData();
		mDisplay->Refresh();
	}
}


void 
dbTableViewDisplay::appendRow()
{
	assert(mDisplay);
	mDisplay->InsertRows(1, ULONG_MAX, 0, 0, true);	// refresh after insert
}


void 
dbTableViewDisplay::highlightRow(unsigned long zeroBasedRowNum, bool /*bSelect*/)
{
	assert(mDisplay);
	mDisplay->SetValue(zeroBasedRowNum);
// now make the highlighted row visible	
	STableCell cell;
	cell.row = zeroBasedRowNum+1; // offset "bug" in ScrollCellIntoFrame (think it's just top edge of cell)
	cell.col = 1;
	mDisplay->ScrollCellIntoFrame(cell);
// NOT YET IMPLEMENTED - don't support highlightRow deselecting current row
}


void 
dbTableViewDisplay::highlightNothing()
{
	assert(mDisplay);
	mDisplay->SetValue(-1);
}


void 
dbTableViewDisplay::deleteRow(unsigned long zeroBasedRowNum)
{
	assert(mDisplay);
	mDisplay->RemoveRows(1, zeroBasedRowNum+1, true);	// refresh after remove
}


long 
dbTableViewDisplay::highlightedRow() const
{
	return mDisplay->GetValue(); // 0-based
}


long 
dbTableViewDisplay::nextHighlightedRow(long afterRow) const
{
	STableCell	nextCell;
	long ret;
	do {
		nextCell.row = afterRow+1;
		if (mDisplay->GetNextSelectedCell(nextCell))
			ret = nextCell.row - 1;		// 0-based
		else
			ret = -1;	// ran out of cells
	} while (ret == afterRow);
	return ret;
}


bool 
dbTableViewDisplay::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	if (msg==OOFmsg_ChangeSelection)	// ignores all others	
		refresh();	// will recalc storage size
	return true;
}


// -------------------------------------------------------
//       d b V i e w A s T a b l e S t o r a g e
// -------------------------------------------------------

dbViewAsTableStorage::dbViewAsTableStorage(
								LTableView			*inTableView,
								dbView				*inView,
								unsigned short	inHeadingRows) : 
	LTableStorage(inTableView), 
	mView(inView),
	mHeadingRows(inHeadingRows)
{
}


void 
dbViewAsTableStorage::SetCellData(
								const STableCell	&inCell,
								const void			*inDataPtr,
								UInt32				inDataSize)
{
// NOT YET IMPLEMENTED
}


void 
dbViewAsTableStorage::GetCellData(
								const STableCell	&inCell,
								void				*outDataPtr,
								UInt32				&ioDataSize) const
{
	unsigned long recNo = inCell.row - 1;	// from 1-based rows
	unsigned long fieldNo = inCell.col - 1; // from 1-based columns
	if (mHeadingRows && recNo < mHeadingRows) {
// use the heading, NOT a record field value	
		if (outDataPtr && ioDataSize) {
			OOF_String aHeading = mView->getHeading(fieldNo);
			aHeading.getChars((char*)outDataPtr, ioDataSize);
		}
	} // get headings
	else {
		recNo -= mHeadingRows;
		if (outDataPtr == nil) {	// still goto record but nowhere to draw for display
			mView->source()->gotoRecord(recNo); 
			const dbField& fld = mView->field(fieldNo);
			ioDataSize = fld.fieldDataLen();	// still return size to conform to other behaviour
		} 
		else {
			if (ioDataSize==0)
				return;  // early exit for trivial case
				
			mView->source()->gotoRecord(recNo); 
			const dbField& fld = mView->field(fieldNo);
			fld.getChars((char*)outDataPtr, ioDataSize);
		}
	}  // get field values
}


Boolean 
dbViewAsTableStorage::FindCellData(
								STableCell			&outCell, 
								const void			*inDataPtr,
								UInt32				inDataSize) const
{
	bool foundIt = false;
	outCell.SetCell(0,0);
	dbTable* viewTable = mView->table();
	if (viewTable) {
		if (inDataSize) {
			assert(inDataPtr);
			const dbField* searchField = &mView->defaultFindField();	
			dbQueryLiteral* rhsLiteral = 0;
			dbQueryField* lhsField = new dbQueryField(searchField);	
			assert(lhsField);
			if (searchField->fieldType() == charField || searchField->fieldType() == compoundField) {
				OOF_String rhsString((const char*)inDataPtr, inDataSize);
				rhsLiteral = new dbQueryLiteralStr(rhsString);
				dbQueryClause* theSearch = new dbQueryBinary(
					lhsField, 
					dbQueryClause::greaterThanOrEqual, 	/* want to get next cell after if no match */
					rhsLiteral
				);
				unsigned long recNo = viewTable->recordNumber(theSearch, &foundIt);	// expect 0 if not found
				recNo++; // from 0-based to 1-based
				outCell.SetCell(recNo,1);
				delete theSearch;
			} // is a char field
			// NOT YET IMPLEMENTED - cope with others, see CQueryEngine::performSearch
		} // have data to find
	}
	else {
		outCell.SetCell(1,1);	// if no table, view behaves as if just one record
		foundIt = true;	
// NOT YET IMPLEMENTED - test the cells on solitary row and set foundIt & SetCell(0,0) if not found
	}
	return foundIt;
}


void 
dbViewAsTableStorage::InsertRows(
								UInt32				inHowMany,
								TableIndexT			inAfterRow,
								const void			*inDataPtr,
								UInt32				inDataSize)
{
// IGNORED - this will be triggered by our dbBrowseHelper
// telling the table to insert rows so it also updates classes
// like the geometry. Our data model has already been expanded 
// behind the scenes.
}


void 
dbViewAsTableStorage::InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol,
								const void			*inDataPtr,
								UInt32				inDataSize)
								
{
// IGNORED - see InsertRows for full explanation
}


void 
dbViewAsTableStorage::RemoveRows(
								UInt32				inHowMany,
								TableIndexT			inFromRow)
{
// IGNORED - see InsertRows for full explanation
}


void 
dbViewAsTableStorage::RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol)
{
// IGNORED - see InsertRows for full explanation
}


void 
dbViewAsTableStorage::GetStorageSize(
								TableIndexT			&outRows,
								TableIndexT			&outCols)
{
	outRows = mView->source()->count() + mHeadingRows;
	outCols = mView->count();
}




// -------------------------------------------------------
//       d e b u g g i n g    t o o l s
// -------------------------------------------------------

ostream& operator<<(ostream& os, PPMessage& inMessage)
{
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
	return os;
}

