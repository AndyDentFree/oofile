// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofGeos.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFGEOS
	#include "oofGeos.h"
#endif
#ifndef H_OOFSRSET
	#include "oofSrSet.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFDSTYL
	#include "oofDStyl.h"
#endif
#ifndef H_oofSting
	#include "oofSting.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
	
#include <assert.h>
#include <math.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// ---------------------------------------------------------------------------------
//		 oofGeo
// ---------------------------------------------------------------------------------


oofGeo::oofGeo(int XIdeal, int YIdeal, oofSeries* SeriesPtr, PositionState pos)
{
	mXIdeal = XIdeal;
	mYIdeal = YIdeal;
	
	mXDraw = 0;
	mYDraw = 0;
	
	mSeriesPtr = SeriesPtr;
	
	mPosition = pos;
}

oofGeo::~oofGeo()
{
}


oofSeries*
oofGeo::SeriesPtr()
{
	return mSeriesPtr;
}


void
oofGeo::setLocation(long inXIdeal, long inYIdeal)
{
	mXIdeal = inXIdeal;
	mYIdeal = inYIdeal;
}


void
oofGeo::draw()
{
	// get a draw style
	const oofDrawStyle* currentStyle=0;	
	if (mDrawStyle) 
		currentStyle = mDrawStyle;
	else
		if (mSeriesPtr) 
			currentStyle = mSeriesPtr->drawStyle();
		else 
			currentStyle = oofGraph::currentGraph()->drawStyle();
	
	assert(currentStyle);	// A style must have been set by this stage
	// call the child type DrawSelf function
	DrawSelf(currentStyle);
}


void
oofGeo::describe(ostream& os) const
{
     os << endl << "geo type: " << geoType() << endl;
     os << "mXIdeal: " << mXIdeal << "mYIdeal: " << mYIdeal << "mXDraw: " << mXDraw << "mYDraw :" << mYDraw << endl << endl ; 
}


oofString
oofGeo::geoType() const
{
	return "oofGeo";
}


oofRect 
oofGeo::BoundingRect(oofTransformationManager*) 
{ 
	return oofRect(0,0,0,0);
}


/* UnJames
void
oofGeo::UpdateMargins(oofTransformationManager* inTransformationManagerPtr, oofRect& oldBounds)
{
	oofRect margins	= inTransformationManagerPtr->getMargins();
	oofRect bounds	= GetAlreadyTransformedBoundingRect(inTransformationManagerPtr);
	bool changed	= false;

	switch (mPosition) {
	case posTop:
	case posCentre:			// posCentre is assumed to be top/centre
		if (bounds.bottom > margins.top) {
			margins.top	+= (bounds.bottom - oldBounds.bottom);
			changed = true;
		}
		break;
	case posToLeft:
		if (bounds.right > margins.left) {
			margins.left += (bounds.right - oldBounds.right);
			changed = true;
		}
		break;
	case posBelow:
		if (bounds.height() > margins.bottom) {
			margins.bottom += (bounds.height() - oldBounds.height());
			changed = true;
		}
		break;
	}
}



oofRect	
oofGeo::GetAlreadyTransformedBoundingRect(oofTransformationManager* tm)		
{ 
	oofRect r = BoundingRect(tm); 	// default ends up with double transform, unless they override this method
	return r; 
}
 end of UnJames  */


// ---------------------------------------------------------------------------------
//		 oofLabelledGeo
// ---------------------------------------------------------------------------------

oofLabelledGeo::oofLabelledGeo(int XIdeal, int YIdeal, oofSeries* SeriesPtr, PositionState pos)
			: oofGeo(XIdeal,YIdeal,SeriesPtr,pos)
{
	mValueLabel	= NULL;
}

oofLabelledGeo::oofLabelledGeo(int XIdeal ,int /*width*/,int /*height*/, oofSeries* SeriesPtr)
	: oofGeo(XIdeal,0,SeriesPtr,posNone)
{
	mValueLabel	= NULL;
}

oofLabelledGeo::~oofLabelledGeo()
{
	if (mValueLabel)
		delete mValueLabel;
}

void
oofLabelledGeo::setLabel(bool required, int value, int x, int y, int width)
{
	if (!required && mValueLabel) {
		delete mValueLabel;
		mValueLabel = NULL;
	}
	mRequired	= required;
	mValue		= value;
	mXTextPos	= x;
	mYTextPos	= y;
	mTextWidth	= width;
}

void
oofLabelledGeo::DrawSelf(const oofDrawStyle* /*inStyle*/)
{
	if (mValueLabel)
		mValueLabel->draw();
}

void
oofLabelledGeo::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	if (mRequired) {
		if (mValueLabel)
			delete mValueLabel;

		// draw the quantity for the column
		// find number of digits
		char numberBuf[20];
		sprintf(numberBuf, "%d", mValue);

		mValueLabel = new oofGeoText(mXTextPos, mYTextPos, mTextWidth, numberBuf, NULL, posCentre);
		mValueLabel->setTextSize(10);

		// Now transform it
		mValueLabel->Transform(inTransformationManagerPtr);
	}
}


void
oofLabelledGeo::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mValueLabel : " << mValueLabel
    << "mRequired : " << mRequired
    << "mValue : " << mValue 
    << "mXTextPos :" << mXTextPos
    << "mYTextPos :" << mYTextPos
    << "mTextWidth :" << mTextWidth << endl << endl ; 
}


oofString
oofLabelledGeo::geoType() const
{
	return "oofLabelledGeo";
}


// ---------------------------------------------------------------------------------
//		 oofGeoColumn
// ---------------------------------------------------------------------------------


oofGeoColumn::oofGeoColumn(int XIdeal, int YIdeal,
			int width,int height, oofSeries* SeriesPtr, PositionState pos)
	: oofLabelledGeo(XIdeal,YIdeal,SeriesPtr,pos)
{
	mIdealWidth = width;
	mIdealHeight = height;
}


oofGeoColumn::oofGeoColumn(int XIdeal ,int width,int height, oofSeries* SeriesPtr)
	: oofLabelledGeo(XIdeal,0,SeriesPtr,posNone)
{
	mIdealWidth = width;
	mIdealHeight = height;
}

void
oofGeoColumn::DrawSelf(const oofDrawStyle* inStyle)
{
	oofLabelledGeo::DrawSelf(inStyle);

// draw AFTER label above bar, so it doesn't erase our top line
	inStyle->useStyleColors();
	oofRect	theRect;
	
	theRect.set(mXDraw,
					mYDraw - mDrawHeight,
					mXDraw + mDrawWidth,
					mYDraw);
#ifdef _Macintosh
	::FillRect(theRect,inStyle->currentPattern());
//	::FrameRect(theRect);	
	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		// Draw pretend 3D stuff
		unsigned depth=mDrawWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		PolyHandle BoxTop=::OpenPoly();
		::MoveTo(theRect.left,theRect.top);
		::Line(depth,-depth);
		::Line(theRect.right-theRect.left,0);
		::Line(-depth,depth);
		::Line(theRect.left-theRect.right,0);
		::ClosePoly();
		PolyHandle BoxSide=::OpenPoly();
		::MoveTo(theRect.right,theRect.top);
		::Line(depth,-depth);
		::Line(0,theRect.bottom-theRect.top);
		::Line(-depth,depth);
		::Line(0,theRect.top-theRect.bottom);
		::ClosePoly();
		if (inStyle->isMono()) {
			::FillPoly(BoxTop,inStyle->currentPattern());
			::FillPoly(BoxSide,inStyle->currentPattern());
// boxes AFTER the fills so they draw on top
			::FrameRect(theRect);	
			::FramePoly(BoxTop);
			::FramePoly(BoxSide);
		}
		else {  // colour variations on the screen

			// OK - Now paint the boxes !
			RGBColor TopColor=inStyle->foreColor();
			RGBColor SideColor=inStyle->foreColor();
			// Make the top colour a bit darker and the side a lot darker
			TopColor.red=TopColor.red*0.8;
			TopColor.green=TopColor.green*0.8;
			TopColor.blue=TopColor.blue*0.8;
			::RGBForeColor(&TopColor);
			::FillPoly(BoxTop,inStyle->currentPattern());
			SideColor.red=SideColor.red*0.4;
			SideColor.green=SideColor.green*0.4;
			SideColor.blue=SideColor.blue*0.4;
			::RGBForeColor(&SideColor);
			::FillPoly(BoxSide,inStyle->currentPattern());
			// Fix the colors !
			inStyle->useStyleColors();
		}
	}
