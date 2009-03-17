#ifndef H_OOFREL
#define H_OOFREL
// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database

#ifndef H_OOF2
	#include "oof2.h"
#endif

class OOF_RelMN;

/**
	Parent for any field that is a relationship to another table.
	Initialisation by subclasses with maxLinks==1 or >1 indicates 
	nature of relationship.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRelRefBase : public dbField, public oofSingleListener
{
public:
	dbRelRefBase(const char* fieldName=0, unsigned long maxLinks=0);
	dbRelRefBase(const dbRelRefBase& rhs);
	virtual ~dbRelRefBase();
	virtual dbField* clone() const;
	
	virtual void postCloneTableCleanup();
	void completeCloneOfInverse(dbRelRefBase* cloner);
	void completeCloneOfInverse(dbTable* clonerTable);
	void setMNlink(OOF_RelMN*);
	OOF_RelMN* getMNlink() const;
	
// finish specifying the relationship
	void relatesTo(dbRelRefBase&);
	
	void joinField(dbField&);
	dbField* joinField() const;
	bool propagatesDeletes() const;
	void propagateRelatedDeletes(bool prop=true);
	
/// \name mandatory dbField overrides
//@{
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
//@}
	
/// \name reflective operations
//@{
	virtual bool fieldIsSameTypeAs(const dbField*) const;
	virtual const OOF_String& fieldName() const;
	virtual void describe(ostream&) const;
	virtual void extract(ostream&) const;
	virtual OOF_fieldTypes nativeType() const;
	virtual bool fieldIsVirtual() const;
	dbRelRefBase* inverse() const;
	dbTable* relatedTable();
	tableNumT relatedTableNumber() const;
	bool isRef() const; 
	virtual bool pointsToCorrectTableType() const;	
//@}
	
/// \name simple user-level queries as to type of relationship
//{@
	bool isOneToMany() const;
	bool isManyToOne() const;
	bool isOneToOne() const;
	bool isManyToMany() const;
	bool isJoin() const;
	bool isRelBackToParent() const;
//@}
	
/// \name perform the relationship traversal
//{@
	unsigned long countAllRelated();	// note macros define countAllRelatedIn()
	bool relateRecord(bool broadcastChange=true);
	bool selectAllRelated(bool broadcastChange=true);
	bool relateFromRecord(bool broadcastChange=true);
//@}

/// \name relationship manipulation
//@{
	void updateRelValue();		// update link for rhs
	void setOID(oidT);
	void breakRelLink();	// maintain referential integrity
	void changeRelationshipTo(dbRelRefBase& newRhs);
//@}	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);

protected:
	unsigned long CountAllRelatedIn(dbTable*);	// used by macros
	void	BuildRelatedTable();
	bool CompletelySpecified() const;
	void SetCommonRelationshipFields(dbRelRefBase&);
	oidT OID() const;
	void OID(oidT);

// data storage
	unsigned long mMinLinks, mMaxLinks;
	dbTable* mRelatedTableProto;  // the prototype of the related table
	dbTable* mActualRelatedTable;  // the cloned table managing the related selection
	fieldNumT	mInverseFieldNumber;
	dbField* mJoinField;
	bool mPropagatesDeletes;
	dbRelRefBase* mInverseField;
	OOF_RelMN*	mMNlink;	// owned, ref-counted
};


/**
	Relationship field to 1 or 0 instances.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRelRef : public dbRelRefBase
{
public:
	dbRelRef(const char* fieldName=0);
	virtual ~dbRelRef();
	dbRelRef(const dbRelRef&);
	const dbRelRef& operator=(const dbRelRef&);

	dbQueryBinary	nullRelationship() const;
	bool setRelatedRecord(oidT);
	bool setRelatedRecord(dbTable*);
	bool setRelatedRecordMatching(const dbQueryClause&);
	
protected:
	dbTable*	mCachedSearchTable;  // owned	
};


/**
	Relationship field to set of 0..N instances.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRelSet : public dbRelRefBase
{
public:
///	ctor allowing you to pass field name for relationship.
	dbRelSet(const char* fieldName=0) : dbRelRefBase(fieldName) {};
	// use default copy ctor
};


/**
	Object used to specify relationship between two dbTable instances.
	Simple relationships just use dbRelationship as a functor. In cases
	like M:N relationships we need to create a named dbRelationship to 
	call additional specification methods like linkMNvia();
	
	\todo factor into mixin, but our OOF_mixRefCount has different semantics
	expecting at least one ref
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRelationship
{
public:	
// just exists to declare a relationship
	dbRelationship(dbRelRefBase&, dbRelRefBase&);
	dbRelationship(dbRelRefBase*, dbRelRefBase*);
// variations with join fields
	dbRelationship(dbRelRefBase&, dbRelRefBase&, dbField&, dbField&);
	dbRelationship(dbRelRefBase*, dbRelRefBase*, dbField*, dbField*);

// method to specify an M:N linker
	void linkMNvia(dbRelRefBase&, dbRelRefBase&);
	void linkMNvia(dbRelRefBase*, dbRelRefBase*);
	
// data storage
private:
	dbRelRefBase& mLHS;
	dbRelRefBase& mRHS;
};


/**
	Created by dbRelationship::linkMNvia to maintain an MN relationship.
	Creates intermediate records mainly to support use of MN related data in
	GUI lists, eg: dragging a selection from one table to indicate linking to another.
	\ingroup oofInternals
*/
class OOFILE_EXPORT  OOF_RelMN {
private:	// only used by friends
	OOF_RelMN(dbRelRefBase& lhs, dbRelRefBase& rhs);
	OOF_RelMN(const OOF_RelMN& rhs); 
	
