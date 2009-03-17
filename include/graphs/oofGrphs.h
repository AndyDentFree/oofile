// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFGRPHS
#define H_OOFGRPHS
// =================================================================================
//	oofGrphs.h
// =================================================================================

/**
	Version number of graphing package.
	\ingroup oofMacros
*/
#define		OOFVERS_GRAPH	0x01400000	// Version 1.4.0

/*
index to version defines 
Note that you will may lack some files depending on your package

file:        VERSION IDENTIFIER
====         ==================
oof0.h	     OOFVERS_CORE core version
oofrep.h     OOFVERS_REP report writer
oofGrphs.h   OOFVERS_GRAPH graphing engine
oofgui.h     OOFVERS_GUI  generic GUI classes used by PP, MFC and other framework integrations
oofpp.h      OOFVERS_PP  PowerPlant framework (Metrowerks CodeWarrior, on Mac)
oofmfc.h     OOFVERS_MFC MFC GUI framework

*/

/**
	\defgroup oofGraph OOFILE Graphing engine.
	Graphing engine for Mac and Windows, usually used 
	within reports.
	Included in the Open Source OOFILE.
	\ingroup Presentation
*/




#ifndef H_OOFSRSET
	#include "oofSrSet.h"
#endif
#ifndef H_OOFPLBIT
	#include "oofPlBit.h"
#endif
#ifndef H_oofSting
	#include "oofSting.h"	// so people can just include oofGrphs and make settings
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#ifdef _Windows
	#ifndef H_OOFDRAW
		#include "oofdraw.h"
	#endif
#endif

class oofDrawStyle;
class dbView;

/**
	Graph class subclassed to draw different graph types.
	Subclasses take at least one dbView to effectively provide an array
	directly from the database.
	Has interesting virtual drawing space architecture which
	makes it react to scaling by trying to rescale nested items.
*/
class OOFGRAPH_EXPORT oofGraph	: public oofColorable {
public:
	oofGraph(oofGraphSettings* adoptedSettings = 0);
	oofGraph(const oofGraph& rhs);	// copy ctor
	virtual ~oofGraph();

	virtual oofGraph* clone() const=0;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight)=0;
	virtual void finishConstructionWithPresets();
			
	void	draw();
	void    describe(ostream& os) const;
	void	setView(dbView*);
	dbView *getView();
	void	setTitle(const char*);
	const char * getTitle();
	const char * getType();
#ifdef _Windows
	typedef RECT	Rect;
#endif
	void	setPaneRect(Rect inRect);
	void	setPaneRect(oofRect inRect);
	void	clearTransform()
			{ mPlotBits.clearTransform(); };
	void	setDrawStyles(oofDrawStyleSet&);
	void	setDrawStyles(oofDrawStyleArray&);
	void	setStyleToMono();
	void	setStyleToColor();
	bool	drawStylesWereSet() const;
	static	oofGraph*		currentGraph();
	const oofGraphSettings*		settings() const;
	oofGraphSettings*			getLocalSettings();
	static	oofGraphSettings*	defaultSettings();

#ifdef _Windows
	// This MUST be set by the view before calling the draw
	// This relies the same way as sCurrentGraph for serialized access
	void			setCurrentDC(HDC hdc);
	const HDC		getCurrentDC();	// ES
#endif	// _Windows
	
protected:
	virtual void	DrawSelf();
	virtual void	DescribeSelf(ostream& os) const;
	
	void	becomeCurrent();
	
	oofPlotBits 	mPlotBits;
	oofSeriesSet 	mSeriesSet;	
	bool	mIsMono;
	static oofGraph*	sCurrentGraph;
	oofGraphSettings *mSettings;  // owned
	bool mDrawStylesWereSet;
	dbView*	mView;		// owned copy
	OOF_String	mTitle;
	OOF_String	mType;
//	static	oofGraphSettings *sDefaultSettings;
};


