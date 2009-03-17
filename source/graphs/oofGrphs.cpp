// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofGrphs.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif

#include <assert.h>
#include <math.h>

#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFGEOS
	#include "oofGeos.h"
#endif
#ifndef H_OOFTRMGR
	#include "oofTrMgr.h"
#endif
#ifndef H_oofSting
	#include "oofSting.h"
#endif
#ifndef H_OOFGRDTA
	#include "oofGrDta.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// static variables
oofGraph*	oofGraph::sCurrentGraph;
//oofGraphSettings*	oofGraph::sDefaultSettings = 0;

// ---------------------------------------------------------------------------------
//		 oofGraph
// ---------------------------------------------------------------------------------

oofGraph::oofGraph(oofGraphSettings* adoptedSettings) :
	mIsMono(false),	// All graphs start colourful
	mSettings(adoptedSettings),
	mDrawStylesWereSet(false),
	mView(0)
{
	mDrawStyle = new oofDrawStyle;  // ALWAYS have a DrawStyle
	// become the current graph
	becomeCurrent();
}


oofGraph::oofGraph(const oofGraph& rhs) : oofColorable(rhs) { assert(0); }
	// THIS NEEDS TO DUPLICATE ALL THE PLOTBITS!!!!!!!


oofGraph::~oofGraph()
{
	delete mView;
	delete mSettings;
}


void
oofGraph::draw()
{
	// do any setup here	
#ifdef _Macintosh
	::TextFace(0);	// reset current style as it is global and not currently set by graph objects
#endif
	becomeCurrent();
	DrawSelf();
}


void
oofGraph::describe(ostream& os) const
{
	// do any setup here	
#ifdef _Macintosh
	::TextFace(0);	// reset current style as it is global and not currently set by graph objects
#endif
// AD - do we need this, breaks const!	becomeCurrent();
	DescribeSelf(os);
}


void
oofGraph::setPaneRect(Rect inRect)
{
	oofRect	rc;
	
	rc = inRect;
	mPlotBits.setPaneRect(rc);
}

void
oofGraph::setPaneRect(oofRect inRect)
{
	mPlotBits.setPaneRect(inRect);
}

void	
oofGraph::setDrawStyles(oofDrawStyleSet& inSet)
{
	if (!mSeriesSet.isEmpty()) {
		mSeriesSet.setDrawStyles(inSet);
		mDrawStylesWereSet = true;
	}
}


void
oofGraph::setDrawStyles(oofDrawStyleArray& inArray)
{
	if (!mSeriesSet.isEmpty()) {
		mSeriesSet.setDrawStyles(inArray);
		mDrawStylesWereSet = true;
	}
}


void
oofGraph::setView(dbView* inView)
{
	if (inView) {
		delete mView;
		mView = new dbView(*inView);	// copy lightweight object
	}
}


dbView *
oofGraph::getView()
{
	return mView;		// returns NULL if no view is attached, caller should check
}


void
oofGraph::setTitle(const char* inTitle)
{
	mTitle = inTitle;
}


const char *
oofGraph::getTitle()
{
	return mTitle;
}

const char *
oofGraph::getType()
{
	return mType;
}

void
oofGraph::setStyleToMono()
{
	mIsMono = true;
}


void
oofGraph::setStyleToColor()
{
	mIsMono = false;
}


oofGraph*
oofGraph::currentGraph()
{
	return sCurrentGraph;
}

const oofGraphSettings*
oofGraph::settings() const
{
	if (mSettings)	// if the graph has its own settings
		return mSettings;
	else			// use defaults
		return defaultSettings();
}


oofGraphSettings*
oofGraph::defaultSettings()
{
	return oofGraphDefaultSettings::settings();
}


void
oofGraph::finishConstructionWithPresets()
{
	assert(mView);
	// blank title is the default and is OK
#ifdef _Windows
	oofCurrentDraw::makeInfoDC();
#endif
	finishConstruction(mView, mTitle, settings()->YAxisHeight());	// default Y-axis height
}


oofGraphSettings*
oofGraph::getLocalSettings()
{
	if (!mSettings)	
		mSettings = new oofGraphSettings();
	return mSettings;
}


void
oofGraph::DrawSelf()
{
	// DrawSelf must be atomic for this to work
	if (mIsMono) {
		oofDrawStyle::setStyleToMono();
		mPlotBits.draw();
		oofDrawStyle::setStyleToColor();
	}
	else
		mPlotBits.draw();
}


