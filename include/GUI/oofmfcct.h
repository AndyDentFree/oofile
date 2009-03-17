// COPYRIGHT 1997 A.D. Software, All rights reserved

/**
\file oofmfcct.h
	MFC controls for OOFILE database
	subclassed to add the attachment model reacting to events that we need
	for dbGUIAdapter
*/

#ifndef OOFMFCCT_H
#define OOFMFCCT_H

#include "oofmsg.h"
#include "oof1.h"
#include "TreeCtrlEx.h"

#include "afxole.h"

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

class dbEditLinkDirtier;
class dbClickLinkDirtier;
class dbComboLinkDirtier;
class dbView;
class dbTable;
class dbGUI;
class OOF_ExpandableLongArray;
class dbHierLevel;
class dbItemCache;
class COOFProgressDlg;
class dbViewOutlineItem;

/**
	Message used in map of COOFListCtrl when selection changes.
	Posted by COOFListCtrl::OnItemChanged.
	\ingroup oofMacros
*/
#define WM_OOFILE_CHANGESEL  WM_APP

/**
	Message used in map of COOFListCtrl when something dropped on list.
	Posted by COOFOleDropTarget::OnDragEnter and COOFOleDropTarget::OnDragOver.
	\ingroup oofMacros
*/
#define WM_OOFILE_ACCEPTDROP (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// OOF_NavigableList mixin for list classes to react to keypresses

/**
	mixin for list classes to react to keypresses.
	\ingroup oofWinSpecific oofGUI
*/
class OOF_NavigableList {
protected:
	OOF_NavigableList(CWnd*);
	~OOF_NavigableList();
	
	void AddChar(char);
	BOOL IsTypingChar(char) const;
	void ResetTimer();
	BOOL FindOIDforString(dbView* inView, oidT& outOID);
	BOOL FindRowforString(dbView* inView, unsigned long& outRow);
	
// data storage
	CWnd* mWndSubclass;
	OOF_String	mAccumulatedString;
	BOOL mTimerActive;
	UINT mNavListTimerID;
};


/////////////////////////////////////////////////////////////////////////////
// COleListBoxDropTarget window

/**
	Drop target window for drag and drop between lists.
	\todo more detailed explanation of why this is needed and if used other than lists.
	\ingroup oofWinSpecific oofGUI
*/
class COOFOleDropTarget : public COleDropTarget
{
// Construction
public:
    COOFOleDropTarget();

// Implementation
public:
	virtual ~COOFOleDropTarget();   
    
	// these members MUST be overridden for an OLE drop target
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave(CWnd* pWnd);               

	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
	void GetDropData(COleDataObject* pDataObject, CWnd** ppCommandTarget, long* pCommandID);
};


class dbGUIAdaptCWnd;
/////////////////////////////////////////////////////////////////////////////
// OOF_mixEditAttachment mixin for OOFILE edit subclasses
/**
	mixin for OOFILE edit subclasses to give Attachment model like PowerPlant.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT OOF_mixEditAttachment {
public:
	OOF_mixEditAttachment();
	void addAttachment(dbGUIAdaptCWnd*);
	
protected:
	void AttachedOnMessage(UINT message, WPARAM, LPARAM) ;
	void AttachedOnKillFocus(CWnd* pNewWnd) ;
	
	dbGUIAdaptCWnd* mAdapter;
};


/////////////////////////////////////////////////////////////////////////////
// COOFEdit window

/**
	Text edit control providing the extra event reactions for OOFILE GUI updates.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT COOFEdit : public CEdit, public OOF_mixEditAttachment
{
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COOFComboBox window

/**
	Popup menu control providing the extra event reactions for OOFILE GUI updates.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT COOFComboBox : public CComboBox, public OOF_mixEditAttachment
{

	/**
		Editor embedded in combo box so OOFILE can react to entry of new item.
		\ingroup oofWinSpecific oofGUI
	*/