#elif defined _Windows
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hdc, (HBRUSH)inStyle->currentPattern());

	// We should already have the correct pen selected
	::Rectangle(hdc, theRect.left, theRect.top, theRect.right, theRect.bottom);
	// Cleanup
	::SelectObject(hdc, hOldBr);

	if(oofGraph::currentGraph()->settings()->has3DEffect()) {
		// Draw pretend 3D stuff
		unsigned depth=(float)mDrawWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		// fill and current drawing handles
		HBRUSH newBrush;
		HBRUSH oldBrush = (HBRUSH)::GetCurrentObject(hdc, OBJ_BRUSH);
		HPEN newPen;
		HPEN oldPen = (HPEN)::GetCurrentObject(hdc, OBJ_PEN);
		COLORREF sideColor;
		COLORREF foreColor = inStyle->foreColor();
		// draw side of the box (which is one point bigger, because border is not affected
		// when area is filled and we are using empty pen when drawing borders)
		POINT points[4] = {
			{theRect.left-1, theRect.top+1},
			{theRect.left+depth+1, theRect.top-depth-1},
			{theRect.right+depth+1, theRect.top-depth-1},
			{theRect.right-1, theRect.top+1}
		};

		sideColor = RGB(
			GetRValue(foreColor) * 0.8,
			GetGValue(foreColor) * 0.8,
			GetBValue(foreColor) * 0.8
		);

		newBrush	= ::CreateSolidBrush(sideColor);
		newPen		= (HPEN)::GetStockObject(NULL_PEN);

		assert(newBrush != NULL);
		assert(newPen != NULL);

		::SelectObject(hdc, newBrush);
		::SelectObject(hdc, newPen);

		::Polygon(hdc, points, 4);

		// draw top of the box
		POINT topPoints[4] = {
			{theRect.right-1, theRect.bottom},
			{theRect.right+depth+1, theRect.bottom-depth-2},
			{theRect.right+depth+1, theRect.top-depth-1},
			{theRect.right-1, theRect.top+1}
		};

		sideColor = RGB(
			GetRValue(foreColor) * 0.4,
			GetGValue(foreColor) * 0.4,
			GetBValue(foreColor) * 0.4
		);

		newBrush = ::CreateSolidBrush(sideColor);
		assert(newBrush != NULL);
		::DeleteObject(::SelectObject(hdc, newBrush));

		::Polygon(hdc, topPoints, 4);

		// return old drawing tools and destroy the old ones, which was selected last
		if (oldBrush)
			::DeleteObject(::SelectObject(hdc, oldBrush));
		if (oldPen)
			::SelectObject(hdc, oldPen);	// don't delete stock object
		// Fix the colors !
		inStyle->useStyleColors();
	}
#endif	// OS
}


oofRect
oofGeoColumn::BoundingRect(oofTransformationManager* )
{	
	oofRect r;
	
	r.left	= mXIdeal;
	r.right	= mXIdeal + mIdealWidth;
	if(mIdealHeight >= 0) {
		r.bottom	= mYIdeal;
		r.top		= mYIdeal + mIdealHeight;
	} 
	else {
		r.top		= mYIdeal;
		r.bottom	= mYIdeal + mIdealHeight;
	}
	
	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		unsigned depth=mIdealWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		r.right+=depth;
		r.top+=depth;
	}
	return r;
}


void
oofGeoColumn::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	// transform the draw start point (bottom left)
	inTransformationManagerPtr->TransformPoint
					(mXIdeal,mYIdeal,mXDraw,mYDraw);
	// transform the column width
	inTransformationManagerPtr->TransformHoLength
					(mIdealWidth,mDrawWidth);
	// transform the column height
	inTransformationManagerPtr->TransformVeLength
					(mIdealHeight,mDrawHeight);

	// See if we need labels
	setLabel(oofGraph::currentGraph()->settings()->hasValuesOverColumns(), 
			mIdealHeight, 
			mXIdeal, 
			mYIdeal + mIdealWidth,
			mIdealWidth );
	oofLabelledGeo::Transform(inTransformationManagerPtr);
}


void
oofGeoColumn::describe(ostream& os) const
{
    oofLabelledGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealWidth : " << mIdealWidth
    << "mIdealHeight : " << mIdealHeight
    << "mDrawWidth : " << mDrawWidth 
    << "mDrawHeight :" << mDrawHeight << endl ; 
}


oofString
oofGeoColumn::geoType() const
{
	return "oofGeoColumn";
}


// ---------------------------------------------------------------------------------
//		 oofGeoPoint
// ---------------------------------------------------------------------------------


oofGeoPoint::oofGeoPoint(int XIdeal, int YIdeal, oofSeries* SeriesPtr, PositionState pos)
	: oofGeo(XIdeal,YIdeal,SeriesPtr,pos)
{
}


void
oofGeoPoint::DrawSelf(const oofDrawStyle* inStyle)
{
	inStyle->useStyleColors();
	inStyle->symbol()->draw(mXDraw,mYDraw);
}


oofRect
oofGeoPoint::BoundingRect(oofTransformationManager* )
{	
	oofRect	r;
	// return a simple point rectangle
	r.set(mXIdeal,mYIdeal,mXIdeal,mYIdeal);
		
	return r;
}


void
oofGeoPoint::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	// transform the draw start point (bottom left)
	inTransformationManagerPtr->TransformPoint
					(mXIdeal,mYIdeal,mXDraw,mYDraw);
}


void
oofGeoPoint::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os <<endl;
}


oofString
oofGeoPoint::geoType() const
{
	return "oofGeoPoint";
}


// ---------------------------------------------------------------------------------
//		 oofGeoLine
// ---------------------------------------------------------------------------------


