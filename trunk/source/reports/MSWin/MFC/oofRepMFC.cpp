// oofRepWn.cpp : implementation file
//

#include "stdafx.h"
#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#ifndef H_OOFPRNWN
	#include "oofPrnWn.h"
#endif
#ifndef H_OOFREPMFC
	#include "oofRepMFC.h"
#endif
#include "oofRPRes.h"
#ifndef H_OOFREPHTML
	#include "oofrpHTM.h"
#endif
#ifndef H_OOFREPRTF
	#include "oofrpRTF.h"
#endif
#ifndef H_OOFREPCSV
	#include "oofrpCSV.h"
#endif
#ifndef H_OOFREPXML
	#include "oofrpXML.h" 
#endif H_OOFREPXML
#ifndef H_OOFREPTSV
	#include "oofrpTSV.h"
#endif
#ifndef H_OOFREPEDITX
	#include "oofrepeditx.h"
#endif

// reloading reports is optional feature
#ifdef OOF_READ_REPORTS
	#ifndef H_OOFRIXML
		#include "oofriXML.h"
	#endif H_OOFREPXML
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -------------------------------------------------------
//        o o f A f x C r e a t e R e p o r t V i e w
// -------------------------------------------------------

CFrameWnd* __stdcall oofAfxCreateReportView(CRuntimeClass* pFrameClass, UINT nId,
											oofRep* pReport, oofRep* pRepLandscape,
											BOOL bMakeMain)
{
	CFrameWnd* pFrame = (CFrameWnd*)pFrameClass->CreateObject();
	
	if (!pFrame || !pFrame->LoadFrame(nId, WS_OVERLAPPEDWINDOW, AfxGetApp()->m_pMainWnd))
		return NULL;

	if (bMakeMain)
		// Required for SDI applications.
		// Do NOT use for MDI apps. Main window should have already been created & set
		// by calling routine.
		AfxGetThread()->m_pMainWnd = pFrame;

	oofRepWindow* pView = oofRepWindow::CreateReportWindow(
											pReport,
											pRepLandscape,
											pFrame, AFX_IDW_PANE_FIRST);
	
	pFrame->SetActiveView(pView);
	pFrame->RecalcLayout();
	pFrame->InitialUpdateFrame(NULL, TRUE); 
	return pFrame;
}

BOOL __stdcall oofAfxDoModalPreview(UINT nId, oofRep* pReport, oofRep* pRepLandscape, BOOL bMakeMain)
{
	CFrameWnd* pFrame =
		oofAfxCreateReportView(RUNTIME_CLASS(oofRepModalPreviewFrame), nId, pReport, pRepLandscape, bMakeMain);

	if (pFrame != NULL) {
		// disable the application window
		::EnableWindow(AfxGetApp()->m_pMainWnd->m_hWnd, FALSE);
		// get, translate and dispatch messages until break out with EndModalLoop()
		pFrame->RunModalLoop(MLF_SHOWONIDLE);
		// destroy the frame
		pFrame->DestroyWindow();
		// activate the main application's window as it may go out of scope on exit from
		// the modal frame if there was a modal dialog displayed over it
		AfxGetMainWnd()->BringWindowToTop();

		return TRUE;
	}

	return FALSE;
}

