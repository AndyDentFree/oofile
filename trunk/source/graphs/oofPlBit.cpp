// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofPlBit.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFPLBIT
	#include "oofPlBit.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif


// ---------------------------------------------------------------------------------
//		 oofPlotBitsItem
// ---------------------------------------------------------------------------------


oofPlotBitsItem::oofPlotBitsItem(oofGeo *inItem)
	:	mNext(0), mItem(inItem)
{
}


oofPlotBitsItem::~oofPlotBitsItem()
{
	delete mItem;	// oofPlotBitsItem owns the oofGeo
} 


oofPlotBitsItem*
oofPlotBitsItem::next()
{
	return mNext;
}


oofGeo*
oofPlotBitsItem::item()
{
	return mItem;
}


void
oofPlotBitsItem::setNext(oofPlotBitsItem* inItem)
{
	mNext = inItem;
}


// ---------------------------------------------------------------------------------
//		 oofPlotBits
// ---------------------------------------------------------------------------------

oofPlotBits::oofPlotBits()
	:	mCount(0),
		mFirst(0),
		mLast(0)
{
	oofRect initialPaneRect(0,0,100,100);
	setPaneRect(initialPaneRect); // to stop div by zero, caused by transforming before pane set before pane set 

}


oofPlotBits::~oofPlotBits()
{
	while (mCount)
		remove();		
}


void 
oofPlotBits::append(oofGeo *inItem)
{
	// Add the GEO to the list
    oofPlotBitsItem *theItem = new oofPlotBitsItem(inItem);

    if (isEmpty()) {		// list is empty
		mFirst = theItem;
		mLast = theItem;
    } 
    else {			// else put it after last
		mLast->setNext(theItem);
		mLast = theItem;
    }
	mCount++;
	
	// Grow the IdealRect to fit this item
	oofRect R=inItem->BoundingRect(&mTransformationManager);
	oofRect M=mTransformationManager.getMargins();
	oofRect I=mTransformationManager.getIdealRect();
	switch (inItem->Position()) {
		case oofGeo::posNone:{
				mTransformationManager.ExpandToInclude(R.left,R.top);
				mTransformationManager.ExpandToInclude(R.right,R.bottom);
			}
			break;
		case oofGeo::posTop:{

// These are commented out since this stuff is all hacked into the
// objects for now - I'll need to make a manager for this !

/*				// NOTE: In this case, we'll have a oofRect in Draw Coords.
				M.top+=(R.bottom-R.top);
				// Modify the margins
				mTransformationManager.setMargins(M);
				// set the location of the geo
				inItem->SetLocation(0,(R.bottom-R.top));
*/			}
			break;

		case oofGeo::posBottom:{
				// NOTE: In this case, we'll have a oofRect in Draw Coords.
/*				M.bottom+=(R.bottom-R.top);
				mTransformationManager.setMargins(M);
				// set the location of the geo
				int dx,dy;
				mTransformationManager.TransformPoint(0,I.bottom,dx,dy);
				inItem->SetLocation(0,dy+(R.bottom-R.top));
*/			}
			break;
		case oofGeo::posBelow:{
			}
			break;
		case oofGeo::posLeft:{
				// NOTE: In this case, we'll have a oofRect in Draw Coords.
				M.left	+= R.width();
				mTransformationManager.setMargins(M);
				inItem->setLocation(0, I.height() / 2 + I.bottom);
			}
			break;
		case oofGeo::posRight:{
				// NOTE: In this case, we'll have a oofRect in Draw Coords.
				M.right	+= R.width();
				mTransformationManager.setMargins(M);
				int Mx,My;
				mTransformationManager.TransformPoint(I.right,0,Mx,My);
				inItem->setLocation(Mx, I.height() / 2 + I.bottom);
			}
			break;
		default: ; // FixMe
	}
	
}


void
oofPlotBits::remove()
{
    if (isEmpty()) 
		return;

    oofPlotBitsItem *theItem = mFirst;
    oofGeo *theGeo = mFirst->item();
    
    if (mFirst == mLast) {	// list had one item, now has none 
        mFirst = 0;
		mLast = 0;
    } else {
        mFirst = theItem->next();
    }
    
	mCount--;
		
    delete theItem;
}


oofPlotBitsItem*
oofPlotBits::first() const
{
	return mFirst;	// return first element
}


bool
oofPlotBits::isEmpty()
{
	return (mCount==0);	// is the list empty?
}


unsigned long
oofPlotBits::count()
{
	return mCount;	// Return the number of items
}


void
oofPlotBits::draw()
{
	// do any setup here
	DrawSelf();
}


void
oofPlotBits::describe(ostream& os) const
{
	// do any setup here
	DescribeSelf(os);
}


void
oofPlotBits::setIdealRect(oofRect& inRect)
{
	mTransformationManager.setIdealRect(inRect);
}


void
oofPlotBits::setPaneRect(oofRect& inRect)
{
	mTransformationManager.setPaneRect(inRect);
}


oofRect&
oofPlotBits::getIdealRect()
{
	return mTransformationManager.getIdealRect();
}


void
oofPlotBits::DrawSelf()
{
	if(mTransformationManager.isDirty()) { // Do we need to transform ?
	  
	  
#ifdef OOF_GRAPH_DEBUG_DUMP
  // this debug stream monitors impact of final tranformation 
  ofstream debugStream("DrawsSelfState",ios::app);  
 int passIndex = 0;
#endif	
	  while(mTransformationManager.isDirty()) {
	    mTransformationManager.setToClean(); // Mark us as clean and make sure we stay that way !
	    								// The reason for this is that some of our GEO's
	    								// may resize the DrawRect because of margin stuff !
		for(oofPlotBitsItem *theItem=first();theItem;theItem=theItem->next()) {
			oofGeo* theGeo=theItem->item();
#ifdef OOF_GRAPH_DEBUG_DUMP
	mTransformationManager.describe(debugStream);	// david debug
	theGeo->describe(debugStream);
#endif
			
			theGeo->Transform( &mTransformationManager );

#ifdef OOF_GRAPH_DEBUG_DUMP			
	mTransformationManager.describe(debugStream);
	theGeo->describe(debugStream);// david debug 990804
#endif

		}

#ifdef OOF_GRAPH_DEBUG_DUMP	
	debugStream << endl << endl;
	debugStream << " transformation pass: "<< passIndex << endl; 
	passIndex++;// david debug 

#endif	 
	  }
#ifdef OOF_GRAPH_DEBUG_DUMP
	debugStream << "---oofplot bits fully transformed----" << endl << endl << endl;
	debugStream.close(); 
#endif	 

	}
	for(oofPlotBitsItem *theItem=first();theItem;theItem=theItem->next()) {
		oofGeo* theGeo=theItem->item();
		theGeo->draw();
	}
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 	
	QDFlushPortBuffer( GetQDGlobalsThePort(), NULL );	
#endif
}


void
oofPlotBits::DescribeSelf(ostream& os) const
{
	for(oofPlotBitsItem *theItem=first();theItem;theItem=theItem->next()) {
		oofGeo* theGeo=theItem->item();
		theGeo->describe(os);
	}
}
