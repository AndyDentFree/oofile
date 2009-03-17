// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories & renderers for Macintosh

#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPMAC
	#include "oofRepMac.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif


// USE WINDOWS DUMMIES

// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepSpaceBand::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepGraphBand::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepLineBand::makeRender(const oofRepWinEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepPictBand::makeRender(const oofRepWinEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepLayoutBand::makeRender(const oofRepWinEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepTextBlock::makeRender(const oofRepWinEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepPictBlock::makeRender(const oofRepWinEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


// ---------------------------------------
//     o o f R e p M a c E n v
// ---------------------------------------

void	
oofRepMacEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepMacEnv&.
// The implication is that, if we add an environment, each drawable must have a makeRender
// added to match, or it will not be rendered (the base empty method will be invoked).

// eg: if we need to handle printing and previewing differently on the Mac, we would end up
// with a makeRender(const oofRepMacPrint&) and makeRender(const oofRepMacGUIPreviewPP&)
// for each oofRepDrawable

// The alternative would be to have each environment know each drawable. That is far
// less extensible - users can add their own drawables that we wouldn't know about,
// so the environment classes wouldn't be closed for extension.
	drawable->makeRender(*this);
}


void	
oofRepMacEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}


// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepMacEnv::renderType() const
{
	return eRenderType;
}


void
oofRepMacEnv::pageSetup(THPrint inPageSetup)
{
	if (inPageSetup) {
		// Copy the one passed to us
		mPrintRecordH = inPageSetup;
		::HandToHand((Handle*)&mPrintRecordH);	// weird toolbox call copies the param and overwrites the param to return!
	}
	else
		mPrintRecordH = 0;
}


void
oofRepMacEnv::drawPageBorder( long top, long left, long bottom, long right )
{
// client-specific hack!
// future will have way to attach border adorners to reports to specify same styles

	Rect borderInside;
	
	borderInside.left = left-1;
	borderInside.right = right+1;
	borderInside.top = top-1;
	borderInside.bottom = bottom+1;
	
	FrameRect( &borderInside );
	
	borderInside.left = borderInside.left - 3;
	borderInside.right = borderInside.right + 3;
	borderInside.top = borderInside.top - 3;
	borderInside.bottom = borderInside.bottom + 3;
	
	PenSize(2,2);
	FrameRect( &borderInside );
		
	borderInside.left = borderInside.left - 2;
	borderInside.right = borderInside.right + 2;
	borderInside.top = borderInside.top - 2;
	borderInside.bottom = borderInside.bottom + 2;
	
	PenSize(1,1);
	FrameRect( &borderInside );
}
		
// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	// as we descend through our bands making renderers, should mimic state of drawing
	oofRepViewBand_Render* theRender = new oofRepViewBand_MacRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_MacRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_MacRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepMacEnv& env)
{
	delete mRender;
	mRender = new oofRepLineBand_MacRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_MacRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_MacRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_MacRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepMacEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_MacRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _ M a c R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_MacRender((oofRepViewBand*)newOwner);	// safe downcast
}


void 
oofRepViewBand_MacRender::finishConstruction ()
{
	oofRepViewBand_Render::finishConstruction ();
	if (!mDrawnColHeadersThisPage) // indicates will be drawing them
		mColHeaders.initSizers(mOwner->view()->count());
}


/**
	reset mDrawnColHeadersThisPage in case we redraw a mac render once constructed.
	eg: if printing after generating a preview
*/
void
oofRepViewBand_MacRender::startTableBody(bool headersHaveChanged)
{
	if (oofRep::currentReport()->shouldDrawViewColumnHeaders()) {
		mDrawnColHeadersThisPage = false;
		if (headersHaveChanged)
			CalcColHeaderWrapping();
	}
	else
		mDrawnColHeadersThisPage = true;
}


/**
	Calculate wrapped widths with Mac font rendering for all column headers.
	\todo more style choice, don't just set bold 
*/
void 
oofRepViewBand_MacRender::CalcColHeaderWrapping()
{
	dbView* view = mOwner->view();
	const unsigned short numFlds = view->count();

	::TextFace(bold); // Hack for now !
	view->start();
	for (unsigned short i=0; i<numFlds; i++) {
		mColHeaders.text(i) = view->getHeading(i);
		mColHeaders.CalculateWrapping(i,view->colSizer()->width(i));
	}
}


