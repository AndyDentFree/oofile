// COPYRIGHT 1996 A.D. Software, All rights reserved

// report-writer layer of OOFILE database

	#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef _STRING
	#include <string.h>
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"	// abstract renderers
#endif
#ifndef H_OOFREPEDITOR
	#include "oofrepeditor.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

oofRep*	oofRep::sCurrentReport;
oofRepLayoutBand*	oofRepLayoutBand::sCurrentLayout;




// -------------------------------------------------------
//    O O F _ r e p M a k e R e n d e r V i s i t o r
// -------------------------------------------------------
/**
	Visit every report item that can be rendered to invoke makeRender.
	\ingroup oofRW
*/
class OOF_repMakeRenderVisitor : public oofRepVisitor {
public:
	OOF_repMakeRenderVisitor(oofRepEnvironment& inEnv) : 
		mEnv(inEnv)
	{};
	
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
	virtual void VisitAdorner(oofAdorner*);

private:
	oofRepEnvironment& mEnv;
};


void
OOF_repMakeRenderVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	VisitAdornerList(inDrawable->adorners());
	mEnv.makeRenderDispatch(inDrawable);
}


void
OOF_repMakeRenderVisitor::VisitAdorner(oofAdorner* inAdorner)
{
	mEnv.makeRenderDispatch(inAdorner);
}


// -------------------------------------------------------
//  O O F _ r e p F i n i s h C o n s t r u c t i o n V i s i t o r
// -------------------------------------------------------
/**
	Finish two-stage report init by visiting all items that have finishConstruction methods.
	\ingroup oofRW
*/
class OOF_repFinishConstructionVisitor : public oofRepVisitor {
protected:
	virtual void VisitBandList(oofRepBandList&);
	virtual void VisitDrawable(oofRepDrawable*); 
};


void
OOF_repFinishConstructionVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	inDrawable->finishConstruction();
}


void
OOF_repFinishConstructionVisitor::VisitBandList(oofRepBandList& inBands)
{
	oofRepVisitor::VisitBandList(inBands);  // must be AFTER bands finished, eg: calc fixed height
	inBands.finishConstruction();
}



// -------------------------------------------------------
//   O O F _ r e p B u f f e r S e t t i n g V i s i t o r
// -------------------------------------------------------
/**
	Set buffer mode of main table as well as others associated with fields in the view.
*/
void
OOF_repBufferSettingVisitor::VisitView(dbView* inView)
{
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
		dbTable* fieldTable = inView->field(col).fieldTable();
		if (fieldTable && fieldTable!=viewTable)
			fieldTable->setSaveOption(newOption);
	} // loop all fields
}


// -------------------------------------------------------
// o o f R e p S c h e m a V i s i t o r
// -------------------------------------------------------
void
oofRepSchemaVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	for (unsigned short i=0;; i++) {
		dbView* theView = inDrawable->view(i);  // drawables can have multiple views
		if (theView)
			VisitView(theView);
		else
			break;
	}
}



// -------------------------------------------------------
//                     o o f R e p D a t a
// -------------------------------------------------------
/**
	Maintain the context of report data to reset dbView's etc. on redraws.
	\see oofRepSection.drawSection
	\todo confirm why it is disabled in oofRep.draw
	\ingroup oofRW
*/
class oofRepData  : public oofSingleListener {
public:
	oofRepData(dbView&);
	oofRepData(dbView*);
	oofRepData(dbTable*);
	virtual ~oofRepData();
	void InitRepData(dbTable*);

	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	
	void useRepContext();
	void restoreDataSelection();
	
private:	
	OOF_Selection*	mRepSelection;	// owned
	OOF_Selection* mSavedSelection;	// owned
	dbTable*	mTable;	// reference, may be nil
	unsigned long mSavedRecNo;	// until bug in setSelection fixed
};


oofRepData::oofRepData(dbView& inView)
{
	InitRepData(inView.table());
}


oofRepData::oofRepData(dbView* inView)
{
	InitRepData(inView->table());
}


oofRepData::oofRepData(dbTable* inTable)
{
	InitRepData(inTable);
}


oofRepData::~oofRepData()
{
	delete mSavedSelection;
	delete mRepSelection;
}


bool 
oofRepData::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
{
	if (msg==OOFmsg_BroadcasterClosing) { // table has been deleted
		mListensTo = 0;
		mTable = 0;	// won't try to change its selection!
	}
	return true;
}


void
oofRepData::InitRepData(dbTable* inTable)
{
	mTable = inTable;
	if (inTable) {	// allow for views without tables!!!
		subscribeTo(inTable);
	// REALLY need a way to declare dbSelection with no params!!!!
		mRepSelection = inTable->currentSelection().internalSelection()->clone();
	}
	else
		mRepSelection = 0;
	mSavedSelection = 0;
}



void 
oofRepData::useRepContext()
{
	if (mTable) {
		delete mSavedSelection;
		mSavedSelection = mTable->currentSelection().internalSelection()->clone();
		const unsigned long numRecs = mSavedSelection->count();
		if (numRecs>0) {
			mSavedRecNo = mTable->recordNumber();
			if (mSavedRecNo>=numRecs)
				mSavedRecNo=0;
		}
		mTable->setSelection(mRepSelection);
	}
}


void 
oofRepData::restoreDataSelection()
{
	if (mSavedSelection) {
		mTable->setSelection(mSavedSelection);
		if (mSavedSelection->count())	// can't gotoRecord if empty selection (weird but true, some reports have no selection!)
			mTable->gotoRecord(mSavedRecNo);	// until bug in setSeleciton fixed
	}
}


// -------------------------------------------------------
//                     o o f R e p
// -------------------------------------------------------

oofRep::oofRep(dbTable* inTableForDefaultBody, unsigned long startingPageNumber) :
	mTopBand(new oofRepViewBand(inTableForDefaultBody->makeView(), false /* default not boxed*/)), 
	mCurrentBand(0),
	mCurrentBreak(0),
	mHeadersStartAt(0),
	mFootersStartAt(0),
	mFirstPage(true),
	mFirstPageHeaderHeight(0),
	mFirstPageFooterHeight(0),
	mPageFooterHeight(0),
	mPageHeaderHeight(0),
	mPageHeight(0),
	mVerticalPos(0),
	mDrawingEnv(0),
	mSettings(0),
	mPageNumber(0),
	mFinishedConstruction(false),
	mLastRenderType(0),
	mNestedViewLevel(0),
	mRepData(0),
	mStartingPageNumber(startingPageNumber)
{
	mReferences = new unsigned short(1);
}