void
oofGraph::DescribeSelf(ostream& os) const
{
	if (mIsMono) 
		os << "mono graph" << endl;
	else
		os << "coloured graph" << endl;
	mPlotBits.describe(os);
}


void
oofGraph::becomeCurrent()
{
	sCurrentGraph = this;
}


// ---------------------------------------------------------------------------------
//		O O F _ m i x O v e r l a y a b l e G r a p h
// ---------------------------------------------------------------------------------
OOF_mixOverlayableGraph::OOF_mixOverlayableGraph(unsigned long numOverlaid) :
	mNumOverlaid(numOverlaid),
	mOverlayView(0)
{}


OOF_mixOverlayableGraph::~OOF_mixOverlayableGraph()
{
	delete mOverlayView;
}

	
void	
OOF_mixOverlayableGraph::setOverlaid(unsigned long numOverlaid)
{
	mNumOverlaid = numOverlaid;
}
	
	
void
OOF_mixOverlayableGraph::setOverlayView(dbView* clonedView)
{
	delete mOverlayView;
	mOverlayView = new dbView(*clonedView);	// copy lightweight object
}
	
	
void
OOF_mixOverlayableGraph::setOverlayView(const dbView& clonedView)
{
	delete mOverlayView;
	mOverlayView = new dbView(clonedView);	// copy lightweight object
}


// ---------------------------------------------------------------------------------
//		 oofColumnGraph
// ---------------------------------------------------------------------------------


oofColumnGraph::oofColumnGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofColumnGraph::clone() const
{
	return new oofColumnGraph(*this);
}


void
oofColumnGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long yAxisHeight)
{
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr;
	
	long CurLeft=0;
	unsigned long SeriesWidth=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
	
	mType = "column";
	oofGraphData Data(theView);
	
	// Iterate over the data
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			if(CurLeft) {
				if(!SeriesWidth)
					SeriesWidth=CurLeft;
				mPlotBits.append(new oofGeoXTickLabel(CurLeft-SeriesWidth,SeriesWidth,thePrevStr));
				CurLeft+=SeriesSpacing;
			}
		}
		mPlotBits.append(new oofGeoColumn(CurLeft,ColumnWidth,Data.dataValue(),
						 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField())));
		thePrevStr.adopt(theFirstFieldStr);
		CurLeft+=(ColumnWidth+Spacing);
	}
	CurLeft-=Spacing;
	mPlotBits.append(new oofGeoXTickLabel(CurLeft-SeriesWidth, SeriesWidth, thePrevStr));
	// add the X axis
	mPlotBits.append(new oofGeoXAxis(CurLeft,SeriesWidth+SeriesSpacing,0));
	// add the Y axis
	long theAxisHeight;
	if(yAxisHeight){   // Quick hack for Andy
		theAxisHeight=yAxisHeight;
	} else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().top);	
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoYAxis(Data.unScaleDataValue(ScaleSizer.Max()),
								     Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the Y axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		mPlotBits.append(new oofGeoYTickLabel
			((int)Data.unScaleDataValue(ScaleSizer.Div())*(p+1), YtickLabelLen, labelStr));
	}
	// add the title
	mPlotBits.append(new oofGeoTitle(CurLeft,theTitle));
	// add X label
	mPlotBits.append(new oofGeoXLabel(CurLeft,Data.FirstField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.DataField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,false /* Use Symbols */));
}


// ---------------------------------------------------------------------------------
//		 oofPointGraph
// ---------------------------------------------------------------------------------


oofPointGraph::oofPointGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofPointGraph::clone() const
{
	return new oofPointGraph(*this);
}


