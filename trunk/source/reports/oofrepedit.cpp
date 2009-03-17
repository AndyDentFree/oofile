// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepedit.cpp
// report-writer layer of OOFILE database - editing classes

#include "oofpch_r.h"  // for precompilation of report files


/**
	\def OOF_REPEDIT_LOG_FILENAME
	Define to set filename for logging user interactions with report preview window.
	Defined by default for debug builds as "oofrep interaction.log"
	\ingroup oofMacros
*/

// default for debug builds 
// to track interaction area coordinates and clicks
#if !defined NDEBUG && defined OOF_DEBUG && !defined OOF_REPEDIT_LOG_FILENAME
	#define OOF_REPEDIT_LOG_FILENAME "oofrep interaction.log"
#endif

#ifndef H_OOFREPEDITX
	#include "oofrepeditx.h"
#endif

// -------------------------------------------------------
//     o o f R e p D r a w a b l e
// -------------------------------------------------------
/**
	Just make a renderer for the GUI environment as if not editing
	because by default most drawables are not editable!
*/
void	
oofRepDrawable::makeRender(const oofRepEditingEnv& inEnv)
{
	makeRender(*inEnv.parentGUIenv());
	mRender = inEnv.editor()->makeDefaultRenderWrapper(mRender ); 
}


// -------------------------------------------------------
//     o o f R e p E d i t i n g E n v i r o n m e n t
// -------------------------------------------------------
oofRepEditingEnv::oofRepEditingEnv(oofRepEditor* theEditor) :
	mEditor(theEditor),
	mLastAreaOnPrevPage(0),
	mInteractionPages(0)
{}


oofRepEditingEnv::~oofRepEditingEnv()
{
	delete mInteractionPages;
	assert(!mLastAreaOnPrevPage);  // shouldn't be deleting editor halfway through split!
}


void 
oofRepEditingEnv::startEnvironment()
{
	#ifdef OOF_REPEDIT_LOG_FILENAME
		ofstream logStream(OOF_REPEDIT_LOG_FILENAME,ios::app);  
		logStream 
			<< endl << endl << "oofRepEditingEnv::startEnvironment" 
#ifdef _MSC_VER
// stupid VC5 bug with manipulator forces less efficient create temp
			<< '\t' << dbDateTime::currentTime() << endl 
#else
			<< '\t' << dbDateTime::now << endl 
#endif
			<< oofString('-', 60) << endl
		;
		logStream.close();
	#endif
	delete mInteractionPages;  // maybe drawing again after oofRepWindow::ReflowReport
	mInteractionPages = new OOF_InteractionPages;  // use pointer so deletable and can hide definition
	mInteractionPages->addPage();
	assert(mInteractionPages->count()==1);
	assert(!mLastAreaOnPrevPage); ;

	parentGUIenvT::startEnvironment();
}


void 
oofRepEditingEnv::endEnvironment()
{
	// we don't delete mAreas here but leave it active now that we've finished
	// drawing the report it maps, the user is using the preview window to possibly
	// interact with the report, mapped through mAreas to oofRepEditLink's
	assert(!mLastAreaOnPrevPage);  // shouldn't be ending draw halfway through split!

	parentGUIenvT::endEnvironment();
}




/**
Add entry used in Flyweight pattern to react to clicks, etc.
*/
void 
oofRepEditingEnv::addInteractionArea(
	short inX, short inY,  short inCol, long inRow, 
	oofRepEditLink* inEditor, oofRepInteractorStrategy* inInteractor, 
	short inWidth, short inHeight
)
{
	#ifdef OOF_REPEDIT_LOG_FILENAME
		ofstream logStream(OOF_REPEDIT_LOG_FILENAME,ios::app);  
		logStream << "Adding Area (x,y,col,row,wid,ht,pagenumber): " 
			<< inX << ", " << inY << ", " 
			<< inCol << ", " << inRow << ", "
			<< inWidth << ", " << inHeight << ", "
			<< (mInteractionPages->count()-1) << endl;
		logStream.close();
	#endif

	if (!inInteractor)
		inInteractor = editor()->defaultInteractor();  // which strategy may have been reset by subclass of oofRepEditor
	mInteractionPages->latestPage().addInteractionArea(
		inX, inY,  inCol, inRow, 
		inEditor, inInteractor, inWidth, inHeight
	);	
}
	

void 
oofRepEditingEnv::addInteractionArea(const OOF_InteractionArea& inArea)
{
	#ifdef OOF_REPEDIT_LOG_FILENAME
		ofstream logStream(OOF_REPEDIT_LOG_FILENAME,ios::app);  
		logStream << "Adding Area (x,y,col,row,pagenumber): " << inArea.X << ", " << inArea.Y << ", " << inArea.mCol << ", " << inArea.mRow <<", "<< (mInteractionPages->count()-1) << endl;
		logStream.close();
	#endif
	mInteractionPages->latestPage().addInteractionArea(inArea);	
}

	
void 
oofRepEditingEnv::changeAreasToNewLink(long matchingRow, oofRepEditLink* matchingLink, oofRepEditLink* newLink)
{
	assert(matchingLink!=newLink);
	mInteractionPages->latestPage().changeAreasToNewLink(matchingRow, matchingLink, newLink);
}


void 
oofRepEditingEnv::startNonEditableArea()
{
	const unsigned long yPos = oofRep::currentReport()->currentVerticalPos();
	mInteractionPages->latestPage().startNonEditableArea(yPos);
}


void	
oofRepEditingEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
	drawable->makeRender(*this);
}


void	
oofRepEditingEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}


void  
oofRepEditingEnv::drawPageBreak()
{
	parentGUIenvT::drawPageBreak();
	// update current page
	mInteractionPages->addPage();
}


/**
	Save a copy of the interaction area that we can use to create a new one
	at the top of the next page.
*/
void 
oofRepEditingEnv::splitBandOverPage()
{
	assert(!mLastAreaOnPrevPage);
	OOF_InteractionArea* splittingArea = mInteractionPages->latestPage().lastAreaAdded();
	assert(splittingArea);  // page can't be empty if we're splitting!
	mLastAreaOnPrevPage = new OOF_InteractionArea( *splittingArea );
	assert(mLastAreaOnPrevPage);  // page can't be empty if we're splitting!
}


void 
oofRepEditingEnv::resumeBandOverPage()
{
	assert(mLastAreaOnPrevPage);
	pageAdoptsEditLink(mLastAreaOnPrevPage->mEditLink);
	delete mLastAreaOnPrevPage;
	mLastAreaOnPrevPage = 0; 
}


