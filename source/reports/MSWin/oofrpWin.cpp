// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories for Macintosh


#include "oofpch_r.h"  // for precompilation of report files

#ifndef _INC_WINDOWSX
	#include "windowsx.h"
#endif

// USE MAC DUMMIES - WE ARE RUNNING WINDOWS

// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepSpaceBand::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepGraphBand::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepLineBand::makeRender(const oofRepMacEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepPictBand::makeRender(const oofRepMacEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepLayoutBand::makeRender(const oofRepMacEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepTextBlock::makeRender(const oofRepMacEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofRepPictBlock::makeRender(const oofRepMacEnv&) 
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}



// -------------------------------------------------------
//            o o f R e p W i n E n v
// -------------------------------------------------------
void oofRepWinEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepHTMLEnv&.
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

void oofRepWinEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}

oofRepRenderT oofRepWinEnv::renderType() const
{
	return eRenderType;
}

unsigned short
oofRepWinEnv::heightOfTextStyle(oofRepTextStyle* theStyle)
{
	if (theStyle != NULL)
	{
		HDC ic = oofCurrentDraw::getInfoDC();
		assert(ic != NULL);

		LOGFONT lf = theStyle->getLogFont();
//		const int pixelsY = ::GetDeviceCaps(ic, LOGPIXELSY);
		lf.lfHeight = -theStyle->fontSize();  // -MulDiv(theStyle->fontSize(), pixelsY, 72);
		HFONT hFont = oofTextDrawer::createFontIndirect(&lf);
		assert(hFont != NULL);
		HFONT hOldFont =  (HFONT)::SelectObject(ic, hFont);
		assert(hOldFont);

		TEXTMETRIC tm;
		memset(&tm, 0, sizeof(TEXTMETRIC));
		::GetTextMetrics(ic, &tm);

		::SelectObject(ic, hOldFont);
		::DeleteObject(hFont);

		return (unsigned short)tm.tmHeight;
	}
	else
		return 10;
}


SIZE 
oofRepWinEnv::getTextExtent(const oofRepTextStyle* textStyle, const oofString& text)
{
	HDC ic = oofCurrentDraw::getInfoDC();
	assert(ic != NULL);

	LOGFONT lf = textStyle->getLogFont();
	lf.lfHeight = -textStyle->fontSize();  // -MulDiv(textStyle->fontSize(), ::GetDeviceCaps(ic, LOGPIXELSY), 72);

	HFONT hFont = oofTextDrawer::createFontIndirect(&lf);
	HFONT hOldFont = (HFONT)::SelectObject(ic, hFont);

	SIZE size;
// doesn't work with win32s		::GetTextExtentPoint32(ic, text, text.length(), &size);
	const unsigned long textLen =  text.length();
	if (textLen==0)
		size.cx = 0;
	else {
		DWORD retSize = ::GetTabbedTextExtent(ic, text, textLen, 0, 0  /* no tab positions */);
		if (retSize==0) {
			DWORD lastErr = ::GetLastError();  // so we can check in debugger
			assert(!"Error in oofRepWinEnv::getTextExtent() getting text size");
		}
		else 
			size.cx = LOWORD(retSize);  // assume don't care about cy - NOT YET IMPLEMENTED rewrite this call to return X value alone
	}
	::SelectObject(ic, hOldFont);
	::DeleteObject(hFont);

	return size;
}


void 
oofRepWinEnv::startEnvironment()
{
	oofCurrentDraw::resetCoordinates();
}


// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepSpaceBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepSpaceBand_WinRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepGraphBand_WinRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepLineBand_WinRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepPictBand_WinRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepTextBlock_WinRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepPictBlock_WinRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;
	mRender = new oofRepLayoutBand_WinRender(this);
}


void	
oofRepViewBand::makeRender(const oofRepWinEnv& env)
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_WinRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


// -------------------------------------------------------
//    o o f R e p V i e w B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_WinRender((oofRepViewBand*)newOwner);	// safe downcast
}


void 
oofRepViewBand_WinRender::finishConstruction ()
{
	oofRepViewBand_Render::finishConstruction ();
	if (!mDrawnColHeadersThisPage) // indicates will be drawing them
		mColHeaders.initSizers(mOwner->view()->count());
}


void
oofRepViewBand_WinRender::startTableBody(bool headersHaveChanged)
{
// reset here in case we redraw a Windows render once constructed,
// eg: if printing after generating a preview
	if (oofRep::currentReport()->shouldDrawViewColumnHeaders()) {
		mDrawnColHeadersThisPage = false;
		if (headersHaveChanged)
			CalcColHeaderWrapping();
	}
	else
		mDrawnColHeadersThisPage = true;
}


void 
oofRepViewBand_WinRender::CalcColHeaderWrapping()
{
	dbView* view = mOwner->view();
	const unsigned short numFlds = view->count();

// HACK - force column headers to bold, NOT YET IMPLEMENTED - need a global setting for this
	oofRepTextStyle* OS = mOwner->textStyle();
	oofRepTextStyle  hackStyle(OS->fontName(), OS->fontSize(), bold);
	oofRep::currentReport()->changeTextStyle(&hackStyle);

	view->start();
	for (unsigned short i=0; i<numFlds; i++) {
		mColHeaders.text(i) = view->getHeading(i);
		mColHeaders.CalculateWrapping(i,view->colSizer()->width(i));
	}
}


