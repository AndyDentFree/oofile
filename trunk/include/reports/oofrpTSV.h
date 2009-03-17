#ifndef H_OOFREPTSV
#define H_OOFREPTSV

// COPYRIGHT 1997 A.D. Software, All rights reserved

// TSV report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif

/**
	\defgroup oofRenderTSV Tabbed text Report rendering.
	Reports rendered to tab-separated text streams
	suitable for import into spreadsheets or other databases.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

// use common base to provide the factory methods
/**
	Specify output to Tab-Separated-Values stream.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepTSVEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='TSV '};
	
	oofRepTSVEnv();
	oofRepTSVEnv(ostream&);
	oofRepTSVEnv(const char* fileName);
	
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	
	virtual void startEnvironment();
	virtual void endEnvironment();
	virtual bool preprocessesPageHeadersAndFooters() const;

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;
};


// TSV renderers for OOFILE database

/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepSpaceBand_TSVRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_TSVRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Write underlying data for graph to stream.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepGraphBand_TSVRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_TSVRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate blocks in oofRepLayoutBand separated with newlines & tabs.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepLayoutBand_TSVRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_TSVRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepLineBand_TSVRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_TSVRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepPictBand_TSVRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_TSVRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Iterate columns in oofRepViewBand separated with newlines & tabs.
	Includes column headings.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepViewBand_TSVRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_TSVRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();	
	virtual void drawColumnHeaders();
};


/**
	Just output the plain text in the oofRepTextBlock.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepTextBlock_TSVRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_TSVRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class OOFREP_EXPORT oofRepPictBlock_TSVRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_TSVRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};

#endif

