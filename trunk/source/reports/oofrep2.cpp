// COPYRIGHT 1996 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// drawing code (Mac GUI only initially)

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFSIZE
	#include "oofsize.h"
#endif
#include <string.h>
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"	// abstract renderers
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef _Macintosh
	#ifndef OOF_REP_NO_GUI 

		#ifdef __PowerPlant__
			#ifndef _H_UStandardFile
				#include <UStandardFile.h> // want UDesktop PowerPlant 'out' to let us call standard file
			#endif
		#endif
		#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
			#ifndef __NAVIGATION__
				#include <Navigation.h>
			#endif
		#else
			#ifndef __STANDARDFILE__
				#include <StandardFile.h>
			#endif
		#endif
		#ifndef _H_UDesktop
			#include <UDesktop.h>
		#endif
	#endif
#endif

ostream* oofRepStreamEnv::sOut = 0;
oofRepPage* oofRepPage::sCurrentPage=0;	

// -------------------------------------------------------
//            o o f R e p V i e w B a n d 
// -------------------------------------------------------

oofRepViewBand::oofRepViewBand(const dbView& view, bool isBoxed) :
	mView(new dbView(view)),		// copy ctor, so people can use temp objects in calling us
	mIsBoxed(isBoxed),
	mTableExtraRender(0),
	mNonEditableFields(0),
	mColHeadersNeedCalc(true)
{
	FinishConstruction();
}


oofRepViewBand::oofRepViewBand(dbView* adoptedView, bool isBoxed) :
	mView(adoptedView),		
	mIsBoxed(isBoxed),
	mTableExtraRender(0),
	mNonEditableFields(0),
	mColHeadersNeedCalc(true)
{
	FinishConstruction();
}


void
oofRepViewBand::FinishConstruction()
{
	if( mIsBoxed ) {
		adorners() << new oofBorderAdorner( oofBorderAdorner::oneThin );
	}
	mCustomDrawers.ownsContents(true);
	const unsigned int numCols = mView->count();
	for (unsigned int i=0; i<numCols; i++)
		mCustomDrawers[i] = 0;
}


oofRepViewBand::oofRepViewBand(const oofRepViewBand& rhs) :
	mHeaders(rhs.mHeaders),
	mFooters(rhs.mFooters),
	mHeaderBreaks(rhs.mHeaderBreaks),
	mFooterBreaks(rhs.mFooterBreaks),
	mView(new dbView(*rhs.mView)),	
	mIsBoxed(rhs.mIsBoxed),
	mHeight(rhs.mHeight),
	mCustomDrawers(rhs.mCustomDrawers),
	mNonEditableFields(0),  // copied below
	mColHeadersNeedCalc(true)	
{
	mTableExtraRender = rhs.mTableExtraRender->finishClone(mRender);  // call virtual fix!
	if (rhs.mNonEditableFields) {
		const unsigned long numFlags = mView->count();
		mNonEditableFields = new bool[numFlags];
		::memcpy(mNonEditableFields, rhs.mNonEditableFields, numFlags*sizeof(bool));
	}
}


oofRepViewBand::~oofRepViewBand()
{
	delete mView;
	delete[] mNonEditableFields;
}


oofRepBand* 
oofRepViewBand::clone() const
{
	return new oofRepViewBand(*this);
}


bool 
oofRepViewBand::isViewBand() const
{
	return true;
}


dbView* 
oofRepViewBand::view(unsigned short whichView) const
{
	if (whichView==0)
		return mView;
	else
		return NULL;
}


oofRepCustomViewDrawer* 
oofRepViewBand::customDrawer(unsigned short zeroBasedCol)
{
	oofRepCustomViewDrawer*  ret;
	if (zeroBasedCol>=mCustomDrawers.count())
		ret = 0;
	else
		ret = (oofRepCustomViewDrawer*) mCustomDrawers.value(zeroBasedCol);  // safe downcast
	return ret;
}


void 
oofRepViewBand::customDrawer(unsigned short zeroBasedCol, oofRepCustomViewDrawer* adoptedDrawer)
{
	mCustomDrawers[zeroBasedCol] = adoptedDrawer;
}

#ifndef OOF_REP_NO_GUI 
oofRepEditLink*	
oofRepViewBand::makeEditLink(const oofRepEditingEnv& inEnv)
{
	return NULL;
}
#endif

void
oofRepViewBand::draw()
{
	assert(mRender);
	mRender->draw();
}

unsigned long
oofRepViewBand::fixedHeight()
{
	return mHeight;
}

void
oofRepViewBand::incrementHeight( unsigned long addedHeight )
{
	mHeight = mHeight + addedHeight;
}

void
oofRepViewBand::clearHeight()
{
	mHeight = 0;
}

