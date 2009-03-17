// COPYRIGHT 1995 A.D. Software, All rights reserved

// Public interface of MFC Integration layer of OOFILE database

#ifndef H_OOFMFC
#define H_OOFMFC

/**
	Version number of MFC GUI package.
	\ingroup oofMacros
*/
#define		OOFVERS_MFC	0x01400000	// Version 1.4.0
/* index to version defines 
Note that you will may lack some files depending on your package

file:        VERSION IDENTIFIER
====         ==================
oof0.h	    OOFVERS_CORE core version
oofrep.h     OOFVERS_REP report writer
oofGrphs.h   OOFVERS_GRAPH graphing engine
oofgui.h     OOFVERS_GUI  generic GUI classes used by PP, MFC and other framework integrations
oofpp.h      OOFVERS_PP  PowerPlant framework (Metrowerks CodeWarrior, on Mac)
oofmfc.h     OOFVERS_MFC MFC GUI framework

*/

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#include "oofmfcct.h"
#include "oofync.h"
#include "oofios.h"

// COPYRIGHT 1995 A.D. Software, All rights reserved

// MFC Integration layer of OOFILE database

// NOTE inline definitions included at end of this header file
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif

#ifndef _H_CWndRedir
	#include "CWndRedir.h"
#endif

#ifndef H_oofCmds
	#include "oofcmds.h"
#endif

#ifndef H_OOFGUI
	#include "oofgui.h"
#endif

/**
	Concrete implementation for MFC.
	\ingroup oofGUI  oofMWinSpecific
*/
class OOFGUI_EXPORT dbBrowseHelper : public dbAbstractBrowseHelper {
public:
	dbBrowseHelper(dbView* adoptedView); 
	virtual ~dbBrowseHelper();

// abstract overrides
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual unsigned long highlightedRecNo() const;
	virtual void highlightRecNo(unsigned long inRecNo, bool bSelect=TRUE) const;
	virtual void highlightNothing() const;
	virtual dbSelection	highlightedSelection() const;
	virtual void updateBrowserSelection() const;
	virtual dbView* view() const;
	virtual void refreshBrowser();
	
	void	adoptView(dbView*);
	// overload with reference parameter for compatibility with AppMaker gen code
	void	browseViewWithListBox(COOFListBox*);
	void	browseViewWithListBox(COOFListBox& box) { browseViewWithListBox(&box); };
	void	browseViewWithListCtrl(COOFListCtrl*);
	void	browseViewWithListCtrl(COOFListCtrl& ctrl) { browseViewWithListCtrl(&ctrl); };

	oidT	highlightedOID() const;
	void gotoHighlightedRecord() const;
	void	appendSelection(const dbSelection&);
	void	removeSelection(const dbSelection&);
	void	clearHighlightedSelection();
	void	clearSelection();
	dbSelection	currentSelection() const;

protected:
	void 	LoadBrowseData();
	void	CheckControlStyle(COOFListCtrl*);
	void	CheckControlStyle(COOFListBox*);
	void	CheckControlStyle(COOFTreeCtrl*);

private:
  	dbView*  mBrowsingView;  // adopted
	dbTableDisplay* mBrowserDisplay;  // owned
	unsigned short mHeadingRows;
};


/**
	dbHelper that manages edit controls accessing a main record.
	May have related data to the main table being edited.
	There will be one of these per dialog editing a record except possibly
	if the only controls on the dialog are lists of records.
	This copy of the class implements MFC linkField methods.
	dbEditHelper will be reimplemented for each GUI platform and has linkField
	classes for that GUI's different control types.
	\ingroup oofGUI oofWinSpecific
*/
class OOFGUI_EXPORT dbEditHelper : public dbAbstractEditHelper {
public:
	dbEditHelper(dbTable*);
	
	virtual bool SendFrameworkMessage(OOFmsgT, void* ioParam=0);
	virtual bool SendImmediateFrameworkMessage(OOFmsgT, void* ioParam=0);

	virtual void* currentGUItarget() const;

	dbFieldLink* getLink(CWnd* W){  return dbAbstractEditHelper::getLink(W); };	;	// retrieve manufactured links for given user control
	dbFieldLink* getLink(CWndRedir* W) { return dbAbstractEditHelper::getLink(W->Wnd()); };	


// platform-specific links
	void linkField(dbField&, COOFComboBox*);
	void linkField(dbField&, COOFComboBox&);
	void linkField(dbField&, COOFEdit*);
	void linkField(dbField&, COOFEdit&);
	void linkField(dbField&, COOFCheckBox*);
	void linkField(dbField&, COOFCheckBox&);
	void linkField(dbField&, CStatic*);
	void linkField(dbField&, CStatic&);
	