void 
oofRepViewBand_WinRender::drawColumnHeaders()
{
	oofRep* 				curRep = oofRep::currentReport();
	const oofRepSettings*	currSettings = curRep->settings();
	const unsigned short	colSepWidth = currSettings->colSepWidth();
//AD981030	const unsigned short	bandIndenting = currSettings->bandIndenting();
//AD981030	const unsigned long		bodyWidth = curRep->bodyWidth();
//AD981030	const unsigned short	leftMargin = currSettings->leftMargin();
	
	mDrawnColHeadersThisPage = true;	// not conditional! we want to avoid calling this method again this page
	if (!curRep->shouldDrawViewColumnHeaders())
		return;

	dbView* view = mOwner->view();
	const unsigned numFlds = view->count();

// HACK - force column headers to bold, NOT YET IMPLEMENTED - need a global setting for this
		oofRepTextStyle* OS = mOwner->textStyle();
		oofRepTextStyle  hackStyle(OS->fontName(), OS->fontSize(), bold);
		oofRepTextStyle* style = &hackStyle;		// changes all code below that used to refer to owner's style

		curRep->changeTextStyle(style);
	const unsigned short lineHeight = style->fixedHeight()+style->leading();
	const short DrawHeight = mColHeaders.maxRows()*lineHeight;
		HDC		hdc = oofCurrentDraw::getCurrentDC();
	
	// Render the Headers
	unsigned long xPos=currSettings->leftMargin()+currSettings->bandIndenting();

	const unsigned long adornCount = mOwner->adorners().count();
	unsigned long cellY = 0;

	if( adornCount ) 
		xPos += colSepWidth/2;
		
	for(unsigned int i=0;i<numFlds;i++){
		unsigned short numrows=mColHeaders.GetNumRows(i);
		const unsigned long* lines = mColHeaders.GetLineStarts(i);
		const unsigned long* lengths = mColHeaders.GetLineLengths(i);
		short bytes;
		oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
		// Cope with natural alignment
		if(alignment==oofColSizer::alignNatural)
			alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());

		const int ybase=curRep->currentVerticalPos()-style->leading();
			
			// Adorner information for the cell
			unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth;
			unsigned long	cellHeight = DrawHeight;
			int				cellHorizontalPos = xPos - (colSepWidth/2);
			int				cellVerticalPos = curRep->currentVerticalPos();
	
		if( i==numFlds-1 ) {		// resize the last cell to right edge
			cellWidth = curRep->bodyWidth() - currSettings->bandIndenting() +
						currSettings->leftMargin() + (colSepWidth/2) - xPos;
		}

		if( adornCount ) {
			// call before adorners for cell
			mOwner->adorners().start();
			for( unsigned long ad=0; ad<adornCount; ad++ ) {
				oofAdorner* currentAdorner = mOwner->adorners().current();
				if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
					oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
					currentAdorner->drawcell(i,cellY,mOwner,cellHeight,cellWidth);
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
			const OOF_String& text = mColHeaders.text(i);
			if(rownum==(numrows-1))		
				bytes=text.length()-lines[rownum];
			else
				bytes=lines[rownum+1]-lines[rownum];
			if (bytes>0) {
				const int yPos = ybase+rownum*lineHeight;
				oofWinTextDE::recordElement( drawPos, yPos, text.chars() + lines[rownum], bytes);  
//981015					oofTextDrawer::textOut(oofCurrentDraw::getCurrentDC(), drawPos, yPos, Text[i]+lines[rownum],bytes);
				}
			}
	/*		::MoveTo(xPos,ybase+numrows*(fi.ascent+fi.descent+fi.leading+textStyle()->leading())+1);
			::PenSize(1,1);
			::Line(mview->colSizer()->width(i),0); // Rule under the headings
	*/		
			//xPos+=view->colSizer()->width(i)+curRep->settings()->colSepWidth();
			//if((isBoxed)&&(i<numflds-1)){
			//	::MoveToEx(
			//		oofCurrentDraw::getCurrentDC(),
			//		xPos-(curRep->settings()->colSepWidth()/2),
			//		curRep->currentVerticalPos(),
			//		NULL);
			//	::LineTo(hdc, 0, DrawHeight);
			//}
			
			if( adornCount ) {
				// call after adorners for cell
				mOwner->adorners().start();
				for( unsigned long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
						oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
						currentAdorner->drawcell(i,cellY,mOwner,cellHeight,cellWidth);
					}
					mOwner->adorners().next();
				}
			}
	
			xPos += view->colSizer()->width(i) + colSepWidth;
		}
		curRep->currentVerticalPos() += DrawHeight;
		
//AD983010	delete [] Text;
//AD983010	delete [] Sizer;
}