void
oofPointGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long yAxisHeight)
{
	long CurLeft=-1;
	unsigned long SeriesWidth=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
		
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr;
	mType = "point";
		
	oofGraphData Data(theView);

	// Iterate over the data
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			if(CurLeft==-1)
				CurLeft=0;
			else {
				mPlotBits.append(new oofGeoXTickLabel(CurLeft,PointWidth,thePrevStr));
				CurLeft+=PointWidth;
			}
		}
		mPlotBits.append(new oofGeoPoint(CurLeft+(PointWidth/2),Data.dataValue(),
						 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField(), true /* symbol */)));
		thePrevStr.adopt(theFirstFieldStr);
	}
	// add the last label
	mPlotBits.append(new oofGeoXTickLabel(CurLeft, PointWidth, thePrevStr));
	// and expand the axis to fit it !
	CurLeft+=PointWidth;
	// add the X axis
	mPlotBits.append(new oofGeoXAxis(CurLeft, PointWidth, 0));
	// add the Y axis
	long theAxisHeight;
	if(yAxisHeight){   // Quick hack for Andy
		theAxisHeight=yAxisHeight;
	} else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().top);	
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoYAxis(Data.unScaleDataValue(ScaleSizer.Max()),
								     Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the Y axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		mPlotBits.append(new oofGeoYTickLabel
			((int)Data.unScaleDataValue(ScaleSizer.Div())*(p+1),YtickLabelLen, labelStr));
	}
	// add the title
	mPlotBits.append(new oofGeoTitle(CurLeft,theTitle));
	// add X label
	mPlotBits.append(new oofGeoXLabel(CurLeft,Data.FirstField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.DataField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,true /* Use Symbols */));
}


// ---------------------------------------------------------------------------------
//		 oofLineGraph
// ---------------------------------------------------------------------------------


oofLineGraph::oofLineGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofLineGraph::clone() const
{
	return new oofLineGraph(*this);
}


void
oofLineGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long yAxisHeight)
{
	long CurLeft=-1;
	unsigned long SeriesWidth=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
		
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr;
	oofGraphData Data(theView);
	mType = "line";
	
	
	// Iterate over the data
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			if(CurLeft==-1)
				CurLeft=0;
			else {
				mPlotBits.append(new oofGeoXTickLabel(CurLeft,PointWidth,thePrevStr));
				CurLeft+=PointWidth;
			}
		}
		mPlotBits.append(new oofGeoLine(CurLeft+(PointWidth/2),Data.dataValue(),
						 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField(), true /* symbol */)));
		thePrevStr.adopt(theFirstFieldStr);
	}
	// add the last label
	mPlotBits.append(new oofGeoXTickLabel(CurLeft, PointWidth, thePrevStr));
	// and expand the axis to fit it !
	CurLeft+=PointWidth;
	// add the X axis
	mPlotBits.append(new oofGeoXAxis(CurLeft, PointWidth, 0));
	// add the Y axis
	long theAxisHeight;
	if(yAxisHeight){   // Quick hack for Andy
		theAxisHeight=yAxisHeight;
	} else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().top);	
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoYAxis(Data.unScaleDataValue(ScaleSizer.Max()),
								     Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the Y axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		mPlotBits.append(new oofGeoYTickLabel
			((int)Data.unScaleDataValue(ScaleSizer.Div())*(p+1),YtickLabelLen, labelStr));
	}
	// add the title
	mPlotBits.append(new oofGeoTitle(CurLeft,theTitle));
	// add X label
	mPlotBits.append(new oofGeoXLabel(CurLeft,Data.FirstField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.DataField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,true /* Use Symbols */));
}


// ---------------------------------------------------------------------------------
//		 oofXYGraph
// ---------------------------------------------------------------------------------


oofXYGraph::oofXYGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofXYGraph::clone() const
{
	return new oofXYGraph(*this);
}