BOOL __stdcall oofAfxPrintReport(oofRep* inRep, oofRep* inRepLandscape, HWND hwndPrtDlgOwner)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(printDlg));

	if (pApp->GetPrinterDeviceDefaults(&printDlg))
	{
		CPageSetupDialog pageSetupDlg;

		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printDlg.hDevMode);
		ASSERT(lpDevMode->dmFields & DM_PAPERSIZE);
		// store current values
		short paperWidth  = lpDevMode->dmPaperWidth;
		short paperLength = lpDevMode->dmPaperLength;
		// convert paper size into hundredths of milimeters (DEVMODE stores it in tenths
		// of millimeters)
		pageSetupDlg.m_psd.ptPaperSize.x = paperWidth * 10;
		pageSetupDlg.m_psd.ptPaperSize.y = paperLength * 10;
		pageSetupDlg.m_psd.Flags         = PSD_INHUNDREDTHSOFMILLIMETERS;

		::GlobalUnlock(printDlg.hDevMode);

		// get the current paper orientation and the active report (depends on the
		// paper orientation)
		const BOOL isLandscapeMode = oofRepWindow::IsLandscapeMode();
		oofRep* activeReport =
			inRepLandscape != NULL && isLandscapeMode ? inRepLandscape : inRep;
		// generate the report with the current settings (we need the number
		// of pages to initialize the print dialog)

		// update paper size according to the orientation
		POINT paperSize;
		VERIFY(oofRepWinPrint::getPaperSize(printDlg.hDevNames, printDlg.hDevMode, &paperSize));
		// set the new paper dimension
		pageSetupDlg.m_psd.ptPaperSize.x = paperSize.x * 10;
		pageSetupDlg.m_psd.ptPaperSize.y = paperSize.y * 10;

		oofRepWinPrint* winPrint = new oofRepWinPrint(NULL, &pageSetupDlg.m_psd, &printDlg);
		activeReport->draw(*winPrint);
		winPrint->finish();

		printDlg.lStructSize	= sizeof(PRINTDLG);
		printDlg.hwndOwner		= hwndPrtDlgOwner;
		printDlg.Flags			= PD_NOSELECTION | PD_PAGENUMS | PD_RETURNDC;
		printDlg.nCopies		= 1;
		printDlg.nFromPage		= 1;
		printDlg.nMinPage		= printDlg.nFromPage;
		printDlg.nToPage		= (unsigned short)winPrint->getNumPages();
		printDlg.nMaxPage		= printDlg.nToPage;

		if (::PrintDlg(&printDlg)) {
			// store the new printer information, but don't try to remove the old one
			// as it has been deleted in PrintDlg
			pApp->SelectPrinter(printDlg.hDevNames, printDlg.hDevMode, FALSE);

			// update paper size according to the orientation
			POINT paperSize;
			VERIFY(oofRepWinPrint::getPaperSize(printDlg.hDevNames, printDlg.hDevMode, &paperSize));

			// if either paper size or its orienation has changed, we have to generate
			// the report again with actual values
			if (paperWidth != paperSize.x || paperLength != paperSize.y ||
				isLandscapeMode != oofRepWindow::IsLandscapeMode())
			{
				// don't print it on deletion
				winPrint->disablePrinting();
				// get rid of old one
				delete winPrint;
				// set the new paper dimension
				pageSetupDlg.m_psd.ptPaperSize.x = paperSize.x * 10;
				pageSetupDlg.m_psd.ptPaperSize.y = paperSize.y * 10;
				// set the active report again (the paper orientation may have changed)
				activeReport =
					inRepLandscape != NULL && oofRepWindow::IsLandscapeMode() ? inRepLandscape : inRep;

				// create and print the new report (don't ask user now)
				activeReport->draw(oofRepWinPrint(NULL, &pageSetupDlg.m_psd, &printDlg));
			}
			else {
				// delete the report (will print in destructor)
				delete winPrint;
			}
		}
		else {  // cleanup if cancel print
			// don't print it on deletion
			winPrint->disablePrinting();
			// get rid of old one
			delete winPrint;

		}  // ask to print

		return TRUE;
	}

	return FALSE;
}

BOOL __stdcall oofAfxDoPageSetup()
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(printDlg));

	if (pApp->GetPrinterDeviceDefaults(&printDlg))
	{
		CPageSetupDialog pageSetupDlg(PSD_DEFAULTMINMARGINS | PSD_DISABLEMARGINS);

		pageSetupDlg.m_psd.hDevNames = printDlg.hDevNames;
		pageSetupDlg.m_psd.hDevMode  = printDlg.hDevMode;

		if (pageSetupDlg.DoModal() == IDOK)
			pApp->SelectPrinter(pageSetupDlg.m_psd.hDevNames, pageSetupDlg.m_psd.hDevMode, FALSE);

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// oofRepWindow

// string description array of zoom types (1-based)
char* oofRepWindow::sZoomType[] = {
	"",
	"Fit Page",
	"Fit Width"
};

IMPLEMENT_DYNCREATE(oofRepWindow, CScrollView)

BEGIN_MESSAGE_MAP(oofRepWindow, CScrollView)
	//{{AFX_MSG_MAP(oofRepWindow)
	ON_COMMAND(ID_REPORT_FIRST, OnReportFirst)
	ON_COMMAND(ID_REPORT_LAST, OnReportLast)
	ON_COMMAND(ID_REPORT_NEXT, OnReportNext)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_REPORT_NEXT, OnUpdateReportNext)
	ON_COMMAND(ID_REPORT_PREV, OnReportPrevious)
	ON_UPDATE_COMMAND_UI(ID_REPORT_PREV, OnUpdateReportPrevious)
	ON_UPDATE_COMMAND_UI(ID_REPORT_BW, OnUpdateReportBW)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
	ON_COMMAND(ID_REPORT_CLOSE, OnFileClose)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_SAVE_AS, OnReportSave)
	ON_COMMAND(ID_REPORT_SAVEAS, OnReportSave)
	ON_COMMAND(ID_REPORT_BW, OnReportBW)
	ON_COMMAND(ID_REPORT_ZOOM, OnReportZoom)
	ON_MESSAGE(ID_REPORT_REFRESH, OnRefresh)
	ON_WM_SIZE()
