// COPYRIGHT 1996 A.D. Software, All rights reserved

// OOFILE basic drawing state classes

#include "oofpch_g.h"  // for precompilation of graph files


#ifndef H_OOF2
	#include "oof2.h" // for OOF_ExpandableLongArray
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#ifndef H_OOFCOLOR
	#include "oofcolor.h"
#endif

//---------------------------------------------------------------------------------
//		o o f P a t t e r n 
//---------------------------------------------------------------------------------

oofPattern::oofPattern(const oofPattern* rhs) :
	mPattern(rhs->mPattern),
	mPatType(rhs->mPatType)
{
}

#ifdef _Macintosh
oofPattern::oofPattern(ePattern inPatType)
{
	set(inPatType);
}


oofPattern::oofPattern(const oofColor& /*inColor*/)  // _Macintosh
{
	set(black);	// Mac differs from windows - our patterns are not coloured
}
#elif defined _Windows
oofPattern::oofPattern(ePattern inPatType) :
	mPatType(inPatType),
	mPattern(oofBrush::createBrush(inPatType))	
{
}


oofPattern::oofPattern(const oofColor& inColor) :
	mPatType(solid),
	mPattern(oofBrush::createBrush(inColor))	
{
}
#endif	// OS


void
oofPattern::set(ePattern inPattern)
{
	mPatType = inPattern;
#ifdef _Macintosh
	mPattern = macPattern(inPattern);
#elif defined _Windows
	mPattern = oofBrush::createBrush(inPattern);
#endif	// OS
}

#ifdef _Macintosh
void
oofPattern::set(Pattern inPattern) // _Macintosh
{
	mPattern = inPattern;
}






#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
Pattern
oofPattern::macPattern(ePattern inPattern)
{
	Pattern theMacPattern;
	switch (inPattern) {
	case white:
		GetQDGlobalsWhite(&theMacPattern);
		break;
	case black:
		GetQDGlobalsBlack(&theMacPattern);
		break;
	case gray:
		GetQDGlobalsGray(&theMacPattern);
		break;
	case dkgray:
		GetQDGlobalsDarkGray(&theMacPattern);
		break;
	case ltgray:
		GetQDGlobalsLightGray(&theMacPattern);
		break;


	case ltstripes:
		GetIndPattern(&theMacPattern, 0, 28);
		break;

	case stripes:
		GetIndPattern(&theMacPattern, 0, 26);
		break;

	case dkstripes:
		GetIndPattern(&theMacPattern, 0, 7);
		break;

	case ltdots:
		GetIndPattern(&theMacPattern, 0, 21);
		break;

	case dots:
		GetIndPattern(&theMacPattern, 0, 10);
		break;

	case dkdots:
		GetIndPattern(&theMacPattern, 0, 9);
		break;


	}
	return theMacPattern;
}
#else	
Pattern
oofPattern::macPattern(ePattern inPattern)
{
	Pattern theMacPattern;
	switch (inPattern) {
	case white:
		theMacPattern = qd.white;
		break;
	case black:
		theMacPattern = qd.black;
		break;
	case gray:
		theMacPattern = qd.gray;
		break;
	case dkgray:
		theMacPattern = qd.dkGray;
		break;
	case ltgray:
		theMacPattern = qd.ltGray;
		break;

	case ltstripes:
		GetIndPattern(&theMacPattern, 0, 28);
		break;

	case stripes:
		GetIndPattern(&theMacPattern, 0, 26);
		break;

	case dkstripes:
		GetIndPattern(&theMacPattern, 0, 7);
		break;

	case ltdots:
		GetIndPattern(&theMacPattern, 0, 21);
		break;

	case dots:
		GetIndPattern(&theMacPattern, 0, 10);
		break;

	case dkdots:
		GetIndPattern(&theMacPattern, 0, 9);
		break;


	}
	return theMacPattern;
}
#endif

oofPattern::operator const Pattern*() const	// casts an oofPattern into a Macintosh 'Pattern'
{
	return &mPattern;
}


oofPattern::operator const Pattern() const
{
	return mPattern;
}

#elif defined _Windows
HBRUSH
oofPattern::winPattern(const oofColor& inColor)
{
	return ::CreateSolidBrush(inColor);
}