void 
oofRepEditingEnv::singleClick(short inX, short inY)
{
	if (!mInteractionPages)
		return;  // in case click forwarded before finish setup - has happened in testing!
		
	const unsigned long currentPageNumber = previewPageOrdinal();
	OOF_InteractionArea* theNearestArea = mInteractionPages->page(currentPageNumber).findAreaBelowAndRightOf(inX, inY);
	#ifdef OOF_REPEDIT_LOG_FILENAME
		ofstream logStream(OOF_REPEDIT_LOG_FILENAME,ios::app);
		if (theNearestArea)
			logStream 
				<< "singleClick (x,y) @: " << inX << ", " << inY << " matching "  
				<< theNearestArea->X << ", " << theNearestArea->Y << endl
			;
		else
			logStream << "singleClick (x,y, no match) @: " << inX << ", " << inY  << endl;
		logStream.close();
	#endif

	if (theNearestArea) {
		theNearestArea->singleClick(inX, inY, this);
	}
}


// -------------------------------------------------------
//          o o f R e p P i c t B l o c k
// -------------------------------------------------------
void	
oofRepPictBlock::makeRender(const oofRepEditingEnv& inEnv)
{
	makeRender(*inEnv.parentGUIenv());
	oofRepRender* wrappedRender = mRender;
	mRender = inEnv.editor()->makeDefaultBlockRenderWrapper(wrappedRender);
}


// -------------------------------------------------------
//          o o f R e p T e x t B l o c k
// -------------------------------------------------------
void	
oofRepTextBlock::makeRender(const oofRepEditingEnv& inEnv)
{
	makeRender(*inEnv.parentGUIenv());
	oofRepRender* wrappedRender = mRender;
	mRender = inEnv.editor()->makeDefaultBlockRenderWrapper(wrappedRender);
}


// -------------------------------------------------------
//   O O F _ I n t e r a c t i o n P a g e A r e a s
// -------------------------------------------------------
OOF_InteractionPageAreas::OOF_InteractionPageAreas() :
	oofArray(sizeof(OOF_InteractionArea), 20, 60), // estimate likely need up to 20 (expand big chunks)
	mAreas(0)
{
	mUserStorage = (void**)&mAreas;  // so parent can adjust ours as we expand

}


OOF_InteractionPageAreas::~OOF_InteractionPageAreas()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = mAreas = 0;  // avoid double-delete by zeroing mBits
	
	const unsigned long numOwnedLinks = mOwnedLinks.count();
	for (unsigned long i=0; i<numOwnedLinks; i++) {
		oofRepEditLink* theLink = (oofRepEditLink*) mOwnedLinks.value(i);  // safe downcast
		assert(theLink);
		theLink->decRefs();  // may cause delete
	}
}


void* 
OOF_InteractionPageAreas::AllocBits(unsigned long numBits) const
{
	OOF_InteractionArea* ret = new OOF_InteractionArea[numBits];
	return ret;
}


void 
OOF_InteractionPageAreas::DeleteBits(bool /* becauseExpanding */)
{
// we are supposed to know our own data pointer
// this gives us a chance to do a deep delete if !becauseExpanding
		delete[] mAreas;
}


OOF_InteractionArea*
OOF_InteractionPageAreas::findAreaBelowAndRightOf(short inX, short inY)
{
	unsigned long lastMatch=0;
	bool		  haveMatch = false;
	
	
	const unsigned long numItems = count();
	for (unsigned short i=0; i<numItems; i++) {
		OOF_InteractionArea& theArea = mAreas[i];
		if ( theArea.matchesPageLocation(inX, inY) ){
			lastMatch = i;
			haveMatch = true;
		} 
		
	}
	
	if(haveMatch){
		OOF_InteractionArea& resultArea = mAreas[lastMatch];
		return &resultArea;
	}
	else
		return NULL;
}


void
OOF_InteractionPageAreas::addInteractionArea(short inX, short inY,  short inCol, long inRow, oofRepEditLink* inEditor, oofRepInteractorStrategy* inInteractor, short inWidth, short inHeight)
{
	assert(inEditor);
	unsigned long allocSlot = AllocSlot();
	OOF_InteractionArea& theArea = mAreas[allocSlot];
	theArea.X = inX;
	theArea.Y = inY;
	theArea.mCol = inCol;
	theArea.mRow = inRow;
	theArea.mEditLink = inEditor;
	theArea.mInteractionStrategy = inInteractor;
	theArea.mWidth = inWidth;
	theArea.mHeight = inHeight;
}


void 
OOF_InteractionPageAreas::addInteractionArea(const OOF_InteractionArea& inArea)
{
	unsigned long allocSlot = AllocSlot();
	OOF_InteractionArea& theArea = mAreas[allocSlot];
	theArea = inArea;
}



OOF_InteractionArea*
OOF_InteractionPageAreas::lastAreaAdded() const
{
	const unsigned long numItems = count();
	if (numItems>0) {
		OOF_InteractionArea& theArea = mAreas[numItems-1];
			return &theArea;
	}		
	else
		return NULL;	
}


/**
Add edit links to index so for links that carry on over pages we can 
find out on which page they start, and adopt those starting on our page
so we can clean them up later
*/
void 
OOF_InteractionPageAreas::pageAdoptsEditLink(oofRepEditLink* inLink)
{
	assert(sizeof(unsigned long)==sizeof(oofRepEditLink*));  // yes I know this is a bit dodgy - guards
	// indicating we'll have to change our array types on some 64 bit platforms!
	const unsigned long asValue = (unsigned long) inLink;
	mOwnedLinks.append(asValue);
	inLink->incRefs();  // possibly share responsibility for triggering deletes
}


void 
OOF_InteractionPageAreas::changeAreasToNewLink(long matchingRow, oofRepEditLink* matchingLink, oofRepEditLink* newLink)
{
	const unsigned long numItems = count();
	for (unsigned short i=0; i<numItems; i++) {
		OOF_InteractionArea& theArea = mAreas[i];
		if ((theArea.Y==matchingRow) && (theArea.mEditLink==matchingLink) )
			theArea.mEditLink = newLink;
	}
}


/**
Sensible algorithm is only set those in the last row
however earlier rows will then match due to their zero height.
So must take care of all zero heights

Note main assumption is that a row either has zero height throughout
or not, which matches the behaviour of view and layout bands.

A layout band mixing zero and fixed heights would be a problem!
*/
void 
OOF_InteractionPageAreas::startNonEditableArea(unsigned long newY)
{
	const unsigned long numItems = count();
	if (numItems==0)
		return;
	
	const unsigned long lastItem = numItems-1;	
	if 	(mAreas[lastItem].mHeight>0)
		return;  // don't bother terminating row with fixed heights
		
	short lastY = mAreas[lastItem].Y;
	assert(newY>=lastY);
	short newHeight = newY - lastY;
	
	for (long i=lastItem; i>=0; i--) {
		OOF_InteractionArea& theArea = mAreas[i];
		if (theArea.Y!=lastY) {  // finished a row, adjust figures
			if (theArea.mHeight>0)
				return;  // terminate as soon as hit row with height
			const short prevRowY = theArea.Y;
			newHeight = lastY - prevRowY;
			lastY = prevRowY;
		}
		theArea.mHeight = newHeight;
	}
}