class COOFSubEdit : public COOFEdit
	{
		// private operations
		protected:
		    virtual WNDPROC* GetSuperWndProcAddr();

		// Attributes
		protected:
			static WNDPROC	pfnSuper;

		friend class COOFComboBox;
	};

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFComboBox)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFComboBox)
	afx_msg void OnSelChange();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Attributes
protected:
	COOFSubEdit mEditPart;
};

/////////////////////////////////////////////////////////////////////////////
// COOFCheckBox window

/**
	Checkbox control providing the extra event reactions for OOFILE GUI updates.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT COOFCheckBox : public CButton, public OOF_mixEditAttachment
{
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFCheckBox)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFCheckBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COOFListBox window

/**
	List control providing the extra event reactions for OOFILE GUI updates.
	\ingroup oofWinSpecific oofGUI
*/
#ifdef __MWERKS__
/// bug in Pro5 x86 compilers still there in Pro7, ref WB1-29189 (reported against Pro7)
class OOFGUI_EXPORT COOFListCtrl : public CListCtrl, public OOF_NavigableList
#else
class OOFGUI_EXPORT COOFListBox : public CListBox, protected OOF_NavigableList
#endif
{
// Construction
public:
	COOFListBox();

// Operations
public:
	void	changeViewTo(dbView* theView);
	dbGUI*	getGUI() const;
	void	initializeSpace(unsigned int count);
	void	adjustNumItems();

	void	enableDragDrop(long int commandID, CWnd* commandTarget);
	void	acceptDrop(unsigned int numID, long* arrayID);
	void	acceptDrop(long ID=0);
	void	enableItemCaching(BOOL bEnable = TRUE);
	void	disableItemCaching();

	void	setSingleClickMessage(MessageT msg);
	void	setDoubleClickMessage(MessageT msg);
	BOOL	isItemCachingEnabled() const;
	const dbView* getBrowsingView() const;

	int		GetCountPerPage() const;
	BOOL	EnsureVisible(int nIndex);

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COOFListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFListBox)
	afx_msg void OnSelChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblClk();
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LONG OnOOFileAcceptDrop(UINT wParam, LONG lParam);

	DECLARE_MESSAGE_MAP()

protected:
	void FillItems(unsigned int count, COOFProgressDlg* progressDlg);
	const char* GetItemString(UINT nIndex);

// Attributes
protected:
	dbView*	mView;

	MessageT mSingleClickMessage;
	MessageT mDoubleClickMessage;

	COOFOleDropTarget mDropTarget;
	// Drag&Drop cache and handle
	COleDataSource	mOleDataSource;
	// IDs which can be dropped into this control via drag&drop operation
	unsigned int mNumID;
	long* mArrayID;
	long  mDropID;
	// command ID which Drag&Drop operation sends to mCommandTarget
	long int		mCommandID;
	CWnd*			mCommandTarget;

	dbItemCache* mItemCache;
	BOOL mItemCaching;

	enum {
		LeftMargin				= 2,
		TopMargin				= 1,
		BufSize					= 100,	// internal buffer for text description
		ProgressDlgThreshold	= 300	// threshold for displaying a progress dialog
	};
};

/////////////////////////////////////////////////////////////////////////////
// COOFListCtrl window