	void linkField(dbField*, COOFComboBox*);
	void linkField(dbField*, COOFComboBox&);
	void linkField(dbField*, COOFEdit*);
	void linkField(dbField*, COOFEdit&);
	void linkField(dbField*, COOFCheckBox*);
	void linkField(dbField*, COOFCheckBox&);
	void linkField(dbField*, CStatic*);
	void linkField(dbField*, CStatic&);
	
protected:
	LRESULT SendMessageToDescendantsStoppingAtFirst(
						HWND hWnd,
						UINT message,
						WPARAM wParam=0, LPARAM lParam=0,
						BOOL bDeep=TRUE,
						BOOL bOnlyPerm=FALSE);
	
public:
	static bool legalCmdMsg(UINT);
	static bool postFrameworkMessage(UINT msg, LPARAM lParam=0);
};


/**
	Adaptor for MFC controls including edit panes.
	\ingroup oofGUI  oofMWinSpecific
*/
class dbGUIAdaptCWnd : public dbGUIAdaptEditor {
public:
	dbGUIAdaptCWnd(CWnd*);
	dbGUIAdaptCWnd(CWnd*, OOF_mixEditAttachment*);  // if people want to have two classes!
	// use default copy ctor, dtor, op=
	virtual dbGUIAdapter* clone() const;

/// \name generic Adapter overrides
//@{
	virtual bool adaptsControl(void*) const;
	virtual void readOnly(bool);
	virtual void highlightContents();
	virtual void setFieldLink(dbFieldLink*);
	virtual oofString getString() const;
	virtual void setString(const oofString&);
//@}

/// \name Windows support
//@{
	virtual void OnMessage(UINT message, WPARAM, LPARAM) ;
	void OnKillFocus(CWnd* pNewWnd) ;
//@}
	
protected:
	CWnd* mWnd;
	OOF_mixEditAttachment* mAttachableWnd;  // may NOT be same object as mWnd
};


/**
	Adaptor for the MFC CEdit.
	\ingroup oofGUI oofWinSpecific
*/
class dbGUIAdaptCEdit : public dbGUIAdaptCWnd {
public:
	dbGUIAdaptCEdit(CWnd*);
	// use default copy ctor, dtor, op=
	virtual dbGUIAdapter* clone() const;

// generic Adapter overrides
	virtual void readOnly(bool);
};


/**
	Adaptor for the MFC CButton which includes checkboxes & radio buttons.
	\ingroup oofGUI oofWinSpecific
*/
class dbGUIAdaptCButton : public dbGUIAdaptCWnd {
public:
	dbGUIAdaptCButton(CWnd*);
	// use default copy ctor, dtor, op=
	virtual dbGUIAdapter* clone() const;

// generic Adapter overrides
	virtual oofString getString() const;
	virtual void setString(const oofString&);
	virtual bool equalsString(const oofString&) const;
	
// Control Adapter overrides
	virtual int getValue() const;
	virtual void setValue(int);

// Windows support
	virtual void OnMessage(UINT message, WPARAM, LPARAM) ;
};


/**
	Adaptor for the MFC CCombo which manages popup lists.
	\ingroup oofGUI oofWinSpecific
*/
class dbGUIAdaptCCombo : public dbGUIAdaptCWnd {
public:
	dbGUIAdaptCCombo(CWnd*);
	// use default copy ctor, dtor, op=
	virtual dbGUIAdapter* clone() const;

// Windows support
	virtual void OnMessage(UINT message, WPARAM, LPARAM) ;
};


// -------------------------------------------------------
//    O B S O L E T E   L I N K   C L A S S E S
// -------------------------------------------------------
// these classes exist because users have directly used them
// in previous versions of the OOFILE GUI

/**
	Link a dbUshort field to a CButton eg: checkbox.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbUshortLink : public dbFieldLink {
public:
	dbUshortLink(dbUshort* fld, CWnd* ctl, bool isReadOnly) :
		dbFieldLink(fld,  new dbGUIAdaptCWnd(ctl), isReadOnly),
		mLinksTo(ctl)
	{
	};
	dbUshortLink(dbUshort* fld, CWnd* ctl, bool isReadOnly, bool /*disableReadOnlyPanes*/) :
		dbFieldLink(fld,  new dbGUIAdaptCWnd(ctl), isReadOnly),
		mLinksTo(ctl)
	{
	};
	dbUshort* field() const { return (dbUshort*) mField; };
	CWnd* linksTo() const { return mLinksTo; };

	void createDirtySensor(CWnd*) const {};  // null method
	
protected:
	CWnd* mLinksTo;  // use this for obsolete subclass, ignoring adaptors
};


