#ifndef H_OOFREPWIN
#define H_OOFREPWIN

// COPYRIGHT 1997 A.D. Software, All rights reserved

// GUI report-writer layer of OOFILE database


#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"
#endif
//  we need class oofSizedTextArray
#ifndef H_OOFDRAW
	#include "oofdraw.h"
#endif
#ifndef H_OOFRECT
	#include "oofrect.h"
#endif

/**
	\defgroup oofRenderWin MS Windows Report rendering
	Reports rendered to the Win32 GUI drawing environment
	able to be previewed and printed under MS Windows.
	Included in the Open Source OOFILE.
	\ingroup oofRW_render
*/

inline int pts2logical(int inPts, HDC inDC) { return MulDiv(inPts, ::GetDeviceCaps(inDC, LOGPIXELSY), 72);};  // eg: *96/72
inline int himetric2pts(int inPts) { return MulDiv(inPts, 72, 2540);};
inline int hienglish2pts(int inPts) { return MulDiv(inPts, 72, 1000);};
inline int himetric2logical(int inPts, HDC inDC) { return MulDiv(inPts,  ::GetDeviceCaps(inDC, LOGPIXELSY), 2540);};
inline int hienglish2logical(int inPts, HDC inDC) { return MulDiv(inPts,  ::GetDeviceCaps(inDC, LOGPIXELSY), 1000);};

class oofGraph;

// use common base to provide the factory methods
/**
	Base for specifying Win32 GUI drawing environment.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepWinEnv :  public oofRepEnvironment {
public:
	enum 	{eRenderType='Win '};
	
	virtual unsigned short heightOfTextStyle(oofRepTextStyle* theStyle);
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRenderDispatch(oofRepDrawable* drawable) const;
	virtual void	makeRenderDispatch(oofAdorner* drawable ) const;
	virtual oofRepRenderT renderType() const;
	virtual void startEnvironment();

	static SIZE getTextExtent(const oofRepTextStyle* textStyle, const OOF_String& text);
};


// Windows renderers for OOFILE database
/**
	Advance report vertical position.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepSpaceBand_WinRender : public oofRepSpaceBand_Render {
public:
	oofRepSpaceBand_WinRender(oofRepSpaceBand* b) : oofRepSpaceBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Draw colour or mono graph as specified by oofRepGraphBand.
	Depends on oofRepSettings::printGraphsInMono().
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepGraphBand_WinRender : public oofRepGraphBand_Render {
public:
	oofRepGraphBand_WinRender(oofRepGraphBand* b) : oofRepGraphBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Iterate blocks in oofRepLayoutBand changing pen position for each.
	\see oofRepLayoutRedirectingRender::addAreaForCurrentBlock. 
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepLayoutBand_WinRender : public oofRepLayoutBand_Render {
public:
	oofRepLayoutBand_WinRender(oofRepLayoutBand* b) : oofRepLayoutBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Draw a line as filled rectangle specified by oofRepLineBand.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepLineBand_WinRender : public oofRepLineBand_Render {
public:
	oofRepLineBand_WinRender(oofRepLineBand* b) : oofRepLineBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};

/**
	Call oofPict::draw positioned centred within oofRepPictBand.
	\ingroup oofRenderWin 
*/
class OOFREP_EXPORT oofRepPictBand_WinRender : public oofRepPictBand_Render {
public:
	oofRepPictBand_WinRender(oofRepPictBand* b) : oofRepPictBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual void draw();
};


/**
	Draw oofRepViewBand contents as grid with page breaks.
	Includes word wrapping of column headers via CalcColHeaderWrapping
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepViewBand_WinRender : public oofRepViewBand_Render {
public:
	oofRepViewBand_WinRender(oofRepViewBand* b) : oofRepViewBand_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;

	virtual void finishConstruction ();
	virtual void draw();	
	virtual void drawColumnHeaders();
	virtual void startTableBody(bool headersHaveChanged);

protected:
	void DrawContinuingOverPage( unsigned long, unsigned long, oofSizedTextArray& sizedBody, bool hasCustomDrawers);

	void CalcColHeaderWrapping();

// data storage
	oofSizedTextArray	mColHeaders;	
};


/**
	Draw text in the current style.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepTextBlock_WinRender : public oofRepTextBlock_Render {
public:
	oofRepTextBlock_WinRender(oofRepTextBlock* b) : oofRepTextBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual unsigned long width() const;
	virtual void draw();
};


/**
	Call oofPict::draw positioned centred within oofRepPictBlock.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepPictBlock_WinRender : public oofRepPictBlock_Render {
public:
	oofRepPictBlock_WinRender(oofRepPictBlock* b) : oofRepPictBlock_Render(b) {};
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	virtual unsigned long width() const;
	virtual void draw();
};


/**
	Draw text in Windows using current font settings.
	\ingroup oofRW oofWinSpecific
*/
class OOFREP_EXPORT oofWinTextDE : public oofTextDE {
public:
	oofWinTextDE(int x, int y, const oofString& inStr)	: 
		oofTextDE(x, y, inStr)
	{};
	oofWinTextDE(int x, int y, LPCTSTR lpString, int cbString)	: 
		oofTextDE(x, y, lpString, cbString)
	{};
	virtual void draw() const;
	