void 
oofRepViewBand_MacRender::draw()	// draws just current record
{
	oofRep* curRep = oofRep::currentReport();
	const oofRepSettings* currSettings = curRep->settings();
	const short colSepWidth = currSettings->colSepWidth();
	const unsigned short	bandIndenting = currSettings->bandIndenting();
	const unsigned long		bodyWidth = curRep->bodyWidth();
	const unsigned short	leftMargin = currSettings->leftMargin();
	oofRepTextStyle*		style = mOwner->textStyle();
	
	curRep->changeTextStyle(style);
	FontInfo fi;
	::GetFontInfo(&fi);	
	const short totalDescent = fi.descent + fi.leading + style->leading();
	const unsigned short lineHeight = fi.ascent + totalDescent;

	dbView* view = mOwner->view();

	const unsigned numFlds=view->count();
	oofSizedTextArray	sizedBody(numFlds);
	
	// Setup for adorners
	const unsigned long adornCount = mOwner->adorners().count();
	unsigned long cellY;
	if (adornCount) {
		// Calculate the current record number we're drawing
		cellY = view->recordNumber();
	}
	else
		cellY = 0;
	
	unsigned int i;
	
	// Get the field text and calculate the wrapping and height
	view->start();
	bool hasCustomDrawers=false;
	unsigned long maxCustomDrawerHeight = 0;
	for (unsigned int i=0; i<numFlds; i++) {	
		oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(i);
		if (theDrawer) {
			hasCustomDrawers = true;
			unsigned long wrappedHeight;
			theDrawer->calculateWrapping(&view->field(i), view->colSizer()->width(i), wrappedHeight);
			if (wrappedHeight>maxCustomDrawerHeight)
				maxCustomDrawerHeight = wrappedHeight;
			curRep->changeTextStyle(style);  // in case custom drawer changed
		}
		else {
			sizedBody.text(i) = view->field(i).copyString();
			sizedBody.CalculateWrapping(i,view->colSizer()->width(i));
		}
	}
	unsigned short colHeaderHeight = 0;
	if (!mDrawnColHeadersThisPage)
		colHeaderHeight = mColHeaders.maxRows()*lineHeight;	// add in col headers when deciding if room on page
		
	unsigned short rowDrawHeight = sizedBody.maxRows()*lineHeight;
	if (maxCustomDrawerHeight>rowDrawHeight)
		rowDrawHeight = maxCustomDrawerHeight;
	const unsigned short DrawHeight = rowDrawHeight + colHeaderHeight;
	
	// Render the Fields
	unsigned long xPos=leftMargin+bandIndenting;
	
	if((DrawHeight+curRep->currentVerticalPos())>curRep->bodyEnd()) {
		// We don't fit, and we'll need to cope with this !
		DrawContinuingOverPage(0,DrawHeight,sizedBody,fi, hasCustomDrawers);
	} else {	
		if (adornCount) {
			xPos+=colSepWidth/2;
		}
		if (!mDrawnColHeadersThisPage) {
			mOwner->drawColumnHeaders();
// restore the style - headers may have changed
			curRep->changeTextStyle(style);
		}		
		
		for(i=0;i<numFlds;i++) {
			oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
			// Cope with natural alignment
			if(alignment==oofColSizer::alignNatural)
				alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());
				
			int ybase=curRep->currentVerticalPos()-totalDescent;
			
			// Adorner information for the cell
			unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth ;
			unsigned long	cellHeight = rowDrawHeight;
			int				cellHorizontalPos = xPos-(colSepWidth/2);
			int				cellVerticalPos = curRep->currentVerticalPos()-1;
			
			if( i==numFlds-1 ) {		// resize the last cell to right edge
				cellWidth = bodyWidth - bandIndenting + leftMargin + (colSepWidth/2) - xPos;
			}
			
			if (adornCount) {
				// call before adorners for cell
				mOwner->adorners().start();
				for( long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
						::MoveTo(cellHorizontalPos, cellVerticalPos);
						currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth );
					}
					mOwner->adorners().next();
				}
			}
			
			// draw the contents of the cell
			oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(i);
			if (theDrawer) {
				theDrawer->draw(xPos, ybase);
				curRep->changeTextStyle(style);  // in case custom drawer changed
			}
			else {  // draw normal text
				short numrows=sizedBody.GetNumRows(i);
				const unsigned long* lines = sizedBody.GetLineStarts(i);
				const unsigned long* lengths = sizedBody.GetLineLengths(i);
				short bytes;
				for(unsigned short rownum=0;rownum<numrows;rownum++){
					if (lengths[rownum]) {
						unsigned long drawPos;
						switch(alignment){
							case (oofColSizer::alignRight):{
										drawPos=xPos+view->colSizer()->width(i)-lengths[rownum];
								 	 }
								 	 break;
							case (oofColSizer::alignCentre):{
										drawPos=xPos+((view->colSizer()->width(i)-lengths[rownum])/2);
								 	 }
								 	 break;
							default: drawPos=xPos;
									 break;
						}
						::MoveTo(drawPos, ybase+(rownum+1)*lineHeight);
						if(rownum==(numrows-1))		
							bytes=sizedBody.text(i).length()-lines[rownum];
						else
							bytes=lines[rownum+1]-lines[rownum];

						const char* mainText = sizedBody.text(i);
						const char* drawText = mainText + lines[rownum];
						while ((bytes>0) && (
							(drawText[bytes-1]==0x0A) || (drawText[bytes-1]==0x0D)
						))  // skip trailing linefeeds or CR's
							--bytes;
						::DrawText(drawText,0,bytes);
					} // if any length in this row
				}  // loop through rows of report
			}  // custom or default text draw
			
			if (adornCount) {
				// call after adorners for cell
				mOwner->adorners().start();
				for( long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
						::MoveTo(cellHorizontalPos, cellVerticalPos);
						currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth );
					}
					mOwner->adorners().next();
				}
			}
			xPos+=view->colSizer()->width(i)+colSepWidth;
		}
		curRep->currentVerticalPos() += rowDrawHeight;
	}
}