oofRep::oofRep(dbView* adoptedViewForDefaultBody, unsigned long startingPageNumber) :
	mTopBand(new oofRepViewBand(adoptedViewForDefaultBody, false /* default not boxed*/)), 
	mCurrentBand(0),
	mCurrentBreak(0),
	mHeadersStartAt(0),
	mFootersStartAt(0),
	mFirstPage(true),
	mFirstPageHeaderHeight(0),
	mFirstPageFooterHeight(0),
	mPageFooterHeight(0),
	mPageHeaderHeight(0),
	mPageHeight(0),
	mVerticalPos(0),
	mDrawingEnv(0),
	mSettings(0),
	mPageNumber(0),
	mFinishedConstruction(false),
	mLastRenderType(0),
	mNestedViewLevel(0),
	mRepData(0),
	mStartingPageNumber(startingPageNumber)
{
	mReferences = new unsigned short(1);
}


oofRep::oofRep(const dbView& inViewForDefaultBody, unsigned long startingPageNumber) :
	mTopBand(new oofRepViewBand(inViewForDefaultBody, false /* default not boxed*/)), 
	mCurrentBand(0),
	mCurrentBreak(0),
	mHeadersStartAt(0),
	mFootersStartAt(0),
	mFirstPage(true),
	mFirstPageHeaderHeight(0),
	mFirstPageFooterHeight(0),
	mPageFooterHeight(0),
	mPageHeaderHeight(0),
	mPageHeight(0),
	mVerticalPos(0),
	mDrawingEnv(0),
	mSettings(0),
	mPageNumber(0),
	mFinishedConstruction(false),
	mLastRenderType(0),
	mNestedViewLevel(0),
	mRepData(0),
	mStartingPageNumber(startingPageNumber)
{
	mReferences = new unsigned short(1);
}


oofRep::oofRep(oofRepViewBand* topBand, unsigned long startingPageNumber) :
	mTopBand(topBand),
	mCurrentBand(0),
	mCurrentBreak(0),
	mHeadersStartAt(0),
	mFootersStartAt(0),
	mFirstPage(true),
	mFirstPageHeaderHeight(0),
	mFirstPageFooterHeight(0),
	mPageFooterHeight(0),
	mPageHeaderHeight(0),
	mPageHeight(0),
	mVerticalPos(0),
	mDrawingEnv(0),
	mSettings(0),
	mPageNumber(0),
	mFinishedConstruction(false),
	mLastRenderType(0),
	mNestedViewLevel(0),
	mRepData(0),
	mStartingPageNumber(startingPageNumber)
{
	mReferences = new unsigned short(1);
}


oofRep::oofRep(const oofRep& rhs) :
	mFirstPageHeaders(rhs.mFirstPageHeaders),
	mFirstPageFooters(rhs.mFirstPageFooters),
	mPageHeaders(rhs.mPageHeaders),
	mPageFooters(rhs.mPageFooters),
	mTopBand(rhs.mTopBand),  // refcount makes this safe to share
	mCurrentBand(0),
	mCurrentBreak(0),
	mFirstPageHeaderHeight(rhs.mFirstPageHeaderHeight),
	mFirstPageFooterHeight(rhs.mFirstPageFooterHeight),
	mPageHeaderHeight(rhs.mPageHeaderHeight),
	mPageFooterHeight(rhs.mPageFooterHeight),
	mPageHeight(rhs.mPageHeight),
	mHeadersStartAt(rhs.mHeadersStartAt),
	mFootersStartAt(rhs.mFootersStartAt),
	mVerticalPos(rhs.mVerticalPos),
	mBodyStart(rhs.mBodyStart),
	mBodyWidth(rhs.mBodyWidth),
	mPageWidth(rhs.mPageWidth),
	mFirstPageBodyStart(rhs.mFirstPageBodyStart),
	mFirstPageFootersStartAt(rhs.mFirstPageFootersStartAt),
	mFirstPage(rhs.mFirstPage),
	mDrawingEnv(rhs.mDrawingEnv),
	mSettings(rhs.mSettings),
	mPageNumber(rhs.mPageNumber),
	mFinishedConstruction(rhs.mFinishedConstruction),
	mLastRenderType(rhs.mLastRenderType),
	mNestedViewLevel(rhs.mNestedViewLevel),
	mRepData(rhs.mRepData),  // refcount makes this safe to share
	mStartingPageNumber(rhs.mStartingPageNumber),
	mReferences(rhs.mReferences)
{
	assert(mReferences);
	(*mReferences)++;
}


oofRep::~oofRep()
{
	if (*mReferences==1) {
		if (mFinishedConstruction && reportAllowsEditing()) {
			OOF_repBufferSettingVisitor saveModeSetter(false);
			saveModeSetter.visit(this);
		}
		delete mReferences;
		delete mTopBand;
		delete mRepData;
		delete mSettings;
	}
	else
		(*mReferences)--;
}


bool 
oofRep::isMultiReport() const
{
	return false;
}



void
oofRep::becomeCurrent()
{
	sCurrentReport = this;
}


oofRepSettings*
oofRep::getLocalSettings()
{
	if (!mSettings)	
		mSettings = new oofRepSettings();
	return mSettings;
}


oofRepSettings*
oofRep::defaultSettings()
{
	return oofRepDefaultSettings::settings();
}


const oofRepSettings*
oofRep::settings() const
{
	if (mSettings)	// if the report has its own settings
		return mSettings;
	else			// use defaults
		return defaultSettings();
}