void
oofXYGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long yAxisHeight)
{
	mType = "XY";
	
	oofGraphData Data(theView);
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
	
	// Iterate over the data
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		mPlotBits.append(new oofGeoPoint(Data.seriesValue(),Data.dataValue(),
						 mSeriesSet.findSeriesByName(*(dbChar*)Data.FirstField(), true /* symbol */)));
	}
	oofRect I=mPlotBits.getIdealRect();
	// add the X axis
	long theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().right);
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoXAxis(Data.unScaleDataValue(ScaleSizer.Max()),
									 Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the X axis labels
	{ // extra scope as MSVC4 doesn't do ANSI for declaration rules
		for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
			char labelStr[255];
			Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
			short round=(int)Data.unScaleDataValue(ScaleSizer.Div())%2;
			mPlotBits.append(new oofGeoXTickLabel
				((int)(Data.unScaleDataValue(ScaleSizer.Div())*(p+1)-Data.unScaleDataValue(ScaleSizer.Div())/2),
				 Data.unScaleDataValue(ScaleSizer.Div())+round, labelStr));
		}	
	}
	// add the Y axis
	long theYAxisHeight;
	if(yAxisHeight){   // Quick hack for Andy
		theYAxisHeight=yAxisHeight;
	} else
		theYAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().top);	
	oofScaleSizer YScaleSizer(theYAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theYAxisHeight);
	mPlotBits.append(new oofGeoYAxis(Data.unScaleDataValue(YScaleSizer.Max()),
								     Data.unScaleDataValue(YScaleSizer.Div()),YScaleSizer.NumMinorTicks()));
	// add the Y axis labels
	for(unsigned p=0;p<YScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(YScaleSizer.Div())*(p+1), labelStr, settings());
		mPlotBits.append(new oofGeoYTickLabel
			((int)Data.unScaleDataValue(YScaleSizer.Div())*(p+1), YtickLabelLen, labelStr));
	}
	// add the title
	mPlotBits.append(new oofGeoTitle(I.right-I.left,theTitle));
	// add X label
	mPlotBits.append(new oofGeoXLabel(I.right-I.left,Data.XField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.YField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,true /* Use Symbols */));
}


// ---------------------------------------------------------------------------------
//		 oofBarGraph
// ---------------------------------------------------------------------------------


oofBarGraph::oofBarGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofBarGraph::clone() const
{
	return new oofBarGraph(*this);
}


void
oofBarGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long xAxisHeight)
{
	// david debug 990908
	//#ifdef _Macintosh
	//	short *theSize = &(qd.thePort->txSize);
	//#endif
	
	long CurTop=0;
	unsigned long SeriesHeight=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
	mType = "bar";
	
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr;
	
	oofGraphData Data(theView);

	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			if(CurTop) {
				if(!SeriesHeight)
					SeriesHeight=CurTop;
				mPlotBits.append(new oofGeoYTickLabel(CurTop-SeriesHeight/2,YtickLabelLen,thePrevStr));
				CurTop+=SeriesSpacing;
			}
		}
		mPlotBits.append(new oofGeoBar(CurTop,BarWidth,Data.dataValue(),
						 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField())));
		thePrevStr.adopt(theFirstFieldStr);
		CurTop+=(BarWidth+Spacing);
	}
	CurTop-=Spacing;
	mPlotBits.append(new oofGeoYTickLabel(CurTop-SeriesHeight/2,YtickLabelLen,thePrevStr));
	// add the X axis
	long theAxisHeight;
	if(xAxisHeight)
		theAxisHeight=xAxisHeight;
	else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().right);
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoXAxis(Data.unScaleDataValue(ScaleSizer.Max()),
									 Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the X axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		short round=(int)Data.unScaleDataValue(ScaleSizer.Div())%2;
		mPlotBits.append(new oofGeoXTickLabel
			((int)(Data.unScaleDataValue(ScaleSizer.Div())*(p+1)-Data.unScaleDataValue(ScaleSizer.Div())/2),
			 Data.unScaleDataValue(ScaleSizer.Div())+round, labelStr));
	}	

	// add the Y axis
	mPlotBits.append(new oofGeoYAxis(CurTop,SeriesHeight+SeriesSpacing,0));
	// add the title
	// add the title
	mPlotBits.append(new oofGeoTitle(theAxisHeight,theTitle));	
	// add X label
	mPlotBits.append(new oofGeoXLabel(theAxisHeight,Data.DataField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.FirstField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,false /* Use Symbols */));
	
	//FinishConstruction();
}


// ---------------------------------------------------------------------------------
//		 oofStackedBarGraph
// ---------------------------------------------------------------------------------


oofStackedBarGraph::oofStackedBarGraph(dbView* inView, unsigned long numOverlaid) :
	OOF_mixOverlayableGraph(numOverlaid)
{
	setView(inView);
}


oofGraph* 
oofStackedBarGraph::clone() const
{
	return new oofStackedBarGraph(*this);
}