void
oofRepViewBand_MacRender::DrawContinuingOverPage(unsigned long startRow, unsigned long heightLeft,
									  oofSizedTextArray& sizedBody, FontInfo &fi, bool hasCustomDrawers)
{
// NOTE heightLeft will include the height of the column headers if they are still to be printed

	dbView* view = mOwner->view();
	//bool isBoxed = mOwner->isBoxed();
	const unsigned numFlds=view->count();
	oofRep* curRep = oofRep::currentReport();
	const oofRepSettings* currSettings = curRep->settings();
	const short colSepWidth = currSettings->colSepWidth();
	const unsigned short	bandIndenting = currSettings->bandIndenting();
	const unsigned long		bodyWidth = curRep->bodyWidth();
	const unsigned short	leftMargin = currSettings->leftMargin();
	oofRepTextStyle*		style = mOwner->textStyle();
	
	const unsigned long adornCount = mOwner->adorners().count();
	unsigned long cellY;
	
	if (adornCount) {
		// Calculate the current record number we're drawing
		cellY = view->recordNumber();
	}
	else
		cellY = 0;
	
	unsigned long xPos=leftMargin+bandIndenting;
	bool lastpage=false;
	unsigned long DrawHeight, rowDrawHeight=0;
	
// draw whatever fits on the page, or the remaining portion of the record
	unsigned long roomLeftOnPage = curRep->bodyEnd() - curRep->currentVerticalPos();
	if (roomLeftOnPage<0)
		roomLeftOnPage = 0;	// even one line won't fit, so force to next page
		
	if (heightLeft > roomLeftOnPage)
		DrawHeight = roomLeftOnPage;	// WARNING may be less than one line room left!!!
	else {
		DrawHeight=heightLeft;
		lastpage=true;
	}
	
	const long totalDescent = fi.descent + fi.leading + style->leading();
	const unsigned long lineHeight = fi.ascent + totalDescent;
	
	unsigned long possiblerows;
	if (!mDrawnColHeadersThisPage) {
		const unsigned long colHeaderHeight = mColHeaders.maxRows()*lineHeight;
// colHeaderHeight will only be non-zero if we are drawing headers and haven't yet drawn them
		rowDrawHeight = DrawHeight-colHeaderHeight;
		if (DrawHeight > colHeaderHeight) 
			possiblerows = rowDrawHeight/lineHeight;	// must be truncation divide - don't draw half rows
		else
			possiblerows = 0;
	}
	else {
		possiblerows=DrawHeight/lineHeight;	// could be zero rows if a single line too tall
		rowDrawHeight = DrawHeight;
	}

	unsigned long maxCustomDrawerHeight = 0;  // largest chunk drawn by a custom drawer 
	// helps define total height drawn if a custom drawer is a few pixels higher than adjacent rows of uniform text
	unsigned long maxCustomDrawerHeightLeft = 0;  // our total left - allows for custom drawers to take more room
	// drawing in chunks than in the original estimate of a single chunk, so our incoming heightLeft may be too low
	// if we have a page break this page - see the correction before recursing below
	
	if (hasCustomDrawers) {
	// repeat a loop to calc height each page because we don't assume that N rows are
	// consumed, unlike the plain text, as we know NOTHING about what is drawn and
	// a custom drawer is free to consume more height over multiple pages than it
	// said its unbroken total height would be
		for (unsigned int i=0; i<numFlds; i++) {	
			oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(i);
			if (theDrawer) {
				unsigned long drawnHeight;
				const unsigned long heightLeftToDraw = theDrawer->calcNextDrawHeight(rowDrawHeight, drawnHeight);
				if (drawnHeight > maxCustomDrawerHeight)
					maxCustomDrawerHeight = drawnHeight;
				assert(drawnHeight<=rowDrawHeight);
				if (heightLeftToDraw>0) {
					lastpage=false;  // correction to calc above, in case the custom draw broke in a way
					// that changed the remaining height we've been passing through. We might have thought
					// this is the last page because other columns fit, but this could correct that.
					if (heightLeftToDraw > maxCustomDrawerHeightLeft)
						maxCustomDrawerHeightLeft = heightLeftToDraw;
				}  // height to draw
				curRep->changeTextStyle(style);  // in case custom drawer changed
			}  // custom drawer
		} // loop fields to see if have custom draw
	}  // if has custom draw

	unsigned short rowsDrawn;
	if(lastpage || possiblerows>1){		// don't allow a widow left on page if more than one row to draw
		
		if (adornCount) {
			xPos+=colSepWidth/2;
		}
		
		// we know we have made room for the col headers if not drawn on this page
		if (!mDrawnColHeadersThisPage) {
			mOwner->drawColumnHeaders();
// restore the style - headers may have changed
			curRep->changeTextStyle(style);
		}		

		for(unsigned i=0;i<numFlds;i++){
			oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
			// Cope with natural alignment
			if(alignment==oofColSizer::alignNatural)
				alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());

			int ybase=curRep->currentVerticalPos()-totalDescent;
			
			// Adorner information for the cell
			unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth;
			unsigned long	cellHeight = rowDrawHeight;
			int				cellHorizontalPos = xPos-(colSepWidth/2);
			int				cellVerticalPos = curRep->currentVerticalPos()-1;
			
			if( i==numFlds-1 ) {		// resize the last cell to right edge
				cellWidth = curRep->bodyWidth() - currSettings->bandIndenting() +
							currSettings->leftMargin() + (colSepWidth/2) - xPos;
			}
			
			if (adornCount) {
				// call before adorners for cell
				mOwner->adorners().start();
				for( long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
						::MoveTo(cellHorizontalPos, cellVerticalPos);
						// draw the correct style for draw overrun cells
						if( lastpage ) {
							if( startRow ) {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::continueStop );
							} else {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::startStop );
							}
						} else {
							if( !startRow ) {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::startContinue );
							} else {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::continueContinue );
							}
						}
					}
					mOwner->adorners().next();
				}
			}
			
			oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(i);
			if (theDrawer) {
				theDrawer->draw(xPos, ybase);  // draws fragment calculated by calcNextDrawHeight() above
				curRep->changeTextStyle(style);  // in case custom drawer changed style
			}
			else {
				long numRowsThisPage=sizedBody.GetNumRows(i)-startRow;
				if(numRowsThisPage<0) // This field might not have any more rows to draw
					numRowsThisPage=0;
				if(numRowsThisPage>possiblerows) 
					numRowsThisPage=possiblerows;
				const unsigned long* lines = sizedBody.GetLineStarts(i);
				const unsigned long* lengths = sizedBody.GetLineLengths(i);
				short bytes;
				// OK - for clarity:
				// * rownum is always 0-based here, but is offset by startRow from the actual textSizer row
				// * numRowsThisPage is the number of rows we can fit on this page
				// So we always use these values unmodified for calculating position, BUT
				// we use them offset for any calculations on the sizer info...
				for(unsigned short rownum=0;rownum<numRowsThisPage;rownum++){
					if (lengths[rownum]) {
						unsigned long drawPos;
						switch(alignment){
							case (oofColSizer::alignRight):{
										drawPos=xPos+view->colSizer()->width(i)-lengths[rownum];
								 	 }
								 	 break;
							case (oofColSizer::alignCentre):{
										drawPos=xPos+((view->colSizer()->width(i)-lengths[rownum])/2);
								 	 }
								 	 break;
							default: drawPos=xPos;
									 break;
						}
						::MoveTo(drawPos, ybase+(rownum+1)*lineHeight);
						if(rownum+startRow==(sizedBody.GetNumRows(i)-1))		
								bytes=sizedBody.text(i).length()-lines[rownum+startRow];
						else
							bytes=lines[rownum+startRow+1]-lines[rownum+startRow];

						const char* mainText = sizedBody.text(i);
						const char* drawText = mainText + lines[rownum+startRow];
						while ((bytes>0) && (
							(drawText[bytes-1]==0x0A) || (drawText[bytes-1]==0x0D)
						))  // skip trailing linefeeds or CR's
							--bytes;
						::DrawText(drawText,0,bytes);
					} // if any length in this row
				}  // loop through rows of report
			} // draw cu
			
			if (adornCount) {
				// call after adorners for cell
				mOwner->adorners().start();
				for( long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
						::MoveTo(cellHorizontalPos, cellVerticalPos);
						// draw the correct style for draw overrun cells
						if( lastpage ) {
							if( startRow ) {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::continueStop );
							} else {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::startStop );
							}
						} else {
							if( !startRow ) {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::startContinue );
							} else {
								currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth, oofAdorner::continueContinue );
							}
						}
					}
					mOwner->adorners().next();
				}
			}
			xPos+=view->colSizer()->width(i)+colSepWidth;
		}
		rowsDrawn = possiblerows;
		unsigned long heightDrawn = rowsDrawn*lineHeight;
		if (maxCustomDrawerHeight > heightDrawn)
			heightDrawn = maxCustomDrawerHeight;  // may have custom draw go a few pixels past the rows
		assert (heightDrawn <= DrawHeight);	// difference is fractional row that didn't fit
		heightLeft -= heightDrawn;	
		if (maxCustomDrawerHeightLeft > heightLeft)
			heightLeft = maxCustomDrawerHeightLeft;  // may have custom draw extend the earlier total
	} // fitted at least 2 rows
	else {
// heightLeft stays the same as on entry - ie: we've called overrun from draw because row didn't fit entirely on original page
		rowsDrawn = 0;			
	}
	
	if(!lastpage){
		oofRepEnvironment* theEnv = curRep->environment();
		theEnv->splitBandOverPage();
		curRep->drawPageBreak();

		// unconditionally draw headers - we KNOW we haven't drawn them 
		theEnv->resumeBandOverPage();
		mOwner->drawColumnHeaders();  // NOT YET IMPLEMENTED - setting to say that we don't draw these on subsequent pages
		curRep->changeTextStyle(style);  // In case the page break or col headers changed styles
		DrawContinuingOverPage(startRow+rowsDrawn, heightLeft, sizedBody, fi, hasCustomDrawers);
	} 
	else
		curRep->currentVerticalPos() += DrawHeight;
}


