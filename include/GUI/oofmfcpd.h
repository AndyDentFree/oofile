#if !defined(AFX_OOFMFCPC_H__162C36C4_3294_11D1_8BEE_444553540000__INCLUDED_)
#define AFX_OOFMFCPC_H__162C36C4_3294_11D1_8BEE_444553540000__INCLUDED_

// oofmfcpd.h : header file
//

#include "oofres.h"
#ifndef __AFXCMN_H__
	#include <AFXCMN.H>
#endif

// disable warning that occurs when bool, true & false are used. GDH
// NOTE these need including after Windows and AFX includes
// as somewhere MS enable all warnings again
	#if _MSC_VER>=1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
		#pragma warning( disable : 4237 )  // bool, true & false
		#pragma warning( disable : 4800 )  // bool conversion to int
		#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
	#endif

/////////////////////////////////////////////////////////////////////////////
// COOFProgressDlg dialog

/**
	Progress bar displayed when delays loading lists.
	\ingroup oofWinSpecific oofGUI
*/
class COOFProgressDlg : public CDialog
{
// Construction
public:
	COOFProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COOFProgressDlg)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CStatic	mProgressTitleCtrl;
	CProgressCtrl	mProgressCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	virtual BOOL Create(unsigned long numItems, const char* progressText);
	void setPos(int pos);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COOFProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
protected:
	enum {ProgressDlgThreshold = 300	/* threshold for displaying a progress dialog */ };
};

/////////////////////////////////////////////////////////////////////////////

inline void
COOFProgressDlg::setPos(int pos)
{
	mProgressCtrl.SetPos(pos);
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OOFMFCPC_H__162C36C4_3294_11D1_8BEE_444553540000__INCLUDED_)