#ifdef OOF_READ_REPORTS
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
#endif
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	// Standard printing commands
	ON_COMMAND(ID_REPORT_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateFileExit)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

oofRepWindow::oofRepWindow() :
	mEnv(0),
	mCachedScaleRatio(0),
	mButtonWasDown(false)
{
	mBlackAndWhitePrinting = oofRep::defaultSettings()->printGraphsInMono();
	mScaleFactor = -ZoomType_FitWidth;
	mRep		= NULL;
	mRepLandscape = NULL;
	mPageSetup	= NULL;
	mCurrentPageIndex = 0;
#ifdef OOF_READ_REPORTS
	mOpenedReportDB=0;
#endif	

	PAGESETUPDLG	ps;
	oofRepWinGUIPreviewMFC::initDefaultPageSetup(&ps);
	mEnv = new oofRepWinGUIPreviewMFC(&ps);
}

oofRepWindow::~oofRepWindow()
{
	mRep->defaultSettings()->printGraphsInMono(mBlackAndWhitePrinting);  // update global state
	if (mRepLandscape)
		mRepLandscape->defaultSettings()->printGraphsInMono(mBlackAndWhitePrinting);

	delete mEnv;
	delete mRep;
	delete mRepLandscape;
#ifdef OOF_READ_REPORTS
	delete mOpenedReportDB; 
#endif	
}

//	Common Factory
oofRepWindow*
oofRepWindow::CreateReportWindow(oofRep* adoptedReport, oofRep* inRepLandscapePtr, CWnd* pParent,
								 UINT nId, CPageSetupDialog* pageSetup)
{
	ASSERT(adoptedReport != NULL);

	oofRepWindow* displayWindow = (oofRepWindow*)RUNTIME_CLASS(oofRepWindow)->CreateObject();
	// views are always created with a border!
	if (!displayWindow->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
								CRect(0,0,0,0), pParent, nId, NULL))
	{
		TRACE0("Warning: could not create view for frame.\n");
		return NULL;        // can't continue without a view
	}

	displayWindow->CreatePreviewContent(adoptedReport, inRepLandscapePtr, pageSetup);
	
	// We should now have some pages !
	displayWindow->ShowWindow(SW_SHOW);
	displayWindow->OnInitialUpdate();
	return displayWindow;
}


void
oofRepWindow::CreatePreviewContent(oofRep* adoptedReport, oofRep* inRepLandscapePtr, CPageSetupDialog* pageSetup)
{
	ASSERT(adoptedReport != NULL);

	oofRepWindow* displayWindow = this;  // to easily copy code from static factory method

// convert main report from multi with single content to a base oofRep, if necessary
	if (adoptedReport->isMultiReport()) {  // see if one section and mutate
		oofRepMulti* asMulti = (oofRepMulti*)adoptedReport;  // safe downcast
		if (asMulti->count()==1) {  // if multi report contains just one report, mutate into single report
			oofRepSection* onlySection = asMulti->section(0);
			oofRep* newSingle = new oofRep(*onlySection);  // copy section to plain base oofRep
			delete adoptedReport;
			adoptedReport = newSingle;
		}  // only one section in multi
	} // is multi

// convert optional landscape report from multi with single content to a base oofRep, if necessary
	if (inRepLandscapePtr && inRepLandscapePtr->isMultiReport()) {  // see if one section and mutate
		oofRepMulti* asMulti = (oofRepMulti*)inRepLandscapePtr;  // safe downcast
		if (asMulti->count()==1) {  // if multi report contains just one report, mutate into single report
			oofRepSection* onlySection = asMulti->section(0);
			oofRep* newSingle = new oofRep(*onlySection);  // copy section to plain base oofRep
			delete inRepLandscapePtr;
			inRepLandscapePtr = newSingle;
		}  // only one section in multi
	} // is multi
	
	oofRep* actRep = inRepLandscapePtr != NULL && IsLandscapeMode() ? inRepLandscapePtr : adoptedReport;;

	// this flag's state will affect the BW button's state too
	displayWindow->blackAndWhitePrinting(actRep->settings()->printGraphsInMono());

	oofRepEditor* possibleEditor = adoptedReport->settings()->reportEditor();
	if (possibleEditor) {
		oofRepEditingEnv* theEditEnv = possibleEditor->makeEditingEnvironment();;
		delete displayWindow->mEnv;  // reset the env but needed before this for some calcs
		displayWindow->mEnv = theEditEnv;
		theEditEnv->parentWindow(this);
	}
			
	displayWindow->setReport(adoptedReport);
	displayWindow->setLandscapeReport(inRepLandscapePtr);

	if (pageSetup)
		displayWindow->setPageSetup(pageSetup);

	displayWindow->start();

	actRep->draw(*displayWindow->mEnv);
	displayWindow->finish();
}