void
oofStackedBarGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long xAxisHeight)
{
	// david debug 990906
	//#ifdef _Macintosh
	//		short *theFont = &(qd.thePort->txFont);
	//	#endif
	// end david debug
	long CurTop=-1;
	long StackPos=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
		
	unsigned long numSeries=0;
	unsigned long seriesNum;
	mType = "stacked bar";
		
	oofGraphData Data(theView,mOverlayView);

	// Prescan to see how many series there are and set up the oofSeriesSet according to 
	// how many are overlaid !
	//
	// NOTE: We boldly assume that all items have the same number of series
	//       Perhaps oofSeriesSet.findSeriesByName() should have a "non-creative"
	//       version !?
	theView->source()->start();
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr = ((char *)Data.FirstField()->binaryContents());
	for (; theView->source()->more();theView->source()->next()) {
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			break;
		}
		oofSeries *newSeries=new oofSeries(*(dbChar*)Data.SeriesField());
		mSeriesSet.append(newSeries);
		thePrevStr.adopt(theFirstFieldStr);
		numSeries++;
	}
	
	// adjust user specifications against actual data
	// was previously an assert which caused annoying crashes!
	if (mNumOverlaid>=numSeries) {
		if (numSeries>0)
			mNumOverlaid = numSeries-1;
		else
			mNumOverlaid = 0;
	}

	for(seriesNum=numSeries-1;seriesNum>(numSeries-mNumOverlaid-1);seriesNum--)
		mSeriesSet[seriesNum]->makeSymbol();
		
	seriesNum=numSeries;
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		if (seriesNum==numSeries) {
			seriesNum=0;
			if(CurTop==-1)
				CurTop=0;
			else
				CurTop+=(BarWidth+Spacing);
			mPlotBits.append(new oofGeoYTickLabel(CurTop+BarWidth/2, YtickLabelLen,*(dbChar*)Data.FirstField()));
			mPlotBits.append(new oofGeoBar(CurTop,BarWidth,Data.dataValue(),
							 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField())));
			StackPos=Data.dataValue();
		} else {
			if(seriesNum>=(numSeries-mNumOverlaid)){
				// Overlaid Plot items !
				mPlotBits.append(new oofGeoPoint(Data.dataValue(),CurTop+(BarWidth/2),
								 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField())));
			} else {
				mPlotBits.append(new oofGeoBar(StackPos,CurTop,BarWidth,Data.dataValue(),
								 mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField()),oofGeo::posNone));		
				StackPos+=Data.dataValue();			
			}
		}
		seriesNum++;
	}
	if(mOverlayView){
		unsigned long CurOverlayTop=0;
		oofSeries *newSeries=new oofSeries(Data.OverlayField()->fieldName(), true /* Symbol */);
		mSeriesSet.append(newSeries);
		for (mOverlayView->source()->start(); mOverlayView->source()->more();
			 mOverlayView->source()->next()) {
			mPlotBits.append(new oofGeoPoint(Data.overlayValue(),CurOverlayTop+(BarWidth/2),newSeries));
			CurOverlayTop+=(BarWidth+Spacing);
		}
	}
	// add the X axis
	long theAxisHeight;
	if(xAxisHeight)
		theAxisHeight=xAxisHeight;
	else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().right);
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoXAxis(Data.unScaleDataValue(ScaleSizer.Max()),
									 Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the X axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		short round=(int)Data.unScaleDataValue(ScaleSizer.Div())%2;
		mPlotBits.append(new oofGeoXTickLabel
			((int)(Data.unScaleDataValue(ScaleSizer.Div())*(p+1)-Data.unScaleDataValue(ScaleSizer.Div())/2),
			 Data.unScaleDataValue(ScaleSizer.Div())+round, labelStr));
	}	
	// add the Y axis
	mPlotBits.append(new oofGeoYAxis(CurTop+BarWidth,BarWidth+Spacing,0));
	// add the title
	mPlotBits.append(new oofGeoTitle(theAxisHeight,theTitle));

// on BRAD's source
//		mPlotBits.append(new oofGeoTitle(mPlotBits.getIdealRect().width(),theTitle));

// add X label
	mPlotBits.append(new oofGeoXLabel(theAxisHeight,Data.DataField()->fieldName()));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.FirstField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,false /* Use Symbols */));
	
	//FinishConstruction();
}


// ---------------------------------------------------------------------------------
//		 oofOrdinalStackedBarGraph
// ---------------------------------------------------------------------------------
oofOrdinalStackedBarGraph::oofOrdinalStackedBarGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofOrdinalStackedBarGraph::clone() const
{
	return new oofOrdinalStackedBarGraph(*this);
}



