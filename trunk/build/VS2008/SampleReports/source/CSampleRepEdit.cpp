// CSampleRepEdit.cpp -- dialog methods
// Created 01/01/95 12:01 PM by AppMaker

#include "stdafx.h"
#include "CSampleRepEdit.h"
#include "oofrepeditx.h"
#include "oofRPRes.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRepEditDialog dialog

CRepEditDialog::CRepEditDialog(CWnd* pParent /*=NULL*/)	: 
	CDialog(CRepEditDialog::IDD, pParent)
{	
//{{AFX_DATA_INIT(CRepEditDialog)		
// NOTE: the ClassExpress will add member initialization here	
//}}AFX_DATA_INIT
}

/*
void CRepEditDialog::DoDataExchange(CDataExchange* pDX)
{	
	CDialog::DoDataExchange(pDX);	
//{{AFX_DATA_MAP(CRepEditDialog)		
// NOTE: the ClassExpress will add DDX and DDV calls here	
//}}AFX_DATA_MAP
}
*/


// static factory
void
CRepEditDialog::CreateEditDialog(	
	oofRepEditingEnv* inEnv, oofRepEditLink* inLink)
{
	CRepEditDialog dlog;
	dlog.FinishCreationByLinkingDataModel (inEnv, inLink);
	dlog.DoModal();
}

BOOL CRepEditDialog::OnInitDialog() 
{
    // This routine happens just before the dialog is displayed

    // Base class function call
   	BOOL ret = CDialog::OnInitDialog();
    
	if (ret) {
		m_CellContent.SubclassDlgItem(IDC_CellContent, this);
		m_Location.SubclassDlgItem(IDC_LOCATION, this);
		InitEditControls( new dbGUIAdaptCEdit(&m_CellContent) );
		oofRepEditLink* saveLink = mCurrentLink;
		mCurrentLink = 0;  // so we don't upset EnterLink
		EnterLink(saveLink);
	}
	return ret;
}

BEGIN_MESSAGE_MAP(CRepEditDialog, CDialog)	
//{{AFX_MSG_MAP(CRepEditDialog)		
	ON_BN_CLICKED(IDC_GoFirstCell, OnButtonGoFirstCell)
	ON_BN_CLICKED(IDC_GoLastCell, OnButtonGoLastCell)
	ON_BN_CLICKED(IDC_GoUpCell, OnButtonGoUpCell)
	ON_BN_CLICKED(IDC_GoDownCell, OnButtonGoDownCell)
	ON_BN_CLICKED(IDC_GoLeftCell, OnButtonGoLeftCell)
	ON_BN_CLICKED(IDC_GoRightCell, OnButtonGoRightCell)
	ON_BN_CLICKED(IDC_GoPrevObjectOnPage, OnButtonGoPrevObject)
	ON_BN_CLICKED(IDC_GoNextObjectOnPage, OnButtonGoNextObject)

// NOTE: the ClassWizard will add message map macros here	
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// mandatory behavioural overrides - depend on type of content field
// and if custom content is used
void 
CRepEditDialog::LoadCellEditField()
{
	assert (mCurrentLink);
	bool isEditable;
	oofString location;
	oofString cellText = mCurrentLink->getCellText(&isEditable, &location);
	cellText.lineEndingsDOS();  // or could call dbText::convertsLineEndingsOnLoad(true)
	mAdapter->setString(cellText);
	m_Location.SetWindowText(location);
}




/////////////////////////////////////////////////////////////////////////////
// CRepEditDialog message handlers
void 
CRepEditDialog::OnCancel() 
{
	if(mAnyContentDirtied){
		mEnv->parentWindow()->PostMessage(ID_REPORT_REFRESH);  // queue for parent to handle when we exit
	}

	CDialog::OnCancel();
}

void 
CRepEditDialog::OnOK() 
{
	if(AttemptSaveCellEditField()){
		if(mAnyContentDirtied){
			mEnv->parentWindow()->PostMessage(ID_REPORT_REFRESH);  // queue for parent to handle when we exit
		}
		CDialog::OnOK();
	}
}


void  
CRepEditDialog::OnButtonGoFirstCell() 
{
	GoToAnotherCell (eGoFirst);
}


	
void 
CRepEditDialog::OnButtonGoLastCell() 
{
	GoToAnotherCell (eGoLast);
}


void 
CRepEditDialog::OnButtonGoUpCell() 
{
	GoToAnotherCell (eGoUp);
}


void 
CRepEditDialog::OnButtonGoDownCell() 
{
	GoToAnotherCell (eGoDown);
}


void 
CRepEditDialog::OnButtonGoLeftCell() 
{
	GoToAnotherCell (eGoLeft);
}


void 
CRepEditDialog::OnButtonGoRightCell() 
{
	GoToAnotherCell (eGoRight);
}


void
CRepEditDialog::OnButtonGoPrevObject() 
{
	GoToAnotherItem (eGoUp);
}


void 
CRepEditDialog::OnButtonGoNextObject() 
{
	GoToAnotherItem (eGoDown);
}



// -------------------------------------------------------
//             o o f R e p E d i t o r
// -------------------------------------------------------
void 
oofRepEditor::startEditing(OOF_InteractionArea* inArea, oofRepEditingEnv* inEnv)
{
// pass in area so 
// NOT YET IMPLEMENTED - launch window with zoomrect from area clicked
	oofRepEditLink* theLink = inArea->mEditLink;
	assert(theLink);
	theLink->setCurrentCoords(inArea->mCol, inArea->mRow);
	
	CRepEditDialog::CreateEditDialog(inEnv, theLink);
}
