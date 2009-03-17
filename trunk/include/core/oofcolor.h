#ifndef H_OOFCOLOR
#define	H_OOFCOLOR
//=================================================================================
//	oofcolor.h
//=================================================================================

#ifndef H_OOF0
	#include "oof0.h"
#endif
#ifndef H_OOF2
	#include "oof2.h" // for OOF_ExpandableLongArray
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif

#ifdef _Macintosh
	#ifndef __QUICKDRAW__
		#include <Quickdraw.h>
	#endif
#endif

class oofColor;		// Forward declaration

#ifdef _Windows
/**
	Abstraction for Win32 Pen.
	\ingroup oofRenderWin
*/
class OOFGRAPH_EXPORT oofPen {
private:  /// only create working pen through factory methods
	oofPen(unsigned short penIndex, HPEN);
public:
	oofPen(const oofPen&);
	const oofPen& operator=(const oofPen&);
	~oofPen();
	
	operator const HPEN() const;
	static oofPen createPen(const oofColor&);
	
private:
	unsigned short	mIndex;	
	HPEN	mHPEN;  // copy of shared pointer
};


/**
	Abstraction for Win32 Brush.
	\ingroup oofRenderWin
*/
class OOFGRAPH_EXPORT oofBrush {
private:  // only create working brush through factories
	oofBrush(unsigned short brushIndex, HBRUSH);
public:
	oofBrush(const oofBrush&);
	const oofBrush& operator=(const oofBrush&);
	~oofBrush();
	
	operator const HBRUSH() const;
	// factories to match oofPattern ctors
	static oofBrush createBrush(int);  // actually an oofPattern::ePattern param
	static oofBrush createBrush(const oofColor&);  // implied pattern type
	
private:
	unsigned short	mIndex;	
	HBRUSH	mHBRUSH;  // copy of shared pointer
};
#endif


/**
	Provide cross-platform way to specify patterns.
	\ingroup oofRW
*/
class OOFGRAPH_EXPORT oofPattern {
public:
	enum ePattern { solid, white, black, gray, dkgray, ltgray, ltstripes, stripes, dkstripes, ltdots, dots, dkdots };

	oofPattern(const oofPattern*);
	oofPattern(ePattern);
	oofPattern(const oofColor&);
// uses default copy ctor, dtor

	void extract(ostream&) const;
	
	void	set(ePattern);

#ifdef _Macintosh
	void	set(Pattern);
	const Pattern&	pattern() const;
	operator const Pattern*() const;
	operator const Pattern() const;
#elif defined _Windows
	operator const HBRUSH() const;
#endif	// OS

	ePattern patternType() const;
	void     patternType(ePattern patType);

private:
// data storage
	ePattern mPatType;
#ifdef _Macintosh
	Pattern mPattern;
#elif defined _Windows
	oofBrush	 mPattern;
#endif	// OS

public:
#ifdef _Macintosh
	static Pattern	macPattern(ePattern);
#elif defined _Windows
	static HBRUSH	winPattern(ePattern);
	static HBRUSH	winPattern(const oofColor&);
	static HBRUSH	createWinBrush(ePattern);
#endif	// OS
};


/**
	Cross-platform color specification class.
*/
class OOFGRAPH_EXPORT oofColor {
public:
	enum eColor { 	White, Yellow, DarkYellow, Red, DarkRed, Magenta,
					DarkMagenta, Blue, DarkBlue, Cyan, DarkCyan, Green,
					DarkGreen, Gray, DarkGray, Black };

		oofColor(unsigned int inRed = 0, unsigned int inGreen = 0, unsigned int inBlue = 0);
		oofColor(const oofColor&);
		oofColor(const oofColor*);
		oofColor(const eColor);

	void extract(ostream&) const;
	
	int red() const;
	int green() const;
	int blue() const;

#ifdef _Macintosh
	const RGBColor&	color() const;
	operator const RGBColor*() const;
	operator const RGBColor() const;
#elif defined _Windows
	operator const COLORREF() const;
#endif	// OS

	// Setter functions
#ifdef _Macintosh
	void	set(const RGBColor color);
#elif defined _Windows
	void	set(const COLORREF color);
#endif
	void	set(const eColor color);
	void	set(unsigned int R, unsigned int G, unsigned int B);

private:
	void	SetShortColor(unsigned short, unsigned short, unsigned short);
#ifdef _Macintosh
	RGBColor	mColor;
#elif defined _Windows
	COLORREF	mColor;
#else
	int mRed, mGreen, mBlue;
#endif	// OS
};



// -------------------------------------------------------
//     o o f P a t t e r n
// -------------------------------------------------------

inline oofPattern::ePattern
oofPattern::patternType() const
{
	return mPatType;
}


inline void
oofPattern::patternType(oofPattern::ePattern patType)
{
	set(patType);
}


inline ostream&
operator<<(ostream& os, const oofPattern& P) 
{
	P.extract(os);
	return os;
}


inline ostream&
operator<<(ostream& os, const oofPattern* P) 
{
	P->extract(os);
	return os;
}


// -------------------------------------------------------
//     o o f C o l o r
// -------------------------------------------------------

inline int
oofColor::red() const
{
#ifdef _Macintosh
	return mColor.red;
#elif defined _Windows
	return GetRValue(mColor);
#else
	return 0;  // NOT YET IMPLEMENTED
#endif
}


inline int
oofColor::green() const
{
#ifdef _Macintosh
	return mColor.green;
#elif defined _Windows
	return GetGValue(mColor);
#else
	return 0;  // NOT YET IMPLEMENTED
#endif
}


inline int
oofColor::blue() const
{
#ifdef _Macintosh
	return mColor.blue;
#elif defined _Windows
	return GetBValue(mColor);
#else
	return 0;  // NOT YET IMPLEMENTED
#endif
}


inline ostream&
operator<<(ostream& os, const oofColor& C) 
{
	C.extract(os);
	return os;
}


inline ostream&
operator<<(ostream& os, const oofColor* C) 
{
	C->extract(os);
	return os;
}


#ifdef _Windows
//---------------------------------------------------------------------------------
//	            o o f P e n 
//---------------------------------------------------------------------------------
inline
oofPen::operator const HPEN() const
{
	return mHPEN;
}



//---------------------------------------------------------------------------------
//	            o o f B r u s h
//---------------------------------------------------------------------------------
inline
oofBrush::operator const HBRUSH() const
{
	return mHBRUSH;
}

#endif // _Windows
#endif	// H_OOFCOLOR
