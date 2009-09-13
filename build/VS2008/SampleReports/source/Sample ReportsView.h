// Sample ReportsView.h : interface of the CSampleReportsView class
//
/////////////////////////////////////////////////////////////////////////////

class CSampleReportsView : public CView
{
protected: // create from serialization only
	CSampleReportsView();
	DECLARE_DYNCREATE(CSampleReportsView)

// Attributes
public:
	CSampleReportsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleReportsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSampleReportsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSampleReportsView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Sample ReportsView.cpp
inline CSampleReportsDoc* CSampleReportsView::GetDocument()
   { return (CSampleReportsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