void
oofRepViewBand_WinRender::draw()
{
	HDC	hdc = oofCurrentDraw::getCurrentDC();
	oofRep*					curRep = oofRep::currentReport();
	const oofRepSettings*	curSettings = curRep->settings();
	const unsigned short	colSepWidth = curSettings->colSepWidth();
	const unsigned short	bandIndenting = curSettings->bandIndenting();
	const unsigned long		bodyWidth = curRep->bodyWidth();
	const unsigned short	leftMargin = curSettings->leftMargin();

	oofRepTextStyle* style = mOwner->textStyle();	
	
	oofRep::currentReport()->changeTextStyle(style);
//	int charHeight = style->calcCharHeight();
//	int charHeight = style->fixedHeight();
//	int descent	= style->calcDescent();
	const unsigned short lineHeight = style->fixedHeight()+style->leading();
	dbView* view = mOwner->view();

	const unsigned numFlds=view->count();
	oofSizedTextArray	sizedBody(numFlds);

	// Setup for adorners
	const unsigned long adornCount = mOwner->adorners().count();
	unsigned long cellY;
	if( adornCount ) {
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
	for (i=0; i<numFlds; i++) {	
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
	unsigned long xPos = leftMargin + bandIndenting;

	if( (DrawHeight + curRep->currentVerticalPos()) > curRep->bodyEnd() ) {
		// We don't fit, and we'll need to cope with this!
		DrawContinuingOverPage( 0, DrawHeight, sizedBody, hasCustomDrawers);
	} else {
		if( adornCount )  {
			xPos += colSepWidth/2;	
		}
		if( !mDrawnColHeadersThisPage ) {
			mOwner->drawColumnHeaders();
			// restore the style - col headers may have changed this
			curRep->changeTextStyle( mOwner->textStyle());
		}
		
		for(i=0;i<numFlds;i++){
			oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
			// Cope with natural alignment
			if(alignment==oofColSizer::alignNatural)
				alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());
	
			// Windows different vertical motion!
			int ybase=oofRep::currentReport()->currentVerticalPos() - style->leading();
	
			// Adorner information for the cell
			unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth;
			unsigned long	cellHeight = rowDrawHeight;
			int				cellHorizontalPos = xPos - (colSepWidth/2);
			int				cellVerticalPos = curRep->currentVerticalPos()-1;
	
			if( i==numFlds-1 ) {
				cellWidth = bodyWidth - bandIndenting + leftMargin + (colSepWidth/2) - xPos;
			}
	
			if( adornCount ) {
				// call before adorners for cell
				mOwner->adorners().start();
				for( unsigned long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
						oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
						currentAdorner->drawcell(i,cellY,mOwner,cellHeight,cellWidth);
					}
					mOwner->adorners().next();
				}
			}
			// draw the contents of the cell
			oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(i);
			if (theDrawer) {
				oofDE::appendElement(theDrawer->makeDrawDE(xPos, ybase));
				curRep->changeTextStyle(style);  // in case custom drawer changed style
			}
			else {  // draw normal text
				unsigned short numrows=sizedBody.GetNumRows(i);
				const unsigned long* lines = sizedBody.GetLineStarts(i);
				const unsigned long* lengths = sizedBody.GetLineLengths(i);
	
				unsigned short bytes;
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
						if(rownum==(numrows-1))	{	
	// all the bytes in the string minus the start of the last line
							bytes=sizedBody.text(i).length()-lines[rownum];				
						}
						else
							bytes=lines[rownum+1]-lines[rownum];
							
	// bit of a hack to avoid CRLF at end of line
					{
							const char* lastLine = lines[rownum] + sizedBody.text(i).chars();
							if (bytes>0) {
								unsigned long lastCharAt = bytes -1;
								char lastChar = lastLine[lastCharAt];
								while(lastCharAt >0 && (lastChar=='\r' || lastChar=='\n')) {
									lastCharAt--;
									lastChar = lastLine[lastCharAt];
								}
								bytes = lastCharAt+1;
							}
					}
						if (bytes) {
							const int yPos = ybase+rownum * lineHeight;
							oofWinTextDE::recordElement( drawPos, yPos, sizedBody.text(i).chars() + lines[rownum], bytes);  
	//981015						oofTextDrawer::textOut(hdc, drawPos, yPos, sizedBody.text(i)+ lines[rownum], bytes);
						}
					} // if any length in this row
				}  // loop through rows of report
			}  // custom draw vs normal text
	
			if( adornCount ) {
				// call after adorners for cell
				mOwner->adorners().start();
				for( unsigned long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
						oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
						currentAdorner->drawcell(i,cellY,mOwner,cellHeight,cellWidth);
					}
					mOwner->adorners().next();
				}
			}
	
			xPos += view->colSizer()->width(i) + colSepWidth;
		}
		oofRep::currentReport()->currentVerticalPos() += rowDrawHeight;
	}
}