// -------------------------------------------------------
//       O O F _ I n t e r a c t i o n P a g e s
// -------------------------------------------------------
OOF_InteractionPages::OOF_InteractionPages() :
	oofArray(sizeof(OOF_InteractionPageAreas), 10, 60), // estimate likely need up to 20 (expand big chunks)
	mPages(0)
{
	mUserStorage = (void**)&mPages;  // so parent can adjust ours as we expand
}


OOF_InteractionPages::~OOF_InteractionPages()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = mPages = 0;  // avoid double-delete by zeroing mBits
}


void* 
OOF_InteractionPages::AllocBits(unsigned long numBits) const
{
	OOF_InteractionPageAreas* ret = new OOF_InteractionPageAreas[numBits];
	return ret;
}


void 
OOF_InteractionPages::DeleteBits(bool becauseExpanding)
{
// don't delete when expanding if our items are objects 
// unless we've copied them somehow other than the default bitwise
	if (!becauseExpanding)
		delete[] mPages;
}


// -------------------------------------------------------
//       O O F _ R e p B l o c k C e l l R o w
// -------------------------------------------------------
OOF_RepBlockCellRow::OOF_RepBlockCellRow() :
	oofArray(sizeof(OOF_RepBlockCellMap), 3, 2),
	mMaps(0)
{
	mUserStorage = (void**)&mMaps;  // so parent can adjust ours as we expand
}


OOF_RepBlockCellRow::~OOF_RepBlockCellRow()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = mMaps = 0;  // avoid double-delete by zeroing mBits
}


void* 
OOF_RepBlockCellRow::AllocBits(unsigned long numBits) const
{
	OOF_RepBlockCellMap* ret = new OOF_RepBlockCellMap[numBits];
	return ret;
}


void 
OOF_RepBlockCellRow::DeleteBits(bool /* becauseExpanding */)
{
// we are supposed to know our own data pointer
// this gives us a chance to do a deep delete if !becauseExpanding
		delete[] mMaps;
}


void 
OOF_RepBlockCellRow::addBlockCells(oofRepBlock* inBlock)
{
	const unsigned int numCells = inBlock->countCells();
	const unsigned long newMax = count() + numCells;
	ExpandToInclude(newMax);  // single-hit expansion instead of (typically) 3!
	for (unsigned int i=0; i<numCells; i++)
		AddMap(i, inBlock);
}


bool 
OOF_RepBlockCellRow::findBlock(oofRepBlock* inBlock, short& outCol) const
{
	const unsigned long numMaps = count();
	for (unsigned long i=0; i<numMaps; i++) {  // top-down search loop
		if (mMaps[i].mBlock==inBlock) {
			outCol = i;
			return true;
		}
	}
	return false;
}


void 
OOF_RepBlockCellRow::AddMap(short inIndex, oofRepBlock* inBlock)
{
	const unsigned long newIndex = AllocSlot();
	mMaps[newIndex].mCellIndex = inIndex;
	mMaps[newIndex].mBlock = inBlock;
}

// -------------------------------------------------------
//       O O F _ R e p B l o c k C e l l R o w s
// -------------------------------------------------------
OOF_RepBlockCellRows::OOF_RepBlockCellRows() :
	oofArray(sizeof(OOF_RepBlockCellRow), 3, 2),
	mRows(0)
{
	mUserStorage = (void**)&mRows;  // so parent can adjust ours as we expand
}


OOF_RepBlockCellRows::~OOF_RepBlockCellRows()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = mRows = 0;  // avoid double-delete by zeroing mBits
}


void* 
OOF_RepBlockCellRows::AllocBits(unsigned long numBits) const
{
	OOF_RepBlockCellRow* ret = new OOF_RepBlockCellRow[numBits];
	return ret;
}


void 
OOF_RepBlockCellRows::DeleteBits(bool becauseExpanding)
{
// don't delete when expanding if our items are objects 
// unless we've copied them somehow other than the default bitwise
	if (!becauseExpanding)
		delete[] mRows;
}






// -------------------------------------------------------
//   O O F _ r e p E d it S e t u p V i s i t o r
// -------------------------------------------------------
/**
	Setup databases for editing.
	In initial implementation used dbTable::bufferForever so no changes in
	preview window would be propagated back to database.
	If overriden, could do any setup you liked.
	\par
	Created by overrideable factory oofRepEditor::makeRepEditingSetupVisitor.
	\ingroup oofRepEdit
*/
class OOF_repEditSetupVisitor : public OOF_repBufferSettingVisitor {
public:
	OOF_repEditSetupVisitor(bool settingBufferForever=true) : 
		OOF_repBufferSettingVisitor(settingBufferForever)
	{};
	
protected:
	virtual void VisitViewBandBody(oofRepViewBand*);
};



/**
	Set databases associated with view to mode where editing is sensible.
	Set editable state as well as buffer mode like 
	OOF_repBufferSettingVisitor::VisitView.
	This is optimised by incorporating the logic from OOF_repBufferSettingVisitor
	so we do a single pass across the table and its fields.
	
	We play it very safe by worrying about whether a field is a join key even
	if there are no dependents on that join in the report. This simplifies tracing
	the dependencies at the cost of a bit more lookup work below.
*/
void 
OOF_repEditSetupVisitor::VisitViewBandBody(oofRepViewBand* inBand)
{
	dbView* inView = inBand->view();  

	dbTable* viewTable = inView->table();
	if (viewTable) {
		if (mSettingBufferForever)
			viewTable->setSaveOption(dbTable::bufferForever);
		else {  // reset
			if (viewTable->isRelatedClone())
				viewTable->setSaveOption(dbTable::requireExplicitAndBuffer);
			else
				viewTable->setSaveOption(dbTable::requireExplicit);			
		}
	}
// now iterate all fields in case any of them point to related tables
// which also require their save option changing
	const unsigned int numFields = inView->count();
	const dbTable::saveOptionsT newOption = mSettingBufferForever ? 
		dbTable::bufferForever : 
		dbTable::requireExplicitAndBuffer;  // if not setting, assume all related
	
	for (unsigned int col=0; col<numFields; col++) {		// start field iteration (horizontal)
		const dbField& theField = inView->field(col);
		dbTable* fieldTable = theField.fieldTable();
		if (fieldTable && fieldTable!=viewTable)
			fieldTable->setSaveOption(newOption);
		
		// now evaluate if can edit
		if (theField.fieldIsVirtual()  || theField.fieldIsJoinKey())
			inBand->setFieldNonEditable(col); 

	} // loop all fields
}


