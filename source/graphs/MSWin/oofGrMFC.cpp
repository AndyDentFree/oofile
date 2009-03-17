// COPYRIGHT 1997 A.D. Software, All rights reserved
//
/**
	\file oofGrMFC.cpp implementation file for graph preview window if using MFC.
*/


#include "stdafx.h"
#include "oofGrMFC.h"
#include "oofDraw.h"
#include "oofRpRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -------------------------------------------------------
//        o o f A f x C r e a t e G r a p h V i e w
// -------------------------------------------------------

/**
	Factory function that creates the view for a graph inside an MFC SDI window.
	\ingroup oofGraph oofWinSpecific
*/
BOOL __stdcall oofAfxCreateGraphView(CRuntimeClass* pFrameClass, UINT nId, oofGraph* pGraph, BOOL bMakeMain)
{
	CFrameWnd* pFrame = (CFrameWnd*)pFrameClass->CreateObject();
	
	if (!pFrame || !pFrame->LoadFrame(nId, WS_OVERLAPPEDWINDOW))
		return FALSE;

	if (bMakeMain)
		// Required for SDI applications.
		// Do NOT use for MDI apps. Main window should have already been created & set
		// by calling routine.
		AfxGetThread()->m_pMainWnd = pFrame;

	oofGraphWindow* pView = oofGraphWindow::CreateGraphWindow(pGraph, pFrame, AFX_IDW_PANE_FIRST);
	
	pFrame->SetActiveView(pView);
	pFrame->RecalcLayout();
	pFrame->InitialUpdateFrame(NULL, TRUE); 

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow

IMPLEMENT_DYNCREATE(oofGraphWindow, CScrollView)

BEGIN_MESSAGE_MAP(oofGraphWindow, CScrollView)
	//{{AFX_MSG_MAP(oofGraphWindow)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageSetup)
	ON_COMMAND(ID_GRAPH_SCALE_TO_FIT, OnGraphScaleToFit)
	ON_COMMAND(ID_GRAPH_FULL_SIZE, OnGraphFullSize)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_FULL_SIZE, OnUpdateGraphFullSize)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_SCALE_TO_FIT, OnUpdateGraphScaleToFit)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

oofGraphWindow::oofGraphWindow()
{
	mGraph = NULL;
#ifdef OOFILE_USE_OLDMETAFILE
	mhmf = 0;
#else
	mhemf = 0;
#endif
	mFullSize = TRUE;

	// Default font. We want a TrueType scaleable default font
	mDefaultFont.CreatePointFont(10, _T("Times New Roman"));

	// Get the default print record - for pagesizing, etc.
	CPageSetupDialog	ps(PSD_RETURNDEFAULT | PSD_MARGINS | PSD_DEFAULTMINMARGINS, NULL);
	ps.DoModal();
	setPageSetup(&ps);
}

oofGraphWindow::~oofGraphWindow()
{
	if (mGraph)
		delete mGraph;
#ifdef OOFILE_USE_OLDMETAFILE
	if (mhmf)
		::DeleteMetaFile(mhmf);
#else
	if (mhemf)
		::DeleteEnhMetaFile(mhemf);
#endif
	::DeleteDC(oofCurrentDraw::getInfoDC());
}

//	Common Factory
oofGraphWindow*
oofGraphWindow::CreateGraphWindow(oofGraph* inGraphPtr, CWnd * pParent, UINT nId, CPageSetupDialog * pageSetup)
{
	oofGraphWindow* displayWindow = (oofGraphWindow *)RUNTIME_CLASS(oofGraphWindow)->CreateObject();
	// views are always created with a border!
	if (!displayWindow->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
								CRect(0,0,0,0), pParent, nId, NULL))
	{
		TRACE0("Warning: could not create view for frame.\n");
		return NULL;        // can't continue without a view
	}

	displayWindow->setGraph(inGraphPtr);
	if (pageSetup)
		displayWindow->setPageSetup(pageSetup);
	displayWindow->start();

	inGraphPtr->finishConstructionWithPresets();
	inGraphPtr->setDrawStyles(oofDrawStyleSetDefaultIteration());
	inGraphPtr->draw();

	displayWindow->finish();  
	
	displayWindow->ShowWindow(SW_SHOW);
	displayWindow->OnInitialUpdate();
	return displayWindow;
}

void oofGraphWindow::start()
{
	if (oofCurrentDraw::getInfoDC() == NULL) {
		HDC ic = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		ASSERT(ic != NULL);
		oofCurrentDraw::setInfoDC(ic);
	}

	if (!mhdc.GetSafeHdc()) {
		// Create our metafile dc
#if OOFILE_USE_OLDMETAFILE
		VERIFY(mhdc.Create());
#else
		VERIFY(mhdc.CreateEnhanced(&mhdcRef, NULL, &mPaperHimetric, NULL));
#endif
		// Notify our static object that this is now the current drawing device context
		oofCurrentDraw::setCurrentDC(mhdc.GetSafeHdc());

		mGraph->setCurrentDC(mhdc.GetSafeHdc());	// ES

/*		// Setup the clipping region for the margins
		mhdc.SelectClipRgn(NULL);
		mhdc.ExcludeClipRect(&mMargin);
		mhdc.MoveTo(0, 0);
*/
		// Select in our default font
//		mhdc.SelectObject(&mDefaultFont);
		oofTextDrawer::selectFont(mhdc, mDefaultFont);

		if (mGraph)
			mGraph->setPaneRect(mPaperRect);
	}
}