void
oofRepViewBand_WinRender::DrawContinuingOverPage(unsigned long startRow, unsigned long heightLeft, oofSizedTextArray& sizedBody, bool hasCustomDrawers)
{
// NOTE heightLeft will include the height of the column headers if they are still to be printed

	HDC						hdc = oofCurrentDraw::getCurrentDC();
	dbView*					view = mOwner->view();
	const unsigned long		numFlds = view->count();
	oofRep*					curRep = oofRep::currentReport();
	const oofRepSettings*	curSettings = curRep->settings();
	const unsigned short	colSepWidth = curSettings->colSepWidth();
	const unsigned short	bandIndenting = curSettings->bandIndenting();
	const unsigned long		bodyWidth = curRep->bodyWidth();
	const unsigned short	leftMargin = curSettings->leftMargin();
	oofRepTextStyle*		style = mOwner->textStyle();

	const unsigned long		adornCount = mOwner->adorners().count();
	unsigned long 			cellY;

	if( adornCount ) {
		// Calculate the current record number we're drawing
		cellY = view->recordNumber();
	}
	else
		cellY = 0;
	
	unsigned long	xPos = leftMargin + bandIndenting;
	bool			lastpage = false;
	unsigned long	DrawHeight = 0, rowDrawHeight=0;
	
// draw whatever fits on the page, or the remaining portion of the record

	unsigned long 	roomLeftOnPage = curRep->bodyEnd() - curRep->currentVerticalPos();
	if( roomLeftOnPage < 0 )
		roomLeftOnPage = 0; 	// even one line won't fit, so force to next page

	if( heightLeft > roomLeftOnPage ) {
		DrawHeight = roomLeftOnPage;		// WARNING may be less than one line room left!!!
	} else {
		DrawHeight=heightLeft;
		lastpage=true;
	}
	
	curRep->changeTextStyle( style );
	const unsigned long		lineHeight = style->fixedHeight() + style->leading();
	
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
	if( lastpage || possiblerows > 1 ) { // don't allow a widow left on page if more than one row to draw
		
		if( adornCount ) {
			xPos+=colSepWidth/2;
		}
		
		// we know we have made room for the col headers if not drawn on this page
		if (!mDrawnColHeadersThisPage) {
			mOwner->drawColumnHeaders();
// restore the style - headers may have changed
			curRep->changeTextStyle(style);
		}		

		for( unsigned long i=0; i<numFlds; i++ ) {
			oofColSizer::colAlignT alignment = view->colSizer()->alignment(i);
			// Cope with natural alignment
			if(alignment==oofColSizer::alignNatural)
				alignment = oofColSizer::naturalAlignmentForField(view->field(i).fieldType());

			// Windows different vertical motion!
			int ybase=curRep->currentVerticalPos() - style->leading();
			
			// Adorner information for the cell
			unsigned long	cellWidth = view->colSizer()->width(i) + colSepWidth;
			unsigned long	cellHeight = rowDrawHeight;
			int				cellHorizontalPos = xPos-(colSepWidth/2);
			int				cellVerticalPos = curRep->currentVerticalPos()-1;
			
			if( i==numFlds-1 ) {		// resize the last cell to right edge
				cellWidth = curRep->bodyWidth() - curSettings->bandIndenting() +
							curSettings->leftMargin() + (colSepWidth/2) - xPos;
			}
			
			if (adornCount) {
				// call before adorners for cell
				mOwner->adorners().start();
				for( unsigned long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawBefore ) {
						oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
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
				oofDE::appendElement(theDrawer->makeDrawDE(xPos, ybase));
				curRep->changeTextStyle(style);  // in case custom drawer changed style
			}
			else {
				long rowsToEndField = sizedBody.GetNumRows(i) - startRow;
				long numRowsFldOnPage;
				if( rowsToEndField < 0 ) { // This field might not have any more rows to draw
					numRowsFldOnPage = 0;
					rowsToEndField = 0;
				}
				if( rowsToEndField > possiblerows ) 
					numRowsFldOnPage = possiblerows;
				else
					numRowsFldOnPage = rowsToEndField;
				const unsigned long* lines = sizedBody.GetLineStarts(i);
				const unsigned long* lengths = sizedBody.GetLineLengths(i);
				short bytes;
				// OK - for clarity:
				// * rownum is always 0-based here, but is offset by startRow from the actual textSizer row
				// * numRowsFldOnPage is the number of rows we can fit on this page
				// So we always use these values unmodified for calculating position, BUT
				// we use them offset for any calculations on the sizer info...
				for(unsigned short rownum=0;rownum<numRowsFldOnPage;rownum++){
					const unsigned short actualRow = rownum+startRow;
					if (lengths[actualRow]) {
						unsigned long drawPos;
						switch(alignment){
							case (oofColSizer::alignRight):{
										drawPos=xPos+view->colSizer()->width(i)-lengths[actualRow];
								 	 }
								 	 break;
							case (oofColSizer::alignCentre):{
										drawPos=xPos+((view->colSizer()->width(i)-lengths[actualRow])/2);
								 	 }
								 	 break;
							default: drawPos=xPos;
									 break;
						}
						if(actualRow==(sizedBody.GetNumRows(i)-1))	{	
							if (rowsToEndField>numRowsFldOnPage)  // more in this field than on this page
								bytes=lines[actualRow+1]-lines[actualRow];  // so safe to measure to line on next page
							else
								bytes=sizedBody.text(i).length()-lines[actualRow]; // remaining text in string is this line
						}
						else
							bytes=lines[actualRow+1]-lines[actualRow];
	
	// bit of a hack to avoid CRLF at end of line
						{
							const char* lastLine = sizedBody.text(i).chars() + lines[actualRow];
							if (bytes>0) {
								unsigned long lastCharAt = bytes -1;
								char lastChar = lastLine[lastCharAt];
								while(lastCharAt >0 && (lastChar=='\r' || lastChar=='\n')) {
									lastCharAt--;
									lastChar = lastLine[lastCharAt];
								}
								bytes = lastCharAt+1;
							}
						}
						
						if (bytes>0) {
							const char* toDraw = sizedBody.text(i).chars() + lines[actualRow];
							const int yPos = ybase+rownum * lineHeight;
							oofWinTextDE::recordElement( drawPos, yPos, toDraw, bytes);  
	//						oofTextDrawer::textOut(hdc, drawPos, yPos, toDraw, bytes);
						}
					} // if any length in this row
				}  // loop through rows of report
			}  // draw custom or normal
						
			if (adornCount) {
				// call after adorners for cell
				mOwner->adorners().start();
				for( unsigned long ad=0; ad<adornCount; ad++ ) {
					oofAdorner* currentAdorner = mOwner->adorners().current();
					if( currentAdorner->drawWhen() == oofAdorner::drawAfter ) {
						oofCurrentDraw::moveToEx( hdc, cellHorizontalPos, cellVerticalPos, NULL );
						//::MoveTo(cellHorizontalPos, cellVerticalPos);
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
		mOwner->drawColumnHeaders();  // NOT YET IMPLEMENTED - setting to say that we don't draw these on subsequent pages
		theEnv->resumeBandOverPage();
		curRep->changeTextStyle(mOwner->textStyle());  // In case the page break or col headers changed styles
		DrawContinuingOverPage(startRow+rowsDrawn, heightLeft, sizedBody, hasCustomDrawers);
	} else
		curRep->currentVerticalPos() += DrawHeight;
}


// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_WinRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void
oofRepSpaceBand_WinRender::draw()
{
	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_WinRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void
oofRepGraphBand_WinRender::draw()
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
	
	oofGraphDE::recordElement(theFrame, mOwner->graph(), printMono);
	
	mOwner->graph()->setPaneRect(theFrame);
	
	currentReport->currentVerticalPos() += drawingHeight;
	
	if (printMono)
		mOwner->graph()->setStyleToColor();
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_WinRender((oofRepLineBand*)newOwner);	// safe downcast
}


void
oofRepLineBand_WinRender::draw()
{
	unsigned long fixedHeight = mOwner->fixedHeight();
	oofRect theLine;

	theLine.set(oofRep::currentReport()->settings()->leftMargin() +
				oofRep::currentReport()->settings()->bandIndenting(),
				oofRep::currentReport()->currentVerticalPos(),
				oofRep::currentReport()->settings()->leftMargin() +
				oofRep::currentReport()->bodyWidth() -
				oofRep::currentReport()->settings()->bandIndenting(),
				oofRep::currentReport()->currentVerticalPos() + fixedHeight);
//981015	HDC	hdc = oofCurrentDraw::getCurrentDC();
	
	if( mOwner->grey() )
	{
//981015		::FillRect(hdc, theLine, (HBRUSH)GetStockObject(GRAY_BRUSH));
		oofWinShadeDE::recordElement(theLine, GRAY_BRUSH);
	} 
	else {
//981015		::FillRect(hdc, theLine, (HBRUSH)GetStockObject(BLACK_BRUSH));
		oofWinShadeDE::recordElement(theLine, BLACK_BRUSH);
	}

	oofRep::currentReport()->currentVerticalPos() += fixedHeight;
}

// -------------------------------------------------------
//    o o f R e p P i c t B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_WinRender((oofRepPictBand*)newOwner);	// safe downcast
}


void
oofRepPictBand_WinRender::draw()
{
	const unsigned short leftMargin = oofRep::currentReport()->settings()->leftMargin();
	const unsigned short topBand = oofRep::currentReport()->currentVerticalPos();
	oofWinPictDE::recordElement(
		leftMargin,
		topBand,
		leftMargin + oofRep::currentReport()->bodyWidth(),
		topBand+mOwner->fixedHeight(),
		mOwner->picture() 
	);
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_WinRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_WinRender::draw()
{
	oofRep::currentReport()->changeTextStyle(mOwner->textStyle());

//981015	HDC hdc = oofCurrentDraw::getCurrentDC();
	const oofString& text = mOwner->text();
//981015	::SetBkMode(hdc, TRANSPARENT);
	oofWinTextDE::recordElement( oofCurrentDraw::x(), oofCurrentDraw::y(), text);  // BEFORE textOutCP advances
	oofCurrentDraw::incX(width());	//981016 substitute for the advance in the textOutCP
//981015	oofTextDrawer::textOutCP(hdc, text, width());
}


unsigned long
oofRepTextBlock_WinRender::width() const
{
	oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
	return oofRepWinEnv::getTextExtent(mOwner->textStyle(), mOwner->text()).cx;
}

// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_WinRender((oofRepPictBlock*)newOwner);	// safe downcast
}


void
oofRepPictBlock_WinRender::draw()
{
	oofPict*	thePict = mOwner->picture();
	const long pictWidth = mOwner->width();
	const long x = oofCurrentDraw::x();
	const long y = oofCurrentDraw::y();
	const long right = x + pictWidth;
	const long bottom = y + mOwner->fixedHeight();
	oofWinPictDE::recordElement(x, y, right, bottom, thePict);
	oofCurrentDraw::incX(pictWidth);
}

unsigned long
oofRepPictBlock_WinRender::width() const
{
	return mOwner->width();
}




// -------------------------------------------------------
//    o o f R e p L a y o u t B a n d _ W i n R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_WinRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_WinRender((oofRepLayoutBand*)newOwner);	// safe downcast
}


/*
void
oofRepLayoutBand_WinRender::draw()
{
	oofRepBlockList& blocks = mOwner->blocks();

	blocks.start();
	oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
//	int charHeight = mBlocks()->textStyle()->calcCharHeight();
//	int ascent = mBlocks()->textStyle()->calcAscent();
	int charHeight = blocks()->textStyle()->fixedHeight();
	int ascent = 5;

	HDC	hdc = oofCurrentDraw::getCurrentDC();

	// When we render the bands, we want to control the positioning from here, not within the
	// band itself. Therefore, get the text functions to ignore the positioning arguments
	UINT	oldAlign = ::GetTextAlign(hdc);
	::SetTextAlign(hdc, oldAlign | TA_UPDATECP);

	::MoveToEx(hdc, oofRep::currentReport()->settings()->leftMargin() +
					oofRep::currentReport()->settings()->bandIndenting(),
		   oofRep::currentReport()->currentVerticalPos(), NULL); 

	while(blocks.more()) {
		oofRepBlock* theBlock = blocks();
		// KENTODO - Handle other alignments !
		// BOLD ASSUMPTION - only one right aligned item per line.
		if(theBlock->align()==oofRepBlock::alignRight) {
			POINT	pt;
			
			// Get the current position first
			::MoveToEx(hdc, 0, 0, &pt);
			// Now reposition properly for the right alignment
			::MoveToEx(hdc, oofRep::currentReport()->bodyWidth() +
				 oofRep::currentReport()->settings()->leftMargin() -
				 oofRep::currentReport()->settings()->bandIndenting() -
				 theBlock->width(), pt.y, NULL);
		}

		if(theBlock->align()==oofRepBlock::alignCentre) {
			POINT	pt;
			
			// Get the current position first
			::MoveToEx(hdc, 0, 0, &pt);
			// Now reposition for centre alignment
			::MoveToEx(hdc, (oofRep::currentReport()->bodyWidth()
				 +oofRep::currentReport()->settings()->leftMargin())/2
				 -(theBlock->width()/2), pt.y, NULL);
		}

		theBlock->draw();
		if (theBlock->fixedWidth()) {
			POINT	pt;
			::MoveToEx(hdc, 0, 0, &pt);
			const int fixedRemainder =  theBlock->fixedWidth() - theBlock->width();
			::MoveToEx(hdc, pt.x+fixedRemainder, pt.y, NULL);// move horizontally
		}
		if (theBlock->moveDownAfter()) {
			// KENTODO - Cope with multi-block lines with differing heights !
			POINT	pt;
			
			// Get the current position first
			::MoveToEx(hdc, 0, 0, &pt);
			// Now reposition properly for the right alignment
			::MoveToEx(hdc, oofRep::currentReport()->settings()->leftMargin() +
							oofRep::currentReport()->settings()->bandIndenting(), 
						pt.y + charHeight + blocks()->textStyle()->leading(), NULL);

			// Terminate this line
			blocks.next();
			if(blocks.more()) {
				oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
			}
		} else
			blocks.next();
	}

	// Re-enable the text function positioning
	::SetTextAlign(hdc, oldAlign);

	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}
*/

void
oofRepLayoutBand_WinRender::draw()
{
	oofRepBlockList& blocks = mOwner->blocks();

	blocks.start();
	oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
//	int charHeight = mBlocks()->textStyle()->calcCharHeight();
//	int ascent = mBlocks()->textStyle()->calcAscent();
	int charHeight = blocks()->textStyle()->fixedHeight();
	int ascent = 5;

	HDC	hdc = oofCurrentDraw::getCurrentDC();

	oofCurrentDraw::moveToEx(hdc, oofRep::currentReport()->settings()->leftMargin() +
					oofRep::currentReport()->settings()->bandIndenting(),
					oofRep::currentReport()->currentVerticalPos(), NULL); 

	while(blocks.more()) {
		oofRepBlock* theBlock = blocks();
		switch(theBlock->align()) {
		// BOLD ASSUMPTION - only one right aligned item per line.
		case oofRepBlock::alignRight : {
			POINT	pt;
			
			// Get the current position first
			oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);
			// Now reposition properly for the right alignment
			oofCurrentDraw::moveToEx(hdc, oofRep::currentReport()->bodyWidth() +
				oofRep::currentReport()->settings()->leftMargin() -
				oofRep::currentReport()->settings()->bandIndenting() -
				theBlock->width(), pt.y, NULL
			);
		}	
		break;

		case oofRepBlock::alignCentre : {
			POINT	pt;
			
			// Get the current position first
			oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);
			// Now reposition for centre alignment
			const long newX = oofRep::currentReport()->settings()->leftMargin() + (oofRep::currentReport()->bodyWidth()-theBlock->width())/2;
			oofCurrentDraw::moveToEx(hdc, newX, pt.y, NULL);
		}
		break;
		
		default :
			;  // NOT YET IMPLEMENTED - cope with other alignments?
		}  // switch alignments	

		SetCurrentBlockCoords(oofCurrentDraw::x(), oofCurrentDraw::y(), theBlock);  // save for callbacks
		theBlock->draw();
		if (theBlock->fixedWidth()) {
			POINT	pt;
			oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);
			const int fixedRemainder =  theBlock->fixedWidth() - theBlock->width();
			oofCurrentDraw::moveToEx(hdc, pt.x+fixedRemainder, pt.y, NULL);// move horizontally
		}
		if (theBlock->moveDownAfter()) {
			POINT	pt;
			
			// Get the current position first
			oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);
			// Now reposition properly for the right alignment
			oofCurrentDraw::moveToEx(hdc, oofRep::currentReport()->settings()->leftMargin() +
							oofRep::currentReport()->settings()->bandIndenting(), 
							pt.y + charHeight + blocks()->textStyle()->leading(), NULL);

			// Terminate this line
			blocks.next();
			if(blocks.more()) {
				oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
			}
		} else {
			if (oofRuntimeEnv::runtimeEnv()==oofRuntimeEnv::WinNT) {
			// AD 99/02/18 HACK TO COPE WITH BAD NT RENDERING
				POINT	pt;
				const int kArbitraryExtraMargin = theBlock->width()/20+2;
				// Get the current position first
				oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);
				oofCurrentDraw::moveToEx(hdc, pt.x+kArbitraryExtraMargin, pt.y, NULL);
			}
			blocks.next();
		}
	}

	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}



