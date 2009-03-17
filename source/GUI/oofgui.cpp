// COPYRIGHT 1999 A.D. Software, All rights reserved

// Platform-independent GUI Integration layer of OOFILE database

#include "oofpch_m.h"  // for precompilation of MFC GUI files

#ifndef H_OOFGUI
	#include "oofgui.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#ifndef OOFYNC_H
	#include "oofync.h"
#endif

const unsigned long kRecordLockedByOtherUser = 'Lock';

// -------------------------------------------------------
//                        d b G U I
// -------------------------------------------------------
dbGUI::dbGUI(dbTable* inTable) :
	mClosing(false),
	mTable(inTable),
	mLastDisplayTable(0),
	mLastBrowser(0),
	mDontCareIfLocked(true)	// until they link something editable, or explicitly change
{
}


bool
dbGUI::ConfirmDelete(const char* confirmMessage) 
{
	bool ret = false;
	if (!confirmMessage)
		confirmMessage = "Are you sure you want to delete this record?";
	oofYNC theMsg(confirmMessage, "Delete", "Cancel");
	theMsg.setDefaultButton(1);	// cancel
	retTypeT buttonClicked = theMsg.msgBox(); 
	if (buttonClicked==IDYES) 
		ret = true;
	return ret;
}


bool
dbGUI::ConfirmRevert() 
{
	bool ret = false;
	oofYNC theMsg("Are you sure you want to Revert this record, losing all your changes?", "Revert", "Cancel");
	theMsg.setDefaultButton(1);	// cancel
	retTypeT buttonClicked = theMsg.msgBox(); 
	if (buttonClicked==IDYES) 
		ret = true;
	return ret;
}


void
dbGUI::newRecord()
{
	if (leaveRecord()) {
		mTable->beginTransaction();
		mTable->newRecord();
		broadcast(OOFmsg_NewRecord);
	}
}


void
dbGUI::saveRecord()
{
	bool canSave = (askAllStoppingAtFirst(false, OOFmsg_AttemptSaveRecord) == 0);
	if (canSave) {
		mTable->saveRecord();
		broadcast(OOFmsg_SaveRecord);
	}
}


void
dbGUI::deleteRecord()
{
// don't need to confirm we can leave it - it's being deleted !!!!
	mTable->deleteRecord();
	changedSelection();
}


void
dbGUI::revertRecord()
{
	if (ConfirmRevert()) {
		mTable->revertRecord();
		broadcast(OOFmsg_RevertRecord);
	}
}


void
dbGUI::cloneRecord()
{
	assert(mTable);
	if (leaveRecord()) {
		mTable->cloneRecord();
		changedSelection();
	}
}


dbAbstractBrowseHelper*
dbGUI::GetDisplayTableHandler(void* ioParam)
{
	dbAbstractBrowseHelper* ret = 0;
	/*if (mLastDisplayTable == ioParam)
		ret = mLastBrowser;
	else*/ {
		ret = (dbAbstractBrowseHelper*) askAllStoppingAtFirst(true, OOFmsg_GetDisplayTableHandler, (unsigned long)ioParam);	// safe downcast
		if (ret) {
			mLastBrowser = ret;
			mLastDisplayTable = ioParam;
		}
	}
	return ret;
}


void 
dbGUI::goFirstRecord()
{
	assert(mTable);
	if (mTable->atFirst()) 
		oofRuntimeEnv::beep();
	else
	{
		if(leaveRecord()) {
			mTable->first();
			changedSelection();
		}
	}
}


void 
dbGUI::goNextRecord()
{
	assert(mTable);
	if (mTable->atLast()) 
		oofRuntimeEnv::beep();
	else
	{
		if(leaveRecord()) {
			mTable->next();
			changedSelection();
		}
	}
}


void 
dbGUI::goPrevRecord()
{
	assert(mTable);
	if (mTable->atFirst()) 
		oofRuntimeEnv::beep();
	else
	{
		if(leaveRecord()) {
			mTable->prev();
			changedSelection();
		}
	}
}


void 
dbGUI::goLastRecord()
{
	assert(mTable);
	if (mTable->atLast()) 
		oofRuntimeEnv::beep();
	else
	{
		if(leaveRecord()) {
			mTable->last();
			changedSelection();
		}
	}
}


/**
	Provide a current record for the form to edit.
*/
void
dbGUI::gotoCurrentOrNewRec()
{
// called before data loaded
	if (mTable->isEmpty())
		mTable->newRecord();
	else
		if (!mTable->isRecordLoaded())	
			mTable->start();
	changedSelection();
}


/**
	Get permission to leave record and unlock including commmit/abort transactions.
	Query all dependent dbHelper and other oofListener subscribers and leave if possible.
	Clears locks and transactions automatically.
	\todo more sophisticated transaction unlocking
	\todo confirm that we need to commitRecord if cancelling
*/
bool
dbGUI::leaveRecord()
{
	bool canLeave = (askAllStoppingAtFirst(false, OOFmsg_AttemptLeaveRecord)==0);
	if (canLeave) {
		if (mTable->isDirty()) {
			mTable->saveRecord();
			broadcast(OOFmsg_SaveRecord);
		}
		if(mTable->transactionDirty())
			mTable->commitTransaction();
		else 
			mTable->abortTransaction();// releases all locks
		mTable->unlockRecord();
	}
	return canLeave;
}