/**
	List control providing the extra event reactions for OOFILE GUI updates.
	\ingroup oofWinSpecific oofGUI
*/
#ifdef __MWERKS__
/// bug WB1-29189 in Pro7
class OOFGUI_EXPORT COOFListCtrl : public CListCtrl, public OOF_NavigableList
#else
class OOFGUI_EXPORT COOFListCtrl : public CListCtrl, protected OOF_NavigableList
#endif
{
// Construction
public:
	COOFListCtrl();

// highlighting mode
public:
	enum EHighlight {HIGHLIGHT_NORMAL, HIGHLIGHT_ALLCOLUMNS, HIGHLIGHT_ROW};

// Operations
public:
	void	changeViewTo(dbView* theView);
	dbGUI*	getGUI() const;
	UINT	getColCount() const;

	void	enableDragDrop(long int commandID, CWnd* commandTarget);
	void	acceptDrop(unsigned int numID, long* arrayID);
	void	acceptDrop(long ID=0);

	EHighlight setHighlightType(EHighlight newHighlight);
	EHighlight getHighlightType() const;

	void	setSingleClickMessage(MessageT msg);
	void	setDoubleClickMessage(MessageT msg);

	MessageT getSingleClickMessage() const;
	MessageT getDoubleClickMessage() const;

	// disables or enables sending notification messages (single or double click)
	// to OOFile engine
	void	disableOOFileNotification();
	void	enableOOFileNotification();

	void	initializeSpace(unsigned long count);
	void	redraw();
	void	adjustNumItems();
	int		selItemRange(BOOL bSelect, int nFirstItem, int nLastItem);
	void	removeFocus();
	void	enableItemCaching(BOOL bEnable = TRUE);
	void	disableItemCaching();
	BOOL	isItemCachingEnabled() const;
	const dbView* getBrowsingView() const;
	char*	getString(int row, int subItem) const;

protected:
	void FillItems(unsigned long count, COOFProgressDlg* progressDlg);
	void RepaintSelectedItems();

// Overrides
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COOFListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LONG OnOOFileChangeSel(UINT wParam, LONG lParam);
	afx_msg LONG OnOOFileAcceptDrop(UINT wParam, LONG lParam);

	DECLARE_MESSAGE_MAP()

// Attributes
protected:
	dbView*	mView;

	MessageT mSingleClickMessage;
	MessageT mDoubleClickMessage;
	MessageT mStoredSingleClickMessage;
	MessageT mStoredDoubleClickMessage;

#ifdef _DEBUG
	bool mNotificationDisabled;
#endif

	COOFOleDropTarget mDropTarget;
	// Drag&Drop cache and handle
	COleDataSource	mOleDataSource;
	// command ID which Drag&Drop operation sends to mCommandTarget
	long int		mCommandID;
	CWnd*			mCommandTarget;
	// IDs which can be dropped into this control via drag&drop operation
	unsigned int mNumID;
	long* mArrayID;
	long  mDropID;

	dbItemCache* mItemCache;
	BOOL mItemCaching;

	EHighlight  mHighlight;		// indicates type of selection highlighting

	enum {
		BufSize=100,				// internal buffer for text description
		ProgressDlgThreshold = 300	// threshold for displaying a progress dialog
	};
};

/////////////////////////////////////////////////////////////////////////////
// COOFTreeCtrl window