// -------------------------------------------------------
//          o o f W i n T e x t S t y l e D E
// -------------------------------------------------------

void 
oofWinTextStyleDE::draw() const
{
	assert(mStyle);
	safeChangeStyle(mStyle);
}


void 
oofWinTextStyleDE::safeChangeStyle(oofRepTextStyle* inStyle)
{
	// horrible hack to get access to an environment that will track the font
	// generated by using this text style and not delete it until later
	// the following is not quite as dangerous as it seems - we wouldn't be using Windows
	// style DE's unless some kind of Windows environment, but maybe vulnerable if
	// we ever move responsibilities up into oofRepWinEnv
	
	// copied from oofRepWinGUIPreviewMFC::drawWithTextStyle
	
	HDC dc = oofCurrentDraw::getCurrentDC();  // maybe there's another context in use apart from us
	if (dc != NULL) {
		HDC ic = oofCurrentDraw::getInfoDC();

		LOGFONT lf = inStyle->getLogFont();
//		const int pixelsY = ::GetDeviceCaps(ic, LOGPIXELSY);
		lf.lfHeight = -inStyle->fontSize();  // -MulDiv(inStyle->fontSize(), pixelsY, 72);
		HFONT hFont = oofTextDrawer::createFontIndirect(&lf);

		if (hFont != NULL) {
			oofTextDrawer::selectFont(dc, hFont);
		}
		// Set the foreground colour for text drawing (returns CLR_INVALID if fails)
		::SetTextColor(dc, inStyle->color());
	}  // have a DC
} 



