//////////////////////////////////////////////////////////////////
// oofPrnWn.cpp - MS-Windows implementation for oofRepWinGUIPreviewMFC
//
// COPYRIGHT 1996 A.D. Software, All rights reserved

#include "stdafx.h"

#include "oof1.h"
#include "oof3.h"
#include "oofram.h"
#include "oofrep.h"
#include "oofrect.h"
#include "oofmacro.h"
#include "oofDraw.h"
#include "oofPrnWn.h"


// -------------------------------------------------------
//     o o f R e p W i n G U I P r e v i e w M F C
// -------------------------------------------------------

oofRepWinGUIPreviewMFC::oofRepWinGUIPreviewMFC(PAGESETUPDLG * pageSetup)
	:	oofRepWinEnv(), 
	mPaperRect(0,0,0,0),
	mBuildingPage(0),
	mCurrentTextStyle(0),
	mParentWnd(0),
	mCurrentPage(0)
{
	if(!pageSetup) {
		// Get the default print record - for pagesizing, etc.
		PAGESETUPDLG	ps;
		initDefaultPageSetup(&ps);
		setPageSetup(&ps);
	} else 
		setPageSetup(pageSetup);

	mFinished	= FALSE;
}

oofRepWinGUIPreviewMFC::~oofRepWinGUIPreviewMFC()
{
	if(!mFinished)
		finish();

	oofCurrentDraw::deleteInfoDC();
	delete mCurrentTextStyle;
}

void
oofRepWinGUIPreviewMFC::initDefaultPageSetup(PAGESETUPDLG* ps)
{
	PRINTDLG		printDlg;

	memset(ps, 0, sizeof(PAGESETUPDLG));
	memset(&printDlg, '\0', sizeof(printDlg));
	bool successfulInit = false;
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL && pApp->GetPrinterDeviceDefaults(&printDlg)) {
		POINT paperSize;
		VERIFY(oofRepWinPrint::getPaperSize(printDlg.hDevNames, printDlg.hDevMode, &paperSize));
		if (paperSize.x!=0 && paperSize.y!=0)  {   // some dodgy drivers return zero!
			ps->ptPaperSize.x = paperSize.x * 10;
			ps->ptPaperSize.y = paperSize.y * 10;
			ps->Flags         = PSD_INHUNDREDTHSOFMILLIMETERS;
			successfulInit = true;
		}  // got valid paper size
	}
	
	if (!successfulInit) {  // old-fashioned default from driver
		ps->Flags		= PSD_RETURNDEFAULT | 
						  PSD_MARGINS | 
						  PSD_DEFAULTMINMARGINS | 
						  PSD_INTHOUSANDTHSOFINCHES;
		::PageSetupDlg(ps);
	}
}


void
oofRepWinGUIPreviewMFC::setPageSetup(PAGESETUPDLG* pageSetup)
{
	int newRight, newBottom;
	
	HDC ic = oofCurrentDraw::getInfoDC();
	
	if (pageSetup->Flags & PSD_INTHOUSANDTHSOFINCHES) {
		newRight = hienglish2pts(pageSetup->ptPaperSize.x);
		newBottom = hienglish2pts(pageSetup->ptPaperSize.y);
	}
	else {
		assert(pageSetup->Flags & PSD_INHUNDREDTHSOFMILLIMETERS);
		newRight = himetric2pts(pageSetup->ptPaperSize.x);
		newBottom = himetric2pts(pageSetup->ptPaperSize.y);
	}

/*
	if (pageSetup->Flags & PSD_INTHOUSANDTHSOFINCHES) {
		newRight = hienglish2logical(pageSetup->ptPaperSize.x, ic);
		newBottom = hienglish2logical(pageSetup->ptPaperSize.y, ic);
	}
	else {
		assert(pageSetup->Flags & PSD_INHUNDREDTHSOFMILLIMETERS);
		newRight = himetric2logical(pageSetup->ptPaperSize.x, ic);
		newBottom = himetric2logical(pageSetup->ptPaperSize.y, ic);
	}
*/
	mPaperRect	= oofRect(0, 0, newRight, newBottom);
//	mMargin		= pageSetup->rtMargin;
	
/*
	// We need to adjust for the reverse Y axis in MM_HIENGLISH or MM_HIMETRIC.
	// Because mPaperHimetric has been transformed twice (if at all), the effect is cancelled.
	mPaperRect.transform(1, -1);
	mMargin.transform(1, -1);
*/
}

