#ifndef H_OOFREP
#define H_OOFREP
// COPYRIGHT 1996 A.D. Software, All rights reserved

/**
	\file oofrep.h 
	GUI report-writer layer of OOFILE database.
*/

/**
	\defgroup oofRW	OOFILE Report writer
	\anchor RW_anchor
	GUI report previews on Mac and Windows, RTF, XML, HTML
	and plain text output on these plus Unix and other generic
	C++ platforms. 
	Included in the Open Source OOFILE.
	\ingroup Presentation
*/



/**
	Version number of report writer package.
	\ingroup oofMacros
*/
#define		OOFVERS_REP	0x01400000	// Version 1.4.0
/* index to version defines 
Note that you will may lack some files depending on your package

file:        VERSION IDENTIFIER
====         ==================
oof0.h	     OOFVERS_CORE core version
oofrep.h     OOFVERS_REP report writer
oofGrphs.h   OOFVERS_GRAPH graphing engine
oofgui.h     OOFVERS_GUI  generic GUI classes used by PP, MFC and other framework integrations
oofpp.h      OOFVERS_PP  PowerPlant framework (Metrowerks CodeWarrior, on Mac)
oofmfc.h     OOFVERS_MFC MFC GUI framework

*/

/**
	\defgroup oofRW_render	Multiple platform rendering
	Group rendering classes for each platform.
	Without rendering classes, a report object will be skipped for a
	a given oofRepEnvironment.
	\ingroup oofRW
*/





#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif
#ifndef H_OOFCOLOR
	#include "oofcolor.h"
#endif
#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFDRAW
	#include "oofdraw.h"
#endif
#ifndef H_OOFSIZE
	#include "oofsize.h"
#endif

#ifndef H_OOFXMLDB
	#include "oofxmldb.h"
#endif 

class oofRepStyle;
class oofRepBand;
class oofRepViewBand;
class oofRepBreak;
class oofRepBlock;
class oofRepTextStyle;
class oofRepBandList;
class oofRepBlockList;
class oofRepBreakList;
class oofRepRender;
class oofGraph;
class oofRepDrawable;
class oofRepData;
class oofRepEditor;
class oofRepEditLink;
class OOF_mixViewExtraRender;

// forward declare environments used in multi-method switching makeRender()
class oofRepEnvironment;
class oofRepMacEnv;
class oofRepMacPrint;
class oofRepWinEnv;
class oofRepWinPrint;
class oofRepTextEnv;
class oofRepRTFEnv;
class oofRepHTMLEnv;
class oofRepCSVEnv;
class oofRepTSVEnv;
class oofRepXMLEnv;

/**
	\def OOF_REP_NO_GUI
	Define to build the report writer for server use, no preview or print.
	Hides the preview, printing and editing environments.
	\ingroup oofMacros 
*/

#ifndef OOF_REP_NO_GUI
class oofRepEditingEnv;
#endif

typedef unsigned long oofRepRenderT;

int twips( int pixels );

/**
	\def OOF_MacStyleEnums
	Indicates we have enums defined for Mac text styles.
	Define to indicate you have externally already defined these enums.
	\todo confirm this exists for compatability with "QuickTime for Windows".
	\ingroup oofMacros
*/
#if defined(_Windows) && !defined(OOF_MacStyleEnums)
#define OOF_MacStyleEnums
/**
	enums matching Macintosh text style context
	\ingroup oofRW
*/
enum OOF_MacStyleEnumT {
		normal		= 0x00,
		bold		= 0x01,
		italic		= 0x02,
		underline	= 0x04,
		outline		= 0x08,
		shadow		= 0x10,
		condense	= 0x20,
		extend		= 0x40
};
#endif

/**
	Creates XML environment and encapsulates other settings.
	subclass this and set the report setting oofRepXMLsetter to override these rules.
	\ingroup oofRW oofXML
*/
class OOFREP_EXPORT oofRepXMLsettings {
public:
	virtual oofRepXMLsettings* clone();
	
	virtual oofRepXMLEnv* makeXMLenv(ostream&);  // yes, you can actually subclass the env!
	virtual oofRepXMLEnv* makeXMLenv(const char* fileName);  // another factory to override 
	
	virtual dbConnect* makeTempConnection();  // choose type of backend 
	virtual void createTempConnectFiles(dbConnect*);  // chooses name and where to store

// methods controlling Save As dialog and file types
	virtual oofString saveReportDocDescription();   
	virtual oofString saveReportDocFileExt();   // eg: ".XML"
	virtual unsigned long saveReportDocMacFileType();  // eg: 'TEXT'
};