oofGeoLine::oofGeoLine(int XIdeal, int YIdeal, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoPoint(XIdeal,YIdeal,SeriesPtr,pos)
{
	mLastPoint = (oofGeoLine*)mSeriesPtr->lastGeo();
	mSeriesPtr->setLastGeo(this);
}


void
oofGeoLine::DrawSelf(const oofDrawStyle* inStyle)
{
/* The first point-line object in a linked set is a special object which causes
all the lines to be drawn for the series with which it is associated.
*/
	inStyle->useStyleColors();
	oofGeoLine *theCurrentPoint;
	if (mLastPoint == 0) {	// if this could be the first of a set of linked points
		theCurrentPoint=(oofGeoLine*)mSeriesPtr->lastGeo();
		if(theCurrentPoint){
#ifdef _Macintosh
			::MoveTo(theCurrentPoint->locationX(),theCurrentPoint->locationY());
#elif defined _Windows
			HDC		hCurrdc = oofCurrentDraw::getCurrentDC();
			::MoveToEx(hCurrdc, theCurrentPoint->locationX(),theCurrentPoint->locationY(), NULL);
#endif	// OS
			theCurrentPoint = theCurrentPoint->lastPoint();
			while (theCurrentPoint) {	// if there are linked points
				// draw the line
#ifdef _Macintosh
				::LineTo(theCurrentPoint->locationX(),theCurrentPoint->locationY());
#elif defined _Windows
				::LineTo(hCurrdc, theCurrentPoint->locationX(),theCurrentPoint->locationY());
#endif	// OS
				// reset the current point
				theCurrentPoint = theCurrentPoint->lastPoint();
			}
		}
	}
	// draw the symbol
	oofGeoPoint::DrawSelf(inStyle);
}


void
oofGeoLine::describe(ostream& os) const
{
    oofGeoPoint::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mLastPoint : " << mLastPoint << endl ; 
}


oofString
oofGeoLine::geoType() const
{
	return "oofGeoLine";
}


// ---------------------------------------------------------------------------------
//		 oofGeoBar
// ---------------------------------------------------------------------------------


oofGeoBar::oofGeoBar(int XIdeal, int YIdeal,
			int width,int height, oofSeries* SeriesPtr, PositionState pos)
	: oofLabelledGeo(XIdeal,YIdeal,SeriesPtr,pos)
{
	mIdealWidth = width;
	mIdealHeight = height;
}

oofGeoBar::oofGeoBar(int YIdeal ,int width,int height, oofSeries* SeriesPtr)
	: oofLabelledGeo(0,YIdeal,SeriesPtr,posNone)
{
	mIdealWidth = width;
	mIdealHeight = height;
}


void
oofGeoBar::DrawSelf(const oofDrawStyle* inStyle)
{
	oofLabelledGeo::DrawSelf(inStyle);

	// draw AFTER label to the right of bar, so it doesn't erase our right line
	inStyle->useStyleColors();
	oofRect	theRect(mXDraw, 
					mYDraw - mDrawWidth, 
					mXDraw + mDrawHeight, 
					mYDraw);
#ifdef _Macintosh
	::FillRect(theRect,inStyle->currentPattern());
//	::FrameRect(theRect);
	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		// Draw pretend 3D stuff
		unsigned depth=(float)mDrawWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		PolyHandle BoxSide=::OpenPoly();
		::MoveTo(theRect.left,theRect.top);
		::Line(depth,-depth);
		::Line(theRect.right-theRect.left,0);
		::Line(-depth,depth);
		::Line(theRect.left-theRect.right,0);
		::ClosePoly();
		PolyHandle BoxTop=::OpenPoly();
		::MoveTo(theRect.right,theRect.top);
		::Line(depth,-depth);
		::Line(0,theRect.bottom-theRect.top);
		::Line(-depth,depth);
		::Line(0,theRect.top-theRect.bottom);
		::ClosePoly();
		if (inStyle->isMono()) {
			::FillPoly(BoxTop,inStyle->currentPattern());
			::FillPoly(BoxSide,inStyle->currentPattern());
// boxes AFTER the fills so they draw on top
			::FrameRect(theRect);	
			::FramePoly(BoxTop);
			::FramePoly(BoxSide);
		}
		else {  // colour variations on the screen

			// OK - Now paint the boxes !
			RGBColor TopColor=inStyle->foreColor();
			RGBColor SideColor=inStyle->foreColor();
			// Make the top colour a bit darker and the side a lot darker
			TopColor.red=TopColor.red*0.4;
			TopColor.green=TopColor.green*0.4;
			TopColor.blue=TopColor.blue*0.4;
			::RGBForeColor(&TopColor);
			::FillPoly(BoxTop,inStyle->currentPattern());
			SideColor.red=SideColor.red*0.8;
			SideColor.green=SideColor.green*0.8;
			SideColor.blue=SideColor.blue*0.8;
			::RGBForeColor(&SideColor);
			::FillPoly(BoxSide,inStyle->currentPattern());
			// Fix the colors !
			inStyle->useStyleColors();
		}
	}
#elif defined _Windows
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hdc, (HBRUSH)inStyle->currentPattern());

	// We should already have the correct pen selected
	::Rectangle(hdc, theRect.left, theRect.top, theRect.right, theRect.bottom);
	// Cleanup
	::SelectObject(hdc, hOldBr);

	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		// Draw pretend 3D stuff
		unsigned depth=(float)mDrawWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		// fill and current drawing handles
		HBRUSH newBrush;
		HBRUSH oldBrush = (HBRUSH)::GetCurrentObject(hdc, OBJ_BRUSH);
		HPEN newPen;
		HPEN oldPen = (HPEN)::GetCurrentObject(hdc, OBJ_PEN);
		COLORREF sideColor;
		COLORREF foreColor = inStyle->foreColor();
		// draw side of the box (which is one point bigger, because border is not affected
		// when area is filled and we are using empty pen when drawing borders)
		POINT points[4] = {
			{theRect.left-1, theRect.top+1},
			{theRect.left+depth+1, theRect.top-depth-1},
			{theRect.right+depth+1, theRect.top-depth-1},
			{theRect.right-1, theRect.top+1}
		};

		sideColor = RGB(
			GetRValue(foreColor) * 0.8,
			GetGValue(foreColor) * 0.8,
			GetBValue(foreColor) * 0.8
		);

		newBrush	= ::CreateSolidBrush(sideColor);
		newPen		= (HPEN)::GetStockObject(NULL_PEN);

		assert(newBrush != NULL);
		assert(newPen != NULL);

		::SelectObject(hdc, newBrush);
		::SelectObject(hdc, newPen);

		::Polygon(hdc, points, 4);

		// draw top of the box
		POINT topPoints[4] = {
			{theRect.right-1, theRect.bottom},
			{theRect.right+depth+1, theRect.bottom-depth-2},
			{theRect.right+depth+1, theRect.top-depth-1},
			{theRect.right-1, theRect.top+1}
		};

		sideColor = RGB(
			GetRValue(foreColor) * 0.4,
			GetGValue(foreColor) * 0.4,
			GetBValue(foreColor) * 0.4
		);

		newBrush = ::CreateSolidBrush(sideColor);
		assert(newBrush != NULL);
		::DeleteObject(::SelectObject(hdc, newBrush));

		::Polygon(hdc, topPoints, 4);

		// return old drawing tools and destroy the old ones, which was selected last
		if (oldBrush)
			::DeleteObject(::SelectObject(hdc, oldBrush));
		if (oldPen)
			::SelectObject(hdc, oldPen);	// don't delete stock object
		// Fix the colors !
		inStyle->useStyleColors();
	}
#endif	// OS
}


oofRect
oofGeoBar::BoundingRect(oofTransformationManager* )
{	
	oofRect r;
	
	r.bottom=mYIdeal;
	r.top=mYIdeal+mIdealWidth;
	if(mIdealHeight>=0) {
		r.left=mXIdeal;
		r.right=mXIdeal+mIdealHeight;
	} else {
		r.right=mXIdeal;
		r.left=mXIdeal+mIdealHeight;	
	}
	
	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		unsigned depth=mIdealWidth*oofGraph::currentGraph()->settings()->the3DEffectMultiplier();
		r.right+=depth;
		r.top+=depth;
	}
	return r;
}


void
oofGeoBar::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	// transform the draw start point (bottom left)
	inTransformationManagerPtr->TransformPoint
					(mXIdeal,mYIdeal,mXDraw,mYDraw);
	// transform the row width
	inTransformationManagerPtr->TransformVeLength
					(mIdealWidth,mDrawWidth);
	// transform the row height
	inTransformationManagerPtr->TransformHoLength
					(mIdealHeight,mDrawHeight);

	// See if we need labels
	setLabel(oofGraph::currentGraph()->settings()->hasValuesNextToBars(), 
			mIdealHeight, 
			mXIdeal + mIdealHeight, 
			mYIdeal,
			mIdealHeight == 0 ? 1 : ((int)log10((double)abs(mIdealHeight)) + 2));		
	oofLabelledGeo::Transform(inTransformationManagerPtr);
}


void
oofGeoBar::describe(ostream& os) const
{
    
    oofLabelledGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealWidth : " << mIdealWidth
     << "mDrawWidth : " << mDrawWidth 
    << "mIdealHeight : " << mIdealHeight
    << "mDrawHeight :" << mDrawHeight 
    << "mDrawHeight :" << mDrawHeight<< endl ; 
}


oofString
oofGeoBar::geoType() const
{
	return "oofGeoBar";
}


// ---------------------------------------------------------------------------------
//		 oofGeoSector
// ---------------------------------------------------------------------------------


oofGeoSector::oofGeoSector(int XIdeal, int YIdeal, int diameter,
						int start, int arc, oofSeries* SeriesPtr , PositionState pos)
	: oofGeo(XIdeal,YIdeal,SeriesPtr,pos)
{
	mIdealDiameter = diameter;
	mStartAngle = start;
	mArcAngle = arc;
}