void 
oofRep::draw(oofRepEnvironment& inEnv)
{
	if (!inEnv.canDraw())
		return;	//****** early exit - unable to draw (probably cancel a Save As dialog)
		
	// pickup defaults if we have none - BEFORE makeRender()
	if (mPageHeaders.count()==0)
		mPageHeaders = settings()->defaultPageHeaders();
	if (mPageFooters.count()==0)
		mPageFooters = settings()->defaultPageFooters();

	becomeCurrent();
	mDrawingEnv = &inEnv;
	resetViewLevel();	// if re-running report which only draws headings at top levels, would see gradual vanishing

	mPageHeight = mDrawingEnv->calcPageHeight();	// *after* we have set the mDrawingEnv
	if (mPageHeight==0)
		return;  //***** early exit - error condition getting page height
		
	mPageWidth = mBodyWidth = mDrawingEnv->calcPageWidth();		// *after* we have set the mDrawingEnv
	mBodyWidth -= settings()->leftMargin();
	mBodyWidth -= settings()->rightMargin();
	mFirstPageHeaderHeight = mFirstPageHeaders.fixedHeight();
	mFirstPageFooterHeight = mFirstPageFooters.fixedHeight();
	mPageHeaderHeight = mPageHeaders.fixedHeight();
	mPageFooterHeight = mPageFooters.fixedHeight();
	mHeadersStartAt = settings()->topMargin();
	if( mFirstPageFooters.count() > 0 )
		mFirstPageFootersStartAt = mPageHeight - (mFirstPageFooterHeight+settings()->bottomMargin());
	mFootersStartAt = mPageHeight - (mPageFooterHeight+settings()->bottomMargin());
	mFirstPage = true;
	if( mFirstPageHeaders.count() > 0 )
		mFirstPageBodyStart = mHeadersStartAt + mFirstPageHeaderHeight;
	mBodyStart = mHeadersStartAt + mPageHeaderHeight;
		

	if (!mFinishedConstruction) {
	// complete the report objects now that we have them all
	// this happens ONCE regardless of how many times we draw and
	// regardless of environment
		mFinishedConstruction = true;		
		OOF_repFinishConstructionVisitor theVisitor;
		theVisitor.visit(this);
		
		if (reportAllowsEditing()) {
			oofRepVisitor* setupVisitor = settings()->reportEditor()->makeRepEditingSetupVisitor();
			assert(setupVisitor);
			setupVisitor->visit(this);
			delete setupVisitor;
		}
	}


// make renderers each time we have a new environment
	oofRepRenderT	newRenderType = inEnv.renderType();
	if (mLastRenderType != newRenderType) {
		mLastRenderType = newRenderType;
		inEnv.makeRender();

		OOF_repMakeRenderVisitor theVisitor(inEnv);
		theVisitor.visit(this);
	}

// bit of a conceptual hack for now - restore the data context unconditionally on re-runs
/*
	if (mRepData)
		mRepData->useRepContext();
	else
		mRepData = new oofRepData(mTopBand->view());	// save context for later
*/
		
	mPageNumber = mStartingPageNumber;	
	
	inEnv.startEnvironment();	// start whatever initial drawing state or precursor written out

	bool drawPageHeadersAndFooters = !mDrawingEnv->preprocessesPageHeadersAndFooters();
	//mDrawingEnv->moveToVerticalPos(mHeadersStartAt);
	mVerticalPos=mHeadersStartAt;
	if (drawPageHeadersAndFooters) {
		if( firstPageHeaders().count() > 0 )
			drawFirstPageHeaders();
		else
			drawPageHeaders();
	}
	
	//mDrawingEnv->moveToVerticalPos(mBodyStart);
	if( mFirstPageHeaders.count() > 0 )
		mVerticalPos=mFirstPageBodyStart;
	else
	mVerticalPos=mBodyStart;
	DrawViewBand(mTopBand);
	
// NOT YET IMPLEMENTED - allow for having just finished a page exactly
// with the last band drawn
// so we would currently draw two lots of footers
	//mDrawingEnv->moveToVerticalPos(mFootersStartAt);
	
	if (drawPageHeadersAndFooters) {
		if( firstPageFooters().count() > 0 && mFirstPage ) {
			mVerticalPos=mFirstPageFootersStartAt;
			drawFirstPageFooters();
			mFirstPage = false;
		}
		else {
			mVerticalPos=mFootersStartAt;
			drawPageFooters();
		}
	}
	
	inEnv.endEnvironment();
	
	if (mRepData)
		mRepData->restoreDataSelection();	
}


void
oofRep::drawPageHeaders()
{
	if( settings()->borderType() > 0 )	{
		// draw from outside the body section
		//Rect borderInfo = { settings()->topMargin(),
		//					settings()->leftMargin(),
		//					pageHeight()-settings()->bottomMargin(),
		//					settings()->leftMargin()+bodyWidth()};
	
		mDrawingEnv->	drawPageBorder( settings()->topMargin(),
						settings()->leftMargin(),
						pageHeight()-settings()->bottomMargin(),
						settings()->leftMargin()+bodyWidth() );
	}
	
	const bool hasHeaders = (mPageHeaders.count()>0);
	if (hasHeaders) {
		DrawBandList(mPageHeaders,false);
	}
}


void	
oofRep::drawPageFooters()
{
	DrawBandList(mPageFooters,false);
}


void
oofRep::drawFirstPageHeaders()
{
	if( settings()->borderType() > 0 )	{
		// draw from outside the body section
		//Rect borderInfo = { settings()->topMargin(),
		//					settings()->leftMargin(),
		//					pageHeight()-settings()->bottomMargin(),
		//					settings()->leftMargin()+bodyWidth()};
	
		mDrawingEnv->	drawPageBorder( settings()->topMargin(),
						settings()->leftMargin(),
						pageHeight()-settings()->bottomMargin(),
						settings()->leftMargin()+bodyWidth() );
	}
		
	const bool hasHeaders = (mFirstPageHeaders.count()>0);
	if (hasHeaders) {
		DrawBandList(mFirstPageHeaders,false);
	}
}


void	
oofRep::drawFirstPageFooters()
{
	DrawBandList(mFirstPageFooters,false);
}


void
oofRep::drawPageBreak()
{
	bool drawPageHeadersAndFooters = !mDrawingEnv->preprocessesPageHeadersAndFooters();
	//assert(mVerticalPos <= mFootersStartAt);  // Debugging
	//mDrawingEnv->moveToVerticalPos(mFootersStartAt);
	if (drawPageHeadersAndFooters) {
		if( firstPageFooters().count() > 0 && mFirstPage ) {
			mVerticalPos=mFirstPageFootersStartAt;
			drawFirstPageFooters();
			mFirstPage = false;
		}
		else {
			mVerticalPos=mFootersStartAt;
			drawPageFooters();
		}
	}

	mPageNumber++;

	mDrawingEnv->drawPageBreak();

	//mDrawingEnv->moveToVerticalPos(mHeadersStartAt);
	mVerticalPos=mHeadersStartAt;
	if (drawPageHeadersAndFooters)
		drawPageHeaders();

	//mDrawingEnv->moveToVerticalPos(mBodyStart);
	mVerticalPos=mBodyStart;
}


oofRepViewBand*  
oofRep::body(oofRepViewBand* adoptedBody)
{
	mLastRenderType = 0;	// if change body want to force regeneration of renderers
	delete mTopBand;
	mTopBand = adoptedBody;
	return adoptedBody;
}


void 
oofRep::changeTextStyle(oofRepTextStyle* S)
{
	mDrawingEnv->changeTextStyle(S);
}


bool
oofRep::shouldDrawViewColumnHeaders() const
{
	unsigned short drawDownTo = settings()->drawHeadingsDownToLevel();
	bool ret = (drawDownTo >= mNestedViewLevel);
	return ret;
}