// -------------------------------------------------------
//          o o f R e p E d i t o r
// -------------------------------------------------------
oofRepEditor::oofRepEditor() :
	mInteractionMappingActive(true),
	mInteractsViaClicks(true), 
	mInteractsViaKeys(false),
	mDefaultInteractor(new oofRepInteractorSimpleClickStrategy)
{
}


oofRepEditor::~oofRepEditor()
{
	delete mDefaultInteractor;
}


oofRepEditor*
oofRepEditor::clone() const
{
	return new oofRepEditor(*this);
}


// overrideable factories
oofRepEditingEnv*
oofRepEditor::makeEditingEnvironment()
{
	return new oofRepEditingEnv(this);
}



oofRepRenderDecorator* 
oofRepEditor::makeDefaultRenderWrapper(oofRepRender* adoptedRender)
{
	return new oofNonEditableRedirectingRender(adoptedRender);
}


oofRepRenderDecorator* 
oofRepEditor::makeDefaultBlockRenderWrapper(oofRepRender* adoptedRender)
{
	return new oofRepBlockRedirectingRender(adoptedRender);
}


oofRepRenderDecorator* 
oofRepEditor::makeLayoutRenderWrapper(oofRepLayoutBand_Render* adoptedRender)
{
	return new oofRepLayoutRedirectingRender(adoptedRender);
}


oofRepViewRedirectingRender* 
oofRepEditor::makeViewRenderWrapper(oofRepViewBand_Render* adoptedRender)
{
	return new oofRepViewRedirectingRender(adoptedRender);
}


oofRepEditLink* 
oofRepEditor::makeColHeaderEditLink(oofRepViewBand* inViewBand)
{
	return new oofRepColHeaderEditLink(inViewBand);
}


oofRepLayoutEditLink* 
oofRepEditor::makeLayoutEditLink(oofRepLayoutBand* inLayoutBand)
{
	return new oofRepLayoutEditLink(inLayoutBand);
}


oofRepEditLink* 
oofRepEditor::makeViewEditLink(oofRepViewBand* inViewBand, bool editingSubSelection)
{
	return new oofRepViewEditLink(inViewBand, editingSubSelection);
}


/**
	Overrideable factory for editsetup so you can invoke your own code.
	\see oofRep::draw
*/
oofRepVisitor* 
oofRepEditor::makeRepEditingSetupVisitor()
{
	return new OOF_repEditSetupVisitor;
}



// -------------------------------------------------------
//  o o f R e p I n t e r a c t o r S i m p l e C l i c k S t r a t e g y
// -------------------------------------------------------
void
oofRepInteractorSimpleClickStrategy::singleClick(short inX, short inY, OOF_InteractionArea* inArea, oofRepEditingEnv* inEnv)
{
	oofRepEditor* theEditor = inEnv->editor();
	assert(theEditor);
	if (theEditor->respondsToClicks()) {
		theEditor->startEditing(inArea, inEnv);
	}
}



// -------------------------------------------------------
//          o o f R e p E d i t L i n k
// -------------------------------------------------------
/**
Bunch of default methods as can't implement this class - must subclass
to at least provide getCellText and setCellText.

Provides defaults on the assumption of a single-celled drawable.
*/
oofRepEditLink::oofRepEditLink(oofRepDrawable* inD) :
	dbFieldLink(NULL, NULL),
	OOF_mixRefCount(0),  // don't have any refs until adopted
	mDrawable(inD),
	mCurrentCol(-1),
	mLastRow(0),
	mCurrentRow(-1),
	mLastCol(0)
{
}
	
	
	
oofRepEditLink::~oofRepEditLink()
{
	mAdapter = 0;   // we do NOT own it, unlike our parent dbFieldLink's expectation
}
	

// style access

/**
textStyle returns a writeable pointer so allows updates
HOWEVER the cascading style model means this may be a pointer to a parent
style so it is NOT recommended to update this value.

See OOFRep_DesignDecisions.txt for discussion of issues
*/
oofRepTextStyle*
oofRepEditLink::textStyle() const
{
	return mDrawable->textStyle();
}


oofRepTextStyle*
oofRepEditLink::localTextStyle() const
{
	return mDrawable->localTextStyle();
}


bool 
oofRepEditLink::allowsCellTextStyles() const
{
	return false;
}

	
// reflective methods to query link
bool 
oofRepEditLink::hasCustomViewDrawers() const
{
	return false;
}


oofRepCustomViewDrawer* 
oofRepEditLink::customViewDrawer(long /*col*/) const
{
	return NULL;
}


void 
oofRepEditLink::enterLink(dbGUIAdapter* inAdapter) 
{
	mAdapter = inAdapter;
	inAdapter->setFieldLink(this);
}


bool 
oofRepEditLink::leaveLink()
{
	owner(NULL);
	return true;
}


void
oofRepEditLink::tellGUIcareAboutLocking() const
{
// empty virtual to hide parent
}



// movement testing - default base assumes on first and only cell

/**
might override atFirst if you had irregular shape allowing first cell
to be further along first row, but usually use this base method
*/
bool 
oofRepEditLink::atFirst() const
{
	return ((mCurrentCol==0) && (mCurrentRow==0));
}


/**
Return true to indicate we are at the last cell already, same as atFirst()
*/
bool 
oofRepEditLink::getLastCoords(short& outCol, long& outRow) const
{
	outCol = mLastCol;
	outRow = mLastRow;
	return ((mCurrentCol==mLastCol) && (mCurrentRow==mLastRow));
}


bool 
oofRepEditLink::getLeftOfCurrent(short& outCol, long& outRow) const
{
	if (mCurrentCol>0) {
		outCol = mCurrentCol-1;
		outRow = mCurrentRow;
		return true;
	}
	else
		return false;
}


bool 
oofRepEditLink::getRightOfCurrent(short& outCol, long& outRow) const
{
	return false;
}


bool 
oofRepEditLink::getUpFromCurrent(short& outCol, long& outRow) const
{
	return false;
}


bool 
oofRepEditLink::getDownFromCurrent(short& outCol, long& outRow) const
{
	return false;
}




// -------------------------------------------------------
//   O O F _ m i x R e p A b s t r a c t C e l l E d i t o r
// -------------------------------------------------------

OOF_mixRepAbstractCellEditor::OOF_mixRepAbstractCellEditor() :
	dbHelper(0),  // no major table
	mAnyContentDirtied(false),
	mEnv(0),
	mCurrentLink(0),
	mAdapter(0)
{}


OOF_mixRepAbstractCellEditor::~OOF_mixRepAbstractCellEditor()
{
	delete mAdapter;
}
	