// -------------------------------------------------------
//                 d b H e l p e r
// -------------------------------------------------------
dbHelper::dbHelper(dbTable* tbl) :
	oofSingleListener(tbl ? tbl->getGUI() : 0)
{
}


dbHelper::~dbHelper()
{
}


void 
dbHelper::maybeChangeTargetLink(dbFieldLink*)
{
// null method so subclasses not forced to override
}


void 
dbHelper::maybeDirty(dbFieldLink*)
{
// null method so subclasses not forced to override
}


dbTable* 
dbHelper::table() const
{
	return NULL;
}


// -------------------------------------------------------
//       d b A b s t r a c t B r o w s e H e l p e r
// -------------------------------------------------------

dbAbstractBrowseHelper::dbAbstractBrowseHelper(dbTable* inTable)
: dbHelper(inTable)
{}


dbTable* 
dbAbstractBrowseHelper::table() const
{
	dbView* theView = view();
	if (theView)
		return theView->table();
	else
		return NULL;
}


void
dbAbstractBrowseHelper::deleteSelection(const dbSelection& inSel)
{
	dbTable* table = view()->table();
	// current selection in the main table
	dbSelection curSel = table->currentSelection();

	// this must be done before the deleteSelection takes place
	if (!curSel.isAllRecs())
		curSel.difference_with(inSel);

	table->setSelection(inSel);
	table->deleteSelection();

	// handle the situation when all records were deleted from the table, which has
	// "allRecs" flag set on; it would be actually OK to restore the curSel, which has
	// the "allRecs" flag set on, to an empty table, but it's better to be consistent,
	// because if the table items are deleted one by one, the state of the table
	// selection will be "empty"
	if (curSel.isAllRecs()) {
		table->selectAll();
		if (table->isEmpty())
			table->selectNone();
	}
	else
		table->setSelection(curSel);
}


bool 
dbAbstractBrowseHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret = true;
	dbTable* ourTable = view()->table();
	assert(ourTable);

	switch (msg) {
	case OOFmsg_NewRecord :	// avoid highlighting another record
		refreshBrowser();	// will pick up added record if any
		highlightNothing();
		break;
	
	case OOFmsg_SaveRecord :	// senderDefined is the OID
		refreshBrowser();	// will pick up added record if any
		break;
	
	case OOFmsg_ChangeSelection : {
// later use new message ChangeRecord and pass new rec no	
		refreshBrowser();	// will recalc storage size
		dbTable* realTable = ourTable->getGUI()->mainTable();
		highlightRecNo(realTable->recordNumber());
		break;
	}
		
	case OOFmsg_AttemptLeaveRecord :
	case OOFmsg_AttemptSaveRecord :
		break;	// later might allow for editable cells with validation, just default to true now

	case OOFmsg_RevertRecord:
		refreshBrowser();
		break;

	default :
		ret = false;	// ignore unknown messages
	};
	return ret;
}


// -------------------------------------------------------
//      d b A b s t r a c t E d i t H e l p e r
// -------------------------------------------------------

dbAbstractEditHelper::dbAbstractEditHelper(dbTable* tbl) :
							dbHelper(tbl),
							mChangingPages(false),
							mAutoSaveOnMoving(tbl ? tbl->tableCachesDirtyRecords() : false),
							mMainTable(tbl),
							mReadOnly(false),
							mCurrentLink(0),
							mMsgEnteringAboutToLoad(OOFmsg_None),
							mMsgEnteringAfterLoad(OOFmsg_None),
							mMsgCanSaveRecord(OOFmsg_None),
							mMsgConfirmSave(OOFmsg_None),
							mMsgChangedCurrentField(OOFmsg_None)						
{
}


dbAbstractEditHelper::~dbAbstractEditHelper()
{
	mLinks.deleteAll();
//	delete mMainTable;
}



void 
dbAbstractEditHelper::adoptLink(dbFieldLink* adoptedLink)
{
	mLinks.append(adoptedLink);
	adoptedLink->owner(this);
}



bool 
dbAbstractEditHelper::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	bool ret = true;
	switch (msg) {
	case OOFmsg_AttemptLeaveRecord :
		ret = LeaveRecord();
		break;
		
	case OOFmsg_AttemptSaveRecord :
		if (mCurrentLink)
			 ret = mCurrentLink->leaveLink();
		if (ret)
			ret = CanSaveRecord();
		break;
		
	case OOFmsg_NewRecord :
		LoadData();
		break;

	case OOFmsg_ChangeSelection :
		if (senderDefined == kRecordLockedByOtherUser)
			readOnly(true);
		LoadData();
		break;

	case OOFmsg_SaveRecord :
		ResetDirtyLinks();
		break;

	case OOFmsg_RevertRecord :
		mMainTable->unloadRecord();	// flush any modified fields
		mMainTable->reloadRecord();
		LoadData();
		break;
		
	default :
		ret = false;	// ignore unknown messages
	};
	return ret;
}


