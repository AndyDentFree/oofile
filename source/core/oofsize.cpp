// ===========================================================================
//	oofsize.cpp 			©1996 A.D. Software   All rights reserved.
// ===========================================================================
//
#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFSIZE
	#include "oofsize.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif

#include <ctype.h>
#include <assert.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef _Macintosh
	#include <QuickdrawText.h>
#endif

// -------------------------------------------------------
//              O O F _ m i x C o l S i z a b l e
// -------------------------------------------------------

OOF_mixColSizable::OOF_mixColSizable(oofColSizer* adoptedSizer) :
	mSizer(adoptedSizer)
{}


OOF_mixColSizable::OOF_mixColSizable(const OOF_mixColSizable& rhs) :
	mSizer(0)
{
	if (rhs.mSizer)
		mSizer = rhs.mSizer->clone(this);
}


OOF_mixColSizable::~OOF_mixColSizable()
{
	delete mSizer;
}
	

void 
OOF_mixColSizable::colSizer(oofColSizer* adoptedSizer)
{
	delete mSizer;
	mSizer = adoptedSizer;
}


oofColSizer* 
OOF_mixColSizable::colSizer()
{
	if (!mSizer)
		mSizer = MakeDefaultSizer();
	return mSizer;
}


oofColSizer* 
OOF_mixColSizable::MakeDefaultSizer(unsigned int numCols) const
{
	return new oofColSizer(numCols);
}



// -------------------------------------------------------
//                   o o f C o l S i z e r
// -------------------------------------------------------
oofColSizer::oofColSizer(unsigned short numCols) :
	mNumCols(numCols),
	mColFixedWidths(kNotSet, numCols),
	mColPercentages(kNotSet, numCols), 
	mColCalcWidths(kNotSet, numCols),
	mColAlignments(alignNatural, numCols),
	mHaveCalculatedWidth(false)
{
}


oofColSizer* 
oofColSizer::clone(OOF_mixColSizable* /*newOwner*/) const
{
	return new oofColSizer(*this);
}
	

void 
oofColSizer::calculateWidths(unsigned long widthToDivide, unsigned short colSepWidth)
{
// loop once setting those with fixed widths or percentages
// and flagging those that are defaulted
// then loop again to set the defaults

	mHaveCalculatedWidth = true;
	unsigned long totalSepWidth = (mNumCols-1)*colSepWidth;
	if (totalSepWidth >= widthToDivide)
		totalSepWidth = 0;
	else
		widthToDivide -= totalSepWidth;
	unsigned long widthLeft = widthToDivide;
	unsigned int numberNotSet = 0;
	unsigned int skipColsAfter = mNumCols;		// model is, if a single col won't fit, we trunc at that col

// try to use pre-specified fixed width or fixed percentage	
	for (unsigned int i=0; i<skipColsAfter; i++) {
		unsigned long calcColWidth=0;
		unsigned long fixedWidth = mColFixedWidths.value(i);
		if (fixedWidth == kNotSet) {
			unsigned long fixedPercentage = mColPercentages.value(i);
			if (fixedPercentage == kNotSet) {
				numberNotSet++;
				mColCalcWidths[i] = kNotSet;
				continue;	//*********** will calculate based on defaults later
			}
			else
				calcColWidth = (widthToDivide*fixedPercentage)/100;	
		}
		else
			calcColWidth = fixedWidth;
			
		if (calcColWidth > widthLeft) {
// allow for some slop here of a couple of pixels (ASSUMING in pixels)
// NOT YET IMPLEMENTED more sophisticated model of range and scalar type to determine amount of slop
			if ((calcColWidth - widthLeft) <= 4) 	//****** SLOP OF 4 UNITS TRUNCATES COL TO FIT
				calcColWidth = widthLeft;
			else 
				calcColWidth = 0;
			widthLeft = 0;
			skipColsAfter = i;	// so ALL columns after this one vanish
		}
		else 
			widthLeft -= calcColWidth;
		mColCalcWidths[i] = calcColWidth;
	}
	
// if not all set by fixed amounts, or fixed amounts didn't fit, either apply defaults
// or clear remaining widths
	if (numberNotSet) {
		if (widthLeft)
			CalculateDefaultWidths(widthLeft, numberNotSet);
		else {
			for (unsigned int j=0; j<mNumCols; j++) {
				if (mColCalcWidths.value(j) == kNotSet) 
					mColCalcWidths[j] = 0;
			}
		}
	}
}


void 
oofColSizer::CalculateDefaultWidths(unsigned long widthToDivide, unsigned int numberNotSet)
{
	unsigned long defaultWidth = widthToDivide / numberNotSet;
	for (unsigned int i=0; i<mNumCols; i++) {
		if (mColCalcWidths.value(i) == kNotSet) {
			mColCalcWidths[i] = defaultWidth;
		}
	}
}




unsigned long 
oofColSizer::width(unsigned short colNum)
{
	assert(mHaveCalculatedWidth);
	return mColCalcWidths[colNum];
}
	

