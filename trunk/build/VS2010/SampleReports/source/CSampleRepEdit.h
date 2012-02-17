// CSampleRepEdit.h -- dialog class
// Created 01/01/95 12:01 PM by AppMaker

#include "oofile.h"
#include "oofrep.h"
#include "database.h"
#include "CSampleBase.h"
#ifndef H_OOFREPEDITWIN
	#include "oofrepeditWin.h"
#endif
#include "resource.h"
#ifndef OOFMFCCT_H
	#include "oofmfcct.h"
#endif
#ifndef H_OOFMFC
	#include "oofmfc.h"
#endif

class CRepEditDialog : public CDialog, public OOF_mixRepCellEditor
{
// Construction
public:
	CRepEditDialog(CWnd* pParent = NULL);	// standard constructor

	static void CreateEditDialog(oofRepEditingEnv*, oofRepEditLink*);

// Dialog Data
	//{{AFX_DATA(CRepEditDialog)
	enum { IDD = IDD_CellEditor };
		// NOTE: the ClassExpress will add data members here
	//}}AFX_DATA

// Implementation
protected:
// ASD	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	COOFEdit m_CellContent;
	CStatic	m_Location;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEditBranch)
	protected:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL


// mandatory behavioural overrides for OOF_mixRepCellEditor
	virtual void LoadCellEditField();

	// Generated message map functions
	//{{AFX_MSG(CRepEditDialog)
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnButtonGoFirstCell();
	afx_msg void OnButtonGoLastCell();
	afx_msg void OnButtonGoUpCell();
	afx_msg void OnButtonGoDownCell();
	afx_msg void OnButtonGoLeftCell();
	afx_msg void OnButtonGoRightCell();
	afx_msg void OnButtonGoPrevObject();
	afx_msg void OnButtonGoNextObject();
		// NOTE: the ClassWizrd will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