/**
	Ask user to confirm if should save or not, allowing cancel.
	Can override by subclassing.
	msgConfirmSave(OOFmsgT) also lets you specify a message to be sent to application
	to allow confirmation - an easier way to hook in behaviour without subclassing.
	\note you could totally disable this behaviour by having your reaction to mMsgConfirmSave
	be No, eg: if you were running in demo mode.
	\return triple value YNCancelT indicating user response.
*/
YNCancelT
dbAbstractEditHelper::ConfirmSave() 
{
	YNCancelT ret = Yes;
	if (mMsgConfirmSave != OOFmsg_None)
		SendImmediateFrameworkMessage(mMsgConfirmSave, &ret);
	else {
		oofYNC theMsg("Do you want to save your changes?", "Save", "Don't Save", "Cancel");
		retTypeT buttonClicked = theMsg.msgBox(); 
		switch (buttonClicked) {
		case IDYES:
			ret = Yes;
			break;
		case IDNO:
			ret = No;
			break;
		case IDCANCEL:
			ret = Cancel;
			break;
		}
	}
	return ret;
}


void 
dbAbstractEditHelper::EnteringAboutToLoad()
{
	if (mMsgEnteringAboutToLoad != OOFmsg_None)
		SendFrameworkMessage(mMsgEnteringAboutToLoad);
}


void 
dbAbstractEditHelper::EnteringAfterLoad()
{
	if (mMsgEnteringAfterLoad != OOFmsg_None)
		SendFrameworkMessage(mMsgEnteringAfterLoad);

}


bool 
dbAbstractEditHelper::CanSaveRecord()
{
	bool ret = ValidateLinks();
	if (ret) {
		if (mMsgCanSaveRecord != OOFmsg_None)
			ret = SendImmediateFrameworkMessage(mMsgCanSaveRecord);
	}
	return ret;
}


void 
dbAbstractEditHelper::ChangedCurrentField()
{
	if (mMsgChangedCurrentField != OOFmsg_None)
		SendFrameworkMessage(mMsgChangedCurrentField);
}


/**
	React to possibly leaving a field, calling validation and update methods.
	Calls updateDerivedFieldDisplays so calculated fields can react to changes.
	\note some GUI toolkits rely on direct dependencies to provide this kind of 
	updating, which has huge performance implications with cascading broadcasts
	(like Swing). By centralising the update here we have the chance to control
	GUI behaviour and react optimally.
	\todo make field display updates optional
*/
void 
dbAbstractEditHelper::maybeChangeTargetLink(dbFieldLink* newLink)
{
	if (mCurrentLink != newLink) {
// NOT YET IMPLEMENTED - option of field-departure validate triggered here	
		if (!mCurrentLink) { // on first field in screen?
			void* maybeTarget = currentGUItarget();
			mCurrentLink = getLink(maybeTarget);	
		}
		bool changedField = true;
		if (mCurrentLink) 
			changedField = mCurrentLink->leaveLink();	// update DB so calc fields etc. work
			
		mCurrentLink = newLink;
		if (changedField) {
			ChangedCurrentField();	// user hook or message broadcast
	
// update displays after user hook in case they change some other value
			updateDerivedFieldDisplays();	// masked formats, calculated fields, default calculated fields etc.
		}
	}
}


void 
dbAbstractEditHelper::maybeDirty(dbFieldLink* inLink)
{
//	if (mCurrentLink) {
//		assert(mCurrentLink == inLink);
// worked out sometimes mCurrentLink may point to a caption, in which case typing in the first
// field will cause the above assertion to fail.
//	}
//	else
		mCurrentLink = inLink;
// NOT YET IMPLEMENTED - locking check - bring up warning if locked		
}


dbTable* 
dbAbstractEditHelper::table() const
{
	return mMainTable;
}


dbFieldLink* 
dbAbstractEditHelper::getLink(void* targetPane)
{
	dbFieldLink* ret = 0;	// default is return nil pointer
	int numLinks = mLinks.count();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		dbGUIAdapter* fa = fl->adapter();
		if (fa && fa->adaptsControl(targetPane)) {
			ret = fl;
			break;
		}
	}
	return ret;
}


/**
	Leave record, asking user to OK changes if necessary.
	If record needs saving and not in auto-save mode, calls ConfirmSave.
	\return true if were allowed to leave.
*/
bool 
dbAbstractEditHelper::LeaveRecord()
{
	bool willLeave = true;
	if (mCurrentLink) {
		mCurrentLink->leaveLink();
	}
		
	if (isDirty()) {
		if (mAutoSaveOnMoving || mMainTable->tableSaveOption()==dbTable::autoOnContextChange) {
			willLeave = CanSaveRecord();
		}
		else {
			YNCancelT reply = ConfirmSave();
			if (reply==No)
				mMainTable->unloadRecord(); 
			else {
				if (reply==Cancel)
					willLeave = false;
				else
					willLeave = CanSaveRecord();
			} // cancelled
		} // auto-save
	} // dirty, otherwise don't care to check further

	if (willLeave)		
		mCurrentLink = 0;	// so we don't think we've stayed in the same edit control
	return willLeave;
}