/**
	Settings object for overriding behaviours of an instance of oofRep.
	\see oofRepDefaultSettings for the defaults you override
	by setting anything with our members.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepSettings {
public:
	oofRepSettings();
	oofRepSettings(oofRepSettings *);
	~oofRepSettings();
		
	unsigned short	lineThickness() const;
	void			lineThickness(unsigned short);

	unsigned short	pageHeight() const;
	void			pageHeight(unsigned short);

	unsigned short	pageWidth() const;
	void			pageWidth(unsigned short);
	
	unsigned short	borderType() const;
	void			borderType(unsigned short);
	
	unsigned short	bandIndenting() const;
	void			bandIndenting(unsigned short);

	unsigned short	leftMargin() const;
	void			leftMargin(unsigned short);

	unsigned short	rightMargin() const;
	void			rightMargin(unsigned short);

	unsigned short	topMargin() const;
	void			topMargin(unsigned short);

	unsigned short	bottomMargin() const;
	void			bottomMargin(unsigned short);

	unsigned short	colSepWidth() const;
	void			colSepWidth(unsigned short);

	unsigned short	blockVertSep() const;
	void			blockVertSep(unsigned short);

	oofRepTextStyle*	defaultTextStyle() const;
	void			defaultTextStyle(oofRepTextStyle* adoptedStyle);
	
	oofRepBandList&	defaultPageHeaders() const;
	void			defaultPageHeaders(oofRepBandList*);
	
	oofRepBandList&	defaultPageFooters() const;
	void			defaultPageFooters(oofRepBandList*);
	
	unsigned short	drawHeadingsDownToLevel() const;
	void			drawHeadingsDownToLevel(unsigned short);	// 1 is first visible band, 0 disables

	bool			printGraphsInMono() const;
	void			printGraphsInMono(bool);	
			
	bool			modalPreviewWindow() const;
	void			modalPreviewWindow(bool);	
	
	unsigned short 	graphBandMinSizePercent() const;
	void			graphBandMinSizePercent(unsigned short);
			
	oofRepXMLsettings*	XMLsettings() const;
	void			XMLsettings(oofRepXMLsettings*);
			
	oofRepEditor*	reportEditor() const;
	void			reportEditor(oofRepEditor*);

	unsigned short&	refcount();
	void describe(ostream& );
private:
// *** WARNING *****
// if you add any settings, remember to update oofRepDefaultSettings ctor
// as well as our ctor, copy ctor & dtor

// all pointers below here are owned
	unsigned short*	mLineThickness;
	unsigned short*	mPageHeight;
	unsigned short*	mPageWidth;
	unsigned short*	mBorderType;
	unsigned short* mBandIndenting;
	unsigned short*	mLeftMargin;
	unsigned short*	mRightMargin;
	unsigned short*	mTopMargin;
	unsigned short*	mBottomMargin;
	unsigned short*	mColSepWidth;
	unsigned short*	mBlockVertSep;
	oofRepTextStyle*	mDefaultTextStyle;
	unsigned short	mRefCount;
	oofRepBandList*		mDefaultPageHeaders;  // NOT exported to XML as part of settings, already copied in <layout>
	oofRepBandList*		mDefaultPageFooters;  // NOT exported to XML as part of settings, already copied in <layout>
	unsigned short*	mDrawHeadingsDownToLevel;
	bool*				mPrintGraphsInMono;
	bool*				mModalPreviewWindow;
	unsigned short*	mGraphBandMinSizePercent;
	oofRepXMLsettings*	mXMLsettings;  // NOT exported to XML - application will use globally only
	oofRepEditor*		mRepEditor;  // NOT exported to XML - application will use globally only
};


/**
	Singleton used to initialise oofRepSettings.
	Obtain this object via oofRep::defaultSettings().
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepDefaultSettings {
public:
		oofRepDefaultSettings();
		~oofRepDefaultSettings();
	static oofRepSettings *	settings();
	
private:
	static oofRepSettings * 	sDefaultSettings;
	static oofRepDefaultSettings		sDefaultSettingsFactory;	// the Singleton
};


/**
	List of bands attached to report or other bands, eg: page headers.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepBandList {
public:
	enum keepTogetherT {eDefaultTogether, eKeepTogether, eAllowSplit};
	
	oofRepBandList(keepTogetherT keepTogether=eDefaultTogether);
	oofRepBandList(const oofRepBandList&);
	const oofRepBandList& operator=(const oofRepBandList&);
	~oofRepBandList();
	oofRepBandList* clone();
	
	unsigned long minHeight() const;
	bool keepTogether() const;
	void keepTogether(bool);  // must call before report runs 
	
	void start();
	bool more() const;
	void next();
	void deleteItem(unsigned long index);
	unsigned long count() const;
	oofRepBand* operator()();
	oofRepBand* current();
	oofRepBand* value(unsigned long) const;
	void append(oofRepBand* adoptedBand);
	oofRepBandList& operator<<(oofRepBand* adoptedBand);
	oofRepBandList& operator<<(const oofRepBand& prototypeBand);
	unsigned long fixedHeight();
	void finishConstruction();
	void describe(ostream& os);
	
	
	
private:
// data storage
	OOF_ExpandableLongArray	mBands;	
	keepTogetherT mKeepTogether;
	unsigned long mFixedHeight, mMinHeight;  // calc in finishConstruction
};


/**
	List of blocks in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepBlockList {
public:
	oofRepBlockList();
	oofRepBlockList(const oofRepBlockList&);
	const oofRepBlockList& operator=(const oofRepBlockList&);
	~oofRepBlockList();
	oofRepBlockList* clone();
	
	void start();
	bool more() const;
	void next();
	unsigned long count() const;
	oofRepBlock* operator()();
	oofRepBlock* current();
	oofRepBlock* value(unsigned long) const;
	void append(oofRepBlock* adoptedBlock);
	oofRepBlockList& operator<<(oofRepBlock* adoptedBlock);
	unsigned long fixedHeight();
	void describe(ostream&);
	
	
private:
// data storage
	OOF_ExpandableLongArray	mBlocks;	
};


/**
	List of Breaks attached to a report band..
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepBreakList {
public:
	oofRepBreakList(bool ownsBreaks=true);
	oofRepBreakList(const oofRepBreakList&);
	const oofRepBreakList& operator=(const oofRepBreakList&);
	~oofRepBreakList();
	oofRepBreakList* clone();
	
	void start();
	bool more() const;
	void next();
	unsigned long count() const;
	oofRepBreak* operator()();
	oofRepBreak* current();
	oofRepBreak* value(unsigned long) const;
	void append(oofRepBreak* adoptedBreak);
	oofRepBreakList& operator<<(oofRepBreak* adoptedBreak);
	
	

private:
// data storage
	OOF_ExpandableLongArray	mBreaks;
	bool mOwnsBreaks;	
};


/**
	Abstracts a drawing environment.
	May be GUI, like oofRepMacPrint or purely textual like oofRepRTFEnv.
	<em>When you add a new oofRepEnvironment you will 
	need to add a makeRender method to oofRepDrawable</em>
	The oofRepEnvironment class type causes different makeRender methods to be invoked.
	\see oofRepViewBand::makeRender(const oofRepMacEnv&) for an example
	\ingroup oofRW
	
*/
class OOFREP_EXPORT oofRepEnvironment {
public:
	virtual void changeTextStyle(oofRepTextStyle*)=0;
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*)=0;
	virtual void 	drawPageBreak()=0;
	virtual void drawSectionBreak();
	virtual void startSection();  // in case we need to know, like startEnvironment
	
	virtual unsigned long calcPageHeight()=0;
	virtual unsigned long calcPageWidth()=0;
	virtual bool canDraw();
	virtual bool isPrintingEnvironment() const;
	
	virtual void makeRender() {};  // in case anyone needs to do things when making them
	virtual void startEnvironment();
	virtual void endEnvironment();
	virtual bool preprocessesPageHeadersAndFooters() const;
	
	virtual void drawPageBorder( long top, long left, long bottom, long right );
	virtual void splitBandOverPage() {};
	virtual void resumeBandOverPage() {};


// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable*) const=0;
	virtual void	makeRenderDispatch(oofAdorner*) const=0;
	
	virtual oofRepRenderT renderType() const=0;
};


/**
	Base environment for rendering to a text stream.
	Used when exporting reports to file or by HTTP servers.
	\ingroup oofRW	
*/
class OOFREP_EXPORT oofRepStreamEnv :  public oofRepEnvironment {
public:

	oofRepStreamEnv();
	oofRepStreamEnv(ostream&);
	oofRepStreamEnv(const char* fileName, bool openBinary=false);
	virtual ~oofRepStreamEnv();
	
	virtual bool canDraw();

protected:
	ostream*	mOut;	// maybe owned
	ofstream* 	mOwnedofstream;  // maybe owned, copy of mOut as different class	
	
public:
	static		ostream& out();
	static ostream* sOut;
};


class oofRep;
class oofRepBandList;
class oofRepViewBand;
class oofRepBreakList;
class oofRepBand;
class oofAdornerList;
class oofAdorner;
class oofRepLayoutBand;
class oofRepBlock;
class oofRepDrawable;
class oofAdornerList;
class oofAdorner;