void
oofGeoSector::DrawSelf(const oofDrawStyle* inStyle)
{
	inStyle->useStyleColors();
	oofRect	theRect;
	
	theRect.bottom=mYDraw;
	theRect.top=mYDraw-mDrawDiameter;
	theRect.left=mXDraw;
	theRect.right=mXDraw+mDrawDiameter;
	
#ifdef _Macintosh
	::FillArc(theRect,mStartAngle,mArcAngle,inStyle->currentPattern());
	::FrameArc(theRect,mStartAngle,mArcAngle);

// The following is just a start - it's nowhere near finished !

/*	if(oofGraph::currentGraph()->settings()->has3DEffect()){
		// Draw pretend 3D stuff
		//unsigned depth=(float)mDrawDiameter/100*K3DMULTIPLIER;
		unsigned depth=20;
		RgnHandle Side=::NewRgn();
		::OpenRgn();
		::FrameArc(theRect,mStartAngle,mArcAngle);
		::Line(0,depth);
		Rect OffsetArc=theRect;
		::OffsetRect(&OffsetArc,0,depth);
		::FrameArc(&OffsetArc,mStartAngle+mArcAngle,-mArcAngle);
		::Line(0,-depth);
		::CloseRgn(Side);
		RGBColor SideColor=inStyle->foreColor();
		// Make the side a bit darker
		SideColor.red=SideColor.red*0.6;
		SideColor.green=SideColor.green*0.6;
		SideColor.blue=SideColor.blue*0.6;
		::RGBForeColor(&SideColor);
		::FillRgn(Side,inStyle->currentPattern());
		// Fix the colors !
		inStyle->useStyleColors();
	}
*/

#elif defined _Windows
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hdc, (HBRUSH)inStyle->currentPattern());
	POINT	ptStart, ptEnd;

	// We have to manipulate the two angles into points on the arc
	ptStart	= CalcRadialFromAngle(theRect, mStartAngle + mArcAngle);
	// Need to convert mArcAngle into degress from vertical instead of
	// degrees from start angle
	ptEnd	= CalcRadialFromAngle(theRect, mStartAngle);

	Pie(hdc, theRect.left, 
			theRect.top, 
			theRect.right, 
			theRect.bottom,
			ptStart.x,
			ptStart.y,
			ptEnd.x,
			ptEnd.y);
	// Cleanup
	::SelectObject(hdc, hOldBr);
#endif	// OS
}

oofRect
oofGeoSector::BoundingRect(oofTransformationManager* )
{
	// All the sectors just return the bounding rectangle of the entire pie
	oofRect r;
	
	r.bottom=mYIdeal;
	r.top=mYIdeal+mIdealDiameter;
	r.left=mXIdeal;
	r.right=mXIdeal+mIdealDiameter;
	
	return r;
}

#ifdef _Windows
POINT
oofGeoSector::CalcRadialFromAngle(oofRect& rcBound, int nAngle)
{
	POINT	ptRet, ptCentre;
	
	// The passed in angle is assumed to be referenced from vertically
	// up being angle 0. The Mac arc functions assume the first 'start'
	// angle is referenced from there, but the second 'arc' angle is referenced
	// from the first angle. 
	// To make this routine generic, the angle MUST be translated back to the 
	// vertical position by the calling routine.

	ptCentre.x	= rcBound.left + (rcBound.right - rcBound.left)/2;

	ptCentre.y	= rcBound.top + (rcBound.bottom - rcBound.top)/2;
	nAngle		= nAngle % 360;
	if (nAngle < 0)
		nAngle += 360;
	
	if (nAngle == 90) {
		ptRet.x	= rcBound.right;
		ptRet.y	= ptCentre.y;
	}
	else if (nAngle == 270) {
		ptRet.x	= rcBound.left;
		ptRet.y	= ptCentre.y;
	}
	else if (nAngle <= 45 || nAngle >= 315) {
		ptRet.x	= ptCentre.x + (ptCentre.y - rcBound.top) * tan(ConvertToRadians(nAngle));
		ptRet.y	= rcBound.top;
	}
	else if (45 < nAngle && nAngle < 135) {
		ptRet.x	= rcBound.right;
		ptRet.y	= ptCentre.y - ((rcBound.right - ptCentre.x) * tan(ConvertToRadians(90 - nAngle)));
	}
	else if (135 <= nAngle && nAngle <= 225) {
		ptRet.x	= ptCentre.x - (ptCentre.y - rcBound.top) * tan(ConvertToRadians(nAngle));
		ptRet.y	= rcBound.bottom;
	}
	else if (225 < nAngle && nAngle < 315) {
		ptRet.x	= rcBound.left;
		ptRet.y	= ptCentre.y + ((rcBound.right - ptCentre.x) * tan(ConvertToRadians(90 - nAngle)));
	}

	return ptRet;
}
#endif	// _Windows

void
oofGeoSector::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	int DrawHo,DrawVe,offh=0,offv=0;
	inTransformationManagerPtr->TransformHoLength
					(mIdealDiameter,DrawHo);
	inTransformationManagerPtr->TransformVeLength
					(mIdealDiameter,DrawVe);
	if(DrawHo>DrawVe) {
		mDrawDiameter=DrawVe;
		offh=(DrawHo-DrawVe)/2;
	} else {
		mDrawDiameter=DrawHo;
		offv=(DrawVe-DrawHo)/2;
	}
	inTransformationManagerPtr->TransformPoint
					(mXIdeal,mYIdeal,mXDraw,mYDraw);
	mXDraw+=offh;
	mYDraw-=offv;
}


void
oofGeoSector::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealDiameter : " << mIdealDiameter
     << "mDrawDiameter : " << mDrawDiameter 
    << "mStartAngle : " << mStartAngle
    << "mArcAngle :" << mArcAngle  << endl ;
}


oofString
oofGeoSector::geoType() const
{
	return "oofGeoSector";
}


// ---------------------------------------------------------------------------------
//		 oofGeoText
// ---------------------------------------------------------------------------------


oofGeoText::oofGeoText(int XIdeal, int YIdeal, int inWidth, 
					   const char* inString, oofSeries* SeriesPtr, PositionState pos) :
	oofGeo (XIdeal,YIdeal,SeriesPtr,pos),
	mText(inString),
	mIdealTextSize(12),
	mWidthIdeal(inWidth),
	mDrawHeight(0),
	mDrawWidth(0)
{
}


oofGeoText::~oofGeoText()
{
}


void
oofGeoText::DrawSelf(const oofDrawStyle* inStyle)
{
	int	charHeight	= calcCharHeight();
// ES
//	int	descent;
	
	setFontSize();
	inStyle->useStyleColors();
	unsigned int numrows=mSizer.GetNumRows();
	const unsigned long * lines = mSizer.GetLineStarts();
	const unsigned long * lengths = mSizer.GetLineLengths();
	short bytes;
	int xmod=0;
	// We have to calculate the descent of the characters for positioning
#ifdef _Macintosh
	FontInfo fi;
	::GetFontInfo(&fi);
	int descent	= fi.descent;
#elif defined _Windows
	HDC	hdc = oofCurrentDraw::getCurrentDC();
// ES
//	TEXTMETRIC	tm;
//	::GetTextMetrics(hdc, &tm);
//	descent	= tm.tmDescent;
	const LPCSTR	pszString	= mText.chars();		// Get a pointer to the string we're going to display
#endif // OS
	for(unsigned int rownum=0; rownum < numrows; rownum++) {
		switch (mPosition) {
			case (posLeft) :
				xmod = 0;
				break;
				
			case (posCentre) :
				xmod = (mDrawWidth-lengths[rownum])/2;
				break;
				
			case (posRight) :
				xmod = mDrawWidth-lengths[rownum];
				break;
				
			default:
				xmod = (mDrawWidth-lengths[rownum])/2; // default to centred text
				break;
		}
		if(rownum==(numrows-1))		
			bytes=mText.length()-lines[rownum];
		else
			bytes=lines[rownum+1]-lines[rownum];
#ifdef _Macintosh
		::MoveTo(mXDraw+xmod, mYDraw+(rownum-numrows+1)*charHeight-descent);
		::DrawText(mText,lines[rownum],bytes);
#elif defined _Windows
		::TextOut(hdc, 
					mXDraw+xmod, 
					mYDraw+(rownum-numrows)*charHeight, 
					pszString + lines[rownum],
					bytes);
#endif // OS
	}
	// can text have a pattern?
}


void
oofGeoText::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	int	charHeight;	

	inTransformationManagerPtr->TransformTextSize(mIdealTextSize,mDrawTextSize);
	inTransformationManagerPtr->TransformHoLength(mWidthIdeal,mDrawWidth);

	if (!(charHeight = calcCharHeight()))
		return;
	setFontSize();
	mSizer.CalculateWrapping(mText, mText.length(), mDrawWidth);
	
	mDrawHeight=mSizer.GetNumRows()*charHeight;
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,mYDraw);
}

