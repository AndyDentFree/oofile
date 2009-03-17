//////////////////////////////////////////////////////////////////
// oofRect.cpp - Implementation for oofRect - platform independent rectangle
//
// COPYRIGHT 1996 A.D. Software, All rights reserved

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif

oofRect::oofRect()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}


oofRect::oofRect(int srcLeft, int srcTop, int srcRight, int srcBottom) 
{
	left = srcLeft;
	top = srcTop;
	right = srcRight;
	bottom = srcBottom;
}


oofRect::oofRect(const oofRect& rhs)
{
	left = rhs.left; 
	top = rhs.top;
	right = rhs.right;
	bottom = rhs.bottom; 
}


#ifdef _Windows
oofRect::oofRect(POINT topLeft, POINT bottomRight)
{
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y; 
}


oofRect::oofRect(POINT point, SIZE size)
{ 
	left = point.x;
	top = point.y;
	right = left + size.cx; 
	bottom = top + size.cy; 
}


oofRect::oofRect(const tagRECT& rhs) :
	tagRECT(rhs)
{ }


oofRect::oofRect(const LPRECT rhs) :
	tagRECT(*rhs)
{ }
#endif


void
oofRect::set(int srcLeft, int srcTop, int srcRight, int srcBottom)
{
	left	= srcLeft; 
	top		= srcTop;
	right	= srcRight;
	bottom	= srcBottom;
}


void
oofRect::setSize(int srcLeft, int srcTop, int width, int height)
{
	left	= srcLeft;
	top		= srcTop;
	right	= srcLeft + width;
	bottom	= srcTop + height;
}

void
oofRect::describe(ostream& os) const
{
    
     os <<  " left: " << left
    << " top: " << top
    << " bottom: " << bottom 
    << " right: " << right
    << endl  ; 
}	

void
oofRect::normalize()
{
	if (left > right) {
		int	temp = right;

		right	= left;
		left	= temp;
	}

	if (top > bottom) {
		int	temp = bottom;

		bottom	= top;
		top		= temp;
	}
}
	
	
void
oofRect::correct()
{
	if (left > right) 
		right	= left + 1;
	if (top > bottom) 
		bottom	= top + 1;
}


void
oofRect::offset(int dx, int dy)
{
	::OffsetRect(this, dx, dy);
}
	
	
void
oofRect::transform(double dx, double dy)
{
	left	*= dx;
	top		*= dy;
	right	*= dx;
	bottom	*= dy;
}
	
#ifdef _Macintosh
oofRect::operator Rect*() 
{
	return this;
}

oofRect::operator const Rect*()
{
	return this;
}

oofRect::operator Rect() 
{
	return *this;
}

oofRect::operator const Rect()
{
	return *this;
}

void
oofRect::operator =(const Rect& rc)
{
	left	= rc.left;
	top		= rc.top;
	right	= rc.right;
	bottom	= rc.bottom;
}

#elif defined _Windows

oofRect::operator LPRECT()
{
	return this;
}	

oofRect::operator LPCRECT() const
{
	return this;
}	

void
oofRect::operator =(const RECT rc)
{
	::CopyRect(this, &rc);
}

#endif	// OS