/**
	visits all report objects.
	Used largely for XML and other output formats
	\see oofRepXMLEnv::StartReport for use of several visitors.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepVisitor {
public:
	oofRepVisitor() :
		mRep(0)  // ensure they call visit(aRep)
	{};
	virtual ~oofRepVisitor() {};  // empty virtual dtor because we subclass
	
	virtual void visit(oofRep*);  
	// the top-level that iterates all the others, encapsulating report structure
	// although virtual, you would NOT normally override this method

// the following methods should be overriden if context is important
// otherwise default to calling the core visit methods below
protected:
	virtual void VisitFirstPageHeaders(oofRepBandList&);
	virtual void VisitFirstPageFooters(oofRepBandList&);
	virtual void VisitPageHeaders(oofRepBandList&);
	virtual void VisitPageFooters(oofRepBandList&);
	virtual void VisitBody(oofRepViewBand*);
	virtual void VisitViewHeaders(oofRepBandList&);
	virtual void VisitViewFooters(oofRepBandList&);
	virtual void VisitViewHeaderBreaks(oofRepBreakList&);
	virtual void VisitViewFooterBreaks(oofRepBreakList&);

// core visit methods
	virtual void VisitBandList(oofRepBandList&);
	virtual void VisitBreakList(oofRepBreakList&);
	virtual void VisitBlockList(oofRepBlockList&);
	virtual void VisitViewBand(oofRepViewBand*);  // does all the headers, footers etc.
	virtual void VisitViewBandBody(oofRepViewBand*);  // just the innermost band
	virtual void VisitLayoutBand(oofRepLayoutBand*);
	virtual void VisitBlock(oofRepBlock*); 
	virtual void VisitBand(oofRepBand*);
	virtual void VisitDrawable(oofRepDrawable*); // ultimate base called by default VisitBand, VisitBlock

// methods for subclass to visit adorners, not called by default visit traversal
	virtual void VisitAdornerList(oofAdornerList&); 
	virtual void VisitAdorner(oofAdorner*);

// override these if you need to update some kind of depth counter
// as the default traverses above are used
	virtual void EnterLevel();
	virtual void LeaveLevel();

// utilities
	void VisitTypedBand(oofRepBand*);
	
// data storage
	oofRep* mRep;
};


/**
	visits all dbViews in a report.
	so you can just subclass and override VisitView to gather schema
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepSchemaVisitor : public oofRepVisitor {
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
	
	// define our own pure virtual
	virtual void VisitView(dbView*)=0;
};



/**
	Top level class for a report.
	Reports consist of a tree of bands rooted in a top level oofRepViewBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRep : public OOF_mixAdornable{
public:
	oofRep(dbTable* inTableForDefaultBody, unsigned long startingPageNumber=1);
	oofRep(dbView* adoptedViewForDefaultBody, unsigned long startingPageNumber=1);
	oofRep(const dbView& inViewForDefaultBody, unsigned long startingPageNumber=1);
	oofRep(oofRepViewBand* adoptedBody=0, unsigned long startingPageNumber=1);
	oofRep(const oofRep&);
	virtual ~oofRep();
	
	virtual bool isMultiReport() const;
	unsigned short nestedViewLevel() const;
	enum { kBodyViewLevel=1 };  // base value for comparing against nestedViewLevel()
	
	oofRepBandList& firstPageHeaders();
	oofRepBandList& firstPageFooters();
	
	oofRepBandList& pageHeaders();
	oofRepBandList& pageFooters();

	virtual void	draw(oofRepEnvironment&);
	
	void	drawFirstPageHeaders();
	void	drawFirstPageFooters();
	
	void	drawPageHeaders();
	void	drawPageFooters();
	
	void	resetViewLevel();
	void	incrementViewLevel();
	void	decrementViewLevel();
	
	void 	drawPageBreak();
	const oofRepSettings*		settings() const;
	oofRepSettings*			getLocalSettings();
	void changeTextStyle(oofRepTextStyle*);
	oofRepEnvironment* environment() const;
	unsigned long& currentVerticalPos();
	unsigned long bodyStart();
	unsigned long bodyEnd();
	unsigned long bodyHeight();
	unsigned long bodyWidth();

	unsigned long pageWidth();
	unsigned long pageHeight();

	bool shouldDrawViewColumnHeaders() const;
	bool reportAllowsEditing() const;
	
	oofRepViewBand* body(oofRepViewBand* adoptedBody);
	oofRepViewBand* body();
	
	void pageNumber(unsigned long);
	unsigned long pageNumber() const;
	unsigned long startingPageNumber() const;
	void startingPageNumber(unsigned long);
	
	static	oofRepSettings*	defaultSettings();
	static	oofRep*		currentReport();

	void becomeCurrent();

	void describe(ostream&);
	
protected:
	void DrawViewBand(oofRepViewBand*);
	void DrawBandList(oofRepBandList&, bool inBody=true);
	void ProcessBreakList(oofRepBreakList& inList, bool beforeRecord);
	void DrawBreakList(oofRepBreakList& inList);
	
// data storage

	oofRepBandList mFirstPageHeaders;
	oofRepBandList mFirstPageFooters;
	oofRepBandList mPageHeaders;
	oofRepBandList mPageFooters;
	oofRepViewBand*	mTopBand;	// owned
	oofRepBand*	mCurrentBand;
	oofRepBreak*	mCurrentBreak;
	unsigned long	mFirstPageHeaderHeight, mFirstPageFooterHeight, mPageHeaderHeight,
					mPageFooterHeight, mPageHeight, mHeadersStartAt, mFootersStartAt, 
				  	mVerticalPos, mBodyStart, /*mBodyEnd,*/ mBodyWidth, mPageWidth,
				  	mFirstPageBodyStart, mFirstPageFootersStartAt/*, mFirstPageBodyEnd*/;
	bool			mFirstPage;
	oofRepEnvironment* mDrawingEnv;
	oofRepSettings* mSettings;  // owned
	unsigned long mPageNumber;
	bool mFinishedConstruction;
	oofRepRenderT	mLastRenderType;
	unsigned short mNestedViewLevel;  // 1=in body view as zero allowed for outside the main body view
	oofRepData*	mRepData;	// owned
	unsigned long mStartingPageNumber;
	unsigned short* mReferences;  // owned

	static oofRep*	sCurrentReport;
};


class oofRepSection;  // forward

/**
	Report that contains list of unrelated other reports as oofRepSection's.
	\todo Allow nesting oofRepMulti within oofRepMulti - currently it's flat
	\ingroup oofRW	
*/
class OOFREP_EXPORT oofRepMulti : public oofRep {
public:
	oofRepMulti();
// 	use default copy ctor
	virtual ~oofRepMulti();
	
	virtual void	draw(oofRepEnvironment&);
	virtual bool isMultiReport() const;

	void appendReport(oofRep* adoptedReport);
	void appendReport(const oofRep&);
	oofRepMulti& operator<<(oofRep* adoptedReport);
	oofRepMulti& operator<<(const oofRep&);
	
	oofRepSection* section(unsigned long) const;
	oofRepSection* orphanSection(unsigned long);
	
	unsigned long count() const;

protected:
	OOF_ExpandableLongArray mSections;
	
friend class oofRepSection;	// allow access to parent attributes
};


/**
	Wrapper for a report contained within an oofRepMulti.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepSection : public oofRep {
public:
	oofRepSection(oofRepViewBand* adoptedBody=0, unsigned long startingPageNumber=1, bool resetPageNumber=true, bool pageBreak=true);
// 	use default copy ctor
	oofRepSection(const oofRep&, bool resetPageNumber=true, bool pageBreak=true);	// special copy ctor

	void	drawSection(oofRepEnvironment&, oofRepMulti* parentReport, unsigned short inSectionNumber);

	
	void setDimensions(oofRepEnvironment&, oofRepMulti* parentReport);
	

protected:
	bool mResetPageNumber, mPageBreak;
};


/**
	Cross-platform specification of text style.
	Attributes specified that identify OS font are fontName, size & style.
	Macintosh style constants are used, OOF_MacStyleEnumT on Windows.
	Rendering is further controlled by alignment, leading & color.
	Uses two-stage init via finishConstruction().
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepTextStyle {
public:
	oofRepTextStyle();  ///< mainly used by subclasses for delayed init, must set fontname & size before valid
	oofRepTextStyle(const char* fontName, unsigned short fontSize, unsigned char styleNum=0, short leading=0, oofColor color=oofColor::Black,oofColSizer::colAlignT Alin = oofColSizer::alignLeft);
	oofRepTextStyle(const char* fontName, unsigned short fontSize,oofColSizer::colAlignT Alin, unsigned char styleNum=0, short leading=0, oofColor color=oofColor::Black );
	// uses default copy ctor
	virtual ~oofRepTextStyle() {};
	
	void finishConstruction();  ///< public so delayed init can be performed by other classes
	bool invariant() const;  ///< true after full initialisation
	
#ifdef _Windows
//	oofRepTextStyle(const LOGFONT& lf);
#endif
// use default copy ctor	
	virtual oofRepTextStyle* clone() const;
	bool operator==(const oofRepTextStyle&) const;

	// getters
	const oofString& fontName() const;
	unsigned short fontSize() const;
	unsigned char fontStyle() const;
	short leading() const;
	const oofColor& color() const;
	oofColSizer::colAlignT alignment() const;
	// setters
	void fontName(const oofString&);
	void fontSize(unsigned short);
	void fontStyle(unsigned char);
	void leading(short);
	void color(const oofColor&);
	void alignment(oofColSizer::colAlignT) ;
	
	unsigned long fixedHeight();	
	void extract(ostream&) const;
	
#ifdef _Windows
	const LOGFONT& getLogFont() const;
#endif

private:
// data storage
#ifdef _Windows
	LOGFONT mLogFont;
#endif
	oofString mFontName;
	unsigned short mFontSize;
	unsigned char mStyleNum;
	short mLeading;
	oofColor mColor;
	oofColSizer::colAlignT mAlignment; 
};


/**
	Report elements that may have text style specified.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepTextStylable {
public:
	oofRepTextStylable(const char* fontName, unsigned short fontSize, unsigned char styleNum=0, short leading=0, oofColor color=oofColor::Black);
	oofRepTextStylable(oofRepTextStyle* adoptedStyle=0);
	oofRepTextStylable(const oofRepTextStylable&);
	virtual ~oofRepTextStylable();
	
	oofRepTextStyle* localTextStyle() const;
	void textStyle(oofRepTextStyle*);
	void textStyle(const char* fontName, unsigned short fontSize, unsigned char styleNum=0, short leading=0, oofColor color=oofColor::Black);
	virtual oofRepTextStyle* textStyle() const;

protected:
// data storage
	oofRepTextStyle* mStyle;	// owned
};


/**
	Base rendering class.
	<em>WARNING if you add methods here they should be virtual and usually overriden
	in oofRepRenderDecorator</em>
	For each oofRepDrawable you add an oofRepRender for each oofRepEnvironment
	in which you want that oofRepDrawable able to be seen.
*/
class OOFREP_EXPORT oofRepRender {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const=0;
	virtual ~oofRepRender() {};
	virtual void finishConstruction() {};
	virtual void draw() {};
	virtual void describeLayout() {};
	
