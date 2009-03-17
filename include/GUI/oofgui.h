// COPYRIGHT 1999 A.D. Software, All rights reserved


#ifndef H_OOFGUI
#define H_OOFGUI

/**
	Version number of generic GUI package.
	\ingroup oofMacros
*/
#define		OOFVERS_GUI	0x01400000	// Version 1.4.0
/**
	\file oofgui.h 
	Platform-independent GUI Integration layer of OOFILE database.
	
	index to version defines 
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
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif

enum YNCancelT {Yes, No, Cancel};

// forward decls
class dbFieldLink;
class dbRelRef;
class dbHelper;
class dbAbstractBrowseHelper;
class dbAbstractEditHelper;

// define MessageT
#ifdef _Macintosh
	#ifndef _H_PP_Types
		#include <PP_Types.h>
	#endif
// otherwise already defined in oofmsh.h
#elif defined _Windows
// here because used Boolean in some common GUI interfaces
		typedef bool Boolean;  // but define compatibly anyway
#endif

/**
	Links a dbField to a known control type or I/O mechanism.
	Subclass to provide an interface between OOFILE and your own GUI control.
	Makes no assumptions about control class - controls do not have to be 
	part of any existing view hierarchy.
	Maps all values to string or int to communicate with the field.
	\note whilst we implement dbGUIAdapter subclasses to talk to controls
	and users will normally do so, it's a generic interface and could hide
	things like a serial port to a hardware button.
	\see dbFieldLink
	\todo map other binary types without going via string
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbGUIAdapter{
public:
	dbGUIAdapter();
	virtual ~dbGUIAdapter() {};
	// use default copy ctor, op=
	
	dbFieldLink* fieldLink() const;  ///< in case someone needs to access it

	bool isValuePicker() const;
	virtual void setValueList(dbView&);  

/// \name generic Adapter mandatory overrides
//@{
	virtual dbGUIAdapter* clone() const=0;
	virtual oofString getString() const=0;	
	virtual void setString(const oofString&)=0;
//@}
	
/// \name generic Adapter optional overrides
//@{
	virtual void highlightContents() {};
	virtual void readOnly(bool) {};  // optional override - may choose to not be able to toggle this state
	virtual void setFieldLink(dbFieldLink*);
	virtual bool equalsString(const oofString&) const; // only override to be more efficient
	virtual bool adaptsControl(void*) const;
//@}

/// \name Control Adapter overrides- defaults defined in terms of string methods
//@{
	virtual int getValue() const;
	virtual void setValue(int);
	virtual bool equalsValue(int) const;
//@}

protected:
	bool	IsReadOnly() const;
	
// data storage
	dbFieldLink*	mFieldLink;	///< reference, maybe null
	bool mIsValuePicker; ///< set at construction to indicate behaviour is picking from list vs entry
};


/**
	Abstract base for Controls.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbGUIAdaptControl : public dbGUIAdapter {
public:

/// \name generic Adapter mandatory overrides
//@{
	virtual oofString getString() const;
	virtual void setString(const oofString&);	
//@}

/// \name generic Adapter optional overrides
//@{
	virtual bool equalsString(const oofString&) const;
//@}
};



/**
	base for any text editing Adapters.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbGUIAdaptEditor : public dbGUIAdapter {
public:
	virtual void editingOccurred();  ///< signals changes to anyone who needs to know
	virtual void setTextEntryLimit(unsigned long);
};


/**
	dbFieldLink is the main link abstraction mapping a database field to
	a GUI object (via a dbGUIAdapter). This two-stage mapping is necessary
	so we can provide overrides at either end without a combinatorial explosion
	from mixing field and GUI control types. (As early OOFILE suffered.)
	
	In Pattern terms, dbFieldLink provides a Facade covering a whole range
	of field access. dbGUIAdapter is our Adapter for control I/O interfacing.
	
	\warning most of the methods of dbFieldLink operate under the original 
	assumptions that the field link has been initialised with both a dbField*
	and dbGUIAdapter*.
	
	Report editing may subclass and use with NULL values for either of these, 
	but uses only a subset of features.
	
	We implement read-only in OOFILE rather than relying on application level controls
	as some frameworks (eg: PowerPlant) may not provide readonly behaviour without 
	having to totally disable a control. If you want to retain select/copy/paste of
	a control, disabling is not suitable.
	
	dbFieldLink retains the setting and can be queried by isReadOnly.
	Implementing the behaviour to inhibit editing is the job of the dbGUIAdapter
	eg, in PowerPlant dbGUIAdaptEditPane::ExecuteSelf swallows msg_KeyPress.
	
	\note it is quite straightforward to use your own subclasses of dbFieldLink.
	Rather than using dbEditHelper::linkField to link the control and dbField, 
	create your own link and give it to the helper:
	\code
	mEditHelper->adoptLink(new YourLink(...));
	\endcode
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbFieldLink : public OOF_PublicBase {
public:
	dbFieldLink(dbField*, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly=false);
	dbFieldLink(const dbFieldLink&);
	virtual ~dbFieldLink();
	dbHelper* owner() const;	
	void owner(dbHelper*);	///< not ctor param as can create without helper

/// \name mainly used by dbGUIadaptor
//@{
	void maybeChangeTargetLink();	
	void maybeDirty();
	virtual void tellGUIcareAboutLocking() const;
//@}

/// \name mainly used by dbEditHelper
//@{
	virtual void copyFieldToLink();
	virtual void copyLinkToField();
	virtual bool linkEqualsField() const;
	virtual bool leaveLink();
	virtual void updateDerivedFieldDisplay(bool isNew);
	virtual void readOnly(bool isReadOnly=true);

	virtual bool validToLeaveLink();
	virtual bool validToLeaveForm();
	
	virtual void highlightLink();
	void markClean();
//@}
	
/// \name reflective calls
//{@
	bool isReadOnly() const;
	bool alwaysReadOnly() const;
	bool isDirty() const;

	bool linkIsEmpty() const;
	dbGUIAdapter* adapter() const;

//@}

	dbField* field() const; ///< purposely non-virtual as subclasses return different types
	void setField(dbField* inField);
	

private:
	void PreApprovedReadOnly(bool isReadOnly);
	
// data storage	
protected:	
	dbField*	mField;
	dbGUIAdapter*	mAdapter;		///< owned
	dbHelper* mOwner;
	bool	mMaybeDirty;	///< tentative flag lets us optimise comparisons
	bool	mReadOnly;			///< default is read-write
	bool	mAlwaysReadOnly;	///< for static controls, etc;

friend class dbAbstractEditHelper;  ///< so it can call PreApprovedReadOnly
};

/**
	dbPickFieldLink maps selection of related records via a control linked to a field.
	
	\param  	
	\param becomeReadOnly is probably irrelevant but maybe we might have an entry
	field that is temporarily readonly depending on user state.
	
	dbFieldLink retains the setting and can be queried by isReadOnly.
	Implementing the behaviour to inhibit editing is the job of the dbGUIAdapter
	eg, in PowerPlant dbGUIAdaptEditPane::ExecuteSelf swallows msg_KeyPress.
	
	\warning the big conceptual difference with dbFieldPickLink is that it separates
	the target field (the dbRelRefBase) being set by user interaction from the field which 
	provides data entry characteristics, eg: a dbChar providing menu titles. 
	Normally these are the same field. 
	
	\todo cope with text entry fields used as pickers, suspect may need to know type of adapter
	\todo just copy the entire table into RAM lookup map for speed.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbFieldPickLink : public dbFieldLink {
public:
	dbFieldPickLink(dbField*, dbGUIAdapter* adoptedAdapter, 
		bool becomeReadOnly, dbRelRef* pickVia);
	dbFieldPickLink(const dbFieldPickLink&);
	virtual ~dbFieldPickLink();

/// \name mainly used by dbEditHelper
//@{
	virtual void copyFieldToLink();
	virtual void copyLinkToField();
	virtual bool linkEqualsField() const;
	virtual bool leaveLink();
	virtual bool validToLeaveForm();
//@}

	bool allowsNullRelationship() const;
	void allowNullRelationship(bool inAllowFlag=true);
	void setDefaultPick(int);
	
protected:
	void InitLookup();
	
	bool mAllowsNullRelationship;  ///< OK to not have lookup value	
	dbRelRef* mPickVia;  ///< only allow Ref as lookups point to just one record
	dbTable* mLookupTable;  ///< owned clone
	unsigned long mCachedRecordNumber; ///< used for popups to optimise comparisons
	int mDefaultPick;
};



/**
	Abstract manager coordinating dbHelper objects that manage interaction on a window.
	Maps between framework-specific actions and the OOFILE equivalents, eg: providing
	stepping through records.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbGUI : public oofBroadcaster {
public:
	dbGUI(dbTable*);
	void gotoCurrentOrNewRec();
	bool leaveRecord();

	void newRecord();	
	void saveRecord();	
	void deleteRecord();	
	void revertRecord();	
	void cloneRecord();	
	void goFirstRecord();
	void goNextRecord();
	void goPrevRecord();
	void goLastRecord();
	
	dbTable* mainTable() const;


/// \name locking
//@{
	bool recordAvailable() const;
	void dontCareIfLocked();
	void careIfLocked();
//@}

protected:
	bool ConfirmDelete(const char* confirmMessage=0);
	bool ConfirmRevert();
	dbAbstractBrowseHelper* GetDisplayTableHandler(void* ioParam);
	
private:
	bool mClosing;	///<temporary state flag
	dbTable* mTable;	///< ref to main table controlling this page
	void* mLastDisplayTable;
	dbAbstractBrowseHelper*	mLastBrowser;
	bool mDontCareIfLocked;

public:
/// \name implement per-platform, no generic implementation
//@{
	bool msgForOOFILE(const MessageT, void* ioParam);
	void goHighlightedRecord(dbAbstractBrowseHelper* inBrowser);
	void changedSelection(oofReceiver* dontNeedToTell=0);
	bool isOOFILEcommandEnabled(long commandID, Boolean& outEnabled) const;
	static bool isOOFILEmsg(const MessageT	);
//@}
};


/**
	Abstract base Controller class managing database to GUI mapping.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbHelper : public oofSingleListener {
public:
	dbHelper(dbTable*);
	virtual ~dbHelper();

	virtual void maybeChangeTargetLink(dbFieldLink*);	
	virtual void maybeDirty(dbFieldLink*);
	virtual dbTable* table() const;
};



/**
	dbHelper for managing lists of records.
	Abstracted cross-platform interface, subclassed for each framework.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbAbstractBrowseHelper : public dbHelper {
public:
	dbAbstractBrowseHelper(dbTable*);
	virtual ~dbAbstractBrowseHelper() {}
public:
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual unsigned long highlightedRecNo() const = 0;
	virtual void highlightRecNo(unsigned long inRecNo, bool bSelect=true) const = 0;
	virtual void highlightNothing() const = 0;
	virtual dbSelection	highlightedSelection() const = 0;
	virtual void deleteSelection(const dbSelection& inSel);
	virtual void updateBrowserSelection() const = 0;
	virtual dbView* view() const=0; // topmost view
	virtual dbTable* table() const;
	virtual void refreshBrowser()=0;
};


/**
	dbHelper for managing an editing form that changes a record.
	Abstracted cross-platform interface, subclassed for each framework.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbAbstractEditHelper : public dbHelper, public oofBroadcaster {
public:
	dbAbstractEditHelper(dbTable*);
	virtual ~dbAbstractEditHelper();

// generic links used if user supplies dbGUIAdapter
	void linkAdapter(dbField&, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly=false);
	void linkAdapter(dbField*, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly=false);

	void linkPickAdapter(dbField&, dbGUIAdapter* adoptedAdapter, 
		bool becomeReadOnly=false, dbRelRef* pickVia=0);

	void linkPickAdapter(dbField*, dbGUIAdapter* adoptedAdapter, 
		bool becomeReadOnly=false, dbRelRef* pickVia=0);

// mandatory platform-specific overrides
	virtual void* currentGUItarget() const=0;
protected:
	virtual bool SendFrameworkMessage(OOFmsgT, void* ioParam=0)=0;
	virtual bool SendImmediateFrameworkMessage(OOFmsgT, void* ioParam=0);

public:
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	void adoptLink(dbFieldLink* adoptedLink);	// for people to setup their own link & be able to access it!
	
	dbFieldLink* getLink(void*);	// retrieve manufactured links for given user control
	virtual dbTable*	table() const;
	
	bool isDirty();
 	void copyLinksToFields();
	void copyFieldsToLinks();
	virtual void maybeChangeTargetLink(dbFieldLink*);
	virtual void maybeDirty(dbFieldLink*);
	virtual void updateDerivedFieldDisplays();
	
	// default settings, individual fields may override
	bool isReadOnly() const;	
	void readOnly(bool isReadOnly=true);
		
/// \name USER-HOOK messages as alternative to subclassing and overriding the hook methods
//@{
	void msgEnteringAboutToLoad(OOFmsgT);
	OOFmsgT msgEnteringAboutToLoad();
	
	void msgEnteringAfterLoad(OOFmsgT);
	OOFmsgT msgEnteringAfterLoad();

	void msgCanSaveRecord(OOFmsgT);
	OOFmsgT msgCanSaveRecord();

	void msgConfirmSave(OOFmsgT);
	OOFmsgT msgConfirmSave();
//@}
	
	

	void msgChangedCurrentField(OOFmsgT);
	OOFmsgT msgChangedCurrentField();
	
protected:
	virtual void LoadData();
	void ResetDirtyLinks();
		
	virtual bool LeaveRecord();
	bool ValidateLinks();
	
/// \name USER-HOOKS override the following virtual methods as documented in guihooks.htm
//@{
	virtual void EnteringAboutToLoad();
	virtual void EnteringAfterLoad();
	virtual bool CanSaveRecord();
	virtual YNCancelT ConfirmSave();
	virtual void	ChangedCurrentField();
//@}
	
// data storage
	bool	mChangingPages, mAutoSaveOnMoving;
	dbTable*	mMainTable;  // owned smart pointer
	OOF_Dictionary mLinks;
// copies of what are probably members of the subclass used to manage the output
// list or other database front-end. We need these so we can provide default
// behaviour here in the mixin, that can be overridden if absolutely needed
// but is useful from day one	

	bool	mReadOnly;			// default is read-write, applies to all links created after change
	dbFieldLink* mCurrentLink;
	
// user-hook messages
	OOFmsgT mMsgEnteringAboutToLoad;
	OOFmsgT mMsgEnteringAfterLoad;
	OOFmsgT mMsgCanSaveRecord;
	OOFmsgT mMsgConfirmSave;
	OOFmsgT mMsgChangedCurrentField;


public:
   static void warnUser(ostream& os);
};


/**
	dbFieldLink for numerics aware of specifics in numeric update.
	\see dbNumericField::formatMask()
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbNumericFieldLink : public dbFieldLink {
public:
	dbNumericFieldLink(dbField*, dbGUIAdapter*, bool becomeReadOnly);
	
	virtual void updateDerivedFieldDisplay(bool isNew);
};


/**
	dbFieldLink for bools that knows how to translate bool to string.
*/
class OOFGUI_EXPORT dbBoolLink : public dbNumericFieldLink {
public:
	dbBoolLink(dbField*, dbGUIAdapter*, bool becomeReadOnly=false);
	
