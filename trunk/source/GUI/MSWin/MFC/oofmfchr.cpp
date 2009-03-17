// COPYRIGHT 1996 A.D. Software, All rights reserved

// MFC Integration layer of OOFILE database
#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofmfchr.h"
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


// -------------------------------------------------------
//        d b V i e w O u t l i n e I t e m 
//-------------------------------------------------------

dbViewOutlineItem::dbViewOutlineItem(HTREEITEM treeItem, unsigned int indentLevel, oidT oid) :
	mTreeItem(treeItem),
	mIndentLevel(indentLevel),
	mOID(oid)
{
}

dbViewOutlineItem::~dbViewOutlineItem()
{
}


// ES
/*
dbViewOutlineItem::dbViewOutlineItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short inLevel) :
	mOID(inOID),
	mHelper(inHelper),
	mIndentLevel(inLevel),
	mCacheIndex(inHelper->nextCacheIndex())
{
}


dbViewOutlineItem::~dbViewOutlineItem()
{
}


Boolean		
dbViewOutlineItem::CanExpand() const
{
// can be mind-bogglingly slow!!!!~
//	unsigned long numNested = mHelper->countNestedItemsFromLevel(mOID, mIndentLevel);
	bool ret = !mHelper->itemIsAtBottom(this);
	return ret;
}

	
void
dbViewOutlineItem::ExpandSelf()
{
	mHelper->insertItemsBelow(this);
}


void
dbViewOutlineItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	if (inCell.col<2)
		return;		// early exit for null cell or disclosure triangle col
		
	fieldNumT fieldNum = inCell.col-2;	// 1-based, and 1st is disclosure triangle
	const char* contents = mHelper->getCachedContents(mCacheIndex, fieldNum);
	if (!contents) {	// assume entire row not cached
		dbView* theView = mHelper->viewAtLevel(mIndentLevel);
		assert(theView);
		dbTable* theTable = theView->table();
		stSaveSelection saveTable(theTable);
		theTable->selectJustOID(mOID);	
// now iterate across the fields and copy all values into the cache		
		unsigned short numFields = theView->count();
		mHelper->initCacheRow(mCacheIndex, numFields);
		for (unsigned short i=0; i<numFields; i++) {
			const dbField& theField = theView->field(i);	
			char* fieldContents = theField.copyAsChars();
			mHelper->setCachedContents(mCacheIndex, i, fieldContents);
		}
		
		contents = mHelper->getCachedContents(mCacheIndex, fieldNum);
		assert(contents);	// we just put them there!
	}
// copy contents to return string	
	unsigned long contentLen = strlen(contents);
	if (contentLen>255)
		contentLen = 255;	// restrict to Pascal string
	memcpy(&ioDrawContents.outTextString[1], contents, contentLen);
	ioDrawContents.outTextString[0] = contentLen;
	ioDrawContents.outShowSelection = true;
}


bool 
dbViewOutlineItem::selectItemRecord()
{
// Flyweight pattern at work here!
	dbView* theView = mHelper->viewAtLevel(mIndentLevel);
	bool ret = theView->table()->selectJustOID(mOID);
	return ret;
}
*/


// -------------------------------------------------------
//            d b H i e r B r o w s e H e l p e r
// -------------------------------------------------------
// ES
//dbHierBrowseHelper::dbHierBrowseHelper(LOutlineTable* inTable) :
dbHierBrowseHelper::dbHierBrowseHelper(dbHierLevel* level) :
// AD980424	dbAbstractBrowseHelper(level->view()),
	dbAbstractBrowseHelper(0),
// ES
//	mOutlineTable(inTable),
	mNextCacheIndex(0),
	mContentsCache(0)
{
	mLoadedData = false;
}


dbHierBrowseHelper::~dbHierBrowseHelper()
{
	unsigned long numLevels = mHierLevels.count();
	for (unsigned short i=0; i<numLevels; i++) {
		dbHierLevel* toDel = level(i);
		delete toDel;
	}
	deleteContentsCache();
}