void
oofRepWinGUIPreviewMFC::start()
{
	mFinished = FALSE;
	startPage();
}

void
oofRepWinGUIPreviewMFC::finish()
{
	endPage();
	mFinished = TRUE;
}

void 
oofRepWinGUIPreviewMFC::startPage()
{
	mBuildingPage = new oofRepPage();
	
	oofCurrentDraw::makeInfoDC();
}

void
oofRepWinGUIPreviewMFC::endPage()
{
	if (mBuildingPage) {
		mPages.appendPage(mBuildingPage);
		mBuildingPage = 0;  // adopted
			// dirty the text style to force selecting the current font when starting
		// a new page
	}
	changeTextStyle(NULL);
}


void
oofRepWinGUIPreviewMFC::drawPageBreak()
{
	assert(!mFinished); // Make sure someone hasn't finished up already !
	endPage();
	startPage();
}


void 
oofRepWinGUIPreviewMFC::changeTextStyle(oofRepTextStyle* inStyle)
{
	if (inStyle == NULL) {
		delete mCurrentTextStyle;
		mCurrentTextStyle = 0;
	}
	else {
		bool changedStyle = false;
		if (mCurrentTextStyle) {
			if (!(*mCurrentTextStyle==*inStyle))  // only set if different
				changedStyle = true;
				*mCurrentTextStyle = *inStyle;  // copy without reallocation
		}
		else {
			changedStyle = true;
			mCurrentTextStyle = inStyle->clone();
		}  // if current style
		// draw & record style regardless of whether we changed it.
		// we may be recording a style to guard against a custom drawer changing the display properties and bypassing
		// our text style mechanism
		drawWithTextStyle(inStyle);
		if (oofRepPage::currentlyConstructing())  // allow for us to be called at page replay time, when we would NOT add another element!
			oofWinTextStyleDE::recordElement(inStyle);
	} // in inStyle
}


void 
oofRepWinGUIPreviewMFC::drawWithTextStyle(oofRepTextStyle* style)
{
	ASSERT(style);
	HDC dc = oofCurrentDraw::getCurrentDC();
	HDC ic = oofCurrentDraw::getInfoDC();

	LOGFONT lf = style->getLogFont();
//	const int pixelsXY = ::GetDeviceCaps(ic, LOGPIXELSY);
	lf.lfHeight = -style->fontSize();  // -MulDiv(style->fontSize(), pixelsXY, 72);
	HFONT hFont = oofTextDrawer::createFontIndirect(&lf);

		if (hFont != NULL) {
		oofTextDrawer::selectFont(dc, hFont);  // will still set ic
	}
	if (dc != NULL) {
		// Set the foreground colour for text drawing
		VERIFY( CLR_INVALID != ::SetTextColor(dc, style->color()) );
	}  // have a DC
}


/*void 	
oofRepWinGUIPreviewMFC::moveToVerticalPos(unsigned long pos)
{
	MoveTo(0,pos);
}
*/ // See comments in oofRepEnvironment

unsigned long 
oofRepWinGUIPreviewMFC::calcPageHeight()
{
	return mPaperRect.height();
}

unsigned long 
oofRepWinGUIPreviewMFC::calcPageWidth()
{
// NOT YET IMPLEMENTED 
/* disabled 98/05/11 due to side-effects from initial settings
// update margin settings to match Windows margins we had copied earlier
	oofRepSettings* localSet = oofRep::currentReport()->getLocalSettings();
	localSet->leftMargin(mMargin.left);
	localSet->rightMargin(mMargin.right);
	localSet->topMargin(mMargin.top);
	localSet->bottomMargin(mMargin.bottom);
*/

// preset width from settings dialog
	return mPaperRect.width();
}

