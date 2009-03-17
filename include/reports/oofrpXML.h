#ifndef H_OOFRPXML
#define H_OOFRPXML

// COPYRIGHT 1998 A.D. Software, All rights reserved

// XML report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif

#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif
#ifndef H_OOFGEOS
	#include "oofGeos.h"
#endif
#ifndef H_OOFSTR
	#include "oofstr.h"
#endif
#ifndef H_OOFXMLDB
	#include "oofxmldb.h"
#endif


/**
	write an oofRepTextStyle out as a CSS text format string.
	\ingroup oofRenderXML
*/
class  oofRepTextStyle2CSS {
public:
	static oofString emitCSSstyle(const oofRepTextStyle&);
	static void makeStyleStrings(oofString& theStyleRun, unsigned char textStyle);
	static oofString makeAlignmentString(oofColSizer::colAlignT);	
};

/**
	\defgroup oofRenderXML XML Report rendering.
	Reports rendered to XML including layout, data schema
	and all source data suitable for re-opening with any 
	OOFILE reporting application and editing in the 
	report preview window.
	Included in the Open Source OOFILE.
	\todo confirm that oofBandShadingAdorner_XMLRender etc. are not needed but these attributes written anyway to XML file.
	\ingroup oofRW_render
*/


// use common base to provide the factory methods
/**
	Specify output to XML stream.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepXMLEnv :  public oofRepStreamEnv {
public:
	enum 	{eRenderType='XML '};
	
	oofRepXMLEnv(ostream&);
	oofRepXMLEnv(const char* fileName);
	
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	virtual void startSection();
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	
	virtual void startEnvironment();
	virtual void endEnvironment();
	virtual bool preprocessesPageHeadersAndFooters() const;

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const;
	virtual void	makeRenderDispatch(oofAdorner*) const;
	virtual oofRepRenderT renderType() const;
// methods to get at utilities
	oofIDfactory&		getIdentities();

protected: 
	void CloseSection();
	void StartReport(const oofString reportID);
	
// data storage
	oofIDfactory mIdentities;
	int mSectionNumber;
};


/**
 Generates report-specific xml tags like oofTagMaker (in oofxml).
 Line and space tags return the form
"......<tag ID="IdString" width="aNumber" height="aNumber"/>" 
*/
class OOFREP_EXPORT oofRepTagMaker {
public:
	static oofString makeLineTag(unsigned long height, const char* indentString=0, const char* idString=0);
	static oofString makeSpaceTag(unsigned long height, const char* indentString=0, const char* idString=0);
	static oofString makeBreakTag(const char* indentString=0);
};



// XML renderers for OOFILE database

/**
	Output a \<space\> tag using oofRepTagMaker::makeSpaceTag.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepSpaceBand_XMLRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_XMLRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void describeLayout();
};


/**
	Write underlying data for graph to stream.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepGraphBand_XMLRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_XMLRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
	virtual void describeLayout();
};


/**
	No output render, OOF_repXMLlayoutVisitor::VisitLayoutBand describes this band.
	\ingroup oofRenderXML.
*/
class OOFREP_EXPORT oofRepLayoutBand_XMLRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_XMLRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
};


/**
	Output description of oofRepLineBand.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepLineBand_XMLRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_XMLRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void describeLayout();
};


/**
	No output render.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepPictBand_XMLRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_XMLRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
};


/**
	Output data contained within the dbView as nested elements.
	Elements named according to the database schema.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepViewBand_XMLRender : public oofRepViewBand_Render {
private:
	oofRepViewBand_XMLRender(const oofRepViewBand_XMLRender&) : oofRepViewBand_Render(0) {;};  // can't copy
	void operator=(const oofRepViewBand_XMLRender&) {;};  // can't copy
	
public:
	oofRepViewBand_XMLRender(oofRepViewBand*);
	virtual ~oofRepViewBand_XMLRender();
	
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;  // doesn't use copy ctor
	virtual void draw();	

	virtual void startTableBody(bool headersHaveChanged);
	virtual void endTableBody();
	
private:
	oofXMLDBWriter* mWriter;	// owned
};


/**
	Output description of the aligned text block as specified by oofRepTextBlock.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepTextBlock_XMLRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_XMLRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void describeLayout();
};


/**
	Output text with ^p bracketed text as specified by oofRepPageNoBlock.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepPageNoBlock_XMLRender : public oofRepTextBlock_XMLRender {
public:
	oofRepPageNoBlock_XMLRender(oofRepTextBlock* b) : oofRepTextBlock_XMLRender(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void describeLayout();
};


/**
	No output render.
	\ingroup oofRenderXML
*/
class OOFREP_EXPORT oofRepPictBlock_XMLRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_XMLRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
};


class oofGraphSettings;  // forward

// CSS output classes defined up here in case people write their own oofRep
// objects and want to output to XML
/**
	render oofGraphSettings to a string string in CSS-like syntax.
	Allows each graph band's settings to be included in the main report <style> element.
	\ingroup oofRenderXML
*/
class  oofGraphSettings2CSS {

public:
	static oofString emitCSSstyle(const oofGraphSettings*);
private:
	static oofString MakePosStateString(oofGeo::PositionState);
};


class oofRepSettings;  // forward
/**
	render oofRepSettings to a string string in CSS-like syntax.
	Allows report settings to be included in a single <style> element.
	\ingroup oofRenderXML
*/
class  oofReportSettings2CSS {
// write out an oofGraphSettings object as our own string in CSS-like syntax
public:
	static oofString emitCSSstyle(const oofRepSettings*, const oofString& defaultTextID);
};


// -------------------------------------------------------
//           o o f R e p X M L E n v
// -------------------------------------------------------
inline oofIDfactory&		
oofRepXMLEnv::getIdentities()
{
	return mIdentities;
}
#endif