void 
oofRepViewBand_MacRender::drawColumnHeaders()
{
	oofRep* curRep = oofRep::currentReport();
	const oofRepSettings* currSettings = curRep->settings();
	const short colSepWidth = currSettings->colSepWidth();
	
	mDrawnColHeadersThisPage = true;	// not conditional! we want to avoid calling this method again this page
	if (!curRep->shouldDrawViewColumnHeaders())
		return;
	
	dbView* view = mOwner->view();
	const unsigned short numFlds = view->count();
	//const bool isBoxed = mOwner->isBoxed();
	
	curRep->changeTextStyle(mOwner->textStyle());
	TextFace(bold); // Hack for now !
	FontInfo fi;
	::GetFontInfo(&fi);	
	const short totalDescent = fi.descent + fi.leading + mOwner->textStyle()->leading();
	const unsigned short lineHeight = fi.ascent + totalDescent;
	const short DrawHeight = mColHeaders.maxRows()*lineHeight;
	
	// Render the Headers
	unsigned long xPos=currSettings->leftMargin()+currSettings->bandIndenting();
	
	const unsigned long adornCount = mOwner->adorners().count();
	unsigned long cellY = 0;
	
	// not necessary as headers can't be the last cell
	//if (adornCount) {
	//	// Calculate the current record number we're drawing

	if( adornCount ) {
		xPos+=colSepWidth/2;
	}

	for(unsigned short i=0;i<numFlds;i++){
		const unsigned short numrows = mColHeaders.GetNumRows(i);
		const unsigned long* lines = mColHeaders.GetLineStarts(i);
		const unsigned long* lengths = mColHeaders.GetLineLengths(i);
		short bytes;
		oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
		// Cope with natural alignment
		if(alignment==oofColSizer::alignNatural)
			alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());

		const int ybase=curRep->currentVerticalPos() - totalDescent;
		
		// Adorner information for the cell
		unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth ;
		unsigned long	cellHeight = DrawHeight;
		int				cellHorizontalPos = xPos-(colSepWidth/2);
		int				cellVerticalPos = curRep->currentVerticalPos()-1;
		
		if( i==numFlds-1 ) {		// resize the last cell to right edge
			cellWidth = curRep->bodyWidth() - currSettings->bandIndenting() +
						currSettings->leftMargin() + (colSepWidth/2) - xPos;
		}
		
		if (adornCount) {
			// call before adorners for cell
			mOwner->adorners().start();
			for( long ad=0; ad<adornCount; ad++ ) {
				oofAdorner* currentAdorner = mOwner->adorners().current();
				if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
					::MoveTo(cellHorizontalPos, cellVerticalPos);
					currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth );
				}
				mOwner->adorners().next();
			}
		}

		for(unsigned short rownum=0;rownum<numrows;rownum++){
			unsigned long drawPos;
			switch(alignment){
				case (oofColSizer::alignRight):{
							drawPos=xPos+view->colSizer()->width(i)-lengths[rownum];
					 	 }
					 	 break;
				case (oofColSizer::alignCentre):{
							drawPos=xPos+((view->colSizer()->width(i)-lengths[rownum])/2);
					 	 }
					 	 break;
				default: drawPos=xPos;
						 break;
			}
			::MoveTo(drawPos, ybase+(rownum+1)*lineHeight);
			const OOF_String& text = mColHeaders.text(i);
			if(rownum==(numrows-1))		
				bytes=text.length()-lines[rownum];
			else
				bytes=lines[rownum+1]-lines[rownum];
			::DrawText(text,lines[rownum],bytes);
		}
