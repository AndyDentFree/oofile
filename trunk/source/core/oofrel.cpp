// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database
// implementing relations

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFRELX
	#include "oofrelx.h"
#endif
#ifndef OOF_EXCEP
	#include "oofexcep.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// statics
OOF_RelMN*	OOF_RelMN::sCurrentlyBuilding = 0;
dbRelRefBase*	OOF_RelMN::sCurrentRHSref = 0;



// -------------------------------------------------------
//                     o o f R e l M N
// -------------------------------------------------------
OOF_RelMN::OOF_RelMN(dbRelRefBase& lhs, dbRelRefBase& rhs) :
	mProtoLHS(lhs),
	mProtoRHS(rhs),
	mReferences(0),
	mRelToLHS(0),
	mRelToRHS(0),
	mMaintainer(0)
{
	// either side usually would propagate deletes to the intermediate MN table
	// but they don't go past that table
	lhs.propagateRelatedDeletes(false);
	rhs.propagateRelatedDeletes(false);
}


OOF_RelMN::OOF_RelMN(const OOF_RelMN& rhs) :
	mProtoLHS(rhs.mProtoLHS),
	mProtoRHS(rhs.mProtoRHS),
	mReferences(0),
	mRelToLHS(rhs.mRelToLHS),	// maybe 0
	mRelToRHS(rhs.mRelToRHS),
	mMaintainer(0)  // won't be listening to new rhs table, so want to recreate
{
}


void
OOF_RelMN::incRefs()
{
	++mReferences;
}


void
OOF_RelMN::decRefs()
{
	if (mReferences)
		--mReferences;
	if (mReferences==0)
		delete this;
}


bool 
OOF_RelMN::cloningRelIs(dbRelRefBase* inRel)
{
	if (sCurrentlyBuilding==this) {
		assert(sCurrentRHSref==0);	// should never be called twice while building
		sCurrentRHSref = inRel;
		return true;
	}
	else
		return false;
}


bool
OOF_RelMN::RelateMN()
{
	assert(mRelToLHS);
	assert(mRelToRHS);
	mRelToLHS->relateFromRecord();
	bool ret = mRelToRHS->selectAllRelated();
	return ret;
}


dbTable*
OOF_RelMN::BuildRelatedTable(dbRelRefBase* cloner)
{
	sCurrentlyBuilding = this;
	
// cope with the fact that this link may have been declared "backwards"
// as far as the calling relationship is concerned. (ie: one link is used to describe
// the two directions of traversal)	
	dbRelRefBase *leftProto, *rightProto;
	if (cloner->fieldTable()->tableNumber()==mProtoLHS.relatedTableNumber()) {
		leftProto = &mProtoLHS;
		rightProto = &mProtoRHS;
	}
	else {	// backwards!
		leftProto = &mProtoRHS;
		rightProto = &mProtoLHS;
	}
	
	
// do some of the work in the normal dbRelRefBase::BuildRelatedTable()
// the BIG difference is that nobody from the LHS points to this object,
// unlike the dbRelRefBase's on each side of a relationship, which point to each other
	dbTable* middleClone = leftProto->fieldTable()->cloneTableWithoutSelection(); // could be either lhs or rhs
	middleClone->setSaveOption(dbTable::requireExplicitAndBuffer);
	
	mRelToLHS = (dbRelRefBase*) middleClone->field(leftProto->fieldNumber());
	mRelToLHS->completeCloneOfInverse(cloner->fieldTable());
	
// we now have a viable cloned table to manage the middle of the MN relationship
// so transition to the RHS is not so hard

	mRelToRHS = (dbRelRefBase*) middleClone->field(rightProto->fieldNumber());
	dbTable* ret = mRelToRHS->relatedTable();	// as if normal relationship
	
/*
At this point, the middle clone and far rhs clone tables have been constructed,
the middle clone's two relationship fields are initialised as is the far rhs
relationship field (which points back to the middle).

This MNLink is the "prototypical" one in that it was pointed to by the rhs table 
being cloned to manage the relationship. Thus, it is inappropriate for us to contain
the 'real' relationship pointers.

As part of the cloning procedure, the dbRelRefBase on the rhs will tell each MN link
that it is being cloned. Thus, we need a mechanism to track exactly which MN link
should bother to listen. We use sCurrentlyBuilding to tell us this, and cloningRelIs()
will record the relref in sCurrentRHSref. Thus as a final step, we need to cleanup these
statics, and clone ourself for the rhs.

Note that the copy will only be pointed to by the rhs of the relationship (the clone
managing the related selection). The lhs will continue to point to this prototypical 
instance.
*/
	assert(sCurrentRHSref);	// when rhs table is cloned above, it points to us
	OOF_RelMN* ourClone = new OOF_RelMN(*this);
	sCurrentRHSref->setMNlink(ourClone);	// this ref is the mRelToLHS of the far rhs clone
	
// cleanup time!
	sCurrentRHSref = 0;
	sCurrentlyBuilding = 0;
	mRelToLHS = 0;	// we don't need to know fields - our clone does!
	mRelToRHS = 0;
	
	return ret;
}


