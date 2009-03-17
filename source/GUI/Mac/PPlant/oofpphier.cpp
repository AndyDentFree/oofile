// COPYRIGHT 1996 A.D. Software, All rights reserved

// PowerPlant Integration layer of OOFILE database
#include "oofpphier.h"
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
#ifndef H_OOFQUERY
	#include "oofquery.h"
#endif
#ifndef H_stSetCursor
	#include "stSetCursor.h"
#endif

// include PowerPlant definitions
#include <LEditField.h>
#include <LCaption.h>
#include <LListBox.h>
#include <UMemoryMgr.h>
#include <PP_Messages.h>
#include <LOutlineTable.h>


// -------------------------------------------------------
//        d b V i e w O u t l i n e I t e m 
//-------------------------------------------------------

dbViewOutlineItem::dbViewOutlineItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel, bool isLeaf) :
	OOF_mixOutlineItem(inOID, inHelper, indentLevel, isLeaf)
{
}


dbViewOutlineItem::~dbViewOutlineItem()
{
}


Boolean		
dbViewOutlineItem::CanExpand() const
{
	const bool ret = !isLeaf();
	return ret;
}

	
void
dbViewOutlineItem::ExpandSelf()
{
	mHelper->insertItemsBelow(this, this);  // pass twice, as LOutlineItem and OOF_mixOutlineItem
}


void
dbViewOutlineItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	GetDrawContentsFromView(inCell, ioDrawContents);
}



void
dbViewOutlineItem::ClickCell(
	const STableCell&		inCell,
	const SMouseDownEvent&	inMouseDown)
{
// COPIED FROM LOutlineItem & changed to use the table's selector to decide what kind of 
// selection we get
	
	// Find out what we're supposed to draw.

	SOutlineDrawContents drawInfo;
	GetDrawContents(inCell, drawInfo);

	// Test disclosure triangle for first column.
	
	if ((inCell.col == 1) && CanExpand()) {
		Rect disclosureFrame;
		CalcLocalDisclosureTriangleRect(disclosureFrame);
		if (::PtInRect(inMouseDown.whereLocal, &disclosureFrame)) {
			TrackDisclosureTriangle(inMouseDown);
			return;
		}
	}

	// If there's an icon, hit test it.
	if (drawInfo.outHasIcon && (drawInfo.outIconSuite != nil)) {
		if (::PtInIconSuite(inMouseDown.whereLocal, &drawInfo.prIconFrame, drawInfo.outIconAlign, drawInfo.outIconSuite)) {
			TrackContentClick(inCell, inMouseDown, drawInfo, false);
			return;
		}
	}
	mOutlineTable->GetTableSelector()->ClickSelect(inCell, inMouseDown);
}


// -------------------------------------------------------
//        O O F _ m i x O u t l i n e I t e m
// -------------------------------------------------------
OOF_mixOutlineItem::OOF_mixOutlineItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel, bool isLeaf) :
	mOID(inOID),
	mHelper(inHelper),
	mIndentLevel(indentLevel),
	mCacheIndex(inHelper->nextCacheIndex()),
	mIsLeaf(isLeaf)
{}


dbView*
OOF_mixOutlineItem::view() const
{
	return mHelper->viewAtLevel(mIndentLevel);
}


dbHierLevel*
OOF_mixOutlineItem::level() const
{
	return mHelper->level(mIndentLevel);
}


bool 
OOF_mixOutlineItem::selectItemRecord()
{
// Flyweight pattern at work here!
	dbView* theView = view();
	dbTable* theTable = theView->table();
	bool ret;
	if (theTable)
		ret = theTable->selectJustOID(mOID);
	else
		ret = false;
	return ret;
}