BOOL
oofRepWindow::IsLandscapeMode()
{
	BOOL bRetVal = FALSE;

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(printDlg));

	if (AfxGetApp()->GetPrinterDeviceDefaults(&printDlg)) {
		// get the current printer description records
		LPDEVNAMES	lpDevNames = (LPDEVNAMES)::GlobalLock(printDlg.hDevNames);
		LPDEVMODE	lpDevMode  = (LPDEVMODE)::GlobalLock(printDlg.hDevMode);

		ASSERT(lpDevNames != NULL);
		ASSERT(lpDevMode != NULL);

		bRetVal = ((lpDevMode->dmFields & DM_ORIENTATION) != 0 &&
					lpDevMode->dmOrientation == DMORIENT_LANDSCAPE);

		::GlobalUnlock(printDlg.hDevMode);
		::GlobalUnlock(printDlg.hDevNames);
	}

	return bRetVal;
}

void oofRepWindow::start()
{
	mEnv->start(); 
}

CString oofRepWindow::GetFileName(LPCTSTR lpszDefFileExt, LPCTSTR lpszFilter)
{
	CFileDialog openFileDlg(
		FALSE,			// file save dialog
		lpszDefFileExt,	// default extension
		"untitled",		// default file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		lpszFilter,		// file filter
		this			// parent window
	);

	if (openFileDlg.DoModal() == IDOK)
		return openFileDlg.GetPathName();
	else
		return CString();
}

void oofRepWindow::ScalePage(int scaleFactor)
{
	mScaleFactor = scaleFactor;
	AdjustScrollSize();
	Invalidate();
}

CSize oofRepWindow::GetTransformedPageSize(const CRect& pageRect)
{
	int scaleFactor = mScaleFactor;

	if (ScalingRelativeToPage())
	{
		CRect pageRect(mEnv->pageRectPts());

		CSize clientSize;
		CSize sizeSB;
		// undocumented, but quite usefull function which gets the client size
		// without scrollbars
		GetTrueClientSize(clientSize, sizeSB);

		const int hScaleFactor = 100 * clientSize.cx / pageRect.Width();
		const int vScaleFactor = 100 * clientSize.cy / pageRect.Height();

		switch (abs(scaleFactor)) {
		case ZoomType_FitPage:
			scaleFactor = min(hScaleFactor, vScaleFactor);
			break;
		case ZoomType_FitWidth:
			// if there's a vertical scrollbar its width will have to be counted with
			if (hScaleFactor > vScaleFactor)
				scaleFactor = 100 * (clientSize.cx - sizeSB.cx) / pageRect.Width();
			else
				scaleFactor = hScaleFactor;
			break;
		default:
			ASSERT(!"Unsupported scale factor!");
		}
	}
	mCachedScaleRatio = scaleFactor;  // so can quickly reverse to scale clicks
	return CSize(pageRect.Width() * scaleFactor / 100, pageRect.Height() * scaleFactor / 100);
}

void oofRepWindow::OnSize(UINT nType, int cx, int cy) 
{
	// update the scrollbars when sizing this window with either of 'Fit...'
	// scale factors
	if (ScalingRelativeToPage())
		AdjustScrollSize();

	CScrollView::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// oofRepWindow drawing

void oofRepWindow::OnDraw(CDC* pDC)
{
	const unsigned long numPicts = mEnv->pages().count();
	if (numPicts>0) {
		oofRect rcBounds(CPoint(0, 0), GetTransformedPageSize(mEnv->pageRectPts()));
		mEnv->drawPage(pDC->m_hDC, mCurrentPageIndex, rcBounds);
	}  // endif some pages to draw
}


/////////////////////////////////////////////////////////////////////////////
// oofRepWindow interaction

/**
	Click reaction.
	trap the down event as well because testing has shown that only having the Up event
	gets spurious calls after using the mouse in a file Open dialog.
*/
void oofRepWindow::OnLButtonDown(UINT /*nFlags*/, CPoint)
{
	mButtonWasDown = true;
}


/**
	Click reaction.
	on Mac this is implemented using an attachment as users tend more to subclass the
	actual window and there's a drawing pane within the window, so we wanted a method
	that would work in their windows as well. MFC lacks attachments so we have a direct
	method on the window itself.
*/
void oofRepWindow::OnLButtonUp(UINT /*nFlags*/, CPoint pt)
{
	if(!mButtonWasDown)
		return;
	
	oofRepEditor* possibleEditor = mRep->settings()->reportEditor();
	if (!possibleEditor) 
		return;
		
	mButtonWasDown = false;
	oofRepEditingEnv* theEditEnv = (oofRepEditingEnv*)mEnv;  // safe downcast
	// may need to be pt.h, pt.v on older MFC ?
	int x = pt.x + GetScrollPos(SB_HORZ);
	int y = pt.y + GetScrollPos(SB_VERT);
	if (mScaleFactor!=100) {
		assert(mCachedScaleRatio!=0);  // have been through GetTransformedPageSize
		x = (x * 100) / mCachedScaleRatio;
		y = (y * 100) / mCachedScaleRatio;
	}
	// hack - ratio determined by experiment
	CDC* theDC = GetDC();
	assert(theDC);
	HDC hdc = theDC->GetSafeHdc();
	assert(hdc);
	const int horzres = ::GetDeviceCaps(hdc, HORZRES);
	assert(horzres>0);

	if (oofRuntimeEnv::runtimeEnv()==oofRuntimeEnv::WinNT) {
	// following divisor worked out from screen diagonal in inches
	// and testing
		const double currentScreenDPI = (double) horzres * 254 / 3200; //15.748031496;
		const double scalingFactor = 96.0 / currentScreenDPI;
		x *= scalingFactor;
		y *= scalingFactor;
	}

	theEditEnv->singleClick(x, y);
}


/////////////////////////////////////////////////////////////////////////////
// oofRepWindow diagnostics

#ifdef _DEBUG
void oofRepWindow::AssertValid() const
{
	CScrollView::AssertValid();
}

void oofRepWindow::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// oofRepWindow message handlers

void oofRepWindow::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	// Calculate the total size of this view
	AdjustScrollSize();
}