/*
original underlining of headers
		::MoveTo(xPos,ybase+numrows*lineHeight+1);
		::PenSize(1,1);
		::Line(view->colSizer()->width(i),0); // Rule under the headings
*/		
		if (adornCount) {
			// call after adorners for cell
			mOwner->adorners().start();
			for( long ad=0; ad<adornCount; ad++ ) {
				oofAdorner* currentAdorner = mOwner->adorners().current();
				if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
					::MoveTo(cellHorizontalPos, cellVerticalPos);
					currentAdorner->drawcell( i, cellY, mOwner, cellHeight, cellWidth );
				}
				mOwner->adorners().next();
			}
		}

		xPos+=view->colSizer()->width(i)+colSepWidth;
		//if((isBoxed)&&(i<numFlds-1)){
		//	MoveTo(xPos-(colSepWidth/2), curRep->currentVerticalPos());
		//	Line(0,DrawHeight);
		//}
	}
	curRep->currentVerticalPos() += DrawHeight;
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _ M a c R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_MacRender((oofRepLayoutBand*)newOwner);	// safe downcast
}


void 
oofRepLayoutBand_MacRender::draw()
{
	Point penLoc;
	
	oofRepBlockList& blocks = mOwner->blocks();
	blocks.start();
	// Move to the initial position below which blocks are aligned
	::MoveTo(	oofRep::currentReport()->settings()->leftMargin()
			+oofRep::currentReport()->settings()->bandIndenting(),
		   oofRep::currentReport()->currentVerticalPos()); 
	while(blocks.more()) {
		oofRepBlock* theBlock = blocks();
		// KENTODO - Handle other alignments !
		// BOLD ASSUMPTION - only one right aligned item per line.
		if(theBlock->align()==oofRepBlock::alignRight){
			::GetPen(&penLoc);
 
			::Move(-penLoc.h
				 +oofRep::currentReport()->bodyWidth()
				 +oofRep::currentReport()->settings()->leftMargin()
				 -oofRep::currentReport()->settings()->bandIndenting()
				 -theBlock->width(),0);
		}		 
		if(theBlock->align()==oofRepBlock::alignCentre) {
			::GetPen(&penLoc);
			const long newX = -penLoc.h+oofRep::currentReport()->settings()->leftMargin() + (oofRep::currentReport()->bodyWidth()-theBlock->width())/2;
			::Move(newX,0);
		}
		::GetPen(&penLoc);
		SetCurrentBlockCoords(penLoc.h, penLoc.v, theBlock);  // save for callbacks
		theBlock->draw();
		if (theBlock->fixedWidth())
			::Move(theBlock->fixedWidth() - theBlock->width(), 0);	// move horizontally
		if (theBlock->moveDownAfter()){
			::GetPen(&penLoc);
			Move(-penLoc.h
				 +oofRep::currentReport()->settings()->leftMargin()
				 +oofRep::currentReport()->settings()->bandIndenting(),
				 theBlock->fixedHeight());
			// Terminate this line
			blocks.next();
/*
			if(blocks.more()){
				oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
				::GetFontInfo(&fi);	
				::Move(0,fi.ascent);
			}
*/
		} else
			blocks.next();
	}

	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}

