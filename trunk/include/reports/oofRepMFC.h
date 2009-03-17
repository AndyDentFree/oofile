//////////////////////////////////////////////////////////////////
// oofRepMFC.h - Definition for oofRepWindow - CScrollView derived encapsulated view
//				  for MFC based applications.
//
// COPYRIGHT 1997 A.D. Software, All rights reserved

#ifndef H_OOFREPMFC
#define H_OOFREPMFC

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofRPRes.h"

// Global helper to construct our report view for MFC apps.
// Replace the document template functionality with calls to this function.
CFrameWnd*	__stdcall oofAfxCreateReportView(CRuntimeClass* pFrameClass, UINT nId,
											 oofRep* pReport, oofRep* pRepLandscape = NULL,
											 BOOL bMakeMain = TRUE);
// Creates and displays the modal preview window.
BOOL		__stdcall oofAfxDoModalPreview(UINT nId,
										   oofRep* pReport, oofRep* pRepLandscape = NULL,
										   BOOL bMakeMain = TRUE);
// Prints the report using the current application settings.
BOOL		__stdcall oofAfxPrintReport(oofRep* inRep, oofRep* inRepLandscape = NULL,
										HWND hwndPrtDlgOwner = NULL);
// Sets the current application's printer and page setup.
BOOL		__stdcall oofAfxDoPageSetup();


/////////////////////////////////////////////////////////////////////////////
// oofRepWindow view

/**
	Preview window using the MFC framework.
	\ingroup oofRenderWin
*/
class oofRepWindow : public CScrollView
{
public:
	// declare command so can be used like Mac
	enum { cmdRefresh=ID_REPORT_REFRESH };
	
protected:
	oofRepWindow();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(oofRepWindow)

// Attributes
public:
	oofRepWinGUIPreviewMFC*	mEnv;
	int mCurrentPageIndex;  // 0-based
#ifdef OOF_READ_REPORTS
	dbConnect*				mOpenedReportDB; // owned
#endif	

protected:
	bool					mBlackAndWhitePrinting;
	// portrait mode report
	oofRep*					mRep; // owned
	// optional report for landscape mode (if NULL then mRep is used for both modes)
	oofRep*					mRepLandscape;  // owned
	unsigned long			mSavePage;
	PAGESETUPDLG*			mPageSetup;
	// percentage scale factor
	//		- positive value - percentage scale factor
	//		- negative value - Fit Page or Fit Width
	int						mScaleFactor, mCachedScaleRatio;
	bool					mButtonWasDown;  // because just catching Up's gets Open dialogs
	// string description array of zoom types (1-based)
	static char*			sZoomType[];

protected:
	enum {
		ZoomType_FitPage  = 1,
		ZoomType_FitWidth = 2
	};
	
// Operations
public:
	static oofRepWindow*	CreateReportWindow(oofRep* inRepPtr, oofRep* inRepLandscapePtr,
								CWnd* pParent, UINT nId = 0, CPageSetupDialog* pageSetup = NULL);
	void CreatePreviewContent(oofRep* adoptedReport, oofRep* inRepLandscapePtr, CPageSetupDialog* pageSetup);
	
	static BOOL	IsLandscapeMode();

	void					setReport(oofRep* inRep);
	void					setLandscapeReport(oofRep* inRepLandscape);

	oofRep*					activeReport() const;
	void					setPageSetup(CPageSetupDialog* pageSetup);
	void					start();
	void					finish();
	bool					blackAndWhitePrinting() const;
	void					blackAndWhitePrinting(bool bFlag);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(oofRepWindow)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~oofRepWindow();
	CString GetFileName(LPCTSTR lpszDefFileExt, LPCTSTR lpszFilter);
	CSize	GetTransformedPageSize(const CRect& pageRect);
	void	ScalePage(int scaleFactor);
	void AdjustScrollSize();
	void ReflowReport();
	bool ScalingRelativeToPage() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(oofRepWindow)
	afx_msg void OnReportFirst();
	afx_msg void OnReportLast();
	afx_msg void OnReportNext();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateReportNext(CCmdUI* pCmdUI);
	afx_msg void OnReportPrevious();
	afx_msg void OnUpdateReportPrevious(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReportBW(CCmdUI* pCmdUI);
	afx_msg void OnFilePageSetup();
	afx_msg void OnFileClose();
#ifdef OOF_READ_REPORTS
	afx_msg void OnFileOpen();
#endif
	afx_msg void OnUpdateFileExit(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnReportSave();
	afx_msg void OnReportBW();
	afx_msg void OnReportZoom();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint pt);
	afx_msg LRESULT OnRefresh(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class oofZoomDlg;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// oofRepPreviewFrame frame

/**
	Frame for preview window using the MFC framework.
	\ingroup oofRenderWin
*/
class oofRepPreviewFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(oofRepPreviewFrame)
protected:
	oofRepPreviewFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	virtual void ActivateFrame(int nCmdShow = -1);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(oofRepPreviewFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~oofRepPreviewFrame();
protected:
	CToolBar	m_wndToolBar;
	CButton m_BWCheckBox;
protected:
	// Generated message map functions
	//{{AFX_MSG(oofRepPreviewFrame)
	afx_msg int OnCreate(LPCREATESTRUCT);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// oofRepModalPreviewFrame frame

/**
	Frame for preview window using the MFC framework in modal window so stays in front.
	\ingroup oofRenderWin
*/
class oofRepModalPreviewFrame : public oofRepPreviewFrame
{
	DECLARE_DYNCREATE(oofRepModalPreviewFrame)
protected:
	oofRepModalPreviewFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(oofRepModalPreviewFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~oofRepModalPreviewFrame();

	// Generated message map functions
	//{{AFX_MSG(oofRepModalPreviewFrame)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// oofZoomDlg dialog used for App About

/**
	Zoom dialog allowing preview zoom to be specified in MFC framework.
	\ingroup oofRenderWin
*/
class oofZoomDlg : public CDialog
{
public:
	oofZoomDlg(int scaleFactor);

// Dialog Data
	//{{AFX_DATA(oofZoomDlg)
	enum { IDD = IDD_OOF_ZOOMDLG };
	CComboBox	mComboCtrl;
	CString		mComboVal;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(oofZoomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(oofZoomDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	// percentage scale factor
	//		- positive value - percentage scale factor
	//		- negative value - Fit Page or Fit Width
	int mScaleFactor;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// inline definitions
//
inline bool 
oofRepWindow::ScalingRelativeToPage() const
{
	return mScaleFactor <= 0;
}

inline bool
oofRepWindow::blackAndWhitePrinting() const
{
	return mBlackAndWhitePrinting;
}

inline void
oofRepWindow::blackAndWhitePrinting(bool bFlag)
{
	mBlackAndWhitePrinting = bFlag;
}

inline void
oofRepWindow::finish()
{
	mEnv->finish();
}

inline void
oofRepWindow::setPageSetup(CPageSetupDialog* pageSetup)
{
	mEnv->setPageSetup(&pageSetup->m_psd);
}

inline void
oofRepWindow::setReport(oofRep* inRep)
{
	mRep = inRep;
}

inline void
oofRepWindow::setLandscapeReport(oofRep* inRepLandscape)
{
	mRepLandscape = inRepLandscape;
}

inline oofRep*
oofRepWindow::activeReport() const
{
	return mRepLandscape != NULL && IsLandscapeMode() ? mRepLandscape : mRep;
}
#endif  // !_Macintosh for doxygen
#endif	// H_OOFREPMFC
