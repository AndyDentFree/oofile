// COPYRIGHT 1995 A.D. Software, All rights reserved

#ifndef H_OOFPowerPlant
#define H_OOFPowerPlant

/**
\file oofpp.h Public interface of PowerPlant Integration layer of OOFILE database
*/

/**
	Version number of PowerPlant GUI package.
	\ingroup oofMacros
*/
#define		OOFVERS_PP	0x01400000	// Version 1.4.0
/* index to version defines 
Note that you will may lack some files depending on your package

file:        VERSION IDENTIFIER
====         ==================
oof0.h	     OOFVERS_CORE core version
oofrep.h     OOFVERS_REP report writer
oofGrphs.h   OOFVERS_GRAPH graphing engine
oofgui.h     OOFVERS_GUI  generic GUI classes used by PP, MFC and other framework integrations
oofpp.h      OOFVERS_PP  PowerPlant framework (Metrowerks CodeWarrior, on Mac)
oofmfc.h     OOFVERS_MFC MFC GUI framework

*/


// NOTE inline definitions included at end of this header file
#ifndef H_OOFGUI
	#include "oofgui.h"
#endif

// include PowerPlant definitions
#ifndef _H_PP_Types
	#include <PP_Types.h>
#endif
#ifndef _H_LAttachment
	#include <LAttachment.h>
#endif
#ifndef _H_LControl
	#include <LControl.h>  // needed so dbUshortLink knows an LControl is an LPane subclass
#endif


/**
	\def OOF_PP17_TEXTEDIT
	Support LEditField and LTextEdit obsolete classes.
	can define in advance if want to force support for a version
	so could continue to use old text edit classes in new PP
	\ingroup oofMacros
*/

/**
	\def OOF_PP18_TEXTEDIT
	Support LEditText and LTextEditView classes.
	\ingroup oofMacros
*/
#ifndef OOF_PP17_TEXTEDIT
	#if __PowerPlant__	< 0x01800000
		#define OOF_PP17_TEXTEDIT
	#endif
#endif
#ifndef OOF_PP18_TEXTEDIT
	#if __PowerPlant__	>= 0x01800000
		#define OOF_PP18_TEXTEDIT
	#endif
#endif

/**
	\name oofPPCommands PowerPlant command constants
	\ingroup oofMacros
*/
//@{

const MessageT	cmd_dbFirst			= 'dFst';	// 1682338676  MFC 0x0DB1
#define	kFirstOOFILEcmd	cmd_dbFirst
const MessageT	cmd_dbPrev			= 'dPrv';	// 1682993782  MFC 0x0DB2
const MessageT	cmd_dbNext			= 'dNxt';	// 1682864244  MFC 0x0DB3
const MessageT	cmd_dbLast			= 'dLst';	// 1682731892  MFC 0x0DB4

const MessageT	cmd_dbNew		 	= 'dNew';	// 1682859383  MFC 0x0DB5
const MessageT	cmd_dbClone			= 'dCln';	// 1682140270  MFC 0x0DB6
const MessageT	cmd_dbEdit			= 'dEdt';	// 1682269300  MFC 0x0DB7
const MessageT	cmd_dbView			= 'dVuw';	// 1683387767  MFC 0x0DB8
const MessageT	cmd_dbDel 			= 'dDel';	// 1682204012  MFC 0x0DB9
const MessageT	cmd_dbDoubleClick 	= 'd2Cl';	// 1681015660  MFC 0x0DBA
const MessageT	cmd_dbBye		 	= 'dBye';	// 1682078053  MFC 0x0DBB

//???  #define	cmd_dbSave	0x0DBC  // Mac version		= 'dSav';
//????define	cmd_dbRevert	0x0DBD  // Mac version		= 'dRev';

const MessageT	cmd_dbLockedOnEntry		 	= 'dLOE';	// 1682722629  MFC 0x0DBE
//@}

#define	kLastOOFILEcmd	cmd_dbLockedOnEntry



// forward declarations of PowerPlant classes to avoid including
class LPane;
#ifdef OOF_PP17_TEXTEDIT
	class LEditField;
	class LTextEdit;
#endif
#ifdef OOF_PP18_TEXTEDIT
	class LEditText;
	class LTextEditView;