// -------------------------------------------------------
//    o o f R e p L i n e B a n d _ M a c R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_MacRender((oofRepLineBand*)newOwner);	// safe downcast
}

void 
oofRepLineBand_MacRender::draw()
{
	unsigned long fixedHeight = mOwner->fixedHeight();
	Rect theLine={oofRep::currentReport()->currentVerticalPos(),
				  oofRep::currentReport()->settings()->leftMargin()+oofRep::currentReport()->settings()->bandIndenting(),
				  oofRep::currentReport()->currentVerticalPos()+fixedHeight,
				  oofRep::currentReport()->bodyWidth()+oofRep::currentReport()->settings()->leftMargin()-oofRep::currentReport()->settings()->bandIndenting()};

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Pattern drawPat;
	if( mOwner->grey() ){
		
		FillRect(&theLine,::GetQDGlobalsGray(&drawPat));
		PenPat(::GetQDGlobalsGray(&drawPat));
	}
	else{
		::FillRect(&theLine,::GetQDGlobalsBlack(&drawPat));
		::PenPat(::GetQDGlobalsBlack(&drawPat));
	}
	FrameRect(&theLine);
	::PenPat(GetQDGlobalsBlack(&drawPat));
#else
	if( mOwner->grey() ){
		::FillRect(&theLine,&qd.gray);
		::PenPat(&qd.gray);
	} else {
		::FillRect(&theLine,&qd.black);
		::PenPat(&qd.black);
	}
	::PenPat(&qd.black);
#endif	
	oofRep::currentReport()->currentVerticalPos() += fixedHeight;
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _ M a c R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_MacRender((oofRepPictBand*)newOwner);	// safe downcast
}