void 
oofRepViewBand::drawColumnHeaders()
{
// central default call for drawing, but still able to be overriden in extreme
// cases. Now that all is factored out to the render objects (Strategy pattern)
// we shouldn't need to do anything.
	assert(mTableExtraRender);
	mTableExtraRender->drawColumnHeaders();	
}


void
oofRepViewBand::startTableBody()
{
	assert(mTableExtraRender);
	mTableExtraRender->startTableBody(mColHeadersNeedCalc);	
	mColHeadersNeedCalc = false;
}


void
oofRepViewBand::endTableBody()
{
	assert(mTableExtraRender);
	mTableExtraRender->endTableBody();	
}


void
oofRepViewBand::startTableWrapper()
{
	assert(mTableExtraRender);
	mTableExtraRender->startTableWrapper();
}


void
oofRepViewBand::endTableWrapper()
{
	assert(mTableExtraRender);
	mTableExtraRender->endTableWrapper();
}


bool
oofRepViewBand::lastRow( unsigned long row )
{
	dbTable* viewTable = view()->table();	// OK to be zero!!!!!!
	if (!viewTable)
		return true;
	if( row == (viewTable->count()-1) )
		return true;
	else
		return false;
}

bool
oofRepViewBand::lastCol( unsigned long col )
{
	if( col == (view()->count()-1) )
		return true;
	else
		return false;
}


void 
oofRepViewBand::setFieldNonEditable(unsigned short col)
{
	if (!mNonEditableFields) {
		const unsigned long numFlags = mView->count();
		mNonEditableFields = new bool[numFlags];
		::memset(mNonEditableFields, 0, numFlags*sizeof(bool));
	}
	mNonEditableFields[col] = true;
}


// -------------------------------------------------------
//            o o f R e p V i e w B a n d _ R e n d e r
// -------------------------------------------------------
void 
oofRepViewBand_Render::finishConstruction ()
{
	if (oofRep::currentReport()->shouldDrawViewColumnHeaders()) {
		mDrawnColHeadersThisPage = false;
	}
	else
		mDrawnColHeadersThisPage = true;
	CalcColWidths();
}



void 
oofRepViewBand_Render::CalcColWidths ()
{
	dbView* view = mOwner->view();
	oofRep* curRep = oofRep::currentReport();
	const oofRepSettings* currSettings = curRep->settings();
	const unsigned short	bandIndenting = currSettings->bandIndenting();
	const short colSepWidth = currSettings->colSepWidth();

	curRep->changeTextStyle(mOwner->textStyle());
	unsigned long widthToDivide = curRep->bodyWidth() - (2*bandIndenting);
	if(mOwner->adorners().count()>0)  // allow room assuming adorners do grid?
		widthToDivide -= colSepWidth;
		
	view->colSizer()->calculateWidths(widthToDivide, colSepWidth);
}


OOF_mixViewExtraRender* 
oofRepViewBand_Render::finishClone(oofRepRender* newClone) const
{
	return (oofRepViewBand_Render*)newClone;  // safe downcast
}


// -------------------------------------------------------
//            o o f R e p L a y o u t B a n d 
// -------------------------------------------------------

oofRepLayoutBand::oofRepLayoutBand()  :
	mCalcHeight(false),
	mFixedHeight(0)
{
}


oofRepBand* 
oofRepLayoutBand::clone() const
{
	return new oofRepLayoutBand(*this);
}


bool 
oofRepLayoutBand::isLayoutBand() const
{
	return true;
}


void
oofRepLayoutBand::BecomeCurrent()
{
	sCurrentLayout = this;
}


void 
oofRepLayoutBand::draw()
{
	BecomeCurrent();
	oofRepDrawable::draw();
}


unsigned long 
oofRepLayoutBand::fixedHeight()
{
	BecomeCurrent();	// so style lookups work

	// Now the space needed by the band
	if (!mCalcHeight) {
		mFixedHeight = mBlocks.fixedHeight();
		mCalcHeight = true;
	}
	return mFixedHeight;
}


oofRepLayoutBand& 
oofRepLayoutBand::operator<<(dbField& F)
{
	oofRepFieldBlock* newBlock = new oofRepFieldBlock(&F);
	mBlocks.append(newBlock);
	return *this;
}


oofRepLayoutBand& 
oofRepLayoutBand::operator<<(dbField* F)
{
	oofRepFieldBlock* newBlock = new oofRepFieldBlock(F);
	mBlocks.append(newBlock);
	return *this;
}


oofRepLayoutBand& 
oofRepLayoutBand::operator<<(const char* str)
{
	oofRepTextBlock* newBlock = new oofRepTextBlock(str);
	mBlocks.append(newBlock);
	return *this;
}