void
oofRepWinGUIPreviewMFC::drawPageBorder( long top, long left, long bottom, long right )
{
	oofRect theLine;

	theLine.set(left,top,right,bottom);
	HDC	hdc = oofCurrentDraw::getCurrentDC();
	
	oofWinRectDE::recordElement(theLine);
	
	theLine.set(left-2,top-2,right+2,bottom+2);
	oofWinRectDE::recordElement(theLine);

	theLine.set(left-3,top-3,right+3,bottom+3);
	oofWinRectDE::recordElement(theLine);

	theLine.set(left-5,top-5,right+5,bottom+5);
	oofWinRectDE::recordElement(theLine);
}


void 
oofRepWinGUIPreviewMFC::drawPage(HDC inDC, unsigned long pageIndex, oofRect inRect)
{
	assert(pageIndex < getNumPages());
	oofCurrentDraw::makeInfoDC();  // guarantee an info DC exists for redraw
	oofCurrentDraw::setCurrentDC(inDC); 
	
	if (!isPrintingEnvironment()) {
	// 99/02/15 adapt to external scaling
		CSize clientSize;
		oofRect pageRect = pageRectPts();
		const long inWidth = inRect.width();
		const int pixelsX = ::GetDeviceCaps(inDC, LOGPIXELSX);
		const long inWidthPts = ::MulDiv(inWidth, 72, pixelsX);
		const double scaleDivisor = double(inWidthPts) / pageRect.width();

// emulate the setting that's just occurred in the call to setCurrentDC
// but with scaling
		const int horzres = ::GetDeviceCaps(inDC, HORZRES);
		const int horzsize = ::GetDeviceCaps(inDC, HORZSIZE);  // always mm
		const int vertres = ::GetDeviceCaps(inDC, VERTRES);
		const int vertsize = ::GetDeviceCaps(inDC, VERTSIZE);
		
		const double devHorDPI = 25.4*horzres/horzsize;
		const double devVertDPI = 25.4*vertres/vertsize;
		
		const int horDP10i = 10 * devHorDPI;
		const int vertDP10i = 10 * devVertDPI;
		assert(horDP10i<32767);  // safe for Win95
		assert(vertDP10i<32767);  // safe for Win95
		SIZE oldViewExtent, oldWindowExtent;
		::SetMapMode(inDC, MM_ANISOTROPIC);
		::SetViewportExtEx(inDC, horDP10i, vertDP10i, &oldViewExtent);
		::SetWindowExtEx(inDC, 720/scaleDivisor, 720/scaleDivisor, &oldWindowExtent);
	}	
	 
	oofTextDrawer::cleanupFont();
	oofRepPage* thePage = mPages.page(pageIndex);
	ASSERT(thePage);
	thePage->draw(inRect);		
	oofCurrentDraw::setCurrentDC(0);
	mCurrentPage = pageIndex;
}


// -------------------------------------------------------
//               o o f R e p W i n P r i n t
// -------------------------------------------------------

oofRepWinPrint::oofRepWinPrint(const char* docStr, LPPAGESETUPDLG pageSetup, LPPRINTDLG printDlg) :
	oofRepWinGUIPreviewMFC(pageSetup),
	mDefaultPrintStr("OOFile report"),
	mPrintDlg(printDlg),
	mPrinted(false)
{
	dbConnect_ram tempConnection;
	tempConnection.newConnection();

	// set up mDocInfo structure
	memset(&mDocInfo, '\0', sizeof(DOCINFO));
	mDocInfo.cbSize = sizeof(DOCINFO);
	mDocInfo.lpszDocName = docStr != NULL ? docStr : mDefaultPrintStr;
	mDocInfo.lpszOutput = NULL;

	start();
}

oofRepWinPrint::~oofRepWinPrint()
{
	if (!mPrinted)
		print();
}

