#ifndef H_OOFREPTXT
#define H_OOFREPTXT

// COPYRIGHT 1997 A.D. Software, All rights reserved

// GUI report-writer layer of OOFILE database
// character stream output

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif

/**
	\defgroup oofRenderText Plain Text Report rendering.
	Reports rendered to text streams in simple fashion.
	\see oofCharRW simple text reporter.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

// use common base to provide the factory methods
/**
	Specify output to simple spaced text stream.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepTextEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='Char'};

	oofRepTextEnv();
	oofRepTextEnv(ostream&);
	oofRepTextEnv(const char* fileName);
	
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;
};


// character stream renderers for OOFILE database

/**
	Advance report vertical position.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepSpaceBand_TextRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_TextRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output render.
	\ingroup oofRenderText.
*/
class OOFREP_EXPORT oofRepGraphBand_TextRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_TextRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepLayoutBand_TextRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_TextRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepLineBand_TextRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_TextRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepPictBand_TextRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_TextRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepViewBand_TextRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_TextRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();	
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepTextBlock_TextRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_TextRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderText
*/
class OOFREP_EXPORT oofRepPictBlock_TextRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_TextRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};



#endif

