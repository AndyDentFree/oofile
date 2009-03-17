// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFGEOS
#define H_OOFGEOS
//=================================================================================
//	oofGeos.h
//=================================================================================

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFCLRBL
	#include "oofClrbl.h"
#endif
#ifndef H_OOFSIZE
	#include "oofsize.h"
#endif
#ifndef H_OOFDRAW
	#include "oofdraw.h"
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#ifndef H_OOFTRMGR
	#include "oofTrMgr.h"
#endif

class oofSeries;
class oofSeriesSet;
class oofGeoText;

/**
	Graph Element Object drawn in an oofGraph.
	These are cross-platform as their drawing code is
	so trivial it is bounded by the _Macintosh and _Windows
	#defines.
	\note UnJames comments refer to reversing the original
	port from the Mac by a programmer who shall otherwise
	remain nameless.
	\todo clarify the legal position of our PNG and GIF drawing via GD.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeo 	: public oofColorable {
public:
	enum PositionState { posNone, posTop, posLeft, posRight, posBottom, posBelow, posToLeft, posCentre };
	
	oofGeo(int XIdeal, int YIdeal, oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual ~oofGeo();

	oofSeries*	SeriesPtr();
	PositionState Position() { return mPosition; };
	virtual oofRect BoundingRect(oofTransformationManager*);
	virtual	void	setLocation(long inXIdeal, long inYIdeal);
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
	int	locationX() { return mXDraw; };
	int locationY() { return mYDraw; };
	
	void draw();

	virtual void	Transform(oofTransformationManager* inTransformationManagerPtr) = 0;
protected:
	virtual void	DrawSelf(const oofDrawStyle*) {};
/* UnJames
	void			UpdateMargins(oofTransformationManager*, oofRect&);
	// MUST override this if the BoundingRect() calls Transform()
	virtual oofRect	GetAlreadyTransformedBoundingRect(oofTransformationManager*);		
*/
	int		mXIdeal,
			mYIdeal,
			mXDraw,
			mYDraw;
			
	oofSeries*	mSeriesPtr;
	PositionState	mPosition;
};