	virtual long leftDrawMargin() const;
	virtual unsigned long width() const;
};


/**
	Decorator pattern - forwards most calls directly to owned renderer.
	but records draw position to build a page map used in editable previews
	to sense where the user clicks.
*/
class OOFREP_EXPORT oofRepRenderDecorator : public oofRepRender {
private:
	oofRepRenderDecorator(const oofRepRenderDecorator&) { assert(0); };  // use clone()

public:
	oofRepRenderDecorator(oofRepRender* adoptedRealRender=0);
	virtual ~oofRepRenderDecorator();
	void adoptRender(oofRepRender* adoptedRealRender);

// methods forwarded
	virtual void finishConstruction();
	virtual void draw();
	virtual void describeLayout();
	virtual long leftDrawMargin() const;
	virtual unsigned long width() const;
	
protected:
	oofRepRender* mWrappedRender;	// owned
};


/**
	Abstract base for anything drawn on report.
	Reponsible for redirecting to actual render object.
	<em>When you add a new oofRepEnvironment you will need to add a makeRender method here</em>
*/ 
class OOFREP_EXPORT oofRepDrawable : public oofRepTextStylable, public OOF_mixAdornable {
public:
	oofRepDrawable(oofRepTextStyle* adoptedStyle=0);
	oofRepDrawable(const oofRepDrawable& rhs);
	virtual ~oofRepDrawable();
	virtual void finishConstruction();

	virtual void draw();
	virtual void describeLayout();
	virtual dbView* view(unsigned short whichView=0) const;  // lots of drawables may be able to show a dbView
	
	virtual unsigned long fixedHeight();		// placed in base class for adorner call
	virtual unsigned long width();

/** 
	\name multi-method dispatch to factories for rendering report objects.
	<em>one for each environment</em>
	\note don't use pure virtuals as some subclasses may not implement
*/
//@{
	virtual void	makeRender(const oofRepMacEnv&);
	virtual void	makeRender(const oofRepWinEnv&);
	virtual void	makeRender(const oofRepTextEnv&);
	virtual void	makeRender(const oofRepRTFEnv&);
	virtual void	makeRender(const oofRepHTMLEnv&);
	virtual void	makeRender(const oofRepCSVEnv&);
	virtual void	makeRender(const oofRepTSVEnv&);
	virtual void	makeRender(const oofRepXMLEnv&);

#ifndef OOF_REP_NO_GUI	
	virtual void	makeRender(const oofRepEditingEnv&);
//@}
	virtual oofRepEditLink* makeEditLink(const oofRepEditingEnv&);
#endif	
	oofString ID() const;
	void ID(const oofString&);
	void clearID();
	
protected:
	oofRepRender*	mRender;	// owned
	oofString mID;  // up to environment if this is used or not, and may not use for all drawables at a time!
};


/**
	Abstract base for report bands.
	The complicated subclasses of interest are oofRepViewBand and oofRepLayoutBand.
	All bands apart from oofRepViewBand are fixed height.
*/
class OOFREP_EXPORT oofRepBand : public oofRepDrawable {
public:
	virtual ~oofRepBand() {};
	virtual oofRepBand* clone() const=0;
	virtual unsigned long fixedHeight();
	virtual unsigned long minHeight();
	virtual bool isViewBand() const;
	virtual bool isLayoutBand() const;  // layout band may be subclassed in future, to be a different layout 
	virtual void drawSquished(long& balanceToSave);
};


/**
	Specify a band which provides a vertical gap on the page.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepSpaceBand : public oofRepBand {
public:
	oofRepSpaceBand(unsigned long heightToSkip);
// use default copy ctor	
	virtual ~oofRepSpaceBand() {};
	virtual oofRepBand* clone() const;
	virtual unsigned long fixedHeight();

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);//dg 22/2/99

private:
// data storage
	unsigned long mHeight;
};


/**
	Specify a fixed height band containing an oofGraph with additional height & width.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepGraphBand : public oofRepBand {
public:
	oofRepGraphBand(oofGraph*, unsigned long height, 
					unsigned long grHgt=400, unsigned long grWid=400, const char* inGraphTitle=0);
	oofRepGraphBand(const oofRepGraphBand& rhs);	
	virtual ~oofRepGraphBand();
	virtual void finishConstruction();
	virtual oofRepBand* clone() const;

	virtual void drawSquished(long& balanceToSave);
	virtual void draw();
	virtual dbView* view(unsigned short whichView=0) const;

// getters	
	virtual unsigned long fixedHeight();
	virtual unsigned long minHeight();
	oofGraph* graph() const;
	unsigned long graphWidth() const;
	unsigned long graphHeight() const;
	unsigned long originalGraphWidth() const;
	unsigned long originalGraphHeight() const;
	unsigned long drawingHeight() const;
	unsigned long numOverlaid() const;

// setters	
	void fixedHeight( unsigned long height ) { mHeight = height; }
	void graphWidth( unsigned long grWidth );
	void graphHeight( unsigned long grHeight );


// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv&);
	
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);//dg 22/2/99

private:
	void AdjustGraphToBandHeight(unsigned long inHeight);
	void RestoreDimensions();
	
// data storage
	unsigned long 	mHeight;	// public fixedHeight() same as other bands
	oofGraph* 		mGraphPtr;  // owned
	unsigned long 	mGrHgt; 
	unsigned long 	mGrWid;
	unsigned long	mMinHeight;	// min size we will scale down to normally
	unsigned short	mMinSizePercent;
	unsigned long	mDrawingHeight;	// scaled drawing height
	unsigned long 	mSavedGrHgt, mSavedGrWid; // saved values whilst scaled
	enum {kUseDefaultScalePercent=0};
};


/**
	Specify a fixed height band to draw a horizontal line black or grey in given thickness.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepLineBand : public oofRepBand {
public:
	oofRepLineBand(unsigned long heightInPoints=2, bool isGrey=true);
// use default copy ctor	
	virtual ~oofRepLineBand() {};
	virtual oofRepBand* clone() const;
	virtual unsigned long fixedHeight();
	virtual bool grey();

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env); //dg 22/2/99

private:
// data storage
	unsigned long	mHeight;
	bool			mGrey;
};


/**
	Specify a fixed-height band that contains a series of oofRepBlock's.
	Blocks are either centred, right-aligned (one only) or presume to be
	dropped in one after the other left-aligned. This default mode makes it
	easy to effectively append data without cluttering up your code with width
	calculations - the appropriate oofRepLayoutBand_Render will either physically
	append data (streamed output) or advance the drawing position.
	Each block has a property indicating if the vertical position should change
	after it is drawn, set and read by oofRepBlock::moveDownAfter().
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepLayoutBand : public oofRepBand {
public:
	oofRepLayoutBand();
// use default copy ctor	
	virtual ~oofRepLayoutBand() {};
	virtual oofRepBand* clone() const;
	virtual bool isLayoutBand() const;  // layout band may be subclassed in future, to be a different layout 
	virtual void draw();
	virtual unsigned long fixedHeight();
	
// getters	
	oofRepBlockList& blocks();

	static	oofRepLayoutBand*	currentLayout();
	
	oofRepLayoutBand& operator<<(dbField&);	
	oofRepLayoutBand& operator<<(dbField*);	
	oofRepLayoutBand& operator<<(const char*);	
	oofRepLayoutBand& operator<<(oofRepBlock* adoptedBlock);	
	oofRepLayoutBand& operator<<(const oofRepBlock& copiedBlock);	

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);
#ifndef OOF_REP_NO_GUI
	virtual void	makeRender(const oofRepEditingEnv& env);
#endif	
private:
	void BecomeCurrent();
	
// data storage
	oofRepBlockList mBlocks;
	bool mCalcHeight;
	unsigned long mFixedHeight;		// cache the value
	static oofRepLayoutBand*	sCurrentLayout;
};


class oofDE;

/**
	Abstract base for user column drawer.
	Allows you to draw any content you like within an oofRepViewBand.
	Doesn't distinguish between rendering environments, requiring you to
	override draw() for Mac, makeDrawDE() for Windows and plainText() for
	all stream environments.
	\todo factor into render classes so can intelligently output to other environments
	\ingroup oofRW oofRenderMac oofRenderWin
*/
class OOFREP_EXPORT oofRepCustomViewDrawer : public OOF_PublicBase,  public oofSerializable  {
public:
	virtual void draw(int left, int top)=0;
	virtual void calculateWrapping(const dbField*, unsigned long sizedWidth, unsigned long& wrappedHeight)=0;
	virtual unsigned long calcNextDrawHeight(unsigned long heightAvailable, unsigned long& heightUsed)=0;  // returns  heightLeftToDraw
	virtual oofString plainText(const dbField*);
#ifdef _Windows
	virtual oofDE* makeDrawDE(int left, int top)=0;  // create your own oofDE  to record the drawing state, instead of calling draw
#endif
};