void 
dbAbstractEditHelper::LoadData()
{
	assert(mMainTable);		// should be setup by now!
	EnteringAboutToLoad();	// user hook
	copyFieldsToLinks();
	broadcast(OOFmsg_ChangeSelection, 0);
	EnteringAfterLoad();	// user hook
}


void 
dbAbstractEditHelper::warnUser(ostream& os)
{
#ifdef _Windows
	OOF_String s(os);
	::MessageBox(0, s, "Warning", MB_ICONEXCLAMATION);
#else
	dbConnect::raise(os, false);
#endif
}


void 
dbAbstractEditHelper::readOnly(bool becomeReadOnly)
{
	if (mReadOnly == becomeReadOnly) 
		return;	// same setting - trivial exit
		
	if (becomeReadOnly || mMainTable->lockedCurrentRecord()) {
		mReadOnly = becomeReadOnly;
		const int numLinks = mLinks.count();
		for (int i=0; i<numLinks; i++) {
			dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
			fl->PreApprovedReadOnly(becomeReadOnly);		
			mLinks.next();
		}
	}
}


void 
dbAbstractEditHelper::ResetDirtyLinks()
{
	const int numLinks = mLinks.count();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		fl->markClean();		
	}
}


/**
	\note not used by OOFILE any more but User may invoke for mass update
	\see dbFieldLink::leaveLink - we copy back on field by field basis
	so database matches visuals, and calc fields work on the visible values
*/
void 
dbAbstractEditHelper::copyLinksToFields()
{
	const int numLinks = mLinks.count();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		fl->copyLinkToField();		
	}
}


/**
	Check all links to see if they are validToLeaveForm, stopping at first failure.
*/
bool 
dbAbstractEditHelper::ValidateLinks()
{
	bool ret = true;
	const int numLinks = mLinks.count();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		if (!fl->validToLeaveForm()) {
			ret = false;
			fl->highlightLink();
			break;
		}
	} // loop all links
	return ret;
}


void 
dbAbstractEditHelper::copyFieldsToLinks()
{
	const int numLinks = mLinks.count();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		fl->copyFieldToLink();
		fl->markClean();		
	}
}


void 
dbAbstractEditHelper::updateDerivedFieldDisplays()
{
	int numLinks = mLinks.count();
	bool isNew = table()->isNewRecord();
	for (int i=0; i<numLinks; i++) {
		dbFieldLink* fl = (dbFieldLink*) (mLinks[i]);  // safe downcast
		fl->updateDerivedFieldDisplay(isNew);
	}
}


/**
	Check if helper is dirty, implied by mMainTable being dirty.
	dbField dirty back-propagation ensures edits to related data dirty main table.
	\return true indicates dirty, will need to save
*/
bool
dbAbstractEditHelper::isDirty()
{
	bool ret = mMainTable->isDirty();	
	return ret;	// assume dirtied by copying link to field, or by other user action
}


/**
	Send a message so any listeners can approve an action.
	Must be implemented by synchronous message posting, not delayed queue
	as is used to provide user hooks to influence editing behaviour.
	\see dbAbstractEditHelper::CanSaveRecord
	\return true if action approved
*/
bool 
dbAbstractEditHelper::SendImmediateFrameworkMessage(OOFmsgT inMsg, void* ioParam)
{
	return SendFrameworkMessage(inMsg, ioParam);
}


/**
*/
void 
dbAbstractEditHelper::linkAdapter(dbField* fld, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly)
{
	dbFieldLink* newLink;
	assert(fld);
	switch (fld->fieldType()) {
	case (boolField):
		newLink = new dbBoolLink(fld, adoptedAdapter, becomeReadOnly);
		break;
		
	case (dateField):
		newLink = new dbDateLink2Editor(fld, adoptedAdapter, becomeReadOnly);
		break;
		
	case (dateTimeField):
		newLink = new dbDateTimeLink2Editor(fld, adoptedAdapter, becomeReadOnly);
		break;
		
	default:
		newLink = new dbFieldLink(fld, adoptedAdapter, becomeReadOnly);
	}
	adoptLink(newLink);
}