	bool RelateMN();
	dbTable* BuildRelatedTable(dbRelRefBase* cloner);
	
public:
	void incRefs();
	void decRefs();	// may delete this
	bool cloningRelIs(dbRelRefBase*);
	
	void makeRelMaintainer();  ///< factory conditionally creates a dbRelMaintainer listening to rhs table
	void appendSelection(dbSelection*);
	void removeSelection(dbSelection*);
	void clearSelection();
	
	
private:
// data storage
	dbRelRefBase& mProtoLHS;	///< the rel in the prototypical middle, pointing back to the LHS
	dbRelRefBase& mProtoRHS;	///< the rel in the prototypical middle, pointing on to the RHS
	unsigned int mReferences;
	dbRelRefBase* mRelToLHS;	
	dbRelRefBase* mRelToRHS;	
	dbRelMaintainer* mMaintainer;	///< not owned, but keep track to avoid creating > 1
	
	static OOF_RelMN*	sCurrentlyBuilding;
	static dbRelRefBase*	sCurrentRHSref;
	
friend class dbRelRefBase;
friend class dbRelationship;
};


// -------------------------------------------------------
//           d b R e l R e f B a s e 
// -------------------------------------------------------
inline OOF_RelMN*
dbRelRefBase::getMNlink() const
{
	return mMNlink;
}


inline bool
dbRelRefBase::CompletelySpecified() const
{
	return mRelatedTableProto!=0;
}


inline bool 
dbRelRefBase::propagatesDeletes() const
{
	return mPropagatesDeletes;
}


inline dbRelRefBase* 
dbRelRefBase::inverse() const
{
	assert(mActualRelatedTable);
	return (dbRelRefBase*) (mActualRelatedTable->field(mInverseFieldNumber) );  // safe downcast
}


inline dbField* 
dbRelRefBase::joinField() const
{
	return mJoinField;
}


/**
	Is "ref" to just one instance.
	\return false if can relate to multiple records
*/
inline bool 
dbRelRefBase::isRef() const
{
	return mMaxLinks==1;
}


inline bool 
dbRelRefBase::isOneToMany() const
{
	assert(mInverseField);
	return !isRef() && mInverseField->isRef();
}


inline bool 
dbRelRefBase::isManyToOne() const
{
	assert(mInverseField);
	return isRef() && !mInverseField->isRef();
}


inline bool 
dbRelRefBase::isOneToOne() const
{
	assert(mInverseField);
	return isRef() && mInverseField->isRef();
}


inline bool 
dbRelRefBase::isManyToMany() const
{
	assert(mInverseField);
	return !isRef() && !mInverseField->isRef();
}


/**
	If we don't have mJoinField then we are a pointer relationship.
	(or some other kind added later than OOFILE 1.4)
*/
inline bool 
dbRelRefBase::isJoin() const
{
	assert(CompletelySpecified());
	return mJoinField!=0;
}


/**
	Do we point left or right?
	ie: in Patients->Visits is this relationship field pointing
	back from Visits to Patients (the inverse relationship).
	This is important to know if you are following a relationship
	chain back from a related table to the source.
	So we test if the table which contains this field actually
	points back to the LHS via this field.
*/
inline bool 
dbRelRefBase::isRelBackToParent() const
{
	return mTable->controllingRelationship()==this;
}

#endif