#endif
class LCommander;
class LWindow;
class LListBox;
class LStream;
class LTextTableView;
class LCaption;
class LStdCheckBox;
class dbHelper;
class dbLinkDirtier;
class LStdPopupMenu;
class LControl;
class LPopupButton;
class LMenuController;

// family of abstract link classes

// head of family of abstract link classes
// rest are declared in oofppx.h


/**
	Adapter for PowerPlant controls eg: buttons, sliders, checkboxes.
	All PowerPlant controls descend from LControl and provide an int
	value interface like GetValue called from dbGUIAdaptLControl::getValue().
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLControl : public LAttachment, public dbGUIAdaptControl {
public:
	dbGUIAdaptLControl(LControl*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;
	virtual bool adaptsControl(void*) const;
	virtual void readOnly(bool);
	virtual void setFieldLink(dbFieldLink*);

// Control Adapter overrides
	virtual int getValue() const;
	virtual void setValue(int);
	
protected:
	virtual void	ExecuteSelf(MessageT inMessage, void *ioParam);

	LControl*	mControl;
};


/**
	Adapter for older PowerPlant popup menu.
	Exists just so we can declaratively identify popup menus.
	The older menus work through the LControl SetValue/GetValue
	interface so no other behavioural changes needed.
*/
class dbGUIAdaptLStdPopupMenu : public dbGUIAdaptLControl {
public:
	dbGUIAdaptLStdPopupMenu(LStdPopupMenu*);
	// use default copy ctor, dtor, op=

	virtual void setValueList(dbView&);  
};


/**
	Adapter for new PowerPlant popup menu.
	Exists just so we can declaratively identify popup menus.
	Still rely on the LControl SetValue/GetValue interface.
	\todo investigate generalising back to LMenuController but need to cope with buttons 
	that are just plain buttons, like LBevelButton.
*/
class dbGUIAdaptLPopupButton : public dbGUIAdaptLControl {
public:
	dbGUIAdaptLPopupButton(LPopupButton*);
	// use default copy ctor, dtor, op=

	virtual void setValueList(dbView&);  

protected:
	LMenuController*	mMenuController;
};


// concrete adapters
// these must be declared at a public level so they are automatically used
// in linkField calls

