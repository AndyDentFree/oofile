// COPYRIGHT 1996 A.D. Software, All rights reserved

//=================================================================================
//	oofDStyl.cp
//=================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFDSTYL
	#include "oofDStyl.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifdef _Windows
	#ifndef H_OOFDRAW
		#include "oofdraw.h"
	#endif
#endif
#include <string.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

const unsigned int symbolWidth = 5;
const unsigned int symbolHeight = 5;
const unsigned int symbolOffsetX = symbolWidth/2;
const unsigned int symbolOffsetY = symbolHeight/2;

//---------------------------------------------------------------------------------
//		* oofDrawStyle
//---------------------------------------------------------------------------------


// static data members:
bool oofDrawStyle::sIsMono = false;

oofDrawStyle::oofDrawStyle(	const oofColor& fg, const oofPattern& colPat,
						const oofPattern& monoPat, const oofColor& bg,
						oofSymbol *sym, bool adoptsSymbol)
						//oofDrawStyle::symbolType sym)
	: mForeColor(fg), mBackColor(bg)
#ifdef _Windows
	,mPen(oofPen::createPen(mForeColor))
#endif	
{
	// These patterns need to be owned in case the pattern reference which
	// is passed in is destroyed.
	mColorPattern = new oofPattern(colPat);
	mMonoPattern = new oofPattern(monoPat);
	if (sym) {
		if (adoptsSymbol)
			mSymbol = sym;	// we now own this pointer, the caller loses it!
		else
			mSymbol = sym->clone();		
	}
	else
		mSymbol = 0;	// zero pointer
}


oofDrawStyle::oofDrawStyle(const oofDrawStyle& rhs) :
	mForeColor(rhs.mForeColor), 
	mBackColor(rhs.mBackColor)
#ifdef _Windows
	,mPen(rhs.mPen)
#endif	
{
	mColorPattern = new oofPattern(rhs.mColorPattern);
	mMonoPattern = new oofPattern(rhs.mMonoPattern);
	mSymbol = rhs.mSymbol->clone();
}

oofDrawStyle::oofDrawStyle(const oofDrawStyle* rhs) :
	mForeColor(rhs->mForeColor), 
	mBackColor(rhs->mBackColor)
#ifdef _Windows
	,mPen(rhs->mPen)
#endif	
{
	mColorPattern = new oofPattern(rhs->mColorPattern);
	mMonoPattern = new oofPattern(rhs->mMonoPattern);
	mSymbol = rhs->mSymbol->clone();
}


oofDrawStyle::~oofDrawStyle()
{
	// Although both pattern pointers are created pointing at different
	// storage, the user is allowed to set them to point at the same storage.
	// Hence we must make a check to avoid a double delete.
	if (mColorPattern!=mMonoPattern) {
		delete mColorPattern;
		delete mMonoPattern;
	}
	else
		delete mColorPattern;
	delete mSymbol;
}


const oofDrawStyle&
oofDrawStyle::operator=(oofDrawStyle& rhs)
{
	mForeColor = rhs.mForeColor;
	mBackColor = rhs.mBackColor;
#ifdef _Windows
	mPen = rhs.mPen;
#endif
	if (mColorPattern!=mMonoPattern) {
		delete mColorPattern;
		delete mMonoPattern;
	}
	else
		delete mColorPattern;	// see destructor for rational for this logic
	mColorPattern = new oofPattern(rhs.mColorPattern);
	mMonoPattern = new oofPattern(rhs.mMonoPattern);
	mSymbol = rhs.mSymbol;
	return *this;
}


void
oofDrawStyle::setPattern(const oofPattern& inPattern)
{
	// This function sets *both the colour pattern *and the mono pattern to the
	// pattern passed in.
	// Since the two patterns may already point at the same piece of storage, a
	// check is performed to avoid a double delete.
	if (mColorPattern!=mMonoPattern) {
		delete mColorPattern;
		delete mMonoPattern;
		mColorPattern = new oofPattern(inPattern);
		mMonoPattern = mColorPattern;
	}
	else {
		delete mColorPattern;
		mColorPattern = new oofPattern(inPattern);
		mMonoPattern = mColorPattern;
	}	
}




void
oofDrawStyle::setJustColorPattern(const oofPattern& inPattern)
{
	if (mColorPattern!=mMonoPattern) {
		delete mColorPattern;
		mColorPattern = new oofPattern(inPattern);
	}
	else
		// We don't delete the storage because mMonoPattern is still using it.
		mColorPattern = new oofPattern(inPattern);
}


void
oofDrawStyle::setJustMonoPattern(const oofPattern& inPattern)
{
	if (mColorPattern!=mMonoPattern) {
		delete mMonoPattern;
		mMonoPattern = new oofPattern(inPattern);
	}
	else
		// We don't delete the storage because mColorPattern is still using it.
		mMonoPattern = new oofPattern(inPattern);
}


