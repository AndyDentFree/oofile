// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofriXML.cpp
// Input of reports from XML
// WARNING requires public domain xpatpp available from http://www.oofile.com.au/expatpp/
// and James Clark's expat available from http://www.jclark.com/xml/expatfaq.html

#ifdef OOF_READ_REPORTS

#ifndef H_OOFRIXML
#define H_OOFRIXML

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFARRAY
	#include "oofArray.h"
#endif
#ifndef H_OOFGEOS
	#include "oofGeos.h"
#endif
#ifndef H_oofSting
	#include "oofSting.h"
#endif
#ifndef H_EXPATPP
	#include "expatpp.h"
#endif


 // forward
class OOF_XMLrepParserState; 
class oofRepPendingManager;  
class oofRepPromiseCompleter;  

/**
	top parser used to parse report from XML file.
	May create and/or populate a dbConnect.
	\ingroup oofRepRead
*/
class OOFREP_EXPORT OOF_XMLrepParser : public expatppNesting {
private:
	OOF_XMLrepParser(const OOF_XMLrepParser&) {};  // can't copy
public:
	OOF_XMLrepParser(dbConnect* inDB=0);
	OOF_XMLrepParser(expatppNesting*, dbConnect* inDB=0);
	virtual ~OOF_XMLrepParser();
	virtual void startElement(const XML_Char *name, const XML_Char **atts);
	virtual void endElement(const XML_Char* name);
	oofRep* orphanReport();
	oofRep* buildingReport() const;
	virtual bool parseFile(const oofFileRef&);
	
protected:
	virtual void FinishReport();
	virtual void startElem_report(const char** atts);
	virtual void startElem_section(const char** atts);
	virtual void startElem_style(const char** atts);
	virtual void startElem_adorners(const char** atts);
	virtual void startElem_layout(const char** atts);
	virtual void startElem_schema(const char** atts);
	virtual void startUnknownElem(const char* name, const char** atts);

	virtual void endElem_report();
	virtual void endElem_section();
	virtual void endElem_style();
	virtual void endElem_adorners();
	virtual void endElem_layout();
	virtual void endElem_schema();
	virtual void endUnknownElem(const char* name);
	
	
// data storage
	oofRep* mReport;    ///< owned - the current report being built, may actually be oofRepSection
	oofRepMulti* mMultiRep;    ///< owned
	dbConnect* mDB;
	oofRepPromiseCompleter* mTextStyleCompleter;  ///< owned
	oofRepPromiseCompleter* mGraphSettingsCompleter;  ///< owned
	oofString mStyleString;
	oofString mID;
	OOF_Dictionary mAdornerDict;
};


/**
	Parse a \<style\> tag mainly for text styles.
	\see OOF_XMLrepParser::startElem_style
	\ingroup oofRepRead
*/
class OOFREP_EXPORT OOF_XMLrepStyleParser : public expatppNesting {
private:
	OOF_XMLrepStyleParser(const OOF_XMLrepStyleParser& rhs) :
		mCallerString(rhs.mCallerString)
	{};  // can't copy
public:
	OOF_XMLrepStyleParser(expatppNesting*, oofString&);
	virtual void charData(const XML_Char* s, int len);
	
private:
	oofString& mCallerString;	
};


/**
	Parse an \<adorners\> tag for serialized adorners.
	Adorners may be ours or user subclasses of oofSerializable.
	\see OOF_XMLrepParser::startElem_adorners
	\ingroup oofRepRead
*/
class OOFREP_EXPORT OOF_XMLrepAdornerParser : public expatppNesting {
private:
	OOF_XMLrepAdornerParser(const OOF_XMLrepAdornerParser& rhs) :
		mCallerDict(rhs.mCallerDict)
	{};  // can't copy
public:
	OOF_XMLrepAdornerParser(expatppNesting*, OOF_Dictionary&);
	virtual void startElement(const XML_Char* name, const XML_Char** atts);
	virtual void endElement(const XML_Char* name);
	
private:
	OOF_Dictionary& mCallerDict;
	oofString mCurrentID;	
	oofSerializable* mCurrentSerializable;
};