/**
	Link an Adapter to control used to select from lookup table.
	Often popup menu but may be an edit control - we abstract that.
	\param pickVia used to clarify which is lookup table, in complex multi-rel chain
	eg: Patients->Visits->Doctor->Staff->Name with pickVia == Patients->Visits->Doctor
	pickVia is the relationship going from left to right. It will usually point to the 
	table containing the choice field, ie:
	fld->table()==pickVia->relatedTable().
	\note a very important role of this method is in most cases work out the default pickVia
	\warning an easy mistake is to specify just the relationship, NOT the field pointed to, eg:
	\code
		mEditHelper->linkPickField (mOOFData->Prefs->DefaultAssignee, mAssignedToPopup);
	instead of
		mEditHelper->linkPickField (mOOFData->Prefs->DefaultAssignee->ShortDesc, mAssignedToPopup);
	\endcode
	\todo allow for picking via date entry fields, like dbDateLink2Editor 
*/
void 
dbAbstractEditHelper::linkPickAdapter(dbField* fld, dbGUIAdapter* adoptedAdapter, 
	bool becomeReadOnly, dbRelRef* pickVia)
{
	assert(!fld->fieldIsStandalone());
	dbTable* rightmostTable = fld->fieldTable();  
	assert(rightmostTable->isRelatedClone());
	if (pickVia) {
		assert(pickVia->fieldTable()->baseTableOfRelationChain() == rightmostTable->baseTableOfRelationChain());
		// confirms pickVia is in the chain
		assert(!pickVia->isRelBackToParent());  // we must be pointing right, ie: toward lookup table.
	}
	else {
		dbRelRefBase* fldRel = rightmostTable->controllingRelationship()->inverse();
		assert(fldRel && (fldRel->isManyToOne() || fldRel->isOneToOne()));
		pickVia = (dbRelRef*) fldRel;  // safe downcast due test above
	}


	dbFieldLink* newLink = new dbFieldPickLink(fld, adoptedAdapter, becomeReadOnly, pickVia);
	adoptLink(newLink);
}

// -------------------------------------------------------
//            d b T a b l e D i s p l a y
// -------------------------------------------------------
dbTableDisplay::dbTableDisplay(unsigned short headingRows) : 
		mHeadingRows(headingRows)
{}



// -------------------------------------------------------
//          d b G U I A d a p t o r 
// -------------------------------------------------------
void 
dbGUIAdapter::setFieldLink(dbFieldLink* inLink)
{
	mFieldLink = inLink;
}


bool 
dbGUIAdapter::equalsString(const oofString& inString) const
{
// by avoiding copying the getString return value to a local var we hopefully avoid a 
// copy of the contents as oofString::operator==(const oofString&) will
// use the temp return value directly
	const bool ret = (inString==getString());
	return ret;
}


int 
dbGUIAdapter::getValue() const
{
	long ret = getString().asLong();  // should use temporary ret value - avoid extra copy
	return ret;
}


void 
dbGUIAdapter::setValue(int inValue)
{
	char buf[20];
	sprintf(buf, "%i", inValue);
	setString(buf);
}


bool 
dbGUIAdapter::equalsValue(int inValue) const
{
	const long theValue = getValue();
	const bool ret = (theValue==inValue);
	return ret;
}


bool 
dbGUIAdapter::adaptsControl(void*) const
{
	return false;
}


/**
	Null virtual method for subclasses to override, eg dbGUIAdaptLPopupButton.
*/
void 
dbGUIAdapter::setValueList(dbView&)
{
}



// -------------------------------------------------------
//          d b G U I A d a p t C o n t r o l
// -------------------------------------------------------
oofString 
dbGUIAdaptControl::getString() const
{
	const int theValue = getValue();
	oofString ret;
	ret.convertNumber(theValue);
	return ret;
}


void 
dbGUIAdaptControl::setString(const oofString& inString)
{
	const long theValue = inString.asLong();
	setValue(theValue);
}


bool 
dbGUIAdaptControl::equalsString(const oofString& inString) const
{
	const long theValue = getValue();
	const long inValue = inString.asLong();
	const bool ret = (theValue==inValue);
	return ret;
}



// -------------------------------------------------------
//          d b G U I A d a p t E d i t o r
// -------------------------------------------------------
void
dbGUIAdaptEditor::editingOccurred()
{
	mFieldLink->maybeDirty();
}


void 
dbGUIAdaptEditor::setTextEntryLimit(unsigned long)
{
// empty virtual to avoid having to implement where not possible/needed
}




// -------------------------------------------------------
//                d b F i e l d L i n k
// -------------------------------------------------------
/**
	\param adoptedAdapter is our interface to a source/display of values (probably control)
	\param becomeReadOnly indicates the control must not allow update of the field.
*/
dbFieldLink::dbFieldLink(dbField* fld, dbGUIAdapter* adoptedAdapter, bool becomeReadOnly) : 
	mField(fld),
	mAdapter(adoptedAdapter),
	mOwner(0),
	mMaybeDirty(false),
	mReadOnly(becomeReadOnly),
	mAlwaysReadOnly(becomeReadOnly)
{
	if (mAdapter)
		mAdapter->setFieldLink(this);
}


dbFieldLink::dbFieldLink(const dbFieldLink& rhs) : 
	mMaybeDirty(rhs.mMaybeDirty),
	mOwner(rhs.mOwner),
	mReadOnly(rhs.mReadOnly),
	mAlwaysReadOnly(rhs.mAlwaysReadOnly)
{
	if (rhs.mAdapter) {
		mAdapter = rhs.mAdapter->clone();
		mAdapter->setFieldLink(this);
	}
	else
		mAdapter = 0;
}