/**
	Base for element in graph which may have text label shown adjacent.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofLabelledGeo : public oofGeo {
public:
	oofLabelledGeo(int XIdeal, int YIdeal, oofSeries* SeriesPtr = 0, PositionState = posNone);
	oofLabelledGeo(int XIdeal ,int width,int height, oofSeries* SeriesPtr);
	virtual ~oofLabelledGeo();
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 

	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );

protected:
	void			setLabel(bool required, int value, int x, int y, int width);
	virtual	void	DrawSelf(const oofDrawStyle*);


	oofGeoText *	mValueLabel;
	bool			mRequired;
	int				mValue;
	int				mXTextPos,
					mYTextPos,
					mTextWidth;
};



/**
	Drawing primitive for vertical bars on graphs.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoColumn : public oofLabelledGeo {
public:
	// Full Constructor
	oofGeoColumn(int XIdeal, int YIdeal,
		int width,int height, oofSeries* SeriesPtr = 0, PositionState = posNone);
	// Standard Constructor
	oofGeoColumn(int XIdeal, int width, int height, oofSeries* SeriesPtr);

	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	int		mIdealWidth,
			mIdealHeight,
			mDrawWidth,
			mDrawHeight;
};



/**
	Drawing primitive for data as series of points on graphs.
	\see oofGeoLine
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoPoint : public oofGeo {
public:
		// Full Constuctor
		oofGeoPoint(int XIdeal, int YIdeal, oofSeries* SeriesPtr = 0, PositionState = posNone);

	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);	
};



/**
	Drawing primitive for lines on line graphs.
	\see oofLineGraph::finishConstruction
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoLine : public oofGeoPoint {
public:
		oofGeoLine(int XIdeal, int YIdeal, oofSeries* SeriesPtr = 0, PositionState = posNone);
		oofGeoLine*	lastPoint() { return mLastPoint; };
		virtual void 	describe(ostream&) const;
		virtual oofString geoType() const; 
protected:
	virtual void	DrawSelf(const oofDrawStyle*);
	oofGeoLine*	mLastPoint;	// for linking points
};



/**
	Drawing primitive for horizontal bars on graphs.
	\see oofBarGraph::finishConstruction
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoBar : public oofLabelledGeo {
public:
		// Full Constuctor
		oofGeoBar(int XIdeal, int YIdeal,
			int width,int height, oofSeries* SeriesPtr = 0, PositionState = posNone);
		// Standard Constructor
		oofGeoBar(int YIdeal ,int width,int height, oofSeries* SeriesPtr);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	int		mIdealWidth,
			mIdealHeight,
			mDrawWidth,
			mDrawHeight;
};



/**
	Drawing primitive for segments in pie graphs.
	\see oofPieGraph::finishConstruction
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoSector : public oofGeo {
public:
		oofGeoSector(int XIdeal, int YIdeal, int diameter,
						int start, int arc, oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	int 	mIdealDiameter,
			mDrawDiameter,
			mStartAngle,
			mArcAngle;
#ifdef _Windows
	POINT	CalcRadialFromAngle(oofRect& rcBound, int nAngle);
	double	ConvertToRadians(int nDegrees)
				{ return nDegrees * 3.1415926535 / 180; };
#endif	// _Windows
};



/**
	Base for text on graphs drawn from data series.
	Contains actual text and font characteristics.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoText : public oofGeo {
public:
		oofGeoText(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual	~oofGeoText();
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	void			setTextSize(int PointSize);
						
	bool  setFontSize(int textSize = -1);
	int   calcCharHeight(int textSize = -1);
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
					
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	oofString	 mText;
	unsigned int mIdealTextSize,
				 mWidthIdeal;
	int			 mDrawTextSize,
				 mDrawWidth,
				 mDrawHeight;
	oofTextSizer mSizer;
};


/**
	Text on graphs that can be suppressed based on scaling.
	Adds decision making into calling oofGeoText::DrawSelf
	by calling virtual method ShouldDraw.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoSuppressibleText : public oofGeoText {
protected:
		// Full Constructor
		oofGeoSuppressibleText(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr = 0, PositionState = posNone);
		// Standard Constructor
		oofGeoSuppressibleText(int Width, const char* inString);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	virtual 	bool	ShouldDraw() const=0;
};



/**
	Title at top of a graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoTitle : public oofGeoSuppressibleText {
public:
		// Full Constructor
		oofGeoTitle(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr = 0, PositionState = posNone);
		// Standard Constructor
		oofGeoTitle(int Width, const char* inString);
	virtual void Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual oofString geoType() const; 
// UnJames	virtual oofRect	GetAlreadyTransformedBoundingRect(oofTransformationManager*);
protected:
	virtual 	bool	ShouldDraw() const;
};



/**
	Label of graph X axis, generated from data series first item.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoXLabel : public oofGeoSuppressibleText {
public:
		// Full Constructor
		oofGeoXLabel(int XIdeal, int YIdeal, int Width, const char* inString,
		             oofSeries* SeriesPtr = 0, PositionState = posBottom);
		// Standard Constructor
		oofGeoXLabel(int Width, const char* inString);
	virtual void Transform( oofTransformationManager* inTransformationManagerPtr);
	virtual oofString geoType() const; 
protected:
	virtual 	bool	ShouldDraw() const;
};



/**
	Label of graph Y axis, generated from data series first item.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoYLabel : public oofGeoSuppressibleText {
public:
		// Full Constructor
		oofGeoYLabel(int XIdeal, int YIdeal, int Width, const char* inString,
		             oofSeries* SeriesPtr = 0, PositionState = posLeft);
		// Standard Constructor
		oofGeoYLabel(/* int Width, */const char* inString);
	virtual void Transform( oofTransformationManager* inTransformationManagerPtr);
protected:
	virtual 	bool	ShouldDraw() const;
	virtual oofString geoType() const; 
};