/**
	Adapter for PowerPlant edit controls.
	All PowerPlant edit controls descend from LPane.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptEditPane : public dbGUIAdaptEditor, public LAttachment  {
// anything that interacts with an LPane based control inherits from this
public:
	dbGUIAdaptEditPane(LPane*,LCommander* editCommander=0);
	
// generic Adapter overrides
	virtual bool adaptsControl(void*) const;
	virtual void readOnly(bool);
	virtual void highlightContents();
	virtual void setFieldLink(dbFieldLink*);
	// provide string interfaces for all LPane derivatives using get/setDescriptor
	virtual oofString getString() const;
	virtual void setString(const oofString&);
	virtual bool equalsString(const oofString&) const;
	
protected:
	virtual void	ExecuteSelf(MessageT inMessage, void *ioParam);

	LPane*	mPane;
	LCommander*	mCommander;
};


/**
	Adapter for PowerPlant edit controls for more than 255 chars.
	Common base adapting to editors that provide a GetTextHandle, SetTextPtr interface.
	Typically used to edit multiple lines.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLargeEditPane : public dbGUIAdaptEditPane  {
public:
	dbGUIAdaptLargeEditPane(LPane*,LCommander* editCommander=0);
	
// generic Adapter overrides
	virtual oofString getString() const;
	virtual void setString(const oofString&);
	virtual bool equalsString(const oofString&) const;

protected:
	virtual Handle GetTextHandle() const=0;
	virtual void  SetTextPtr(const oofString&) const=0;
};


/**
	Adapter for PowerPlant static text LCaption.
	We still want to interface static text display to the database, eg:
	used for displaying calculated fields. 
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLCaption : public dbGUIAdaptEditPane {
public:
	dbGUIAdaptLCaption(LCaption*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;

/// hide the editable behaviour and use base 
	virtual void readOnly(bool becomeReadOnly) { dbGUIAdaptEditor::readOnly(becomeReadOnly); };
	virtual void setFieldLink(dbFieldLink* inLink);
};


#ifdef OOF_PP17_TEXTEDIT

/**
	Adapter for the obsolete PowerPlant LTextEdit.
	Only available if #define OOF_PP17_TEXTEDIT
	LTextEditView replaces LTextEdit.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLTextEdit : public dbGUIAdaptLargeEditPane {  // OOF_PP17_TEXTEDIT
public:
	dbGUIAdaptLTextEdit(LTextEdit*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;

protected:
	virtual Handle GetTextHandle() const;
	virtual void  SetTextPtr(const oofString&) const;
};


/**
	Adapter for the obsolete PowerPlant LEditField.
	Only available if #define OOF_PP17_TEXTEDIT.
	LEditText relaces LEditField.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLEditField : public dbGUIAdaptEditPane {  // OOF_PP17_TEXTEDIT
public:
	dbGUIAdaptLEditField(LEditField*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;

	virtual void setTextEntryLimit(unsigned long);
};
#endif


#ifdef OOF_PP18_TEXTEDIT

/**
	Adapter for the PowerPlant LEditField.
	Only available if #define OOF_PP18_TEXTEDIT which is on by default.
	LEditText relaced the obsolete LEditField.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLEditText : public dbGUIAdaptLargeEditPane {  // OOF_PP18_TEXTEDIT
public:
	dbGUIAdaptLEditText(LEditText*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;

	virtual void setTextEntryLimit(unsigned long);

protected:
	virtual Handle GetTextHandle() const;
	virtual void  SetTextPtr(const oofString&) const;
};


/**
	Link a PowerPlant LTextEditView to an OOFILE field.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIAdaptLTextEditView : public dbGUIAdaptLargeEditPane {  // OOF_PP18_TEXTEDIT
public:
	dbGUIAdaptLTextEditView(LTextEditView*);
	// use default copy ctor, dtor, op=

// generic Adapter overrides
	virtual dbGUIAdapter* clone() const;

protected:
	virtual Handle GetTextHandle() const;
	virtual void  SetTextPtr(const oofString&) const;
};
#endif



class dbTable;
class dbChar;
class dbText;
class dbLong;
class dbUlong;
class dbShort;
class dbUshort;
class dbBool;
class dbDate;
class dbDateTime;
class dbReal;
class dbView;
class dbTableDisplay;
class dbBrowseHelper;


/**
	Concrete implementation for PowerPlant.
	\ingroup oofGUI  oofMacSpecific
*/
class dbBrowseHelper : public dbAbstractBrowseHelper {
public:
	dbBrowseHelper(dbView* adoptedView);
	virtual ~dbBrowseHelper();

// abstract overrides
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual dbView* view() const; // topmost view
	virtual unsigned long	highlightedRecNo() const;
	virtual dbSelection	highlightedSelection() const;
	virtual void highlightNothing() const;
	virtual void 	highlightRecNo(unsigned long, bool bSelect=TRUE) const;
	virtual void updateBrowserSelection() const;
	virtual void 	refreshBrowser();	

	void	adoptView(dbView*);
	void	browseViewWithListBox(LListBox*);
	void	browseViewWithTable(LTextTableView*);
	
	oidT	highlightedOID() const;
	void 	gotoHighlightedRecord() const;
	void	appendSelection(const dbSelection&);
	void	removeSelection(const dbSelection&);
	void	clearHighlightedSelection();
	void	clearSelection();
	dbSelection	currentSelection() const;
	
protected:
	
	void 	LoadBrowseData();

private:
  	dbView*  mBrowsingView;  ///< adopted
	dbTableDisplay* mBrowserDisplay;  ///< owned
	unsigned short mHeadingRows;
};


/**
	Simplify dbEditHelper by creating correct Adapter based on control type.
	The correct constructor of this class will be invoked and make an
	adapter, rather than having to declare an ever-expanding list of classes.
	Copying uses default copy ctor, copying mAdapter.
	We use default dtor, so don't delete our mAdapter
*/
class dbGUIAdapterFactoryPP {
public:
	dbGUIAdapterFactoryPP(LCaption*);
	dbGUIAdapterFactoryPP(LControl*);
	dbGUIAdapterFactoryPP(LStdPopupMenu*);
	dbGUIAdapterFactoryPP(LPopupButton*);
	
#ifdef OOF_PP18_TEXTEDIT
	dbGUIAdapterFactoryPP(LEditText*);
	dbGUIAdapterFactoryPP(LTextEditView*);
#endif
#ifdef OOF_PP17_TEXTEDIT
	dbGUIAdapterFactoryPP(LEditField*);
	dbGUIAdapterFactoryPP(LTextEdit*);
#endif