/**
	The main band of a report from which you can hang other bands.
	Nested bands including other oofRepViewBand (deep hieararchies of
	5 or more bands have been used).
	This is the only band type which is variable height and so
	knows how to break across pages.
	Allows for attached headers and footers as well as calculations
	performed 
	
	\note a common idiom to put just a graph on a report is to have
	an empty dbView attached and put the graph in the heading of 
	the oofRepViewBand, which will draw nothing below the graph.
	
	\todo new report architecture which abstracts variable-height bands
	\todo allow simpler band types to be top band on report.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepViewBand : public oofRepBand {
public:
	oofRepViewBand(const dbView&, bool isBoxed=false);
	oofRepViewBand(dbView* adoptedView, bool isBoxed=false);
	oofRepViewBand(const oofRepViewBand&);
	virtual ~oofRepViewBand();
	virtual oofRepBand* clone() const;
	
	virtual bool isViewBand() const;
	virtual dbView* view(unsigned short whichView=0) const;
	
	virtual void draw();
	virtual unsigned long fixedHeight();
	void incrementHeight( unsigned long addedHeight );
	void clearHeight();

	oofRepBandList& headers();
	oofRepBandList& footers();
	oofRepBreakList& headerBreaks();
	oofRepBreakList& footerBreaks();
	bool isBoxed() const;
	
	oofRepCustomViewDrawer* customDrawer(unsigned short col);
	void customDrawer(unsigned short col, oofRepCustomViewDrawer* adoptedDrawer);
	bool hasCustomViewDrawers() const;

	bool fieldIsNonEditable(unsigned short col) const;
	void setFieldNonEditable(unsigned short col);

// methods for the adorners to use to detect the edges of the tables
	bool lastRow( unsigned long row );
	bool lastCol( unsigned long col );
	
// methods to wrap tables in case it requires special pre and post information
	void startTableWrapper();
	void startTableBody();
	void endTableBody();
	void endTableWrapper();
	
	void drawColumnHeaders();
	void dirtyColHeaders();
	
	void setBoxed();

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);
#ifndef OOF_REP_NO_GUI
	
	virtual void	makeRender(const oofRepEditingEnv&);
	
	virtual oofRepEditLink* makeEditLink(const oofRepEditingEnv&);
#endif
private:
	void FinishConstruction();
// data storage
	oofRepBandList mHeaders;
	oofRepBandList mFooters;
	oofRepBreakList mHeaderBreaks;
	oofRepBreakList mFooterBreaks;
	dbView*	mView;
	bool	mIsBoxed;
	unsigned long mHeight;
	OOF_Dictionary	mCustomDrawers;
	OOF_mixViewExtraRender* mTableExtraRender;  // probably ref to 2nd interface of mRender
	bool* mNonEditableFields;  // owned
	bool mColHeadersNeedCalc;
};




/**
	Abstract base for a fixed-size block drawn by an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepBlock : public oofRepDrawable {
public:
	enum blockAlignT { alignLeft, alignCentre, alignRight};

	oofRepBlock(oofRepTextStyle* adoptedStyle=0);
	// use default copy ctor, op=
	virtual ~oofRepBlock() {};

	virtual oofRepBlock* clone() const=0;
	virtual oofRepTextStyle* textStyle() const;
	
	blockAlignT align() const;
	oofRepBlock& align(blockAlignT);
	
	bool moveDownAfter() const;
	void moveDownAfter(bool);
	virtual unsigned long fixedHeight();
	unsigned long fixedWidth();
	oofRepBlock&  fixedWidth(unsigned long);
	
	// interface for iterating and editing text pieces as if they were cells in array
	virtual unsigned int countCells() const;
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=0, short col=0) const;
	virtual void  setCellText(const oofString&, long row=0, short col=0);
	
protected:
// data storage
	unsigned long mFixedWidth;
	blockAlignT mAlignment;
	bool mMoveDownAfter;
};


/**
	Specify a block to draw mono-styled text in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepTextBlock : public oofRepBlock {
public:
	oofRepTextBlock(const char* str=0, oofRepTextStyle* adoptedStyle=0);
	oofRepTextBlock(const oofString&, oofRepTextStyle* adoptedStyle=0);	
//	oofRepTextBlock(ostream&, oofRepTextStyle* adoptedStyle=0);	// allow complex string construction
// use default copy ctor	
	virtual ~oofRepTextBlock() {};
	virtual oofRepBlock* clone() const;

// getters	
	virtual oofString text() const;

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);
#ifndef OOF_REP_NO_GUI	
	virtual void	makeRender(const oofRepEditingEnv&);
#endif	

	// interface for iterating and editing text pieces as if they were cells in array
	virtual unsigned int countCells() const;
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=0, short col=0) const;
	virtual void  setCellText(const oofString&, long row=0, short col=0);

private:
	void CompleteInit();

protected:
// data storage
	oofString mText;
};

/**
	Abstract base for drawing text content with some bracketing text in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepBracketedTextBlock : public oofRepTextBlock {
public:
	oofRepBracketedTextBlock(const char* leadingText, const char* trailingText, oofRepTextStyle* adoptedStyle=0);	
	// take only const char* args as our subclasses will often pass 0, avoid temporary construction

// getters	
	virtual oofString text() const;
	oofString leadingText() const;
	oofString trailingText() const;

	// interface for iterating and editing text pieces as if they were cells in array
	virtual unsigned int countCells() const;

protected:
	virtual oofString MiddleText() const=0;
	
protected:
// data storage
	oofString mTrailingText;
};


/**
	Specify a block to draw an oofPict in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPictBlock : public oofRepBlock, public OOF_mixPictOwner {
public:
	oofRepPictBlock(oofPict* adoptedPict, oofRepTextStyle* adoptedStyle=0);
	// use default copy ctor, dtor
	virtual oofRepBlock* clone() const;
	
	virtual unsigned long fixedHeight();
	virtual unsigned long width();

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);//dg 22/2/99
#ifndef OOF_REP_NO_GUI	
	virtual void	makeRender(const oofRepEditingEnv&);
#endif	

	// interface for iterating and editing text pieces as if they were cells in array
	virtual unsigned int countCells() const;
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=0, short col=0) const;
};


/**
	Specify a block to draw field contents with some bracketing text in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepFieldBlock : public oofRepBracketedTextBlock {
public:
	oofRepFieldBlock(dbField*, oofRepTextStyle* adoptedStyle=0);
	oofRepFieldBlock(const oofString& leadingText, dbField*, oofRepTextStyle* adoptedStyle=0);
	oofRepFieldBlock(const oofString& leadingText, dbField*, const oofString& trailingText, oofRepTextStyle* adoptedStyle=0);
// use default copy ctor	
	virtual ~oofRepFieldBlock() {};
	virtual oofRepBlock* clone() const;

	// interface for iterating and editing text pieces as if they were cells in array
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=0, short col=0) const;
	virtual void  setCellText(const oofString&, long row=0, short col=0);

protected:
	virtual oofString MiddleText() const;

private:
// data storage
	dbField* mField;
};


/**
	Specify a block to draw current page number with some bracketing text in an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPageNoBlock : public oofRepBracketedTextBlock {
public:
	oofRepPageNoBlock(const char* leadingText=0, const char* trailingText=0, oofRepTextStyle* adoptedStyle=0);
	oofRepPageNoBlock(const oofString& leadingText, oofRepTextStyle* adoptedStyle=0);
	oofRepPageNoBlock(const oofString& leadingText, const oofString& trailingText, oofRepTextStyle* adoptedStyle=0);
// use default copy ctor	
	virtual ~oofRepPageNoBlock() {};

	virtual void	makeRender(const oofRepXMLEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);

	// interface for iterating and editing text pieces as if they were cells in array
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=0, short col=0) const;
	virtual void  setCellText(const oofString&, long row=0, short col=0);

protected:
	virtual oofString MiddleText() const;
	
	virtual oofRepBlock* clone() const;
};


/**
	Calculation attached to an oofRepBand.
	Only used with oofRepViewBand but is designed in ignorance
	of that current limitation.
*/
class OOFREP_EXPORT oofRepBreak  : public oofSerializable {
public:
	oofRepBreak(oofRepBand* adoptedBand, bool newPageEveryTime=false);
	oofRepBreak(const oofRepBreak&);
	virtual ~oofRepBreak();
	virtual oofRepBreak* clone()=0;
	virtual bool breaks()=0;
	virtual void finishConstruction();
	
