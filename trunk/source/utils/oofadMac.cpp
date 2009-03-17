// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for Mac output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNMAC
	#include "oofadMac.h"
#endif


// USE WINDOWS DUMMIES

// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofBorderAdorner::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofShadingAdorner::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}


void	
oofBandShadingAdorner::makeRender(const oofRepWinEnv&)
{
	assert(0);	// THIS IS A DUMMY NOT MEANT TO BE CALLED
}

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepMacEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_MacRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ M a c R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_MacRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_MacRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void 
oofBorderAdorner_MacRender::draw( oofRepDrawable* /*drawable*/, unsigned long height, unsigned long width )
{
	Rect borderRect = { oofRep::currentReport()->currentVerticalPos(),
						oofRep::currentReport()->settings()->leftMargin(),
						oofRep::currentReport()->currentVerticalPos()+height,
						oofRep::currentReport()->settings()->leftMargin()+width };
	
	switch( mOwner->borderType() ) {
		
		case oofBorderAdorner::oneThin:
			PenSize(1,1);
			FrameRect( &borderRect );
			break;
		
		case oofBorderAdorner::twoThin:
			PenSize(1,1);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 2;
			borderRect.right = borderRect.right + 2;
			borderRect.top = borderRect.top - 2;
			borderRect.bottom = borderRect.bottom + 2;
			FrameRect( &borderRect );
			break;
		case oofBorderAdorner::threeThin:
			PenSize(1,1);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 2;
			borderRect.right = borderRect.right + 2;
			borderRect.top = borderRect.top - 2;
			borderRect.bottom = borderRect.bottom + 2;
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 2;
			borderRect.right = borderRect.right + 2;
			borderRect.top = borderRect.top - 2;
			borderRect.bottom = borderRect.bottom + 2;
			FrameRect( &borderRect );
			break;
		case oofBorderAdorner::oneThinOneThick:
			PenSize(1,1);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 3;
			borderRect.right = borderRect.right + 3;
			borderRect.top = borderRect.top - 3;
			borderRect.bottom = borderRect.bottom + 3;
			PenSize(2,2);
			FrameRect( &borderRect );
			PenSize(1,1);
			break;
		case oofBorderAdorner::oneThickoneThin:
			PenSize(2,2);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 2;
			borderRect.right = borderRect.right + 2;
			borderRect.top = borderRect.top - 2;
			borderRect.bottom = borderRect.bottom + 2;
			PenSize(1,1);
			FrameRect( &borderRect );
			break;
		case oofBorderAdorner::oneThinOneThickOneThin:
			PenSize(1,1);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 3;
			borderRect.right = borderRect.right + 3;
			borderRect.top = borderRect.top - 3;
			borderRect.bottom = borderRect.bottom + 3;
			PenSize(2,2);
			FrameRect( &borderRect );
			borderRect.left = borderRect.left - 2;
			borderRect.right = borderRect.right + 2;
			borderRect.top = borderRect.top - 2;
			borderRect.bottom = borderRect.bottom + 2;
			PenSize(1,1);
			FrameRect( &borderRect );
			break;
	}
}

void
oofBorderAdorner_MacRender::drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode )
{
	PenState	penState;		// place to save current pen configuration
	GetPenState(&penState);
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Point penPt;
	GetPen(&penPt);
  	const int origPenV = penPt.v;
	const int origPenH =  penPt.h;
#else
	const int origPenV = qd.thePort->pnLoc.v;
	const int origPenH = qd.thePort->pnLoc.h;
#endif
	const bool isLastCol = drawable->lastCol( x );
	
	// For the last cell
	// -1 to ensure it draws 'within' the table area
	if (isLastCol) 
		width--;

	// draw left
	MoveTo( origPenH, origPenV );
	drawTo( mOwner->leftStyle(), 0, height );

	if (isLastCol) {
		// draw right
// WARNING if draw multiple lines of same size on top of each other causes crash when print in colour		
		MoveTo( origPenH+width, origPenV );
		drawTo( mOwner->rightStyle(), 0, height );
	}
	
	// draw top
	if (overrunCode==oofAdorner::startStop || overrunCode==oofAdorner::startContinue) {
		MoveTo( origPenH, origPenV );
		drawTo( mOwner->topStyle(), width, 0 );
	}

	// draw bottom
	if ( (overrunCode==oofAdorner::startStop || overrunCode==oofAdorner::continueStop) 
		&& drawable->lastRow(y) ) {
// WARNING if draw multiple lines of same size on top of each other causes crash when print in colour		
		MoveTo( origPenH, origPenV+height );
		drawTo( mOwner->bottomStyle(), width, 0 );
	}

	SetPenState(&penState);
}

