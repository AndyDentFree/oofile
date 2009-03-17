// COPYRIGHT 1997 A.D. Software, All rights reserved

//=================================================================================
//	oofGrDta.cpp
//=================================================================================
#include "oofpch_g.h"  // for precompilation of graph files

#include "oofGrDta.h"
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef _STDIO
	#include "stdio.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif

#include "assert.h"

oofGraphData::oofGraphData(dbView* theView, dbView* overlayView)
{
	mFirstField = (dbField  *) (*theView)[(unsigned int)0];
	// Hideously unsafe assumption about the field being a string below
	assert(mFirstField->fieldType()==charField);

	// Discern the valid data types
	mSeriesField = (dbField  *) (*theView)[1];
	switch(mSeriesField->fieldType()){
		case charField: mFormat=standard;
		 				break;
		 				
		case longField: mFormat=xy;
						mSerType=integer;
						mSerScaling=10;
		 				break;
		 				
		case realField: mFormat=xy;
						mSerType=real;
						mSerScaling=100;
		 				break;
		 				
		default: assert(0);
	}	
	
	mDataField = (dbField  *) (*theView)[2];
	switch(mDataField->fieldType()){
		case longField: mDataType=integer;
						mDataScaling=10;
						break;

		case realField: mDataType=real;
						mDataScaling=100;
						break;
		 				
		default: assert(0);
	}
	
	if(overlayView) {
		mOverlayField = (dbField  *) (*overlayView)[(unsigned int)0];
		assert((mOverlayField->fieldType()==longField)||(mOverlayField->fieldType()==realField));
		assert(mOverlayField->fieldType()==mDataField->fieldType());
	} else
		mOverlayField = 0;
}

oofGraphData::~oofGraphData()
{
}

long
oofGraphData::seriesValue()
{
	assert(mFormat==xy);  // Otherwise this call is meaningless !
	
	switch(mSerType){
		case integer: return (*(dbLong*)mSeriesField  * mSerScaling);
					  break; // for completeness !
		
		case real: return (long)(*(dbReal*)mSeriesField  * (double)mSerScaling + 0.5);
				   break; // for completeness !
				   
		default: assert(0);
	}
	
	return 0;   // Should never get here !
}

long
oofGraphData::dataValue()
{
	switch(mDataType){
		case integer: return (*(dbLong*)mDataField  * mDataScaling);
					  break; // for completeness !
		
		case real: return (long)(*(dbReal*)mDataField  * (double)mDataScaling + 0.5);
				   break; // for completeness !

		default: assert(0);
	}
	
	return 0;  // Should never get here !
}

long
oofGraphData::overlayValue()
{
	assert(mOverlayField);  // In case we were never given an overlay view !
	
	switch(mDataType){
		case integer: return (*(dbLong*)mOverlayField  * mDataScaling);
					  break; // for completeness !
		
		case real: return (long)(*(dbReal*)mOverlayField  * (double)mDataScaling + 0.5);
				   break; // for completeness !

		default: assert(0);
	}
	
	return 0;  // Should never get here !
}

long
oofGraphData::scaleSeriesValue(long value)
{
	return (value  / mSerScaling);
}

long
oofGraphData::scaleDataValue(long value)
{
	return (value  / mDataScaling);
}

double
oofGraphData::scaleSeriesValue(double value)
{
	return (value  / (double)mSerScaling);
}

double
oofGraphData::scaleDataValue(double value)
{
	return (value  / (double)mDataScaling);
}

long
oofGraphData::unScaleSeriesValue(long value)
{
	return (value  * mSerScaling);
}

long
oofGraphData::unScaleDataValue(long value)
{
	return (value  * mDataScaling);
}

double
oofGraphData::unScaleSeriesValue(double value)
{
	return (value  * (double)mSerScaling);
}

double
oofGraphData::unScaleDataValue(double value)
{
	return (value  * (double)mDataScaling);
}


char*
oofGraphData::makeScaleString(long scaleVal)
{
	char* labelStr=new char[255];
	
	assert(labelStr);
	
	setScaleString(scaleVal, labelStr, oofGraph::defaultSettings());

	return labelStr;
}


void
oofGraphData::setScaleString(long scaleVal, char* theString, const oofGraphSettings* settings)
{
	switch(mDataType){
	case integer: 
		{
		const char* intMask = settings->intTickLabelMask();
		sprintf(theString,"%d",scaleVal/mDataScaling);
		break;
		}
	case real:
		const char* realMask = settings->realTickLabelMask();
		sprintf(theString, realMask, (double)scaleVal/(double)mDataScaling);
		break;
	}
}