void 
oofRep::DrawBandList(oofRepBandList& inList, bool inBody)
{
	if (inList.count()==0)
		return;  // TRIVIAL EXIT - empty bandlist
		
	if (inBody) {  // check if band fits, breaks etc.
		if (inList.keepTogether()) {  // definitely not a view band in the list!
			const long spaceLeft = bodyEnd()-mVerticalPos;
			const long fixedHeight = inList.fixedHeight();
			long reduceBy = 0;
			if (fixedHeight>spaceLeft) {  // too large, maybe it can squeeze to fit
			// if our min height allows us to fit then we calculate an amount
			// by which we have to reduce from our normal fixedHeight, and ask 
			// each band in turn to whittle away at that reduceBy space.
			
			// Note: this is not a very good algorithm if you had several bands mixed
			// which could all reduce but our common situation at present is that only
			// one type of band (graph) will reduce and usually only one is mixed with
			// other bands in a list
				const long maxAvailHeight = bodyHeight();
				const long minHeight = inList.minHeight();
				if (minHeight<=spaceLeft || minHeight>maxAvailHeight)
					reduceBy = fixedHeight - spaceLeft; // it thinks it can fit or we make it 
					// because it's bigger than the page
				else {
					if (fixedHeight>maxAvailHeight)  // list is taller than page without shrinking
						reduceBy = fixedHeight - maxAvailHeight;
					drawPageBreak();  // can't fit and fits within a page, so get a new one
				}
			}  // might not fit

			for (inList.start(); inList.more(); inList.next()) {
				mCurrentBand = inList();
				if (reduceBy>0)
					mCurrentBand->drawSquished(reduceBy);  // always reduces reduceBy
				else
					mCurrentBand->draw();  // no need to see if things fit
			} // loop kept together bands
		}
		else {  // may contain view bands, may split across pages
			for (inList.start(); inList.more(); inList.next()) {
				oofRepBand* theBand = inList();
				if (theBand->isViewBand()) {
					DrawViewBand((oofRepViewBand*)theBand);
				}
				else {
					mCurrentBand = theBand;
					const long bandFixedHeight = theBand->fixedHeight();
					long reduceByForWholePage=bandFixedHeight-bodyHeight();
					if (reduceByForWholePage<0)
						reduceByForWholePage=0;
						
					if (mVerticalPos > mBodyStart) { // only part page available
						const long spaceLeft = bodyEnd()-mVerticalPos;
						if (bandFixedHeight > spaceLeft) 
							drawPageBreak();  // break because more space next page
					}
	// either at top of new page, or top of original, and need to squish anyway, or we fitted				
					if (reduceByForWholePage>0)
						theBand->drawSquished(reduceByForWholePage);  // squish - it's taller than a page
					else
						theBand->draw();  // draw original height
				}
			}  // loop bands
		}  // keep together or not
	}
	else {  // not in body so don't care about keeptogether etc.
		for (inList.start(); inList.more(); inList.next()) {
			oofRepBand* theBand = inList();
			if (theBand->isViewBand()) {
				DrawViewBand((oofRepViewBand*)theBand);
			}
			else {  // can't break if not in body
				mCurrentBand = theBand;
				theBand->draw();
			}
		}  // loop bands
	} // inBody or not
}


void 
oofRep::ProcessBreakList(oofRepBreakList& inList, bool beforeRecord)
{
	bool newPage = false;
	oofRepBreakList triggeredBreaks(false);
	for (inList.start(); inList.more(); inList.next()) {
		oofRepBreak* theBreak = inList();
		if (theBreak->breaks()) {
	        if (theBreak->newPage())
	                newPage = true;
			triggeredBreaks << theBreak;
		}
	}	// loop all possible breaks
	
	if (beforeRecord) {
		if (newPage)
			drawPageBreak();
		DrawBreakList(triggeredBreaks);
	}
	else {
		DrawBreakList(triggeredBreaks);
		if (newPage)
			drawPageBreak();
	}
}


void 
oofRep::DrawBreakList(oofRepBreakList& inList)
{
	for (inList.start(); inList.more(); inList.next()) {
		oofRepBreak* theBreak = inList();
		oofRepBand* theBand = theBreak->band();
		if (theBand) {
			if (theBand->isViewBand()) {
				DrawViewBand((oofRepViewBand*)theBand);
			}
			else {
				// Do we fit on the page..
				if ((theBand->fixedHeight()+mVerticalPos)>bodyEnd()) {
					drawPageBreak();
				}	
				mCurrentBand = theBand;
				theBand->draw();
			}
		}	// break without a band
	}
}


/**
	Draw a single row of the oofRepViewBand.
	\note it may seem a little weird that we draw a view band rather than simply calling it to draw itself
	but as you will see below, a view band *does* know how to draw itself - it just draws a single
	row of fields.

	The control logic that knows how to process breaks resides up at the report level, as the report
	contains other bands (eg: page headers) and works out how they interact. The view band knows nothing
	of these complex interactions.
*/
void 
oofRep::DrawViewBand(oofRepViewBand* inBand)
{
	if (!inBand)
		return;	// it *is* legal to have empty bands
	
	incrementViewLevel();	// so we know what level we're down to, for heading control
	// remembering breaks and other bands on the following view may themselves be view bands
		
	mCurrentBand = inBand;
	
	inBand->startTableWrapper();
	DrawBandList(inBand->headers());
	dbTable* viewTable = inBand->view()->table();	
	inBand->startTableBody();
	
	if (viewTable) {
		for (viewTable->start(); viewTable->more(); viewTable->next()) {
			ProcessBreakList(inBand->headerBreaks(), true);
			inBand->draw();
			ProcessBreakList(inBand->footerBreaks(), false);
		}
	}
	else {	// just one iteration if have no table associated with view
		ProcessBreakList(inBand->headerBreaks(), true);
		inBand->draw();
		ProcessBreakList(inBand->footerBreaks(), false);
	}	
	inBand->endTableBody();
	DrawBandList(inBand->footers());
	inBand->endTableWrapper();

	decrementViewLevel();
}

void
oofRep::describe(ostream& os)
{
	mSettings->describe(os);

}

// -------------------------------------------------------
//            o o f R e p M u l t i
// -------------------------------------------------------
oofRepMulti::oofRepMulti()
{}

oofRepMulti::~oofRepMulti()
{
	const unsigned long numSections = mSections.count();
	for (unsigned long i=0; i<numSections; i++) {
		oofRepSection* theSection = section(i);
		delete theSection;
	}
	mSections.deleteAllCells()  ;
}


bool 
oofRepMulti::isMultiReport() const
{
	return true;
}


void 
oofRepMulti::appendReport(oofRep* adoptedReport)
{
	appendReport(*adoptedReport);
 	delete adoptedReport;
}


void 
oofRepMulti::appendReport(const oofRep& inRep)
{
	if (inRep.isMultiReport()) {
		oofRepMulti* multiRep = (oofRepMulti*)(&inRep);  // safe downcast
		const unsigned short numAdopting = multiRep->count();
		for (unsigned short i=0; i<numAdopting; i++) {
			oofRepSection* theSection = multiRep->orphanSection(i);
			mSections.append((unsigned long)theSection);
		}
	}
	else {
		oofRepSection* theSection = new oofRepSection(inRep);
		mSections.append((unsigned long)theSection);
	}
}


oofRepMulti& 
oofRepMulti::operator<<(oofRep* adoptedReport)
{
	appendReport(*adoptedReport);
	delete adoptedReport;
	return *this;
}


oofRepMulti& 
oofRepMulti::operator<<(const oofRep& inRep)
{
	appendReport(inRep);
	return *this;
}


oofRepSection* 
oofRepMulti::orphanSection(unsigned long n)
{
	oofRepSection*  ret = (oofRepSection*) mSections.value(n);
	mSections[n] = 0;  // clear our pointer
	return ret;
}


