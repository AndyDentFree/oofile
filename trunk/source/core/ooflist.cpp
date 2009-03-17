// COPYRIGHT 1994 A.D. Software, All rights reserved

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifndef H_OOFLIST
	#include "ooflist.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//  O O F _ L i s t
// -------------------------------------------------------

//----------------------------------------------------------------------
// OOF_List::append
//      append an "item" to the end of the list.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the end.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//----------------------------------------------------------------------

void 
OOF_List::append(unsigned long theItem)
{
    OOF_ListElement *element = new OOF_ListElement(theItem);

    if (isEmpty()) {		// list is empty
			mFirst = element;
			mLast = element;
    } else {			// else put it after last
			mLast->mNext = element;
			mLast = element;
    }
		mCount++;
}

//----------------------------------------------------------------------
// OOF_List::prepend
//      Put an "item" on the front of the list.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the beginning.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//----------------------------------------------------------------------

void OOF_List::prepend(unsigned long theItem)
{
    OOF_ListElement *element = new OOF_ListElement(theItem);

    if (isEmpty()) {		// list is empty
			mFirst = element;
			mLast = element;
    } else {						// else put it before first
			element->mNext = mFirst;
			mFirst = element;
    }
		mCount++;
}

//----------------------------------------------------------------------
// OOF_List::remove
//      remove the first "item" from the front of the list.
// 
// Returns:
//	Removed item, 0 if nothing on the list.
//----------------------------------------------------------------------

unsigned long OOF_List::remove()
{
    OOF_ListElement *element = mFirst;
    unsigned long thing;

    if (isEmpty()) 
			return 0;

    thing = mFirst->mItem;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return thing;
}

//----------------------------------------------------------------------
// OOF_List::member
//      Returns true if the list contains theItem.
//----------------------------------------------------------------------

bool OOF_List::member(unsigned long theItem) const
{ 
	  	OOF_ListElement *ptr;		// keep track

	  	for (ptr = mFirst; ptr != 0; ptr = ptr->mNext)
			{
					if(ptr->mItem == theItem)
						return(true);
			}

		return false;
}

//----------------------------------------------------------------------
// OOF_List::sortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void OOF_List::sortedInsert(unsigned long theItem)
{
    OOF_ListElement *element = new OOF_ListElement(theItem);
    OOF_ListElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, put
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem < mFirst->mItem) {	
				// item goes on front of list
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list bigger than item
		  for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		      if (theItem < ptr->mNext->mItem) {
						element->mNext = ptr->mNext;
					  ptr->mNext = element;
						mCount++;
						return;
			    }
			}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}

//----------------------------------------------------------------------
// List::sortedInsertNoDups
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order and there are no duplicates.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void OOF_List::sortedInsertNoDups(unsigned long theItem)
{
    OOF_ListElement *element = new OOF_ListElement(theItem);
    OOF_ListElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, bung it in
        mFirst = element;
        mLast = element;
        mCount = 1;
	} else if (theItem < mFirst->mItem) {	
		// item goes on front of list
		element->mNext = mFirst;
		mFirst = element;
		mCount++;
	} else {		// look for first one in list bigger than item
		for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		  if (theItem < ptr->mNext->mItem) {
					if (theItem == ptr->mItem) {
						delete element;
						return;
					} else {
						element->mNext = ptr->mNext;
				    ptr->mNext = element;
						mCount++;
						return;
					}
		    }
		}
		// We've tested everything except the last item, and
		// we're either larger, or equal to the last item
		if(theItem != mLast->mItem) {
		    mLast->mNext = element;	// item goes at end of list
		    mLast = element;
		    mCount++;
		}
	}
}



// -------------------------------------------------------
//  O O F _ S o r t e r (various versions - no templates)
// -------------------------------------------------------

//----------------------------------------------------------------------
// OOF_ULSorter::remove
//      remove the first "item" from the front of the list.
// 
// Returns:
//	Removed item, 0 if nothing on the list.
//----------------------------------------------------------------------