void 
oofRepPictBand_MacRender::draw()
{
	unsigned long	fixedHeight = mOwner->fixedHeight();

	const unsigned short leftMargin = oofRep::currentReport()->settings()->leftMargin();
	const unsigned short topBand = oofRep::currentReport()->currentVerticalPos();
	oofPict* thePict = mOwner->picture();
	if (thePict) {
		thePict->draw(
			leftMargin,
			topBand,
			leftMargin + oofRep::currentReport()->bodyWidth(),
			topBand+fixedHeight
		);
	}
	oofRep::currentReport()->currentVerticalPos() += fixedHeight;		
}


// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _ M a c R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_MacRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_MacRender::draw()
{
	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ M a c R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_MacRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_MacRender::draw()
{
	oofRep* currentReport = oofRep::currentReport();
	bool printMono = currentReport->environment()->isPrintingEnvironment() && currentReport->settings()->printGraphsInMono();
	if (printMono)
		mOwner->graph()->setStyleToMono();

	currentReport->changeTextStyle(mOwner->textStyle());
	
	const unsigned long drawingHeight = mOwner->drawingHeight();	
	const unsigned long bodyWidth = currentReport->bodyWidth();
	const unsigned long graphWidth = mOwner->graphWidth();
	const unsigned long graphHeight = mOwner->graphHeight();
	
	assert(mOwner->graphWidth()>0);
	
	oofRect theFrame;
	
	unsigned long graphLeft, graphRight, graphBottom, graphTop;
	if (bodyWidth > graphWidth) {
		graphLeft = (bodyWidth-graphWidth)/2;
		graphRight = graphWidth+graphLeft;
	}
	else {
		graphLeft = 0;
		graphRight = bodyWidth;
	}
	if (drawingHeight > graphHeight) {
	  	graphTop = (drawingHeight-graphHeight)/2;
		graphBottom = graphTop+graphHeight;
	}
	else {
		graphTop = 0;
		graphBottom = drawingHeight;
	}
	theFrame.set(graphLeft,graphTop,graphRight ,graphBottom);
	
	theFrame.offset(currentReport->settings()->leftMargin(),
		currentReport->currentVerticalPos());
	
	mOwner->graph()->setPaneRect(theFrame);

	#ifdef OOF_DEBUG_GRAPHS 
		ofstream logFile(mOwner->graph()->getTitle(),ios::out);
		mOwner->graph()->describe(logFile);	
		logFile.close;
	#endif
		
	//end logging stuff
	mOwner->graph()->draw();

	currentReport->currentVerticalPos() += drawingHeight;

	if (printMono)
		mOwner->graph()->setStyleToColor();
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ M a c P r i n t R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_MacPrintRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_MacPrintRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_MacPrintRender::draw()
{
	bool printMono = oofRep::currentReport()->settings()->printGraphsInMono();
	if (printMono)
		mOwner->graph()->setStyleToMono();
	oofRepGraphBand_MacRender::draw();
	if (printMono)
		mOwner->graph()->setStyleToColor();
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _ M a c R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_MacRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_MacRender::draw()
{
// draw below the current position, so temporarily duck down to Mac font baseline
// then return
	oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
	FontInfo fi;
	::GetFontInfo(&fi);	
	::Move(0,fi.ascent);
	const OOF_String& text = mOwner->text();
	::DrawText(text,0,text.length());
	::Move(0,-fi.ascent);
}


unsigned long
oofRepTextBlock_MacRender::width() const
{
	oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
	const OOF_String& text = mOwner->text();
	unsigned long ret = ::TextWidth(text,0,text.length());
	return ret;
}

// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _ M a c R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_MacRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_MacRender((oofRepPictBlock*)newOwner);	// safe downcast
}

void
oofRepPictBlock_MacRender::draw()
{
// draw below the current location, so align with the top of any text on this line
	oofPict*	thePict = mOwner->picture();
	const long pictWidth = mOwner->width();
	// unlike Windows, Mac draw is called as we actually draw so gets coords from the port
	Point penLoc;
	::GetPen(&penLoc);
	const long x = penLoc.h;
	const long right = x + pictWidth;
	const long y = penLoc.v;
	const long bottom = y + mOwner->fixedHeight();
	if (thePict) {
		thePict->draw(x, y, right, bottom);
	}
	::Move(pictWidth, 0);  // move horizontally width of the picture
}


unsigned long
oofRepPictBlock_MacRender::width() const
{
	return mOwner->width();
}

