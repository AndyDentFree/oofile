#ifndef H_OOFREPMAC
#define H_OOFREPMAC

// COPYRIGHT 1997 A.D. Software, All rights reserved

// GUI report-writer layer of OOFILE database


#ifndef H_OOFFILE
	#include "oofile.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif
#ifndef H_OOFDRAW
	#include "oofdraw.h"
#endif

#if !defined(TARGET_RT_MAC_MACHO) || !TARGET_RT_MAC_MACHO
	#include <Printing.h>
#endif

/**
	\defgroup oofRenderMac Macintosh Report rendering
	Reports rendered to the Macintosh GUI drawing environment
	able to be previewed and printed under Classic MacOS
	and MacOS/X using Carbon.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/


// use common base to provide the factory methods
/**
	Base for specifying Macintosh GUI drawing environment.
	\ingroup oofRenderMac
*/
class oofRepMacEnv :  public oofRepEnvironment {
public:
	enum 	{eRenderType='Mac '};
	
	oofRepMacEnv() :
		mPrintRecordH(0),
		mSavedPort(0)
	{}
	
	THPrint		pageSetup() const { return mPrintRecordH; }
	
	void pageSetup(THPrint);
	
	// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;
	
	virtual void	drawPageBorder( long top, long left, long bottom, long right );
		
protected:
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Handle			mFlatFormat;
	Handle			mFlatSettings;

#endif
	THPrint			mPrintRecordH;  // OWNED - Macintosh Handle, setup by children
	GrafPtr			mSavedPort;
};


// Macintosh renderers for OOFILE database

/**
	Advance report vertical position.
	\ingroup oofRenderMac
*/
class oofRepSpaceBand_MacRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_MacRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Draw colour or mono graph as specified by oofRepGraphBand.
	Depends on oofRepSettings::printGraphsInMono().
	\ingroup oofRenderMac
*/
class oofRepGraphBand_MacRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_MacRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Use oofRepGraphBand_MacRender::draw to draw colour or mono graph as specified by oofRepGraphBand.
	Depends on oofRepSettings::printGraphsInMono().
	\todo confirm still needed - oofRepGraphBand_MacRender::draw seems to check printing.
	\ingroup oofRenderMac
*/
class oofRepGraphBand_MacPrintRender : public oofRepGraphBand_MacRender {
public:
	oofRepGraphBand_MacPrintRender(oofRepGraphBand* b) : oofRepGraphBand_MacRender(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate blocks in oofRepLayoutBand changing pen position for each.
	\see oofRepLayoutRedirectingRender::addAreaForCurrentBlock. 
	\ingroup oofRenderMac
*/
class oofRepLayoutBand_MacRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_MacRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Draw a line as filled rectangle specified by oofRepLineBand.
	\ingroup oofRenderMac
*/
class oofRepLineBand_MacRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_MacRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Call oofPict::draw positioned centred within oofRepPictBand.
	\ingroup oofRenderMac 
*/
class oofRepPictBand_MacRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_MacRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Draw oofRepViewBand contents as grid with page breaks.
	Includes word wrapping of column headers via CalcColHeaderWrapping
	\ingroup oofRenderMac
*/
class oofRepViewBand_MacRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_MacRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void finishConstruction ();

	virtual void draw();	
	virtual void drawColumnHeaders();
	virtual void startTableBody(bool headersHaveChanged);
	
protected:
	void DrawContinuingOverPage(unsigned long, unsigned long, oofSizedTextArray&, FontInfo&, bool hasCustomDrawers);
	void CalcColHeaderWrapping();
	oofSizedTextArray	mColHeaders;	
};


/**
	Draw text in the current style.
	\ingroup oofRenderMac
*/
class oofRepTextBlock_MacRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_MacRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual unsigned long width() const;
	virtual void draw();
};


/**
	Call oofPict::draw positioned centred within oofRepPictBlock.
	\ingroup oofRenderMac 
*/
class oofRepPictBlock_MacRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_MacRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual unsigned long width() const;
	virtual void draw();
};

#endif