dbFieldLink::~dbFieldLink()
{
	delete mAdapter;
}

void 
dbFieldLink::copyFieldToLink()
{
	assert(field() && mAdapter);
	if (!field()->fieldIsStandalone())
		field()->fieldTable()->ensureRecordLoaded(true /* autoCreateRelated */);

	const oofString fldContents = field()->copyString();
	mAdapter->setString(fldContents);
}


void 
dbFieldLink::copyLinkToField()
{
	assert(field() && mAdapter);
	const oofString guiContents = mAdapter->getString();
	const oofString fldContents = mField->copyString();
	if (fldContents!=guiContents)
		mField->setString(guiContents);
}


/**
	Visually highlight a link, usually becoming edit focus.
	dbAbstractEditHelper::ValidateLinks calls us to highlight the first 
	invalid link it finds.
*/
void
dbFieldLink::highlightLink()
{
	assert(mAdapter);
	mAdapter->highlightContents();
}


void
dbFieldLink::tellGUIcareAboutLocking() const
{
	if (!mField)
		return;
		
	if (!mField->fieldIsStandalone()) {
		dbGUI* fieldGUI = mField->fieldTable()->getGUI();
		assert(fieldGUI);
		fieldGUI->careIfLocked();
	}
}


bool 
dbFieldLink::leaveLink()
{
	bool ret = false;	// assume fails
	if (mMaybeDirty) { // state set by editing the linked control onscreen
		if (validToLeaveLink()) {
			if (field()->fieldIsUniqueIndexed() && linkIsEmpty())
				copyFieldToLink();	// don't allow user to leave a unique key empty
				// usually used in conjunction with a default calculator on that key
				// so a calculated value would be displayed for an empty new field
			else {
				if (!linkEqualsField()) { // link has been updated so keep dbField in synch
					copyLinkToField();
				}  // not equal field
				ret = true;
			} // not unique key
		} // valid to leave
	}  // dirty
	else
		ret = true;	// if not dirty assume still valid
	// NOT YET IMPLEMENTED - think about this in future, it doesn't cope well if we have a field
	// that is valid when entered but becomes invalid due to other changes
	// however form-level validation should catch those cases
	return ret;
}


/**
	Validate field contents, overridden if can check just contents.
	Lightweight check that allows tabbing between fields in open record.
	\see validToLeaveForm for heavier check.
	\see dbDateLink2Editor::validToLeaveLink
*/
bool 
dbFieldLink::validToLeaveLink()
{
	return true;
}


/**
	Validate unique fields to ensure can accept this record without error from database.
	\warning you probably still want to call this method if you override.
*/
bool 
dbFieldLink::validToLeaveForm()
{
	bool ret = validToLeaveLink();
	
// check unique indexed fields
	if (ret && mField->fieldIsUniqueIndexed()) {
		if (mField->valueIsDuplicate()) {  // could allow dups in just leaveField, but not for final exit
			OOF_String fldStr = mField->copyString();
			oofYNC(
				stringstream() << flush << mField->fieldName() << '\'' << fldStr << "' is not unique, please change it and try saving again"
			);
			ret = false;
		} // BAD FIELD - DUPLICATE KEY
	} // test unique field
	return ret;
}


/**
	Change readonly state of link to accept or deny data entry.
	Won't allow enabling editing if can't edit record due to locking.
*/
void 
dbFieldLink::readOnly(bool becomeReadOnly)
{
	if (!mAlwaysReadOnly && (becomeReadOnly!=mReadOnly)) {
	// only change if able to change and not already in state
		if (!becomeReadOnly) {
			dbTable* tbl = mOwner->table();	
			// the table we want locked is that being edited
			// this field may be a standalone field, NOT one linked to database
			if (tbl) {
				const bool isLockedByOther = !tbl->getGUI()->recordAvailable();
				if (isLockedByOther)
					return;	// programmer can't make individual field writable if record locked
			}
		}
		PreApprovedReadOnly(becomeReadOnly);
	}
}


/**
	Fast implementation of readOnly().
	Avoids overhead of checking if table locked.
*/
void 
dbFieldLink::PreApprovedReadOnly(bool becomeReadOnly)
{
	// repeat test in readOnly() because we may be called directly
	if (!mAlwaysReadOnly && (becomeReadOnly!=mReadOnly)) {
		mReadOnly = becomeReadOnly;
		mAdapter->readOnly(becomeReadOnly);
	}
}


bool 
dbFieldLink::linkEqualsField() const
{
	assert(mAdapter);
	assert(mField);
	const oofString fldContents = mField->copyString();
	const bool ret = mAdapter->equalsString(fldContents);
	return ret;
}


bool
dbFieldLink::linkIsEmpty() const
{
	const oofString theStr = mAdapter->getString();
	const bool ret = theStr.isEmpty();
	return ret;
}