	void newPageEveryTime(bool);
	virtual bool newPage();	// allow overrides
	oofRepBand* band() const;
	
protected:
// data storage
	oofRepBand* mBand;	// owned
	bool mNewPageEveryTime;
};


/**
	Force break to a new page for each record drawn by oofRepViewBand.
	Commonly used in reports where you have nested data, eg: student
	results where each student starts a new page.
*/
class OOFREP_EXPORT oofRepBreakEveryRecord : public oofRepBreak {
public:
	oofRepBreakEveryRecord(oofRepBand* adoptedBand=0, bool newPageEveryTime=false);
	virtual ~oofRepBreakEveryRecord() {};
	virtual oofRepBreak* clone();
	virtual bool breaks();

// serialization methods
	static void registerSerializable();  
	static oofSerializable* makeBreak(const oofString&);
	virtual oofString serialize() const;  
	virtual oofString signature() const;
};


/**
	Specify a fixed height band to draw an oofPict.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPictBand : public oofRepBand, public OOF_mixPictOwner {
public:
	oofRepPictBand(oofPict* adoptedPict);
	// use default copy ctor, dtor

	virtual oofRepBand* clone() const;
	virtual unsigned long fixedHeight();
	unsigned long fixedWidth();

// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv& env);
	virtual void	makeRender(const oofRepWinEnv& env);
	virtual void	makeRender(const oofRepTextEnv& env);
	virtual void	makeRender(const oofRepRTFEnv& env);
	virtual void	makeRender(const oofRepHTMLEnv& env);
	virtual void	makeRender(const oofRepCSVEnv& env);
	virtual void	makeRender(const oofRepTSVEnv& env);
	virtual void	makeRender(const oofRepXMLEnv& env);// dg 22/2/99

};


/**
	Abstract report-writer drawing element.
	Used primarily for Windows - we had to implement our own 
	after Windows NT proved unreliable at redrawing metafiles.
	\todo migrate the Mac PICT-based drawing to use oofDE as
	some users have found this a useful bottleneck.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofDE { 
public:
	virtual ~oofDE() {};	
	virtual void draw() const=0;
	static void appendElement(oofDE* adoptedDE);
};


/**
	Abstract report-writer drawing element with position.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofPositionedDE : public oofDE { 
public:
	oofPositionedDE(int x, int y) : 
		mX(x), mY(y) 
	{};
	
protected:
	int mX, mY;
};


/**
	Abstract report-writer drawing element with position & rectangular size.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofSizedDE : public oofPositionedDE { 
public:
	oofSizedDE(int x, int y, int right, int bottom) : 
		oofPositionedDE(x, y),
		mRight(right), 
		mBottom(bottom) 
	{};
	
	oofSizedDE(const oofRect& R) : 
		oofPositionedDE(R.left, R.top),
		mRight(R.right), 
		mBottom(R.bottom) 
	{};
	
	int width() const;
	int height() const;
	
protected:
	int mRight, mBottom;
};


/**
	Abstract report-writer drawing element for piece of mono-styled text.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofTextDE : public oofPositionedDE { 
public:
	oofTextDE(int x, int y, const oofString& inStr)	: 
		oofPositionedDE(x, y),
		mString(inStr)
	{};
	oofTextDE(int x, int y, const char* lpString, int cbString)	: 
		oofPositionedDE(x, y),
		mString(lpString, cbString)
	{};
	 
	virtual void draw() const=0;
	const oofString&	str() const;
	void str(const oofString&);
	
protected:
	oofString	mString;
};


/**
	Single page in a report writer preview window.
	\ingroup oofRW
*/
class oofRepPage : private OOF_ExpandableLongArray {
public:
	oofRepPage();
	void draw(const oofRect&);
	~oofRepPage();
	
	void appendElement(oofDE* adoptedElement);
	oofDE* element(unsigned long) const;
	OOF_ExpandableLongArray::count;
	
	static oofRepPage* currentlyConstructing();
private:
	static oofRepPage* sCurrentPage;	
};


/**
	Manage pages in a report writer preview.
	Only used for report environments drawin using oofDE.
	\see oofRepWinGUIPreviewMFC
	\ingroup oofRW
*/
class oofRepPageList : private OOF_ExpandableLongArray {
public:
	~oofRepPageList();
	
	void appendPage(oofRepPage* adoptedPage);
	oofRepPage* page(unsigned long) const;
	OOF_ExpandableLongArray::count;
	OOF_ExpandableLongArray::index;
	void deleteAll();
};



/**
	Abstract report-writer drawing element for change of style between text items.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofTextStyleDE : public oofDE { // report-writer drawing element
public:
	oofTextStyleDE(const oofRepTextStyle* copiedStyle);
	virtual ~oofTextStyleDE();
	 
	const oofRepTextStyle*	style() const;
	
protected:
	oofRepTextStyle*	mStyle;  // owned
};



/**
	Setup databases for editing.
	In initial implementation used dbTable::bufferForever so no changes in
	preview window would be propagated back to database.
	If overriden, could do any setup you liked.
	\par
	Created by overrideable factory oofRepEditor::makeRepEditingSetupVisitor.
	\todo determine if this abstraction is needed - it appears unused
	\ingroup oofRepEdit
*/
class OOFREP_EXPORT OOF_repBufferSettingVisitor : public oofRepSchemaVisitor {
public:
	OOF_repBufferSettingVisitor(bool settingBufferForever) : 
		mSettingBufferForever(settingBufferForever)
	{};
	
protected:
	virtual void VisitView(dbView*);
	
	bool mSettingBufferForever; 
};


// -------------------------------------------------------
//     o o f R e p R e n d e r D e c o r a t o r
// -------------------------------------------------------
inline void 
oofRepRenderDecorator::adoptRender(oofRepRender* adoptedRealRender)
{
	assert(!mWrappedRender);
	mWrappedRender = adoptedRealRender;
}