void 
OOF_RelMN::appendSelection(dbSelection* inSel)
{
// ASSERT - by the time we are called we are guaranteed that the selection is NOT
// already related, so avoid some checks to see if middle record exists
	dbTable*	middleTable = mRelToLHS->fieldTable();
	dbTable* rhsTable = mRelToRHS->relatedTable();
	assert(rhsTable);
	
	dbField* rhsJoinField = mRelToRHS->joinField();
	OOF_recordSelection* addingSel = (OOF_recordSelection*) inSel->internalSelection();	// unsafe downcast, safe only as long as we don't have another selection type
	unsigned long numAdding = addingSel->count();

	if (rhsJoinField) {
		stSaveSelection preserveRHS(*rhsTable);
		for (unsigned long i=0; i<numAdding; i++) {
			oidT rhsOID = addingSel->value(i);
				rhsTable->selectJustOID(rhsOID);
				if (rhsTable->count()==1) {
					middleTable->newRecord();
					mRelToLHS->updateRelValue();
					mRelToRHS->updateRelValue();
				}
				else {
	// the rhs has disappeared - do we care?
			} // get rhs record to copy join value
	// left unsaved as it should be cached until the parent is saved
		} // loop rhs selection we're adding links to
	} // rel to rhs is join
		
	else {
		for (unsigned long i=0; i<numAdding; i++) {
			middleTable->newRecord();
			mRelToLHS->updateRelValue();
			oidT rhsOID = addingSel->value(i);
			mRelToRHS->setOID(rhsOID);
		} // loop rhs selection we're adding links to
	}  // rel to rhs is pointer, so don't need to go and change rhs selectoin
}


/**
	Remove the selected records from the intermedate link file, immediately.
	\todo cache deletes so these are LIVE deletes, 
	unlike adds in OOF_RelMN::appendSelection where the related records are kept in the dirty cache
*/
void 
OOF_RelMN::removeSelection(dbSelection* inSel)
{
	dbTable*	middleTable = mRelToLHS->fieldTable();  // the table CONTAINING the rel, not to which it relates
	dbTable* rhsTable = mRelToRHS->relatedTable();
	assert(rhsTable);
	
	dbField* rhsJoinField = mRelToRHS->joinField();
	OOF_recordSelection* removingSel = (OOF_recordSelection*) inSel->internalSelection();	// unsafe downcast, safe only as long as we don't have another selection type
	unsigned long numRemoving = removingSel->count();

	if (rhsJoinField) {
		if (mRelToLHS->relateFromRecord()) {  // first get a selection from LHS
			dbSelection narrowingSel = middleTable->currentSelection();
			stSaveSelection preserveRHS(*rhsTable);
			for (unsigned long i=0; i<numRemoving; i++) {
				oidT rhsOID = removingSel->value(i);
					rhsTable->selectJustOID(rhsOID);
					if (rhsTable->count()==1) {
						if (mRelToRHS->relateFromRecord())	// ALL related record links, not just for lhs
	// now intersect against the lhs set that we got in advance and are whittling down					
							narrowingSel.intersection_with(*middleTable);
					}
					else {
		// the rhs has disappeared already so don't care about link record
				} // get rhs record to copy join value
			} // loop rhs selection we're removing links from
			if (narrowingSel.count()) {
				middleTable->setSelection(narrowingSel);	// finally, just the set to delete
				middleTable->deleteSelection();
			} // got a set to delete
		} // have any middle recs from lhs
	} // rel to rhs is join
		
	else {
		assert(!"Removing MN links with pointers not yet supported");
	}  // rel to rhs is pointer, so don't need to go and change rhs selectoin
}


void
OOF_RelMN::clearSelection()
{
	dbTable*	middleTable = mRelToLHS->fieldTable();
	middleTable->deleteSelection();
}


void
OOF_RelMN::makeRelMaintainer()
{
// an OOF_RelMN doesn't need to know the dbRelRefBase that directly
// owns it, so that object is passed in here
	if (!mMaintainer)
		mMaintainer = new dbRelMaintainer(mRelToRHS, this);
}


// -------------------------------------------------------
//              d b R e l R e f B a s e 
// -------------------------------------------------------
dbRelRefBase::dbRelRefBase(const char* fieldName, unsigned long maxLinks) : 
	dbField(fieldName),
	mMinLinks(0),
	mMaxLinks(maxLinks),
	mRelatedTableProto(0),
	mActualRelatedTable(0),
	mInverseFieldNumber(0),
	mJoinField(0),
	mPropagatesDeletes(false),
	mInverseField(0),
	mMNlink(0)
{
}

dbRelRefBase::dbRelRefBase(const dbRelRefBase& rhs) : 
				dbField(rhs),
				mMinLinks(rhs.mMinLinks),
				mMaxLinks(rhs.mMaxLinks),
				mRelatedTableProto(rhs.mRelatedTableProto),
				mActualRelatedTable(0),
				mInverseFieldNumber(rhs.mInverseFieldNumber),
				mJoinField(rhs.mJoinField),  // see postCloneTableCleanup
				mPropagatesDeletes(rhs.mPropagatesDeletes),
				mInverseField(0),
				mMNlink(0) 
{
	if (rhs.mMNlink) { 
		bool thisIsTheRelForWhichWeAreBeingCloned = rhs.mMNlink->cloningRelIs(this); // link will later fix us up
		if (!thisIsTheRelForWhichWeAreBeingCloned)
			setMNlink(rhs.mMNlink);
	}
}


dbRelRefBase::~dbRelRefBase()
{
	if (mMNlink) 
		mMNlink->decRefs();
}


dbField*
dbRelRefBase::clone() const 
{
	return new dbRelRefBase(*this);
}


void
dbRelRefBase::postCloneTableCleanup() 
{
	if (mJoinField) {
		// by this stage we are guaranteed that all fields in the table exist and 
		// have been registered with the table, so can pull out our real field from
		// the dictionary.
		mJoinField = mTable->field(mJoinField->fieldNumber());
	}

}