HBRUSH
oofPattern::winPattern(ePattern inPattern)
{
	HBRUSH	hbr;

	switch (inPattern) {
	case white:
		hbr = (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	case black:
		hbr = (HBRUSH)::GetStockObject(BLACK_BRUSH);
		break;
	case gray:
		hbr = (HBRUSH)::GetStockObject(GRAY_BRUSH);
		break;
	case dkgray:
		hbr = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);
		break;
	case ltgray:
		hbr = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	case ltdots:
	case dots:
	case dkdots:
	case ltstripes:
	case stripes:
	case dkstripes:
		hbr = createWinBrush(inPattern);
		break;
	default:
		hbr = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}
	return hbr;
}


HBRUSH
oofPattern::createWinBrush(ePattern inPattern)
{
	/*
	static WORD wLtDots[]		= { 0xff, 0xbf, 0xff, 0xfb, 0xff, 0xbf, 0xff, 0xfb };
	static WORD wDots[]			= { 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb };
	static WORD wDkDots[]		= { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };
	static WORD wLtStripes[]	= { 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0xfe, 0xfd };
	static WORD wStripes[]		= { 0xbb, 0x77, 0xee, 0xdd, 0xbb, 0x77, 0xee, 0xdd };
	static WORD wDkStripes[]	= { 0x44, 0x88, 0x11, 0x22, 0x44, 0x88, 0x11, 0x22 };
	*/

	static WORD wLtDots[]		= { 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	static WORD wDots[]			= { 0xff, 0xbf, 0xff, 0xfb, 0xff, 0xbf, 0xff, 0xfb };
	static WORD wDkDots[]		= { 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb };
	static WORD wLtStripes[]	= { 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0xfe, 0xfd };
	static WORD wStripes[]		= { 0xbb, 0x77, 0xee, 0xdd, 0xbb, 0x77, 0xee, 0xdd };
	static WORD wDkStripes[]	= { 0x44, 0x88, 0x11, 0x22, 0x44, 0x88, 0x11, 0x22 };

	LPWORD wPattern;

	switch (inPattern) {
	case ltdots:
		wPattern = wLtDots;
		break;
	case dots:
		wPattern = wDots;
		break;
	case dkdots:
		wPattern = wDkDots;
		break;
	case ltstripes:
		wPattern = wLtStripes;
		break;
	case stripes:
		wPattern = wStripes;
		break;
	case dkstripes:
		wPattern = wDkStripes;
		break;
	default:
		return NULL;
	}

	HBITMAP hBmpPattern = ::CreateBitmap(8,8,1,1,wPattern);
	HBRUSH  hBrush = ::CreatePatternBrush(hBmpPattern);

	// pattern brushes use a copy of the bitmap, so the source bitmap
	// may be safely deleted
	::DeleteObject(hBmpPattern);

	return hBrush;
}

oofPattern::operator const HBRUSH() const// casts an oofPattern into a Windows Brush
{
	return mPattern;  // invokes the HBRUSH cast on an oofBrush in turn
}

#endif	// OS


void
oofPattern::extract(ostream& os) const
{
	switch (mPatType) {
	case solid:
		os << "solid";
		break;
	case white:
		os << "white";
		break;
	case black:
		os << "black";
		break;
	case gray:
		os << "gray";
		break;
	case dkgray:
		os << "dkgray";
		break;
	case ltgray:
		os << "ltgray";
		break;
	case ltdots:
		os << "ltdots";
		break;
	case dots:
		os << "dots";
		break;
	case dkdots:
		os << "dkdots";
		break;
	case ltstripes:
		os << "ltstripes";
		break;
	case stripes:
		os << "stripes";
		break;
	case dkstripes:
		os << "dkstripes";
		break;
	default:
		os << "unknown pattern type";
	}
}



//---------------------------------------------------------------------------------
//		o o f C o l o r 
//---------------------------------------------------------------------------------


oofColor::oofColor(unsigned int inRed, unsigned int inGreen, unsigned int inBlue)
{
	set(inRed, inGreen, inBlue);
}


oofColor::oofColor(const oofColor& rhs) 
	: mColor(rhs.mColor)
{
}


oofColor::oofColor(const oofColor* rhs) 
	: mColor(rhs->mColor)
{
}


oofColor::oofColor(const eColor rhs)
{
	set(rhs);
}


#ifdef _Macintosh

const RGBColor&
oofColor::color() const
{
	return mColor;
}


oofColor::operator const RGBColor*() const
{
	return &mColor;
}


oofColor::operator const RGBColor() const
{
	return mColor;
}
#elif defined _Windows

oofColor::operator const COLORREF() const
{
	return mColor;
}

#endif	// OS

void
oofColor::SetShortColor(unsigned short R, unsigned short G, unsigned short B)
{
#ifdef _Macintosh
	mColor.red = R*257;
	mColor.green = G*257;
	mColor.blue = B*257;
#elif defined _Windows
	mColor = RGB(R, G, B);
#else  
	set(R, G, B);
#endif	// OS
}


#ifdef _Macintosh
void
oofColor::set(const RGBColor color)
{
	mColor = color;
}
#elif defined _Windows
void
oofColor::set(const COLORREF color)
{
	mColor = color;
}
#endif


void
oofColor::set( const eColor color )
{
	switch (color) {
	case White:
		SetShortColor(255,255,255);
		break;
	case Yellow:
		SetShortColor(255,255,0);
		break;
	case DarkYellow:
		SetShortColor(128,128,0);
		break;
	case Red:
		SetShortColor(255,0,0);
		break;
	case DarkRed:
		SetShortColor(128,0,0);
		break;
	case Magenta:
		SetShortColor(255,0,255);
		break;
	case DarkMagenta:
		SetShortColor(128,0,128);
		break;
	case Blue:
		SetShortColor(0,0,255);
		break;
	case DarkBlue:
		SetShortColor(0,0,128);
		break;
	case Cyan:
		SetShortColor(0,255,255);
		break;
	case DarkCyan:
		SetShortColor(0,128,128);
		break;
	case Green:
		SetShortColor(0,255,0);
		break;
	case DarkGreen:
		SetShortColor(0,128,0);
		break;
	case Gray:
		SetShortColor(192,192,192);
		break;
	case DarkGray:
		SetShortColor(128,128,128);
		break;
	case Black:
		SetShortColor(0,0,0);
		break;
	default: // make it black
		SetShortColor(0,0,0);
	}
}

void
oofColor::set(unsigned int R, unsigned int G, unsigned int B)
{
#ifdef _Macintosh
	mColor.red = R;
	mColor.green = G;
	mColor.blue = B;
#elif defined _Windows
// ES
//	mColor = RGB(R / 257, G / 257, B / 257);
	mColor = RGB(R,G,B);
#else
	mRed=R;
	mGreen=G;
	mBlue=B;
#endif	// OS
}


void
oofColor::extract(ostream& os) const
{
	os << "(" 
		<< red() << ", "
		<< green() << ", "
		<< blue()
	<< ")";
}


#ifdef _Windows
//---------------------------------------------------------------------------------
//		     O O F _ P e n s D i c t
//---------------------------------------------------------------------------------

/**
	Reference counted Pen resource with oofColor info for searching.
	\note should be nested in OOF_PensDict but some older compilers do not 
	handle nested classes, so keep this def out here.
	\ingroup oofWinSpecific  oofInternals
*/
struct OOF_PenInfo {
	short mRefs;
	oofColor mColor;
	HPEN mHPEN;
};


/**
	Singleton cache of Windows Pen resources allocated.
	Dictionary provides reuse of pens that can be selected into different graphics contexts.
	\ingroup oofWinSpecific  oofInternals
*/
class OOF_PensDict : public oofArray {
public:
	OOF_PensDict();
	virtual ~OOF_PensDict();
	
	static void inc(unsigned short);
	static void dec(unsigned short);

	static unsigned short findOrCreatePen(const oofColor& matchingColor, HPEN& returnPen);

protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	
private:
	static OOF_PenInfo* sPens;  ///< reference to mBits in oofArray
	static OOF_PensDict sPensManager;
};

/// static Singleton storage
OOF_PensDict OOF_PensDict::sPensManager;  
OOF_PenInfo* OOF_PensDict::sPens;  


OOF_PensDict::OOF_PensDict() :
	oofArray(sizeof(OOF_PenInfo), 8) // estimate likely need up to 8 (can expand)
{
	mUserStorage = (void**)&sPens;  // so parent can adjust ours as we expand
}


OOF_PensDict::~OOF_PensDict()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = sPens = 0;  // avoid double-delete by zeroing mBits
}