/**
	Parse a \<layout\> tag describing report classes.
	This is the one totally vital tag - without it a report can't be built.
	\see OOF_XMLrepParser::startElem_layout
	\ingroup oofRepRead
*/
class OOFREP_EXPORT OOF_XMLrepLayoutParser : public expatppNesting {
private:
	OOF_XMLrepLayoutParser(const OOF_XMLrepLayoutParser&) {};  // can't copy
public:
	OOF_XMLrepLayoutParser(OOF_XMLrepParser*, dbConnect*);
	virtual ~OOF_XMLrepLayoutParser();
	virtual void startElement(const XML_Char* name, const XML_Char** atts);
	virtual void endElement(const XML_Char* name);
	virtual void charData(const XML_Char* s, int len);
	
private:
	void OpenLayout(const char** atts);
	void CloseLayout();
	void BuildBreak();
	void BuildBlock(const char** atts);
	void BuildGraphBand(const char** atts);
	void BuildSpaceBand(const char** atts);
	void BuildLineBand(const char** atts);
	dbTable* GetDatabaseInContext(const char** atts, unsigned short startAttribute=0, const char* attName="source");
	void OpenTable(const char* name, const char** atts);
	void CloseTable();
	void OpenColumn(const char** atts);
	void OpenHeaderOrFooter(const char** atts, int firstState, int otherState);
	void AppendBandDependingOnState(oofRepBand* orphanedBand);
	oofRepBandList* CurrentBandListDependingOnState() const;
	oofRepViewBand* CurrentViewBand() const;
	void ParseAccumulatedCharData();

// data storage
	dbConnect* mDB;
	oofRep* mReport;    
	OOF_XMLrepParserState* mBuildState;   // owned
	oofString mCurrentCSSID;
	oofString mdbTxtBuffer;
	oofRepLayoutBand *mCurrentLayout;
	oofRepBandList  mTablesStack;
	oofRepTextBlock *mLastBlock;
	oofString       mCSSstylesString;
	oofRepBand*	mCurrentBreakBand;
	bool mCurrentBreakNewPageEveryTime;
	oofRepBlock::blockAlignT mCurrentBlockAlign;
	int mCurrentColNumber;
	oofSerializable* mCurrentSerializable;
	oofString mCharData;
	int mCurrentBlockFixedWidth;
};


/**
	Parse a string in CSS format to create an oofRepTextStyle.
	Used by OOF_XMLrepStyleParser.
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofCSS2RepTextStyle{
public:
	// use default ctor, dtor, copy ctor
	void parseStyleText(const oofString&, oofRepTextStyle*);
	bool isOofReptxtStyle(const oofString&);
	
private:
	unsigned char MakeStyleNumber();
	oofString 		FindFontFamily();
	short 			FindFontSize();
	float 			FindLineHeight();
	oofColor		FindFontColour(); 
	oofColSizer::colAlignT FindAlignment();
	short 			CalculateLeading(short fontSize,float lineHeight) const;

// data storage
	char* mParseBuffer;	 // owned, managed in parseStyleText
};


/**
	Parse a string in CSS-like format to create an oofGraphSettings.
	Used by OOF_XMLrepStyleParser for each graph band in the report.
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofCSS2GraphSettings{ 
public:
	// use default ctor, dtor, copy ctor
	static void parseSettingsText(const oofString&, oofGraphSettings*);

private:
	static oofGeo::PositionState ParsePosString(const char*);
	static bool ParseBoolString(const char*);
};


/**
	Parse a string in CSS-like format to create an oofRepSettings.
	Used by OOF_XMLrepStyleParser usually for only one instance.
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofCSS2ReportSettings{ 
public:
	// use default ctor, dtor, copy ctor
	static void parseSettingsText(const oofString&, oofRepSettings*);

private:
	static unsigned long Hash(const char* theStr);
};


// -------------------------------------------------------
//            O O F _ X M L r e p P a r s e r
// -------------------------------------------------------
inline	oofRep* 
OOF_XMLrepParser::buildingReport() const
{
	return mReport;
}

#endif

#endif   // entire file conditional on OOF_READ_REPORTS