oofRepLayoutBand& 
oofRepLayoutBand::operator<<(oofRepBlock* adoptedBlock)
{
	mBlocks.append(adoptedBlock);
	return *this;
}


oofRepLayoutBand& 
oofRepLayoutBand::operator<<(const oofRepBlock& copiedBlock)
{
	mBlocks.append(copiedBlock.clone());
	return *this;
}


// -------------------------------------------------------
//            o o f R e p S p a c e B a n d 
// -------------------------------------------------------

oofRepSpaceBand::oofRepSpaceBand(unsigned long heightInPoints) :
	mHeight(heightInPoints)
{
}


oofRepBand* 
oofRepSpaceBand::clone() const
{
	return new oofRepSpaceBand(*this);
}



// -------------------------------------------------------
//            o o f R e p G r a p h B a n d 
// -------------------------------------------------------

oofRepGraphBand::oofRepGraphBand(oofGraph* inGraphPtr, unsigned long height, 
								 unsigned long grHgt, unsigned long grWid, const char* inGraphTitle) :
	mHeight(height),
	mGraphPtr(inGraphPtr),
	mGrHgt(grHgt),
	mGrWid(grWid),
	mMinSizePercent(kUseDefaultScalePercent),
	mSavedGrHgt(grHgt),
	mSavedGrWid(grWid)
{
	AdjustGraphToBandHeight(height);

	oofRect theFrame(0,0,mGrWid,mGrHgt);  // make sure to use member vars after Adjust...
	inGraphPtr->setPaneRect(theFrame);
	if (inGraphTitle)
		inGraphPtr->setTitle(inGraphTitle);
}


oofRepGraphBand::oofRepGraphBand(const oofRepGraphBand& rhs) :
	mHeight(rhs.mHeight),
	mGraphPtr(0),
	mGrHgt(rhs.mGrHgt),
	mGrWid(rhs.mGrWid),
	mMinSizePercent(rhs.mMinSizePercent),
	mSavedGrHgt(rhs.mSavedGrHgt),
	mSavedGrWid(rhs.mSavedGrWid)
{
	assert(rhs.mGraphPtr);
	if (rhs.mGraphPtr)
		mGraphPtr = rhs.mGraphPtr->clone();
}


oofRepGraphBand::~oofRepGraphBand()
{
	delete mGraphPtr;
}


oofRepBand* 
oofRepGraphBand::clone() const
{
	return new oofRepGraphBand(*this);
}


dbView* 
oofRepGraphBand::view(unsigned short whichView) const
{
	assert(mGraphPtr);
	dbView*  ret = 0;
	
	if (whichView==0)	
		ret = mGraphPtr->getView();
	else if (whichView==1)	{
		OOF_mixOverlayableGraph* asOverlayable = dynamic_cast<OOF_mixOverlayableGraph*>(mGraphPtr);
		if  (asOverlayable)
			ret = asOverlayable->getOverlayView();
	}
	return ret;
}


unsigned long 
oofRepGraphBand::numOverlaid() const
{
	assert(mGraphPtr);
	unsigned long  ret = 0;
	
	OOF_mixOverlayableGraph* asOverlayable = dynamic_cast<OOF_mixOverlayableGraph*>(mGraphPtr);
	if  (asOverlayable)
		ret = asOverlayable->getNumOverlaid();
	return ret;
}


void
oofRepGraphBand::draw()
{
	RestoreDimensions();
	oofRepBand::draw();
}


void
oofRepGraphBand::RestoreDimensions()
{
	mDrawingHeight = mHeight;
	mGrHgt = mSavedGrHgt;
	mGrWid = mSavedGrWid;
}


void 
oofRepGraphBand::graphHeight( unsigned long grHeight )	
{ 
	mSavedGrHgt = mGrHgt = grHeight; 
}


void 
oofRepGraphBand::graphWidth( unsigned long grWidth )	
{ 
	mSavedGrWid = mGrWid = grWidth; 
}


void
oofRepGraphBand::drawSquished(long& balanceToSave)
{
	RestoreDimensions();  // may be adjusted in AdjustGraphToBandHeight
	const long availableReduction = mHeight-mMinHeight;
	
	long reducedToFit;
/*
we don't refuse to shrink any more - our caller makes that decision
	if (availableReduction<balanceToSave) {
		balanceToSave -= availableReduction;
		reducedToFit = mMinHeight;
	}
	else 
*/
	{
		reducedToFit = mHeight - balanceToSave;	
		balanceToSave = 0;  // this graph absorbed entire loss
	}
// NOTE this is a pretty weak algorithm that assumes just one graph band
// in a keptTogether bandlist will shrink
		
	AdjustGraphToBandHeight(reducedToFit);
	oofRepBand::draw();
}


