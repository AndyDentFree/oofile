// COPYRIGHT 1996 A.D. Software, All rights reserved

/*
oofSting.cp
*/
#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_oofSting
	#include "oofSting.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif

// static variables
oofGraphSettings* 	oofGraphDefaultSettings::sDefaultSettings;

oofGraphDefaultSettings		oofGraphDefaultSettings::sDefaultSettingsFactory;	// the Singleton

// -------------------------------------------------------
//            o o f G r a p h S e t t i n g s
// -------------------------------------------------------

oofGraphSettings::oofGraphSettings() : 
	mRefCount(1),
	mHasLegend(0),
	mHas3DEffect(0),
	mHasTitle(0),
	mHasValuesOverColumns(0),
	mHasValuesNextToBars(0),
	mHasXAxisTicks(0),
	mHasYAxisTicks(0),
	mHasXTickLabels(0),
	mHasYTickLabels(0),
	mTitlePos(0),
	mXLabelPos(0),
	mYLabelPos(0),
	mMaxYLabelLen(0),
	mMaxLegendLen(0),
	mMinorTickSize(0),
	mMajorTickSize(0),
	mYAxisHeight(0),
	mLegendOffset(0),
	m3DEffectMultiplier(0),
	mHasXLabel(0),
	mHasYLabel(0),
	mMaxYTickLabelLen(0),
	mIntTickLabelMask(0),
	mRealTickLabelMask(0)
{
}


oofGraphSettings::oofGraphSettings(oofGraphSettings* rhs) :
	mRefCount(1),
	mHasLegend(0),
	mHas3DEffect(0),
	mHasTitle(0),
	mHasValuesOverColumns(0),
	mHasValuesNextToBars(0),
	mHasXAxisTicks(0),
	mHasYAxisTicks(0),
	mHasXTickLabels(0),
	mHasYTickLabels(0),
	mTitlePos(0),
	mXLabelPos(0),
	mYLabelPos(0),
	mMaxYLabelLen(0),
	mMaxLegendLen(0),
	mMinorTickSize(0),
	mMajorTickSize(0),
	mYAxisHeight(0),
	mLegendOffset(0),
	m3DEffectMultiplier(0),
	mHasXLabel(0),
	mHasYLabel(0),
	mMaxYTickLabelLen(0),
	mIntTickLabelMask(0),
	mRealTickLabelMask(0)
{
	if (rhs->mHasLegend)
		hasLegend(*rhs->mHasLegend);
	
	if (rhs->mHas3DEffect)
		has3DEffect(*rhs->mHas3DEffect);
	
	if (rhs->mHasTitle)
		hasTitle(*rhs->mHasTitle);
	
	if (rhs->mHasValuesOverColumns)
		hasValuesOverColumns(*rhs->mHasValuesOverColumns);
	
	if (rhs->mHasValuesNextToBars)
		hasValuesNextToBars(*rhs->mHasValuesNextToBars);
	
	if (rhs->mHasXAxisTicks)
		hasXAxisTicks(*rhs->mHasXAxisTicks);
	
	if (rhs->mHasYAxisTicks)
		hasYAxisTicks(*rhs->mHasYAxisTicks);
	
	if (rhs->mHasXTickLabels)
		hasXTickLabels(*rhs->mHasXTickLabels);
	
	if (rhs->mHasYTickLabels)
		hasYTickLabels(*rhs->mHasYTickLabels);
	
	if (rhs->mTitlePos)
		titlePos(*rhs->mTitlePos);
	
	if (rhs->mXLabelPos)
		XLabelPos(*rhs->mXLabelPos);
	
	if (rhs->mYLabelPos)
		YLabelPos(*rhs->mYLabelPos);
	
	if (rhs->mMaxYLabelLen)
		maxYLabelLen(*rhs->mMaxYLabelLen);
	
	if (rhs->mMaxLegendLen)
		maxLegendLen(*rhs->mMaxLegendLen);
	
	if (rhs->mMinorTickSize)
		minorTickSize(*rhs->mMinorTickSize);
	
	if (rhs->mMajorTickSize)
		majorTickSize(*rhs->mMajorTickSize);
	
	if (rhs->mLegendOffset)
		legendOffset(*rhs->mLegendOffset);
		
	if (rhs->m3DEffectMultiplier)
		the3DEffectMultiplier(*rhs->m3DEffectMultiplier);
		
	if (rhs->mYAxisHeight)
		YAxisHeight(*rhs->mYAxisHeight);
		
	if (rhs->mHasXLabel)
		hasXLabel(*rhs->mHasXLabel);
	
	if (rhs->mHasYLabel)
		hasYLabel(*rhs->mHasYLabel);
	
	if (rhs->mMaxYTickLabelLen)
		maxYTickLabelLen(*rhs->mMaxYTickLabelLen);
		
	if (rhs->mIntTickLabelMask)
		intTickLabelMask(*rhs->mIntTickLabelMask);
	
	if (rhs->mRealTickLabelMask)
		realTickLabelMask(*rhs->mRealTickLabelMask);
}