	dbGUIAdapter* orphanAdapter() const;
	bool defaultBecomeReadOnly() const;
private:
	dbGUIAdapter* mAdapter;  ///< not owned as we're just a factory
	bool	mBecomeReadOnlyByDefault; ///< set depending on control type
};


/**
	dbHelper that manages edit controls accessing a main record.
	May have related data to the main table being edited.
	There will be one of these per dialog editing a record except possibly
	if the only controls on the dialog are lists of records.
	This copy of the class implements PowerPlant linkField methods.
	dbEditHelper will be reimplemented for each GUI platform and has linkField
	classes for that GUI's different control types.
	\ingroup oofGUI oofMacSpecific
*/
class dbEditHelper : public dbAbstractEditHelper {
public:
	dbEditHelper(dbTable*);

	virtual void* currentGUItarget() const;

/// \name platform-specific links
//@{
	void linkField(dbField&, const dbGUIAdapterFactoryPP&);
	void linkField(dbField*, const dbGUIAdapterFactoryPP&);
	void linkPickField(dbField&, const dbGUIAdapterFactoryPP&, dbRelRef* pickVia=0);
	void linkPickField(dbField*, const dbGUIAdapterFactoryPP&, dbRelRef* pickVia=0);
//@}
		
protected:		
	virtual bool SendFrameworkMessage(OOFmsgT, void* ioParam=0);
};



// -------------------------------------------------------
//    O B S O L E T E   L I N K   C L A S S E S
// -------------------------------------------------------
// these classes exist because users have directly used them
// in previous versions of the OOFILE GUI

/**
	Link a dbUshort field to a LControl eg: checkbox.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	when we moved to the generic structure.
	\see linkField(dbField*, LControl*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbUshortLink : public dbFieldLink {
public:
	dbUshortLink(dbUshort* fld, LControl* ctl, bool becomeReadOnly) :
		dbFieldLink(fld,  new dbGUIAdaptLControl(ctl), becomeReadOnly),
		mLinksTo(ctl)
	{
	};
	dbUshortLink(dbUshort* fld, LControl* ctl, bool becomeReadOnly, bool /*disableReadOnlyPanes*/) :
		dbFieldLink(fld,  new dbGUIAdaptLControl(ctl), becomeReadOnly),
		mLinksTo(ctl)
	{
	};
	dbUshort* field() const { return (dbUshort*) mField; };
	LPane* linksTo() const { return mLinksTo; };

	void createDirtySensor(LPane*) const {};  // null method
	
protected:
	LPane* mLinksTo;  ///< use this for obsolete subclass, ignoring adapters
};

#ifdef OOF_PP17_TEXTEDIT
/**
	Link a dbChar field to a LEditField for small text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LEditField*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbCharLink2EditField : public dbFieldLink {
public:
	dbCharLink2EditField(dbChar* fld, LEditField* ctl, bool becomeReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptLEditField(ctl), becomeReadOnly)
	{};
};


/**
	Link a dbReal field to a LEditField for small text entry of numbers.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LEditField*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbRealLink2EditField : public dbNumericFieldLink {
public:
	dbRealLink2EditField(dbReal* fld, LEditField* ctl, bool becomeReadOnly=false) :
		dbNumericFieldLink(fld, new dbGUIAdaptLEditField(ctl), becomeReadOnly)
	{};
};


/**
	Link a dbShort field to a LEditField for small text entry of numbers.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LEditField*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbShortLink2EditField : public dbNumericFieldLink {
public:
	dbShortLink2EditField(dbShort* fld, LEditField* ctl, bool becomeReadOnly=false) :
		dbNumericFieldLink(fld, new dbGUIAdaptLEditField(ctl), becomeReadOnly)
	{};
};


/**
	Link a dbChar field to a LTextEdit for larger text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LTextEdit*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbCharLink2TextEdit : public dbFieldLink {
public:
	dbCharLink2TextEdit(dbChar* fld, LTextEdit* ctl, bool becomeReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptLTextEdit(ctl), becomeReadOnly)
	{};
};



/**
	Link a dbText field to a LTextEdit for larger text entry.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LTextEdit*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbTextLink2TextEdit : public dbFieldLink {
public:
	dbTextLink2TextEdit(dbText* fld, LTextEdit* ctl, bool becomeReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptLTextEdit(ctl), becomeReadOnly)
	{};
};



/**
	Link a dbText field to a LCaption for readonly display.
	\note Concrete link used directly by name in previous OOFILE apps and so reimplemented
	this interface when we moved to the generic structure.
	\see linkField(dbField*, LCaption*) generic link implementation.
	\ingroup oofGUI oofMacSpecific
*/
class dbTextLink2Caption : public dbFieldLink {
public:
	dbTextLink2Caption(dbText* fld, LCaption* ctl, bool becomeReadOnly=false) :
		dbFieldLink(fld, new dbGUIAdaptLCaption(ctl), becomeReadOnly)
	{};
};