void	
oofRepMulti::draw(oofRepEnvironment& inEnv)
{
	if (!inEnv.canDraw())
		return;	//****** early exit - unable to draw (probably cancel a Save As dialog)
		
	const unsigned long numSections = mSections.count();
	assert(numSections);  // silly to be drawing if no reports
	if (numSections==0)
		return;	// clean runtime exit
		 
	// pickup defaults if we have none - BEFORE makeRender()
	if (mPageHeaders.count()==0)
		mPageHeaders = settings()->defaultPageHeaders();
	if (mPageFooters.count()==0)
		mPageFooters = settings()->defaultPageFooters();

	becomeCurrent();
	mDrawingEnv = &inEnv;
	resetViewLevel();	// if re-running report which only draws headings at top levels, would see gradual vanishing

	mPageHeight = mDrawingEnv->calcPageHeight();	// *after* we have set the mDrawingEnv
	mPageWidth = mBodyWidth = mDrawingEnv->calcPageWidth();		// *after* we have set the mDrawingEnv
	mBodyWidth -= settings()->leftMargin();
	mBodyWidth -= settings()->rightMargin();
	mFirstPageHeaderHeight = mFirstPageHeaders.fixedHeight();
	mFirstPageFooterHeight = mFirstPageFooters.fixedHeight();
	mPageHeaderHeight = mPageHeaders.fixedHeight();
	mPageFooterHeight = mPageFooters.fixedHeight();
	mHeadersStartAt = settings()->topMargin();
	if( mFirstPageFooters.count() > 0 )
		mFirstPageFootersStartAt = mPageHeight - (mFirstPageFooterHeight+settings()->bottomMargin());
	mFootersStartAt = mPageHeight - (mPageFooterHeight+settings()->bottomMargin());
	mFirstPage = true;
	if( mFirstPageHeaders.count() > 0 )
		mFirstPageBodyStart = mHeadersStartAt + mFirstPageHeaderHeight;
	mBodyStart = mHeadersStartAt + mPageHeaderHeight;
		
	for (unsigned short i=0; i<numSections; i++) {
		oofRepSection* theSection = section(i);
		assert(theSection);
		theSection->setDimensions(inEnv, this);
	}
	
	if (!mFinishedConstruction) { 
		mFinishedConstruction = true;
		
		OOF_repFinishConstructionVisitor theVisitor;
		theVisitor.visit(this);
	}
	oofRepRenderT	newRenderType = inEnv.renderType();
	if (mLastRenderType != newRenderType) {
		mLastRenderType = newRenderType;
		inEnv.makeRender();

		OOF_repMakeRenderVisitor theVisitor(inEnv);
		theVisitor.visit(this);
	}

	becomeCurrent();
	inEnv.startEnvironment();	// AFTER getting page headers and footers & makeRender
		
	for (unsigned short j=0; j<numSections; j++) {
		oofRepSection* theSection = section(j);
		assert(theSection);
		theSection->drawSection(inEnv, this, j);
	}
	
	becomeCurrent();
	inEnv.endEnvironment();
}


// -------------------------------------------------------
//            o o f R e p S e c t i o n
// -------------------------------------------------------
oofRepSection::oofRepSection(oofRepViewBand* adoptedBody, unsigned long startingPageNumber, bool resetPageNumber, bool pageBreak) :
	oofRep(adoptedBody, startingPageNumber),
	mResetPageNumber(resetPageNumber),
	mPageBreak(pageBreak)
{
}


oofRepSection::oofRepSection(const oofRep& inRep, bool resetPageNumber, bool pageBreak) :	// special copy ctor
	oofRep(inRep),
	mResetPageNumber(resetPageNumber),
	mPageBreak(pageBreak)

{
	assert(mTopBand);  // current model demands a view band
}








void	
oofRepSection::setDimensions(oofRepEnvironment& inEnv, oofRepMulti* parentReport)
{
	becomeCurrent();
	resetViewLevel();
	// pickup parent's, which will have been defaulted if not set
	if (mPageHeaders.count()==0)
		mPageHeaders = parentReport->mPageHeaders;
	if (mPageFooters.count()==0)
		mPageFooters = parentReport->mPageFooters;

	if (mFirstPageHeaders.count()==0)
		mFirstPageHeaders = parentReport->mFirstPageHeaders;
	if (mFirstPageFooters.count()==0)
		mFirstPageFooters = parentReport->mFirstPageFooters;

	mDrawingEnv = &inEnv;
	resetViewLevel();	// if re-running report which only draws headings at top levels, would see gradual vanishing
	mPageHeight = parentReport->mPageHeight;	
	mPageWidth = parentReport->mPageWidth;		
	mBodyWidth = parentReport->mBodyWidth;
	mFirstPageHeaderHeight = mFirstPageHeaders.fixedHeight();
	mFirstPageFooterHeight = mFirstPageFooters.fixedHeight();
	mPageHeaderHeight = mPageHeaders.fixedHeight();
	mPageFooterHeight = mPageFooters.fixedHeight();
	mHeadersStartAt = settings()->topMargin();
	if( mFirstPageFooters.count() > 0 )
		mFirstPageFootersStartAt = mPageHeight - (mFirstPageFooterHeight+settings()->bottomMargin());
	mFootersStartAt = mPageHeight - (mPageFooterHeight+settings()->bottomMargin());
	if( mFirstPageHeaders.count() > 0 )
		mFirstPageBodyStart = mHeadersStartAt + mFirstPageHeaderHeight;
	mBodyStart = mHeadersStartAt + mPageHeaderHeight;
}


void	
oofRepSection::drawSection(oofRepEnvironment& inEnv, oofRepMulti* parentReport, unsigned short inSectionNumber)
{
	mFirstPage = (inSectionNumber==0);
		
	becomeCurrent();
	resetViewLevel();
// bit of a conceptual hack for now - restore the data context unconditionally on re-runs
	if (mRepData)
		mRepData->useRepContext();
	else
		mRepData = new oofRepData(mTopBand->view());	// save context for later
		
// never set for sections	assert(mFinishedConstruction);
		
	if (mResetPageNumber)
		mPageNumber = mStartingPageNumber;	
	else
		mPageNumber = parentReport->pageNumber();
	
	mDrawingEnv->startSection();
// if not the first section being drawn, we need to break
// after the last one, 
	if (!mFirstPage && mPageBreak) {
	// assume prev section drew its final page footer
		mDrawingEnv->drawSectionBreak();
	// our normal start of section logic below will draw header if appropriate
	}

	bool drawPageHeadersAndFooters = !mDrawingEnv->preprocessesPageHeadersAndFooters();
	mVerticalPos=mHeadersStartAt;
	if (drawPageHeadersAndFooters) { // ie we have not drawn them in start enviroment
		if( firstPageHeaders().count() > 0 )
			drawFirstPageHeaders();
		else
			drawPageHeaders();
	}
	
	if( mFirstPageHeaders.count() > 0 )
		mVerticalPos=mFirstPageBodyStart;
	else
	mVerticalPos=mBodyStart;
	DrawViewBand(mTopBand);
	
// NOT YET IMPLEMENTED - allow for having just finished a page exactly
// with the last band drawn
// so we would currently draw two lots of footers
	
	if (drawPageHeadersAndFooters) {
		if( firstPageFooters().count() > 0 && mFirstPage ) {
			mVerticalPos=mFirstPageFootersStartAt;
			drawFirstPageFooters();
			mFirstPage = false;
		}
		else {
			mVerticalPos=mFootersStartAt;
			drawPageFooters();
		}
	}
	
	if (mRepData)
		mRepData->restoreDataSelection();	

// reset the current page number in the parent so it knows where we are up to
	parentReport->pageNumber(mPageNumber);
}