oofGraphSettings::~oofGraphSettings()
{
	if (mRefCount <= 1) {
		delete mHasLegend;
		delete mHas3DEffect;
		delete mHasTitle;
		delete mHasValuesOverColumns;
		delete mHasValuesNextToBars;
		delete mHasXAxisTicks;
		delete mHasYAxisTicks;
		delete mHasXTickLabels;
		delete mHasYTickLabels;
		delete mTitlePos;
		delete mXLabelPos;
		delete mYLabelPos;
		delete mMaxYLabelLen;
		delete mMaxLegendLen;
		delete mMinorTickSize;
		delete mMajorTickSize;
		delete mYAxisHeight;
		delete mLegendOffset;
		delete m3DEffectMultiplier;
		delete mHasXLabel;
		delete mHasYLabel;
		delete mMaxYTickLabelLen;
		delete	mIntTickLabelMask;
		delete mRealTickLabelMask;
	}
	else
		--mRefCount;
}


bool
oofGraphSettings::hasLegend() const
{
	if (mHasLegend)
		return *mHasLegend;
	else
		return oofGraphDefaultSettings::settings()->hasLegend();
}

void
oofGraphSettings::hasLegend(bool isLegend)
{
	if (mHasLegend)
		*mHasLegend = isLegend;
	else
		mHasLegend = new bool(isLegend);
}


bool
oofGraphSettings::has3DEffect() const
{
	if (mHas3DEffect)
		return *mHas3DEffect;
	else
		return oofGraphDefaultSettings::settings()->has3DEffect();
}

void
oofGraphSettings::has3DEffect(bool is3DEffect)
{
	if (mHas3DEffect)
		*mHas3DEffect = is3DEffect;
	else
		mHas3DEffect = new bool(is3DEffect);
}


bool
oofGraphSettings::hasTitle() const
{
	if (mHasTitle)
		return *mHasTitle;
	else
		return oofGraphDefaultSettings::settings()->hasTitle();
}

void
oofGraphSettings::hasTitle(bool isTitle)
{
	if (mHasTitle)
		*mHasTitle = isTitle;
	else
		mHasTitle = new bool(isTitle);
}


bool
oofGraphSettings::hasValuesOverColumns() const
{
	if (mHasValuesOverColumns)
		return *mHasValuesOverColumns;
	else
		return oofGraphDefaultSettings::settings()->hasValuesOverColumns();
}

void
oofGraphSettings::hasValuesOverColumns(bool showValues)
{
	if (mHasValuesOverColumns)
		*mHasValuesOverColumns = showValues;
	else
		mHasValuesOverColumns = new bool(showValues);
}


bool
oofGraphSettings::hasValuesNextToBars() const
{
	if (mHasValuesNextToBars)
		return *mHasValuesNextToBars;
	else
		return oofGraphDefaultSettings::settings()->hasValuesNextToBars();
}

void
oofGraphSettings::hasValuesNextToBars(bool showValues)
{
	if (mHasValuesNextToBars)
		*mHasValuesNextToBars = showValues;
	else
		mHasValuesNextToBars = new bool(showValues);
}


bool
oofGraphSettings::hasXAxisTicks() const
{
	if (mHasXAxisTicks)
		return *mHasXAxisTicks;
	else
		return oofGraphDefaultSettings::settings()->hasXAxisTicks();
}

void
oofGraphSettings::hasXAxisTicks(bool hasTicks)
{
	if (mHasXAxisTicks)
		*mHasXAxisTicks = hasTicks;
	else
		mHasXAxisTicks = new bool(hasTicks);
}


bool
oofGraphSettings::hasYAxisTicks() const
{
	if (mHasYAxisTicks)
		return *mHasYAxisTicks;
	else
		return oofGraphDefaultSettings::settings()->hasYAxisTicks();
}

void
oofGraphSettings::hasYAxisTicks(bool hasTicks)
{
	if (mHasYAxisTicks)
		*mHasYAxisTicks = hasTicks;
	else
		mHasYAxisTicks = new bool(hasTicks);
}


bool
oofGraphSettings::hasXTickLabels() const
{
	if (mHasXTickLabels)
		return *mHasXTickLabels;
	else
		return oofGraphDefaultSettings::settings()->hasXTickLabels();
}