	virtual void copyFieldToLink();
	virtual void copyLinkToField();
	virtual bool linkEqualsField() const;
};


/**
	dbFieldLink for dbDate that knows how to translate and validate.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbDateLink2Editor : public dbNumericFieldLink {
public:
	dbDateLink2Editor(dbField*, dbGUIAdapter*, bool becomeReadOnly=false);
	
	virtual void updateDerivedFieldDisplay(bool isNew);
	virtual bool validToLeaveLink();
};


/**
	dbFieldLink for dbDateTimeLink that knows how to translate and validate.
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbDateTimeLink2Editor : public dbNumericFieldLink {
public:
	dbDateTimeLink2Editor(dbField*, dbGUIAdapter*, bool becomeReadOnly=false);
	
	virtual void updateDerivedFieldDisplay(bool isNew);
	virtual bool validToLeaveLink();
};


/**
	abstract base to allow us to wrap different list controls.	
	\ingroup oofGUI
*/
class OOFGUI_EXPORT dbTableDisplay { 
public:
	dbTableDisplay(unsigned short headingRows=0);
	virtual ~dbTableDisplay() {};
	virtual void loadData()=0;
	virtual void refresh()=0;
	virtual void changeViewTo(dbView*)=0;
	virtual long highlightedRow() const=0;
	virtual long nextHighlightedRow(long afterRow) const=0;
	virtual void highlightRow(unsigned long, bool bSelect=true)=0;
	virtual void highlightNothing()=0;
	virtual void appendRow()=0;
	virtual void deleteRow(unsigned long zeroBasedRowNum)=0;
	