void 
dbRelRefBase::SetCommonRelationshipFields(dbRelRefBase& rhs)
{
	assert(!mInverseField);  // good check to see multiple calls
	mInverseField = &rhs;  // will be changed later to point to actual inverse but useful for lookups
	mInverseFieldNumber = rhs.fieldNumber();
	mRelatedTableProto = rhs.fieldTable();
	if (!rhs.isRef() && !mJoinField)
		index(kIndexed);  // unless a join field later specified
#ifdef OOF_DEBUG
	if ((mJoinField==0) != (rhs.mJoinField==0))
		dbConnect::raise(stringstream() << flush << "dbRelRefBase::SetCommonRelationshipFields\n"
			<< "Relationship specified with join field on only one side, between:\n"
			<< fieldName() << " and " << rhs.fieldName()
		);
#else
	assert((mJoinField==0) == (rhs.mJoinField==0));
#endif
}


/**
	Called by dbRelationship to tie two instances of dbRelRefBase together.
	Makes an actual relationship occur between the known tables in a dbConnection,
	finishing the work of declaring dbRelRef or dbRelSet members in the tables.
	
	Sets the relational integrity up by default to make sense for 
	Western users. The left table owns the right table unless the relationship
	is back to front. ie: an N:1 doesn't propagate deletes.
	Assumes ctor initialises mPropagatesDeletes false.
	If you have explicitly set mPropagatesDeletes true already it
	will retain that setting.
	
	At this level we are working with the virtual and base methods of the dbRelRefBase
	
	\note After this is called (ie: after dbRelationship functor) you can
	call propagateRelatedDeletes explicitly. eg: if you had a 1:1 relationship
	just to avoid loading large records, you would want to propagate deletes.
	We err on the side of caution in not propagating by default when it is hard
	to tell.
*/
void 
dbRelRefBase::relatesTo(dbRelRefBase& rhs)
{
	SetCommonRelationshipFields(rhs);
	const bool wasLHS = !rhs.CompletelySpecified();
	if (wasLHS) { 
		rhs.relatesTo(*this);  // recurse to setup RHS "rhs" in this call will be the original left
	}
	// finish init now both sides of relationship are pointing at each other
	if (wasLHS) {
		if (isOneToMany())
			propagateRelatedDeletes(true);  // ie: 1:N
	}
	else {
		if (!pointsToCorrectTableType() || !rhs.pointsToCorrectTableType() ) {
			dbConnect::raise(stringstream() << flush << "dbRelRefBase::SetCommonRelationshipFields\n"
				<< "incorrectly specified relationship, check the table type on both sides of\n"
			<< fieldTable()->defaultName() 
			<< " to "
			<< rhs.fieldTable()->defaultName() 
		);
		
		}
	}  // checks for !wasLHS
}


void 
dbRelRefBase::joinField(dbField& joinBy)
{
	assert(joinBy.fieldTable() == mTable);
	mJoinField = &joinBy;
	index(kNotIndexed);  // only indexed if an N-ary side of an oid relationship
}


void 
dbRelRefBase::BuildRelatedTable()
{
	#ifdef OOF_DEBUG
		if (!mRelatedTableProto)
			dbConnect::raise(stringstream() << flush << "Relationship field No " << fieldNumber() 
			<< " in table " << tableName() << " lacks a rhs");
	#else
		assert(mRelatedTableProto);
	#endif
	assert(mActualRelatedTable==0);  // should only be called in this circumstance
	if (mMNlink)
		mActualRelatedTable = mMNlink->BuildRelatedTable(this);
	else
		mActualRelatedTable = mRelatedTableProto->cloneTableWithoutSelection();
	assert(mActualRelatedTable);
	mActualRelatedTable->setSaveOption(dbTable::requireExplicitAndBuffer);
	mActualRelatedTable->setTableValid(false);  // only validates selection when referred to, lazy evalution of actual relationship
	
	mInverseField = inverse();
	assert(mInverseField);
	
	assert( (mJoinField==0) == (mInverseField->joinField()==0) );  // either both zero or both pointers
	mInverseField->completeCloneOfInverse(this);
}


void
dbRelRefBase::completeCloneOfInverse(dbRelRefBase* cloner) 
{
	mActualRelatedTable = cloner->fieldTable();
	
	if (mMNlink) {  // we have another rel that links to a middle table used to manage the MN sets
	// it is our caller's responsibility to make sure this is after the other rel has been linked
		assert(mTable->mRelToLHS);	// the other rel
		mTable->mRelToLHS->unsubscribe();	// tell the other rel not to react (to changes in the middle table)
	// a table only needs to know mRelToLHS when the table is a clone managing a related selection
	}
	mTable->mRelToLHS = this;  
	subscribeTo(mActualRelatedTable); 
	// depend on the lhs of the rel for things like context changes, selection changes
	mInverseField = cloner;
}


void
dbRelRefBase::completeCloneOfInverse(dbTable* clonerTable) 
{
	mActualRelatedTable = clonerTable;
	mTable->mRelToLHS = this;  
	
	if (!mMNlink) {
		subscribeTo(mActualRelatedTable); 
	}
	mInverseField = inverse();
// also make the inverse point at us	
	// assert(!mInverseField->mActualRelatedTable);
	mInverseField->mActualRelatedTable = mTable;
	mInverseField->mInverseField = this;
}