void oofGraphWindow::finish()
{
	if (mhdc.GetSafeHdc()) {
#ifdef OOFILE_USE_OLDMETAFILE
		mhmf  = mhdc.Close();
#else
		mhemf = mhdc.CloseEnhanced();
#endif
		// Notify our static object that we don't have a valid device context
		oofCurrentDraw::setCurrentDC(mhdc.GetSafeHdc());
	}

	if (mhdcRef.GetSafeHdc()) 
		mhdcRef.DeleteDC();
}

void oofGraphWindow::setPageSetup(CPageSetupDialog * psd)
{
	PAGESETUPDLG* pageSetup = &psd->m_psd;
	
	mMargin		= pageSetup->rtMargin;
	mPaperRect	= oofRect(0, 0, pageSetup->ptPaperSize.x, pageSetup->ptPaperSize.y);
	mPaperHimetric = mPaperRect;

	mhdcRef.CreateDC("DISPLAY",NULL,NULL,NULL);

	// Now convert to pixels & HIMETRIC for drawing & creating the metafile.
	mhdcRef.SaveDC();
	// Set the maping mode according to the units used in the PAGESETUPDLG structure.
	if (pageSetup->Flags & PSD_INTHOUSANDTHSOFINCHES)
		mhdcRef.SetMapMode(MM_HIENGLISH);
	else if (pageSetup->Flags & PSD_INHUNDREDTHSOFMILLIMETERS)
		mhdcRef.SetMapMode(MM_HIMETRIC);

	mhdcRef.LPtoDP(mPaperRect);
	mhdcRef.LPtoDP(mPaperHimetric);
	mhdcRef.SetMapMode(MM_HIMETRIC);
	mhdcRef.DPtoLP(mPaperHimetric);
	mhdcRef.RestoreDC(-1);
	// We need to adjust for the reverse Y axis in MM_HIENGLISH.
	// Because mPaperHimetric has been transformed twice, the effect is cancelled.

	mPaperRect.transform(1, -1);
}

BOOL oofGraphWindow::setMappingMode(CDC* pDC)
{
	pDC->SetMapMode(MM_ANISOTROPIC);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	PRINTDLG printDlg;
	memset(&printDlg, '\0', sizeof(PRINTDLG));

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

BOOL oofGraphWindow::setMappingMode(HDC hDC)
{
	CDC* pDC = CDC::FromHandle(hDC);
	BOOL bRetVal = setMappingMode(pDC);
	pDC->Detach();

	return bRetVal;
}

BOOL oofGraphWindow::getPaperSize(HANDLE hDevNames, HANDLE hDevMode, LPPOINT paperSize)
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

	if (lpDevMode->dmOrientation == DMORIENT_LANDSCAPE && dwOrientRel != 0) {
		paperSize->y = lpDevMode->dmPaperWidth;
		paperSize->x = lpDevMode->dmPaperLength;
	}
	else {
		paperSize->x = lpDevMode->dmPaperWidth;
		paperSize->y = lpDevMode->dmPaperLength;
	}

	::GlobalUnlock(hDevMode);
	::GlobalUnlock(hDevNames);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow drawing

void oofGraphWindow::OnDraw(CDC* pDC)
{
#ifdef OOFILE_USE_OLDMETAFILE
	pDC->PlayMetaFile(mhmf);
#else
	pDC->PlayMetaFile(mhemf, mPaperRect);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow diagnostics

#ifdef _DEBUG
void oofGraphWindow::AssertValid() const
{
	CScrollView::AssertValid();
}

void oofGraphWindow::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow message handlers

void oofGraphWindow::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	// Calculate the total size of this view
	SetScrollSizes(MM_TEXT, CSize(mPaperRect.width(), mPaperRect.height()));
}

BOOL oofGraphWindow::OnPreparePrinting(CPrintInfo* pInfo) 
{
	pInfo->SetMaxPage(1);

	DoPreparePrinting(pInfo);
	return CScrollView::OnPreparePrinting(pInfo);
}

void oofGraphWindow::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
#ifdef OOFILE_USE_OLDMETAFILE
	pDC->PlayMetaFile(mhmf);
#else
	pDC->PlayMetaFile(mhemf, &pInfo->m_rectDraw);
#endif
}


void oofGraphWindow::OnFilePageSetup() 
{
	CPageSetupDialog	ps(PSD_MARGINS | PSD_DEFAULTMINMARGINS, this);

	if (ps.DoModal() == IDOK) {
		setPageSetup(&ps);
		// Redo the report for the new size
		start();
		if (mGraph)
			mGraph->draw();
		finish();
		Invalidate();
	}
}

void oofGraphWindow::OnGraphScaleToFit() 
{
	mFullSize = FALSE;
	SetScaleToFitSize(CSize(mPaperRect.width(), mPaperRect.height()));
	Invalidate();
}

void oofGraphWindow::OnUpdateGraphScaleToFit(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!mFullSize);
}

void oofGraphWindow::OnGraphFullSize() 
{
	mFullSize = TRUE;
	SetScrollSizes(MM_TEXT, CSize(mPaperRect.width(), mPaperRect.height()));
	Invalidate();
}

void oofGraphWindow::OnUpdateGraphFullSize(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(mFullSize);
}


void oofGraphWindow::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	if (pDC->IsPrinting())
	{
#ifdef OOFILE_USE_OLDMETAFILE
		// Set the mapping mode that will convert the report's dimension from screen
		// into the printer resolution. We needn't do that for the enhanced metafile
		// as it allows the sizing directly.
		VERIFY(setMappingMode(pDC));
#endif
	}
	else
		CScrollView::OnPrepareDC(pDC, pInfo);
}