void oofRepWinPrint::FillInPrintDlg(LPPRINTDLG printDlg) const
{
	printDlg->lStructSize	= sizeof(PRINTDLG);
	printDlg->Flags			= PD_NOSELECTION | PD_PAGENUMS | PD_RETURNDC;
	printDlg->nCopies		= 1;
	printDlg->nFromPage		= 1;
	printDlg->nMinPage		= printDlg->nFromPage;
	printDlg->nToPage		= (unsigned short)getNumPages();
	printDlg->nMaxPage		= printDlg->nToPage;
}

void oofRepWinPrint::print()
{
	mPrinted = true;

	finish();

	PRINTDLG	printDlg;
	LPPRINTDLG	pd = mPrintDlg;

	if (pd == NULL) {
		memset(&printDlg, '\0', sizeof(printDlg));

		FillInPrintDlg(&printDlg);

		if (!::PrintDlg(&printDlg))
			return;
		else
			pd = &printDlg;
	}
	else {
		pd->nCopies = max(1, pd->nCopies);

		if (pd->hDC == NULL) {
			// printer DC has to be obtained, so fill in the required fields
			FillInPrintDlg(pd);

			if (!::PrintDlg(pd))
				return;
		}
	}

	assert(pd != NULL);
	assert(pd->hDC != NULL);

	RECT pageRect;
	GetLogicalPageRect(&pageRect, pd->hDC);

	unsigned int firstPage = 0;
	unsigned int lastPage  = getNumPages();

	if (pd->Flags & PD_PAGENUMS) {
		firstPage = max(0, pd->nFromPage-1);  // 0-based index
		lastPage  = min(getNumPages(), pd->nToPage);
	}

	if (::StartDoc(pd->hDC, &mDocInfo) > 0) {
		// handle collated copies
		for (WORD colCopy = 0; colCopy < (pd->Flags & PD_COLLATE ? pd->nCopies : 1); colCopy++) {
			for (unsigned long i = firstPage; i < lastPage; i++) {
				PrintPage(i, &pageRect, pd);
			}
		}
		::EndDoc(pd->hDC);
	}

	// delete the DC
	VERIFY(::DeleteDC(pd->hDC));
	// print() might be called multiple times, so set hDC to NULL
	pd->hDC = NULL;
}

void oofRepWinPrint::PrintPage(unsigned long pageNo, const LPRECT pageRect, LPPRINTDLG pd)
{
	// handle noncollated copies
	for (WORD nonColCopy = 0; nonColCopy < (pd->Flags & PD_COLLATE ? 1 : pd->nCopies); nonColCopy++) {
			// Set the mapping mode that will convert the report's dimension from screen
			// into the printer resolution. We needn't do that for the enhanced metafile
			// as it allows the sizing directly via pageRect. We have to set the mapping
			// mode for each page individually.
			if (::StartPage(pd->hDC) > 0) {
				VERIFY(setPrinterMappingMode(pd->hDC));
				drawPage(pd->hDC, pageNo, pageRect);
				::EndPage(pd->hDC);
			}
/*		{
#ifdef OOFILE_USE_OLDMETAFILE
			// Set the mapping mode that will convert the report's dimension from screen
			// into the printer resolution. We needn't do that for the enhanced metafile
			// as it allows the sizing directly via pageRect. We have to set the mapping
			// mode for each page individually.
			VERIFY(setPrinterMappingMode(pd->hDC));
			::PlayMetaFile(pd->hDC, hmf);
#else
			::PlayEnhMetaFile(pd->hDC, hemf, pageRect);
#endif
			::EndPage(pd->hDC);
		}
*/
	}  // loop number of copies
}


void 
oofRepWinPrint::GetLogicalPageRect(LPRECT rect, HDC dc) const
{
	rect->top		= 0;
	rect->left		= 0;
	rect->right		= ::GetDeviceCaps(dc, HORZRES);
	rect->bottom	= ::GetDeviceCaps(dc, VERTRES);

	::DPtoLP(dc, (POINT*)rect, 2);
}