void 
OOF_mixRepAbstractCellEditor::FinishCreationByLinkingDataModel(oofRepEditingEnv* inEnv, oofRepEditLink* inLink)  // final after ctor and FinishCreateSelf
{
	assert(!mEnv);  // this is a construction step, not resetting model!
	mEnv = inEnv;  // may utterly ignore if can't move to other items
	EnterLink(inLink);  // point to it and load data
}
	

void 
OOF_mixRepAbstractCellEditor::EnterLink(oofRepEditLink* inLink) 
{
	assert(!mCurrentLink);
	assert(mAdapter);  // test if user forgot InitEditControls in their subclass
	mCurrentLink = inLink;  // this is data link for the cells we're editing now!
	inLink->enterLink(mAdapter);
	inLink->owner(this);  // init the dbFieldLink side to think we own it!
	
	LoadAndUpdate();
}

	
// optional behavioural overrides
/**
We are initialised with a cell position and maintain it ourselves
so can query the link and enable buttons to suit

If you don't want your buttons disabled, don't pass them into the 
InitMovementControls.
*/
void 
OOF_mixRepAbstractCellEditor::UpdateMovementButtons()  
{
// override if want disabling etc. 
}


/**
Warning method called when try to trigger a movement and we 
can't move in that direction (eg: GoLeftCell when already at
leftmost cell). You may wish to override with an alert.
*/
void 
OOF_mixRepAbstractCellEditor::UnableToMoveWarning(movementT, bool /*movingByCell*/)  // default beep
{
	oofRuntimeEnv::beep();
}



// optional actions to override

/**
Typical reasons to override - want to display location or a different display
of the editable state (eg: visible lock icon)
*/
void 
OOF_mixRepAbstractCellEditor::LoadCellEditField()
{
	assert (mCurrentLink);
	oofString cellText = mCurrentLink->getCellText();
	cellText.lineEndingsCurrentPlatform();  // or could have application call dbText::convertsLineEndingsOnLoad(true)
	mAdapter->setString(cellText);	
}


bool 
OOF_mixRepAbstractCellEditor::AttemptSaveCellEditField()
{
	assert (mCurrentLink);
	if (!mCurrentLink->isDirty())  // don't bother saving if not dirty
		return true;
		
	mCurrentLink->setCellText(mAdapter->getString());
	return true;  // NOT YET IMPLEMENTED - failure returns
}


void 
OOF_mixRepAbstractCellEditor::GoToAnotherCell(movementT inMove)
{
	bool couldMove = false;
	short newCol;
	long newRow ;
	
	switch (inMove) {
		case (eGoFirst) :
			if (!mCurrentLink->atFirst()) {
				couldMove = true;  // may still not move if Save cancels the attempt
				newCol = 0;
				newRow = 0;
			}
			break;
			
		case (eGoLast) : {
				couldMove = !mCurrentLink->getLastCoords(newCol, newRow);
			}
			break;
			
		case (eGoLeft) :
			couldMove = mCurrentLink->getLeftOfCurrent(newCol, newRow);
			break;
			
		case (eGoRight) :
			couldMove = mCurrentLink->getRightOfCurrent(newCol, newRow);
			break;
			
		case (eGoUp) :
			couldMove = mCurrentLink->getUpFromCurrent(newCol, newRow);
			break;
			
		case (eGoDown) :
			couldMove = mCurrentLink->getDownFromCurrent(newCol, newRow);
			break;
			
		default : {
		}
			
	}
	
	if (couldMove) {
		if (AttemptSaveCellEditField()) {
			mCurrentLink->setCurrentCoords(newCol, newRow);
			LoadAndUpdate();
		}
	}
	else
		UnableToMoveWarning(inMove, true);  
}


/**
separation of these three steps, which commonly occurs, is example of the Template Method pattern
allowing for overriding of minor points
*/
void 
OOF_mixRepAbstractCellEditor::LoadAndUpdate()
{
	assert(mCurrentLink);
	mCurrentLink->markClean();
	LoadCellEditField();
	UpdateMovementButtons();
}


/**
Separate out item movement from cell movement because you may wish to implement one
but use default for the other. eg: use default moves to items but custom cells
because you have fancy report objects in your application.
*/
void 
OOF_mixRepAbstractCellEditor::GoToAnotherItem(movementT inMove)
{
	UnableToMoveWarning(inMove, false);  // warn because we don't know how to do it!
	return;  // until we actually implement the find prev/next below
// remove the above two lines when movement implemented below
	
	assert(mCurrentLink);
	oofRepEditLink* newLink = 0;
	switch (inMove) {
		case (eGoUp) :
			// NOT YET IMPLEMENTED - find prev item
			break;
			
		case (eGoDown) :
			// NOT YET IMPLEMENTED - find next item
			break;
			
		default :
			UnableToMoveWarning(inMove, false);  // don't support most movement
	}
	if (newLink) {
		mCurrentLink->leaveLink();
		mCurrentLink = 0;
		EnterLink(newLink);
	}
}


void 
OOF_mixRepAbstractCellEditor::maybeDirty(dbFieldLink*)
{
	mAnyContentDirtied = true;  // latch dirtying of a cell so can test when save
}



// -------------------------------------------------------
//       o o f R e p L a y o u t E d i t L i n k
// -------------------------------------------------------
oofRepLayoutEditLink::oofRepLayoutEditLink(oofRepLayoutBand* inBand) :
	oofRepEditLink(inBand)
{}


// cell content access methods
oofString 
oofRepLayoutEditLink::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col)
{
	if (row==-1)
		row = mCurrentRow;
	if (col==-1)
		col = mCurrentCol;
	
	short cellInBlock;
	oofRepBlock* theBlock = MapCoordsToBlock(row, col, cellInBlock);	
	assert(theBlock);
	// row/col for block is in terms of cells within block NOT within layout band
	bool isBlockEditable;
	const oofString ret = theBlock->getCellText(&isBlockEditable, outLocationName, 0, cellInBlock);
	if (outIsEditable)
		*outIsEditable = isBlockEditable;
	readOnly(!isBlockEditable); 
	return ret;
}


void
oofRepLayoutEditLink::setCellText(const oofString& inString, long row, short col)
{
	if (row==-1)
		row = mCurrentRow;
	if (col==-1)
		col = mCurrentCol;
	
	short cellInBlock;
	oofRepBlock* theBlock = MapCoordsToBlock(row, col, cellInBlock);	
	assert(theBlock);
	theBlock->setCellText(inString, 0, cellInBlock);
}



// reflective methods to query link
bool 
oofRepLayoutEditLink::allowsCellTextStyles() const
{
	return true;
}


