//////////////////////////////////////////////////////////////////
// oofRect.h - Definition for oofRect
//
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFRECT
#define H_OOFRECT

#ifndef H_OOF0
	#include "oof0.h"
#endif

#ifdef _Macintosh
	#ifndef __QUICKDRAW__
		#include <Quickdraw.h>
	#endif
#endif

#if !defined(__MWERKS__) && !defined(__MATH__)
	#include <Math.h>
#endif

#ifdef _Macintosh
/**
	Cross-platform way to specify rectangle with Mac-style setter.
	\ingroup oofRW
*/
class oofRect : public Rect
#elif defined _Windows
/**
	Cross-platform way to specify rectangle with Mac-style setter.
	\ingroup oofRW
*/
class OOFGRAPH_EXPORT oofRect : public tagRECT
#endif	// OS
{
public:
	// Constructors
	oofRect();
	oofRect(int left, int top, int right, int bottom);	// WINDOWS order, Mac Rect structure is top, left, bot, right
	oofRect(const oofRect&);
#ifdef _Windows
	oofRect(POINT, POINT);
	oofRect(POINT, SIZE);
	oofRect(const tagRECT&);
	oofRect(const LPRECT);
#endif

	// Attributes
	virtual void 	describe(ostream& os) const;
	void	set(int left, int top, int right, int bottom);
	void	setSize(int left, int top, int width, int height);
	void	normalize();
	void	correct();
	void	offset(int dx, int dy);
	void	transform(double dx, double dy);
	void	transform(double factor)
				{ transform(factor, factor); };

	int		width()
				{ return abs(right - left); };
	int		height()
				{ return abs(bottom - top); };
	int		rawWidth()
				{ return right - left; };
	int		rawHeight()
				{ return bottom - top; };
	int		invWidth()
				{ return left - right; };
	int		invHeight()
				{ return top - bottom; };
	bool	isRectEqual(oofRect * pRect)
// ES - compiler generates "performance warning" for this line (::EqualRect returns int);
// compiler probably generates the same test (and that is the reason for the warning),
// but it should be safer to write it explicitly
//				{ return ::EqualRect(this, pRect); };
				{ return ::EqualRect(this, pRect) == 0 ? false : true; };

	// Platform dependent operators
#ifdef _Macintosh
	operator Rect*();
	operator const Rect*();
	operator Rect();
	operator const Rect();
	void operator =(const Rect&);
#elif defined _Windows
	typedef const RECT* LPCRECT;    // pointer to read/only RECT

	operator LPRECT();
	operator LPCRECT() const;
	void operator =(const RECT);
#endif	// OS
};

#endif	// H_OOFRECT