unsigned long
oofRepGraphBand::fixedHeight()
{
	return mHeight;
}


unsigned long 
oofRepGraphBand::minHeight()
{
	return mMinHeight;
}


void 
oofRepGraphBand::finishConstruction()
{
	// need to ensure a font is set before graph sizes
	oofRep::currentReport()->changeTextStyle(textStyle());
	
// work out min height BEFORE scaling mHeight to report body size
	unsigned short minSizePercent;
	if (mMinSizePercent==kUseDefaultScalePercent)
		minSizePercent = oofRep::currentReport()->settings()->graphBandMinSizePercent();
	else
		minSizePercent = mMinSizePercent;
		
	assert(minSizePercent<=100);
	if (minSizePercent<100)
		mMinHeight = (mHeight*minSizePercent)/100;
	else
		mMinHeight = mHeight;

	const unsigned long bodyHeight = oofRep::currentReport()->bodyHeight();
	AdjustGraphToBandHeight(bodyHeight);  // guarantees will fit page

	mGraphPtr->finishConstructionWithPresets();
	if (!mGraphPtr->drawStylesWereSet()) {
		oofDrawStyleSetDefaultIteration tempIter;
		mGraphPtr->setDrawStyles(tempIter);
	}
}


void
oofRepGraphBand::AdjustGraphToBandHeight(unsigned long inHeight)
{
	if (mGrHgt>inHeight) {
		mDrawingHeight = inHeight;  // from some contexts, may also change band height
		unsigned long reduceBy = mGrHgt - mDrawingHeight;
//		mGrWid -= mGrWid * reduceBy / mGrHgt;	// reduce width in proportion to height reduction
		mGrHgt -= reduceBy;
	}
	else if (mDrawingHeight>inHeight)
		mDrawingHeight = inHeight;  // from some contexts, may also change band height
	if (mMinHeight>mDrawingHeight)
		mMinHeight = mDrawingHeight;
}


// -------------------------------------------------------
//            o o f R e p L i n e B a n d 
// -------------------------------------------------------

oofRepLineBand::oofRepLineBand(unsigned long heightInPoints, bool isGrey ) :
	mHeight(heightInPoints),
	mGrey(isGrey)
{
}


oofRepBand* 
oofRepLineBand::clone() const
{
	return new oofRepLineBand(*this);
}



// -------------------------------------------------------
//            o o f R e p P i c t B a n d 
// -------------------------------------------------------

oofRepPictBand::oofRepPictBand(oofPict* adoptedPict) :
	OOF_mixPictOwner(adoptedPict)
{}


oofRepBand* 
oofRepPictBand::clone() const
{
	return new oofRepPictBand(*this);
}


// -------------------------------------------------------
//          o o f R e p T e x t B l o c k
// -------------------------------------------------------
oofRepTextBlock::oofRepTextBlock(const char* text, oofRepTextStyle* adoptedStyle) :
	oofRepBlock(adoptedStyle),
	mText(text)
{
	CompleteInit();
}


oofRepTextBlock::oofRepTextBlock(const oofString& text, oofRepTextStyle* adoptedStyle) :
	oofRepBlock(adoptedStyle),
	mText(text)
{
	CompleteInit();
}


/*
oofRepTextBlock::oofRepTextBlock(ostream& os, oofRepTextStyle* S) :
	oofRepBlock(S),
	mText(os)
{
	CompleteInit();
}
*/

void
oofRepTextBlock::CompleteInit()
{
	if (mText.lastChar()=='\n') {	// remove linefeeds - treat as flag to set moveDownAfter
		mText.shorten(1);
		moveDownAfter(true);
	}
}

oofRepBlock*
oofRepTextBlock::clone() const
{
	return new oofRepTextBlock(*this);
}


oofString 
oofRepTextBlock::text() const
{
	return mText;  // but our subclasses will construct their return string
}


unsigned int 
oofRepTextBlock::countCells() const
{
	return 1;
}


oofString 
oofRepTextBlock::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col) const
{
	assert(row==0);
	assert(col==0);
	if (outIsEditable)
		*outIsEditable = true;  // fixed text always editable
		
	if (outLocationName)
		outLocationName->clear();  // we have no idea where we are!
		
	return mText;
}


void  
oofRepTextBlock::setCellText(const oofString& inString, long row, short col)
{

	assert(row==0);
	assert(col==0);
	mText = inString;
}