void
oofOrdinalStackedBarGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long xAxisHeight)
{
	long CurTop=-1;
	long StackPos=0;
	const unsigned short YtickLabelLen = settings()->maxYTickLabelLen();
		
	unsigned long numSeries=0;
	unsigned long seriesNum;
	OOF_String theFirstFieldStr;
	OOF_String thePrevStr;
	OOF_String xLabel;
	bool doneXLabel=false;
	mType = "ordinal stacked bar";
			
	oofGraphData Data(theView,mOverlayView);

	dbField *theSeriesField = (dbField  *) (*theView)[3]; 	// Hideously unsafe downcast below..
	// Make sure the View is the one we expect !
	assert(theSeriesField->fieldType()==charField);
	
	xLabel = Data.DataField()->fieldName();

	// camerons hack -- get rid of x label cuffoonie
	
	
//	xLabel += "\n";

	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		// Get the data out of the View
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			seriesNum=0;
			if(CurTop==-1) {
			//	xLabel += (char *)Data.SeriesField()->binaryContents();
				CurTop=0;
			} else {
				CurTop+=(BarWidth+Spacing);
				doneXLabel=true;
			}
			mPlotBits.append(new oofGeoYTickLabel(CurTop+BarWidth/2, YtickLabelLen,*(dbChar*)Data.FirstField()));
			mPlotBits.append(new oofGeoBar(CurTop,BarWidth,Data.dataValue(),
							 mSeriesSet.findSeriesByName(*(dbChar*)theSeriesField)));
			StackPos=Data.dataValue();
		} else {
			mPlotBits.append(new oofGeoBar(StackPos,CurTop,BarWidth,Data.dataValue(),
							 mSeriesSet.findSeriesByName(*(dbChar*)theSeriesField),oofGeo::posNone));		
			StackPos+=Data.dataValue();			
//			if(!doneXLabel){
//				xLabel += " - ";
//				xLabel += (char *)Data.SeriesField()->binaryContents();
//			}
		}
		thePrevStr.adopt(theFirstFieldStr);
	}
	
	if(mOverlayView){
		unsigned long CurOverlayTop=0;
		oofSeries *newSeries=new oofSeries(Data.OverlayField()->fieldName(), true /* Symbol */);
		mSeriesSet.append(newSeries);
		for (mOverlayView->source()->start(); mOverlayView->source()->more();
			 mOverlayView->source()->next()) {
			mPlotBits.append(new oofGeoPoint(Data.overlayValue(),CurOverlayTop+(BarWidth/2),newSeries));
			CurOverlayTop+=(BarWidth+Spacing);
		}
	}
	// add the X axis
	long theAxisHeight;
	if(xAxisHeight)
		theAxisHeight=xAxisHeight;
	else
		theAxisHeight=Data.scaleDataValue((long)mPlotBits.getIdealRect().right);
	oofScaleSizer ScaleSizer(theAxisHeight);
	theAxisHeight=Data.unScaleDataValue(theAxisHeight);
	mPlotBits.append(new oofGeoXAxis(Data.unScaleDataValue(ScaleSizer.Max()),
									 Data.unScaleDataValue(ScaleSizer.Div()),ScaleSizer.NumMinorTicks()));
	// add the X axis labels
	for(unsigned p=0;p<ScaleSizer.NumDivs();p++){
		char labelStr[255];
		Data.setScaleString(Data.unScaleDataValue(ScaleSizer.Div())*(p+1), labelStr, settings());
		short round=(int)Data.unScaleDataValue(ScaleSizer.Div())%2;
		mPlotBits.append(new oofGeoXTickLabel
			((int)(Data.unScaleDataValue(ScaleSizer.Div())*(p+1)-Data.unScaleDataValue(ScaleSizer.Div())/2),
			 Data.unScaleDataValue(ScaleSizer.Div())+round, labelStr));
	}	
	// add the Y axis
	mPlotBits.append(new oofGeoYAxis(CurTop+BarWidth,BarWidth+Spacing,0));
	// add the title
	mPlotBits.append(new oofGeoTitle(theAxisHeight,theTitle));
	// add X label
	mPlotBits.append(new oofGeoXLabel(theAxisHeight,xLabel));
	// add Y label
	mPlotBits.append(new oofGeoYLabel(Data.FirstField()->fieldName()));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,false /* Use Symbols */));
	
	//FinishConstruction();
}