// -------------------------------------------------------
//            o o f R e p D r a w a b l e
// -------------------------------------------------------
oofRepDrawable::oofRepDrawable(oofRepTextStyle* adoptedStyle) :
	oofRepTextStylable(adoptedStyle),
	mRender(0)
{
}


oofRepDrawable::oofRepDrawable(const oofRepDrawable& rhs)  :
	oofRepTextStylable(rhs)
{
	if (rhs.mRender)
		mRender = rhs.mRender->clone(this);
	else
		mRender = 0;
}

oofRepDrawable::~oofRepDrawable()
{
	delete mRender;
}


/**
 null base method occasionally overriden
*/
void 
oofRepDrawable::finishConstruction()
{
}


dbView*
oofRepDrawable::view(unsigned short /*whichView*/) const
{
	return 0;
}


/**
 central default call like draw() but still able to be overriden in extreme
 cases. Provides a second virtual function for renderers used by backends such
 as XML which have distinct drawing time and declarative time operations
*/
void 
oofRepDrawable::describeLayout()
{
	assert(mRender);
	mRender->describeLayout();
}



unsigned long
oofRepDrawable::fixedHeight()
{
	assert( !"oofRepDrawable::fixedHeight should be overridden in all derived classes" );
	return 0;		// placed in base class for adorner call
}


unsigned long
oofRepDrawable::width()
{
	unsigned long ret = mRender->width();	
	return ret;
}



/**
 central default call for drawing, but still able to be overriden in extreme
 cases. Now that all is factored out to the render objects (Strategy pattern)
 we shouldn't need to do anything.regarding actual output
*/
void 
oofRepDrawable::draw()
{
	if (!mRender)
		return;
	
	const unsigned long adornCount = adorners().count();
	unsigned long i;
	
	if (adornCount) {
		// call adorners for before the band
		adorners().start();
		for( i=0; i<adornCount; i++ ) {
			oofAdorner*	currentAdorner = adorners().current();
			if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
				currentAdorner->draw( this, fixedHeight(), oofRep::currentReport()->bodyWidth() );
			}
			adorners().next();
		}
	}
	mRender->draw();
	
	if (adornCount) {
		// call adorners for after the band
		adorners().start();
		for( i=0; i<adornCount; i++ ) {
			oofAdorner*	currentAdorner = adorners().current();
			if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
				currentAdorner->draw( this, fixedHeight(), oofRep::currentReport()->bodyWidth() );
			}
			adorners().next();
		}
	}
}


// default render factories so if user chooses not to implement
// a render for their report object, we still behave OK, just ignore them
void	
oofRepDrawable::makeRender(const oofRepMacEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepWinEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepTextEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepRTFEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepHTMLEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepCSVEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepTSVEnv&)
{
	delete mRender;
	mRender = 0;
}


void	
oofRepDrawable::makeRender(const oofRepXMLEnv&)
{
	delete mRender;
	mRender = 0;
}

#ifndef OOF_REP_NO_GUI

/**
Make an editor for this drawable - by default return a null
link. Note that this is separated from the making of render wrappers
as editing may have its own complexities - a given drawable may 
therefore provide a subclassed edit link but use standard wrapping.
*/
oofRepEditLink*	
oofRepDrawable::makeEditLink(const oofRepEditingEnv& inEnv)
{
	return NULL;
}
#endif

// -------------------------------------------------------
//           o o f R e p R e n d e r 
// -------------------------------------------------------
long
oofRepRender::leftDrawMargin() const
{
	oofRep* curRep = oofRep::currentReport();
	const oofRepSettings* currSettings = curRep->settings();
	const unsigned short	leftMargin = currSettings->leftMargin();
	const unsigned short	bandIndenting = currSettings->bandIndenting();

	const long ret = leftMargin+bandIndenting;
	return ret;
}


unsigned long
oofRepRender::width() const
{
	return 0;  // NOT YET IMPLEMENTED - smarter default
}


// -------------------------------------------------------
//     o o f R e p R e n d e r D e c o r a t o r
// -------------------------------------------------------
oofRepRenderDecorator::oofRepRenderDecorator(oofRepRender* adoptedRender) :
	mWrappedRender(adoptedRender)
{
}


oofRepRenderDecorator::~oofRepRenderDecorator()
{
	delete mWrappedRender;
}


void 
oofRepRenderDecorator::finishConstruction()
{
	mWrappedRender->finishConstruction();
}


void 
oofRepRenderDecorator::draw()
{
	// just forward to wrapped render, but subclasses likely to override
	mWrappedRender->draw();
}


void 
oofRepRenderDecorator::describeLayout()
{
	mWrappedRender->describeLayout();
}


long 
oofRepRenderDecorator::leftDrawMargin() const
{
	return mWrappedRender->leftDrawMargin();
}


unsigned long 
oofRepRenderDecorator::width() const
{
	return mWrappedRender->width();
}



// -------------------------------------------------------
//            o o f R e p B a n d 
// -------------------------------------------------------

unsigned long 
oofRepBand::fixedHeight()
{
	assert( !"You should override this function: oofRepBand::fixedHeight()" );
	return 0;
}


unsigned long 
oofRepBand::minHeight()
{
	return fixedHeight();
}


bool 
oofRepBand::isViewBand() const
{
	return false;
}


bool 
oofRepBand::isLayoutBand() const
{
	return false;
}


void
oofRepBand::drawSquished(long& /*balanceToSave*/)
{
	draw();
}


// -------------------------------------------------------
//          o o f R e p B l o c k
// -------------------------------------------------------

oofRepBlock::oofRepBlock(oofRepTextStyle* adoptedStyle) :
	oofRepDrawable(adoptedStyle),
	mAlignment(alignLeft),
	mMoveDownAfter(false),
	mFixedWidth(0)
{
}


oofRepBlock&
oofRepBlock::align(oofRepBlock::blockAlignT A)
{
	mAlignment = A;
	return *this;	// allow chaining in << lists
}

	
oofRepBlock&  
oofRepBlock::fixedWidth(unsigned long inWidth) 
{
	mFixedWidth = inWidth;
	return *this;	// allow chaining in << lists
}


void 
oofRepBlock::moveDownAfter(bool b)
{
	mMoveDownAfter = b;
}


