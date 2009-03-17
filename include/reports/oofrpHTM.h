#ifndef H_OOFREPHTML
#define H_OOFREPHTML

// COPYRIGHT 1997 A.D. Software, All rights reserved

// HTML report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif
/**
	\defgroup oofRenderHTML HTML Report rendering.
	Reports rendered to HTML text streams.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

// use common base to provide the factory methods
/**
	Specify output to HTML stream.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepHTMLEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='HTML'};
	
	oofRepHTMLEnv();
	oofRepHTMLEnv(ostream&);
	oofRepHTMLEnv(const char* fileName);

	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	
	virtual void startEnvironment();
	virtual void endEnvironment();
	
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;
};


// HTML renderers for OOFILE database

/**
	Output a \<br\> tag.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepSpaceBand_HTMLRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_HTMLRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output render.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepGraphBand_HTMLRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_HTMLRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate block renderers according to the oofRepLayoutBand.
	Uses \<br\> and \<div\> tags to separate blocks.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepLayoutBand_HTMLRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_HTMLRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Output an \<hr\> tag without other size info.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepLineBand_HTMLRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_HTMLRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Debug output render
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepPictBand_HTMLRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_HTMLRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Draw oofRepViewBand contents as nested \<table\> elements.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepViewBand_HTMLRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_HTMLRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void startTableBody(bool headersHaveChanged);
	virtual void endTableBody();
	virtual void draw();	

protected:
	virtual void CalcColWidths();
};



/**
	Just output the plain text in the oofRepTextBlock.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepTextBlock_HTMLRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_HTMLRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Debug output renderer.
	\ingroup oofRenderHTML
*/
class OOFREP_EXPORT oofRepPictBlock_HTMLRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_HTMLRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


#endif