// -------------------------------------------------------
//    o o f R e p B r a c k e t e d T e x t B l o c k
// -------------------------------------------------------
oofRepBracketedTextBlock::oofRepBracketedTextBlock(const char* leadingText, const char* trailingText, oofRepTextStyle* adoptedStyle) :
	oofRepTextBlock(leadingText, adoptedStyle), // leadingText becomes mText in our parent
	mTrailingText(trailingText)
{
// NOTE our parent may have already set moveDownAfter due to the leading text having a 
// trailing linefeed
	if (mTrailingText.lastChar()=='\n') {	// remove linefeeds - treat as flag to set moveDownAfter
		mTrailingText.shorten(1);
		moveDownAfter(true);
	}
}


oofString 
oofRepBracketedTextBlock::text() const
{
// virtual call for subclass to provide the middle!
	return oofString(mText, MiddleText(), mTrailingText);
}



unsigned int 
oofRepBracketedTextBlock::countCells() const
{
	return 3;  // even if no content in leading or trailing, user may want to insert
}



// -------------------------------------------------------
//          o o f R e p P i c t B l o c k
// -------------------------------------------------------
oofRepPictBlock::oofRepPictBlock(oofPict* adoptedPict, oofRepTextStyle* adoptedStyle) :
	oofRepBlock(adoptedStyle),
	OOF_mixPictOwner(adoptedPict)
{}


oofRepBlock*
oofRepPictBlock::clone() const
{
	return new oofRepPictBlock(*this);
}


unsigned long
oofRepPictBlock::fixedHeight()
{
	unsigned long ret = OOF_mixPictOwner::height();
	if (mFixedWidth) {  // scale the height to ratio of widths as user constrained
		const unsigned long actualWidth = OOF_mixPictOwner::width();
		if (mFixedWidth < actualWidth)
			ret = ret * mFixedWidth / actualWidth;  // scale down
// if pic smaller than specified width do NOT stretch up
	}
	return ret;
}


unsigned long
oofRepPictBlock::width()
{
	if (mFixedWidth)
		return mFixedWidth;  // allows block to be constrained, rather than driven by picture
	else
		return OOF_mixPictOwner::width();
}


unsigned int 
oofRepPictBlock::countCells() const
{
	return 1;
}


oofString 
oofRepPictBlock::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col) const
{
	assert(row==0);
	assert(col==0);
	if (outIsEditable)
		*outIsEditable = false;

	if (outLocationName)
		*outLocationName = "picture";
				
	return picture()->describe();
}


