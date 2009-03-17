// COPYRIGHT 1997 A.D. Software, All rights reserved

#ifndef H_OOFGRDTA
#define H_OOFGRDTA
//=================================================================================
//	oofGrDta.h
//=================================================================================
class dbView;
class dbField;
class oofGraphSettings;

/**
	Temporarily accumulate graph data to be able to scale based on range of all data.
	\see oofBarGraph::finishConstruction for examples of use.
	\ingroup oofGraph
*/
class oofGraphData {

public:
	enum DataFormat	{ standard, xy, ordinal };
	enum DataType	{ integer, real };
	
			oofGraphData(dbView*, dbView* overlayView=0);
			~oofGraphData();
			
	long	seriesValue();
	long	dataValue();

	long	overlayValue();

	long	scaleSeriesValue(long);
	long	scaleDataValue(long);

	double	scaleSeriesValue(double);
	double	scaleDataValue(double);

	long	unScaleSeriesValue(long);
	long	unScaleDataValue(long);

	double	unScaleSeriesValue(double);
	double	unScaleDataValue(double);

	char*	makeScaleString(long);
	void	setScaleString(long, char*, const oofGraphSettings*);
	
	// Standard Accessors
	dbField*	FirstField() { return mFirstField; };
	dbField*	SeriesField() { return mSeriesField; };
	dbField*	DataField() { return mDataField; };
	dbField*	OverlayField() { return mOverlayField; };

	// XY Accessors
	dbField*	XField() { return mSeriesField; };
	dbField*	YField() { return mDataField; };
private:

	// Scaling Factors
	long		mSerScaling;
	long		mDataScaling;
	
	// Formats
	DataFormat	mFormat;
	DataType	mSerType;
	DataType	mDataType;
	
	// View Accessors
	dbField*	mFirstField;
	dbField*	mSeriesField;
	dbField*	mDataField;
	dbField*	mOverlayField;
};

#endif
