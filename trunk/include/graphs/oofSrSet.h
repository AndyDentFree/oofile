// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFSRSET
#define H_OOFSRSET
// =================================================================================
//	oofSrSet.h
// =================================================================================

#include "oofClrbl.h"

class	oofDrawStyleSet;
class	oofGeo;

/**
	Contains series of data in a graph, eg: a single line of a graph.
	The dbView's which pass data into a graph are copied into these 
	oofSeries.
*/
class OOFGRAPH_EXPORT oofSeries : public oofColorable {
	public:
		oofSeries();
		oofSeries(const char *theName,const bool isSymbol=false);
		oofSeries(const char *theName,const oofColor& inColor,const bool isSymbol=false);
		~oofSeries();
		
		char	*Name() const { return mName;};
		void	setLastGeo(oofGeo* g) { mLastGeo = g; };
		oofGeo*	lastGeo() { return mLastGeo; };
		bool	isSymbol() { return mSymbol; };
		void	makeSymbol() { mSymbol=true; };
		
	private:
		char*	mName; // owned
		bool	mSymbol;	// are we displayed as a symbol ?
		oofGeo*	mLastGeo;	// pointer to the last geo to be associated with the series
};


/**
	Track a single oofSeries in an oofSeriesSet.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofSeriesSetItem {
public:
		oofSeriesSetItem(oofSeries *inItem);
		~oofSeriesSetItem();
	
	oofSeriesSetItem*	next();
	oofSeries*	item();
	void	setNext(oofSeriesSetItem*);

private:
	oofSeriesSetItem*	mNext;	// next element on list, 
							// NULL if this is the last
	oofSeries*	mItem;	    	// owned - item on the list
};


/**
	Manage the oofSeries containing data for an oofGraph.
	There's one instance of this in each oofGraph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofSeriesSet {
public:
		oofSeriesSet();
	void			append(oofSeries *inItem);
	const oofSeries*		remove();
	oofSeriesSetItem*	first();
	int				isEmpty();
	unsigned long	count();
	
	oofSeries *findSeriesByName(const char* theSeries, const bool makeNewItemSymbol=false);
	oofSeries *operator[](unsigned long index);

	void	setDrawStyles(oofDrawStyleSet&);
	void	setDrawStyles(oofDrawStyleArray&);
	
	virtual	~oofSeriesSet();
	
private:
	unsigned long	mCount;		// number of items
	oofSeriesSetItem*	mFirst;	// Head of the list, NULL if list is empty
	// owned - special case as linked list of owned items
	oofSeriesSetItem*	mLast;		// Last element of list
};
#endif