unsigned long OOF_ULSorter::remove()
{
    OOF_ULSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_ULSorter::sortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void
OOF_ULSorter::sortedInsert(unsigned long theItem, unsigned long theData)
{
    OOF_ULSorterElement *element = new OOF_ULSorterElement(theItem, theData);
    OOF_ULSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mLastInsertEqual=false;
        mCount = 1;
    } else if (theItem <= mFirst->mItem) {	
		// item goes on front of list
		if (theItem == mFirst->mItem) {
			mLastInsertEqual=true;
			mEqualInserted=element;				
		} else
			mLastInsertEqual=false;
		element->mNext = mFirst;
		mFirst = element;
		mCount++;
	} else {	// look for first one in list bigger than item
		for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
			if (theItem == ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				ptr->mNext = element;
				mLastInsertEqual=true;
				mEqualInserted=element;
				mCount++;
				return;
			} else if (theItem < ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				ptr->mNext = element;
				mLastInsertEqual=false;
				mCount++;
				return;
			}
		mLast->mNext = element;		// item goes at end of list
		mLast = element;
		mLastInsertEqual=false;
		mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_ULSorter::sortedInsertReverse
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void
OOF_ULSorter::sortedInsertReverse(unsigned long theItem, unsigned long theData)
{
    OOF_ULSorterElement *element = new OOF_ULSorterElement(theItem, theData);
    OOF_ULSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mLastInsertEqual=false;
        mCount = 1;
    } else if (theItem >= mFirst->mItem) {	
			// item goes on front of list
			if (theItem == mFirst->mItem) {
				mLastInsertEqual=true;
				mEqualInserted=element;				
			} else
				mLastInsertEqual=false;
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list smaller than item
			for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
				if (theItem > ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					ptr->mNext = element;
					mLastInsertEqual=false;
					mCount++;
					return;
				} else if (theItem == ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					ptr->mNext = element;
					mLastInsertEqual=true;
					mEqualInserted=element;
					mCount++;
					return;
				}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mLastInsertEqual=false;
			mCount++;
    }
}

void
OOF_ULSorter::exchange(OOF_ULSorterElement *theItem)
{
#ifdef OOF_DEBUG
	if(theItem->mNext==0)
			dbConnect::raise("Hmm, sorter::exchange got called, and there's no following record");
#endif

	// Swap the data
	unsigned long temp;
	temp=theItem->mData;
	theItem->mData=theItem->mNext->mData;
	theItem->mNext->mData=temp;
	
	// Reset the Equality stuff
	if((theItem->mNext->mNext!=0)&&(theItem->mNext->mItem==theItem->mNext->mNext->mItem)) {
			mLastInsertEqual=true;
			mEqualInserted=theItem->mNext;		
	} else {
			mLastInsertEqual=false;	
	}
}

//--------//

//----------------------------------------------------------------------
// OOF_LSorter::remove
//----------------------------------------------------------------------

unsigned long OOF_LSorter::remove()
{
    OOF_LSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_LSorter::sortedInsert
//----------------------------------------------------------------------

void
OOF_LSorter::sortedInsert(long theItem, unsigned long theData)
{
    OOF_LSorterElement *element = new OOF_LSorterElement(theItem, theData);
    OOF_LSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem < mFirst->mItem) {	
				// item goes on front of list
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list bigger than item
		  for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		      if (theItem < ptr->mNext->mItem) {
						element->mNext = ptr->mNext;
					  ptr->mNext = element;
					  mCount++;
						return;
			    }
			}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_LSorter::sortedInsertReverse
//----------------------------------------------------------------------

void
OOF_LSorter::sortedInsertReverse(long theItem, unsigned long theData)
{
    OOF_LSorterElement *element = new OOF_LSorterElement(theItem, theData);
    OOF_LSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem > mFirst->mItem) {	
				// item goes on front of list
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list bigger than item
		  for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		      if (theItem > ptr->mNext->mItem) {
						element->mNext = ptr->mNext;
					  ptr->mNext = element;
					  mCount++;
						return;
			    }
			}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}

//--------//

//----------------------------------------------------------------------
// OOF_DSorter::remove
//----------------------------------------------------------------------

unsigned long OOF_DSorter::remove()
{
    OOF_DSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_DSorter::sortedInsert
//----------------------------------------------------------------------

void
OOF_DSorter::sortedInsert(double theItem, unsigned long theData)
{
    OOF_DSorterElement *element = new OOF_DSorterElement(theItem, theData);
    OOF_DSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem < mFirst->mItem) {	
				// item goes on front of list
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list bigger than item
		  for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		      if (theItem < ptr->mNext->mItem) {
						element->mNext = ptr->mNext;
					  ptr->mNext = element;
					  mCount++;
						return;
			    }
			}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_DSorter::sortedInsertReverse
//----------------------------------------------------------------------

void
OOF_DSorter::sortedInsertReverse(double theItem, unsigned long theData)
{
    OOF_DSorterElement *element = new OOF_DSorterElement(theItem, theData);
    OOF_DSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem > mFirst->mItem) {	
				// item goes on front of list
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list bigger than item
		  for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext) {
		      if (theItem > ptr->mNext->mItem) {
						element->mNext = ptr->mNext;
					  ptr->mNext = element;
					  mCount++;
						return;
			    }
			}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}


// -------------------------------------------------------
//  O O F _ P S o r t e r (various versions - no templates)
// -------------------------------------------------------

//----------------------------------------------------------------------
// OOF_ULPSorter::remove
//      remove the first "item" from the front of the list.
// 
// Returns:
//	Removed item, 0 if nothing on the list.
//----------------------------------------------------------------------

unsigned long OOF_ULPSorter::remove()
{
    OOF_ULPSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_ULPSorter::sortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void
OOF_ULPSorter::sortedInsert(unsigned long theItem, unsigned long theData)
{
    OOF_ULPSorterElement *element = new OOF_ULPSorterElement(theItem, theData);
    OOF_ULPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mLastInsertEqual=false;
        mCount = 1;
    } else if (theItem <= mFirst->mItem) {	
		// item goes on front of list
		if (theItem == mFirst->mItem) {
			mLastInsertEqual=true;
			mEqualInserted=element;		
			element->mPart=mFirst->mPart+1;
		} else
			mLastInsertEqual=false;
		element->mNext = mFirst;
		mFirst = element;
		mCount++;
	} else {	// look for first one in list bigger than item
		for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
			if (theItem == ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				element->mPart = ptr->mNext->mPart+1;
				ptr->mNext = element;
				mLastInsertEqual=true;
				mEqualInserted=element;
				mCount++;
				return;
			} else if (theItem < ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				ptr->mNext = element;
				mLastInsertEqual=false;
				mCount++;
				return;
			}
		mLast->mNext = element;		// item goes at end of list
		mLast = element;
		mLastInsertEqual=false;
		mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_ULPSorter::sortedInsertReverse
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//----------------------------------------------------------------------

void
OOF_ULPSorter::sortedInsertReverse(unsigned long theItem, unsigned long theData)
{
    OOF_ULPSorterElement *element = new OOF_ULPSorterElement(theItem, theData);
    OOF_ULPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mLastInsertEqual=false;
        mCount = 1;
    } else if (theItem >= mFirst->mItem) {	
			// item goes on front of list
			if (theItem == mFirst->mItem) {
				mLastInsertEqual=true;
				mEqualInserted=element;				
				element->mPart = mFirst->mPart+1;
			} else
				mLastInsertEqual=false;
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list smaller than item
			for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
				if (theItem > ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					ptr->mNext = element;
					mLastInsertEqual=false;
					mCount++;
					return;
				} else if (theItem == ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					element->mPart = ptr->mNext->mPart+1;
					ptr->mNext = element;
					mLastInsertEqual=true;
					mEqualInserted=element;
					mCount++;
					return;
				}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mLastInsertEqual=false;
			mCount++;
    }
}

void
OOF_ULPSorter::exchange(OOF_ULPSorterElement *theItem)
{
#ifdef OOF_DEBUG
	if(theItem->mNext==0)
			dbConnect::raise("Hmm, PSorter::exchange got called, and there's no following record");
#endif

	// Swap the data
	unsigned long temp, temp2;
	temp=theItem->mData;
	temp2=theItem->mPart;
	theItem->mData=theItem->mNext->mData;
	theItem->mPart=theItem->mNext->mPart;
	theItem->mNext->mData=temp;
	theItem->mNext->mPart=temp2;
	
	// Reset the Equality stuff
	if((theItem->mNext->mNext!=0)&&(theItem->mNext->mItem==theItem->mNext->mNext->mItem)) {
			mLastInsertEqual=true;
			mEqualInserted=theItem->mNext;		
	} else {
			mLastInsertEqual=false;	
	}
}

//--------//

//----------------------------------------------------------------------
// OOF_LPSorter::remove
//----------------------------------------------------------------------

unsigned long OOF_LPSorter::remove()
{
    OOF_LPSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_LPSorter::sortedInsert
//----------------------------------------------------------------------

void
OOF_LPSorter::sortedInsert(long theItem, unsigned long theData)
{
    OOF_LPSorterElement *element = new OOF_LPSorterElement(theItem, theData);
    OOF_LPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem <= mFirst->mItem) {	
		// item goes on front of list
		if (theItem == mFirst->mItem) {
			element->mPart=mFirst->mPart+1;
		}
		element->mNext = mFirst;
		mFirst = element;
		mCount++;
	} else {	// look for first one in list bigger than item
		for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
			if (theItem == ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				element->mPart = ptr->mNext->mPart+1;
				ptr->mNext = element;
				mCount++;
				return;
			} else if (theItem < ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				ptr->mNext = element;
				mCount++;
				return;
			}
		mLast->mNext = element;		// item goes at end of list
		mLast = element;
		mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_LPSorter::sortedInsertReverse
//----------------------------------------------------------------------

void
OOF_LPSorter::sortedInsertReverse(long theItem, unsigned long theData)
{
    OOF_LPSorterElement *element = new OOF_LPSorterElement(theItem, theData);
    OOF_LPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem >= mFirst->mItem) {	
			// item goes on front of list
			if (theItem == mFirst->mItem) {
				element->mPart = mFirst->mPart+1;
			}
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list smaller than item
			for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
				if (theItem > ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					ptr->mNext = element;
					mCount++;
					return;
				} else if (theItem == ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					element->mPart = ptr->mNext->mPart+1;
					ptr->mNext = element;
					mCount++;
					return;
				}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}

//--------//

//----------------------------------------------------------------------
// OOF_DPSorter::remove
//----------------------------------------------------------------------

unsigned long OOF_DPSorter::remove()
{
    OOF_DPSorterElement *element = mFirst;
    unsigned long data;

    if (isEmpty()) 
			return 0;

    data = mFirst->mData;
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
				mLast = 0;
    } else {
        mFirst = element->mNext;
    }
		mCount--;
    delete element;
    return data;
}

//----------------------------------------------------------------------
// OOF_DPSorter::sortedInsert
//----------------------------------------------------------------------

void
OOF_DPSorter::sortedInsert(double theItem, unsigned long theData)
{
    OOF_DPSorterElement *element = new OOF_DPSorterElement(theItem, theData);
    OOF_DPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem <= mFirst->mItem) {	
		// item goes on front of list
		if (theItem == mFirst->mItem) {
			element->mPart=mFirst->mPart+1;
		}
		element->mNext = mFirst;
		mFirst = element;
		mCount++;
	} else {	// look for first one in list bigger than item
		for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
			if (theItem == ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				element->mPart = ptr->mNext->mPart+1;
				ptr->mNext = element;
				mCount++;
				return;
			} else if (theItem < ptr->mNext->mItem) {
				element->mNext = ptr->mNext;
				ptr->mNext = element;
				mCount++;
				return;
			}
		mLast->mNext = element;		// item goes at end of list
		mLast = element;
		mCount++;
    }
}

//----------------------------------------------------------------------
// OOF_DPSorter::sortedInsertReverse
//----------------------------------------------------------------------

void
OOF_DPSorter::sortedInsertReverse(double theItem, unsigned long theData)
{
    OOF_DPSorterElement *element = new OOF_DPSorterElement(theItem, theData);
    OOF_DPSorterElement *ptr;		// keep track

    if (isEmpty()) {	// if list is empty, just bung it in !
        mFirst = element;
        mLast = element;
        mCount = 1;
    } else if (theItem >= mFirst->mItem) {	
			// item goes on front of list
			if (theItem == mFirst->mItem) {
				element->mPart = mFirst->mPart+1;
			}
			element->mNext = mFirst;
			mFirst = element;
			mCount++;
		} else {		// look for first one in list smaller than item
			for (ptr = mFirst; ptr->mNext != 0; ptr = ptr->mNext)
				if (theItem > ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					ptr->mNext = element;
					mCount++;
					return;
				} else if (theItem == ptr->mNext->mItem) {
					element->mNext = ptr->mNext;
					element->mPart = ptr->mNext->mPart+1;
					ptr->mNext = element;
					mCount++;
					return;
				}
			mLast->mNext = element;		// item goes at end of list
			mLast = element;
			mCount++;
    }
}
