// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFPLBIT
#define H_OOFPLBIT
// =================================================================================
//	oofPlBit.h
// =================================================================================

#include "oofGeos.h"

class oofGeo;


/**
	Single item in an oofPlotBits to track one oofGeo.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofPlotBitsItem {
public:
		oofPlotBitsItem(oofGeo *inItem);
	    ~oofPlotBitsItem();
	    
	oofPlotBitsItem*	next();
	oofGeo*	item();
	void	setNext(oofPlotBitsItem*);

private:
	oofPlotBitsItem*	mNext;	// next element on list, 
							// NULL if this is the last
	oofGeo*	mItem;	    	// item on the list
};


/**
	Collection of oofGEO to draw for an oofGraph.
	Consider a graph's version of a metafile or pict, these are
	generated from the information passed into the graph.
	\todo allow specification of different oofTransformationManager
	\see oofSeriesSet for the management of info in graphs.
*/
class OOFGRAPH_EXPORT oofPlotBits {
public:
		oofPlotBits();
		virtual	~oofPlotBits();
		
	void			append(oofGeo *inItem);
	void			remove();
	oofPlotBitsItem*	first() const;
	bool			isEmpty();
	unsigned long	count();
	
	void	draw();
	void    describe(ostream& os) const;
	void	setIdealRect(oofRect& inRect);
	void	setPaneRect(oofRect& inRect);
	oofRect getPaneRect() const		// EV 8/13/98
			{ return mTransformationManager.getPaneRect(); }
	oofRect&	getIdealRect();
	void	clearTransform()
				{ mTransformationManager.clear(); };

protected:
	virtual void	DrawSelf();
	virtual void 	DescribeSelf(ostream& os) const;
	oofTransformationManager	mTransformationManager;
	unsigned long	mCount;		// number of items
	oofPlotBitsItem*	mFirst;	// Head of the list, NULL if list is empty
	// owned - special case as linked list of owned items
	oofPlotBitsItem*	mLast;		// Last element of list
};
#endif