void* 
OOF_PensDict::AllocBits(unsigned long numBits) const
{
	return new OOF_PenInfo[numBits];
}


void 
OOF_PensDict::DeleteBits(bool /* becauseExpanding */)
{
// we are supposed to know our own data pointer
// this gives us a chance to do a deep delete if !becauseExpanding
	delete[] sPens;
}



/**
	static factory method.
*/
unsigned short
OOF_PensDict::findOrCreatePen(const oofColor& matchingColor, HPEN& returnPen)
{
	const unsigned long numItems = sPensManager.count();
	for (unsigned short i=0; i<numItems; i++) {
		OOF_PenInfo& thePen = sPens[i];
		if (thePen.mColor==matchingColor) {
		// may have been deleted
			if (thePen.mRefs==0)  // free slot with this color - reuse it
  			thePen.mHPEN = ::CreatePen(PS_SOLID, 1, matchingColor);
			thePen.mRefs++;
			returnPen = thePen.mHPEN;
			return i;  //*** RETURN existing or reused slot
		}
	}
	// create Windows Pen
  returnPen =  ::CreatePen(PS_SOLID, 1, matchingColor);
	
	// try to reuse a slot
	for (unsigned short j=0; j<numItems; j++) {
		OOF_PenInfo& thePen = sPens[j];
		if (thePen.mRefs==0) {  // free slot - reuse it
			thePen.mRefs = 1;
			thePen.mHPEN = returnPen;
			thePen.mColor = matchingColor;
			return j;  //*** RETURN reused slot
		}
	}
	// fall through to creating new slot
	unsigned long allocSlot = sPensManager.AllocSlot();
	OOF_PenInfo& thePen = sPens[allocSlot];
	thePen.mRefs = 1;
	thePen.mHPEN = returnPen;
	thePen.mColor = matchingColor;
	return allocSlot;
}