void oofRepWindow::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	if (pDC->IsPrinting()) {
		// Set the mapping mode that will convert the report's dimension from screen
		// into the printer resolution. 
		VERIFY(oofRepWinPrint::setPrinterMappingMode(pDC));
	}
	else {
		// call the base class first to think this is MM_TEXT (CScrollView doesn't
		// support MM_ISOTROPIC and MM_ANISOTROPIC modes directly)
		CScrollView::OnPrepareDC(pDC, pInfo);
		// set up the device context
		pDC->SetMapMode(MM_ANISOTROPIC);

		CRect pageRect(mEnv->pageRectPts());

		pageRect.right = pts2logical(pageRect.right, pDC->m_hDC);
		pageRect.bottom = pts2logical(pageRect.bottom, pDC->m_hDC);

		pDC->SetWindowExt(pageRect.Size());
		pDC->SetViewportExt(GetTransformedPageSize(pageRect));
	}
}

BOOL oofRepWindow::OnPreparePrinting(CPrintInfo* pInfo) 
{
	BOOL bRetVal;

	ASSERT(mRep != NULL);

	if (!pInfo->m_bPreview) {  // NOT USING MFC PREVIEW
		activeReport()->defaultSettings()->printGraphsInMono(mBlackAndWhitePrinting);	// a way to make the default Print buttons print colour
		if (activeReport()->settings()->printGraphsInMono()!=mBlackAndWhitePrinting)
			activeReport()->getLocalSettings()->printGraphsInMono(mBlackAndWhitePrinting);	// change local setting if the above is overriden
		oofAfxPrintReport(mRep, mRepLandscape, GetSafeHwnd());
		// skip the MFC printing loops
		bRetVal = FALSE;
	}
	else {
		pInfo->SetMaxPage(mEnv->getNumPages());

	/*	CWinApp* pApp = AfxGetApp();
		ASSERT(pApp != NULL);

		CWnd* pMainWnd = pApp->m_pMainWnd;
		pApp->m_pMainWnd = GetParent();
	*/
		bRetVal = DoPreparePrinting(pInfo);

	//	pApp->m_pMainWnd = pMainWnd;
	}

	return bRetVal;
}

void oofRepWindow::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// save the current page
	mSavePage	= mCurrentPageIndex;
	CScrollView::OnBeginPrinting(pDC, pInfo);
}

void oofRepWindow::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// Restore the save page number
	mCurrentPageIndex = mSavePage;
	CScrollView::OnEndPrinting(pDC, pInfo);
}

void oofRepWindow::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	const unsigned long pageIndex = pInfo->m_nCurPage - 1; 
	const oofRect rcBounds = pInfo->m_rectDraw;
	
	mEnv->drawPage(pDC->m_hDC, pageIndex, rcBounds);

	activeReport()->defaultSettings()->printGraphsInMono(false);	// a way to make the default Print buttons print colour
	if (activeReport()->settings()->printGraphsInMono())	
		activeReport()->getLocalSettings()->printGraphsInMono(false);	// change local setting if the above is overriden
}


void oofRepWindow::OnReportFirst() 
{
	mCurrentPageIndex = 0;
	Invalidate();
}

void oofRepWindow::OnReportLast() 
{
	mCurrentPageIndex = mEnv->getNumPages()-1;
	Invalidate();
}