/**
	Point this relationship at the object which maintains the link.
	Calling setMNlink completes specifying the relationship - we 
	assume the OOF_RelMN class manages the other relationships used
	to provide the 1:N N:1 that implement the M:N.
	\note doesn't propagateRelatedDeletes because the other relationships
	that implement the link take care of propagation.
	However we could call  propagateRelatedDeletes(true) if we wanted
	deleting one side to delete the other automatically.
*/
void 
dbRelRefBase::setMNlink(OOF_RelMN* inLink) 
{
	if (mMNlink)
		mMNlink->decRefs();	
	mMNlink = inLink;
	inLink->incRefs();
	propagateRelatedDeletes(false);
}


bool 
dbRelRefBase::relateRecord(bool broadcastChange)
{
	assert(mInverseField);
	assert(mActualRelatedTable);  // can't get here without being complete
	bool ret = mInverseField->relateFromRecord(false);
	if (broadcastChange) {
		mActualRelatedTable->broadcast(OOFmsg_ChangeSelection);
	}
	return ret;
}


bool 
dbRelRefBase::selectAllRelated(bool broadcastChange)
{
	if (!mActualRelatedTable)
		BuildRelatedTable();

	mTable->setTableValid(true);	// don't care if loaded or not, we're using it
	unsigned long numRecs = mTable->count();

	bool ret = false;
	if (numRecs==0)
		mActualRelatedTable->selectNone();
	else {
		mTable->start();
		relateRecord(false);	// get rhs selection for 1st record
		if (numRecs>1) {
			assert(mActualRelatedTable);
			dbSelection combiningSels = mActualRelatedTable->currentSelection();
			numRecs--;
			for (unsigned long i=0; i < numRecs ; i++) {
				mTable->next();
				if (relateRecord(false)) {	// updates the clone in mActualRelatedTable
					combiningSels.union_with(*mActualRelatedTable);
				}
			}
			mActualRelatedTable->setSelection(combiningSels);
		}
		ret = !mActualRelatedTable->isEmpty();
	}  // any recs

	if (broadcastChange)
		mActualRelatedTable->broadcast(OOFmsg_ChangeSelection);
	return ret;
}


/**
	Used to load a related selection.
	Mainly called from dbTable::ensureRecordLoaded and dbTable::MakeTableValid.
	
	It may seem a little backwards but think of the normal context of using this method.
	We have already loaded a current record in the LHS of a relationship and now want the related
	record or selection. Remember that OOFILE allows you to iterate a table without forcing 
	instantiation of relationships. Only when you refer to a related selection or specific
	fields in a related record is the relationship instantiated.
	
	There are three distinct cases dealt with by this method:
	-# an M:N relationship, which is implemented by our member mMNlink.
	-# a 1:1, 1:N or N:1 join relationship which is handled by 
	   OOF_simpleRecordBackend::loadRelatedContextJoiningFromTo (normally via a search)
	-# a 1:1, 1:N or N:1 pointer relationship using record OID's in a search or direct lookup
	
	If mInverseField (ie: other side of relationship pointing to us) isRef() then
	it contains a copy of the OID of a record in our table and we have a direct lookup (not
	caring if this is 1:1 or N:1 situation).
	
	Otherwise it is 1:N and the OID on the other side is its record's OID, used as a key
	to search our relationship fields.
	\todo explore using virtual overrides of relateFromRecord for user-defined relationships
*/
bool 
dbRelRefBase::relateFromRecord(bool broadcastChange)
{
	mTable->setTableValid(true);	
OOF_DEBUG_LOG(stringstream() << flush << "dbRelRefBase::relateFromRecord\t" 
	<< hex << (void*)this 
	<< "\tfrom table: " << mActualRelatedTable->tableName() << " (" << (void*)mActualRelatedTable << ')'
	<< "\tour table: " << mTable->tableName() << " (" << (void*)mTable << ')'
);

	bool ret = false;
	if (mMNlink) {
		ret = mMNlink->RelateMN();
	}
	else {
		if (mJoinField)  // is a join relationship
		{
			ret = mBackend->loadRelatedContextJoiningFromTo(mInverseField->joinField(), mJoinField);
		}
		else {
			if (mInverseField->isRef()) {
				oidT theOID = mInverseField->OID();		// reading value from a field ensures rec loaded
				if (theOID) 
					ret = mBackend->selectJustOID(theOID);
				else {
					mBackend->selectNone();
					ret = false;
				} 
			}  // 1:1 or N:1
			else {
				oidT oidToFind = mInverseField->fieldTable()->currentOID();
				ret = mBackend->searchEqual(this, &oidToFind);
			}  // 1:N
		}  // join vs pointer 1:1, 1:N or N:1
	} // M:N relationship
	mTable->setTableValid(true);  // should be redundant but possibly invalidated by above searches	
	if (broadcastChange)
		mTable->broadcast(OOFmsg_ChangeSelection);
	return ret;
}



unsigned long
dbRelRefBase::countAllRelated()
{
	unsigned long numRecs = mTable->count();

	if (numRecs==0)
		return 0;
		
	// NOT YET IMPLEMENTED - optimise these - just need counts, not selections
	// BUT may not be that simple - must NOT have duplicates
	mTable->start();
	if (!mActualRelatedTable)
		BuildRelatedTable();
	relateRecord(false);	// get rhs selection for 1st record
	if (numRecs==1) 
		return mActualRelatedTable->count();

	dbSelection combiningSels = mActualRelatedTable->currentSelection();
	numRecs--;
	for (unsigned long i=0; i < numRecs ; i++) {
		mTable->next();
		relateRecord(false);	// updates the clone in mActualRelatedTable
		combiningSels.union_with(*mActualRelatedTable);
	}
	return combiningSels.count();
}