void 
dbFieldLink::updateDerivedFieldDisplay(bool isNew)
{
	if (mField->fieldIsVirtual() || (isNew && mField->hasDefaultCalculator()) )
		copyFieldToLink();
}


// -------------------------------------------------------
//                d b F i e l d P i c k L i n k
// -------------------------------------------------------

/**
	
*/
dbFieldPickLink::dbFieldPickLink(dbField* fld, dbGUIAdapter* adoptedAdapter, 
		bool becomeReadOnly, dbRelRef* pickVia) :
	dbFieldLink(fld, adoptedAdapter, becomeReadOnly) ,
	mAllowsNullRelationship(false),
	mPickVia(pickVia),
	mLookupTable(0),
	mCachedRecordNumber(ULONG_MAX),
	mDefaultPick(1)
{
	assert(!pickVia->isRelBackToParent());
}


dbFieldPickLink::dbFieldPickLink(const dbFieldPickLink& rhs) :
	dbFieldLink(rhs),
	mAllowsNullRelationship(rhs.mAllowsNullRelationship),
	mPickVia(rhs.mPickVia),
	mLookupTable(0),
	mCachedRecordNumber(rhs.mCachedRecordNumber),
	mDefaultPick(rhs.mDefaultPick)
{
}

dbFieldPickLink::~dbFieldPickLink()
{
	delete mLookupTable;
}


/**
	Load related record as popup setting or copy value.
	\todo cope with null relationships allowed for pickers (ie: popup menus)
	\todo cope with join relationships on pickers like setRelatedRecord(dbTable*);
	\todo more flexibility in initialising popup menu
*/
void 
dbFieldPickLink::copyFieldToLink()
{
	const bool firstCall = mLookupTable==0;
	if (firstCall) 
		InitLookup();

	assert(mAdapter);
	assert(mLookupTable);
	if (mAdapter->isValuePicker()) {
		if (firstCall)  {
			dbView valueView(mLookupTable);
			valueView << mField;
			mAdapter->setValueList(valueView);
		}
		dbTable* currentLookup = mPickVia->relatedTable();
		int pickIndex;
		if (currentLookup->count()==0) {
			pickIndex = mDefaultPick;
			if (mAllowsNullRelationship)
				mCachedRecordNumber = ULONG_MAX;  // invalid
			else {
				mCachedRecordNumber = mDefaultPick-1;
				mLookupTable->gotoRecord(mCachedRecordNumber);
				mPickVia->setRelatedRecord( mLookupTable );
			}
		}
		else {
			assert(!mPickVia->isJoin());  // see comment above method
			mCachedRecordNumber = mLookupTable->recordNumberOfOID( currentLookup->currentOID());
			pickIndex = 1 + mCachedRecordNumber;
		}
		mAdapter->setValue(pickIndex);
	}
	else {
		dbFieldLink::copyFieldToLink();
	}
}


/**
	Clone tables for lookup operations.
	\todo cache RAM-based map
*/
void 
dbFieldPickLink::InitLookup()
{
	assert(!mLookupTable);
	assert(mField && !mField->fieldIsStandalone() );
	mLookupTable = mField->fieldTable()->cloneTableWithoutSelection();  // we will selectAll
	mLookupTable->selectAll();
}


/**
	Update the linked dGUIAdapter from our field, actually what lookup record is related.
	If this is the first time called on this screen, may need to load a popup menu.
	Maps one-based indexes returned from pickers to 0-based records numbers.
	\todo handle picking via edit fields, doing a search
*/
void 
dbFieldPickLink::copyLinkToField()
{
	assert(mAdapter);
	assert(mLookupTable);
	if (mAdapter->isValuePicker()) {
		const int pickIndex = mAdapter->getValue() - 1;  // pickIndex should be a valid record number
		assert((pickIndex >= 0) && pickIndex < mLookupTable->count());
		mLookupTable->gotoRecord(pickIndex);
		mCachedRecordNumber = pickIndex;
		mPickVia->setRelatedRecord( mLookupTable );
	}
	else {
	// something like;
	// search the mLookupTable by the target field value
	// maybe do a setRelatedRecord(search expr)
		assert(!"dbFieldPickLink::copyLinkToField doesn't yet support picking via edit fields");
	}
}


/**
	Compare ordinal position for popups, use base for values.
*/
bool 
dbFieldPickLink::linkEqualsField() const
{
	assert(mAdapter);
	assert(mLookupTable);
	if (mAdapter->isValuePicker()) {
		const int pickIndex = mAdapter->getValue() - 1;  // pickIndex should be a valid record number
		return pickIndex==mCachedRecordNumber;
	}
	else {
		return dbFieldLink::linkEqualsField();
	}
}