void oofRepWindow::OnReportNext() 
{
	if (mCurrentPageIndex == mEnv->getNumPages()-1)
		::MessageBeep(0);
	else {
		mCurrentPageIndex++;
		Invalidate();
	}
}

void oofRepWindow::OnEditCopy()
{
#if DISABLED_FOR_NOW_981013
	if (OpenClipboard()) {
		if (::EmptyClipboard()) {

#ifdef OOFILE_USE_OLDMETAFILE
			HGLOBAL			hGMem;
			LPMETAFILEPICT	pMFP;

			// allocate and lock the METAFILEPICT structure in the global memory pool,
			// as its handle is going to be inserted into the clipboard
			hGMem = ::GlobalAlloc(GHND, sizeof(METAFILEPICT));
			pMFP  = (LPMETAFILEPICT)::GlobalLock(hGMem);

			pMFP->mm   = MM_TEXT;
			pMFP->xExt = mEnv->pageRectPts()->width();
			pMFP->yExt = mEnv->pageRectPts()->height();
			pMFP->hMF  = ::SetMetaFileBitsEx(mPICTs->PICT.length(), (LPBYTE)mPICTs->PICT.bodyAddress());

			::GlobalUnlock(hGMem);

			VERIFY(::SetClipboardData(CF_METAFILEPICT, hGMem));
#else
			HENHMETAFILE hemf;

			hemf = ::SetEnhMetaFileBits(mPICTs->PICT.length(), (LPBYTE)mPICTs->PICT.bodyAddress());

			ENHMETAHEADER hMetaHeader;
			UINT nRetVal = ::GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &hMetaHeader);

			VERIFY(::SetClipboardData(CF_ENHMETAFILE, hemf));
#endif

		}

		VERIFY(::CloseClipboard());
	}
	#endif  // DISABLED_FOR_NOW_981013
}

void oofRepWindow::OnUpdateReportNext(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(!mPICTs->atLast());
}

void oofRepWindow::OnReportPrevious() 
{
	if (mCurrentPageIndex==0)
		::MessageBeep(0);
	else {
		mCurrentPageIndex--;
		Invalidate();
	}
}

void oofRepWindow::OnUpdateReportPrevious(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(!mPICTs->atFirst());
}


afx_msg LRESULT 
oofRepWindow::OnRefresh(WPARAM, LPARAM)
{
	ReflowReport();
	return 1;  // worked
}


void
oofRepWindow::AdjustScrollSize()
{
// not necessary now that we are scaling in render mode	CSize scrollSize = GetTransformedPageSize(mEnv->pageRectPts());
/*
	CSize scrollSize(ptsRect.width(), ptsRect.height());

*/
	oofRect ptsRect = mEnv->pageRectPts();
	CDC* pDC = GetDC();
	HDC dc = pDC->m_hDC;
	
	// now compensate back from pts to device coords
	const int pixelsX = ::GetDeviceCaps(dc, LOGPIXELSX);
	ptsRect.right = ::MulDiv(ptsRect.right, 72, pixelsX);
	
	const int pixelsY = ::GetDeviceCaps(dc, LOGPIXELSY);
// HACK - dunnno why but this is too much, so don't scale down
// and not all report visible	ptsRect.bottom = ::MulDiv(ptsRect.bottom, 72, pixelsY);

	CSize scrollSize = GetTransformedPageSize(ptsRect);
	SetScrollSizes(MM_TEXT, scrollSize);
	ReleaseDC(pDC);			// release device context
}


void oofRepWindow::ReflowReport() 
{
	// important to delete cached pages as they were wrapped to different size
	// and we are re-creating them with the draw below - if not deleted then they 
	// will show up as both old and new pages in the preview!
	int nPage = mCurrentPageIndex;
	const int oldNumPages = mEnv->pages().count();

	mEnv->pages().deleteAll();

	start();
	activeReport()->draw(*mEnv);
	finish();
	// Go to the saved page, adjusted relative to the new range eg: if go Landscape to Portrait
	const int newNumPages = mEnv->pages().count();
	if (newNumPages!=oldNumPages) {
		nPage = nPage * newNumPages/oldNumPages;  // eg: 4/7 becomes 3/5
		if (nPage > newNumPages)
			nPage = newNumPages;
	} // adjust curr page because numPages changed
	mCurrentPageIndex = nPage;

	// Calculate the new size of this view
	AdjustScrollSize();
	// repaint the window
	Invalidate();
}