unsigned long 
dbRelRefBase::CountAllRelatedIn(dbTable* rhs)
{
	unsigned long numRecs = mTable->count();

	if (numRecs==0)
		return 0;
		
	mTable->start();
	if (!mActualRelatedTable)
		BuildRelatedTable();
	relateRecord(false);	// get rhs selection for 1st record
	if (numRecs==1) 
		return mActualRelatedTable->count();

	dbSelection combiningSels = mActualRelatedTable->currentSelection();
	if (numRecs>1)  {
		numRecs--;
		for (unsigned long i=0; i < numRecs ; i++) {
			mTable->next();
			relateRecord(false);	// updates the clone in mActualRelatedTable
			combiningSels.union_with(*mActualRelatedTable);
		}
	}
	combiningSels.intersection_with(*rhs);
	return combiningSels.count();
}


OOF_fieldTypes 
dbRelRefBase::fieldType() const
{
	return relationshipField; 
}


OOF_fieldTypes 
dbRelRefBase::nativeType() const
{
	if (mJoinField)
		return fieldType();
	else
		return uLongField;
}


bool 
dbRelRefBase::fieldIsVirtual() const
{
	if (mJoinField)
		return true;
	else
		return false;  // stores an OID
}


unsigned long dbRelRefBase::fieldStorageLen() const
{
	if (mJoinField)
		return 0;  // no storage in the record
	else
		return sizeof(oidT); // stores an OID
}


oidT
dbRelRefBase::OID() const
{
	assert(!mJoinField);  // shouldn't be called this is a join relationship
	oidT *readFrom = (oidT *)mBackend->getFieldReadFrom(mFieldNumber);
	oidT currentOID = 0;
	if (readFrom)
		currentOID = *readFrom;
	return currentOID;	
}


void 
dbRelRefBase::OID(oidT oid)
{
	oidT currentOID = OID();
	if (oid!=currentOID) { 
		oidT *writeTo = (oidT *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = oid;	
		mTable->markDirty();
	}
}


/**
	Set relationship to propagate deletes, ie: owns related records.
	\warning It is tempting to only allow this if (mTable->inDeclarativePhase())
	but that check is not sufficient - during cloning of a table it is possible
	and legal to call propagateRelatedDeletes, as happens in ooftst42 with 
	the MN link.
	\todo see if we can add back the commented out test below safely
	\todo propagate deletes for N:1 with pointer relationships.
*/
void
dbRelRefBase::propagateRelatedDeletes(bool prop)
{
	assert(mTable);
/*
This test is not sufficient as we can also be called whilst cloning of a table
is occurring. We need to be able to tell that cloning is complete to enforce this
check.
	if (!mTable->inDeclarativePhase()) {
#ifdef OOF_DEBUG
		dbConnect::raise(
			stringstream() << flush << "propagateRelatedDeletes called after opening, on  " 
			<< mTable->tableName() << ':' << fieldName()
		);
#endif
		return;  // can't change once open
	}
*/	
	
	if (prop) {	
		if (isOneToMany() || isOneToOne())
			mPropagatesDeletes = true;
		else {
			if (isJoin())
				mPropagatesDeletes = true;
			else {
		#ifdef OOF_DEBUG
				dbConnect::raise(
					stringstream() << flush << "propagateRelatedDeletes can't handle pointer relationships for N:1 or N:M  " 
					<< mTable->tableName() << ':' << fieldName()
				);
		#endif
			}
		}  // may not handle this kind
	}
	else
		mPropagatesDeletes = false;  // always accept them turning it off
}


bool 
dbRelRefBase::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
// we are on the RHS of a relationship listening to broadcasts from the immediate LHS table
	switch (msg) {
	case (OOFmsg_ChangeSelection) :
	case (OOFmsg_ChangeContext) :
	case (OOFmsg_UnloadRecord) :
	#ifdef FIX_FOR_GABY_MULTIPLE_LEVEL_AUTO_SAVE
		mTable->ContextChange();  
		mTable->setTableValid(false); 
	#else
		mTable->setTableValid(false);  // don't just unload the record - there are NO related recs!
		mTable->unloadRecord();
		mTable->mBackend->unloadCache();
	#endif
		break;
		
		
	case (OOFmsg_DeleteRecord) :
		if (mInverseField->propagatesDeletes())  // it propagates to us
			mTable->deleteSelection();
		break;
		
	case (OOFmsg_SaveRecord) :
	// senderDefined is isNewRecord()
		if (mTable->isTableValid() && mTable->isRecordLoaded() && mTable->isDirty()) {
			mTable->saveRecord();  // triggers probably save of multiple cached related records
		   	if (!mJoinField) {	
				if (mInverseField->isRef())  // rhs of something:1, almost certainly 1:1
					// tell our inverse to point straight at us, now we have an oid
					mInverseField->OID(mTable->currentOID());
				else  // lhs of N:something
					assert (isRef());  // lhs of N:1
					// other side's OID is itself, used to search us so it can be left set at 0
					// Can't end up here with N:N as we handle them with a separate pair
					// of relationships, through intermediate table, which has it's own
					// traversal path that catches this save message
			}
		}
		break;
		
	case (OOFmsg_BroadcasterClosing) : 
		mListensTo = 0;  // won't be around to tell it that we're being deleted
		delete mTable;  // yes, delete the table! it's a clone on the heap, managing this relationship
		break;
		
	case (OOFmsg_SuspendSorting) :
		mTable->suspendSorting();  // our LHS has just told us
		break;
		
	case (OOFmsg_ResumeSorting) :
		mTable->resumeSorting();  // our LHS has just told us
		break;

	default :
		oofSingleListener::receiveMsg(msg, senderDefined);
	};
	return true;
}