void 
dbHierBrowseHelper::refreshBrowser()
{
	if (mDisplay != NULL) {
		mDisplay->ignoreChangedSelection();
		insertItems();
		mDisplay->ignoreChangedSelection(FALSE);
	}
}


unsigned long
dbHierBrowseHelper::nextCacheIndex()
{
	unsigned long ret = mNextCacheIndex;
	mNextCacheIndex++;
	return ret;
}


void
dbHierBrowseHelper::deleteContentsCache()
{
	unsigned long numRows = mContentsCache.count();
	for (unsigned long i=0; i<numRows; i++) {
		char** contentsRow = (char**) mContentsCache.value(i);
		if (contentsRow) {
			for (unsigned short j=0; contentsRow[j]; j++) {
				char* aString = contentsRow[j];
				delete[] aString;
			}
		}
		delete[] contentsRow;
		mContentsCache[i] = 0;
	}
}


void 
dbHierBrowseHelper::initCacheRow(unsigned long cacheIndex, fieldNumT numFields)
{
	char** contentsRow = (char**) mContentsCache.value(cacheIndex);	// may be existing
	if (contentsRow==0) {
		contentsRow = new char*[numFields+1];
		for (unsigned short i=0; i<=numFields; i++)	// YES I MEAN <=numFields
			contentsRow[i] = 0;
		mContentsCache[cacheIndex] = (unsigned long) contentsRow;
	}
}


const char*
dbHierBrowseHelper::getCachedContents(unsigned long cacheIndex, fieldNumT fieldNum)
{
	char** contentsRow = (char**) mContentsCache.value(cacheIndex);
	const char* ret = 0;
	if (contentsRow)
		ret = contentsRow[fieldNum];
	return ret;	
}


void
dbHierBrowseHelper::setCachedContents(unsigned long cacheIndex, fieldNumT fieldNum, char* adoptedStr)
{
	char** contentsRow = (char**) mContentsCache.value(cacheIndex);
	assert(contentsRow);
	char* existingContent = contentsRow[fieldNum] ;
	delete[] existingContent;
	contentsRow[fieldNum] = adoptedStr;
}

// ES
/*
void dbHierBrowseHelper::browseViewWithTable(LOutlineTable* inTable)
{
	mOutlineTable = inTable;
	assert(mHierLevels.count());	// should have set our levels first

	dbView* topView = viewAtLevel(0);
	unsigned short numCols = 1 + topView->count();	// 1 for disclosure triangle
	inTable->InsertCols(numCols, 0, 0, 0, 0);
	const short kTriangleColWidth = 16;
	inTable->SetColWidth(kTriangleColWidth, 1, 1);	// hardcode triangle col
	
// set col widths after loading establishes number of columns
	SDimension16 tableSize;
	inTable->GetFrameSize(tableSize);
	oofColSizer* sizer = topView->colSizer();
	sizer->calculateWidths(tableSize.width-kTriangleColWidth);	// divides up widths according to field defaults or user rules
	
	for (unsigned short i=0; i<numCols-1; i++) {
		unsigned long dataColWidth = sizer->width(i);
		unsigned short col = i+2;	// tables are 1-based and skip 1 for triangle
		inTable->SetColWidth(dataColWidth, col, col);	// from, to col No's
	}
		

// need to set cols before the table is displayed,
// but has no effect on the following
	insertItemsBelow(0);	// add all the top-level items
}
*/

void dbHierBrowseHelper::browseViewWithTreeCtrl(COOFTreeCtrl* inTree)
{
	mDisplay = inTree;
	mDisplay->SetDefaultImageList();
	assert(mHierLevels.count());	// should have set our levels first
	mDisplay->setHierLevels(&mHierLevels);
	insertItems();	// add all the top-level items
}

void 
dbHierBrowseHelper::resetTableAfterViewRestructure()
{
	mDisplay->setHierLevels(&mHierLevels);
	insertItems();	// add all the top-level items
}


void 
dbHierBrowseHelper::deleteAllLevels()
{
	unsigned long numLevels = mHierLevels.count();
	for (unsigned short i=0; i<numLevels; i++) {
		dbHierLevel* toDel = level(i);
		delete toDel;
	}
	mHierLevels = 0;
	deleteContentsCache();
}