BOOL oofRepWinPrint::setPrinterMappingMode(CDC* pDC)
{
	pDC->SetMapMode(MM_ANISOTROPIC);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(printDlg));

	if (pApp->GetPrinterDeviceDefaults(&printDlg))
	{
		POINT paperSize;
		VERIFY(getPaperSize(printDlg.hDevNames, printDlg.hDevMode, &paperSize));

		CDC ic;
		VERIFY(ic.CreateIC("DISPLAY", NULL, NULL, NULL));

		ic.SetMapMode(MM_HIMETRIC);
		// paperSize is in tens of millimetres, but we need hundreds of millimetres
		POINT paperRect = {paperSize.x * 10, paperSize.y * 10};
		// convert paperRect into screen points
		ic.LPtoDP(&paperRect);
		// adjust the paper height
		paperRect.y *= -1;

		// set the transforming mapping mode
		pDC->SetWindowExt(paperRect.x, paperRect.y);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		return TRUE;
	}
	else
		return FALSE;
}

BOOL oofRepWinPrint::setPrinterMappingMode(HDC hDC)
{
	CDC* pDC = CDC::FromHandle(hDC);
	BOOL bRetVal = setPrinterMappingMode(pDC);
	pDC->Detach();

	return bRetVal;
}

BOOL oofRepWinPrint::getPaperSize(HANDLE hDevNames, HANDLE hDevMode, LPPOINT paperSize)
{
	LPDEVNAMES	lpDevNames = (LPDEVNAMES)::GlobalLock(hDevNames);
	LPDEVMODE	lpDevMode  = (LPDEVMODE)::GlobalLock(hDevMode);

	if (lpDevNames == NULL || lpDevMode == NULL)
		return FALSE;

	// get relationship between portrait and landscape orientations
	// (0, 90 or 270 degrees)
	DWORD dwOrientRel = ::DeviceCapabilities(
			(LPTSTR)lpDevNames + lpDevNames->wDeviceOffset,
			(LPTSTR)lpDevNames + lpDevNames->wOutputOffset,
			DC_ORIENTATION,
			NULL,
			lpDevMode);

	const bool hasAbsolutePaperDimension = (lpDevMode->dmFields & DM_PAPERLENGTH) != 0;
	const bool hasOrientation = (lpDevMode->dmFields & DM_ORIENTATION) != 0;

	BOOL retVal = TRUE;

	// if there is absolute paper dimension specified then use it as it overrides
	// the dmPaperSize field
	if (hasAbsolutePaperDimension) {
		paperSize->x = lpDevMode->dmPaperWidth;
		paperSize->y = lpDevMode->dmPaperLength;
	}
	else {
	// if the absolute paper dimension isn't specified, get the paperSize from
	// the dmPaperSize field of current DEVMODE structure
		retVal = getPaperSizeFromSymbolicValue(lpDevMode->dmPaperSize, paperSize);
	}

	// swap width and height if LANDSCAPE is specified
	if (retVal && hasOrientation &&
		lpDevMode->dmOrientation == DMORIENT_LANDSCAPE && dwOrientRel != 0)
	{
		LONG tmp = paperSize->x;
		paperSize->x = paperSize->y;
		paperSize->y = tmp;
	}

	::GlobalUnlock(hDevMode);
	::GlobalUnlock(hDevNames);

	return retVal;
}