/**
	Propagate values to maintain a relationship.
	Either copies a join key or stores an OID for pointer traversal.
	\note updating the lhs relationship is performed in receiveMsg, AFTER our saveRecord
	in case our table's OID is required by the rhs.
*/
void 
dbRelRefBase::updateRelValue()
{
	assert(mActualRelatedTable);  // we are on RHS of relationship
	if (mJoinField) {
		if (!mMNlink)	// if this is the direct rel to the lhs we don't copy the ID, only the rel to the MN link file in the middle copies
			mJoinField->copyValueIfDifferent(mInverseField->joinField());
	}
	else { // grab an OID
		if (isRef())  // rhs of something:1 rel
			OID(mInverseField->fieldTable()->currentOID());  // we point straight at our inverse
		else  // rhs of something:N rel
			if (mInverseField->isRef())  // rhs of 1:N rel
				OID(mTable->currentOID());  // store our own table's OID to search inverse
			else  // N:N - need independent OID
				RAISE_EXCEPTION(
					oofE_General("N:N pointer relationships not yet supported")  // later get a relationship OID
				);
	}
}


void 
dbRelRefBase::setOID(oidT inOID)
{
	assert (!mJoinField);	// NOT a join
	assert (isRef()); // rhs of something:1 rel
	OID(inOID); 
}


/**
	Break the link from another table pointing at us.
	Doesn't need to do anything for join relationships as we deem it
	too dangerous to go and blithely clear key fields which may have
	multiple uses.
	\br
	Called when update record on rhs
	OR when reassign link
	\todo if save type of rhs is requireExplicitAndBuffer
	then need to cache some kind of undo comand in case need to cancel delete.
	\todo reassess need to clear join key fields - maybe that is needed in some cases
	to retain referential integrity
*/
void 
dbRelRefBase::breakRelLink()
{
	if (!mJoinField) {
		if (mInverseField->isRef())  // we are rhs of 1:1 or N:1 rel
			mInverseField->OID(0);  // stop lhs pointing to us
		else  
			if (!isRef())// M:N - need independent OID
				RAISE_EXCEPTION(
					oofE_General("M:N relationships not yet supported")
				);  // later get a relationship OID
	}
}


/**
	Change an existing relationship to point to a different field.
	\warning 2001/10/24 this is a user contribution which I don't think works.
	\todo write sample to see if this works and evaluate if we should be saving rhs.
	\ingroup experimental
*/
void 
dbRelRefBase::changeRelationshipTo(dbRelRefBase& newRhs)
{
	assert(&newRhs != this);
	if (!mJoinField) {
		if (!mActualRelatedTable)
			BuildRelatedTable();
		if (mInverseField->isRef())  { // rhs of 1:1 or N:1 rel
		//	newRhs.OID( fieldTable()->currentOID() );  // rhs points to US
		//	OID( newRhs.fieldTable()->currentOID() );  	// we point to RHS
			assert(!mTable->isNewRecord()); // only work on a saved record
			// begin transaction here, end it when caller calls saveRecord()
			if (OID()) {
				relateRecord(false);
				mInverseField->OID(0);  // related no longer points to us
				mInverseField->fieldTable()->saveRecord(); // is this OK?
			}
			OID(newRhs.OID()); // we point to new related
			if (newRhs.OID()) {
				newRhs.OID(0); // old parent no longer points there
				newRhs.mTable->saveRecord();
			}
			if (relateRecord(false))
				mInverseField->OID(mTable->currentOID()); // new related points to us
		}
		else  
			if (!isRef())// M:N - need independent OID
				RAISE_EXCEPTION(
					oofE_General("M:N relationships not yet supported")  // later get a relationship OID
				);
	}
}



void 
dbRelRefBase::describe(ostream& os) const
{
	if (CompletelySpecified()) {
		os << "field: " << fieldName() << " is a ";
		dbRelRefBase* inverseProto = (dbRelRefBase* )mRelatedTableProto->field(mInverseFieldNumber);    // safe downcast
	// always exists regardless of relationship status
		
		if (inverseProto->isRef())
			os << "1";
		else {
			if (isRef())
				os << "N";
			else
				os << "M";	// is M:N by convention, not N:N
		}
		if (isRef())
			os << ":1";
		else
			os << ":N";
		
		os << " relationship ";
		
		if (mJoinField)
			os << "joining across field: " << mJoinField->fieldName();
	
		if (propagatesDeletes())
			os << " and propagating deletes to related records";
	}
	else {
		os << "incompletely specified relationship";
	}
	os << endl;
}


/**
	Default base method.
	Macro REL_REF_BODY in oofmacro.h defines virtual overrides for each subclass
	that do an actual test using the macro-generated dbTable::defaultName() override.
	
	\note it is possible for people to create relationships other than through the
	declarative macros so we need to keep the base dbRelRef and dbRelSet usable, which
	is why this method isn't a pure virtual.
	
	\return true always, macro-generated override returns true OR false
*/
bool 
dbRelRefBase::pointsToCorrectTableType() const
{
	return true;
}