oofRect
oofGeoText::BoundingRect(oofTransformationManager* tm)
{
	// OK - This is awful - but we're going to return real pixel coords for
	//      the moment - until I get organised and make two functions to do
	//      this.
	Transform(tm);
	
	oofRect	r(0, 0, mDrawWidth, mDrawHeight);
	return r;
}


void
oofGeoText::describe(ostream& os) const
{
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mText : " << mText
     << "mIdealTextSize : " << mIdealTextSize 
    << "mWidthIdeal : " << mWidthIdeal
    << "mDrawTextSize :" << mDrawTextSize 
   	<< "mDrawWidth :" << mDrawWidth
    << "mDrawHeight :" << mDrawHeight << endl ;
}


oofString
oofGeoText::geoType() const
{
	return "oofGeoText";
}


// ---------------------------------------------------------------------------------
//		 oofGeoSuppressibleText
// ---------------------------------------------------------------------------------


oofGeoSuppressibleText::oofGeoSuppressibleText(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoText(XIdeal,YIdeal,Width,inString,SeriesPtr,pos)
{
}


oofGeoSuppressibleText::oofGeoSuppressibleText(int Width, const char* inString)
	: oofGeoText(0,0,Width,inString,0,posTop)
{
}


void
oofGeoSuppressibleText::DrawSelf(const oofDrawStyle* inStyle)
{
	if (ShouldDraw()) 
		oofGeoText::DrawSelf(inStyle);
}


oofRect
oofGeoSuppressibleText::BoundingRect(oofTransformationManager* tm)
{
	oofRect r;
	if (ShouldDraw()) 
		r = oofGeoText::BoundingRect(tm);
	else
		r.set(0,0,0,0);	
	return r;
}


void
oofGeoSuppressibleText::describe(ostream& os) const
{
    oofGeoText::describe(os);      // call the DIRECT parent, not always oofGeo
    os << endl ;
}


oofString
oofGeoSuppressibleText::geoType() const
{
	return "oofGeoSuppressibleText";
}


// ---------------------------------------------------------------------------------
//		 oofGeoTitle
// ---------------------------------------------------------------------------------


oofGeoTitle::oofGeoTitle(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoSuppressibleText(XIdeal,YIdeal,Width,inString,SeriesPtr,pos)
{
	mIdealTextSize = 18;
}

oofGeoTitle::oofGeoTitle(int Width, const char* inString)
	: oofGeoSuppressibleText(0,0,Width,inString,0,posTop)
{
	mIdealTextSize = 18;
}


bool
oofGeoTitle::ShouldDraw() const
{
	bool ret = oofGraph::currentGraph()->settings()->hasTitle();
	return ret;
}


/* UnJames
oofRect
oofGeoTitle::GetAlreadyTransformedBoundingRect(oofTransformationManager* tm)
{
	oofRect r;
	if (oofGraph::currentGraph()->settings()->hasTitle()) 
		r = oofGeoText::GetAlreadyTransformedBoundingRect(tm);
	else
		r.set(0,0,0,0);	
	return r;
}
*/

void
oofGeoTitle::Transform( oofTransformationManager* inTransformationManagerPtr)
{
	if (!oofGraph::currentGraph()->settings()->hasTitle()) 
		return;	// save the effort if not drawing Title
		
	int	charHeight;	
	mPosition		= oofGraph::currentGraph()->settings()->titlePos();
	// Text object sizes are constant
	mDrawTextSize	= mIdealTextSize;
	inTransformationManagerPtr->TransformHoLength(mWidthIdeal,mDrawWidth);
	if (!(charHeight = calcCharHeight()))
		return;
	setFontSize();
	// Save the old sizes first
	int saveHeight	= mDrawHeight;
// UnJames		oofRect saveBound = GetAlreadyTransformedBoundingRect(inTransformationManagerPtr); 
	mSizer.CalculateWrapping(mText, mText.length(), mDrawWidth);
	mDrawHeight		= mSizer.GetNumRows() * charHeight;
	// OK - we may have changed height here !
	oofRect M = inTransformationManagerPtr->getMargins();
	if(saveHeight != mDrawHeight) {
		M.top	+= (mDrawHeight - saveHeight);
		inTransformationManagerPtr->setMargins(M);
		mYIdeal	+= (mDrawHeight - saveHeight);
	}
	//	For now - we just lock this text to the top of the window
	int dx,dy, dummy;
	oofRect I=inTransformationManagerPtr->getIdealRect();
	inTransformationManagerPtr->TransformPoint(0,I.top,dx,dy);
	mYDraw=dy-2;
	//mYDraw=mYIdeal;
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,dummy);
}


oofString
oofGeoTitle::geoType() const
{
	return "oofGeoTitle";
}


// ---------------------------------------------------------------------------------
//		 oofGeoXLabel
// ---------------------------------------------------------------------------------


oofGeoXLabel::oofGeoXLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoSuppressibleText(XIdeal,YIdeal,Width,inString, SeriesPtr, pos)
{
}

oofGeoXLabel::oofGeoXLabel(int Width, const char* inString)
	: oofGeoSuppressibleText(0,0,Width,inString, 0, posBottom)
{
}


bool
oofGeoXLabel::ShouldDraw() const
{
	bool ret = oofGraph::currentGraph()->settings()->hasXLabel();
	return ret;
}


void
oofGeoXLabel::Transform( oofTransformationManager* inTransformationManagerPtr)
{
	if (!oofGraph::currentGraph()->settings()->hasXLabel()) 
		return;	// save the effort if not drawing
		
	int	charHeight;
	
	// Text object sizes are constant
	mPosition		= oofGraph::currentGraph()->settings()->XLabelPos();
	mDrawTextSize	= mIdealTextSize;
	inTransformationManagerPtr->TransformHoLength(mWidthIdeal,mDrawWidth);
	if (!(charHeight = calcCharHeight()))
		return;
	setFontSize();
	mSizer.CalculateWrapping(mText, mText.length(), mDrawWidth);
	int saveHeight	= mDrawHeight;
	mDrawHeight		= mSizer.GetNumRows() * charHeight;
	oofRect M=inTransformationManagerPtr->getMargins();
	// OK - we may have changed height here !
	if(saveHeight != mDrawHeight) {
		// Work out what the hell to do ! - Remember - we are below the tick labels !
		M.bottom	+= (mDrawHeight - saveHeight);
		inTransformationManagerPtr->setMargins(M);
	}
//	For now - we just lock this text to the bottom of the window
	int dx,dy, dummy;
	oofRect I=inTransformationManagerPtr->getIdealRect();
	inTransformationManagerPtr->TransformPoint(0,I.bottom,dx,dy);
	mYDraw=dy+M.bottom-2;
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,dummy);
}


oofString
oofGeoXLabel::geoType() const
{
	return "oofGeoXLabel";
}


// ---------------------------------------------------------------------------------
//		 oofGeoYLabel
// ---------------------------------------------------------------------------------


oofGeoYLabel::oofGeoYLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoSuppressibleText(XIdeal,YIdeal,Width,inString, SeriesPtr, pos)
{
}


oofGeoYLabel::oofGeoYLabel(const char* inString)
	: oofGeoSuppressibleText(0,0,0,inString, 0, posLeft)
{
}


bool
oofGeoYLabel::ShouldDraw() const
{
	bool ret = oofGraph::currentGraph()->settings()->hasYLabel();
	return ret;
}