void
oofGraphSettings::hasXTickLabels(bool hasLabels)
{
	if (mHasXTickLabels)
		*mHasXTickLabels = hasLabels;
	else
		mHasXTickLabels = new bool(hasLabels);
}


bool
oofGraphSettings::hasYTickLabels() const
{
	if (mHasYTickLabels)
		return *mHasYTickLabels;
	else
		return oofGraphDefaultSettings::settings()->hasYTickLabels();
}

void
oofGraphSettings::hasYTickLabels(bool hasLabels)
{
	if (mHasYTickLabels)
		*mHasYTickLabels = hasLabels;
	else
		mHasYTickLabels = new bool(hasLabels);
}


bool
oofGraphSettings::hasXLabel() const
{
	if (mHasXLabel)
		return *mHasXLabel;
	else
		return oofGraphDefaultSettings::settings()->hasXLabel();
}


void
oofGraphSettings::hasXLabel(bool hasLabel)
{
	if (mHasXLabel)
		*mHasXLabel = hasLabel;
	else
		mHasXLabel = new bool(hasLabel);
}


bool
oofGraphSettings::hasYLabel() const
{
	if (mHasYLabel)
		return *mHasYLabel;
	else
		return oofGraphDefaultSettings::settings()->hasYLabel();
}


void
oofGraphSettings::hasYLabel(bool hasLabel)
{
	if (mHasYLabel)
		*mHasYLabel = hasLabel;
	else
		mHasYLabel = new bool(hasLabel);
}


oofGeo::PositionState
oofGraphSettings::titlePos() const
{
	if (mTitlePos)
		return *mTitlePos;
	else
		return oofGraphDefaultSettings::settings()->titlePos();
}


void
oofGraphSettings::titlePos(oofGeo::PositionState pos)
{
	if (mTitlePos)
		*mTitlePos = pos;
	else
		mTitlePos = new oofGeo::PositionState(pos);
}


oofGeo::PositionState
oofGraphSettings::XLabelPos() const
{
	if (mXLabelPos)
		return *mXLabelPos;
	else
		return oofGraphDefaultSettings::settings()->XLabelPos();
}


void
oofGraphSettings::XLabelPos(oofGeo::PositionState pos)
{
	if (mXLabelPos)
		*mXLabelPos = pos;
	else
		mXLabelPos = new oofGeo::PositionState(pos);
}


oofGeo::PositionState
oofGraphSettings::YLabelPos() const
{
	if (mYLabelPos)
		return *mYLabelPos;
	else
		return oofGraphDefaultSettings::settings()->YLabelPos();
}


void
oofGraphSettings::YLabelPos(oofGeo::PositionState pos)
{
	if (mYLabelPos)
		*mYLabelPos = pos;
	else
		mYLabelPos = new oofGeo::PositionState(pos);
}

	
unsigned short	
oofGraphSettings::maxYLabelLen() const
{
	if (mMaxYLabelLen)
		return *mMaxYLabelLen;
	else
		return oofGraphDefaultSettings::settings()->maxYLabelLen();
}


void			
oofGraphSettings::maxYLabelLen(unsigned short n)
{
	if (mMaxYLabelLen)
		*mMaxYLabelLen = n;
	else
		mMaxYLabelLen = new unsigned short(n);
}

	
unsigned short	
oofGraphSettings::maxYTickLabelLen() const
{
	if (mMaxYTickLabelLen)
		return *mMaxYTickLabelLen;
	else
		return oofGraphDefaultSettings::settings()->maxYTickLabelLen();
}


void			
oofGraphSettings::maxYTickLabelLen(unsigned short n)
{
	if (mMaxYTickLabelLen)
		*mMaxYTickLabelLen = n;
	else
		mMaxYTickLabelLen = new unsigned short(n);
}

	
unsigned short	
oofGraphSettings::maxLegendLen() const
{
	if (mMaxLegendLen)
		return *mMaxLegendLen;
	else
		return oofGraphDefaultSettings::settings()->maxLegendLen();
}


void			
oofGraphSettings::maxLegendLen(unsigned short n)
{
	if (mMaxLegendLen)
		*mMaxLegendLen = n;
	else
		mMaxLegendLen = new unsigned short(n);
}

	
unsigned short	
oofGraphSettings::minorTickSize() const
{
	if (mMinorTickSize)
		return *mMinorTickSize;
	else
		return oofGraphDefaultSettings::settings()->minorTickSize();
}


void			
oofGraphSettings::minorTickSize(unsigned short n)
{
	if (mMinorTickSize)
		*mMinorTickSize = n;
	else
		mMinorTickSize = new unsigned short(n);
}

	
unsigned short	
oofGraphSettings::majorTickSize() const
{
	if (mMajorTickSize)
		return *mMajorTickSize;
	else
		return oofGraphDefaultSettings::settings()->majorTickSize();
}


