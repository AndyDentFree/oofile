// CSampleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSampleDlg dialog
#include "CSampleBase.h"

class CSampleDlg : public CDialog
{
// Construction
public:
	CSampleDlg(CSampleBase* reportBase, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSampleDlg)
	enum { IDD = IDD_SAMPLE_REP_DIALOG };
	CComboBox	m_ReportCombo;
	CStatic	m_Description;
	//}}AFX_DATA

	CSampleBase* mSampleReports;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupRepCombo();
	afx_msg void OnPrintRep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