#endif // OOF_PP17_TEXTEDIT


// -------------------------------------------------------
//       d e b u g g i n g    t o o l s
// -------------------------------------------------------


/**
	Class wrapper aroung MessageT so can write with stream IO.
	manually expand what we could do with the templates in iomanip.h
	without requiring iomanip.h to be included at this point
	\note can't write an overloaded operator<< on MessageT directly
	as it's a typedef, so turn it into a class!
	\ingroup oofMacSpecific oofUtils
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
inline 
dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LCaption* ctl) :
	mAdapter(new dbGUIAdaptLCaption(ctl)),
		mBecomeReadOnlyByDefault(true)
{}


inline
dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LControl* ctl) :
	mAdapter(new dbGUIAdaptLControl(ctl)),
		mBecomeReadOnlyByDefault(false)
{}


inline
dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LStdPopupMenu* ctl) :
	mAdapter(new dbGUIAdaptLStdPopupMenu(ctl)),
		mBecomeReadOnlyByDefault(false)
{}


inline
dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LPopupButton* ctl) :
	mAdapter(new dbGUIAdaptLPopupButton(ctl)),
		mBecomeReadOnlyByDefault(false)
{}


#ifdef OOF_PP18_TEXTEDIT
	inline
	dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LEditText* ctl) :
		mAdapter(new dbGUIAdaptLEditText(ctl)),
		mBecomeReadOnlyByDefault(false)
	{}
	
	
	
	inline
	dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LTextEditView* ctl) :
		mAdapter(new dbGUIAdaptLTextEditView(ctl)),
		mBecomeReadOnlyByDefault(false)
	{}
#endif
#ifdef OOF_PP17_TEXTEDIT
	inline
	dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LEditField* ctl) :
		mAdapter(new dbGUIAdaptLEditField(ctl)),
		mBecomeReadOnlyByDefault(false)
	{}
	
	
	inline
	dbGUIAdapterFactoryPP::dbGUIAdapterFactoryPP(LTextEdit* ctl) :
		mAdapter(new dbGUIAdaptLTextEdit(ctl)),
		mBecomeReadOnlyByDefault(false)
	{}
#endif

inline dbGUIAdapter* 
dbGUIAdapterFactoryPP::orphanAdapter() const
{
	return mAdapter;
}

inline bool 
dbGUIAdapterFactoryPP::defaultBecomeReadOnly() const
{
	return mBecomeReadOnlyByDefault;
}


// -------------------------------------------------------
//              d b E d i t H e l p e r
// -------------------------------------------------------
inline void 
dbEditHelper::linkField(dbField& fld, const dbGUIAdapterFactoryPP& inFactory)
{
	linkAdapter(&fld, inFactory.orphanAdapter(), inFactory.defaultBecomeReadOnly());
}


inline void 
dbEditHelper::linkField(dbField* fld, const dbGUIAdapterFactoryPP& inFactory)
{
	linkAdapter(fld, inFactory.orphanAdapter(), inFactory.defaultBecomeReadOnly());
}


inline void 
dbEditHelper::linkPickField(dbField& fld, const dbGUIAdapterFactoryPP& inFactory, dbRelRef* pickVia)
{
	linkPickAdapter(&fld, inFactory.orphanAdapter(), inFactory.defaultBecomeReadOnly(), pickVia);
}


inline void 
dbEditHelper::linkPickField(dbField* fld, const dbGUIAdapterFactoryPP& inFactory, dbRelRef* pickVia)
{
	linkPickAdapter(fld, inFactory.orphanAdapter(), inFactory.defaultBecomeReadOnly(), pickVia);
}

#endif