	virtual bool displayUsesControl(void*) const=0;
	virtual void setDoubleClickMessage(MessageT)=0;
protected:
	unsigned short mHeadingRows;
};


// -------------------------------------------------------
//                      d b G U I
// -------------------------------------------------------

inline dbTable* 
dbGUI::mainTable() const
{
	return mTable;
}


/**
	Editing-aware check if record available.
	it is possible a screen like a settings screen may have dbEditHelper's on a given table
	without intending to edit it, so will call dbGUI::dontCareIfLocked
*/
inline bool
dbGUI::recordAvailable() const
{
	bool ret = mDontCareIfLocked || mTable->lockedCurrentRecord();
	return ret;
}


inline void 
dbGUI::dontCareIfLocked()
{
	mDontCareIfLocked = true;
}


inline void 
dbGUI::careIfLocked()
{
	mDontCareIfLocked = false;
}



// -------------------------------------------------------
//           d b A b s t r a c t E d i t H e l p e r
// -------------------------------------------------------
inline bool 
dbAbstractEditHelper::isReadOnly() const
{
	return mReadOnly;
}


inline void 
dbAbstractEditHelper::msgEnteringAboutToLoad(OOFmsgT inMsg)
{
	mMsgEnteringAboutToLoad = inMsg;
}


inline OOFmsgT 
dbAbstractEditHelper::msgEnteringAboutToLoad()
{
	return mMsgEnteringAboutToLoad;
}

	
inline void 
dbAbstractEditHelper::msgEnteringAfterLoad(OOFmsgT inMsg)
{
	mMsgEnteringAfterLoad = inMsg;
}