void
oofGeoYLabel::Transform( oofTransformationManager* inTransformationManagerPtr)
{
	if (!oofGraph::currentGraph()->settings()->hasYLabel()) 
		return;	// save the effort if not drawing
		
	// All of this stuff has been moved from the constructor because the 
	// MS-Windows version needs an active DC to measure the fonts & this is
	// not likely during object construction.

	// All Text Objects are constant
	mPosition		= oofGraph::currentGraph()->settings()->YLabelPos();
	mDrawTextSize	= mIdealTextSize;
	int charHeight	= calcCharHeight();
	setFontSize();

	mSizer.CalculateWrapping(mText, mText.length(), 
		oofGraph::currentGraph()->settings()->maxYLabelLen());
	const unsigned long* lineLengths=mSizer.GetLineLengths();
	unsigned short maxLen=0;
	int saveWidth	= mDrawWidth;
	for(unsigned short i=0;i<mSizer.GetNumRows();i++) {
		if(lineLengths[i]>maxLen)
			maxLen=lineLengths[i];
	}
	mWidthIdeal	= maxLen;
	mDrawWidth	= mWidthIdeal;
	mDrawHeight	= mSizer.GetNumRows()*charHeight;
	
	oofRect rcMargins	= inTransformationManagerPtr->getMargins();
	// OK - we may have changed width here !
	if(saveWidth != mDrawWidth) {
		// Work out what the hell to do ! - Remember - we are below the tick labels !
// ES
//		rcMargins.left	+= (mDrawWidth - saveWidth);
// ES - just a hard-coded value for now
		rcMargins.left += 5;
		inTransformationManagerPtr->setMargins(rcMargins);
	}
//	For now - we just lock this text to the left of the window
	int dx,dy;
	oofRect rcIdeal	= inTransformationManagerPtr->getIdealRect();
	inTransformationManagerPtr->TransformPoint(rcIdeal.left,0,dx,dy);
	mXDraw = dx - rcMargins.left;
	int dummy;
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,dummy,mYDraw);
}


oofString
oofGeoYLabel::geoType() const
{
	return "oofGeoYLabel";
}


// ---------------------------------------------------------------------------------
//		 oofGeoXTickLabel
// ---------------------------------------------------------------------------------


oofGeoXTickLabel::oofGeoXTickLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoText(XIdeal,YIdeal,Width,inString, SeriesPtr, pos)
{
	mIdealTextSize = 10;
}


oofGeoXTickLabel::oofGeoXTickLabel(int XIdeal, int Width, const char* inString)
	: oofGeoText(XIdeal,0,Width,inString, 0, posBelow) // No series pointer !
{
	mIdealTextSize = 10;
}


void
oofGeoXTickLabel::DrawSelf(const oofDrawStyle* inStyle)
{
	if (oofGraph::currentGraph()->settings()->hasXTickLabels()) {
		oofGeoText::DrawSelf(inStyle);
	}
}


void
oofGeoXTickLabel::Transform( oofTransformationManager* inTransformationManagerPtr)
{
	int	charHeight;
	
	if (oofGraph::currentGraph()->settings()->hasXTickLabels()) {
		// All Text Object sizes are constant
		mDrawTextSize	= mIdealTextSize;
		inTransformationManagerPtr->TransformHoLength(mWidthIdeal,mDrawWidth);
		
		if (!(charHeight = calcCharHeight()))
			return;
		setFontSize();
		mSizer.CalculateWrapping(mText, mText.length(), mDrawWidth);
		int saveHeight	= mDrawHeight;
		mDrawHeight		= mSizer.GetNumRows() * charHeight;
		// OK - we may have changed height here !
		Point P=inTransformationManagerPtr->getTickMargins();
		if(saveHeight != mDrawHeight) {
			// Work out what the hell to do ! - Remember - we are above the axis label !
			if(saveHeight < mDrawHeight) {
				if(P.v<mDrawHeight) {
					P.v=mDrawHeight;
					inTransformationManagerPtr->setTickMargins(P);
				}
			} 
			else {
				if(P.v>mDrawHeight) {
					P.v=mDrawHeight;
					inTransformationManagerPtr->setTickMargins(P);
				}
			}
		}
		// In case we didn't change height - make sure we fit !
		if(P.v<mDrawHeight) {
			P.v=mDrawHeight;
			inTransformationManagerPtr->setTickMargins(P);
		}
		//	For now - we just lock this text to the bottom of the window
		int dx,dy, dummy;
		oofRect I=inTransformationManagerPtr->getIdealRect();
		inTransformationManagerPtr->TransformPoint(0,I.bottom,dx,dy);
		mYDraw=dy + mDrawHeight + oofGraph::currentGraph()->settings()->minorTickSize();
		inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,dummy);
	}
}


oofString
oofGeoXTickLabel::geoType() const
{
	return "oofGeoXTickLabel";
}



// ---------------------------------------------------------------------------------
//		 oofGeoYTickLabel
// ---------------------------------------------------------------------------------


oofGeoYTickLabel::oofGeoYTickLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState pos)
	: oofGeoText(XIdeal,YIdeal,Width,inString, SeriesPtr, pos)
{
	mIdealTextSize = 10;
}


oofGeoYTickLabel::oofGeoYTickLabel(int YIdeal, int Width, const char* inString)
	: oofGeoText(0,YIdeal,Width,inString, 0, posBelow) // No series pointer !
{
	mIdealTextSize = 10;
}


void
oofGeoYTickLabel::DrawSelf(const oofDrawStyle* inStyle)
{
	if (oofGraph::currentGraph()->settings()->hasYTickLabels()) {
		oofGeoText::DrawSelf(inStyle);
	}
}


void
oofGeoYTickLabel::Transform( oofTransformationManager* inTransformationManagerPtr)
{
	int	charHeight;
	
	if (oofGraph::currentGraph()->settings()->hasYTickLabels()) {
		// All Text Object sizes are constant
		mDrawTextSize	= mIdealTextSize;
		if (!(charHeight = calcCharHeight()))
			return;
		setFontSize();
		mSizer.CalculateWrapping(mText, mText.length(), mWidthIdeal);
		const unsigned long* lineLengths=mSizer.GetLineLengths();
		unsigned short maxLen=0;
		for(unsigned short i=0;i<mSizer.GetNumRows();i++) {
			if(lineLengths[i]>maxLen)
				maxLen=lineLengths[i];
		}
		mWidthIdeal	= maxLen;
		mDrawWidth	= mWidthIdeal;
		mDrawHeight	= mSizer.GetNumRows() * charHeight;
		
		Point ptMargins	= inTransformationManagerPtr->getTickMargins();
		if (ptMargins.h < mDrawWidth) {
			ptMargins.h = mDrawWidth;
			inTransformationManagerPtr->setTickMargins(ptMargins);
		}
		int dummy;
		int dx;
		oofRect rcIdeal	= inTransformationManagerPtr->getIdealRect();
		inTransformationManagerPtr->TransformPoint(rcIdeal.left,0,dx,dummy);//dg to me does this just return 0 always?
		mXDraw=dx-ptMargins.h;
		inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,dummy,mYDraw);
		mYDraw+=mDrawHeight/2;
	}
}


oofString
oofGeoYTickLabel::geoType() const
{
	return "oofGeoYTickLabel";
}


// ---------------------------------------------------------------------------------
//		 oofGeoXAxis
// ---------------------------------------------------------------------------------


oofGeoXAxis::oofGeoXAxis(int inLength, int MaInterval, int NumDivisions,
			oofSeries* SeriesPtr, PositionState pos)
	: oofGeo(0,0,SeriesPtr,pos)
{
	mIdealLength = inLength;
	mIdealMaTickMark = MaInterval;
	mNumDivisions = NumDivisions;
}


void
oofGeoXAxis::DrawSelf(const oofDrawStyle* inStyle)
{
	inStyle->useStyleColors();
#ifdef _Macintosh
	::MoveTo(mXDraw,mYDraw);
	::Line(mDrawLength,0);
#elif defined _Windows
	HDC	hdc = oofCurrentDraw::getCurrentDC();
	
	if (hdc) {
		::MoveToEx(hdc, mXDraw, mYDraw, NULL);
		::LineTo(hdc, mXDraw + mDrawLength, mYDraw);
	}
#endif	// OS

	if (oofGraph::currentGraph()->settings()->hasXAxisTicks()) {
		// Let's do this in double domain to avoid rounding errors.
		double MaMeasure       = 0.0;
		double MiMeasure       = 0.0;
		if(mNumDivisions)
			MiMeasure          = mDrawMaTickMark / (double)mNumDivisions;
			
		unsigned short minorTickSize = oofGraph::currentGraph()->settings()->minorTickSize();
		unsigned short majorTickSize = oofGraph::currentGraph()->settings()->majorTickSize();
		while (MaMeasure < mDrawLength) {
			int	x = (int)(MaMeasure + 0.5) + mXDraw;
#ifdef _Macintosh
			::MoveTo(x,mYDraw);
			::Line(0, majorTickSize);
#elif defined _Windows
			if (hdc) {
				::MoveToEx(hdc, x, mYDraw, NULL);
				::LineTo(hdc, x, mYDraw + majorTickSize);
			}
#endif	// OS
			if(mNumDivisions){
				unsigned theNumDivisions = mNumDivisions;
				if((int)(MaMeasure+mDrawMaTickMark+0.5)>mDrawLength)
					theNumDivisions=(int)(((double)mDrawLength-MaMeasure)/MiMeasure)+1;
				for(unsigned i=1;i<theNumDivisions;i++) {
					x = (int)(MaMeasure + i * MiMeasure + 0.5) + mXDraw;
#ifdef _Macintosh
					::MoveTo(x,mYDraw);
					::Line(0, minorTickSize);
#elif defined _Windows
					if (hdc) {
						::MoveToEx(hdc, x, mYDraw, NULL);
						::LineTo(hdc, x, mYDraw + minorTickSize);
					}
#endif	// OS
				}
			}
			MaMeasure += mDrawMaTickMark ;//-1; // Don't ask about the -1 please :)
		}
	}
}


