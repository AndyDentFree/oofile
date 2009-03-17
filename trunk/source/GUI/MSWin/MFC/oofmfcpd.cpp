// oofmfcpd.cpp : implementation file
//

#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofres.h"
#include "oofmfcpd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COOFProgressDlg::COOFProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COOFProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COOFProgressDlg)
	//}}AFX_DATA_INIT
}

void COOFProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COOFProgressDlg)
	DDX_Control(pDX, IDC_STATIC_PROGRESS_TITLE, mProgressTitleCtrl);
	DDX_Control(pDX, IDC_PROGRESS, mProgressCtrl);
	//}}AFX_DATA_MAP
}

BOOL COOFProgressDlg::Create(unsigned long numItems, const char* progressText)
{
	if (!CDialog::Create(COOFProgressDlg::IDD))
		return FALSE;

	mProgressTitleCtrl.SetWindowText(progressText);
	mProgressCtrl.SetRange(0, numItems);
	return TRUE;
}


BEGIN_MESSAGE_MAP(COOFProgressDlg, CDialog)
	//{{AFX_MSG_MAP(COOFProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COOFProgressDlg message handlers

void COOFProgressDlg::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// COOFProgressDlg message handlers

BOOL COOFProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#endif // !_Macintosh for doxygen