	static void recordElement(int x, int y, const oofString& inStr);
	static void recordElement(int x, int y, LPCTSTR lpString, int cbString);
};




/**
	Draw an oofGraph within a report.
	\todo rename or move to platform-agnostic oofrep.h
	\ingroup oofGraph
*/
class OOFREP_EXPORT oofGraphDE : public oofSizedDE { 
public:
	oofGraphDE(int x, int y, int right, int bottom, oofGraph* inGraph, bool isMono) :
		oofSizedDE(x, y, right, bottom),
		mGraph(inGraph), 
		mIsMono(isMono) 
	{};
	
	oofGraphDE(const oofRect& R, oofGraph* inGraph, bool isMono) :
		oofSizedDE(R),
		mGraph(inGraph), 
		mIsMono(isMono) 
	{};
	
	virtual void draw() const;
	
	static void recordElement(int x, int y, int right, int bottom, oofGraph* inGraph, bool isMono);
	static void recordElement(const oofRect& R, oofGraph* inGraph, bool isMono);
protected:
	oofGraph*	mGraph;
	bool	mIsMono;
};


/**
	Draw a line in a report.
	Draws from top left to bottom right of its rectangle so may slope down or be vertical.
	\todo allow colour specification.
	\ingroup oofRW oofWInSpecific
*/
class OOFREP_EXPORT oofWinLineDE : public oofSizedDE { 
public:
	oofWinLineDE(int x, int y, int right, int bottom) :
		oofSizedDE(x, y, right, bottom)
	{};
	
	oofWinLineDE(const oofRect& R) :
		oofSizedDE(R)
	{};
	
	virtual void draw() const;
	
	static void recordElement(int x, int y, int right, int bottom);
	static void recordElement(const oofRect& R);
};


/**
	Draw a rectangle in a report.
	\todo allow colour specification.
	\ingroup oofRW oofWInSpecific
*/
class OOFREP_EXPORT oofWinRectDE : public oofSizedDE { 
// for FrameRects
public:
	oofWinRectDE(int x, int y, int right, int bottom) :
		oofSizedDE(x, y, right, bottom)
	{};
	
	oofWinRectDE(const oofRect& R) :
		oofSizedDE(R)
	{};
	
	virtual void draw() const;
	
	static void recordElement(int x, int y, int right, int bottom);
	static void recordElement(const oofRect& R);
};


/**
	Draw a filled rectangle in a report.
	\see oofShadingAdorner_WinRender::drawcell  oofRepLineBand_WinRender::draw
	\ingroup oofRW oofWInSpecific
*/
class OOFREP_EXPORT oofWinShadeDE : public oofSizedDE { 
public:
	oofWinShadeDE(int x, int y, int right, int bottom, COLORREF fillColor) :
		oofSizedDE(x, y, right, bottom),
		mFillColor(fillColor),
		mUseStockBrush(false)
	{};
	
	oofWinShadeDE(const oofRect& R, COLORREF fillColor) :
		oofSizedDE(R),
		mFillColor(fillColor),
		mUseStockBrush(false)
	{};
	
	oofWinShadeDE(int x, int y, int right, int bottom, int stockBrush);	
	oofWinShadeDE(const oofRect& R, int stockBrush);
	
	virtual void draw() const;
	
	static void recordElement(int x, int y, int right, int bottom, COLORREF fillColor);
	static void recordElement(const oofRect& R, COLORREF fillColor);
protected:
	COLORREF mFillColor;
	bool mUseStockBrush;
	int mStockBrush;
};


/**
	Draw an oofPict in a report.
	\todo maybe allow clip or scale specification
	\ingroup oofRW oofWInSpecific
*/
class OOFREP_EXPORT oofWinPictDE : public oofSizedDE { 
public:
	oofWinPictDE(int x, int y, int right, int bottom, oofPict* inPict) :
		oofSizedDE(x, y, right, bottom),
		mPict(inPict)
	{};
	
	oofWinPictDE(const oofRect& R, oofPict* inPict) :
		oofSizedDE(R),
		mPict(inPict)
	{};
	
	oofWinPictDE(const oofWinPictDE&);
	virtual ~oofWinPictDE();
	
	virtual void draw() const;
	
	static void recordElement(int x, int y, int right, int bottom, oofPict*);
	static void recordElement(const oofRect& R, oofPict*);
protected:
	oofPict* mPict;  // ref to parent pict
};


/**
	Change current font settings to affect all oofWinTextDE from now until next change.
	\ingroup oofRW oofWinSpecific
*/
class OOFREP_EXPORT oofWinTextStyleDE : public oofTextStyleDE {
public:
	oofWinTextStyleDE(const oofRepTextStyle* copiedStyle)	: 
		oofTextStyleDE(copiedStyle)
	{};
	virtual void draw() const;
	
	static void recordElement(const oofRepTextStyle* copiedStyle);
	static void safeChangeStyle(oofRepTextStyle*);  // utility for others to use
};

#endif