void 
oofRepWindow::OnFilePageSetup() 
{
	CPageSetupDialog ps(PSD_DEFAULTMINMARGINS | PSD_DISABLEMARGINS, this);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(printDlg));

	if (pApp->GetPrinterDeviceDefaults(&printDlg)) {
		// setup the default printer information for the page setup dialog
		ps.m_psd.hDevNames = printDlg.hDevNames;
		ps.m_psd.hDevMode  = printDlg.hDevMode;
//		ps.m_psd.rtMargin  = *mEnv->getMargin();

		if (ps.DoModal() == IDOK) {
			// update printer information, but do not delete the previous one
			pApp->SelectPrinter(ps.m_psd.hDevNames, ps.m_psd.hDevMode, FALSE);
	
			setPageSetup(&ps);

			// Redo the report for the new size with respect to the orientation
			ReflowReport();
		}
	}
	else
		AfxMessageBox("There is no default printer selected.", MB_OK | MB_ICONEXCLAMATION);
}


void oofRepWindow::OnFileClose() 
{
	GetParent()->SendMessage(WM_CLOSE);
}


void oofRepWindow::OnUpdateFileExit(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(false);
}
  
  
#ifdef OOF_READ_REPORTS
void 
oofRepWindow::OnFileOpen() 
{
// select and parse an XML file
	oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	const oofString saveExt = XMLsettings->saveReportDocFileExt();
	CFileDialog theDlg(
		TRUE  /* open file*/, 
		saveExt, 
		NULL, /* no default name */
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, 
		"All Files (*.*) |*.*||", 
		this /* parentWnd */
	);
	if (theDlg.DoModal()!=IDOK) 
		return;  // EARLY EXIT - cancelled open
		
	const oofString thePathName=theDlg.GetPathName();
	if (thePathName.isEmpty()) 
		return;  // EARLY EXIT - entered null filename
		
// have got a file to try opening!		
	delete mOpenedReportDB;
	mOpenedReportDB = XMLsettings->makeTempConnection();
	
	OOF_XMLrepParser theParser;
	if (theParser.parseFile( oofFileRef(thePathName) ) ) {
		delete mRep;
		delete mRepLandscape;  // NOT YET IMPLEMENTED - read landscape rep from file as well
		mRep = theParser.orphanReport();
		mRepLandscape = 0;
		ReflowReport();
	}
}
#endif
  
  
void 
oofRepWindow::OnReportSave() 
{
	oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	const oofString saveExt = XMLsettings->saveReportDocFileExt();
	
	oofString getFileDescs(saveExt, XMLsettings->saveReportDocDescription(), " (*.");
	getFileDescs += saveExt;
	getFileDescs += ")|*.";
	getFileDescs += saveExt;
	getFileDescs +=  "RTF Files (*.rtf)|*.rtf|HTML Files (*.htm;*.html)|*.htm;*.html|CSV Files (*.csv)|*.csv|Tabbed Text Files (*.txt)|*.txt||";
	oofString fileName = GetFileName(saveExt, getFileDescs);

	if (!fileName.isEmpty()) {

		if( fileName.endsWith(saveExt ) )  {// XML -  allow for user having subclassed the environment
			oofRepXMLEnv* theXMLenv = XMLsettings->makeXMLenv(fileName);  // use factory so could make user subclass
			activeReport()->draw(*theXMLenv);
			delete theXMLenv;
		}
		else if( fileName.endsWith( ".htm" )) // HTM
			activeReport()->draw(oofRepHTMLEnv(fileName));
		else if( fileName.endsWith( ".html" )  ) // HTML
			activeReport()->draw(oofRepHTMLEnv(fileName));
		else if( fileName.endsWith( ".csv" ) ) // CSV
			activeReport()->draw(oofRepCSVEnv(fileName));
		else if( fileName.endsWith( ".txt" ) ) // TSV
			activeReport()->draw(oofRepTSVEnv(fileName));
		else
			activeReport()->draw(oofRepRTFEnv(fileName)); // RTF
	}
}

void oofRepWindow::OnReportBW()
{
	mBlackAndWhitePrinting = !mBlackAndWhitePrinting;
}

void oofRepWindow::OnUpdateReportBW(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(mBlackAndWhitePrinting);
}

void oofRepWindow::OnReportZoom()
{
	oofZoomDlg zoomDlg(mScaleFactor);

	int newScaleFactor;
	CString& val = zoomDlg.mComboVal;

	if (zoomDlg.DoModal() == IDOK && !val.IsEmpty()) {
		if (val[0] >= '0' && val[0] <= '9') {
			if (::sscanf(val, "%d", &newScaleFactor) != EOF && newScaleFactor > 10 && newScaleFactor <= 800)
				ScalePage(newScaleFactor);
			else
				::MessageBeep(0);
		}
		else {
			if (val.Compare(sZoomType[ZoomType_FitPage]) == 0)
				ScalePage(-ZoomType_FitPage);
			else if (val.Compare(sZoomType[ZoomType_FitWidth]) == 0)
				ScalePage(-ZoomType_FitWidth);
			else
				::MessageBeep(0);
		}
	}
}

void oofRepWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case VK_INSERT:
		if (::GetKeyState(VK_CONTROL) < 0) {
			OnEditCopy();
		}
		break;
	default:
		break;
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// oofRepPreviewFrame

IMPLEMENT_DYNCREATE(oofRepPreviewFrame, CFrameWnd)

oofRepPreviewFrame::oofRepPreviewFrame()
{
}

oofRepPreviewFrame::~oofRepPreviewFrame()
{
}


BEGIN_MESSAGE_MAP(oofRepPreviewFrame, CFrameWnd)
	//{{AFX_MSG_MAP(oofRepPreviewFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// oofRepPreviewFrame overrides

void oofRepPreviewFrame::ActivateFrame(int nCmdShow)
{
	if (nCmdShow == -1)	{
		if (!IsWindowVisible())
			nCmdShow = SW_SHOWMAXIMIZED;  // report-writer preview defaults to maximized
	}
	CFrameWnd::ActivateFrame(nCmdShow);
}


/////////////////////////////////////////////////////////////////////////////
// oofRepPreviewFrame message handlers

int oofRepPreviewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_PREVIEW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

#ifdef OOF_BW_CHECKBOX
	// width=170, height=30 - TODO, calculate the proper sizes...
	m_wndToolBar.SetButtonInfo(5, ID_REPORT_BW, TBBS_SEPARATOR, 170);
	CRect rect;
	m_wndToolBar.GetItemRect(5, &rect);
	rect.bottom=rect.top+30;
	m_BWCheckBox.Create("Print in Black && White", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, &m_wndToolBar, ID_REPORT_BW);
#endif

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	const bool usingWin32s = oofRuntimeEnv::runtimeEnv()==oofRuntimeEnv::Win32s;
	if (usingWin32s) {
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_TOOLTIPS  | CBRS_SIZE_DYNAMIC);
	}
	else {
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_BOTTOM);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// oofRepModalPreviewFrame

IMPLEMENT_DYNCREATE(oofRepModalPreviewFrame, oofRepPreviewFrame)

oofRepModalPreviewFrame::oofRepModalPreviewFrame()
{
}

oofRepModalPreviewFrame::~oofRepModalPreviewFrame()
{
}


BEGIN_MESSAGE_MAP(oofRepModalPreviewFrame, oofRepPreviewFrame)
	//{{AFX_MSG_MAP(oofRepModalPreviewFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// oofRepModalPreviewFrame message handlers

void oofRepModalPreviewFrame::OnClose() 
{
   // end the modal loop
   EndModalLoop(TRUE);
   // enable the main application window or else another application can be activated
   ::EnableWindow(AfxGetApp()->m_pMainWnd->m_hWnd, TRUE);
   // hide this window (don't destroy it)
	ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// oofZoomDlg dialog used for App About

oofZoomDlg::oofZoomDlg(int scaleFactor) : CDialog(oofZoomDlg::IDD)
{
	//{{AFX_DATA_INIT(oofZoomDlg)
	//}}AFX_DATA_INIT

	mScaleFactor = scaleFactor;
}

void oofZoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(oofZoomDlg)
	DDX_Control(pDX, IDC_OOF_COMBO_ZOOM, mComboCtrl);
	DDX_CBString(pDX, IDC_OOF_COMBO_ZOOM, mComboVal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(oofZoomDlg, CDialog)
	//{{AFX_MSG_MAP(oofZoomDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL oofZoomDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	static char buf[20];
	char* comboText;

	for(int i = 50; i <= 150; i+=25) {
		sprintf(buf, "%d", i);
		mComboCtrl.AddString(buf);
	}

	mComboCtrl.AddString("200");
	mComboCtrl.AddString("400");
	mComboCtrl.AddString("800");
	mComboCtrl.AddString(oofRepWindow::sZoomType[oofRepWindow::ZoomType_FitPage]);
	mComboCtrl.AddString(oofRepWindow::sZoomType[oofRepWindow::ZoomType_FitWidth]);

	if (mScaleFactor > 0) {
		sprintf(buf, "%d", mScaleFactor);
		comboText = buf;
	} else {  // ScalingRelativeToPage
		switch(abs(mScaleFactor)) {
		case oofRepWindow::ZoomType_FitPage:
			comboText = oofRepWindow::sZoomType[oofRepWindow::ZoomType_FitPage];
			break;
		case oofRepWindow::ZoomType_FitWidth:
			comboText = oofRepWindow::sZoomType[oofRepWindow::ZoomType_FitWidth];
			break;
		default:
			ASSERT(!"Unsupported scale factor!");
		}
	}

	mComboCtrl.SetWindowText(comboText);
	mComboCtrl.SetFocus();

	return FALSE;
}

#endif  // !_Macintosh for doxygen
