//////////////////////////////////////////////////////////////////
// oofPrnWn.h - Definition for dbRepPICTs & oofRepWinGUIPreviewMFC
//
// COPYRIGHT 1997 A.D. Software, All rights reserved

#ifndef H_OOFPRNWN
#define H_OOFPRNWN

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif
#include "OOFRpWin.h"

/**
	Conversion used in oofRepWinPrint::getPaperSizeFromSymbolicValue
	\ingroup oofMacros
*/
#define INCHENTRY(x, y) (LONG)((x)*254), (LONG)((y)*254)
/**
	Conversion used in oofRepWinPrint::getPaperSizeFromSymbolicValue
	\ingroup oofMacros
*/
#define MMENTRY(x, y)   (LONG)((x)*10), (LONG)((y)*10)


class CDC;  // forward
/**
	Specify MS Windows GUI drawing environment previewed in MFC window.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepWinGUIPreviewMFC : public oofRepWinEnv
{
public:
	oofRepWinGUIPreviewMFC(PAGESETUPDLG * pageSetup = NULL);
	~oofRepWinGUIPreviewMFC();
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	//virtual void 	moveToVerticalPos(unsigned long); // see oofRepEnvironment comments
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	
	virtual void		finish();
	virtual void		start();
	void				setPageSetup(PAGESETUPDLG * pageSetup);
	static void 		initDefaultPageSetup(PAGESETUPDLG*);

	oofRect*			getMargin() { return &mMargin; }
	virtual oofRect*	pageRectPts() { return &mPaperRect; }

	virtual void drawPageBorder( long top, long left, long bottom, long right );
	
	void drawWithTextStyle(oofRepTextStyle*);  // 
	
	oofRepPageList& pages();
	unsigned long getNumPages() const;
	unsigned long previewPageOrdinal() const;  // returns ordinal number, NOT the printable page number which may reset

	void drawPage(HDC, unsigned long pageIndex, oofRect);

	void parentWindow(CWnd*);
	CWnd* parentWindow() const;
	
protected:
	void			startPage();
	void			endPage();

private:
	BOOL			mFinished;
	oofRect			mPaperRect;			// Paper Size in pts
	oofRect			mMargin;
	oofRepPageList mPages;
	oofRepPage*	mBuildingPage;
	oofRepTextStyle*	mCurrentTextStyle;	// owned
	CWnd*			mParentWnd;
	unsigned long	mCurrentPage;
};


/**
	Specify MS Windows GUI drawing environment being printed.
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofRepWinPrint : public oofRepWinGUIPreviewMFC
{
public:
	oofRepWinPrint(
		const char* docStr			= NULL,		// print job string description
		LPPAGESETUPDLG pageSetup	= NULL,		// page setup structure
		LPPRINTDLG printDlg			= NULL);	// print setup structure
	~oofRepWinPrint();

// private type defs
protected:
	typedef struct paperSizeLookupTag {
		short paperSizeID;
		LONG  width;
		LONG  height;
	} paperSizeLookup;

// operations
public:
	void print();
	void disablePrinting();

// static functions
public:
	// sets the mapping mode that will enlarge the old metafile outputs on the printer
	// (gets the application's printer defaults). We have to work with CDC because of
	// the preview device contexts that override functions like SetWindowExt, SetWindowOrg,
	// etc.
	static BOOL setPrinterMappingMode(CDC* pDC);
	// utility function that converts HDC into a temporary CDC and passes it to the CDC
	// version of setPrinterMappingMode
	static BOOL setPrinterMappingMode(HDC hDC);
	// Gets paper size from the provided handles
	static BOOL getPaperSize(HANDLE hDevNames, HANDLE hDevMode, LPPOINT paperSize);
	// gets the paper size from dmPaperSize field of DEVMODE strucure
	static BOOL getPaperSizeFromSymbolicValue(short paperSizeID, LPPOINT paperSize);

//	overrides
public:
	virtual bool isPrintingEnvironment() const;

protected:
	void PrintPage(unsigned long pageNo, const LPRECT pageRect, LPPRINTDLG pd);
	void GetLogicalPageRect(LPRECT rect, HDC dc) const;
	void FillInPrintDlg(LPPRINTDLG printDlg) const;


// attributes
protected:
	DOCINFO		mDocInfo;
	const char*	mDefaultPrintStr;
	LPPRINTDLG	mPrintDlg;
	bool		mPrinted;
};


// -------------------------------------------------------
//      o o f R e p W i n G U I P r e v i e w M F C
// -------------------------------------------------------
inline oofRepPageList& 
oofRepWinGUIPreviewMFC::pages()
{
	return mPages;
}


inline unsigned long
oofRepWinGUIPreviewMFC::getNumPages() const
{
	return mPages.count();
}


inline unsigned long
oofRepWinGUIPreviewMFC::previewPageOrdinal() const
{
	return mCurrentPage;
}


inline void 
oofRepWinGUIPreviewMFC::parentWindow(CWnd* inWnd)
{
	mParentWnd = inWnd;
}


inline CWnd* 
oofRepWinGUIPreviewMFC::parentWindow() const
{
	return mParentWnd;
}

// -------------------------------------------------------
//                 o o f R e p P r i n t
// -------------------------------------------------------
inline void
oofRepWinPrint::disablePrinting()
{
	mPrinted = true;
}

inline bool
oofRepWinPrint::isPrintingEnvironment() const
{
	return true;
}


#endif	// H_OOFPRNWN