void 
oofWinTextStyleDE::recordElement(const oofRepTextStyle* copiedStyle)
// factory that records element with current page
{
	appendElement(new oofWinTextStyleDE(copiedStyle));
}



// -------------------------------------------------------
//                 o o f W i n T e x t D E
// -------------------------------------------------------

void 
oofWinTextDE::draw() const
{
	const unsigned long strLen = mString.length();
	if (strLen>0) {
		HDC		hdc = oofCurrentDraw::getCurrentDC();
		::SetBkMode(hdc, TRANSPARENT);
		::TextOut(hdc, mX, mY, mString, strLen);
	}
} 


void 
oofWinTextDE::recordElement(int x, int y, const oofString& inStr)
// factory that records element with current page
{
	appendElement(new oofWinTextDE(x, y, inStr));
}


void 
oofWinTextDE::recordElement(int x, int y, LPCTSTR lpString, int cbString)
// factory that records element with current page
{
	appendElement(new oofWinTextDE(x, y, lpString, cbString));
}


// -------------------------------------------------------
//             o o f G r a p h D E
// -------------------------------------------------------
void 
oofGraphDE::draw() const
{
	if (mIsMono)
		mGraph->setStyleToMono();
	const oofRect theFrame(mX, mY, mRight, mBottom);
	mGraph->setPaneRect(theFrame);
	mGraph->draw();
	if (mIsMono)
		mGraph->setStyleToColor();
}
	