void 
dbHierBrowseHelper::appendLevel(dbHierLevel* adoptedLevel)
{
	if (mHierLevels.count()==0) {
		dbView* topView = adoptedLevel->view();
		if (topView) {
			dbTable* topTable = topView->table();
			if (topTable) {
				dbGUI* ourGUI = topTable->getGUI();
				subscribeTo(ourGUI);
			}
		}
	}
	mHierLevels.append((unsigned long)adoptedLevel);
}

	

unsigned long 
dbHierBrowseHelper::countNestedItemsFromLevel(oidT inOID, unsigned short inLevel) const
{
	dbHierLevel* theLevel = level(inLevel);
	assert(theLevel);
	unsigned long ret = theLevel->countNestedItemsFor(inOID);
	return ret;
}


dbView* 
dbHierBrowseHelper::viewAtLevel(unsigned short inLevel) const
{
	dbHierLevel* theLevel = level(inLevel);
	assert(theLevel);
	dbView* ret = theLevel->view();
	return ret;
}


dbView*
dbHierBrowseHelper::view() const
{
	return viewAtLevel(0);
}


void
dbHierBrowseHelper::deleteSelection(const dbSelection& inSel)
{
	// Not implemented.
}

void
dbHierBrowseHelper::updateBrowserSelection() const
{
}

// ES
/*
void 
dbHierBrowseHelper::insertItemsBelow(dbViewOutlineItem* inSuper)
{
// used to initially set the contents of the LOutlineTable
	dbSelection* savedTopSelection=0;
	dbTable* addingTable;
	unsigned short nestedIndentLevel = 0;
	dbTable* superTable = 0;
	if (inSuper) {
		superTable = viewAtLevel(inSuper->mIndentLevel)->table();
		savedTopSelection = new dbSelection(superTable->currentSelection());
		dbHierLevel* superLevel = level(inSuper->mIndentLevel);
		assert(superLevel);
		addingTable = superLevel->selectNestedItemsFor(inSuper->mOID);
		nestedIndentLevel = inSuper->mIndentLevel+1;
	}
	else {
		dbView* topView = viewAtLevel(0);
		addingTable = topView->table();
	}

	if (addingTable) {	// we are not yet at bottom!
		dbViewOutlineItem* prevItem = 0;
		unsigned short numToInsert = addingTable->count();
		addingTable->start();
		for (unsigned short i=0; i<numToInsert; i++) {
			oidT theOID = addingTable->currentOID();
			dbViewOutlineItem* theItem = new dbViewOutlineItem(theOID, this, nestedIndentLevel);
			mOutlineTable->InsertItem(theItem, inSuper, prevItem);
			prevItem = theItem;
			addingTable->next();
		}	// add loop
	} // bottom of hierarchy
	
	if (savedTopSelection && superTable) {
		superTable->setSelection(*savedTopSelection);
		delete savedTopSelection;
	}
}
*/

void dbHierBrowseHelper::insertItems()
{
	ASSERT(mDisplay != NULL);

	if (!mLoadedData) {
		VERIFY(mDisplay->DeleteAllItems());
		mDisplay->initializeSpace();
		mLoadedData = true;
	}
	else {
		// just for now
		VERIFY(mDisplay->DeleteAllItems());
		mDisplay->initializeSpace();
		mLoadedData = true;

//		mDisplay->adjustNumItems();
//		mDisplay->Invalidate();
	}
}

/*
void dbHierBrowseHelper::insertItems()
{
	assert(mDisplay != NULL);
	VERIFY(mDisplay->DeleteAllItems());

	dbView* topView = viewAtLevel(0);
	dbTable* addingTable = topView->table();
	unsigned long savedRecNo = addingTable->recordNumber();

	{
		// store current selection in table
		stSaveSelection saveSelection(addingTable);

		addingTable->first();
		while (addingTable->more()) {
			HTREEITEM treeItem = loadRow(topView);
			insertSubItems(0, addingTable->currentOID(), treeItem);
			addingTable->next();
		}
	}

	addingTable->gotoRecord(savedRecNo);
}
*/