/**
	Mixin base for oofGraph's that draw lines as overlays on original bars.
	Just performs the data management for the overlay.
	Either overlay by specifying a single dbView to have one column of dbLong values
	or specify a number N saying the last N series of the original data are drawn as overlays.
	\todo allow the dbView incoming to contain more than one sries
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT OOF_mixOverlayableGraph {
public:
	OOF_mixOverlayableGraph(unsigned long numOverlaid=0);
	virtual ~OOF_mixOverlayableGraph();

// AD 990602 the inconsistency in using getXX and setXX here vs other XX() overloads
// will be resolved at a future date - this is what happens when you don't code review
// junior programmers with a will of iron!	
	void	setOverlaid(unsigned long numOverlaid);
	void	setOverlayView(dbView* clonedView);  ///< will become adoptedView in future
	void	setOverlayView(const dbView& clonedView);
	dbView*	getOverlayView() const;
	unsigned long	getNumOverlaid() const;
	
protected:	
	unsigned long	mNumOverlaid;///< Allow for items in the main view to be overlaid
	dbView*	mOverlayView;  ///< OWNED Allow for adding another view (long) with an overlay in it
};


/**
	Graph of oofGeoColumn vertical bars.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofColumnGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are bar constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	enum { ColumnWidth = 20 };
	enum { Spacing = 2 };
	enum { SeriesSpacing = 10 };
	oofColumnGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};


/**
	Graph of oofGeoPoint individual points.
	\see oofLineGraph for connected point graphs.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofPointGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are point constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	// Because the physical font size is used to ratio against this value, we need
	// to have adjustments for the ratio of physical font size in co-ordinate space 
	// for the various platforms.
#ifdef _Macintosh
	enum { PointWidth = 20 };
#elif defined _Windows
	enum { PointWidth = 80 };
#endif
	oofPointGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};


/**
	Graph of oofGeoLine connected points.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofLineGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are point constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	// Because the physical font size is used to ratio against this value, we need
	// to have adjustments for the ratio of physical font size in co-ordinate space 
	// for the various platforms.
#ifdef _Macintosh
	enum { PointWidth = 20 };
#elif defined _Windows
	enum { PointWidth = 80 };
#endif
	oofLineGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};


/**
	Graph of oofGeoPoint arbitrary points from XY data, no assumptions of order.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofXYGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are point constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	// Because the physical font size is used to ratio against this value, we need
	// to have adjustments for the ratio of physical font size in co-ordinate space 
	// for the various platforms.
#ifdef _Macintosh
	enum { PointWidth = 20 };
#elif defined _Windows
	enum { PointWidth = 80 };
#endif
	oofXYGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};


/**
	Graph of oofGeoBar horizontal bars.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofBarGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are bar constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	enum { BarWidth = 20 };
	enum { Spacing = 2 };
	enum { SeriesSpacing = 10 };
	oofBarGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};


/**
	Graph of oofGeoBar horizontal bars where each point in series is percentage of one bar.
	Total bar size at any given X point is sum of values of all series at that point.
	So stacked bar makes it easy to compare both relative values at each X point and totals between
	different X points.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofStackedBarGraph : public oofGraph, public OOF_mixOverlayableGraph {
public:
	// Bold Assumption follows:
	// These are bar constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	enum { BarWidth = 20 };
	enum { Spacing = 4 };
	enum { SeriesSpacing = 10 };
	
	oofStackedBarGraph(dbView* clonedView=0, unsigned long numOverlaid=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
	};


/**
	Graph of oofGeoBar horizontal bars where each series is percentage of one bar, all same length.
	Better for comparing how proportions change over X (usually time).
	\see oofStackedBarGraph for similar graph without ordinal restriction
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofOrdinalStackedBarGraph : public oofGraph, public OOF_mixOverlayableGraph {
public:
	// Bold Assumption follows:
	// These are bar constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	enum { BarWidth = 20 };
	enum { Spacing = 4 };
	enum { SeriesSpacing = 10 };
	
	oofOrdinalStackedBarGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor

	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long yAxisHeight);
};

/**
	Circular graph of oofGeoSector wedge-like pieces.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofPieGraph : public oofGraph {
public:
	// Bold Assumption follows:
	// These are bar constants for the ideal space only - the have little bearing
	// on the final view - EXCEPT for the ratios between them !
	enum { ChartDiameter = 1000 };
	oofPieGraph(dbView* clonedView=0);
	// uses default copy ctor, dtor
	
	virtual oofGraph* clone() const;
	virtual void finishConstruction(dbView*, const char* theTitle, unsigned long meaningless=0);
};


// Graph Scale sizing class

/**
	Temporarily used to calculate axis divisions based on length.
	\see oofBarGraph::finishConstruction for example of use.
	\todo create from oofGraphSettings so can subclass with different rules.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofScaleSizer {
public:
	enum { kMinDivs=5 };
	enum { kMaxDivs=10 }; // Must be double (for now)
	
	oofScaleSizer(double Max); ///< Perform all calcs in ctor for easy subclassing.
	~oofScaleSizer() {};
	
	double Max() 			{ return mMax;};
	double Div() 			{ return mDiv;};
	double NumDivs() 		{ return mNumDivs;};
	long NumMinorTicks()	{ return mNumMinorTicks;};

private:
	double mMax;
	double mDiv;
	double mNumDivs;
	long   mNumMinorTicks;
};


// ---------------------------------------------------------------------------------
//	                	o o f G r a p h
// ---------------------------------------------------------------------------------

inline bool
oofGraph::drawStylesWereSet() const
{
	return mDrawStylesWereSet;
}


#ifdef _Windows
inline const HDC 
oofGraph::getCurrentDC()
{
	return oofCurrentDraw::getCurrentDC();
}


inline void 
oofGraph::setCurrentDC(HDC hdc)
{
	oofCurrentDraw::setCurrentDC(hdc);
}
#endif


// ---------------------------------------------------------------------------------
//	              O O F _ m i x O v e r l a y a b l e G r a p h
// ---------------------------------------------------------------------------------
inline dbView*
OOF_mixOverlayableGraph::getOverlayView() const
{
	return mOverlayView;
}


inline unsigned long
OOF_mixOverlayableGraph::getNumOverlaid() const
{
	return mNumOverlaid;
}


#endif