BOOL oofRepWinPrint::getPaperSizeFromSymbolicValue(short paperSizeID, LPPOINT paperSize)
{
	static paperSizeLookup paperSizeArray[] = {
		DMPAPER_LETTER		/* Letter 8 1/2 x 11 in               */, INCHENTRY(8.5, 11),
		DMPAPER_LETTERSMALL	/* Letter Small 8 1/2 x 11 in         */, INCHENTRY(8.5, 11),
		DMPAPER_TABLOID		/* Tabloid 11 x 17 in                 */, INCHENTRY(11, 17),
		DMPAPER_LEDGER		/* Ledger 17 x 11 in                  */, INCHENTRY(17, 11),
		DMPAPER_LEGAL		/* Legal 8 1/2 x 14 in                */, INCHENTRY(8.5, 14),
		DMPAPER_STATEMENT	/* Statement 5 1/2 x 8 1/2 in         */, INCHENTRY(5.5, 8.5),
		DMPAPER_EXECUTIVE	/* Executive 7 1/4 x 10 1/2 in        */, INCHENTRY(7.25,10.5),
		DMPAPER_A3			/* A3 297 x 420 mm                    */, MMENTRY(297, 420),
		DMPAPER_A4			/* A4 210 x 297 mm                    */, MMENTRY(210, 297),
		DMPAPER_A4SMALL		/* A4 Small 210 x 297 mm              */, MMENTRY(210, 297),
		DMPAPER_A5			/* A5 148 x 210 mm                    */, MMENTRY(148, 210),
		DMPAPER_B4			/* B4 (JIS) 250 x 354                 */, MMENTRY(250, 354),
		DMPAPER_B5			/* B5 (JIS) 182 x 257 mm              */, MMENTRY(182, 257),
		DMPAPER_FOLIO		/* Folio 8 1/2 x 13 in                */, INCHENTRY(8.5, 13),
		DMPAPER_QUARTO		/* Quarto 215 x 275 mm                */, MMENTRY(215, 275),
		DMPAPER_10X14		/* 10x14 in                           */, INCHENTRY(10, 14),
		DMPAPER_11X17		/* 11x17 in                           */, INCHENTRY(11, 17),
		DMPAPER_NOTE		/* Note 8 1/2 x 11 in                 */, INCHENTRY(8.5, 11),
		DMPAPER_ENV_9		/* Envelope #9 3 7/8 x 8 7/8          */, INCHENTRY(3.875, 8.875),
		DMPAPER_ENV_10		/* Envelope #10 4 1/8 x 9 1/2         */, INCHENTRY(4.125, 9.5),
		DMPAPER_ENV_11		/* Envelope #11 4 1/2 x 10 3/8        */, INCHENTRY(4.5, 10.375),
		DMPAPER_ENV_12		/* Envelope #12 4 \276 x 11           */, INCHENTRY(4, 11),
		DMPAPER_ENV_14		/* Envelope #14 5 x 11 1/2            */, INCHENTRY(5, 11.5),
		DMPAPER_CSHEET		/* C size sheet                       */, INCHENTRY(0,0),
		DMPAPER_DSHEET		/* D size sheet                       */, INCHENTRY(0,0),
		DMPAPER_ESHEET		/* E size sheet                       */, INCHENTRY(0,0),
		DMPAPER_ENV_DL		/* Envelope DL 110 x 220mm            */, MMENTRY(110, 220),
		DMPAPER_ENV_C5		/* Envelope C5 162 x 229 mm           */, MMENTRY(162, 229),
		DMPAPER_ENV_C3		/* Envelope C3  324 x 458 mm          */, MMENTRY(324, 458),
		DMPAPER_ENV_C4		/* Envelope C4  229 x 324 mm          */, MMENTRY(229, 324),
		DMPAPER_ENV_C6		/* Envelope C6  114 x 162 mm          */, MMENTRY(114, 162),
		DMPAPER_ENV_C65		/* Envelope C65 114 x 229 mm          */, MMENTRY(114, 229),
		DMPAPER_ENV_B4		/* Envelope B4  250 x 353 mm          */, MMENTRY(250, 353),
		DMPAPER_ENV_B5		/* Envelope B5  176 x 250 mm          */, MMENTRY(176, 250),
		DMPAPER_ENV_B6		/* Envelope B6  176 x 125 mm          */, MMENTRY(176, 125),
		DMPAPER_ENV_ITALY	/* Envelope 110 x 230 mm              */, MMENTRY(110, 230),
		DMPAPER_ENV_MONARCH	/* Envelope Monarch 3.875 x 7.5 in    */, INCHENTRY(3.875, 7.5),
		DMPAPER_ENV_PERSONAL/* 6 3/4 Envelope 3 5/8 x 6 1/2 in    */, INCHENTRY(3.625, 6.5),
		DMPAPER_FANFOLD_US	/* US Std Fanfold 14 7/8 x 11 in      */, INCHENTRY(14.875, 11),
		DMPAPER_FANFOLD_STD_GERMAN/* German Std Fanfold 8 1/2 x 12 in   */, INCHENTRY(8.5, 12),
		DMPAPER_FANFOLD_LGL_GERMAN/* German Legal Fanfold 8 1/2 x 13 in */, INCHENTRY(8.5, 13)
#if (WINVER >= 0x0400)
		,  // comma to continue above
		DMPAPER_ISO_B4		/* B4 (ISO) 250 x 353 mm              */, MMENTRY(250, 353),
		DMPAPER_JAPANESE_POSTCARD/* Japanese Postcard 100 x 148 mm     */, MMENTRY(100, 148),
		DMPAPER_9X11		/* 9 x 11 in                          */, INCHENTRY(9, 11),
		DMPAPER_10X11		/* 10 x 11 in                         */, INCHENTRY(10, 11),
		DMPAPER_15X11		/* 15 x 11 in                         */, INCHENTRY(15, 11),
		DMPAPER_ENV_INVITE	/* Envelope Invite 220 x 220 mm       */, MMENTRY(220, 220),
		DMPAPER_LETTER_EXTRA/* Letter Extra 9 \275 x 12 in        */, INCHENTRY(9, 12),
		DMPAPER_LEGAL_EXTRA	/* Legal Extra 9 \275 x 15 in         */, INCHENTRY(9, 15),
		DMPAPER_TABLOID_EXTRA/* Tabloid Extra 11.69 x 18 in        */, INCHENTRY(11.69, 18),
		DMPAPER_A4_EXTRA	/* A4 Extra 9.27 x 12.69 in           */, INCHENTRY(9.27, 12.69),
		DMPAPER_LETTER_TRANSVERSE/* Letter Transverse 8 \275 x 11 in   */, INCHENTRY(8, 11),
		DMPAPER_A4_TRANSVERSE/* A4 Transverse 210 x 297 mm         */, MMENTRY(210, 297),
		DMPAPER_LETTER_EXTRA_TRANSVERSE/* Letter Extra Transverse 9\275 x 12 in*/, INCHENTRY(9, 12),
		DMPAPER_A_PLUS		/* SuperA/SuperA/A4 227 x 356 mm      */, MMENTRY(227, 356),
		DMPAPER_B_PLUS		/* SuperB/SuperB/A3 305 x 487 mm      */, MMENTRY(305, 487),
		DMPAPER_LETTER_PLUS	/* Letter Plus 8.5 x 12.69 in         */, INCHENTRY(8.5, 12.69),
		DMPAPER_A4_PLUS		/* A4 Plus 210 x 330 mm               */, MMENTRY(210, 330),
		DMPAPER_A5_TRANSVERSE/* A5 Transverse 148 x 210 mm         */, MMENTRY(148, 210),
		DMPAPER_B5_TRANSVERSE/* B5 (JIS) Transverse 182 x 257 mm   */, MMENTRY(182, 257),
		DMPAPER_A3_EXTRA	/* A3 Extra 322 x 445 mm              */, MMENTRY(322, 445),
		DMPAPER_A5_EXTRA	/* A5 Extra 174 x 235 mm              */, MMENTRY(174, 235),
		DMPAPER_B5_EXTRA	/* B5 (ISO) Extra 201 x 276 mm        */, MMENTRY(201, 276),
		DMPAPER_A2			/* A2 420 x 594 mm                    */, MMENTRY(420, 594),
		DMPAPER_A3_TRANSVERSE/* A3 Transverse 297 x 420 mm         */, MMENTRY(297, 420),
		DMPAPER_A3_EXTRA_TRANSVERSE/* A3 Extra Transverse 322 x 445 mm   */, MMENTRY(322, 445)
#endif /* WINVER >= 0x0400 */                                                   
	};

	const int lookupNumItems = sizeof(paperSizeArray) / sizeof(paperSizeLookup);

	for (int i=0; i<lookupNumItems; i++) {
		if (paperSizeArray[i].paperSizeID == paperSizeID) {
			paperSize->x = paperSizeArray[i].width;
			paperSize->y = paperSizeArray[i].height;
			return TRUE;
		}
	}

	return FALSE;
}