const oofColor&	
oofDrawStyle::foreColor() const
{
	return mForeColor;
}


const oofColor&
oofDrawStyle::backColor() const
{
	return mBackColor;
}


const oofPattern&
oofDrawStyle::colorPattern() const
{
	return *mColorPattern;
}


const oofPattern&
oofDrawStyle::monoPattern() const
{
	return *mMonoPattern;
}


const oofPattern&
oofDrawStyle::currentPattern() const
{
	if (isMono())
		return *mMonoPattern;
	else
		return *mColorPattern;
}


void
oofDrawStyle::useStyleColors() const
{
#ifdef _Macintosh
	if (isMono()) {
		RGBColor	black = {0,0,0},
					white = {0xFFFF,0xFFFF,0xFFFF};
		::RGBForeColor(&black);
		::RGBBackColor(&white);
	}
	else {
		::RGBForeColor(foreColor());
		::RGBBackColor(backColor());
	}
#elif defined _Windows
	HDC	hdc = oofCurrentDraw::getCurrentDC();
	if (hdc) {
		if (isMono()) {
			::SetTextColor(hdc, RGB(0, 0, 0));		
			::SetBkColor(hdc, RGB(255, 255, 255));	

			::SelectObject(hdc, ::GetStockObject(BLACK_PEN));	// Include the Pen for outlining
		}
		else {
			::SetTextColor(hdc, foreColor());
			::SetBkColor(hdc, backColor());

			if (mPen)
				::SelectObject(hdc, mPen);						// Include the Pen for outlining
		}
	}
#endif	// OS
}


void
oofDrawStyle::setStyleToMono()
{
	sIsMono = true;
}


void
oofDrawStyle::setStyleToColor()
{
	sIsMono = false;
}


bool
oofDrawStyle::isMono()
{
	return sIsMono;
}

//---------------------------------------------------------------------------------
//		 oofDrawStyleArray
//---------------------------------------------------------------------------------


oofDrawStyleArray::oofDrawStyleArray()
{
}


oofDrawStyleArray::~oofDrawStyleArray()
{
	unsigned long numDrawStyles = mDrawStyles.count();
	// delete all our owned pointers to draw styles
	for (unsigned long index=0; index<numDrawStyles; index++)
		delete (oofDrawStyle*) mDrawStyles.value(index);
}


void
oofDrawStyleArray::append(oofDrawStyle& ds)
{
	// we OWN our styles
	oofDrawStyle *ourDS = new oofDrawStyle(ds);
	mDrawStyles.append((unsigned long)ourDS);
}


oofDrawStyleArray&
oofDrawStyleArray::operator<<(oofDrawStyle *ds)
{
	append(*ds);
	return *this;
}


oofDrawStyleArray&
oofDrawStyleArray::operator<<(oofDrawStyle& ds)
{
	append(ds);
	return *this;
}


const oofDrawStyle&
oofDrawStyleArray::item(unsigned long index) const
{
// the ELArray convention is that 'value' is const, 'item' is a ref
	const oofDrawStyle* theStyle = (const oofDrawStyle*) mDrawStyles.value(index);
	assert(theStyle);
	return *theStyle;
}


/**
	adopts the storage of the array passed in, appending to the end of the current storage.
	thus we work at a lower level than normal - we are just grabbing
	these items directly rather than copying as in our normal append()
*/
void
oofDrawStyleArray::adopt(oofDrawStyleArray* inArray)
{
	OOF_ExpandableLongArray& rhsArray = inArray->mDrawStyles;	// ugly intrusion into rhs
	unsigned long numItems = rhsArray.count();
	for (unsigned long index = 0; index<numItems; index++) {
		mDrawStyles.append(rhsArray.value(index));
	}
	rhsArray.deleteAllCells();	// so other side won't think it still owns the pointers
								// resets count, so its loop to set all to a value does nought!
}


unsigned long
oofDrawStyleArray::count() const
{
	return mDrawStyles.count();
}


//---------------------------------------------------------------------------------
//		 oofDrawStyleSet
//---------------------------------------------------------------------------------


oofDrawStyle	
oofDrawStyleSet::getNextDrawStyle() { ++mNumDrawStylesProduced; return
oofDrawStyle(); };


