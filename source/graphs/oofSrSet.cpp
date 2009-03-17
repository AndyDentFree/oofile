// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofSrSet.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFSRSET
	#include "oofSrSet.h"
#endif
#include <string.h>
#ifndef H_OOFDSTYL
	#include "oofDStyl.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif


// ---------------------------------------------------------------------------------
//		 oofSeries
// ---------------------------------------------------------------------------------


oofSeries::oofSeries():
	mName(0),mLastGeo(0)
{
}


oofSeries::oofSeries(const char *theName, const bool isSymbol):
	mLastGeo(0),
	mSymbol(isSymbol)
{
	mName=new char[strlen(theName)+1];
	strcpy(mName,theName);
}


oofSeries::oofSeries(const char *theName, const oofColor& inColor, const bool isSymbol):
	oofColorable(inColor),
	mLastGeo(0),
	mSymbol(isSymbol)
{
	mName=new char[strlen(theName)+1];
	strcpy(mName,theName);
}


oofSeries::~oofSeries()
{
	delete [] mName;
}


// ---------------------------------------------------------------------------------
//		 oofSeriesSetItem
// ---------------------------------------------------------------------------------

oofSeriesSetItem::oofSeriesSetItem(oofSeries *inItem)
	:	mNext(0),
    	mItem(inItem)
{
}


oofSeriesSetItem::~oofSeriesSetItem()
{
	delete mItem;	// oofSeriesSetItem owns the oofSeries
} 


oofSeriesSetItem*
oofSeriesSetItem::next()
{
	return mNext;
}


oofSeries*
oofSeriesSetItem::item()
{
	return mItem;
}


void
oofSeriesSetItem::setNext(oofSeriesSetItem* inItem)
{
	mNext = inItem;
}


// ---------------------------------------------------------------------------------
//		 oofSeriesSet
// ---------------------------------------------------------------------------------

oofSeriesSet::oofSeriesSet()
	:	mCount(0),
		mFirst(0),
		mLast(0)
{
}


void 
oofSeriesSet::append(oofSeries *inItem)
{
    oofSeriesSetItem *theItem = new oofSeriesSetItem(inItem);

    if (isEmpty()) {		// list is empty
		mFirst = theItem;
		mLast = theItem;
    } 
    else {			// else put it after last
		mLast->setNext(theItem);
		mLast = theItem;
    }
	mCount++;
}


const oofSeries*
oofSeriesSet::remove()
{
    if (isEmpty()) 
		return 0;

    oofSeriesSetItem *theItem = mFirst;
    const oofSeries *theSeries = mFirst->item();
    
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
		mLast = 0;
    } else {
        mFirst = theItem->next();
    }
    
	mCount--;
		
    delete theItem;
    return theSeries;
}


oofSeriesSetItem*
oofSeriesSet::first()
{
	return mFirst;	// return first element
}


int
oofSeriesSet::isEmpty()
{
	return (mCount==0);	// is the list empty?
}


unsigned long
oofSeriesSet::count()
{
	return mCount;	// Return the number of items
}


oofSeriesSet::~oofSeriesSet()
{
	while (mCount)
		remove();
}

oofSeries*
oofSeriesSet::findSeriesByName(const char* theName, const bool makeNewItemSymbol)
{
	for(oofSeriesSetItem *theItem=first();theItem;theItem=theItem->next()) {
		oofSeries* theSeries=theItem->item();
		if (strcmp(theSeries->Name(),theName)==0)
			return(theSeries);
	}
	// We didn't find an existing one - add a new one
	oofSeries *newSeries=new oofSeries(theName,makeNewItemSymbol);
	append(newSeries);
	
	return newSeries;
}

oofSeries*
oofSeriesSet::operator[](unsigned long index)
{
	if(!mFirst)
		return 0;
	
	if(!index)
		return mFirst->item();
		
	if(index>mCount)
		return mLast->item();
	
	unsigned long i=0;
	for(oofSeriesSetItem *theItem=first();theItem;theItem=theItem->next()){
		if(index==i)
			return theItem->item();
		i++;
	}
	
	// never get here !
	return 0;
}

void	
oofSeriesSet::setDrawStyles(oofDrawStyleSet& inSet)
{
	for(oofSeriesSetItem *theItem=first();theItem;theItem=theItem->next()) {
		// oofSeries* theSeries=theItem->Item();
		// theSeries->setDrawStyle(inSet.getNextDrawStyle());
		theItem->item()->setDrawStyle(inSet.getNextDrawStyle());
	}
}


void	
oofSeriesSet::setDrawStyles(oofDrawStyleArray& inArray)
{
	unsigned long index = 0;
	for(oofSeriesSetItem *theItem=first();theItem;theItem=theItem->next()) {
		theItem->item()->setDrawStyle(inArray.item(index));
		index++;
	}
}


/*
void
oofSeriesSet::ChangeSeriesColor(int inSeriesNumber, RGBColor NewBaseColor)
{
	for(oofSeriesSetItem *theItem=First();theItem;theItem=theItem->Next()) {
		oofSeries* theSeries=theItem->Item();
		if (theSeries->SeriesNumber() == inSeriesNumber)
			theSeries->SetColor(NewBaseColor);
	}
}
*/