unsigned long
oofColSizer::fixedWidth(unsigned short colNum) const
{
	assert(mHaveCalculatedWidth);
	return mColFixedWidths.value(colNum);
}
	

oofColSizer::colAlignT
oofColSizer::alignment(unsigned short colNum) const
{
	assert(mHaveCalculatedWidth);
	colAlignT ret = (colAlignT)mColAlignments.value(colNum);
	return ret;
}


unsigned long
oofColSizer::fixedPercentage(unsigned short colNum) const
{
	assert(mHaveCalculatedWidth);
	return mColPercentages.value(colNum);
}


void
oofColSizer::setFixedWidth(unsigned short colNum, unsigned long n)
{
	mColFixedWidths[colNum] = n;
}


void
oofColSizer::setFixedPercentage(unsigned short colNum, unsigned long n)
{
	mColPercentages[colNum] = n;
}


void
oofColSizer::setAlignment(unsigned short colNum, oofColSizer::colAlignT n)
{
	mColAlignments[colNum] = n;
}


oofColSizer::colAlignT 
oofColSizer::naturalAlignmentForField(OOF_fieldTypes inType)
{
	oofColSizer::colAlignT ret;
	switch(inType){
		case charField:
		case textField: 
			ret=oofColSizer::alignLeft;
			break;
			
		default: 
			ret=oofColSizer::alignRight;
			break;
	}
	return ret;
}



// -------------------------------------------------------
//  o o f V i e w F i e l d D e f a u l t s C o l S i z e r
// -------------------------------------------------------
oofViewFieldDefaultsColSizer::oofViewFieldDefaultsColSizer(const dbView* v) :
	oofColSizer(v->count()),
	mView(v)
{}

oofViewFieldDefaultsColSizer::oofViewFieldDefaultsColSizer(const oofViewFieldDefaultsColSizer& rhs, const dbView* v) :
	oofColSizer(rhs),
	mView(v)
{}

oofColSizer* 
oofViewFieldDefaultsColSizer::clone(OOF_mixColSizable* newOwner) const
{
	return new oofViewFieldDefaultsColSizer(*this, (const dbView*)newOwner);	// safe?? downcast
// use special form of copy ctor to pass in new owner, but still copy parent members	
}


void 
oofViewFieldDefaultsColSizer::CalculateDefaultWidths(unsigned long widthToDivide, unsigned int/* numberNotSet*/)
{
// NOTE if these repeated loops look inefficient, remember we are talking about a small set
// of fields - it would be highly unusual to exceed 20 fields in a view

// loop once to work out the proportions and total wrappable widths of the unspecified fields

// NOT YET IMPLEMENTED - more sophisticated algorigths, need ideal widths, with constraint by min wrap widths

	unsigned long* fieldMinWrapChars = new unsigned long[mNumCols];
	unsigned long totalFieldWrapChars = 0;
	for (unsigned int i=0; i<mNumCols; i++) {
		if (mColCalcWidths.value(i) == kNotSet) {
			unsigned long fieldWrap = mView->field(i).fieldMinWrapWidth();
			totalFieldWrapChars += fieldWrap;
			fieldMinWrapChars[i] = fieldWrap;
		}
	}

	unsigned long* fieldUsualWrapChars = new unsigned long[mNumCols];
	unsigned long totalFieldUsualWrapChars = 0;
	for (unsigned int j=0; j<mNumCols; j++) {
		if (mColCalcWidths.value(j) == kNotSet) {
			unsigned long fieldWrap = mView->field(j).fieldUsualWrapWidth();
			totalFieldUsualWrapChars += fieldWrap;
			fieldUsualWrapChars[j] = fieldWrap;
		}
	}

	unsigned long consumedWidth=0;
// loop to assign the default widths for cols with stringent minimum
// which uses up some portion of the space
	for (unsigned int k=0; k<mNumCols; k++) {
		if ((mColCalcWidths.value(k) == kNotSet) && (fieldUsualWrapChars[k]==fieldMinWrapChars[k])) {	// calc a default width
			unsigned long calcWidth  = (widthToDivide * fieldMinWrapChars[k]) / totalFieldWrapChars;	// straight proportion
			consumedWidth += calcWidth;
			mColCalcWidths[k] = calcWidth;
			totalFieldUsualWrapChars -= fieldUsualWrapChars[k];	// remove us from total used to divvy up remaining space in next loop
		}
	}

	widthToDivide -= consumedWidth;
// loop to assign the usual widths for other cols, as portion of what's left
	for (unsigned int l=0; l<mNumCols; l++) {
		if (mColCalcWidths.value(l) == kNotSet) {	// calc a default width
			mColCalcWidths[l] = (widthToDivide * fieldUsualWrapChars[l]) / totalFieldUsualWrapChars;	// straight proportion
		}
	}
	delete[] fieldMinWrapChars;
	delete[] fieldUsualWrapChars;
}