oofDrawStyle	
oofDrawStyleSetDefaultIteration::getNextDrawStyle()
{
// work out an algorithm for producing nice DrawStyles, based on the count so far
 	oofPattern monoPat(oofPattern::white);
 	oofSymbol* theSymbol;
 	
 	short genIndex = mNumDrawStylesProduced % 8; 	
 	switch (genIndex) {
 	case 0:
 		monoPat.set(oofPattern::ltdots);
 		theSymbol = new oofSquareSymbol;
 		break;
 		
 	case 1:
		monoPat.set(oofPattern::ltstripes);
 		theSymbol = new oofCircleSymbol;
 		break;
 	
 	case 2:
		monoPat.set(oofPattern::dots);
 		theSymbol = new oofCrossSymbol;
 		break;
 		
 	case 3:
		monoPat.set(oofPattern::stripes);
 		theSymbol = new oofEcsSymbol;
 		break;

 	case 4:
		monoPat.set(oofPattern::ltgray);
 		theSymbol = new oofSquareSymbol;
 		break;
 		
 	case 5:
		monoPat.set(oofPattern::dkdots);
 		theSymbol = new oofCircleSymbol;
 		break;
 		
 	case 6:
		monoPat.set(oofPattern::gray);
 		theSymbol = new oofCrossSymbol;
 		break;
 	
  case 7:
		monoPat.set(oofPattern::dkstripes);
 		theSymbol = new oofSquareSymbol;
 		break;
 	
 	case 8:
		monoPat.set(oofPattern::dkgray);
 		theSymbol = new oofCircleSymbol;
 		break;
 	
 	case 9:
		monoPat.set(oofPattern::black);
 		theSymbol = new oofCrossSymbol;
 		break;
 	
	default:
		monoPat.set(oofPattern::white);
 		theSymbol = new oofEcsSymbol;
 		break;
 	
 	};

/*
	if (mColorSet) // or something like this
		// use the colour set's nth item
*/

	

 	oofColor fore = CalculateColor(mNumDrawStylesProduced);
 	oofColor back = oofColor::White;		// by default
 	oofPattern colPat = fore;				// oofPattern::gray;	// by default

	++mNumDrawStylesProduced;	// will start from 0

	return oofDrawStyle(fore,colPat,monoPat,back,theSymbol, true /* adoptsSymbol */);
};


oofColor
oofDrawStyleSetDefaultIteration::CalculateColor(unsigned long index)
{
	typedef const unsigned short indexT;
	typedef unsigned short colorT;
	
	indexT kNumPrimQuanta = 5;	// size of following list
	indexT kNumSecQuanta = 10;	// size of following list

	// we want to start at 0 to force pure R G B to be first out
	// to avoid repeats, we do
	// 0 ... 20000 against radial 0 = 4 combos
	// 65535 ... 20000 against radial 20700 = 3 combos
	// 65535 ... 20000 against radial 37000 = 3 combos
	// 65535 ... 43000 against radial 49000 = 2 combos - NO TOO LIGHT
	const colorT secQuanta[] = {
		0,     65535, 43000, 20000,
		65535, 43000, 20000,
		65535, 43000, 20000
	};
	
	const colorT radialQuanta[] = {
		0,     0,     0,     0,
		20700, 20700, 20700,
		37000, 37000, 37000
	};

	
	const colorT primQuanta[] = {65535, 49000, 37000, 20700, 0};
	indexT kNumPrim = 3;	// 3 primary colours

	
// for each primary number 	
// we do a factorial of mixes
// then we lower the primary number to get darker colours
	indexT mixIndex = index % kNumPrim;
	indexT kNumMix = index / kNumPrim;	
	indexT secIndex = kNumMix % kNumSecQuanta;
	indexT primIndex = (kNumMix / kNumSecQuanta) % kNumPrimQuanta;

	unsigned long primColor, secColor, lowColor;
	lowColor = radialQuanta[secIndex];
	secColor = secQuanta[secIndex];
	primColor = primQuanta[primIndex];
	
	colorT R, G, B;
	if (mixIndex == 0)	{
		R = primColor;
		G = secColor;
		B = lowColor;
	}
	else
		if (mixIndex == 1)	{
			G = primColor;
			B = secColor;
			R = lowColor;
		}
		else {
			B = primColor;
			R = secColor;
			G = lowColor;
		}
	return oofColor(R, G, B);
}


oofDrawStyleSetSupplied::oofDrawStyleSetSupplied(oofDrawStyleArray* da)
{
	// adopt the storage of the array supplied
	mDrawStyles.adopt(da);
}


oofDrawStyle
oofDrawStyleSetSupplied::getNextDrawStyle()
{
	// loop over the array to get the new draw style
	unsigned long NumDrawStyles = mDrawStyles.count();
	oofColor fore = mDrawStyles.item
				(mNumDrawStylesProduced%NumDrawStyles).foreColor();
	// the characteristics to follow are default for the moment
	// later we will get these from the user supplied structure too
	oofPattern monoPat(oofPattern::white);
 	oofSymbol* theSymbol;
 	theSymbol = new oofSquareSymbol;
 	oofColor back = oofColor::White;
 	oofPattern colPat = oofPattern::gray;
 	
	++mNumDrawStylesProduced;	// will start from 0
	return oofDrawStyle(fore,colPat,monoPat,back,theSymbol, true /* adoptsSymbol */);
}