inline void
OOF_PensDict::inc(unsigned short penIndex)
{
	assert(penIndex < sPensManager.count());
	sPens[penIndex].mRefs++;
}


void
OOF_PensDict::dec(unsigned short penIndex)
{
	assert(penIndex < sPensManager.count());
	OOF_PenInfo& thePen = sPens[penIndex];
	const short refs = (--thePen.mRefs);
	if (refs==0) {
	// delete the Windows object
		const bool couldDelete = ::DeleteObject(thePen.mHPEN);
		if (couldDelete)
			thePen.mHPEN = 0;  // useful debugging - can see any failed to del
	// leave the oofColor as is - there is no such thing as an invalid one
	}
}


//---------------------------------------------------------------------------------
//	            o o f P e n 
//---------------------------------------------------------------------------------

oofPen::oofPen(unsigned short penIndex, HPEN inPen) :
	mIndex(penIndex),
	mHPEN(inPen)
{
}


oofPen::oofPen(const oofPen& rhs) :
	mIndex(rhs.mIndex),
	mHPEN(rhs.mHPEN)
{
	OOF_PensDict::inc(mIndex);
}
		

const oofPen&
oofPen::operator=(const oofPen& rhs)
{
	if (*this == rhs)
		return *this;  // trivial assignment to self
		
	OOF_PensDict::dec(mIndex);
	mIndex = rhs.mIndex;
	mHPEN = rhs.mHPEN;
	OOF_PensDict::inc(mIndex);
	return *this;
}	


oofPen::~oofPen()
{
	OOF_PensDict::dec(mIndex);
}


/**
	static factory method.
*/
oofPen
oofPen::createPen(const oofColor& inColor)
{
	HPEN returnPen;
	const unsigned short penIndex = OOF_PensDict::findOrCreatePen(inColor, returnPen);
	return oofPen(penIndex, returnPen);
}