oofRect
oofGeoXAxis::BoundingRect(oofTransformationManager* tm)
{	
	oofRect r;
	
	int IdealMajTickSize;
	
	tm->InverseTransformVeLength(
		oofGraph::currentGraph()->settings()->majorTickSize(),
		IdealMajTickSize
	);
	if (!IdealMajTickSize)
		IdealMajTickSize = 1; // temporary
	
	r.left=mXIdeal;
	r.right=mXIdeal+mIdealLength;
	
	r.top=mYIdeal;
	r.bottom=mYIdeal-IdealMajTickSize;
	
	return r;
}


void
oofGeoXAxis::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,mYDraw);
	inTransformationManagerPtr->TransformHoLength(mIdealLength,mDrawLength);
	inTransformationManagerPtr->TransformHoLength(mIdealMaTickMark,mDrawMaTickMark);
}


void
oofGeoXAxis::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealLength : " << mIdealLength
     << "mDrawLength : " << mDrawLength 
    << "mIdealMaTickMark : " << mIdealMaTickMark
    << "mNumDivisions :" << mNumDivisions 
   	<< "mDrawMaTickMark :" << mDrawMaTickMark << endl ;
}


oofString
oofGeoXAxis::geoType() const
{
	return "oofGeoXAxis";
}


// ---------------------------------------------------------------------------------
//		 oofGeoYAxis
// ---------------------------------------------------------------------------------


oofGeoYAxis::oofGeoYAxis(int inLength, int MaInterval, int NumDivisions,
			oofSeries* SeriesPtr, PositionState pos)
	: oofGeo(0,0,SeriesPtr,pos)
{
	mIdealLength = inLength;
	mIdealMaTickMark = MaInterval;
	mNumDivisions = NumDivisions;
}


void
oofGeoYAxis::DrawSelf(const oofDrawStyle* inStyle)
{
	inStyle->useStyleColors();
#ifdef _Macintosh
	::MoveTo(mXDraw,mYDraw);
	::Line(0,-mDrawLength);
#elif defined _Windows
	HDC	hdc = oofCurrentDraw::getCurrentDC();

	if (hdc) {
		::MoveToEx(hdc, mXDraw, mYDraw, NULL);
		::LineTo(hdc, mXDraw, mYDraw - mDrawLength);
	}
#endif	// OS
	
	if (oofGraph::currentGraph()->settings()->hasYAxisTicks()) {
		// Let's do this in double domain to avoid rounding errors.
		double MaMeasure       = 0.0;
		double MiMeasure       = 0.0;
		if(mNumDivisions)
			MiMeasure          = mDrawMaTickMark / (double)mNumDivisions;
			
		unsigned short minorTickSize = oofGraph::currentGraph()->settings()->minorTickSize();
		unsigned short majorTickSize = oofGraph::currentGraph()->settings()->majorTickSize();
		while (MaMeasure < mDrawLength) {
#ifdef _Macintosh
			::MoveTo(mXDraw,mYDraw-(int)(MaMeasure+0.5));
			::Line(-majorTickSize,0);
#elif defined _Windows
			if (hdc) {
				int	y = mYDraw - (int)(MaMeasure + 0.5);
				::MoveToEx(hdc, mXDraw, y, NULL);
				::LineTo(hdc, mXDraw - majorTickSize, y);
			}
#endif	// OS
			if(mNumDivisions) {
				unsigned theNumDivisions=mNumDivisions;
				if((int)(MaMeasure+mDrawMaTickMark+0.5)>mDrawLength)
					theNumDivisions=(int)(((double)mDrawLength-MaMeasure)/MiMeasure)+1;
				for(unsigned i=1;i<theNumDivisions;i++) {
#ifdef _Macintosh
					::MoveTo(mXDraw,mYDraw-(int)(MaMeasure+i*MiMeasure));
					::Line(-minorTickSize,0);
#elif defined _Windows
					if (hdc) {
						int y = mYDraw - (int)(MaMeasure + i * MiMeasure);
						::MoveToEx(hdc, mXDraw, y, NULL);
						::LineTo(hdc, mXDraw - minorTickSize, y);
					}
#endif	// OS
				}
			}
			MaMeasure += mDrawMaTickMark ;// -1; // See XAxisobject
		}
	}
}


oofRect
oofGeoYAxis::BoundingRect(oofTransformationManager* tm)
{	
	oofRect r;
	
	int IdealMajTickSize;
	
	tm->InverseTransformHoLength(oofGraph::currentGraph()->settings()->majorTickSize(),IdealMajTickSize);
	if (!IdealMajTickSize)
		IdealMajTickSize = 1; // temporary
	
	r.left=mXIdeal-IdealMajTickSize;
	r.right=mXIdeal;
	r.top=mYIdeal+mIdealLength;
	r.bottom=mYIdeal;
	
	return r;
}


void
oofGeoYAxis::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,mXDraw,mYDraw);
	inTransformationManagerPtr->TransformVeLength(mIdealLength,mDrawLength);
	inTransformationManagerPtr->TransformVeLength(mIdealMaTickMark,mDrawMaTickMark);
}


void
oofGeoYAxis::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealLength : " << mIdealLength
     << "mDrawLength : " << mDrawLength 
    << "mIdealMaTickMark : " << mIdealMaTickMark
    << "mNumDivisions :" << mNumDivisions 
   	<< "mDrawMaTickMark :" << mDrawMaTickMark << endl ;
}


oofString
oofGeoYAxis::geoType() const
{
	return "oofGeoYAxis";
}


// ---------------------------------------------------------------------------------
//		 oofGeoLegend
// ---------------------------------------------------------------------------------


oofGeoLegend::oofGeoLegend(oofSeriesSet *oofSeriesSet, bool useSymbols,
					oofSeries *SeriesPtr, PositionState pos)
	: oofGeo(0,0,SeriesPtr,pos),
	  mSeriesSet(oofSeriesSet),
	  mSymbols(useSymbols)
{
	// MS-Windows platform notes:
	// All this work in the constructor relies on the parent oofGraph
	// being current and the device context set as the current context.
	// If this class is used outside the oofGraph framework, it will crash!

	mSizers = new oofTextSizer[mSeriesSet->count()];
	int	charHeight	= oofTextDrawer::calcCharHeight();
	const unsigned long* lineLengths;
	oofTextDrawer::setFontSize();
	unsigned short maxLen=0;
	unsigned int n=0;
	// see also DrawSelf - calculates height again in case font has changed
	mLegendHeight=4;		// Two two pisel spacers 
	for(oofSeriesSetItem *theItem=mSeriesSet->first();theItem;theItem=theItem->next())
	{
		mSizers[n].CalculateWrapping(
			theItem->item()->Name(),
			strlen(theItem->item()->Name()), 
			oofGraph::currentGraph()->settings()->maxLegendLen()
		);
		// accumulate legend width
		lineLengths=mSizers[n].GetLineLengths();
		for(unsigned short i=0;i<mSizers[n].GetNumRows();i++) {
			if(lineLengths[i]>maxLen)
				maxLen=lineLengths[i];
		}
		// accumulate legend height
		mLegendHeight+=mSizers[n].GetNumRows() * charHeight;
		n++;
	}
	mLegendWidth = maxLen + 8 + 6;	// plus symbol width - hardwired for now & Plus three 2 pixel spacers 
	mLegendWidth += 4;  // Andy add extra 4 pixels on end to avoid being too close to end of text
}