bool 
dbRelRefBase::fieldIsSameTypeAs(const dbField* rhs) const
{
	if (dbField::fieldIsSameTypeAs(rhs)) {
		const dbRelRefBase* rhsField = (const dbRelRefBase*) rhs;	// safe downcast
		return ( mRelatedTableProto && rhsField->mRelatedTableProto
			&&    mRelatedTableProto->tableName() == rhsField->mRelatedTableProto->tableName()
			&&    mRelatedTableProto->isCopyCompatibleWith(rhsField->mRelatedTableProto)
		);
	}
	else
		return false;
}


void 
dbRelRefBase::extract(ostream& os) const
{
	if (mJoinField) {
		mJoinField->extract(os);
	}
	else {
		if (isRef())
			os << mRelatedTableProto->tableName() << "->[" << OID() << ']';  // if we are the destination of 1:something link then will contain parent OID
		// if we are the 1 side then output parent OID
		else
			os << '[' << mTable->currentOID() << "]->" << mRelatedTableProto->tableName();
		// thus dumps of 1-1 or 1-N links should point at each other!
	}
}


/**
	Override to return related table name as sensible default.
	Usually we don't bother naming individual relationships
	however you are able to call setName or pass in a name
	to the ctor. When modelling more than one relationship
	between the same classes we recommend naming the relationship
	variables.
	\return specified name or name of related table.
*/
const OOF_String& 
dbRelRefBase::fieldName() const
{
// override as the table name could change at a later date
// than our initialisation - it's very messy getting the ultimate name
// and this is NOT a function that needs to be particularly efficient
	const oofString& tempName = dbField::fieldName();
	if (CompletelySpecified() && tempName.isEmpty()) {
			return mRelatedTableProto->tableName();
	}
	else
		return tempName;  // returns empty name if not completely specified
		// so that we have an empty oofString to safely return via reference
}


tableNumT 
dbRelRefBase::relatedTableNumber() const
{
	assert(mRelatedTableProto);
	return mRelatedTableProto->tableNumber();
}


dbTable* 
dbRelRefBase::relatedTable()
{
	if (!mActualRelatedTable)
		BuildRelatedTable();
	return mActualRelatedTable;
}


// -------------------------------------------------------
//              d b R e l R e f
// -------------------------------------------------------

/**
	ctor allowing you to pass field name for relationship.
	Inits dbRelRefBase with limit of only 1 related value allowed.
*/
dbRelRef::dbRelRef(const char* fieldName) : 
	dbRelRefBase(fieldName, 1 /* max related record */),
	mCachedSearchTable(0) 
{}


dbRelRef::~dbRelRef()
{
	delete mCachedSearchTable;
}


dbRelRef::dbRelRef(const dbRelRef& rhs) :
	dbRelRefBase(rhs),
	mCachedSearchTable(0) 
{
}


const dbRelRef& 
dbRelRef::operator=(const dbRelRef& rhs)
{
	delete mCachedSearchTable;
	mCachedSearchTable = 0;
	dbRelRefBase::operator=(rhs);
	return *this;
}


/**
	Search expression to find records which have a relationship pointing nowhere.
	
	Pointer relationships on the rhs of a 1:something relationship store 
	the OID of their related record, for direct lookup.
	
	Sample ooftst46 uses this to search for the Root of a tree, being something with
	no Parent.
	
	For the current record \see dbRelRefBase::relateRecord
	
	\todo extend to handle join relationships, which implies finding records
	with no join key as well as those where the join key has no match.
*/
dbQueryBinary	
dbRelRef::nullRelationship() const
{
	assert(!mJoinField);  // only pointer relationships right now
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralLong(0));
}


/**
	Set our link to a single record to the specified OID.
	Much faster to use if we know we are using pointer relationships.
	\todo at least have an assert that it's valid
*/
bool 
dbRelRef::setRelatedRecord(oidT inOID)
{
	OID(inOID);
	if (mActualRelatedTable)
		mActualRelatedTable->setTableValid(false); // so next time we want something related will get correct
	return inOID!=0;
}


/**
	Set our link to a single record to the specified record.
	Can cope with both join and pointer relationships.
*/
bool 
dbRelRef::setRelatedRecord(dbTable* rhsTable)
{
	dbTable* inverseTable = relatedTable();  // force instantiation if never before traversed relationship
	assert(rhsTable && inverseTable->tableNumber()==rhsTable->tableNumber());
	if (rhsTable->isEmpty())
		return false;
		
	bool settoRHS = false;
	if (isJoin()) {
		dbField* inverseJoinKey;
		if (inverseTable==rhsTable)
			inverseJoinKey = mInverseField->joinField();
		else
			inverseJoinKey = mInverseField->equivalentFieldFromTable(rhsTable);			
		mJoinField->copyValueIfDifferent(inverseJoinKey);  // simply update key fields
	}
	else {
		const oidT rhsOID = rhsTable->currentOID();
		OID(rhsOID);
	}
	inverseTable->setTableValid(false); // so next time we want something related will re-load record
	return true;  // there must have been something on rhs
}