// oofScaleSizer

oofScaleSizer::oofScaleSizer(double ymax)
{
	const short kDivs[5]={0,1,2,5,10}; // Must be an order 10 repeating set !
									   // This is used for base ascending values

	short order=log10(ymax);
	
	short mult=ymax/pow(10.0,order);
	
	double sub=ymax-mult*pow(10.0,order);
	short submult=sub/pow(10.0,order-1);
	double product=(double)submult*pow(10.0,order-1);
	if(sub!=product)
		submult++;	// Make sure we at least encapsulate the maximum value
		
	mMax=mult*pow(10.0,order);
	unsigned i=0;
	if((submult<=mult)&&(submult!=0))
		submult=mult+1;
	while(submult>kDivs[i])
		i++;
	mMax+=kDivs[i]*pow(10.0,order-1);
	
	if(kDivs[i]) {
		mDiv=kDivs[i]*pow(10.0,order-1);
		mNumMinorTicks=kDivs[i];	
	} else {
		if(order==log10(ymax)) //  We have an actual factor of ten !
			mDiv=pow(10.0,order-1);
		else
			mDiv=pow(10.0,order);
		mNumMinorTicks=2;	
	}
	
	while((mMax/mDiv)<kMinDivs)
		mDiv=mDiv/2;
	while((mMax/mDiv)>kMaxDivs) {
		mDiv=mDiv*2;
		if((mMax/mDiv)!=(double)(long)(mMax/mDiv)) {
			mMax+=mDiv/2;
		}
	}
	
	mNumDivs=mMax/mDiv;
}

// ---------------------------------------------------------------------------------
//		 oofPieGraph
// ---------------------------------------------------------------------------------


oofPieGraph::oofPieGraph(dbView* inView)
{
	setView(inView);
}


oofGraph* 
oofPieGraph::clone() const
{
	return new oofPieGraph(*this);
}


void
oofPieGraph::finishConstruction(dbView *theView, const char* theTitle, unsigned long /* meaningless*/)
{	
	int WhichSeries=0;	// Placeholder for a future setting to allow for choosing which series
						// We may also want to offer the ability to choose the series by name.
						//
						// NOTE: The series number is zero based !

	mType = "pie";
	
	oofGraphData Data(theView);	

	// Scan for the n'th series and find out the cumulative total of its values
	//
	// NOTE: If the series number specified is larger than the number of actual series
	//       then we will just get the last series.
	theView->source()->start();
	OOF_String theFirstFieldStr;
	unsigned seriesNum=0;
	unsigned long Seriestotal=0;
	OOF_String thePrevStr = ((char *)Data.FirstField()->binaryContents());
	for (; theView->source()->more();theView->source()->next()) {
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr) {
			if(seriesNum==WhichSeries) // Have we processed the series we want ?
				break;
			Seriestotal=0;
			seriesNum++;
			thePrevStr = ((char *)Data.FirstField()->binaryContents());
		}
		Seriestotal+=Data.dataValue();
	}
	
	// OK - now we scan forward for the first item to chart, and actually add the items
	//      thePrevStr still holds the name of the series we want.
	//
	bool done=false;
	double start=0;
	long roundedstart=0;
	for (theView->source()->start(); theView->source()->more();theView->source()->next()) {
		theFirstFieldStr = (char *)Data.FirstField()->binaryContents();
		if (theFirstFieldStr != thePrevStr)
			if (done)
				break;
			else
				continue;
		// We've got to the data - add the sectors
		double arc=((double)Data.dataValue()/(double)Seriestotal)*360.0;
		mPlotBits.append(new oofGeoSector(0, 0, ChartDiameter, roundedstart, arc+0.5, mSeriesSet.findSeriesByName(*(dbChar*)Data.SeriesField()) , oofGeo::posNone));
		start+=arc;
		roundedstart=start+0.5;
	}
	
	// add the title - with the series title as well - perhaps this should be a setting.
	OOF_String TitleStr(theTitle);
	TitleStr+="\n";
	TitleStr+=thePrevStr;
	mPlotBits.append(new oofGeoTitle(ChartDiameter,TitleStr));
	// add a legend
	mPlotBits.append(new oofGeoLegend(&mSeriesSet,false /* Use Symbols */));
}