// -------------------------------------------------------
//          o o f R e p D r a w a b l e
// -------------------------------------------------------
inline oofString 
oofRepDrawable::ID() const
{
	return mID;
}


inline void 
oofRepDrawable::ID(const oofString& inID)
{
	assert(mID.isEmpty() || (mID==inID));
	mID = inID;
}


inline void 
oofRepDrawable::clearID()
{
	mID.clear();
}



// -------------------------------------------------------
//                     o o f R e p
// -------------------------------------------------------
inline oofRepBandList& 
oofRep::pageHeaders()
{
	return mPageHeaders;
}


inline oofRepBandList& 
oofRep::pageFooters()
{
	return mPageFooters;
}

inline oofRepBandList& 
oofRep::firstPageHeaders()
{
	return mFirstPageHeaders;
}


inline oofRepBandList& 
oofRep::firstPageFooters()
{
	return mFirstPageFooters;
}

inline oofRepEnvironment*
oofRep::environment() const
{
	return mDrawingEnv;
}


inline unsigned long
oofRep::pageNumber() const
{
	return mPageNumber;
}


inline unsigned long
oofRep::startingPageNumber() const
{
	return mStartingPageNumber;
}


inline oofRepViewBand*
oofRep::body()
{
	return mTopBand;
}



inline unsigned long&
oofRep::currentVerticalPos()
{
	return mVerticalPos;
}


inline unsigned long
oofRep::bodyStart()
{
	return mBodyStart;
}


inline unsigned long
oofRep::bodyEnd()
{
	return mFootersStartAt;
}

inline unsigned long
oofRep::bodyHeight()
{
	return mFootersStartAt-mBodyStart;
}


inline unsigned long
oofRep::bodyWidth()
{
	return mBodyWidth;
}


inline unsigned long
oofRep::pageWidth()
{
	return mPageWidth;
}


inline unsigned long
oofRep::pageHeight()
{
	return mPageHeight;
}


inline void
oofRep::pageNumber(unsigned long n)
{
	mPageNumber = n;
}


inline void
oofRep::startingPageNumber(unsigned long n)
{
	mStartingPageNumber = n;
}


inline oofRep*
oofRep::currentReport()
{
	return sCurrentReport;
}

inline void 
oofRep::resetViewLevel()
{
	mNestedViewLevel = 0;
}


inline void 
oofRep::incrementViewLevel()
{
	mNestedViewLevel++;
}


inline void 
oofRep::decrementViewLevel()
{
	mNestedViewLevel--;
}


inline unsigned short 
oofRep::nestedViewLevel() const
{
	return mNestedViewLevel;
}


inline bool
oofRep::reportAllowsEditing() const
{
	return (settings()->reportEditor() != NULL);
}


// -------------------------------------------------------
//            o o f R e p M u l t i
// -------------------------------------------------------
inline unsigned long 
oofRepMulti::count() const
{
	return mSections.count();
}


inline oofRepSection* 
oofRepMulti::section(unsigned long n) const
{
	return (oofRepSection*) mSections.value(n);
}


// -------------------------------------------------------
//              o o f R e p V i e w B a n d
// -------------------------------------------------------
inline oofRepBandList& 
oofRepViewBand::headers()
{
	return mHeaders;
}


inline oofRepBandList& 
oofRepViewBand::footers()
{
	return mFooters;
}


inline oofRepBreakList& 
oofRepViewBand::headerBreaks()
{
	return mHeaderBreaks;
}


inline oofRepBreakList& 
oofRepViewBand::footerBreaks()
{
	return mFooterBreaks;
}


inline void 
oofRepViewBand::setBoxed()
{
	// Place a check in there to make sure they aren't trying to set it more than once.
	if( mIsBoxed==false ) {
		adorners() << new oofBorderAdorner( oofBorderAdorner::oneThin );
	}
	
	mIsBoxed=true;
}


inline bool 
oofRepViewBand::isBoxed() const
{
	return mIsBoxed;
}


inline bool 
oofRepViewBand::hasCustomViewDrawers() const
{
	return (mCustomDrawers.count()>0);
}


inline bool 
oofRepViewBand::fieldIsNonEditable(unsigned short col) const
{
	// if no flags ever set, no array exists
	return (mNonEditableFields && mNonEditableFields[col]);
}


inline void
oofRepViewBand::dirtyColHeaders()
{
	mColHeadersNeedCalc = true;
}


// -------------------------------------------------------
//            o o f R e p S p a c e B a n d 
// -------------------------------------------------------
inline unsigned long
oofRepSpaceBand::fixedHeight()
{
	return mHeight;
}


// -------------------------------------------------------
//            o o f R e p L i n e B a n d 
// -------------------------------------------------------
inline unsigned long
oofRepLineBand::fixedHeight()
{
	return mHeight;
}

inline bool
oofRepLineBand::grey()
{
	return mGrey;
}


// -------------------------------------------------------
//            o o f R e p L a y o u t B a n d 
// -------------------------------------------------------
inline oofRepLayoutBand*
oofRepLayoutBand::currentLayout()
{
	return sCurrentLayout;
}


// -------------------------------------------------------
//          o o f R e p B l o c k
// -------------------------------------------------------
inline oofRepBlock::blockAlignT
oofRepBlock::align() const
{
	return mAlignment;
}

	
inline bool 
oofRepBlock::moveDownAfter() const
{
	return mMoveDownAfter;
}


inline unsigned long
oofRepBlock::fixedHeight()
{
	return textStyle()->fixedHeight();		
}


inline unsigned long 
oofRepBlock::fixedWidth() 
{
	return mFixedWidth;
}



// -------------------------------------------------------
//          o o f R e p B l o c k
// -------------------------------------------------------
inline oofString 
oofRepBracketedTextBlock::leadingText() const
{
	return mText;
}


inline oofString 
oofRepBracketedTextBlock::trailingText() const
{
	return mTrailingText;
}



// -------------------------------------------------------
//            o o f R e p B a n d L i s t
// -------------------------------------------------------

inline void 
oofRepBandList::start()
{
	mBands.start();
}


inline bool 
oofRepBandList::more() const
{
	return mBands.more();
}
	
	
inline void
oofRepBandList::next()
{
	mBands.next();
}

inline void
oofRepBandList::deleteItem(unsigned long index)
{
	mBands.deleteItem(index);

}
inline unsigned long 
oofRepBandList::count() const
{
	return mBands.count();
}


inline oofRepBand* 
oofRepBandList::operator()()
{
	oofRepBand* ret = (oofRepBand*) mBands();	// safe downcast
	return ret;
}


inline oofRepBand* 
oofRepBandList::current()
{
	oofRepBand* ret = (oofRepBand*) mBands();	// safe downcast
	return ret;
}


inline oofRepBand* 
oofRepBandList::value(unsigned long index) const
{
	oofRepBand* ret = (oofRepBand*) mBands.value(index);	// safe downcast
	return ret;
}


inline oofRepBandList& 
oofRepBandList::operator<<(oofRepBand* adoptedBand)
{
	append(adoptedBand);
	return *this;
}


inline oofRepBandList& 
oofRepBandList::operator<<(const oofRepBand& prototypeBand)
{
	append(prototypeBand.clone());
	return *this;
}


inline unsigned long 
oofRepBandList::minHeight() const
{
	return mMinHeight;
}


inline bool 
oofRepBandList::keepTogether() const
// user enquiry expected to be called after finishConstruction
{
	assert(mKeepTogether != eDefaultTogether);
	const bool ret = (mKeepTogether == eKeepTogether);
	return ret;
}


inline void 
oofRepBandList::keepTogether(bool willKeep)
{
	if (willKeep)
		mKeepTogether = eKeepTogether;
	else
		mKeepTogether = eAllowSplit;
}


// -------------------------------------------------------
//            o o f R e p B l o c k L i s t
// -------------------------------------------------------

inline void 
oofRepBlockList::start()
{
	mBlocks.start();
}