void dbHierBrowseHelper::insertSubItems(int indentLevel, oidT oid, HTREEITEM treeItem)
{
	dbHierLevel* superLevel = level(indentLevel);
	dbTable* superTable = superLevel->view()->table();
	dbSelection* savedTopSelection = new dbSelection(superTable->currentSelection());
	unsigned long savedRecNo = superTable->recordNumber();

	assert(superLevel);

	dbTable* addingTable = superLevel->selectNestedItemsFor(oid);
	int nestedIndentLevel = indentLevel+1;

	if (addingTable) {	// we are not yet at bottom!
		unsigned long numToInsert = addingTable->count();
		dbView* nestedView = viewAtLevel(nestedIndentLevel);
		dbTable* nestedTable = nestedView->table();
		if (nestedTable != addingTable) // only == if used related table in view
			nestedTable->setSelection(addingTable->currentSelection());
		nestedTable->start();
		for (unsigned short i=0; i<numToInsert; i++) {
			HTREEITEM treeSubItem = loadRow(nestedView, treeItem);
			insertSubItems(
				nestedIndentLevel,
				addingTable->currentOID(),
				treeSubItem);
			nestedTable->next();
		}	// add loop
	} // bottom of hierarchy

	if (savedTopSelection && superTable) {
		superTable->setSelection(*savedTopSelection);
		superTable->gotoRecord(savedRecNo);
		delete savedTopSelection;
	}
}


HTREEITEM dbHierBrowseHelper::loadRow(dbView* view, HTREEITEM itemParent)
{
	CString item;
	oofString str;

	view->start();

	for (int col = 0; view->more(); col++) {
		dbField* theField = (dbField*)(view->item(col)); 		  // safe downcast
		str = theField->copyString();  // good test of allocator, in this loop!
		if (col > 0)
			item += CString(", ");
		item += str;
		view->next();
	}

	return mDisplay->InsertItem(item, 0, 2, itemParent);
}

oidT 
dbHierBrowseHelper::highlightedOID() const
{
// much more efficient than highlightedSelection if we just want one record
// make it return the OID of the FIRST record selected, even if multiple selected

	return mDisplay->highlightedOID();

// works a little differently to the dbBrowser version because we deal with views that
// (for now) are assumed to be at different levels of a file.
// thus, higher levels are translated into a set of the bottom-most view's records
// BUT we only need to take this process far enough to get one OID

// ES
/*
	assert(mHierLevels.count());
	dbTable* tbl = lowestLevel()->view()->table();
	oidT ret = 0;

	TableIndexT lastRow = 0;
// still do iteration as may have higher levels selected that lack a lower level
// equivalent to a Finder selection that starts with several empty folders,
// and we want the first file in a depth-first traversal	
	for (STableCell selCell = mOutlineTable->GetFirstSelectedCell(); 
		!selCell.IsNullCell(); 
		mOutlineTable->GetNextSelectedCell(selCell)
	) 
	{
		if ((selCell.col==1) || (selCell.row==lastRow))
			continue;		// skip first col which contains triangle, or cells on same row
			
		dbViewOutlineItem* theItem = (dbViewOutlineItem*) mOutlineTable->FindItemForRow(selCell.row);	// safe downcast
		if (itemIsAtBottom(theItem)) {
			ret = theItem->mOID;	// just one row added
			break;	// GOT ONE!
		}
		else {	// selected further up tree - propagate to lowest level
			ret = FirstOIDBelowItem(theItem->mIndentLevel, theItem->mOID);
		}
		lastRow	= selCell.row;
	}
	return ret;
*/
}