//---------------------------------------------------------------------------------
//		     O O F _ B r u s h e s D i c t
//---------------------------------------------------------------------------------

/**
	Reference counted Brush resource with oofPattern & oofColor info for searching.
	\note should be nested in OOF_BrushesDict but some older compilers do not 
	handle nested classes, so keep this def out here.
	\ingroup oofWinSpecific  oofInternals
*/
struct OOF_BrushInfo {
	short mRefs;
	oofPattern::ePattern mPatType;  // primary determinant
	oofColor mColor;  // only used for solid PatType
	HBRUSH mHBRUSH;
};


/**
	Singleton cache of Windows Brush resources allocated.
	Dictionary provides reuse of brushes that can be selected into different graphics contexts.
	\ingroup oofWinSpecific  oofInternals
*/
class OOF_BrushesDict : public oofArray {
public:
	OOF_BrushesDict();
	virtual ~OOF_BrushesDict();
	
	static void inc(unsigned short);
	static void dec(unsigned short);

	static unsigned short findOrCreateBrush(const oofColor& matchingColor, HBRUSH& returnBrush);
	static unsigned short findOrCreateBrush(oofPattern::ePattern, HBRUSH& returnBrush);

protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	
private:
	static OOF_BrushInfo* sBrushes;  // reference to mBits in oofArray
	static OOF_BrushesDict sBrushesManager;
};

// static Singleton storage
OOF_BrushesDict OOF_BrushesDict::sBrushesManager;  
OOF_BrushInfo* OOF_BrushesDict::sBrushes;  


OOF_BrushesDict::OOF_BrushesDict() :
	oofArray(sizeof(OOF_BrushInfo), 8) // estimate likely need up to 8 (can expand)
{
	mUserStorage = (void**)&sBrushes;  // so parent can adjust ours as we expand
}


OOF_BrushesDict::~OOF_BrushesDict()
{
// could do with check to see all deallocated
	DeleteBits(false);
	mBits = sBrushes = 0;  // avoid double-delete by zeroing mBits
}


void* 
OOF_BrushesDict::AllocBits(unsigned long numBits) const
{
	return new OOF_BrushInfo[numBits];
}


void 
OOF_BrushesDict::DeleteBits(bool /* becauseExpanding */)
{
// we are supposed to know our own data pointer
// this gives us a chance to do a deep delete if !becauseExpanding
	delete[] sBrushes;
}


/**
	static factory method.
*/
unsigned short
OOF_BrushesDict::findOrCreateBrush(const oofColor& matchingColor, HBRUSH& returnBrush)
{
	const unsigned long numItems = sBrushesManager.count();
	for (unsigned short i=0; i<numItems; i++) {
		OOF_BrushInfo& theBrush = sBrushes[i];
		if ( (theBrush.mPatType==oofPattern::solid) && (theBrush.mColor==matchingColor)) {
		// may have been deleted
			if (theBrush.mRefs==0)  // free slot with this color - reuse it
  			theBrush.mHBRUSH = oofPattern::winPattern(matchingColor);
			theBrush.mRefs++;
			returnBrush = theBrush.mHBRUSH;
			return i;  //*** RETURN existing or reused slot
		}
	}
	// create Windows Brush
  returnBrush =  oofPattern::winPattern(matchingColor);
	
	// try to reuse a slot
	for (unsigned short j=0; j<numItems; j++) {
		OOF_BrushInfo& theBrush = sBrushes[j];
		if (theBrush.mRefs==0) {  // free slot - reuse it
			theBrush.mRefs = 1;
			theBrush.mHBRUSH = returnBrush;
			theBrush.mPatType = oofPattern::solid;
			theBrush.mColor = matchingColor;
			return j;  //*** RETURN reused slot
		}
	}
	// fall through to creating new slot
	unsigned long allocSlot = sBrushesManager.AllocSlot();
	OOF_BrushInfo& theBrush = sBrushes[allocSlot];
	theBrush.mRefs = 1;
	theBrush.mHBRUSH = returnBrush;
	theBrush.mPatType = oofPattern::solid;
	theBrush.mColor = matchingColor;
	return allocSlot;
}