inline OOFmsgT 
dbAbstractEditHelper::msgEnteringAfterLoad()
{
	return mMsgEnteringAfterLoad;
}


inline void 
dbAbstractEditHelper::msgCanSaveRecord(OOFmsgT inMsg)
{
	mMsgCanSaveRecord = inMsg;
}


inline OOFmsgT 
dbAbstractEditHelper::msgCanSaveRecord()
{
	return mMsgCanSaveRecord;
}


inline void 
dbAbstractEditHelper::msgConfirmSave(OOFmsgT inMsg)
{
	mMsgConfirmSave = inMsg;
}


inline OOFmsgT 
dbAbstractEditHelper::msgConfirmSave()
{
	return mMsgConfirmSave;
}


inline void 
dbAbstractEditHelper::msgChangedCurrentField(OOFmsgT inMsg)
{
	mMsgChangedCurrentField = inMsg;
}


inline OOFmsgT 
dbAbstractEditHelper::msgChangedCurrentField()
{
	return mMsgChangedCurrentField;
}


inline void 
dbAbstractEditHelper::linkAdapter(dbField& fld, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly)
{
	linkAdapter(&fld, adoptedAdapter, becomeReadOnly);
}


inline void 
dbAbstractEditHelper::linkPickAdapter(dbField& fld, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly, dbRelRef* pickVia)
{
	linkPickAdapter(&fld, adoptedAdapter, becomeReadOnly, pickVia);
}