void
oofBorderAdorner_MacRender::drawTo( oofBorderAdorner::BorderCode type, long distHoriz, long distVert )
{
	//This code only draws straight lines...
	assert( distHoriz==0 || distVert==0 );
	
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Point penPt;
	GetPen(&penPt);
  	const int origPenV = penPt.v;
	const int origPenH =  penPt.h;
#else
	const int origPenV = qd.thePort->pnLoc.v;
	const int origPenH = qd.thePort->pnLoc.h;
#endif
	switch( type ) {
	case oofBorderAdorner::oneThin:
		PenSize(1,1);
		Line( distHoriz, distVert );
		//FrameRect( &borderRect );
		break;
	
	case oofBorderAdorner::twoThin:
		PenSize(1,1);
		if( distHoriz > distVert ) {		// we're drawing a horizontal line
			MoveTo( origPenH-1, origPenV+1 );	
			Line( distHoriz+2, distVert );
			MoveTo( origPenH-1, origPenV-1 );
			Line( distHoriz+2, distVert );
		}
		else {								// we're drawing a vertical line
			MoveTo( origPenH+1, origPenV-1 );
			Line( distHoriz, distVert+2 );
			MoveTo( origPenH-1, origPenV-1 );
			Line( distHoriz, distVert+2 );
		}
		break;
// 	case oofBorderAdorner::threeThin:
// 		PenSize(1,1);
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 2;
// 		borderRect.right = borderRect.right + 2;
// 		borderRect.top = borderRect.top - 2;
// 		borderRect.bottom = borderRect.bottom + 2;
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 2;
// 		borderRect.right = borderRect.right + 2;
// 		borderRect.top = borderRect.top - 2;
// 		borderRect.bottom = borderRect.bottom + 2;
// 		FrameRect( &borderRect );
// 		break;
// 	case oofBorderAdorner::oneThinOneThick:
// 		PenSize(1,1);
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 3;
// 		borderRect.right = borderRect.right + 3;
// 		borderRect.top = borderRect.top - 3;
// 		borderRect.bottom = borderRect.bottom + 3;
// 		PenSize(2,2);
// 		FrameRect( &borderRect );
// 		PenSize(1,1);
// 		break;
// 	case oofBorderAdorner::oneThickoneThin:
// 		PenSize(2,2);
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 2;
// 		borderRect.right = borderRect.right + 2;
// 		borderRect.top = borderRect.top - 2;
// 		borderRect.bottom = borderRect.bottom + 2;
// 		PenSize(1,1);
// 		FrameRect( &borderRect );
// 		break;
// 	case oofBorderAdorner::oneThinOneThickOneThin:
// 		PenSize(1,1);
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 3;
// 		borderRect.right = borderRect.right + 3;
// 		borderRect.top = borderRect.top - 3;
// 		borderRect.bottom = borderRect.bottom + 3;
// 		PenSize(2,2);
// 		FrameRect( &borderRect );
// 		borderRect.left = borderRect.left - 2;
// 		borderRect.right = borderRect.right + 2;
// 		borderRect.top = borderRect.top - 2;
// 		borderRect.bottom = borderRect.bottom + 2;
// 		PenSize(1,1);
// 		FrameRect( &borderRect );
// 		break;
	}
}


// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepMacEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_MacRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ M a c R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_MacRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_MacRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void 
oofShadingAdorner_MacRender::draw( oofRepDrawable* /*drawable*/, unsigned long height, unsigned long width )
{
	RGBColor 		gray;			// shaded gray colour
	PenState		penState;		// place to save current pen configuration
	PixPatHandle 	ppat;			// grey pattern
	
	//int height = drawable->fixedHeight();
	//int width = oofRep::currentReport()->bodyWidth();
	
	Rect box = {	oofRep::currentReport()->currentVerticalPos(),
					oofRep::currentReport()->settings()->leftMargin(),
					oofRep::currentReport()->currentVerticalPos()+height,
					oofRep::currentReport()->settings()->leftMargin()+width };

	GetPenState(&penState);

	// Create memory for pixpat
	ppat = NewPixPat();

	gray.red = 0xFFFF - (0xFFFF * ((float)mOwner->red()/100));
	gray.green = 0xFFFF - (0xFFFF * ((float)mOwner->green()/100));
	gray.blue = 0xFFFF - (0xFFFF * ((float)mOwner->blue()/100));

	if( ppat ) MakeRGBPat(ppat, &gray);

	if( ppat ) PenPixPat( ppat );

	PaintRect(&box);

	DisposePixPat( ppat );
	
	SetPenState(&penState);
}