/**
	Set our link to a single record found by the search expression.
	\note we are guaranteed that the expression will NOT be on our 
	cached search table so go ahead and convert it.
	
	\todo something simpler - this seems very complicated!
*/
bool 
dbRelRef::setRelatedRecordMatching(const dbQueryClause& inSearch)
{
	bool ret = false;
	if (!mCachedSearchTable)
		mCachedSearchTable = relatedTable()->cloneTableWithoutSelection();
	
	dbQueryClause* convertedSearch = 0;
	try {
		const dbQueryClause* searchBy = 0;
		convertedSearch = inSearch.cloneReplacingTable(relatedTable(), mCachedSearchTable);
		if (convertedSearch)
			searchBy = convertedSearch;
		else
			searchBy = &inSearch;  // use original if didn't change
		if (mCachedSearchTable->search(searchBy)) {
			mCachedSearchTable->start();  // load record to get the OID!
			if (mJoinField) {
				dbField* cachedJoinField = mCachedSearchTable->field( mInverseField->joinField()->fieldNumber() );
				mJoinField->copyValueIfDifferent(cachedJoinField);
			}
			else
				OID(mCachedSearchTable->currentOID());
			ret = true;
		}
		else
			if (mJoinField) 
				mJoinField->clear();
			else
				OID(0);  // if not found, clear current OID!
	}
	catch (...) {
		delete convertedSearch;
		throw;
	}	
	delete convertedSearch;
	return ret;
}


// -------------------------------------------------------
//              d b R e l a t i o n s h i p 
// -------------------------------------------------------
dbRelationship::dbRelationship(dbRelRefBase& lhs, dbRelRefBase& rhs) :
	mLHS(lhs),
	mRHS(rhs)
{
	lhs.relatesTo(rhs);
}


dbRelationship::dbRelationship(dbRelRefBase* lhs, dbRelRefBase* rhs) :
	mLHS(*lhs),
	mRHS(*rhs)
{
	lhs->relatesTo(*rhs);
}


dbRelationship::dbRelationship(dbRelRefBase& lhs, dbRelRefBase& rhs, dbField& lhsJoin, dbField& rhsJoin) :
	mLHS(lhs),
	mRHS(rhs)
{
	lhs.relatesTo(rhs);
	lhs.joinField(lhsJoin);
	rhs.joinField(rhsJoin);
}


dbRelationship::dbRelationship(dbRelRefBase* lhs, dbRelRefBase* rhs, dbField* lhsJoin, dbField* rhsJoin) :
	mLHS(*lhs),
	mRHS(*rhs)
{
	lhs->relatesTo(*rhs);
	lhs->joinField(*lhsJoin);
	rhs->joinField(*rhsJoin);
}


/**
	Finish specifying an M:N relationship by pointing out from middle table to both sides.
	Assumes a pair of 1:N relationships have been created from the left and right tables
	to the middle table which maps the M:N.
	We can create an M:N link directly between the left and right tables usng a dbRelationship
	but need to not only specify the end-points but also the middle via linkMNvia().
*/
void
dbRelationship::linkMNvia(dbRelRefBase& lhsLink, dbRelRefBase& rhsLink)
{
#ifdef OOF_DEBUG
	if (lhsLink.fieldTable() != rhsLink.fieldTable()) {
		dbConnect::raise(stringstream() << flush << "dbRelationship::linkMNvia\n"
			<< "incorrectly specified relationship, should be same table on both sides, NOT\n"
			<< lhsLink.fieldTable()->tableName() 
			<< " to "
			<< rhsLink.fieldTable()->tableName() 
		);
	}
#else
	assert(lhsLink.fieldTable() == rhsLink.fieldTable());
#endif
	OOF_RelMN* link = new OOF_RelMN(lhsLink, rhsLink);	// ref counted, deleted by the following
	mLHS.setMNlink(link);
	mRHS.setMNlink(link);
	
// SEE dbRelRefBase::SetCommonRelationshipFields for test that ensures both
// sides of relationship are joins or not

/*  96/08/14 not sure this is necessary. If the link file is via join, there is no
             need for the direct link to be specified as a join
             
// ENSURE both the rel to the rhs and the rel to the link file are joins/not
#ifdef OOF_DEBUG
	if ((mLHS.joinField()==0) != (lhsLink.joinField()==0))
		dbConnect::raise(stringstream() << flush << "dbRelationship::linkMNvia\n"
			<< "M:N Relationship specified with join field on only one of the link and the actual relationship:\n"
			<< mLHS.fieldName()
		);

	if ((mRHS.joinField()==0) != (rhsLink.joinField()==0))
		dbConnect::raise(stringstream() << flush << "dbRelationship::linkMNvia\n"
			<< "M:N Relationship specified with join field on only one of the link and the actual relationship:\n"
			<< mRHS.fieldName()
		);
#else
	assert((mLHS.joinField()==0) == (lhsLink.joinField()==0));
	assert((mRHS.joinField()==0) == (rhsLink.joinField()==0));
#endif
*/
}


void
dbRelationship::linkMNvia(dbRelRefBase* lhsLink, dbRelRefBase* rhsLink)
{
	linkMNvia(*lhsLink, *rhsLink);	
}


// -------------------------------------------------------
//           d b R e l M a i n t a i n e r
// -------------------------------------------------------
dbRelMaintainer::dbRelMaintainer(dbRelRefBase* relToRHS, OOF_RelMN* linkWeAlert) :
	oofSingleListener(relToRHS->relatedTable()),
	mLink(linkWeAlert)
{
}


dbRelMaintainer::~dbRelMaintainer()
{
}


bool
dbRelMaintainer::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	switch (msg) {
	case (OOFmsg_AppendSelection) :
		mLink->appendSelection((dbSelection*) senderDefined);
		break;
		
	case (OOFmsg_RemoveSelection) :
		mLink->removeSelection((dbSelection*) senderDefined);
		break;
		
	case (OOFmsg_ClearSelection) :
		mLink->clearSelection();
		break;
			
	default :
		oofSingleListener::receiveMsg(msg, senderDefined);
	
	}; 
	return true;
}