void
OOF_mixOutlineItem::GetDrawContentsFromView(const STableCell&	 inCell, SOutlineDrawContents& ioDrawContents)
{
	const fieldNumT fieldNum = inCell.col-1;	// 1-based
	dbView* theView = view();
	const unsigned short numFields = theView->count();
	if (fieldNum>=numFields)
		return;		// early exit for more columns than we have in this view
	
	const char* contents = mHelper->getCachedContents(mCacheIndex, fieldNum);
	if (!contents) {	// assume entire row not cached
		dbTable* theTable = theView->table();
		stSaveSelection saveTable(theTable);
		theTable->selectJustOID(mOID);	
// now iterate across the fields and copy all values into the cache		
		mHelper->initCacheRow(mCacheIndex, numFields);
		for (unsigned short i=0; i<numFields; i++) {
			const dbField& theField = theView->field(i);
			assert(
				!theField.fieldTable() || // standalone fields OK
				!theView->table() ||  // view without table iterates once, might have fields from anywhere
				theField.fieldTable()==theView->table() ||  // field should be on view's table, if both have tables
				theField.fieldTable()->baseTableOfRelationChain()==theView->table()  // or be on related table
			);
			oofString fieldContents = theField.copyString();
			mHelper->setCachedContents(mCacheIndex, i, fieldContents.orphan());
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


oidT 
OOF_mixOutlineItem::findOIDforData(const void* inDataPtr,	UInt32 inDataSize) const
{
// sole use to-date is CDragDropOutlineTable::FindCellData where it is used
// to locate a visible cell matching the user's typing

	oidT ret = 0;
	dbView* theView = view();
	dbTable* viewTable = theView->table();
	if (viewTable) {  // legal to have views with no table - they behave as if 1 record
		if (inDataSize) {
			assert(inDataPtr);
			const dbField* searchField = &theView->defaultFindField();	
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
				stSaveSelection saveView(viewTable);
				viewTable->search(theSearch);	// in hier tables, always destroying the context
				viewTable->start();	// goto the first found record
				ret = viewTable->currentOID();
				delete theSearch;
			} // is a char field
			// NOT YET IMPLEMENTED - cope with others, see CQueryEngine::performSearch
		} // have data to find
	}
	return ret;
}


// -------------------------------------------------------
//            d b H i e r B r o w s e H e l p e r
// -------------------------------------------------------
dbHierBrowseHelper::dbHierBrowseHelper(LOutlineTable* inTable) :
	dbAbstractBrowseHelper(0),
	mOutlineTable(inTable),
	mNextCacheIndex(0),
	mContentsCache(0)
{
}


dbHierBrowseHelper::~dbHierBrowseHelper()
{
	mOutlineTable = 0;	// so don't try to update it in the following
	deleteAllLevels();
}


void
dbHierBrowseHelper::deleteAllLevels()
{
	if(mOutlineTable) {
		dbView* topView = viewAtLevel(0);
		const unsigned short numCols = 1 + topView->count();	// 1 for disclosure triangle

// delete all the items in our associated Outline Table
		const LArray& firstLevelItems = mOutlineTable->GetFirstLevelItems();
		UInt32 numItems = firstLevelItems.GetCount();
		if (numItems) {
			LOutlineItem* item;
			while (numItems>1) {	
				firstLevelItems.FetchItemAt(numItems, &item);	// 1-based arrays
				mOutlineTable->RemoveItem(item, false /* no refresh */, false /*don't adjust image size*/);
				numItems--;
			}	// loop to remove all bar first item
			firstLevelItems.FetchItemAt(1, &item);	// 1-based arrays
			mOutlineTable->RemoveItem(item, false /* no refresh */, true /* FINALLY adjust image size*/);
		}
// now quickly remove the cols to make sure LSelector's etc. are updated
		mOutlineTable->RemoveCols(numCols, 1, false /* don't refresh */);
	} // delete outline table's stuff
	
// delete our level info
	const unsigned short numLevels = mHierLevels.count();
	for (unsigned short i=0; i<numLevels; i++) {
		dbHierLevel* toDel = level(i);
		delete toDel;
	}
	mHierLevels = 0;
	deleteContentsCache();
}


void 
dbHierBrowseHelper::resetTableAfterViewRestructure()
{
// largely a copy of browseViewWithTable

	assert(mHierLevels.count());	// should have set our levels first

	dbView* topView = viewAtLevel(0);
	const unsigned short numCols = topView->count();	
	mOutlineTable->InsertCols(numCols, 0, 0, 0, 0);
	const short kTriangleColWidth = 16; // hardcode triangle col
	
// set col widths after loading establishes number of columns
	SDimension16 tableSize;
	mOutlineTable->GetFrameSize(tableSize);
	oofColSizer* sizer = topView->colSizer();
	sizer->calculateWidths(tableSize.width-kTriangleColWidth);	// divides up widths according to field defaults or user rules
	
	const unsigned long firstDataColWidth = sizer->width(0) + kTriangleColWidth;
	mOutlineTable->SetColWidth(firstDataColWidth, 1, 1);	
	for (unsigned short colFld=1; colFld<numCols; colFld++) {  // assign remaining col widths
		const unsigned long dataColWidth = sizer->width(colFld);
		const unsigned short oneBasedCol = colFld+1;
		mOutlineTable->SetColWidth(dataColWidth, oneBasedCol, oneBasedCol);	// from, to col No's
	}

// need to set cols before the table is displayed,
// but has no effect on the following
	insertItemsBelow(0, 0);	// add all the top-level items
}


void 
dbHierBrowseHelper::refreshBrowser()
{
	deleteContentsCache();
	if (mOutlineTable) 
		mOutlineTable->Refresh();  
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
	const unsigned long numRows = mContentsCache.count();
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
	mContentsCache.deleteAllCells();
	mNextCacheIndex = 0;
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


void dbHierBrowseHelper::browseViewWithTable(LOutlineTable* inTable)
{
	mOutlineTable = inTable;
	resetTableAfterViewRestructure();
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


void 
dbHierBrowseHelper::insertItemsBelow(LOutlineItem* inSuper, OOF_mixOutlineItem* inSuperDataManager)
{
	stSetCursor displayWatchByDefault;  // bit of user feedback so they know it's OK if it takes a while
	
// used to initially set the contents of the LOutlineTable
	dbSelection* savedTopSelection=0;
	dbTable* addingTable;
	dbTable* superTable = 0;
	dbHierLevel* superLevel;
	unsigned short nestedIndentLevel = 0;
	if (inSuperDataManager) {
		superLevel = inSuperDataManager->level();
		assert(superLevel);
		superTable = superLevel->view()->table();
		savedTopSelection = new dbSelection(superTable->currentSelection());
		addingTable = superLevel->selectNestedItemsFor(inSuperDataManager->oid());
		nestedIndentLevel = inSuperDataManager->indentLevel()+1;
	}
	else {
		dbView* topView = viewAtLevel(0);
		addingTable = topView->table();
		superLevel = level(0);
	}

	if (addingTable) {	// records below specific outline item
		if (nestedIndentLevel>deepestIndent()) { // assume recursive situation, need to make a level
			dbHierLevel* nestedLevel = superLevel->makeNestedLevel();  // may be zero if non-recursive levels
			assert(nestedLevel);  // assert our levels are capable of extending themselves
			appendLevel(nestedLevel);  
		}
		LOutlineItem* prevItem = 0;
		const unsigned short numToInsert = addingTable->count();
		addingTable->start();
		for (unsigned short i=0; i<numToInsert; i++) {
			const oidT theOID = addingTable->currentOID();
			LOutlineItem* theItem = superLevel->makeNestedOutlineItem(theOID, this, nestedIndentLevel);
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


oidT 
dbHierBrowseHelper::highlightedOID() const
{
// much more efficient than highlightedSelection if we just want one record
// make it return the OID of the FIRST record selected, even if multiple selected

// works a little differently to the dbBrowser version because we deal with views that
// (for now) are assumed to be at different levels of a file.
// thus, higher levels are translated into a set of the bottom-most view's records
// BUT we only need to take this process far enough to get one OID

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
		if (selCell.row==lastRow)
			continue;		// skip cells on same row
			
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI
#endif	
		OOF_mixOutlineItem* theItem = dynamic_cast<OOF_mixOutlineItem*>(mOutlineTable->FindItemForRow(selCell.row));
		if (theItem && theItem->isLeaf()) {
			ret = theItem->oid();	// just one row added
			break;	// GOT ONE!
		}
		else {	// selected further up tree - propagate to lowest level
			ret = FirstOIDBelowItem(theItem->indentLevel(), theItem->oid());
		}
		lastRow	= selCell.row;
	}
	return ret;
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

	TableIndexT lastRow = 0;
	for (STableCell selCell = mOutlineTable->GetFirstSelectedCell(); 
		!selCell.IsNullCell(); 
		mOutlineTable->GetNextSelectedCell(selCell)
	) 
	{
		if (selCell.row==lastRow)
			continue;		// skip cells on same row
			
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI
#endif	
		OOF_mixOutlineItem* theItem = dynamic_cast<OOF_mixOutlineItem*>(mOutlineTable->FindItemForRow(selCell.row));
		if (theItem && theItem->isLeaf()) {
			ret += theItem->oid();	// just one row added
		}
		else {	// selected further up tree - propagate to lowest level
			IncludeAllBelowItem(theItem->indentLevel(), theItem->oid(), ret);
		}
		lastRow	= selCell.row;
	}
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
	const bool nestedLevelIsLowest = ( nestedIndentLevel==deepestIndent() );

	unsigned short numToInsert = nestedTable->count();
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
	const bool nestedLevelIsLowest = ( nestedIndentLevel==deepestIndent() );

	unsigned short numToInsert = nestedTable->count();
	nestedTable->start();
	for (unsigned short i=0; i<numToInsert; i++) {
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


OOF_mixOutlineItem* 
dbHierBrowseHelper::highlightedItem() const
{
	OOF_mixOutlineItem* ret = 0;
	// caller assumes single selection
	for (STableCell selCell = mOutlineTable->GetFirstSelectedCell(); 
		!selCell.IsNullCell(); 
		mOutlineTable->GetNextSelectedCell(selCell)
	) 
	{
#ifndef OOF_RTTI_ENABLED
	#pragma error "OOFILE GUI requires RTTI
#endif	
		ret = dynamic_cast<OOF_mixOutlineItem*>(mOutlineTable->FindItemForRow(selCell.row));	
		assert(ret);  // assume it's one of ours!
		return ret;	// EXIT LOOP EARLY, SO JUST FIND SINGLE ITEM
	}
	return ret;
}


bool 
dbHierBrowseHelper::selectItemRecord()
{
	bool ret = false;
	OOF_mixOutlineItem* theItem = highlightedItem();
	if (theItem) {
		ret = theItem->selectItemRecord();
	}
	return ret;
}


unsigned long	
dbHierBrowseHelper::highlightedRecNo() const
{
	return 0;  // NOT YET IMPLEMENTED
}



void 
dbHierBrowseHelper::highlightNothing() const
{
	// NOT YET IMPLEMENTED
}



void 	
dbHierBrowseHelper::highlightRecNo(unsigned long, bool bSelect) const
{
	// NOT YET IMPLEMENTED
}


 
void 
dbHierBrowseHelper::updateBrowserSelection() const
{
	// NOT YET IMPLEMENTED
}


dbView*
dbHierBrowseHelper::view() const
{
	return viewAtLevel(0);
}


bool 
dbHierBrowseHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret;
	switch (msg) {
	case OOFmsg_GetDisplayTableHandler :
		ret = (mOutlineTable==(void*)senderDefined);
		break;	
		
	default :
		ret = dbAbstractBrowseHelper::receiveMsg(msg, senderDefined);
	};
	return ret;
}


// -------------------------------------------------------
//    O O F _ m i x O u t l i n e I t e m F a c t o r y
// -------------------------------------------------------
LOutlineItem* 
OOF_mixOutlineItemFactory::makeItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel)
{
// default factory
// assumes leaves only at bottom level
// makes dbViewOutlineItems

	const bool isLeaf = (indentLevel==inHelper->deepestIndent());
	LOutlineItem* ret = new dbViewOutlineItem(inOID, inHelper, indentLevel, isLeaf);
	return ret;
}


// -------------------------------------------------------
//     d b V i e w O u t l i n e I t e m F a c t o r y
// -------------------------------------------------------
dbOutlineItemFactory::dbOutlineItemFactory(fieldNumT leafFieldNum) :
	mLeafFieldNum(leafFieldNum),
	mLastIndentLevel(0),
	mLastLeafField(0)
{}


dbOutlineItemFactory::dbOutlineItemFactory(const dbBool& leafField) :
	mLeafFieldNum(leafField.fieldNumber()),
	mLastIndentLevel(0),
	mLastLeafField(0)
{}


bool
dbOutlineItemFactory::IsLeaf(dbHierBrowseHelper* inHelper, unsigned short indentLevel)
{
// nice bit of caching avoids lookups of the actual field being checked whilst we are working 
// at the same level. This speeds up item creation when multiple items created twisting open a level above
// which is the most common circumstance
	if (!mLastLeafField || mLastIndentLevel!=indentLevel) {  // cached values for different level or never set
		dbTable* levelTable = inHelper->viewAtLevel(indentLevel)->table();  // assumes views not reset EVER whilst this object exists
		mLastLeafField = (dbBool*) levelTable->field(mLeafFieldNum);   // safe downcast verified below 
		assert(mLastLeafField->fieldType()==boolField);
		// note that this field may not be in the visible view, and probably isn't!
		mLastIndentLevel=indentLevel;
	}
	const bool ret = mLastLeafField->value();
	return ret;
}


LOutlineItem* 
dbOutlineItemFactory::makeItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel)
{
// default factory typically used in recursive trees
// works out if leaf based on a given field, at the current record of this level
// makes dbViewOutlineItems but subclass and just override this to make your own types
	const bool itemIsLeaf = IsLeaf(inHelper, indentLevel);
	LOutlineItem* ret = new dbViewOutlineItem(inOID, inHelper, indentLevel, itemIsLeaf);
	return ret;
}


// -------------------------------------------------------
//            d b H i e r L e v e l
// -------------------------------------------------------
dbHierLevel::dbHierLevel(dbView* adoptedView, OOF_mixOutlineItemFactory* adoptedFactory) :
	mView(adoptedView),
	mItemFactory(adoptedFactory)
{
	if (!mItemFactory)
		mItemFactory = this;  // recursive pattern is simpler than having default be new'd
}


dbHierLevel::dbHierLevel(dbView* adoptedView, const dbBool& isLeafField) :
	mView(adoptedView)
{
	assert(adoptedView);
	assert(isLeafField.fieldTable()->tableNumber()==adoptedView->table()->tableNumber());
	mItemFactory = new dbOutlineItemFactory(isLeafField.fieldNumber());
}


dbHierLevel::~dbHierLevel()
{
	delete mView;
	if (mItemFactory!=this)
		delete mItemFactory;
}


void 
dbHierLevel::setAlternateItemFactory(OOF_mixOutlineItemFactory* adoptedFactory)
{
	if (mItemFactory!=this)
		delete mItemFactory;
	mItemFactory = adoptedFactory;
}


LOutlineItem* 
dbHierLevel::makeNestedOutlineItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel)
{
	LOutlineItem*  ret = mItemFactory->makeItem(inOID, inHelper, indentLevel);
	return ret;
}


dbHierLevel* 
dbHierLevel::makeNestedLevel() const
{
	return 0;  // must override for recursive levels
}



// -------------------------------------------------------
//            d b H i e r R e l a t e d L e v e l
// -------------------------------------------------------
dbHierRelatedLevel::dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase* inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory) :
	dbHierLevel(adoptedView, adoptedFactory),
	mTraversalField(inTraversalField)
{
// this version of ctor exists mainly so can pass in just dbView, as bottom-most of fixed set of levels
// note that the assert below has to differ slightly to allow for this circumstance

// big danger is traversal field is on different table, so creates related selection that the
// next level doesn't see. A dbView may clone the table passed into it so we need this safety check.

// Regardless, they must be on the same prototypical table hence we compare table numbers
// (effectively the same table class, except OOFILE allows you to instantate a class several times
// for different physical databases, so I have to be careful about using the term 'same class')
	assert(!mTraversalField || mTraversalField->fieldTable()->tableNumber() == mView->table()->tableNumber());

	if (mTraversalField) {
		const dbTable* const viewTable = mView->table();
		if (mTraversalField->fieldTable() !=viewTable)
			mTraversalField = (dbRelRefBase*) viewTable->field(mTraversalField->fieldNumber()); // safe downcast
	}
}


dbHierRelatedLevel::dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase& inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory) :
	dbHierLevel(adoptedView, adoptedFactory),
	mTraversalField(&inTraversalField)
{
// see first ctor for big explanation of this assert
	assert(mTraversalField->fieldTable()->tableNumber() == mView->table()->tableNumber());

	const dbTable* const viewTable = mView->table();
	if (mTraversalField->fieldTable() !=viewTable)
		mTraversalField = (dbRelRefBase*) viewTable->field(mTraversalField->fieldNumber()); // safe downcast
}


dbHierRelatedLevel::dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase& inTraversalField, const dbBool& isLeafField) :
	dbHierLevel(adoptedView, isLeafField),
	mTraversalField(&inTraversalField)
{
// see first ctor for big explanation of this assert
	assert(mTraversalField->fieldTable()->tableNumber() == mView->table()->tableNumber());

	const dbTable* const viewTable = mView->table();
	if (mTraversalField->fieldTable() !=viewTable)
		mTraversalField = (dbRelRefBase*) viewTable->field(mTraversalField->fieldNumber()); // safe downcast
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
	dbTable* ret = 0;
	if (mTraversalField) {
		mView->table()->selectJustOID(inOID);
		assert(mView->table()->count()==1);
		mView->table()->start();	// just try
		ret = mTraversalField->relatedTable();
		mTraversalField->relateRecord();
	}
	return ret;
}




// -------------------------------------------------------
//  d b H i e r R e c u r s i v e R e l a t e d L e v e l
// -------------------------------------------------------
dbHierRecursiveRelatedLevel::dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase* inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory) :
	dbHierRelatedLevel(adoptedView, inTraversalField, adoptedFactory)
{}


dbHierRecursiveRelatedLevel::dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase& inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory) :
	dbHierRelatedLevel(adoptedView, inTraversalField, adoptedFactory)
{}


dbHierRecursiveRelatedLevel::dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase& inTraversalField, const dbBool& isLeafField) :
	dbHierRelatedLevel(adoptedView, inTraversalField, isLeafField)
{}


dbHierLevel* 
dbHierRecursiveRelatedLevel::makeNestedLevel() const
{
	assert(mTraversalField);
	assert(mTraversalField->fieldTable() == mView->table());
	dbTable* nestedTable = mTraversalField->relatedTable();
	assert(mTraversalField->fieldTable()->tableNumber() == nestedTable->tableNumber());  // proof of recursion
	dbView* orphanedView = new dbView(*mView, nestedTable);  // copy view but in related clone table
	dbRelRefBase* nestedTraversalField = (dbRelRefBase* ) nestedTable->field(mTraversalField->fieldNumber());  // safe downcast, given asserts
	dbHierLevel* ret = new dbHierRecursiveRelatedLevel(orphanedView, nestedTraversalField, mItemFactory);
	return ret;
}