/**
Our mapping may have holes in it but we treat them
as short rows rather than trying to correspond to the 
centring or right-alignment 
(which have to be in terms of the max number of cols in the entire layout!)
*/
void 
oofRepLayoutEditLink::initMapping()
{
// leave mLastCol as 0 as we have to check row by row

	oofRepLayoutBand* theLayout = Drawable();
	const oofRepBlockList& theBlocks = theLayout->blocks();
	const unsigned long numBlocks = theLayout->blocks().count();
	assert(numBlocks>0);  // guarantees at least one row
	
	// iterate all blocks, counting cells and putting into rows/cols
	bool nextBlockStartsRow = true;
	long numRows = 0;
	OOF_RepBlockCellRow* currentRow = 0;
	for (unsigned long i=0; i<numBlocks; i++) {
		if (nextBlockStartsRow) {
			currentRow = mRows.addRow();
			numRows++;
		}
			
		oofRepBlock* theBlock = theBlocks.value(i);
		assert(theBlock);
		
		currentRow->addBlockCells(theBlock);
		if (theBlock->moveDownAfter())
			nextBlockStartsRow = true;
		else
			nextBlockStartsRow = false;
	}  // loop blocks
	mLastRow = numRows-1;
}


oofRepBlock* 
oofRepLayoutEditLink::MapCoordsToBlock(long inRow, short inCol, short& outCellInBlock) const
{
	OOF_RepBlockCellRow& theRow = mRows.row(inRow);
	OOF_RepBlockCellMap& theMap = theRow.map(inCol);
	oofRepBlock*  ret = theMap.mBlock;
	outCellInBlock = theMap.mCellIndex;
	return ret;
}


void 
oofRepLayoutEditLink::getFirstCoordsForBlock(oofRepBlock* inBlock, short& outCol, short& outRow) const
{
	const unsigned long numRows = mRows.count();
	for (unsigned long i=0; i<numRows; i++) {  // top-down search loop
		OOF_RepBlockCellRow& theRow = mRows.row(i);
		if (theRow.findBlock(inBlock, outCol)) {  // first of many possible in a row
			outRow = i;
			return;
		}
	}
	outRow = outCol = -1;
}


bool 
oofRepLayoutEditLink::getRightOfCurrent(short& outCol, long& outRow) const
{
	const long lastColInRow = mRows.row(mCurrentRow).count()-1;
	if (mCurrentCol<lastColInRow) {
		outCol = mCurrentCol+1;
		outRow = mCurrentRow;
		return true;
	}
	else
		return false;
}


bool 
oofRepLayoutEditLink::getUpFromCurrent(short& outCol, long& outRow) const
{
	if (mCurrentRow>0) {
		const long newRow = mCurrentRow-1;
		const long lastColInRow = mRows.row(newRow).count()-1;
		if (lastColInRow<mCurrentCol)
			outCol = lastColInRow;  // can't move up vertically but jump in a bit to last cell
		else
			outCol = mCurrentCol;
		outRow = newRow;
		return true;
	}
	else
		return false;
}


bool 
oofRepLayoutEditLink::getDownFromCurrent(short& outCol, long& outRow) const
{
	if (mCurrentRow<mLastRow) {
		const long newRow = mCurrentRow+1;
		const long lastColInRow = mRows.row(newRow).count()-1;
		if (lastColInRow<mCurrentCol)
			outCol = lastColInRow;  // can't move down vertically but jump in a bit to last cell
		else
			outCol = mCurrentCol;
		outRow = newRow;
		return true;
	}
	else
		return false;
}


// -------------------------------------------------------
//          o o f R e p V i e w E d i t L i n k
// -------------------------------------------------------
oofRepViewEditLink::oofRepViewEditLink(oofRepViewBand* inBand, bool editingSubSelection) :
	oofRepEditLink(inBand),
	mSelection(0),
	mRecordNumber(0)
{
	dbView* theView = inBand->view();
	dbTable* viewTable = theView->table();
	mLastCol = theView->count() - 1;
	assert(mLastCol>=0);  // check for empty view
	
	if (viewTable) {
		mLastRow = viewTable->count();
		if (mLastRow>0)
			--mLastRow;  // valid to pass in empty selection so check before decrement
			
		if (editingSubSelection) {
			dbTable* tableToReset = viewTable->baseTableOfRelationChain();
			// tableToReset may be the same table as the view, if the sub selection
			// was on a unique table rather than a child related to the top view
			if (viewTable==tableToReset)
				mSelection = new dbSelection(tableToReset->currentSelection());
			else
				mRecordNumber = tableToReset->recordNumber();  
		}
	}
	else
		mLastRow = 0;
}


oofRepViewEditLink::~oofRepViewEditLink()
{
	delete mSelection;
}


// init and cleanup code when edit dialog launches and if it changes links
void 
oofRepViewEditLink::enterLink(dbGUIAdapter* inAdapter)
{
	oofRepEditLink::enterLink(inAdapter);
	dbTable* viewTable = View()->table();
	if (viewTable) {
		dbTable* tableToReset = viewTable->baseTableOfRelationChain();
		if (mSelection)
			tableToReset->setSelection(*mSelection);
		else
			tableToReset->gotoRecord(mRecordNumber);  
	}  // has a table	
}


// cell content access methods
oofString 
oofRepViewEditLink::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col)
{
	if (row==-1)
		row = mCurrentRow;
	if (col==-1)
		col = mCurrentCol;
			
	assert(row<=mLastRow && col<=mLastCol);
	
	dbTable* viewTable = View()->table();
	if (viewTable) {
		viewTable->gotoRecord(row);
	}
	else {
		assert(row==0);  // only one row if no table
	}


	const dbField& theField = View()->field(col);

	const bool unableToEdit = Drawable()->fieldIsNonEditable(col); 
	readOnly(unableToEdit); 
	// NOT YET IMPLEMENTED allow for database locking - may fail if allowed updates
	// safe in these initial circumstances where all updates to database are buffered locally
	// due to bufferForever mode used on all report views

	if (outIsEditable)
		*outIsEditable = !unableToEdit;
	
	if (outLocationName) {
		const oofString theHeading = View()->getHeading(col);
		if (theHeading.isEmpty())
			outLocationName->convertNumber(col, "col %d");
		else
			*outLocationName = theHeading;
		outLocationName->convertNumber((unsigned long)row, " / %d", true /* append */);
	}

	return theField.copyString();
}


void 	
oofRepViewEditLink::setCellText(const oofString& inString, long row, short col)
{
	if (row==-1)
		row = mCurrentRow;
	if (col==-1)
		col = mCurrentCol;
			
	assert(row<=mLastRow && col<=mLastCol);
	
	dbTable* viewTable = View()->table();
	if (viewTable) {
		viewTable->gotoRecord(row);
	}
	else {
		assert(row==0);  // only one row if no table
	}

	View()->writeableField(col)->setString(inString);
}