oofDrawStyleSetSupplied::~oofDrawStyleSetSupplied()
{
}



//---------------------------------------------------------------------------------
//		* oofSymbol
//---------------------------------------------------------------------------------

void
oofSymbol::draw(short x,short y)
{
	// do any setup here
	DrawSelf(x,y);
}

//---------------------------------------------------------------------------------
//		* oofSimpleSymbol + derivatives
//---------------------------------------------------------------------------------


oofRect
oofSimpleSymbol::BoundingRect()
{	
	unsigned short x = getSymbolSizeX();
	unsigned short y = getSymbolSizeY();
	
	oofRect	r(0, 0, x, y);
	
	return r;
}


oofSymbol*	
oofEcsSymbol::clone() const
{
	return new oofEcsSymbol(*this);
}


oofSymbol*	
oofCrossSymbol::clone() const
{
	return new oofCrossSymbol(*this);
}


oofSymbol*	
oofCircleSymbol::clone() const
{
	return new oofCircleSymbol(*this);
}


oofSymbol*	
oofSquareSymbol::clone() const
{
	return new oofSquareSymbol(*this);
}


void
oofEcsSymbol::DrawSelf(short mXDraw, short mYDraw){
	unsigned short x = getSymbolSizeX();
	unsigned short y = getSymbolSizeY();
	
#ifdef _Macintosh
	::MoveTo(mXDraw-x/2,mYDraw-y/2);
	::LineTo(mXDraw+x/2,mYDraw+y/2);
	::MoveTo(mXDraw+x/2,mYDraw-y/2);
	::LineTo(mXDraw-x/2,mYDraw+y/2);
#elif defined _Windows
	HDC		hCurrdc = oofCurrentDraw::getCurrentDC();

	::MoveToEx(hCurrdc, mXDraw-x/2,mYDraw-y/2, NULL);
	::LineTo(hCurrdc, mXDraw+x/2,mYDraw+y/2);
	::MoveToEx(hCurrdc, mXDraw+x/2,mYDraw-y/2, NULL);
	::LineTo(hCurrdc, mXDraw-x/2,mYDraw+y/2);
#endif	// OS
}


void
oofCrossSymbol::DrawSelf(short mXDraw, short mYDraw){
	unsigned short x = getSymbolSizeX();
	unsigned short y = getSymbolSizeY();
	
#ifdef _Macintosh
	::MoveTo(mXDraw-x/2,mYDraw);
	::LineTo(mXDraw+x/2,mYDraw);
	::MoveTo(mXDraw,mYDraw-y/2);
	::LineTo(mXDraw,mYDraw+y/2);
#elif defined _Windows
	HDC		hCurrdc = oofCurrentDraw::getCurrentDC();

	::MoveToEx(hCurrdc, mXDraw-x/2,mYDraw, NULL);
	::LineTo(hCurrdc, mXDraw+x/2,mYDraw);
	::MoveToEx(hCurrdc, mXDraw,mYDraw-y/2, NULL);
	::LineTo(hCurrdc, mXDraw,mYDraw+y/2);
#endif	// OS
}


void
oofCircleSymbol::DrawSelf(short mXDraw, short mYDraw){
	unsigned short x = getSymbolSizeX();
	unsigned short y = getSymbolSizeY();
	
	oofRect theRect(mXDraw - x / 2,
					mYDraw - y / 2,
					mXDraw + x / 2,
					mYDraw + y / 2);
#ifdef _Macintosh
	::FrameOval(theRect);
#elif defined _Windows
	HDC		hCurrdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hCurrdc, ::GetStockObject(NULL_BRUSH));

	::Ellipse(hCurrdc, theRect.left, theRect.top, theRect.right, theRect.bottom);
	::SelectObject(hCurrdc, hOldBr);
#endif	// OS
}


void
oofSquareSymbol::DrawSelf(short mXDraw, short mYDraw){
	unsigned short x = getSymbolSizeX();
	unsigned short y = getSymbolSizeY();
	
	oofRect theRect(mXDraw - x / 2,
					mYDraw - y / 2,
					mXDraw + x / 2,
					mYDraw + y / 2);
#ifdef _Macintosh
	::FrameRect(theRect);
#elif defined _Windows
	HDC		hCurrdc = oofCurrentDraw::getCurrentDC();
	HBRUSH	hOldBr = (HBRUSH)::SelectObject(hCurrdc, ::GetStockObject(NULL_BRUSH));

	::Rectangle(hCurrdc, theRect.left, theRect.top, theRect.right, theRect.bottom);
	::SelectObject(hCurrdc, hOldBr);
#endif	// OS
}