/**
	\return text style of this block or whatever's drawing it.
	Can cope with no currentLayout as may be called when not drawing.
	\see OOF_repXMLstyleCleanupVisitor::VisitDrawable 
*/
oofRepTextStyle* 
oofRepBlock::textStyle() const
{
	if (mStyle)
		return mStyle;
	else {
		oofRepLayoutBand* drawingUs = oofRepLayoutBand::currentLayout();  // in case called from context when not true,
		
		oofRepTextStyle* ret = 0;
		if (drawingUs)
			ret = drawingUs->textStyle();
		return ret;
	}
}


unsigned int 
oofRepBlock::countCells() const
{
	return 0;
}


oofString 
oofRepBlock::getCellText(bool* outIsEditable, oofString* /*outLocationName*/, long /*row*/, short /*col*/) const
{
	if (outIsEditable)
		*outIsEditable = false;
		
	return oofString();
}


void  
oofRepBlock::setCellText(const oofString&, long /*row*/, short /*col*/)
{
}



// -------------------------------------------------------
//     o o f R e p T e x t S t y l e
// -------------------------------------------------------

oofRepTextStyle::oofRepTextStyle():  // protected default
	mFontSize(0),
	mStyleNum(0),
	mLeading(0),
	mColor(oofColor::Black),
	mAlignment(oofColSizer::alignLeft)
{	
#ifdef _Windows
	memset(&mLogFont, 0, sizeof(LOGFONT));
#endif
}


oofRepTextStyle::oofRepTextStyle(const char* fontName, unsigned short fontSize, unsigned char styleNum, short leading, oofColor color,oofColSizer::colAlignT aLin) :
	mFontName(fontName),
	mFontSize(fontSize),
	mStyleNum(styleNum),
	mLeading(leading),
	mColor(color),
	mAlignment(aLin)
{	
	finishConstruction();
}

#ifdef _Windows
/*
AD 981008 removed as it doesn't set the other members
and isn't in use at present. If we want to use it again it
MUST set the other members particularly mLeading

oofRepTextStyle::oofRepTextStyle(const LOGFONT& lf) :
	mLogFont(lf)
{
}
*/
#endif

oofRepTextStyle::oofRepTextStyle(const char* fontName, unsigned short fontSize,oofColSizer::colAlignT aLin, unsigned char styleNum, short leading, oofColor color):
	mFontName(fontName),
	mFontSize(fontSize),
	mStyleNum(styleNum),
	mLeading(leading),
	mColor(color),
	mAlignment(aLin)
{	
	finishConstruction();
}


void
oofRepTextStyle::finishConstruction()
{
#ifdef _Windows
	memset(&mLogFont, 0, sizeof(LOGFONT));

	strcpy(mLogFont.lfFaceName, mFontName);
	mLogFont.lfHeight = -mFontSize;
	mLogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	mLogFont.lfCharSet = DEFAULT_CHARSET;  // rely on size & family
	mLogFont.lfWeight = mStyleNum & bold ? FW_BOLD : FW_NORMAL;
	mLogFont.lfItalic = mStyleNum & italic ? TRUE : FALSE;
	mLogFont.lfUnderline = mStyleNum & underline ? TRUE : FALSE;
#endif
	assert(invariant());
}


oofRepTextStyle*
oofRepTextStyle::clone() const
{
	return new oofRepTextStyle(*this);
}


bool 
oofRepTextStyle::operator==(const oofRepTextStyle& rhs) const
{
// compare in order of fastest comparisons (eg: ints) but also most likely to differ
// so put color last
	const bool ret = (
		(mFontSize == rhs.mFontSize) &&
		(mStyleNum == rhs.mStyleNum) &&
		(mLeading == rhs.mLeading) &&
		(mFontName == rhs.mFontName) &&
		(mColor == rhs.mColor)
	);
	return ret;
}



unsigned long 
oofRepTextStyle::fixedHeight()
{
	unsigned long ret = oofRep::currentReport()->environment()->heightOfTextStyle(this);
	return ret;
}


void
oofRepTextStyle::extract(ostream& os) const
{
	os << mFontName << " " << mFontSize << " " << mStyleNum << " " << mLeading << " " << mColor ;
}


// -------------------------------------------------------
//     o o f R e p T e x t S t y l a b l e
// -------------------------------------------------------
oofRepTextStylable::oofRepTextStylable(oofRepTextStyle* adoptedStyle) :
	mStyle(adoptedStyle)
{
}


oofRepTextStylable::oofRepTextStylable(const char* fontName, unsigned short fontSize, unsigned char styleNum, short leading, oofColor color)
{
	mStyle = new oofRepTextStyle(fontName, fontSize, styleNum, leading, color);
}


oofRepTextStylable::oofRepTextStylable(const oofRepTextStylable& rhs) :
	mStyle(0)
{
	if (rhs.mStyle)
		mStyle = rhs.mStyle->clone();
}


oofRepTextStylable::~oofRepTextStylable()
{
	delete mStyle;
}


void 
oofRepTextStylable::textStyle(oofRepTextStyle* adoptedStyle)
{
	delete mStyle;
	mStyle = adoptedStyle;
}


void
oofRepTextStylable::textStyle(const char* fontName, unsigned short fontSize, unsigned char styleNum, short leading, oofColor color)
{
	delete mStyle;
	mStyle = new oofRepTextStyle(fontName, fontSize, styleNum, leading, color);
}


oofRepTextStyle* 
oofRepTextStylable::textStyle() const
{
	if (mStyle)
		return mStyle;
	else {
		oofRepTextStyle* ret = oofRep::currentReport()->settings()->defaultTextStyle();
		return ret;
	}
}


// -------------------------------------------------------
//            o o f R e p B a n d L i s t
// -------------------------------------------------------

oofRepBandList::oofRepBandList(keepTogetherT keepTogether) :
	mKeepTogether(keepTogether),
	mFixedHeight(ULONG_MAX)
{
}


oofRepBandList::oofRepBandList(const oofRepBandList& rhs) 
{
	*this = rhs;
}


const oofRepBandList& 
oofRepBandList::operator=(const oofRepBandList& rhs)
{
	mKeepTogether = rhs.mKeepTogether;
	mFixedHeight = rhs.mFixedHeight;
	mMinHeight = rhs.mMinHeight;
	unsigned long numItems = rhs.count();
	for (unsigned long i=0; i<numItems; i++) {
		oofRepBand* theBand = rhs.value(i);
		append(theBand->clone());
	}
	return *this;
}


oofRepBandList::~oofRepBandList()
{
	for (start(); more(); next()) {
		oofRepBand* theBand = current();
		delete theBand;
	}
}
	

oofRepBandList* 
oofRepBandList::clone()
{
	return new oofRepBandList(*this);
}


void 
oofRepBandList::append(oofRepBand* adoptedBand)
{
	mBands.append((unsigned long) adoptedBand);
}