/**
	Label of tick marks graph X axis, generated from data series.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoXTickLabel : public oofGeoText {
public:
		// Full Constructor
		oofGeoXTickLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState);
		//Standard Constructor
		oofGeoXTickLabel(int XIdeal, int Width, const char* inString);
		virtual void Transform( oofTransformationManager* inTransformationManagerPtr);
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
	virtual oofString geoType() const; 
};



/**
	Label of tick marks graph Y axis, generated from data series.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoYTickLabel : public oofGeoText {
public:
		// Full Constructor
		oofGeoYTickLabel(int XIdeal, int YIdeal, int Width,
			const char* inString, oofSeries* SeriesPtr, PositionState);
		//Standard Constructor
		oofGeoYTickLabel(int YIdeal, int Width, const char* inString);
		virtual void Transform( oofTransformationManager* inTransformationManagerPtr);
		virtual oofString geoType() const; 
protected:
	virtual	void	DrawSelf(const oofDrawStyle*);
};



/**
	X axis, auto-sized from data series.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoXAxis : public oofGeo {
public:
		oofGeoXAxis(int inLength, int MaInterval, int NumDivisions,
			oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
private:
	virtual void	DrawSelf(const oofDrawStyle*);
	int		mIdealLength,
			mDrawLength,
			mIdealMaTickMark,
			mNumDivisions;
	double	mDrawMaTickMark;
};



/**
	Y axis, auto-sized from data series max value or constrained.
	\todo document effect of changing PositionState
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoYAxis : public oofGeo {
public:
		oofGeoYAxis(int inLength, int MaInterval, int NumDivisions,
			oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
private:
	virtual void	DrawSelf(const oofDrawStyle*);
	int		mIdealLength,
			mDrawLength,
			mIdealMaTickMark,
			mNumDivisions;
	double	mDrawMaTickMark;
};



/**
	Graph legend generated from data series names.
	\see oofGraphSettings::hasLegend to suppress legend.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoLegend : public oofGeo {
public:
		oofGeoLegend(oofSeriesSet*, bool, oofSeries* SeriesPtr = 0, PositionState = posRight);
		~oofGeoLegend();
		
	virtual void	Transform (oofTransformationManager* inTransformationManagerPtr);
	virtual	oofRect	BoundingRect(oofTransformationManager*);
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
private:
	virtual void	DrawSelf(const oofDrawStyle*);
// UnJames	virtual oofRect	GetAlreadyTransformedBoundingRect(oofTransformationManager* tm);
	
	oofTextSizer*	mSizers;  // owned
	oofSeriesSet*	mSeriesSet;
	bool			mSymbols;
	unsigned int	mLegendWidth, mLegendHeight;
};


/**
	Drawing primitive for circles on graphs.
	\todo find out if we actually use this ever - may be here for completeness
	\see oofCircleSymbol for marks on line graphs.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofGeoCircle : public oofGeo {
public:
		oofGeoCircle(int XIdeal, int YIdeal,
			int diameter, oofSeries* SeriesPtr = 0, PositionState = posNone);
	virtual void	Transform( oofTransformationManager* inTransformationManagerPtr );
	virtual void 	describe(ostream&) const;
	virtual oofString geoType() const; 
private:
	virtual void	DrawSelf(const oofDrawStyle*);
	int		mIdealDiameter,
			mDrawDiameter;
};


// -------------------------------------------------------
//             o o f G e o T e x t
// -------------------------------------------------------
inline bool
oofGeoText::setFontSize(int textSize) 
{
	if (textSize == -1)
		textSize = mDrawTextSize;
	return oofTextDrawer::setFontSize(textSize);
}


inline int
oofGeoText::calcCharHeight(int textSize) 
{
	if (textSize == -1)
		textSize = mDrawTextSize;
	return oofTextDrawer::calcCharHeight(textSize);
}


inline 	void			
oofGeoText::setTextSize(int PointSize)
{ 
	mIdealTextSize = PointSize; 
}
						

#endif