void 
oofGraphDE::recordElement(int x, int y, int right, int bottom, oofGraph* inGraph, bool isMono)
// factory that records element with current page
{
	appendElement(new oofGraphDE(x, y, right, bottom, inGraph, isMono));
}


void 
oofGraphDE::recordElement(const oofRect& R, oofGraph* inGraph, bool isMono)
// factory that records element with current page
{
	appendElement(new oofGraphDE(R, inGraph, isMono));
}


// -------------------------------------------------------
//             o o f W i n L i n e D E
// -------------------------------------------------------
void 
oofWinLineDE::draw() const
{
	POINT pt;
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	::MoveToEx(hdc, mX, mY, &pt);
	::LineTo(hdc, mRight, mBottom );
}
	

void 
oofWinLineDE::recordElement(int x, int y, int right, int bottom)
// factory that records element with current page
{
	appendElement(new oofWinLineDE(x, y, right, bottom));
}


void 
oofWinLineDE::recordElement(const oofRect& R)
// factory that records element with current page
{
	appendElement(new oofWinLineDE(R));
}


// -------------------------------------------------------
//             o o f W i n R e c t D E
// -------------------------------------------------------
void 
oofWinRectDE::draw() const
{
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	const oofRect theBox(mX, mY, mRight, mBottom);
	::FrameRect(hdc, theBox, (HBRUSH)GetStockObject(BLACK_BRUSH));
}
	

