// COPYRIGHT 1997 A.D. Software, All rights reserved

/**
	\file oofGrWin.cpp implementation file for graph preview window if using plain WinSDK.
*/

#include "stdafx.h"
#include "oofGrWin.hpp"
#include "\OOFJames\source\utils\oofDraw.hpp"
#include "oofRes.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -------------------------------------------------------
//        o o f A f x C r e a t e G r a p h V i e w
// -------------------------------------------------------

/**
	Factory function that creates the view for a graph inside an SDK SDI window.
	\ingroup oofGraph oofWinSpecific
*/
BOOL __stdcall oofAfxCreateGraphView(CRuntimeClass * pFrameClass, UINT nId, oofGraph * pGraph, BOOL bMakeMain)
{
	CFrameWnd *	pFrame = (CFrameWnd *)pFrameClass->CreateObject();
	
	if (!pFrame)
		return FALSE;

	if (!pFrame->LoadFrame(nId, WS_OVERLAPPEDWINDOW))
		return FALSE;
	if (bMakeMain)
		// Required for SDI applications.
		// Do NOT use for MDI apps. Main window should have already been created & set
		// by calling routine.
		AfxGetThread()->m_pMainWnd = pFrame;

	oofGraphWindow * pView = oofGraphWindow::CreateGraphWindow(pGraph, pFrame, AFX_IDW_PANE_FIRST);
	
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
	mhemf = 0;
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
	if (mhemf)
		::DeleteEnhMetaFile(mhemf);
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
	if (!mhdc.GetSafeHdc()) {
		// Create our metafile dc
		mhdc.CreateEnhanced(&mhdcRef, NULL, &mPaperHimetric, NULL);
		// Notify our static object that this is now the current drawing device context
		oofCurrentDraw::setCurrentDC(mhdc.GetSafeHdc());

		// Setup the clipping region for the margins
		mhdc.SelectClipRgn(NULL);
		mhdc.ExcludeClipRect(&mMargin);
		mhdc.MoveTo(0, 0);
		// Select in our default font
		mhdc.SelectObject(&mDefaultFont);

		if (mGraph)
			mGraph->setPaneRect(mPaperRect);
	}
}

void oofGraphWindow::finish()
{
	if (mhdc.GetSafeHdc()) {
		mhemf = mhdc.CloseEnhanced();

		// Notify our static object that we don't have a valid device context
		oofCurrentDraw::setCurrentDC(mhdc.GetSafeHdc());
	}
	if (mhdcRef.GetSafeHdc()) 
		mhdcRef.DeleteDC();
}

void oofGraphWindow::setPageSetup(CPageSetupDialog * psd)
{
	PAGESETUPDLG * pageSetup = &psd->m_psd;
	
	mMargin		= pageSetup->rtMargin;
	mPaperRect	= oofRect(0, 0, pageSetup->ptPaperSize.x, pageSetup->ptPaperSize.y);
	mPaperHimetric = mPaperRect;

	// Now convert to pixels & HIMETRIC for drawing & creating the metafile.
	mhdcRef.CreateCompatibleDC(NULL); 
	
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

/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow drawing

void oofGraphWindow::OnDraw(CDC* pDC)
{
	pDC->PlayMetaFile(mhemf, mPaperRect);
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
	pDC->PlayMetaFile(mhemf, &pInfo->m_rectDraw);
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