bool 
oofRepViewEditLink::getRightOfCurrent(short& outCol, long& outRow) const
{
	if (mCurrentCol<mLastCol) {
		outCol = mCurrentCol+1;
		outRow = mCurrentRow;
		return true;
	}
	else
		return false;
}


bool 
oofRepViewEditLink::getUpFromCurrent(short& outCol, long& outRow) const
{
	if (mCurrentRow>0) {
		outCol = mCurrentCol;
		outRow = mCurrentRow-1;
		return true;
	}
	else
		return false;
}


bool 
oofRepViewEditLink::getDownFromCurrent(short& outCol, long& outRow) const
{
	if (mCurrentRow<mLastRow) {
		outCol = mCurrentCol;
		outRow = mCurrentRow+1;
		return true;
	}
	else
		return false;
}


bool 
oofRepViewEditLink::allowsCellTextStyles() const
{
	return false;  // may change with adorners!
}


bool 
oofRepViewEditLink::hasCustomViewDrawers() const
{
	return Drawable()->hasCustomViewDrawers();
}


oofRepCustomViewDrawer* 
oofRepViewEditLink::customViewDrawer(long col) const
{
	return Drawable()->customDrawer(col);
}


// -------------------------------------------------------
//     o o f R e p C o l H e a d e r E d i t L i n k
// -------------------------------------------------------
oofRepColHeaderEditLink::oofRepColHeaderEditLink(oofRepViewBand*inBand) :
	oofRepEditLink(inBand)
{
	dbView* theView = inBand->view();
	mLastCol = theView->count() - 1;
}


bool 
oofRepColHeaderEditLink::getRightOfCurrent(short& outCol, long& outRow) const
{
	if (mCurrentCol<mLastCol) {
		outCol = mCurrentCol+1;
		outRow = 0;
		return true;
	}
	else
		return false;
}

// cell content access methods
oofString 
oofRepColHeaderEditLink::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col)
{
	if (row==-1)
		row = 0;
	if (col==-1)
		col = mCurrentCol;
			
	assert(row<=mLastRow && col<=mLastCol);
	
	const bool canEdit = true; 
	readOnly(!canEdit); 

	if (outIsEditable)
		*outIsEditable = canEdit;
	
	if (outLocationName) {
		outLocationName->convertNumber(col, "Colum %d Heading");
	}

	return View()->getHeading(col);
}


/**
If there is no custom heading on this column, the heading will have come from the field name.
Writing to the heading always creates a custom heading.
*/
void 	
oofRepColHeaderEditLink::setCellText(const oofString& inString, long row, short col)
{
	if (row==-1)
		row = 0;
	if (col==-1)
		col = mCurrentCol;
			
	assert(row<=mLastRow && col<=mLastCol);
	
	View()->setHeading(col, inString);
	Drawable()->dirtyColHeaders();
}




// -------------------------------------------------------
//      o o f R e p R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
oofRepRedirectingRender::oofRepRedirectingRender(oofRepRender* adoptedRender) :
	oofRepRenderDecorator(adoptedRender),
	mEditLink(0)
{
}


oofRepRedirectingRender::~oofRepRedirectingRender()
{
}


bool 
oofRepRedirectingRender::editRedirectionActive() const
{
	oofRep* curRep = oofRep::currentReport();
	oofRepEnvironment* theEnv = curRep->environment();
	if (theEnv->isPrintingEnvironment())
		return false;

	return true;  // still rendering
}


// -------------------------------------------------------
//      o o f R e p L a y o u t R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
oofRepLayoutRedirectingRender*  oofRepLayoutRedirectingRender::sCurrentLayoutRedirectingRender;

oofRepLayoutRedirectingRender::oofRepLayoutRedirectingRender(oofRepLayoutBand_Render* adoptedRender) :
	oofRepRedirectingRender(adoptedRender),
	mEditLink(0)
{

}


/**
Sometimes we will be deleted and page interaction areas still active, eg:
if re-render a report from the preview window.

When we re-flow a report we want to avoid recalculating the edit link mapping
so we've kept a reference but the interaction areas will be deleted.
*/
oofRepLayoutRedirectingRender::~oofRepLayoutRedirectingRender()
{
	if(mEditLink)
		mEditLink->decRefs();
}


void 
oofRepLayoutRedirectingRender::draw()
{
	if (editRedirectionActive()) {
		sCurrentLayoutRedirectingRender = this;
		if (!mEditLink)  // create for use by addAreaForCurrentBlock
			SetupEditLink();			

		editingEnv()->pageAdoptsEditLink(mEditLink);  // shares ownership with us
	}
	
	mWrappedRender->draw();

	if (editRedirectionActive()) 
		sCurrentLayoutRedirectingRender = 0;
}


void 
oofRepLayoutRedirectingRender::SetupEditLink()
{
	oofRepLayoutBand* theLayout = LayoutRender()->ownerLayout();
	assert(!mEditLink);
	mEditLink = editor()->makeLayoutEditLink(theLayout);	
	mEditLink->initMapping();  // link knows enough to go through and create map
	mEditLink->incRefs();   // extra link because we want it to stay around even if interaction areas deleted 
}


oofRepRender* 
oofRepLayoutRedirectingRender::clone(const oofRepDrawable* newOwner) const
{
	oofRepLayoutBand_Render* clonedWrappee = (oofRepLayoutBand_Render*) mWrappedRender->clone(newOwner);  // safe downcast
	return new oofRepLayoutRedirectingRender(clonedWrappee);
}


/**
This is just a trigger callback from the wrapped layout render
calling block renderers - our wrapped layout render has stashed
the necessary coordinates to add an area.
*/
void
oofRepLayoutRedirectingRender::addAreaForCurrentBlock() const
{
	short xPos, yPos, col, row;
	oofRepBlock* theBlock;
	LayoutRender()->getCurrentBlockCoords(xPos, yPos, theBlock);
	mEditLink->getFirstCoordsForBlock(theBlock, col, row);  // block may have several cells
	editingEnv()->addInteractionArea(xPos, yPos, col, row, mEditLink,
		NULL /* strategy */, 0, theBlock->fixedHeight()
	);
	// could use theBlock->fixedWidth() but fails to take alignment into account
}


// -------------------------------------------------------
//  o o f R e p N o n E d i t a b l e R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
oofNonEditableRedirectingRender::oofNonEditableRedirectingRender(oofRepRender* adoptedRender) :
	oofRepRedirectingRender(adoptedRender)
{
}


void 
oofNonEditableRedirectingRender::draw()
{
	if (editRedirectionActive()) {
		editingEnv()->startNonEditableArea();
	}
	
// after all that processing, forward the draw	
	mWrappedRender->draw();
}


oofRepRender* 
oofNonEditableRedirectingRender::clone(const oofRepDrawable* newOwner) const
{
	oofRepRender* clonedWrappee = mWrappedRender->clone(newOwner);
	return new oofNonEditableRedirectingRender(clonedWrappee);
}




