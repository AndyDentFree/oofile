// Sample ReportsDoc.cpp : implementation of the CSampleReportsDoc class
//

#include "stdafx.h"
#include "Sample Reports.h"

#include "Sample ReportsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsDoc

IMPLEMENT_DYNCREATE(CSampleReportsDoc, CDocument)

BEGIN_MESSAGE_MAP(CSampleReportsDoc, CDocument)
	//{{AFX_MSG_MAP(CSampleReportsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsDoc construction/destruction

CSampleReportsDoc::CSampleReportsDoc()
{
	// TODO: add one-time construction code here

}

CSampleReportsDoc::~CSampleReportsDoc()
{
}

BOOL CSampleReportsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsDoc serialization

void CSampleReportsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsDoc diagnostics

#ifdef _DEBUG
void CSampleReportsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSampleReportsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleReportsDoc commands