oofGeoLegend::~oofGeoLegend()
{
	delete [] mSizers;
}


void
oofGeoLegend::DrawSelf(const oofDrawStyle* inStyle)
{
	if (!oofGraph::currentGraph()->settings()->hasLegend())
		return;
		
	inStyle->useStyleColors();
	
	// if we are required by the graph's settings to draw a legend
	int	charHeight	= oofTextDrawer::calcCharHeight();
	int	charDescent;
	short YTextLoc;

// draw the enclosing frame, after recalculating legend height
// the problem is that the font when we construct this object is probably not the one we
// draw with. Whilst we can change the font size at construction time, we don't know the
// total font height. If that font has different leading to the current font then we
// will have a subtle total difference in frame height, probably 1 pixel per item in the legend
// NASTY HARD-CODED HACK same as in ctor
	mLegendHeight=4;		// Two two pisel spacers. RESET height to accumulate below
	unsigned short numSizers = mSeriesSet->count();
	for(unsigned short i=0; i<numSizers; i++)
	{
		unsigned short numrows=mSizers[i].GetNumRows();
		unsigned short blockHeight = charHeight * numrows;
		mLegendHeight += blockHeight;
	}
	oofTextDrawer::setFontSize();
	oofRect	r(mXDraw,
				mYDraw - mLegendHeight / 2,
				mXDraw + mLegendWidth,
				mYDraw + mLegendHeight / 2);

#ifdef _Macintosh
	FontInfo fi;
	::GetFontInfo(&fi);
	charDescent = fi.descent;

	::FrameRect(r); 
	
	YTextLoc	= mYDraw - mLegendHeight / 2 + charHeight + 2;	// spacer 
#elif defined _Windows
	HDC			hdc = oofCurrentDraw::getCurrentDC();
	HBRUSH		hOldBr = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
	TEXTMETRIC	tm;

	::GetTextMetrics(oofCurrentDraw::getCurrentDC(), &tm);
	charDescent	= tm.tmDescent;

	::Rectangle(hdc, r.left, r.top, r.right, r.bottom);

	YTextLoc	= mYDraw - mLegendHeight / 2 + 2;	// spacer 
#endif	// OS
	unsigned int n=0;
	for(oofSeriesSetItem *theItem=mSeriesSet->first();theItem;theItem=theItem->next())
	{
		unsigned short numrows=mSizers[n].GetNumRows();
		unsigned short blockHeight = charHeight * numrows;
		
		// Draw the symbol or shading centred with the text !
		theItem->item()->drawStyle()->useStyleColors();
		if(theItem->item()->isSymbol())
			theItem->item()->drawStyle()->symbol()->draw(mXDraw+7,
#ifdef _Macintosh
			YTextLoc+blockHeight/2- charHeight
#else
			YTextLoc+blockHeight/2
#endif	// _Macintosh
			);
		else {
#ifdef _Macintosh
			oofRect	s(
				mXDraw + 2,
				YTextLoc + (blockHeight - 8) / 2 - charHeight,		
				mXDraw + 10,
				YTextLoc + (blockHeight - 8) / 2 + 8 - charHeight
			);
			::FillRect(s,theItem->item()->drawStyle()->currentPattern());
			if (!inStyle->isMono())
				::FrameRect(s);
#elif defined _Windows
			::SelectObject(hdc, (HBRUSH)theItem->item()->drawStyle()->currentPattern());
			::Rectangle(
				hdc, 
				mXDraw + 2, 
				YTextLoc + (blockHeight - 8) / 2, 
				mXDraw + 10, 
				YTextLoc + (blockHeight - 8) / 2 + 8)
			;
#endif // OS
		}
		
		// Label the legend item
		inStyle->useStyleColors();
		const unsigned long* lines = mSizers[n].GetLineStarts();
		const unsigned long* lengths = mSizers[n].GetLineLengths();
		short bytes;
		for(short rownum=0;rownum<numrows;rownum++) {
			//int loc=(mLegendWidth-11-lengths[rownum])/2+11; OLD CENTRED TITLES
			if(rownum==(numrows-1))	// last row length is to end	
				bytes=strlen(theItem->item()->Name())-lines[rownum];
			else // row length is from start of this to start of next line
				bytes=lines[rownum+1]-lines[rownum];
#ifdef _Macintosh
			::MoveTo(mXDraw+11, YTextLoc-charDescent);
			::DrawText(theItem->item()->Name(),lines[rownum],bytes);
#elif defined _Windows
			::TextOut(hdc, 
						mXDraw+11, 
						YTextLoc, 
						theItem->item()->Name() + lines[rownum],
						bytes);
#endif // OS
			YTextLoc+=charHeight;
		}
		n++;
	}

#ifdef _Windows
	if (hOldBr)
		::SelectObject(hdc, hOldBr);
#endif	// _Windows
}


oofRect
oofGeoLegend::BoundingRect(oofTransformationManager* tm)
{
	// OK - This is awful - but we're going to return real pixel coords for
	//      the moment - until I get organised and make two functions to do
	//      this.
	Transform(tm);
	oofRect r;
	if (oofGraph::currentGraph()->settings()->hasLegend()) 
		r.set(0,0,mLegendWidth+2+oofGraph::currentGraph()->settings()->legendOffset(),mLegendHeight);
	else
		r.set(0,0,0,0);	
	return r;
}

/* UnJames
oofRect
oofGeoLegend::GetAlreadyTransformedBoundingRect(oofTransformationManager*)
{
	// OK - This is awful - but we're going to return real pixel coords for
	//      the moment - until I get organised and make two functions to do
	//      this.
	oofRect r;
	if (oofGraph::currentGraph()->settings()->hasLegend()) 
		r.set(0,0,mLegendWidth+2,mLegendHeight);
	else
		r.set(0,0,0,0);	
	return r;
}
*/

void
oofGeoLegend::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	int dummy;
	oofRect I=inTransformationManagerPtr->getIdealRect();
	inTransformationManagerPtr->TransformPoint(I.right,0,mXDraw,dummy);
	mXDraw+=2+oofGraph::currentGraph()->settings()->legendOffset();
	inTransformationManagerPtr->TransformPoint(mXIdeal,mYIdeal,dummy,mYDraw);
}


void
oofGeoLegend::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mSymbols : " << mSymbols
    << "mLegendWidth : " << mLegendWidth 
    << "mLegendHeight : " << mLegendHeight << endl ;
}


oofString
oofGeoLegend::geoType() const
{
	return "oofGeoLegend";
}


// ---------------------------------------------------------------------------------
//		 oofGeoCircle
// ---------------------------------------------------------------------------------


oofGeoCircle::oofGeoCircle(int XIdeal, int YIdeal,
			int diameter, oofSeries* SeriesPtr, PositionState pos)
	: oofGeo(XIdeal,YIdeal, SeriesPtr, pos)
{
	mIdealDiameter = diameter;
}


void
oofGeoCircle::DrawSelf(const oofDrawStyle* inStyle)
{
	inStyle->useStyleColors();
	oofRect	theRect(mXDraw, mYDraw, mXDraw + mDrawDiameter, mYDraw+mDrawDiameter);
#ifdef _Macintosh
	::FrameOval(theRect);
#elif defined _Windows
	HDC		hdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
	::Ellipse(hdc, theRect.left, theRect.top, theRect.right, theRect.bottom);
#endif	// OS
}


void
oofGeoCircle::Transform( oofTransformationManager* inTransformationManagerPtr )
{
	int DrawRight,DrawBottom;
	inTransformationManagerPtr->TransformPoint
					(mXIdeal,mYIdeal,mXDraw,mYDraw);
	inTransformationManagerPtr->TransformPoint
					(mXIdeal+mIdealDiameter,mYIdeal+mIdealDiameter,
					DrawRight,DrawBottom);
	mDrawDiameter = DrawRight - mXDraw;
}


void
oofGeoCircle::describe(ostream& os) const
{
    
    oofGeo::describe(os);      // call the DIRECT parent, not always oofGeo
    os << "mIdealDiameter : " << mIdealDiameter
     << "mDrawDiameter : " << mDrawDiameter << endl ;
}


oofString
oofGeoCircle::geoType() const
{
	return "oofGeoCircle";
}

