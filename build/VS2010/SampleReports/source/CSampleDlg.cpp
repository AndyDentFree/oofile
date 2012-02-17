// CSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sample Reports.h"
#include "CSampleDlg.h"
#include "CSampleBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg dialog


CSampleDlg::CSampleDlg(CSampleBase* reportBase, CWnd* pParent /*=NULL*/)
	: CDialog(CSampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	mSampleReports = reportBase;
}


void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleDlg)
	DDX_Control(pDX, IDC_REP_COMBO, m_ReportCombo);
	DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSampleDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleDlg)
	ON_CBN_CLOSEUP(IDC_REP_COMBO, OnCloseupRepCombo)
	ON_BN_CLICKED(IDC_PRINT_REP, OnPrintRep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg message handlers

BOOL CSampleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	const unsigned short repNum = mSampleReports->numberOfReps();
	for(unsigned short i=0; i<repNum; i++){
		oofString comboItem = mSampleReports->menuEntry(i);
		m_ReportCombo.AddString(comboItem);
	}
	m_ReportCombo.SetCurSel(0);
	OnCloseupRepCombo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSampleDlg::OnCloseupRepCombo() 
{

	int choice = m_ReportCombo.GetCurSel();
	//set the report choice
	CSampleBase::mReportChoice = choice;
	//set the description
	oofString repDescription = mSampleReports->description(choice);
	m_Description.SetWindowText(repDescription);
	
}


void CSampleDlg::OnPrintRep() 
{
		oofRep* theRep = mSampleReports->makeReport(CSampleBase::mReportChoice);
		mSampleReports->printReport(theRep);
		delete theRep;
}