void			
oofGraphSettings::majorTickSize(unsigned short n)
{
	if (mMajorTickSize)
		*mMajorTickSize = n;
	else
		mMajorTickSize = new unsigned short(n);
}

	
unsigned short	
oofGraphSettings::YAxisHeight() const
{
	if (mYAxisHeight)
		return *mYAxisHeight;
	else
		return oofGraphDefaultSettings::settings()->YAxisHeight();
}


void			
oofGraphSettings::YAxisHeight(unsigned short n)
{
	if (mYAxisHeight)
		*mYAxisHeight = n;
	else
		mYAxisHeight = new unsigned short(n);
}


unsigned short	
oofGraphSettings::legendOffset() const
{
	if (mLegendOffset)
		return *mLegendOffset;
	else
		return oofGraphDefaultSettings::settings()->legendOffset();
}


void			
oofGraphSettings::legendOffset(unsigned short n)
{
	if (mLegendOffset)
		*mLegendOffset = n;
	else
		mLegendOffset = new unsigned short(n);
}


double	
oofGraphSettings::the3DEffectMultiplier() const
{
	if (m3DEffectMultiplier)
		return *m3DEffectMultiplier;
	else
		return oofGraphDefaultSettings::settings()->the3DEffectMultiplier();
}


void			
oofGraphSettings::the3DEffectMultiplier(double n)
{
	if (m3DEffectMultiplier)
		*m3DEffectMultiplier = n;
	else
		m3DEffectMultiplier = new double(n);
}


const char*		
oofGraphSettings::intTickLabelMask() const
{
	if (mIntTickLabelMask)
		return *mIntTickLabelMask;
	else
		return oofGraphDefaultSettings::settings()->intTickLabelMask();
}


void			
oofGraphSettings::intTickLabelMask(const char* str)
{
	if (mIntTickLabelMask)
		*mIntTickLabelMask = str;
	else
		mIntTickLabelMask = new OOF_String(str);

}
	
		
const char*		
oofGraphSettings::realTickLabelMask() const
{
	if (mRealTickLabelMask)
		return *mRealTickLabelMask;
	else
		return oofGraphDefaultSettings::settings()->realTickLabelMask();
}


void			
oofGraphSettings::realTickLabelMask(const char* str)
{
	if (mRealTickLabelMask)
		*mRealTickLabelMask = str;
	else
		mRealTickLabelMask = new OOF_String(str);
}


unsigned int&
oofGraphSettings::refcount()
{
	return mRefCount;
}



// -------------------------------------------------------
//       o o f G r a p h D e f a u l t S e t t i n g s 
// -------------------------------------------------------

oofGraphDefaultSettings::oofGraphDefaultSettings()
{
	if (sDefaultSettings==0) {
		sDefaultSettings = new oofGraphSettings;
		/* Note that at this point we must provide default values
		for any states which the oofGraphSettings object tracks, or else
		we get serious problems. Beware.
		*/
		sDefaultSettings->hasLegend(true);
		sDefaultSettings->has3DEffect(false);
		sDefaultSettings->hasTitle(true);
		sDefaultSettings->hasValuesOverColumns(false);
		sDefaultSettings->hasValuesNextToBars(false);
		sDefaultSettings->hasXLabel(true);
		sDefaultSettings->hasYLabel(true);
		sDefaultSettings->hasXAxisTicks(true);
		sDefaultSettings->hasYAxisTicks(true);
		sDefaultSettings->hasXTickLabels(true);
		sDefaultSettings->hasYTickLabels(true);
		sDefaultSettings->titlePos(oofGeo::posCentre);
		sDefaultSettings->XLabelPos(oofGeo::posBelow);
		sDefaultSettings->YLabelPos(oofGeo::posLeft);
// all figures in points		
		sDefaultSettings->maxYLabelLen(200);
		sDefaultSettings->maxLegendLen(100);
		sDefaultSettings->minorTickSize(3);
		sDefaultSettings->majorTickSize(5);
		sDefaultSettings->YAxisHeight(0);
		sDefaultSettings->legendOffset(0);
		sDefaultSettings->the3DEffectMultiplier(0.5);
		sDefaultSettings->maxYTickLabelLen(100);
		sDefaultSettings->intTickLabelMask("%d");
		sDefaultSettings->realTickLabelMask("%5.0f");
	}
}


oofGraphDefaultSettings::~oofGraphDefaultSettings()
{
	if (--sDefaultSettings->refcount()==0)
		delete sDefaultSettings;
}