unsigned short
OOF_BrushesDict::findOrCreateBrush(oofPattern::ePattern matchingPatType, HBRUSH& returnBrush)
{
// static factory method
	assert(matchingPatType!=oofPattern::solid);  // should call other variant with colour
	
	const unsigned long numItems = sBrushesManager.count();
	for (unsigned short i=0; i<numItems; i++) {
		OOF_BrushInfo& theBrush = sBrushes[i];
		if (theBrush.mPatType==matchingPatType) {
		// may have been deleted
			if (theBrush.mRefs==0)  // free slot with this color - reuse it
  			theBrush.mHBRUSH = oofPattern::winPattern(matchingPatType);
			theBrush.mRefs++;
			returnBrush = theBrush.mHBRUSH;
			return i;  //*** RETURN existing or reused slot
		}
	}
	// create Windows Brush
  returnBrush = oofPattern::winPattern(matchingPatType);
	
	// try to reuse a slot
	for (unsigned short j=0; j<numItems; j++) {
		OOF_BrushInfo& theBrush = sBrushes[j];
		if (theBrush.mRefs==0) {  // free slot - reuse it
			theBrush.mRefs = 1;
			theBrush.mHBRUSH = returnBrush;
			theBrush.mPatType = matchingPatType;
//			theBrush.mColor ignored;
			return j;  //*** RETURN reused slot
		}
	}
	// fall through to creating new slot
	unsigned long allocSlot = sBrushesManager.AllocSlot();
	OOF_BrushInfo& theBrush = sBrushes[allocSlot];
	theBrush.mRefs = 1;
	theBrush.mHBRUSH = returnBrush;
	theBrush.mPatType = matchingPatType;
//	theBrush.mColor; ignored
	return allocSlot;
}


inline void
OOF_BrushesDict::inc(unsigned short penIndex)
{
	assert(penIndex < sBrushesManager.count());
	sBrushes[penIndex].mRefs++;
}


void
OOF_BrushesDict::dec(unsigned short brushIndex)
{
	assert(brushIndex < sBrushesManager.count());
	OOF_BrushInfo& theBrush = sBrushes[brushIndex];
	const short refs = (--theBrush.mRefs);
	if (refs==0) {
	// delete the Windows object
		const bool couldDelete = ::DeleteObject(theBrush.mHBRUSH);
		if (couldDelete)
			theBrush.mHBRUSH = 0;  // useful debugging - can see any failed to del
	// leave the oofColor as is - there is no such thing as an invalid one
	}
}


//---------------------------------------------------------------------------------
//	            o o f B r u s h
//---------------------------------------------------------------------------------

oofBrush::oofBrush(unsigned short penIndex, HBRUSH inBrush) :
	mIndex(penIndex),
	mHBRUSH(inBrush)
{
}


oofBrush::oofBrush(const oofBrush& rhs) :
	mIndex(rhs.mIndex),
	mHBRUSH(rhs.mHBRUSH)
{
	OOF_BrushesDict::inc(mIndex);
}
		
	
const oofBrush&
oofBrush::operator=(const oofBrush& rhs)
{
	if (*this == rhs)
		return *this;  // trivial assignment to self
		
	OOF_BrushesDict::dec(mIndex);
	mIndex = rhs.mIndex;
	mHBRUSH = rhs.mHBRUSH;
	OOF_BrushesDict::inc(mIndex);
	return *this;
}	


oofBrush::~oofBrush()
{
	OOF_BrushesDict::dec(mIndex);
}


/// static factory method
oofBrush
oofBrush::createBrush(int patternType)
{
	HBRUSH returnBrush;
	const unsigned short brushIndex = OOF_BrushesDict::findOrCreateBrush(oofPattern::ePattern(patternType), returnBrush);
	return oofBrush(brushIndex, returnBrush);
}


/// static factory method
oofBrush
oofBrush::createBrush(const oofColor& inColor)
{
	HBRUSH returnBrush;
	const unsigned short brushIndex = OOF_BrushesDict::findOrCreateBrush(inColor, returnBrush);
	return oofBrush(brushIndex, returnBrush);
}

#endif	// _Windows