void 
oofWinRectDE::recordElement(int x, int y, int right, int bottom)
// factory that records element with current page
{
	appendElement(new oofWinRectDE(x, y, right, bottom));
}


void 
oofWinRectDE::recordElement(const oofRect& R)
// factory that records element with current page
{
	appendElement(new oofWinRectDE(R));
}


// -------------------------------------------------------
//             o o f W i n S h a d e D E
// -------------------------------------------------------
oofWinShadeDE::oofWinShadeDE(int x, int y, int right, int bottom, int stockBrush) :
		oofSizedDE(x, y, right, bottom),
		mUseStockBrush(true),
		mStockBrush(stockBrush)
{
// based on constants in wingdi.h
	assert(stockBrush >= WHITE_BRUSH && stockBrush <=NULL_BRUSH);
}
	
oofWinShadeDE::oofWinShadeDE(const oofRect& R, int stockBrush) :
		oofSizedDE(R),
		mUseStockBrush(true),
		mStockBrush(stockBrush)
{
	assert(stockBrush >= WHITE_BRUSH && stockBrush <=NULL_BRUSH);
}


void 
oofWinShadeDE::draw() const
{
	HBRUSH		colourBrush;
	const oofRect theBox(mX, mY, mRight, mBottom);
	if (mUseStockBrush)
		colourBrush = CreateSolidBrush(mStockBrush);
	else
		colourBrush = CreateSolidBrush(mFillColor);
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	::FillRect(hdc, theBox, colourBrush);
	::DeleteBrush( colourBrush );
}
	

void 
oofWinShadeDE::recordElement(int x, int y, int right, int bottom, COLORREF fillColor)
// factory that records element with current page
{
	appendElement(new oofWinShadeDE(x, y, right, bottom, fillColor));
}


void 
oofWinShadeDE::recordElement(const oofRect& R, COLORREF fillColor)
// factory that records element with current page
{
	appendElement(new oofWinShadeDE(R, fillColor));
}


// -------------------------------------------------------
//             o o f W i n P i c t D E
// -------------------------------------------------------
oofWinPictDE::oofWinPictDE(const oofWinPictDE& rhs) :
	oofSizedDE(rhs)
{
	assert(rhs.mPict);
	mPict = rhs.mPict->clone();
}


oofWinPictDE::~oofWinPictDE()
{
}


void 
oofWinPictDE::draw() const
{
	HDC		drawDC = oofCurrentDraw::getCurrentDC();
	const bool isPrinting = oofRep::currentReport()->environment()->isPrintingEnvironment();
	mPict->draw(mX, mY, mRight, mBottom, isPrinting, drawDC);
}


void 
oofWinPictDE::recordElement(int x, int y, int right, int bottom, oofPict* thePict)
// factory that records element with current page
{
	appendElement(new oofWinPictDE(x, y, right, bottom, thePict));
}


void 
oofWinPictDE::recordElement(const oofRect& R, oofPict* thePict)
// factory that records element with current page
{
	appendElement(new oofWinPictDE(R, thePict));
}
