// Sample ReportsView.cpp : implementation of the CSampleReportsView class
//

#include "stdafx.h"
#include "Sample Reports.h"

#include "Sample ReportsDoc.h"
#include "Sample ReportsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsView

IMPLEMENT_DYNCREATE(CSampleReportsView, CView)

BEGIN_MESSAGE_MAP(CSampleReportsView, CView)
	//{{AFX_MSG_MAP(CSampleReportsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsView construction/destruction

CSampleReportsView::CSampleReportsView()
{
	// TODO: add construction code here

}

CSampleReportsView::~CSampleReportsView()
{
}

BOOL CSampleReportsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsView drawing

void CSampleReportsView::OnDraw(CDC* pDC)
{
	CSampleReportsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsView diagnostics

#ifdef _DEBUG
void CSampleReportsView::AssertValid() const
{
	CView::AssertValid();
}

void CSampleReportsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSampleReportsDoc* CSampleReportsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleReportsDoc)));
	return (CSampleReportsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsView message handlers

BOOL CSampleReportsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