dbSelection	
dbHierBrowseHelper::highlightedSelection() const
{

// works a little differently to the dbBrowser version because we deal with views that
// (for now) are assumed to be at different levels of a file.
// thus, higher levels are translated into a set of the bottom-most view's records

	assert(mHierLevels.count());
	dbTable* tbl = lowestLevel()->view()->table();
	dbSelection ret = tbl->makeEmptySelection();

	for (HTREEITEM selItem = mDisplay->GetFirstSelectedItem();
		selItem != NULL;
		selItem = mDisplay->GetNextSelectedItem(selItem))
	{
		dbViewOutlineItem theItem = mDisplay->getItemDesc(selItem);

		if (itemIsAtBottom(theItem))
			ret += theItem.oid();
		else
			IncludeAllBelowItem(theItem.indentLevel(), theItem.oid(), ret);
	}

// ES
/*
	TableIndexT lastRow = 0;
	for (STableCell selCell = mOutlineTable->GetFirstSelectedCell(); 
		!selCell.IsNullCell(); 
		mOutlineTable->GetNextSelectedCell(selCell)
	) 
	{
		if ((selCell.col==1) || (selCell.row==lastRow))
			continue;		// skip first col which contains triangle, or cells on same row
			
		dbViewOutlineItem* theItem = (dbViewOutlineItem*) mOutlineTable->FindItemForRow(selCell.row);	// safe downcast
		if (itemIsAtBottom(theItem)) {
			ret += theItem->mOID;	// just one row added
		}
		else {	// selected further up tree - propagate to lowest level
			IncludeAllBelowItem(theItem->mIndentLevel, theItem->mOID, ret);
		}
		lastRow	= selCell.row;
	}
*/
	return ret;
}


void	
dbHierBrowseHelper::IncludeAllBelowItem(unsigned short inIndentLevel, oidT inOID, dbSelection& addTo) const
{
	dbHierLevel* superLevel = level(inIndentLevel);
	assert(superLevel);
	stSaveSelection saveSuper(superLevel->view()->table());	// before selectNestedItemsFor() selects single OID out of selection
	dbTable* nestedTable = superLevel->selectNestedItemsFor(inOID);
	assert (nestedTable);	// shouldn't be called on the last level

	unsigned short nestedIndentLevel = inIndentLevel+1;
	bool nestedLevelIsLowest = ( nestedIndentLevel==(mHierLevels.count()-1) );

	unsigned long numToInsert = nestedTable->count();
	nestedTable->start();
	for (unsigned short i=0; i<numToInsert; i++) {
		oidT theOID = nestedTable->currentOID();
		if (nestedLevelIsLowest) {
			addTo += theOID;	// just one row added
		}
		else {	// selected further up tree - propagate to lowest level
		// 	BUT can't assume is twisted open at this level, therefore need to iterate down the 
		//  related table, NOT using items
			IncludeAllBelowItem(nestedIndentLevel, theOID, addTo);
		}			
		nestedTable->next();
	}
}


oidT	
dbHierBrowseHelper::FirstOIDBelowItem(unsigned short inIndentLevel, oidT inOID) const
{
// nearly straight copy of IncludeAllBelowItem but just return one OID rather than
// assembling a selection
	dbHierLevel* superLevel = level(inIndentLevel);
	assert(superLevel);
	stSaveSelection saveSuper(superLevel->view()->table());	// before selectNestedItemsFor() selects single OID out of selection
	dbTable* nestedTable = superLevel->selectNestedItemsFor(inOID);
	assert (nestedTable);	// shouldn't be called on the last level
	oidT ret = 0;
	unsigned short nestedIndentLevel = inIndentLevel+1;
	bool nestedLevelIsLowest = ( nestedIndentLevel==(mHierLevels.count()-1) );

	unsigned long numToInsert = nestedTable->count();
	nestedTable->start();
	for (unsigned long i=0; i<numToInsert; i++) {
		oidT theOID = nestedTable->currentOID();
		if (nestedLevelIsLowest) {
			ret = theOID;	// GOT IT
			break;
		}
		else {	// selected further up tree - propagate to lowest level
		// 	BUT can't assume is twisted open at this level, therefore need to iterate down the 
		//  related table, NOT using items
			ret = FirstOIDBelowItem(nestedIndentLevel, theOID);
		}			
		nestedTable->next();
	}
	return ret;
}