// -------------------------------------------------------
//          o o f R e p F i e l d B l o c k
// -------------------------------------------------------
oofRepFieldBlock::oofRepFieldBlock(dbField* F, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(0, 0, adoptedStyle),
	mField(F)
{
}


oofRepFieldBlock::oofRepFieldBlock(const oofString& leadingText, dbField* F, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(leadingText, 0, adoptedStyle),
	mField(F)
{}


oofRepFieldBlock::oofRepFieldBlock(const oofString& leadingText, dbField* F, const oofString& trailingText, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(leadingText, trailingText, adoptedStyle),
	mField(F)
{}


oofString 
oofRepFieldBlock::MiddleText() const
{
	return oofString(mField->copyString());
}


oofRepBlock* 
oofRepFieldBlock::clone() const
{
	return new oofRepFieldBlock(*this);
}


oofString 
oofRepFieldBlock::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col) const
{
	assert(row==0);
	assert(col<3);
	
	if (outIsEditable) {
		if (col==1)
			*outIsEditable = mField->fieldIsStandalone();  // don't let them edit fields on databases
		else
			*outIsEditable = true;  // fixed text always editable
	}
	
	if (outLocationName)
		outLocationName->clear();  // we have no idea where we are!
		
	switch(col) {
	case 0 :
		return mText;
	
	case 1 :
		return mField->copyString();
	
	default :
		return mTrailingText;
	}
}


void  
oofRepFieldBlock::setCellText(const oofString& inString, long row, short col)
{
	assert(row==0);
	assert(col<3);
	
	switch(col) {
	case 0 :
		mText = inString;
		break;
	
	case 1 :
		assert( mField->fieldIsStandalone() );
		mField->setString(inString);
		break;
	
	default :
		mTrailingText = inString;
	}
}


// -------------------------------------------------------
//          o o f R e p P a g e N o B l o c k
// -------------------------------------------------------
oofRepPageNoBlock::oofRepPageNoBlock(const char* leadingText, const char* trailingText, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(leadingText, trailingText, adoptedStyle)
{
	mAlignment = alignRight;
}


oofRepPageNoBlock::oofRepPageNoBlock(const oofString& leadingText, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(leadingText, 0, adoptedStyle)
{
	mAlignment = alignRight;
}


oofRepPageNoBlock::oofRepPageNoBlock(const oofString& leadingText, const oofString& trailingText, oofRepTextStyle* adoptedStyle) :
	oofRepBracketedTextBlock(leadingText, trailingText, adoptedStyle)
{
	mAlignment = alignRight;
}


oofString 
oofRepPageNoBlock::MiddleText() const
{
	char pn[10];
	sprintf(pn,"%d",oofRep::currentReport()->pageNumber());
	return oofString(pn);
}


oofRepBlock*
oofRepPageNoBlock::clone() const
{
	return new oofRepPageNoBlock(*this);
}


oofString 
oofRepPageNoBlock::getCellText(bool* outIsEditable, oofString* outLocationName, long row, short col) const
{
	assert(row==0);
	assert(col<3);
	
	if (outIsEditable) 
		*outIsEditable = (col!=1);  // fixed text always editable, but not page num!
	
	switch(col) {
	case 0 :
		if (outLocationName)
			*outLocationName = "Page number prefix";
		return mText;
	
	case 1 :
		if (outLocationName)
			*outLocationName = "Page number";
		return MiddleText();
	
	default :
		if (outLocationName)
			*outLocationName = "Page number suffix";
		return mTrailingText;
	}
}


void  
oofRepPageNoBlock::setCellText(const oofString& inString, long row, short col)
{
	assert(row==0);
	assert(col==0 || col==2);
	
	if (col==0)
		mText = inString;
	else
		mTrailingText = inString;
}


// -------------------------------------------------------
//          o o f R e p E n v i r o n m e n t
// -------------------------------------------------------
bool
oofRepEnvironment::canDraw()
{
	return true;
}


bool
oofRepEnvironment::isPrintingEnvironment() const
{
	return false;
}


bool
oofRepEnvironment::preprocessesPageHeadersAndFooters() const
{
	return false;
}


void 
oofRepEnvironment::startEnvironment()
{
// dummy so overrides not required
}


void 
oofRepEnvironment::startSection()
{
// dummy so overrides not required
}


void 
oofRepEnvironment::endEnvironment()
{
// dummy so overrides not required
}

void
oofRepEnvironment::drawPageBorder( long /*top*/, long /*left*/, long /*bottom*/, long /*right*/ )
{
// dummy so overrides not required
}


void
oofRepEnvironment::drawSectionBreak()
{
	drawPageBreak();
}


// -------------------------------------------------------
//          o o f R e p S t r e a m E n v
// -------------------------------------------------------
oofRepStreamEnv::oofRepStreamEnv() :
	mOut(0),
	mOwnedofstream(0)
{
}


oofRepStreamEnv::oofRepStreamEnv(ostream& os) :
	mOut(&os),
	mOwnedofstream(0)  // dunno what kind of stream, not owned by us anyway
{
}


oofRepStreamEnv::oofRepStreamEnv(const char* fileName, bool openBinary) 
{
	if (openBinary)
		mOwnedofstream = new ofstream(fileName, ios::out | ios::binary);	// vulnerable to failure
	else
		mOwnedofstream = new ofstream(fileName);	// vulnerable to failure
	mOut = mOwnedofstream;
	// NOT YET IMPLEMENTED - have exception handling to survive failure to 
	// produce report
}


oofRepStreamEnv::~oofRepStreamEnv()
{
	if (mOwnedofstream) {
		mOwnedofstream->close();  // Mac libs apparently not closing on deletion, only on app exit
		delete mOwnedofstream;	// of which mOut & sOut will be a copy
	}
}


/**
	Confirm can draw to stream - only used for GUI to choose destination
	\todo check why there's only a Mac file dialog here - do we use under Win?
	\todo add exception for NavGetDefaultDialogOptions failing.
*/
bool
oofRepStreamEnv::canDraw()
{
   	bool ret = false;
	if (mOut) // got stream
		ret = (mOut->good() != 0);
	else { // open a file
#ifdef _Macintosh
	#ifndef OOF_REP_NO_GUI
		#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
			OSErr               anErr = noErr;
			NavReplyRecord      reply;
			NavDialogOptions    dialogOptions;
			OSType              fileTypeToSave = 'TEXT';
			OSType              creatorType = 'xAPP';
			//NavEventUPP         eventProc = NewNavEventProc(myEventProc);

			anErr = NavGetDefaultDialogOptions(&dialogOptions);
			if (anErr == noErr){
				dialogOptions.dialogOptionFlags ^= kNavAllowStationery;  /* No stationery files */
				unsigned char* fName = (unsigned char*)&(dialogOptions.savedFileName);
				unsigned char** fNameH = &fName;
				::SetString(fNameH,"\pUntitled Report");
	
	
	
			
			short nothing;
			 anErr = NavPutFile( (AEDesc *)NULL, &reply, &dialogOptions, (NavEventUPP)NULL,fileTypeToSave, creatorType,&nothing );
				if (anErr == noErr && reply.validRecord){
					AEKeyword   theKeyword;
					DescType    actualType;
					Size        actualSize;
					FSSpec      dstFSSpec;
							
					anErr = AEGetNthPtr(&(reply.selection), 1, typeFSS,
										&theKeyword, &actualType,
										&dstFSSpec, sizeof(dstFSSpec),
										&actualSize );
					if (anErr == noErr){
						stSaveDirectory saveDir;
						oofDirectory reportDir(dstFSSpec);
						reportDir.gotoDirectory();
						// convert Mac name from Pascal string to C string			
						char fileName[63];
						size_t nameLen = dstFSSpec.name[0];
						memcpy(fileName, &dstFSSpec.name[1], nameLen);
						fileName[nameLen] = '\0';
						mOut = new ofstream(fileName);
						ret = mOut->good();
					}
					(void) NavDisposeReply(&reply);
				}
	   
			}
			else 
				assert(!anErr);// to do add an exception
	#else
		
			StandardFileReply reply;
			#ifdef __PowerPlant__
					UDesktop::Deactivate(); // PowerPlant 'out' to let us call standard file
			#endif
				::StandardPutFile("\pSave As...","\pUntitled Report",&reply);
			#ifdef __PowerPlant__
					UDesktop::Activate(); // PowerPlant 'out' to let us call standard file
			#endif
				if (reply.sfGood) {  // user clicked Save button
					stSaveDirectory saveDir;
					oofDirectory reportDir(reply.sfFile);
					reportDir.gotoDirectory();
		// convert Mac name from Pascal string to C string			
					char fileName[63];
					size_t nameLen = reply.sfFile.name[0];
					memcpy(fileName, &reply.sfFile.name[1], nameLen);
					fileName[nameLen] = '\0';
					mOut = new ofstream(fileName);
					ret = mOut->good();
				}
		#endif  // Carbon 
	#else
			assert(0);
	#endif  // OOF_REP_NO_GUI
#endif	// _Macintosh
		}
		sOut = mOut;	// WE ARE NOW ACTIVE!!!!  - need a Mutex around the entire rep draw idea
		return ret;
}

// -------------------------------------------------------
//                 o o f R e p P a g e L i s t
// -------------------------------------------------------
oofRepPageList::~oofRepPageList()
{
	deleteAll();
}


void
oofRepPageList::deleteAll()
{
	const unsigned long numPages = count();
	for (unsigned long i=0; i<numPages; i++)
		delete page(i);
	deleteAllCells();
}


// -------------------------------------------------------
//                 o o f R e p P a g e 
// -------------------------------------------------------
oofRepPage::oofRepPage()
{
	sCurrentPage = this;
}


oofRepPage::~oofRepPage()
{
	const unsigned long numElements = count();
	for (unsigned long i=0; i<numElements; i++)
		delete element(i);
}


void
oofRepPage::draw(const oofRect&)
{
	sCurrentPage = 0;  // only used for construction
	const unsigned long numElements = count();
	for (unsigned long i=0; i<numElements; i++) {
		oofDE* theElement = element(i);
		theElement->draw();
	}
}


// -------------------------------------------------------
//          o o f T e x t S t y l e D E 
// -------------------------------------------------------
oofTextStyleDE::oofTextStyleDE(const oofRepTextStyle* copiedStyle) :
	mStyle(copiedStyle->clone())
{
}


oofTextStyleDE::~oofTextStyleDE()
{
	delete mStyle;
}



// -------------------------------------------------------
//                o o f R e p V i s i t o r
// -------------------------------------------------------

void 
oofRepVisitor::visit(oofRep* inRep)
{
	mRep = inRep;

// we test the count of lists because the overridden visit methods
// may not want to be entered for an empty list - easier to keep that
// test here to save user having to bother	

	if (inRep->isMultiReport()) {
		oofRepMulti* multiRep = (oofRepMulti*)inRep;  // safe downcast
		const unsigned long numSections = multiRep->count();
		for (unsigned long i=0; i<numSections; i++) {
			oofRepSection* theSection = multiRep->section(i);
			assert(theSection);
			visit(theSection);  // recurse each section!
		}
	}
	else {  // case for single report
		inRep->becomeCurrent();  // set horrible static pointer
		inRep->resetViewLevel();
		if (inRep->firstPageHeaders().count()>0) 
			VisitFirstPageHeaders(inRep->firstPageHeaders());
			
		if (inRep->firstPageFooters().count()>0) 
			VisitFirstPageFooters(inRep->firstPageFooters());
	
		if (inRep->pageHeaders().count()>0) 
			VisitPageHeaders(inRep->pageHeaders());
			
		if (inRep->pageFooters().count()>0) 
			VisitPageFooters(inRep->pageFooters());

		oofRepViewBand* bodyBand = inRep->body();
		if (bodyBand) {
			VisitViewBand(bodyBand);
			inRep->resetViewLevel();
		}
	}
}


void
oofRepVisitor::VisitFirstPageHeaders(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitFirstPageFooters(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitPageHeaders(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitPageFooters(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitBody(oofRepViewBand* inBand)
{
	EnterLevel();
	VisitViewBand(inBand);
	LeaveLevel();
}


void
oofRepVisitor::VisitViewHeaders(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitViewFooters(oofRepBandList& inBands)
{
	VisitBandList(inBands);
}


void
oofRepVisitor::VisitViewHeaderBreaks(oofRepBreakList& inBreaks)
{
	VisitBreakList(inBreaks);
}


void
oofRepVisitor::VisitViewFooterBreaks(oofRepBreakList& inBreaks)
{
	VisitBreakList(inBreaks);
}


void
oofRepVisitor::VisitTypedBand(oofRepBand* inBand)
{
	if (inBand->isViewBand())
		VisitViewBand((oofRepViewBand*)inBand);  // safe downcast
	else if (inBand->isLayoutBand())
		VisitLayoutBand((oofRepLayoutBand*)inBand);  // safe downcast
	else
		VisitBand(inBand);
}


void
oofRepVisitor::VisitBandList(oofRepBandList& inBands)
{
	EnterLevel();
	const unsigned long numBands = inBands.count();
	for (unsigned long i=0; i<numBands; i++) {
		oofRepBand* theBand = inBands.value(i);
		assert(theBand);
		VisitTypedBand(theBand);
	}
	LeaveLevel();
}


void
oofRepVisitor::VisitBreakList(oofRepBreakList& inBreaks)
{
	EnterLevel();
	const unsigned long numBreaks = inBreaks.count();
	for (unsigned long i=0; i<numBreaks; i++) {
		oofRepBreak* theBreak = inBreaks.value(i);
		assert(theBreak);
		oofRepBand* theBand = theBreak->band();
		if (theBand) 
			VisitTypedBand(theBand);
	}  // loop breaks
	LeaveLevel();
}


void
oofRepVisitor::VisitAdornerList(oofAdornerList& inList)
{
	const unsigned long numAdorners = inList.count();
	if (numAdorners>0) {
		EnterLevel();
		for (unsigned long i=0; i<numAdorners; i++) {
			oofAdorner* theAdorner = inList.value(i);
			assert(theAdorner);
			VisitAdorner(theAdorner);
		}
		LeaveLevel();
	}
}


void
oofRepVisitor::VisitAdorner(oofAdorner*)
{
// empty default
}


void
oofRepVisitor::VisitBlock(oofRepBlock* inBlock)
{
	VisitDrawable(inBlock);
}


void
oofRepVisitor::VisitBand(oofRepBand* inBand)
{
	VisitDrawable(inBand);
}


void 
oofRepVisitor::VisitDrawable(oofRepDrawable*)
{
// empty default
}


void
oofRepVisitor::VisitViewBand(oofRepViewBand* inBand)
{
	assert(inBand);
	mRep->incrementViewLevel();	
	if (inBand->headers().count()>0)
		VisitViewHeaders(inBand->headers());
		
	if (inBand->headerBreaks().count()>0)
			VisitViewHeaderBreaks(inBand->headerBreaks());
			
	VisitViewBandBody(inBand);
	
	if (inBand->footerBreaks().count()>0)
			VisitViewFooterBreaks(inBand->footerBreaks());
			
	if (inBand->footers().count()>0)
			VisitViewFooters(inBand->footers());
	mRep->decrementViewLevel();	
}


void
oofRepVisitor::VisitViewBandBody(oofRepViewBand* inBand)
{
	VisitBand(inBand);
}


void
oofRepVisitor::VisitLayoutBand(oofRepLayoutBand* inBand)
{
	VisitBlockList(inBand->blocks());
	VisitBand(inBand);  // don't forget we are a band in our own right!
}


void
oofRepVisitor::VisitBlockList(oofRepBlockList& inBlocks)
{
	EnterLevel();
	const unsigned long numBlocks = inBlocks.count();
	for (unsigned long i=0; i<numBlocks; i++) {
		oofRepBlock* theBlock = inBlocks.value(i);
		assert(theBlock);
		VisitBlock(theBlock);
	}  // loop blocks
	LeaveLevel();
}


void
oofRepVisitor::EnterLevel()
{
// empty default
}


void
oofRepVisitor::LeaveLevel()
{
// empty default
}
