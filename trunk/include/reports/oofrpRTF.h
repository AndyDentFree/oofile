#ifndef H_OOFREPRTF
#define H_OOFREPRTF

// COPYRIGHT 1997 A.D. Software, All rights reserved

// RTF report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif
/**
	\defgroup oofRenderRTF RTF Report rendering.
	Reports rendered to Rich Text Format text streams
	suitable for import into MS Word or other word processors.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

// use common base to provide the factory methods
/**
	Specify output to MS Word-compatible Rich Text stream.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepRTFEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='RTF '};
	
	oofRepRTFEnv();
	oofRepRTFEnv(ostream&);
	oofRepRTFEnv(const char* fileName);
	
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	virtual void startSection();

	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	
	virtual void startEnvironment();
	virtual void endEnvironment();
	virtual bool preprocessesPageHeadersAndFooters() const;
	static  void encodeToRTFStream(ostream&, const char*);
	static oofString encodeRTFString(const oofString&);

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;

protected:
	int mSectionNumber;
};


// RTF renderers for OOFILE database

/**
	Output pard tag to insert fixed height paragraph as specified by oofRepSpaceBand.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepSpaceBand_RTFRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_RTFRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Write graph as Mac or Win32 picture embedded in RTF.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepGraphBand_RTFRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_RTFRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate block renderers according to the oofRepLayoutBand.
	Uses par and tab tags to separate blocks.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepLayoutBand_RTFRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_RTFRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Render an oofRepLineBand by creating an embedded picture.
	\todo consider using Word draw commands as option.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepLineBand_RTFRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_RTFRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Output picture as appropriate tag based on oofPict image type.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepPictBand_RTFRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_RTFRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	
	virtual void draw();
};


/**
	Draw oofRepViewBand contents as table.
	Uses trowd, pard and intbl tags.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepViewBand_RTFRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_RTFRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();	
	virtual void drawColumnHeaders();
};


/**
	Output encoded text using oofRepRTFEnv::encodeToRTFStream.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepTextBlock_RTFRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_RTFRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	No output renderer.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepPictBlock_RTFRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_RTFRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	output a chpgn tag to show page number in body of page.
	\ingroup oofRenderRTF
*/
class OOFREP_EXPORT oofRepPageNoBlock_RTFRender : public oofRepPageNoBlock_Render {
public:
	oofRepPageNoBlock_RTFRender(oofRepPageNoBlock* b) : oofRepPageNoBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};

#endif