void
oofShadingAdorner_MacRender::drawcell( int /*x*/, int /*y*/, oofRepViewBand* /*drawable*/, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode /*overrunCode*/ )
{
	RGBColor 		gray;			// shaded gray colour
	PenState		penState;		// place to save current pen configuration
	PixPatHandle 	ppat;			// grey pattern
	
	
	
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Point penPt;
	GetPen (&penPt);
  	Rect borderRect = { penPt.v,
						penPt.h,
						penPt.v+height,
						penPt.h+width };
#else	
	
	
	Rect borderRect = { qd.thePort->pnLoc.v,
						qd.thePort->pnLoc.h,
						qd.thePort->pnLoc.v+height,
						qd.thePort->pnLoc.h+width };
#endif	
	GetPenState(&penState);

	// Create memory for pixpat
	ppat = NewPixPat();

	gray.red = 0xFFFF - (0xFFFF * ((float)mOwner->red()/100));
	gray.green = 0xFFFF - (0xFFFF * ((float)mOwner->green()/100));
	gray.blue = 0xFFFF - (0xFFFF * ((float)mOwner->blue()/100));

	if( ppat ) MakeRGBPat(ppat, &gray);

	if( ppat ) PenPixPat( ppat );

	PaintRect(&borderRect);

	DisposePixPat(ppat);

	SetPenState(&penState);
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepMacEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_MacRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ M a c R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_MacRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_MacRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_MacRender::draw( oofRepDrawable* /*drawable*/, unsigned long height, unsigned long width )
{
	RGBColor 		gray;			// shaded gray colour
	PenState		penState;		// place to save current pen configuration
	PixPatHandle 	ppat;			// grey pattern

	Rect box = {	oofRep::currentReport()->currentVerticalPos(),
					oofRep::currentReport()->settings()->leftMargin(),
					oofRep::currentReport()->currentVerticalPos()+height,
					oofRep::currentReport()->settings()->leftMargin()+width };

	GetPenState(&penState);

	// Create memory for pixpat
	ppat = NewPixPat();

	gray.red = 0xFFFF - (0xFFFF * ((float)mOwner->percentage()/100));
	gray.green = gray.red;
	gray.blue = gray.red;

	if( ppat ) MakeRGBPat(ppat, &gray);

	if( ppat ) PenPixPat( ppat );

	PaintRect(&box);

	DisposePixPat( ppat );
	
	SetPenState(&penState);
}


void 
oofBandShadingAdorner_MacRender::drawcell( int x, int /*y*/, oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode /*overrunCode*/ )
{
	RGBColor 		gray;			// shaded gray colour
	PenState		penState;		// place to save current pen configuration
	PixPatHandle 	ppat;			// grey pattern
	
		
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Point penPt;
	GetPen (&penPt);
  	Rect borderRect = { penPt.v,
						penPt.h,
						penPt.v+height,
						penPt.h+width };
#else	
	Rect borderRect = { qd.thePort->pnLoc.v,
						qd.thePort->pnLoc.h,
						qd.thePort->pnLoc.v+height,
						qd.thePort->pnLoc.h+width };
#endif	
	// we want to extend the drawing to the left margin on the first cell,
	// and to the right margin on the far most cell
	
	// first cell
	if( x == 0 )
		borderRect.left = oofRep::currentReport()->settings()->leftMargin();
		
	// last cell
	if( drawable->lastCol( x ) )
		borderRect.right =	oofRep::currentReport()->settings()->leftMargin() +
							oofRep::currentReport()->bodyWidth();
		
	GetPenState(&penState);

	// Create memory for pixpat
	ppat = NewPixPat();

	gray.red = 0xFFFF - (0xFFFF * ((float)mOwner->percentage()/100));
	gray.green = gray.red;
	gray.blue = gray.red;

	if( ppat ) MakeRGBPat(ppat, &gray);

	if( ppat ) PenPixPat( ppat );

	PaintRect(&borderRect);
	
	DisposePixPat(ppat);
	
	SetPenState(&penState);
}
