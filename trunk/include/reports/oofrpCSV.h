#ifndef H_OOFREPCSV
#define H_OOFREPCSV

// COPYRIGHT 1997 A.D. Software, All rights reserved

// CSV report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif

/**
	\defgroup oofRenderCSV CSV Report rendering.
	Reports rendered to Comma Separated Value text streams.	
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

// use common base to provide the factory methods
/**
	Specify output to Comma-Separated-Values stream.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepCSVEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='CSV '};
	
	oofRepCSVEnv();
	oofRepCSVEnv(ostream&);
	oofRepCSVEnv(const char* fileName);
	
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


// CSV renderers for OOFILE database

/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepSpaceBand_CSVRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_CSVRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Write underlying data for graph to stream.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepGraphBand_CSVRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_CSVRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate blocks in oofRepLayoutBand separated with newlines & commas.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepLayoutBand_CSVRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_CSVRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepLineBand_CSVRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_CSVRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepPictBand_CSVRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_CSVRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Iterate columns in oofRepViewBand separated with newlines & commas.
	Includes column headings.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepViewBand_CSVRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_CSVRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();	
	virtual void drawColumnHeaders();
};


/**
	Output quoted plain text in the oofRepTextBlock.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepTextBlock_CSVRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_CSVRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class OOFREP_EXPORT oofRepPictBlock_CSVRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_CSVRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};

#endif