/**
	Tree control providing the extra event reactions for OOFILE GUI updates.
	Also allows us to load data as required instead of having to preload a 
	huge list when browsing a big database selection.
	\ingroup oofWinSpecific oofGUI
*/
#ifdef __MWERKS__
/// bug WB1-29189 in Pro7
class OOFGUI_EXPORT COOFTreeCtrl : public CTreeCtrlEx, public OOF_NavigableList
#else
class OOFGUI_EXPORT COOFTreeCtrl : public CTreeCtrlEx, protected OOF_NavigableList
#endif
{
// Construction
public:
	COOFTreeCtrl();

// Operations
public:
	BOOL	SetDefaultImageList();
	BOOL	SetNewImageList(UINT bitmapID);

	virtual void	highlightNothing();
	virtual void	highlightRecNo(unsigned long recNo, BOOL bSelect=TRUE);
	void	ignoreChangedSelection(BOOL value = TRUE);
	virtual unsigned long highlightedRecNo() const;
	dbGUI*	getGUI() const;
	void	setHierLevels(OOF_ExpandableLongArray* hierLevels);
	void	initializeSpace();
	void	adjustNumItems();
	dbView*	viewAtLevel(unsigned short inLevel) const;

	void	enableDragDrop(long int commandID, CWnd* commandTarget);
	void	acceptDrop(unsigned int numID, long* arrayID);
	void	acceptDrop(long ID=0);

	void	setSingleClickMessage(MessageT msg);
	void	setDoubleClickMessage(MessageT msg);

	unsigned short highlightedIndentLevel() const;
	oidT	highlightedOID() const;

	dbViewOutlineItem getItemDesc(HTREEITEM item) const;

	bool	isLeafNode(HTREEITEM item) const;

protected:
	dbHierLevel*	level(unsigned short inLevel) const;
	void			FillItems(unsigned long count, dbTable* table, COOFProgressDlg* progressDlg);
	unsigned short	GetIndentLevel(HTREEITEM item) const;
	unsigned long	InsertSubItems(unsigned short indentLevel, oidT oid, HTREEITEM hTreeItem);
	void			LoadRow(dbView* view, char* buf, int bufLen);
	oidT			GetItemOID(HTREEITEM item) const;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COOFTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COOFTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(COOFTreeCtrl)
	afx_msg void OnKeyDownReflect(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LONG OnOOFileAcceptDrop(UINT wParam, LONG lParam);

	DECLARE_MESSAGE_MAP()

// Attributes
protected:
	OOF_ExpandableLongArray*	mHierLevels;
	CImageList*					mImgList;
	BOOL						mIgnoreChangedSelection;

	MessageT mSingleClickMessage;
	MessageT mDoubleClickMessage;

	COOFOleDropTarget mDropTarget;
	// Drag&Drop cache and handle
	COleDataSource	mOleDataSource;
	// command ID which Drag&Drop operation sends to mCommandTarget
	long int		mCommandID;
	CWnd*			mCommandTarget;
	// IDs which can be dropped into this control via drag&drop operation
	unsigned int mNumID;
	long* mArrayID;
	long  mDropID;

	enum {
		BufSize					= 1024,	// internal buffer for text description
		ProgressDlgThreshold	= 500	// threshold for displaying a progress dialog
	};
};

/////////////////////////////////////////////////////////////////////////////
// inline definitions

inline void
COOFListBox::setSingleClickMessage(MessageT msg)
{
	mSingleClickMessage = msg;
}
inline void
COOFListBox::setDoubleClickMessage(MessageT msg)
{
	mDoubleClickMessage = msg;
}

inline BOOL
COOFListBox::isItemCachingEnabled() const
{
	return mItemCaching;
}

inline const dbView*
COOFListBox::getBrowsingView() const
{
	return mView;
}

inline void
COOFListBox::disableItemCaching()
{
	enableItemCaching(FALSE);
}

inline BOOL
COOFListCtrl::isItemCachingEnabled() const
{
	return mItemCaching;
}

inline void
COOFListCtrl::setSingleClickMessage(MessageT msg)
{
	mSingleClickMessage = msg;
}

inline void
COOFListCtrl::setDoubleClickMessage(MessageT msg)
{
	mDoubleClickMessage = msg;
}

inline MessageT
COOFListCtrl::getSingleClickMessage() const
{
	return mSingleClickMessage;
}

inline MessageT
COOFListCtrl::getDoubleClickMessage() const
{
	return mDoubleClickMessage;
}

inline const dbView*
COOFListCtrl::getBrowsingView() const
{
	return mView;
}

inline void
COOFListCtrl::disableItemCaching()
{
	enableItemCaching(FALSE);
}

inline COOFListCtrl::EHighlight
COOFListCtrl::getHighlightType() const
{
	return mHighlight;
}

inline void
COOFTreeCtrl::setSingleClickMessage(MessageT msg)
{
	mSingleClickMessage = msg;
}

inline void
COOFTreeCtrl::setDoubleClickMessage(MessageT msg)
{
	mDoubleClickMessage = msg;
}

inline void
COOFTreeCtrl::ignoreChangedSelection(BOOL value)
{
	mIgnoreChangedSelection = value;
}

inline void
COOFTreeCtrl::highlightNothing()
{
	ClearSelection();
}

inline oidT
COOFTreeCtrl::GetItemOID(HTREEITEM item) const
{
	return GetItemData(item);
}


inline void
OOF_mixEditAttachment::addAttachment(dbGUIAdaptCWnd* inAdapter)
{
	mAdapter = inAdapter;
}


#endif