dbViewOutlineItem
dbHierBrowseHelper::highlightedItem() const
{
	// caller assumes single selection
	return mDisplay->getItemDesc(mDisplay->GetFirstSelectedItem());

/*	dbViewOutlineItem* ret = 0;
	// caller assumes single selection
	for (STableCell selCell = mOutlineTable->GetFirstSelectedCell(); 
		!selCell.IsNullCell(); 
		mOutlineTable->GetNextSelectedCell(selCell)
	) 
	{
		if (selCell.col==1) 
			continue;		// skip first col which contains triangle, or cells on same row
			
		ret = (dbViewOutlineItem*) mOutlineTable->FindItemForRow(selCell.row);	// safe downcast
		return ret;	// EXIT LOOP EARLY, SO JUST FIND SINGLE ITEM
	}
	return ret;
*/
}


bool 
dbHierBrowseHelper::selectItemRecord()
{
	dbViewOutlineItem theItem = highlightedItem();

	if (!theItem.isNullItem()) {
		dbView* theView = viewAtLevel(theItem.indentLevel());
		return theView->table()->selectJustOID(theItem.oid());
	}
	else
		return false;

/*	bool ret = false;
	dbViewOutlineItem* theItem = highlightedItem();
	if (theItem) {
		ret = theItem->selectItemRecord();
	}
	return ret;
*/
}


bool
dbHierBrowseHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret = true;
	switch (msg) {
	case OOFmsg_DeleteRecord : {	// senderDefined is the record number in the selection
		break;
	}

	case OOFmsg_GetDisplayTableHandler :
		assert(mDisplay);
		ret = (mDisplay == (void*)senderDefined);
		break;	

	default :
		ret = dbAbstractBrowseHelper::receiveMsg(msg, senderDefined);
	};
	return ret;
}


unsigned long
dbHierBrowseHelper::highlightedRecNo() const
{
	return mDisplay->highlightedRecNo();
}


void
dbHierBrowseHelper::highlightRecNo(unsigned long inRecNo, bool inSelect) const
{
	mDisplay->highlightRecNo(inRecNo, inSelect);
}


void
dbHierBrowseHelper::highlightNothing() const
{
	mDisplay->highlightNothing();
}

dbViewOutlineItem
dbViewOutlineItem::nullItem()
{
	return dbViewOutlineItem(0,0,0);
}

// -------------------------------------------------------
//            d b H i e r L e v e l
// -------------------------------------------------------
dbHierLevel::dbHierLevel(dbView* adoptedView) :
	mView(adoptedView)
{
}


dbHierLevel::~dbHierLevel()
{
	delete mView;
}




// -------------------------------------------------------
//            d b H i e r R e l a t e d L e v e l
// -------------------------------------------------------
dbHierRelatedLevel::dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase*	inTraversalField) :
	dbHierLevel(adoptedView),
	mTraversalField(inTraversalField)
{
// big danger is traversal field is on different table, so creates related selection that the
// next level doesn't see
	assert( (inTraversalField==0) || (adoptedView->table() == inTraversalField->fieldTable()) );
}


dbHierRelatedLevel::dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField) :
	dbHierLevel(adoptedView),
	mTraversalField(&inTraversalField)
{
// big danger is traversal field is on different table, so creates related selection that the
// next level doesn't see
	assert(adoptedView->table() == inTraversalField.fieldTable());
}


dbHierRelatedLevel::~dbHierRelatedLevel()
{
}


unsigned long 
dbHierRelatedLevel::countNestedItemsFor(oidT inOID)
{
	unsigned long ret = 0;
	if (mTraversalField) {
		dbTable* rhs = selectNestedItemsFor(inOID);
		ret = rhs->count();
	}
	return ret;
}


dbTable* 
dbHierRelatedLevel::selectNestedItemsFor(oidT inOID)
{
//	assert(mTraversalField->fieldTable() == mView->table());
	dbTable* ret = 0;
	if (mTraversalField) {
		mView->table()->selectJustOID(inOID);
		unsigned long countThisLevel = mView->table()->count();
// ES
//		if (!(countThisLevel==1) )
//			DebugStr("\pbad count");
		mView->table()->start();	// just try
		ret = mTraversalField->relatedTable();
		mTraversalField->relateRecord();
	}
	return ret;
}

#endif  // !_Macintosh for doxygen
