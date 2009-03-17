//////////////////////////////////////////////////////////////////
// oofGrWin.hpp - Definition for oofGraphWindow - CScrollView derived encapsulated view
//				  for MFC based applications.
//
// COPYRIGHT 1997 A.D. Software, All rights reserved

#ifndef H_OOFGRWIN
#define H_OOFGRWIN


// Global helper to construct our report view for MFC apps.
// Replace the document template functionality with calls to this function.
BOOL __stdcall	oofAfxCreateGraphView(CRuntimeClass * pFrameClass, UINT nId, oofGraph * pGraph, BOOL bMakeMain = TRUE);


/////////////////////////////////////////////////////////////////////////////
// oofGraphWindow view

class oofGraphWindow : public CScrollView
{
protected:
	oofGraphWindow();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(oofGraphWindow)

// Attributes
public:

protected:
#ifdef OOFILE_USE_OLDMETAFILE
	HMETAFILE				mhmf;
#else
	HENHMETAFILE			mhemf;
#endif
	oofGraph *				mGraph;
	CDC						mhdcRef;
	CMetaFileDC				mhdc;
	oofRect					mPaperRect;		// Paper Size
	oofRect					mMargin;
	oofRect					mPaperHimetric;
	CFont					mDefaultFont;
	BOOL					mFullSize;

	
// Operations
public:
	static oofGraphWindow*	CreateGraphWindow(oofGraph* inGraphPtr, CWnd * pParent, UINT nId = 0, CPageSetupDialog * pageSetup = NULL);
	// Sets the mapping mode that will enlarge the old metafile outputs on the printer
	// (gets the application's printer defaults). We have to work with CDC because of
	// the preview device contexts that override functions like SetWindowExt, SetWindowOrg,
	// etc.
	static BOOL				setMappingMode(CDC* pDC);
	// utility function that converts HDC into a temporary CDC and passes it to the CDC
	// version of setMappingMode
	static BOOL				setMappingMode(HDC hDC);
	static BOOL				getPaperSize(HANDLE hDevNames, HANDLE hDevMode, LPPOINT paperSize);

	void					setGraph(oofGraph * inGraph)
								{ mGraph = inGraph; };
	void					setPageSetup(CPageSetupDialog * psd);
	void					start();
	void					finish();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(oofGraphWindow)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~oofGraphWindow();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(oofGraphWindow)
	afx_msg void OnFilePageSetup();
	afx_msg void OnGraphScaleToFit();
	afx_msg void OnGraphFullSize();
	afx_msg void OnUpdateGraphFullSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphScaleToFit(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// H_OOFGRWIN