/**
	Mainly override to avoid checks in base.
	It is common for our target fields (the lookup field) to be uniquely indexed.
	We rely on the check in validToLeaveForm.
	\return true at all times
*/
bool 
dbFieldPickLink::leaveLink()
{
	bool ret = false;	// assume fails
// currently we are not reliably marked dirty so assume always dirty	
//	if (mMaybeDirty) { // state set by editing the linked control onscreen
		if (validToLeaveLink()) {
			if (!linkEqualsField()) { // link has been updated so keep dbField in synch
				copyLinkToField();
			}  // not equal field
			ret = true;
		} // valid to leave
//	}  // dirty
//	else
//		ret = true;	// if not dirty assume still valid
	return ret;
}


bool 
dbFieldPickLink::validToLeaveForm()
{
	const bool pickerIsValid = mAllowsNullRelationship || (mPickVia->relatedTable()->count()>0);
	return pickerIsValid;
}

	



// -------------------------------------------------------
//        d b N u m e r i c F i e l d L i n k 
// -------------------------------------------------------
dbNumericFieldLink::dbNumericFieldLink(dbField* fld, dbGUIAdapter* inAdapter, bool becomeReadOnly) :
	dbFieldLink(fld, inAdapter, becomeReadOnly)
{}


void
dbNumericFieldLink::updateDerivedFieldDisplay(bool isNew)
{
	const dbNumericField* numField = (const dbNumericField*)mField;	// safe downcast
	if (isDirty()) {
// we have a format mask, so update the display
		if (numField->formatMask().length()) {	
			copyFieldToLink();
		}
	}
	else {
		if (numField->fieldIsVirtual() || (isNew && numField->hasDefaultCalculator()) )
			copyFieldToLink();
	}
}


// -------------------------------------------------------
//        d b B o o l L i n k
// -------------------------------------------------------
dbBoolLink::dbBoolLink(dbField* fld, dbGUIAdapter* inAdapter, bool becomeReadOnly) :
	dbNumericFieldLink(fld, inAdapter, becomeReadOnly)
{}

void 
dbBoolLink::copyFieldToLink()
{
	if (!field()->fieldIsStandalone())
		field()->fieldTable()->ensureRecordLoaded(true /* autoCreateRelated */);

	assert(field() && field()->fieldType()==boolField);
	const dbBool* asBool = (dbBool*) field();  // safe downcast
	if (asBool->value())
		mAdapter->setValue(1);
	else
		mAdapter->setValue(0);
}


void 
dbBoolLink::copyLinkToField()
{
	assert(field() && field()->fieldType()==boolField);
	dbBool* asBool = (dbBool*) field();  // safe downcast

	const bool guiContents = mAdapter->getValue();
	const bool fldContents = asBool->value();
	if (fldContents!=guiContents)
		*asBool = guiContents;
}


bool 
dbBoolLink::linkEqualsField() const
{
	assert(field() && field()->fieldType()==boolField);
	const dbBool* asBool = (dbBool*) field();  // safe downcast

	const bool guiContents = mAdapter->getValue();
	const bool fldContents = asBool->value();
	const bool ret = (fldContents==guiContents);
	return ret;
}



// -------------------------------------------------------
//        d b D a t e L i n k 2 E d i t o r
// -------------------------------------------------------
dbDateLink2Editor::dbDateLink2Editor(dbField* fld, dbGUIAdapter* inAdapter, bool becomeReadOnly) :
	dbNumericFieldLink(fld, inAdapter, becomeReadOnly)
{}


void
dbDateLink2Editor::updateDerivedFieldDisplay(bool isNew)
{
	const dbDate* dateField = (const dbDate*)mField;	// safe downcast
	if (isDirty() && dateField->isValid()) {
		if (dateField->formatMask().length()) {	// we have a format mask, so update the display
			copyFieldToLink();
		}
	}
	else {
		if (dateField->fieldIsVirtual() || (isNew && dateField->hasDefaultCalculator()) )
			copyFieldToLink();
	}
}


bool 
dbDateLink2Editor::validToLeaveLink()
{
	assert(mAdapter);
	bool ret = dbDate::checkDate(mAdapter->getString());
	if (!ret) {
		oofYNC("Invalid date");
	}
	return true;
}



// -------------------------------------------------------
//        d b D a t e T i m e L i n k 2 E d i t o r
// -------------------------------------------------------
dbDateTimeLink2Editor::dbDateTimeLink2Editor(dbField* fld, dbGUIAdapter* inAdapter, bool becomeReadOnly) :
	dbNumericFieldLink(fld, inAdapter, becomeReadOnly)
{}


void
dbDateTimeLink2Editor::updateDerivedFieldDisplay(bool isNew)
{
	const dbDateTime* dateField = (const dbDateTime*)mField;	// safe downcast
	if (isDirty() && dateField->isValid()) {
		if (dateField->formatMask().length()) {	// we have a format mask, so update the display
			copyFieldToLink();
		}
	}
	else {
		if (dateField->fieldIsVirtual() || (isNew && dateField->hasDefaultCalculator()) )
			copyFieldToLink();
	}
}


bool 
dbDateTimeLink2Editor::validToLeaveLink()
{
	assert(mAdapter);
	bool ret = dbDateTime::checkTime(mAdapter->getString());
	if (!ret) {
		oofYNC("Invalid date");
	}
	return true;
}