/**
	Link a dbChar field to a CEdit for small text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbCharLink2EditField : public dbFieldLink {
public:
	dbCharLink2EditField(dbChar* fld, COOFEdit* ctl, bool isReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptCEdit(ctl), isReadOnly)
	{};
	dbCharLink2EditField(dbChar* fld, COOFComboBox* ctl, bool isReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptCCombo(ctl), isReadOnly)
	{};
};


/**
	Link a dbReal field to a CEdit for small text entry of numbers.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbRealLink2EditField : public dbNumericFieldLink {
public:
	dbRealLink2EditField(dbReal* fld, COOFEdit* ctl, bool isReadOnly=false) :
		dbNumericFieldLink(fld, new dbGUIAdaptCEdit(ctl), isReadOnly)
	{};
};


/**
	Link a dbShort field to a CEdit for small text entry of numbers.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
	\endif
*/
class dbShortLink2EditField : public dbNumericFieldLink {
public:
	dbShortLink2EditField(dbShort* fld, COOFEdit* ctl, bool isReadOnly=false) :
		dbNumericFieldLink(fld, new dbGUIAdaptCEdit(ctl), isReadOnly)
	{};
};


/**
	Link a dbChar field to a COOFEdit for larger text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbCharLink2TextEdit : public dbFieldLink {
public:
	dbCharLink2TextEdit(dbChar* fld, COOFEdit* ctl, bool isReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptCEdit(ctl), isReadOnly)
	{};
};


/**
	Link a dbText field to a COOFEdit for larger text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbTextLink2TextEdit : public dbFieldLink {
public:
	dbTextLink2TextEdit(dbText* fld, COOFEdit* ctl, bool isReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptCEdit(ctl), isReadOnly)
	{};
};


/**
	Link a dbText field to a CStatic for readonly display.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbTextLink2Caption : public dbFieldLink {
public:
	dbTextLink2Caption(dbText* fld, CStatic* ctl) :
		dbFieldLink(fld, new dbGUIAdaptCEdit(ctl), true /* readonly */)
	{};
};


/**
	Link a dbChar field to a CStatic for readonly display.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, CWnd*) generic link implementation.
	\ingroup oofGUI oofWinSpecific
*/
class dbCharLink2Caption : public dbFieldLink {
public:
	dbCharLink2Caption(dbChar* fld, CStatic* ctl) :
		dbFieldLink(fld, new dbGUIAdaptCEdit(ctl), true /* readonly */)
	{};
};

// -------------------------------------------------------
//       d e b u g g i n g    t o o l s
// -------------------------------------------------------

/**
	Class wrapper aroung MessageT so can write with stream IO.
	manually expand what we could do with the templates in iomanip.h
	without requiring iomanip.h to be included at this point
	\note can't write an overloaded operator<< on MessageT directly
	as it's a typedef, so turn it into a class!
	\ingroup oofWinSpecific oofUtils
*/
class PPMessage {
public:
	PPMessage(MessageT	m) : msg(m) {}; 
	MessageT	msg;
};


ostream& operator<<(ostream&, PPMessage&);


// -------------------------------------------------------
//              d b E d i t H e l p e r
// -------------------------------------------------------
inline void 
dbEditHelper::linkField(dbField& fld, COOFComboBox* ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCCombo(ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, COOFComboBox& ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCCombo(&ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, COOFEdit* ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCEdit(ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, COOFEdit& ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCEdit(&ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, COOFCheckBox* ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCButton(ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, COOFCheckBox& ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCButton(&ctl));
}


inline void 
dbEditHelper::linkField(dbField& fld, CStatic* ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCEdit(ctl), true /*isReadOnly*/);
}


inline void 
dbEditHelper::linkField(dbField& fld, CStatic& ctl)
{
	linkAdapter(&fld, new dbGUIAdaptCEdit(&ctl), true /*isReadOnly*/);
}

inline void 
dbEditHelper::linkField(dbField* fld, COOFComboBox* ctl)
{
	linkAdapter(fld, new dbGUIAdaptCCombo(ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, COOFComboBox& ctl)
{
	linkAdapter(fld, new dbGUIAdaptCCombo(&ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, COOFEdit* ctl)
{
	linkAdapter(fld, new dbGUIAdaptCEdit(ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, COOFEdit& ctl)
{
	linkAdapter(fld, new dbGUIAdaptCEdit(&ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, COOFCheckBox* ctl)
{
	linkAdapter(fld, new dbGUIAdaptCButton(ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, COOFCheckBox& ctl)
{
	linkAdapter(fld, new dbGUIAdaptCButton(&ctl));
}


inline void 
dbEditHelper::linkField(dbField* fld, CStatic* ctl)
{
	linkAdapter(fld, new dbGUIAdaptCEdit(ctl), true /*isReadOnly*/);
}


inline void 
dbEditHelper::linkField(dbField* fld, CStatic& ctl)
{
	linkAdapter(fld, new dbGUIAdaptCEdit(&ctl), true /*isReadOnly*/);
}

#endif // _Windows
#endif