// -------------------------------------------------------
//      o o f R e p B l o c k R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
oofRepBlockRedirectingRender::oofRepBlockRedirectingRender(oofRepRender* adoptedRender) :
	oofRepRedirectingRender(adoptedRender)
{
}


void 
oofRepBlockRedirectingRender::draw()
{
	if (editRedirectionActive()) {
		oofRepLayoutRedirectingRender::currentLayoutRedirectingRender()->addAreaForCurrentBlock();
		// layout does all the work - it knows location!
	}
	
// after all that processing, forward the draw	
	mWrappedRender->draw();
}


oofRepRender* 
oofRepBlockRedirectingRender::clone(const oofRepDrawable* newOwner) const
{
	oofRepRender* clonedWrappee = mWrappedRender->clone(newOwner);
	return new oofRepBlockRedirectingRender(clonedWrappee);
}




// -------------------------------------------------------
//      o o f R e p V i e w R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
oofRepViewRedirectingRender::oofRepViewRedirectingRender(oofRepViewBand_Render* adoptedRender) :
	oofRepRedirectingRender(adoptedRender),
	mColHeaderLink(0)
{
}


oofRepViewRedirectingRender::~oofRepViewRedirectingRender()
{
	if(mColHeaderLink)
		mColHeaderLink->decRefs();
}


oofRepRender* 
oofRepViewRedirectingRender::clone(const oofRepDrawable* newOwner) const
{
	oofRepViewBand_Render* clonedWrappee = (oofRepViewBand_Render*) mWrappedRender->clone(newOwner);  // safe downcast
	return new oofRepViewRedirectingRender(clonedWrappee);
}


/**
Here we record the current position on entry to the view cell.

DrawContinuingOverPage is used to draw cells that split on to the next page
however the top of them comes here first so is still encountered.
*/
void 
oofRepViewRedirectingRender::draw()
{
// WARNING drawColumnHeaders() may well be called during the
// wrapped draw() below 

	if (editRedirectionActive()) {
	// record position before we do anything to manipulate
		AddRowInteractionAreas();
	}

// after all that processing, forward the draw
	mWrappedRender->draw();
}



void 
oofRepViewRedirectingRender::AddRowInteractionAreas()
{
	oofRep* curRep = oofRep::currentReport();

	dbView* theView = ViewBand()->view();
	assert(theView);
	oofColSizer* theSizer = theView->colSizer();	
	const long numCols = theView->count();
	const short halfColSepWidth = curRep->settings()->colSepWidth() / 2;
	
	assert(mEditLink);
	const bool hasAdorners = (ViewBand()->adorners().count() > 0);
	short xPos =  mWrappedRender->leftDrawMargin();
	const short yPos = curRep->currentVerticalPos();
	const long row = theView->recordNumber();
	for (unsigned short col=0; col<numCols; col++) {
		if (hasAdorners) // precompensate for gutter offset below
			xPos += halfColSepWidth;
		const long xPosInclGutter = xPos - halfColSepWidth;
		editingEnv()->addInteractionArea(xPosInclGutter, yPos, col, row, mEditLink);
		xPos += theSizer->width(col) + halfColSepWidth;
	}
}


OOF_mixViewExtraRender* 
oofRepViewRedirectingRender::finishClone(oofRepRender* newClone) const
{
	return (oofRepViewRedirectingRender*)newClone;  // safe downcast
}


/**
drawColumnHeaders() occurs as a callback during the wrapped draw() called from our draw()
therefore all the colSizer init in draw() is guaranteed to have occurred.

As our draw() has already added a bunch of interaction areas for the initial row
we need to modify them to point to us, and add a set for the REAL first row after the headers

mColHeaderLink acts as a flag - if we are doing column headers it is created in startTableBody
*/
void 
oofRepViewRedirectingRender::drawColumnHeaders()
{
	const bool editingHeaders = (editRedirectionActive() && mColHeaderLink); 
	if (editingHeaders)  {
		const short yPosBeforeHeaders = oofRep::currentReport()->currentVerticalPos();
		editingEnv()->changeAreasToNewLink(yPosBeforeHeaders, mEditLink, mColHeaderLink);
	}
	
	((oofRepViewBand_Render*)mWrappedRender)->drawColumnHeaders();  // safe downcast because of ctor

	if (editingHeaders)  {
		AddRowInteractionAreas();
	}
}


/**
By overriding startTableBody this wrapping render gets to be processed once for
the main table and every time a sub-table is created. There will be potentially
many edit links for some oofRepViewBand's because each sub-table is probably a 
different data context.
*/
void 
oofRepViewRedirectingRender::startTableBody(bool headersHaveChanged)
{
	if (editRedirectionActive()) {	
		oofRepViewBand* theViewBand = ViewBand();
		oofRep* curRep = oofRep::currentReport();
	
		const bool needsToCloneToSaveSubSelections = (curRep->nestedViewLevel()>oofRep::kBodyViewLevel);
		if (mEditLink)  {  // we are sub-table having called startTableBody before
			// don't delete old link, it's owned by someone else so we just cease to point at it
			// or we may be reflowing a report!
		}

		mEditLink = editor()->makeViewEditLink(theViewBand, needsToCloneToSaveSubSelections);	
		editingEnv()->pageAdoptsEditLink(mEditLink);  // this time is adopting it as link starts this page!
		if (!mColHeaderLink && curRep->shouldDrawViewColumnHeaders()) {
			mColHeaderLink = editor()->makeColHeaderEditLink(theViewBand);
			mColHeaderLink->incRefs();  // we want to hang onto it!
		}
		if (mColHeaderLink)
			editingEnv()->pageAdoptsEditLink(mColHeaderLink);  // adopt the sole copy! - multiple refs
			// even if we already created it earlier, each new set of pages must share a ref
	}
	
	((oofRepViewBand_Render*) mWrappedRender)->startTableBody(headersHaveChanged);  // safe downcast
}


void 
oofRepViewRedirectingRender::endTableBody()
{
}



// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepEditingEnv& inEnv)
{
	makeRender(*inEnv.parentGUIenv());
	oofRepViewRedirectingRender* theRender = inEnv.editor()->makeViewRenderWrapper((oofRepViewBand_Render*)mRender);  // safe downcast
	mRender = theRender;
	mTableExtraRender = theRender;
// do NOT call	mRender->finishConstruction();
// as the wrapped render will have done its own finishConstruction in makeRender parent method
}


void	
oofRepLayoutBand::makeRender(const oofRepEditingEnv& inEnv)
{
	makeRender(*inEnv.parentGUIenv());
	mRender = inEnv.editor()->makeLayoutRenderWrapper((oofRepLayoutBand_Render*)mRender );  // safe downcast
}