// -------------------------------------------------------
//                d b G U I A d a p t e r
// -------------------------------------------------------
inline	dbGUIAdapter::dbGUIAdapter() : 
	mFieldLink(0),
	mIsValuePicker(false)
{}
	
	
inline	bool	
dbGUIAdapter::IsReadOnly() const
{
	return mFieldLink->isReadOnly();
}


inline dbFieldLink*
dbGUIAdapter::fieldLink() const
{
	return mFieldLink;
}


inline bool 
dbGUIAdapter::isValuePicker() const
{
	return mIsValuePicker;
}


// -------------------------------------------------------
//                d b F i e l d L i n k
// -------------------------------------------------------
inline void 
dbFieldLink::maybeDirty()
{
	mMaybeDirty = true;
	mOwner->maybeDirty(this);
}


inline void 
dbFieldLink::markClean()
{
	mMaybeDirty = false;
}


inline void 
dbFieldLink::maybeChangeTargetLink()
{
	assert(mOwner);
	mOwner->maybeChangeTargetLink(this);
}


inline bool 
dbFieldLink::isReadOnly() const
{
	return mReadOnly;
}


inline bool 
dbFieldLink::alwaysReadOnly() const
{
	return mAlwaysReadOnly;
}


inline bool 
dbFieldLink::isDirty() const
{
	if (mField)
		return mField->isDirty();
	else
		return mMaybeDirty; // if don't have field, use local status
}


inline void 
dbFieldLink::owner(dbHelper* inOwner)
{
	mOwner = inOwner;
}


inline dbHelper*  
dbFieldLink::owner() const
{
	return mOwner;
}



/**
	Accessor for field being edited.
	Used by dbGUIAdapters such as dbGUIAdaptEditPane::setFieldLink
	to get the field in order to query database characteristics.
	\return dbField* to the field we link.
*/
inline dbField* 
dbFieldLink::field() const
{
	return mField;	
}


inline dbGUIAdapter* 
dbFieldLink::adapter() const
{
	return mAdapter;
}


inline void 
dbFieldLink::setField(dbField* inField)
{
	mField = inField;	
}


// -------------------------------------------------------
//                d b F i e l d L i n k
// -------------------------------------------------------
inline bool 
dbFieldPickLink::allowsNullRelationship() const
{
	return mAllowsNullRelationship;
}


inline void 
dbFieldPickLink::allowNullRelationship(bool inAllowFlag)
{
	mAllowsNullRelationship = inAllowFlag;
}


inline void 
dbFieldPickLink::setDefaultPick(int inPick)
{
	mDefaultPick = inPick;
	assert(!mLookupTable || inPick<mLookupTable->countAll());
}


#endif