inline bool 
oofRepBlockList::more() const
{
	return mBlocks.more();
}
	
	
inline void
oofRepBlockList::next()
{
	mBlocks.next();
}


inline unsigned long 
oofRepBlockList::count() const
{
	return mBlocks.count();
}


inline oofRepBlock* 
oofRepBlockList::operator()()
{
	oofRepBlock* ret = (oofRepBlock*) mBlocks();	// safe downcast
	return ret;
}


inline oofRepBlock* 
oofRepBlockList::current()
{
	oofRepBlock* ret = (oofRepBlock*) mBlocks();	// safe downcast
	return ret;
}


inline oofRepBlock* 
oofRepBlockList::value(unsigned long index) const
{
	oofRepBlock* ret = (oofRepBlock*) mBlocks.value(index);	// safe downcast
	return ret;
}


inline oofRepBlockList& 
oofRepBlockList::operator<<(oofRepBlock* adoptedBlock)
{
	append(adoptedBlock);
	return *this;
}


// -------------------------------------------------------
//            o o f R e p B r e a k L i s t
// -------------------------------------------------------

inline void 
oofRepBreakList::start()
{
	mBreaks.start();
}


inline bool 
oofRepBreakList::more() const
{
	return mBreaks.more();
}
	
	
inline void
oofRepBreakList::next()
{
	mBreaks.next();
}


inline unsigned long 
oofRepBreakList::count() const
{
	return mBreaks.count();
}


inline oofRepBreak* 
oofRepBreakList::operator()()
{
	oofRepBreak* ret = (oofRepBreak*) mBreaks();	// safe downcast
	return ret;
}


inline oofRepBreak* 
oofRepBreakList::current()
{
	oofRepBreak* ret = (oofRepBreak*) mBreaks();	// safe downcast
	return ret;
}


inline oofRepBreak* 
oofRepBreakList::value(unsigned long index) const
{
	oofRepBreak* ret = (oofRepBreak*) mBreaks.value(index);	// safe downcast
	return ret;
}

inline oofRepBreakList& 
oofRepBreakList::operator<<(oofRepBreak* adoptedBreak)
{
	append(adoptedBreak);
	return *this;
}


// -------------------------------------------------------
//            o o f R e p B r e a k 
// -------------------------------------------------------
inline oofRepBand*
oofRepBreak::band() const
{
	return mBand;
}


inline void
oofRepBreak::newPageEveryTime(bool newPage)
{
	mNewPageEveryTime = newPage;
}


// -------------------------------------------------------
//      o o f R e p D e f a u l t S e t t i n g s 
// -------------------------------------------------------
inline oofRepSettings *
oofRepDefaultSettings::settings()
{
	return sDefaultSettings;
}


// -------------------------------------------------------
//     o o f R e p T e x t S t y l  e
// -------------------------------------------------------
inline const oofString& 
oofRepTextStyle::fontName() const
{
	assert(invariant());
	return mFontName;
}


inline unsigned short 
oofRepTextStyle::fontSize() const
{
	assert(invariant());
	return mFontSize;
}


inline unsigned char 
oofRepTextStyle::fontStyle() const
{
	return mStyleNum;
}


inline short 
oofRepTextStyle::leading() const
{
	return mLeading;
}


inline const oofColor& 
oofRepTextStyle::color() const
{
	return mColor;
}


inline oofColSizer::colAlignT
oofRepTextStyle::alignment() const
{
	return mAlignment;
}


inline void
oofRepTextStyle::fontName(const oofString& inName)
{
	mFontName = inName;
}


inline void 
oofRepTextStyle::fontSize(unsigned short inSize)
{
	mFontSize = inSize;
}


inline void 
oofRepTextStyle::fontStyle(unsigned char inStyle)
{
	mStyleNum = inStyle;
}


inline void 
oofRepTextStyle::leading(short inLeading)
{
	mLeading = inLeading;
}


inline void 
oofRepTextStyle::color(const oofColor& inColor)
{
	mColor = inColor;
}

inline void
oofRepTextStyle::alignment(oofColSizer::colAlignT inAlignment)
{
	mAlignment = inAlignment;
} 

#ifdef _Windows
inline const LOGFONT&
oofRepTextStyle::getLogFont() const
{
	return mLogFont;
}
#endif


inline bool
oofRepTextStyle::invariant() const 
{
	const bool ret = 
		!mFontName.isEmpty() &&
		(mFontSize > 0)
#ifdef _Windows
		&& (mLogFont.lfHeight != 0)   // absolute test, don't assume positive
#endif
	;
	return ret;
}


// -------------------------------------------------------
//     o o f R e p T e x t S t y l a b l e
// -------------------------------------------------------
inline oofRepTextStyle* 
oofRepTextStylable::localTextStyle() const
{
	return mStyle;
}


inline ostream&
operator<<(ostream& os, oofRepTextStyle* S) 
{
	S->extract(os);
	return os;
}


// -------------------------------------------------------
//            o o f R e p G r a p h B a n d 
// -------------------------------------------------------
inline oofGraph* 
oofRepGraphBand::graph() const
{
	return mGraphPtr;
}


inline unsigned long 
oofRepGraphBand::graphWidth() const 
{
	return mGrWid;
}


inline unsigned long 
oofRepGraphBand::graphHeight() const 
{
	return mGrHgt;	
}


inline unsigned long 
oofRepGraphBand::originalGraphWidth() const 
{
	return mSavedGrWid;
}


inline unsigned long 
oofRepGraphBand::originalGraphHeight() const 
{
	return mSavedGrHgt;	
}


inline unsigned long 
oofRepGraphBand::drawingHeight() const 
{
	return mDrawingHeight;
}



// -------------------------------------------------------
//            o o f R e p L a y o u t B a n d 
// -------------------------------------------------------
inline oofRepBlockList& 
oofRepLayoutBand::blocks() 
{
	return mBlocks;
}

// -------------------------------------------------------
//       o o f R e p S t r e a m E n v 
// -------------------------------------------------------
inline ostream&
oofRepStreamEnv::out()
{
	return *sOut;
}

// -------------------------------------------------------
//            o o f R e p P i c t B a n d 
// -------------------------------------------------------
inline unsigned long
oofRepPictBand::fixedHeight()
{
	return OOF_mixPictOwner::height();
}

inline unsigned long
oofRepPictBand::fixedWidth()
{
	return OOF_mixPictOwner::width();
}


// -------------------------------------------------------
//                 o o f R e p P a g e L i s t
// -------------------------------------------------------
inline oofRepPage* 
oofRepPageList::page(unsigned long n) const
{
	return (oofRepPage*)value(n);
}

	
inline void
oofRepPageList::appendPage(oofRepPage* adoptedPage)
{
	append((unsigned long)adoptedPage);
}


// -------------------------------------------------------
//                 o o f R e p P a g e
// -------------------------------------------------------
inline oofDE* 
oofRepPage::element(unsigned long n) const
{
	return (oofDE*)value(n);
}

	
inline void
oofRepPage::appendElement(oofDE* adoptedElement)
{
	append((unsigned long)adoptedElement);
}


inline oofRepPage* 
oofRepPage::currentlyConstructing()
{
	return sCurrentPage;
}



// -------------------------------------------------------
//                     o o f T e x t S t y l e D E 
// -------------------------------------------------------
inline const oofRepTextStyle*	
oofTextStyleDE::style() const
{
	return mStyle;
}



// -------------------------------------------------------
//    u t i l i t y   f u n c t i o n s
// -------------------------------------------------------
inline int 
twips( int pixels )
{
	return pixels*20;
}


// -------------------------------------------------------
//                     o o f T e x t D E 
// -------------------------------------------------------
inline const oofString&	
oofTextDE::str() const
{
	return mString;
}


inline void 
oofTextDE::str(const oofString& inStr)
{
	mString = inStr;
}


// -------------------------------------------------------
//       o o f S i z e d D E 
// -------------------------------------------------------
inline int
oofSizedDE::width() const
{
	return mRight-mX;
}


inline int
oofSizedDE::height() const
{
	return mBottom-mY;
}


#endif


