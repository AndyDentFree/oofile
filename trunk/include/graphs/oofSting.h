// COPYRIGHT 1996 A.D. Software, All rights reserved

/*
oofSting.h

Contains all the setting options for a graph

*/
#ifndef H_oofSting
#define H_oofSting

#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#ifndef H_OOFGEOS
	#include "oofGeos.h"
#endif

/**
	Settings object for overriding behaviours of an instance of oofGraph.
	\see oofGraphDefaultSettings for the defaults you override
	by setting anything with our members.
*/
class OOFGRAPH_EXPORT oofGraphSettings {
public:
		oofGraphSettings();
		oofGraphSettings(oofGraphSettings *);
		~oofGraphSettings();
	bool			hasLegend() const;
	void			hasLegend(bool);
	
	bool			has3DEffect() const;
	void			has3DEffect(bool);
	
	bool			hasTitle() const;
	void			hasTitle(bool);
	
	bool			hasValuesOverColumns() const;
	void			hasValuesOverColumns(bool);
	
	bool			hasValuesNextToBars() const;
	void			hasValuesNextToBars(bool);
	
	bool			hasXLabel() const;
	void			hasXLabel(bool);
	
	bool			hasYLabel() const;
	void			hasYLabel(bool);

	bool			hasXAxisTicks() const;
	void			hasXAxisTicks(bool);
	
	bool			hasYAxisTicks() const;
	void			hasYAxisTicks(bool);
	
	bool			hasXTickLabels() const;
	void			hasXTickLabels(bool);
	
	bool			hasYTickLabels() const;
	void			hasYTickLabels(bool);
	
	oofGeo::PositionState	titlePos() const;
	void			titlePos(oofGeo::PositionState);
	
	oofGeo::PositionState	XLabelPos() const;
	void			XLabelPos(oofGeo::PositionState);
	
	oofGeo::PositionState	YLabelPos() const;
	void			YLabelPos(oofGeo::PositionState);
	
	double			the3DEffectMultiplier() const;
	void			the3DEffectMultiplier(double);
	
	unsigned short	legendOffset() const;
	void			legendOffset(unsigned short);
	
	unsigned short	maxYLabelLen() const;
	void			maxYLabelLen(unsigned short);
	
	unsigned short	maxYTickLabelLen() const;
	void			maxYTickLabelLen(unsigned short);
	
	unsigned short	maxLegendLen() const;
	void			maxLegendLen(unsigned short);
	
	unsigned short	minorTickSize() const;
	void			minorTickSize(unsigned short);
	
	unsigned short	majorTickSize() const;
	void			majorTickSize(unsigned short);
			
	unsigned short	YAxisHeight() const;
	void			YAxisHeight(unsigned short);	
		
	const char*		intTickLabelMask() const;
	void			intTickLabelMask(const char*);
		
	const char*		realTickLabelMask() const;
	void			realTickLabelMask(const char*);
	
		
	unsigned int&	refcount();

private:
// *** WARNING *****
// if you add any settings, remember to update oofGraphDefaultSettings ctor
// as well as our ctor, copy ctor & dtor

// all pointers below here are owned
	bool *			mHasLegend;
	bool *			mHas3DEffect;
	bool *			mHasTitle;
	bool *			mHasValuesOverColumns;
	bool *			mHasValuesNextToBars;
	bool *			mHasXAxisTicks;
	bool *			mHasYAxisTicks;
	bool *			mHasXTickLabels;
	bool *			mHasYTickLabels;
	oofGeo::PositionState *	mTitlePos;
	oofGeo::PositionState *	mXLabelPos;
	oofGeo::PositionState *	mYLabelPos;
	unsigned short*	mMaxYLabelLen;
	unsigned short*	mMaxLegendLen;
	unsigned short*	mMinorTickSize;
	unsigned short*	mMajorTickSize;
	unsigned short*	mYAxisHeight;
	unsigned short*	mLegendOffset;
	double *		m3DEffectMultiplier;
	bool *			mHasXLabel;
	bool *			mHasYLabel;
	unsigned short*	mMaxYTickLabelLen;
	OOF_String*		mIntTickLabelMask;
	OOF_String*		mRealTickLabelMask;

	unsigned int	mRefCount;
};

/**
	Singleton used to initialise oofGraphSettings.
	Obtain this object via oofGraph::defaultSettings().
*/
class oofGraphDefaultSettings {
public:
		oofGraphDefaultSettings();
		~oofGraphDefaultSettings();
	static oofGraphSettings *	settings();
	
private:
	static oofGraphSettings * 	sDefaultSettings;
	static oofGraphDefaultSettings		sDefaultSettingsFactory;	// the Singleton
};


inline oofGraphSettings *
oofGraphDefaultSettings::settings()
{
	return sDefaultSettings;
}

#endif