unsigned long 
oofRepBandList::fixedHeight()
{
	if (mFixedHeight==ULONG_MAX) {  // calc it
		unsigned long totFixedHeight = 0;
		const unsigned long numItems = count();
		for (unsigned long i=0; i<numItems; i++) {
			oofRepBand* theBand = value(i);
			totFixedHeight += theBand->fixedHeight();		
		}
		mFixedHeight = totFixedHeight;
	}
	return mFixedHeight;
}


void 
oofRepBandList::finishConstruction()
{
// remember that finishConstruction is not called again just because report size changes!
	bool canKeepTogether = true;
	unsigned long totFixedHeight = 0;
	unsigned long totMinHeight = 0;
	const unsigned long numItems = count();
	for (unsigned long i=0; i<numItems; i++) {
		oofRepBand* theBand = value(i);
		if (theBand->isViewBand())
			canKeepTogether = false;  // variable height view bands can't be in keepTogether
		else {
		// save virtual calls to viewband as it has no height
			totFixedHeight += theBand->fixedHeight();		
			totMinHeight += theBand->minHeight();  // call after band has finishConstruction		
		}
	}
	if (mKeepTogether==eDefaultTogether) 
		keepTogether(canKeepTogether);

// store totals back into member variables
// fixed height uncalculated or hasn't changed
	assert((mFixedHeight==ULONG_MAX) || (mFixedHeight == totFixedHeight));
	mFixedHeight = totFixedHeight;
	mMinHeight = totMinHeight;
}



	

// -------------------------------------------------------
//            o o f R e p B l o c k L i s t
// -------------------------------------------------------

oofRepBlockList::oofRepBlockList() 
{
}


oofRepBlockList::oofRepBlockList(const oofRepBlockList& rhs) 
{
	*this = rhs;
}


const oofRepBlockList& 
oofRepBlockList::operator=(const oofRepBlockList& rhs)
{
	unsigned long numItems = rhs.count();
	for (unsigned long i=0; i<numItems; i++) {
		oofRepBlock* theBlock = rhs.value(i);
		append(theBlock->clone());
	}
	return *this;
}


oofRepBlockList* 
oofRepBlockList::clone()
{
	return new oofRepBlockList(*this);
}


oofRepBlockList::~oofRepBlockList()
{
	for (start(); more(); next()) {
		oofRepBlock* theBlock = current();
		delete theBlock;
	}
}
	

void 
oofRepBlockList::append(oofRepBlock* adoptedBlock)
{
	mBlocks.append((unsigned long) adoptedBlock);
}


unsigned long 
oofRepBlockList::fixedHeight()
{
// sum of tallest blocks in each row
	unsigned long ret = 0;
	unsigned long maxHeightInRow = 0;
	unsigned long numItems = count();
	for (unsigned long i=0; i<numItems; i++) {
		oofRepBlock* theBlock = value(i);
		unsigned long bh = theBlock->fixedHeight();
		if (bh > maxHeightInRow) {
			maxHeightInRow = bh;
		}
		if (theBlock->moveDownAfter()) {
			ret += maxHeightInRow;
			maxHeightInRow = 0;
		}
	}
	ret += maxHeightInRow;	// add height of last row even if not a moveDownAfter
	return ret;
}



	

// -------------------------------------------------------
//            o o f R e p B r e a k L i s t
// -------------------------------------------------------

oofRepBreakList::oofRepBreakList(bool ownsBreaks) :
	mOwnsBreaks(ownsBreaks)
{
}


oofRepBreakList::oofRepBreakList(const oofRepBreakList& rhs) 
{
	*this = rhs;
}


const oofRepBreakList& 
oofRepBreakList::operator=(const oofRepBreakList& rhs)
{
	unsigned long numItems = rhs.count();
	for (unsigned long i=0; i<numItems; i++) {
		oofRepBreak* theBreak = rhs.value(i);
		append(theBreak->clone());
	}
	return *this;
}


oofRepBreakList* 
oofRepBreakList::clone()
{
	return new oofRepBreakList(*this);
}


oofRepBreakList::~oofRepBreakList()
{
	if (mOwnsBreaks)
		for (start(); more(); next()) {
			oofRepBreak* theBreak = current();
			delete theBreak;
		}
}


void 
oofRepBreakList::append(oofRepBreak* adoptedBreak)
{
	mBreaks.append((unsigned long) adoptedBreak);
}


// -------------------------------------------------------
//            o o f R e p B r e a k 
// -------------------------------------------------------

oofRepBreak::oofRepBreak(oofRepBand* band, bool newPageEveryTime) :
	mBand(band),
	mNewPageEveryTime(newPageEveryTime)
{
}


oofRepBreak::oofRepBreak(const oofRepBreak& rhs) :
	mBand(rhs.mBand->clone()),
	mNewPageEveryTime(rhs.mNewPageEveryTime)
{
}


oofRepBreak::~oofRepBreak()
{
	delete mBand;
}


bool 
oofRepBreak::newPage()
{
	return mNewPageEveryTime;
}


void 
oofRepBreak::finishConstruction()
{
	if (mBand)
		mBand->finishConstruction();
		// we OWN this band so we finishConstructing it, the band is not owned
		// or accessible by any other report entity
}



// -------------------------------------------------------
//      o o f R e p B r e a k E v e r y R e c o r d
// -------------------------------------------------------
oofRepBreakEveryRecord::oofRepBreakEveryRecord(oofRepBand* band, bool newPageEveryTime) :
	oofRepBreak(band, newPageEveryTime)
{
}


oofRepBreak* 
oofRepBreakEveryRecord::clone()
{
	return new oofRepBreakEveryRecord(*this);
}


bool 
oofRepBreakEveryRecord::breaks()
{
	return true;
}


oofString
oofRepBreakEveryRecord::signature() const
{
	return "oofRepBreakEveryRecord";
}


void 
oofRepBreakEveryRecord::registerSerializable()
{
#ifdef OOF_READ_XML	
	oofSerializableFactory::registerDefaultMake("oofRepBreakEveryRecord", oofRepBreakEveryRecord::makeBreak);
#endif
}


oofSerializable* 
oofRepBreakEveryRecord::makeBreak (const oofString& /*inData*/)
{
	// NOT YET IMPLEMENTED
	return 0;
}


oofString 
oofRepBreakEveryRecord::serialize() const
{
	if (mNewPageEveryTime)
		return "T";
	else
		return "F";
}



// -------------------------------------------------------
//            o o f D E
// -------------------------------------------------------
void 
oofDE::appendElement(oofDE* adoptedDE)
// factory that records element with current page
{
	oofRepPage* currentPage = oofRepPage::currentlyConstructing();
	assert(currentPage);
	currentPage->appendElement(adoptedDE);
}



// -------------------------------------------------------
//    o o f R e p C u s t o m V i e w D r a w e r
// -------------------------------------------------------
/**
	Called for non-GUI rendering eg: oofRepViewBand_TSVRender::draw.
	override if your field contains custom content, eg: RTF
	if your custom drawer just manipulates a plain text field then 
	you can rely on this base method
*/	
oofString 
oofRepCustomViewDrawer::plainText(const dbField* inField)
{
	return inField->copyString();
}
