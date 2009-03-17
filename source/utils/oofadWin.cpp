// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for Win output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPWIN
	#include "oofrpwin.h"
#endif
#ifndef H_OOFADORNWIN
	#include "oofadWin.h"
#endif


// USE MAC DUMMIES

// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofBorderAdorner::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofShadingAdorner::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofBandShadingAdorner::makeRender(const oofRepMacEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepWinEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_WinRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ W i n R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_WinRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_WinRender((oofBorderAdorner*)newOwner);	// safe downcast
}


void 
oofBorderAdorner_WinRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_WinRender::draw";
}


void
oofBorderAdorner_WinRender::drawcell( int x, int y, oofRepViewBand* drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode )
{
	HDC	hdc = oofCurrentDraw::getCurrentDC();

	// save current pen location
	POINT	pt;
	oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);

	// left line
	oofCurrentDraw::moveToEx(hdc, pt.x, pt.y, NULL );
	const int lineBottom = pt.y+height;
//981015	::LineTo( hdc, pt.x, lineBottom);
	oofWinLineDE::recordElement(pt.x, pt.y, pt.x, lineBottom);

	// right line
	const int lineRight = pt.x+width;
	oofCurrentDraw::moveToEx(hdc, lineRight, pt.y, NULL );
//981015	::LineTo( hdc, lineRight, lineBottom );
	oofWinLineDE::recordElement(lineRight, pt.y, lineRight, lineBottom);

	// top line
	if( overrunCode==oofAdorner::startStop || overrunCode==oofAdorner::startContinue ) {
		oofCurrentDraw::moveToEx(hdc, pt.x, pt.y, NULL );
//981015		::LineTo( hdc, pt.x+width, pt.y );
		oofWinLineDE::recordElement(pt.x, pt.y, lineRight, pt.y);
	}
	
	// bottom line
	if( overrunCode==oofAdorner::startStop || overrunCode==oofAdorner::continueStop ) {
		oofCurrentDraw::moveToEx(hdc, pt.x, lineBottom, NULL );
//981015		::LineTo( hdc, lineRight, lineBottom );
		oofWinLineDE::recordElement(pt.x, lineBottom, lineRight, lineBottom);
	}
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepWinEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_WinRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ W i n R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_WinRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_WinRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void
oofShadingAdorner_WinRender::draw(oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
	oofRect theBox;
//	HBRUSH	colourBrush;

	theBox.set(oofRep::currentReport()->settings()->leftMargin() + 1,
				oofRep::currentReport()->currentVerticalPos(),
				oofRep::currentReport()->settings()->leftMargin() + width - 1,
				oofRep::currentReport()->currentVerticalPos()+height);

	COLORREF brushColor = 
				RGB(0xFF - (0xFF * ((float)mOwner->red()/100)),
					0xFF - (0xFF * ((float)mOwner->green()/100)),
					0xFF - (0xFF * ((float)mOwner->blue()/100))
					);
//	colourBrush = CreateSolidBrush(brushColor);
//981015	HDC	hdc = oofCurrentDraw::getCurrentDC();
//981015	::FillRect(hdc, theBox, colourBrush);
//981015	DeleteBrush( colourBrush );
	oofWinShadeDE::recordElement(theBox, brushColor);
}

void
oofShadingAdorner_WinRender::drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode )
{
	HDC	hdc = oofCurrentDraw::getCurrentDC();
//	HBRUSH	colourBrush;
	
	// save current pen location
	POINT	pt;
	oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);

	oofRect theBox;

	theBox.set( pt.x, pt.y, pt.x + width, pt.y + height+1 );

	COLORREF brushColor = 
				RGB(0xFF - (0xFF * ((float)mOwner->red()/100)),
					0xFF - (0xFF * ((float)mOwner->green()/100)),
					0xFF - (0xFF * ((float)mOwner->blue()/100))
					);
//981015	colourBrush = CreateSolidBrush(brushColor);
//981015	::FillRect(hdc, theBox, colourBrush);
//981015	DeleteBrush( colourBrush );
	oofWinShadeDE::recordElement(theBox, brushColor);
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepWinEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_WinRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ W i n R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_WinRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_WinRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}


void 
oofBandShadingAdorner_WinRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long height, unsigned long width )
{
	oofRect theBox;
//	HBRUSH	colourBrush;

	theBox.set(oofRep::currentReport()->settings()->leftMargin() + 1,
				oofRep::currentReport()->currentVerticalPos(),
				oofRep::currentReport()->settings()->leftMargin() + width - 1,
				oofRep::currentReport()->currentVerticalPos()+height);

//981015	HDC	hdc = oofCurrentDraw::getCurrentDC();

	COLORREF brushColor = 
				RGB(0xFF - (0xFF * ((float)mOwner->percentage()/100)),
					0xFF - (0xFF * ((float)mOwner->percentage()/100)),
					0xFF - (0xFF * ((float)mOwner->percentage()/100))
					);
//981015	colourBrush = CreateSolidBrush(brushColor);
//981015	::FillRect(hdc, theBox, colourBrush);
//981015	DeleteBrush( colourBrush );
	oofWinShadeDE::recordElement(theBox, brushColor);
}


void 
oofBandShadingAdorner_WinRender::drawcell( int x, int /*y*/, oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode /*overrunCode*/ )
{
	HDC			hdc = oofCurrentDraw::getCurrentDC();
//	HBRUSH		colourBrush;
	oofRect 	theBox;
	
	// save current pen location
	POINT	pt;
	oofCurrentDraw::moveToEx(hdc, 0, 0, &pt);

	theBox.set( pt.x, pt.y, pt.x + width, pt.y + height+1 );
	
	// we want to extend the drawing to the left margin on the first cell,
	// and to the right margin on the far most cell
	
	// first cell
	if( x == 0 )
		theBox.left = oofRep::currentReport()->settings()->leftMargin() + 1;
		
	// last cell
	if( drawable->lastCol( x ) )
		theBox.right =	oofRep::currentReport()->settings()->leftMargin() +
							oofRep::currentReport()->bodyWidth() - 1;

	COLORREF brushColor = 
				RGB(0xFF - (0xFF * ((float)mOwner->percentage()/100)),
					0xFF - (0xFF * ((float)mOwner->percentage()/100)),
					0xFF - (0xFF * ((float)mOwner->percentage()/100))
					);
//981015	colourBrush = CreateSolidBrush(brushColor);
//981015	::FillRect(hdc, theBox, colourBrush);
//981015	DeleteBrush( colourBrush );
	oofWinShadeDE::recordElement(theBox, brushColor);
}
