#ifndef H_OOFRECS
#define H_OOFRECS

// OOFILE record-oriented abstract backend implementation
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFQUERY
	#include "oofquery.h" 
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif

#ifndef OOF_FIRST_SYNTHETIC_RECORD_OFFSET
/**
	value for new records that are being cached, typically related records.
	so the this default allows for 2048 records to be cached in this 
	manner before problems occur.	
*/
	#define OOF_FIRST_SYNTHETIC_RECORD_OFFSET (ULONG_MAX - 9216)
#endif

class OOF_simpleRecordBackend;
class OOF_recordFieldEntry;
class OOF_recordCacheCopier;
class OOF_recordSelectionRep;
class OOF_recordBufferedContext;



/**
	Describe entries in schema like OOF_simpleRecordBackend::mFieldBufMap.	
	\ingroup oofInternals
*/
class OOF_recordFieldEntry {  
public:
// use default copy ctor
// data storage
	unsigned long	mOffset, mLength;
};


/**
	Describe a subset of fields in a table which are dirty.
	Used by OOF_ctreeKeywordIndex to control reindexing.
	\ingroup oofInternals
*/
class OOF_dirtyFieldMap {  
public:
	OOF_dirtyFieldMap();
	OOF_dirtyFieldMap(const OOF_dirtyFieldMap&);
	~OOF_dirtyFieldMap();
	
	void finishMap(unsigned short numFields);
	
	void markDirty(unsigned short);
	bool isDirty(unsigned short) const;
	bool anyDirty() const;
	void reset();
	
private:
// data storage
	unsigned short mNumFields;
	bool*	mDirtyMap;	// NOT YET IMPLEMENTED - use bitfield
};


/**
	Store RAM-resident cache of records, eg: OOF_simpleRecordBackend::mDirtyRecordCache.
	\todo copy ctor OOF_recordCache(const OOF_recordCache &rhs)
	\ingroup oofInternals
*/
class OOF_recordCache
{
public:
	enum {kNotFound=ULONG_MAX};
	
	OOF_recordCache();
	OOF_recordCache(OOF_recordBufferedContext*);
	

private:
	OOF_recordCache(const OOF_recordCache&) { assert(0); };
public:
	~OOF_recordCache();
  
	void append(OOF_recordBufferedContext* adoptingContext);
	void deleteRecord(unsigned long recOffset);
	void deleteAll();
	unsigned long containsContext(const OOF_recordBufferedContext*) const;
	unsigned long containsOffset(unsigned long recOffset) const;
	unsigned long firstFree() const;
	OOF_recordBufferedContext* orphanBuffer(unsigned long);
	OOF_recordBufferedContext* item(unsigned long);
	void setItem(unsigned long, OOF_recordBufferedContext* adoptingContext);
	void reuseDeletedItem(unsigned long, OOF_recordBufferedContext* adoptingContext);
	unsigned long count() const;
	unsigned long countNew() const;
	unsigned long countOccupied() const;
	bool anyDeleted() const;
  
protected:
// data storage
	OOF_ExpandableLongArray mBuffers;
	unsigned short mDeletedCells;
};



/**
	just exists to give several classes access to the enum without qualification.
	\ingroup oofInternals
*/
class OOF_SelectionStateDefiner {  
public:
	enum EselectionState {empty, oneRec, someRecs, allRecs};
};	
OOFILE_EXPORT ostream& operator<<(ostream&, OOF_SelectionStateDefiner::EselectionState);


/**
	Simplest internal representation of a record's contents and context in dbTable.
	OOF_recordCache contains the contents of OOF_recordBufferedContext and can recreate
	but doesn't necessarily have to store OOF_recordBufferedContext objects.
	\ingroup oofInternals
*/
class OOF_recordBufferedContext : public dbContext
{
public:
	enum ERecordState {eNew, eLoaded, eUnloaded};
	OOF_recordBufferedContext();
	OOF_recordBufferedContext(const OOF_recordBufferedContext&);
	virtual ~OOF_recordBufferedContext();

	void copyFrom(const OOF_recordBufferedContext*);
	virtual void copyFrom(const OOF_recordBufferedContext&);
	const OOF_recordBufferedContext& operator=(const OOF_recordBufferedContext&);
	unsigned long currentOffset() const;
	operator unsigned long() const;
	bool operator==(unsigned long) const;
	bool operator==(const OOF_recordBufferedContext&) const;
	bool isNew() const;

	char* buffer() const;

protected:
	void AllocBuffer();
	
// data storage
	ERecordState mRecordState;
	unsigned long mCurrLoadedRecOffset;
	unsigned long mRecBufLen;
	char *mBuffer;	// owned
	OOF_ExpandableLongArray* mBlobFieldBodies;	// owned
	OOF_dirtyFieldMap mDirtyMap;
	
	friend class OOF_simpleRecordBackend;
};
OOFILE_EXPORT ostream& operator<<(ostream&, OOF_recordBufferedContext::ERecordState);


/**
	Selection of records in context of a single dbTable instance.
	Top level in 3-layer abstraction of selection.
	Implements physical sorting of the set of oidT's pointing to records.
	Also implements combinatorial operations as a set.
	Handle class for OOF_recordSelectionRep which implements the actual operations.
	\ingroup oofInternals
*/
class OOF_recordSelection : public OOF_Selection, public OOF_SelectionStateDefiner 
#ifdef OOF_DEBUG
, public oofSingleListener
#endif
{  
// default constructor, or passing in zero items  skips allocating storage
public:
	OOF_recordSelection(dbTable* prototypicalTable);
	OOF_recordSelection(unsigned long numSlots, dbTable* prototypicalTable);
	OOF_recordSelection(const EselectionState, unsigned long numSlots, dbTable* prototypicalTable);
	OOF_recordSelection(unsigned long numSlots, unsigned int expansionChunk, dbTable* prototypicalTable);
	OOF_recordSelection(const OOF_recordSelection&);  
	virtual ~OOF_recordSelection();

	virtual OOF_Selection* clone() const;
	void describeState(ostream& os) const;	
	void describeRecsRep(ostream&) const;
	
	void resetFakeNewRecordOffSet();

#ifdef OOF_DEBUG
// so we know if prototypical table goes away
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
#endif	
	
	OOF_recordSelection& operator=(const OOF_recordSelection&);	
	void copyFrom(const OOF_recordSelection&, dbTable::selSharingT);	
		
	virtual void setSortOrder(dbSorter* adoptSorter);
	virtual dbSorter* sorter() const;

	unsigned long* allocSelection(unsigned long numSlots);
	unsigned long* allocConsumedSelection(unsigned long numSlots);
	void expandToInclude(unsigned long numSlots);
	void adopt(OOF_recordSelection&);
	void copyContents(const OOF_recordSelection&);
	void aboutToDirtySelection();
  
/// \name combinatorial operators
//@{
  virtual void difference_with(const OOF_Selection*);
  virtual void intersection_with(const OOF_Selection*);
  virtual void intersection_with(oidT);
  virtual void union_with(const OOF_Selection*);
  virtual void union_with_no_overlap(const OOF_Selection*);
  virtual void union_with(oidT);
  virtual void invert();
//@}

/// \name local combinatorial implementations
//@{
  void difference_with(const OOF_recordSelection&);
  void intersection_with(const OOF_recordSelection&);
  void union_with(const OOF_recordSelection&);
  void union_with_no_overlap(const OOF_recordSelection&);
//@}

/// \name reflective operators  
//@{
	EselectionState state() const;
	unsigned long oneRecOffset() const;
	virtual bool isEmpty() const;
	virtual bool isAllRecs() const;
	virtual bool contains(oidT) const;
	virtual bool find(oidT item, unsigned long& foundAt) const;
	virtual const dbTable* prototypicalTable() const;
//@}

/// \name change state
//@{
	void selectNone();
	void selectOneRec(unsigned long);
	void selectSome();
	void selectAll();
//@}
	
///  \name access operators
//@{
	unsigned long& operator[](unsigned long);
	unsigned long& operator[](long);
	unsigned long& operator()();
	unsigned long value(unsigned long) const;
	void append(oidT);
	unsigned long appendNewRecord(bool appendIt);  ///< returns new synthetic record number
	void deleteCurrent();
	void deleteItem(unsigned long index);
	void updateCurrent(unsigned long); 
	unsigned long iter() const;  ///< current position in selection
	unsigned long current() const;  ///< value of current item
//@}

/// \name iterator protocol
//@{
	void start();
	bool more() const;
	void prev();
	void next();
	virtual unsigned long count() const;
	void gotoRecord(unsigned long);
//@}

private:
	void DropSelection();

// data storage
private:
	OOF_recordSelectionRep *mRep;	///< owned and shared (copy-on-write or shared selection)
	unsigned long mInternalIter;
	unsigned long mNextFakeNewRecOffset;
	dbTable*	mPrototypicalTable;  ///< NOT necessarily the one on which we were created!
};


/**

	Main implementation parent for backends following the record-oriented model.
	An SQL backend would be a sibling to this class.
	Implements all the non-indexed searches, sorts and iteration.
	Adding a new non-indexed backend is simply a matter of subclassing this and
	adding necessary IO methods.
	\ingroup oofInternals
*/
class OOF_simpleRecordBackend : public OOF_tableBackend, public OOF_recordBufferedContext {
protected: 

/// \name Ken's wildcard header bits
//@{
	enum OOF_wctype { prefix, single, multiple, singlefinal, multiplefinal };
	/**
		Wildcard type for parsing string literals when querying database.
		\ingroup oofInternals
	*/
	typedef struct {	OOF_wctype thewc;
						char*  thechars;
					} WC;
//@}

// constructors
	OOF_simpleRecordBackend(dbTable*);
	OOF_simpleRecordBackend(const OOF_simpleRecordBackend&, dbTable::selSharingT, dbTable*);
//	void operator=(const OOF_simpleRecordBackend&) {assert(0);};
	
 public:
	virtual ~OOF_simpleRecordBackend();

	virtual OOF_tableBackend* clone(dbTable::selSharingT, dbTable*) const=0;
	virtual void buildSchema(bool rebuilding=false);
	
	virtual void describeState(ostream&) const;
	virtual void setBlobLength(fieldNumT, unsigned long len);
	virtual void setFileExtensions(const char* /*dataExt*/, const char* /*indexExt*/) {};
	
/// \name OID generation
//@{	
	virtual oidT currentOID();
	virtual oidT oidOfRecord(unsigned long);
//@}

/// \name selection manipulation 
//@{
    virtual unsigned long recordNumber() const;
    virtual unsigned long recordNumberOfOID(oidT inOID);	
	virtual void selectAll();
	virtual void selectNone();

	virtual void invert();  
	virtual void difference_with(const OOF_tableBackend*);
	virtual void intersection_with(const OOF_tableBackend*);  
	virtual void union_with(const OOF_tableBackend*);  
	virtual void union_with_no_overlap(const OOF_tableBackend*);  
	virtual void difference_with(const dbSelection&);
	virtual void intersection_with(const dbSelection&);  
	virtual void union_with(const dbSelection&);  
	virtual void union_with_no_overlap(const dbSelection&);  
//@}

	virtual void ignoreDuplicateRecords();
	virtual void noticeDuplicateRecords();

/// \name public search functions
//@{
	virtual bool search(const dbQueryClause* qClause);
	virtual unsigned long recordNumberMatching(const dbQueryClause* qClause, bool* outFoundIt=0) const;
	virtual bool checkDuplicate(const dbQueryClause*);
	virtual bool searchSelection(const dbQueryClause*);
	virtual bool searchEqual(const dbField*, const char*, bool matchEntireKey=true);
	virtual bool searchEqual(const dbField*, const void*);
	virtual bool searchSelContainsAnyDelimited(const dbField* schField, const char *schStr, char delim);
	virtual bool searchSelContainsAnyOf(const dbField* schField, const char **schStrs, unsigned long count);
	virtual bool searchSelContainsAllDelimited(const dbField* schField, const char *schStr, char delim);
	virtual bool searchSelContainsAllOf(const dbField* schField, const char **schStrs, unsigned long count);
	virtual bool loadRelatedContextJoiningFromTo(const dbField*, const dbField*);
//@}	

/// \name recordwise access
//@{
	virtual bool atFirst() const;
	virtual bool atLast() const;
	virtual bool atRelative(unsigned long) const;
	virtual void prev();
	virtual bool more() const;
	virtual unsigned long count() const;
	void gotoBOF();
//@}
	
/// \name data access
//@{
	virtual void newRecord();
	virtual void saveRecord();
	virtual void unloadRecord();
	virtual void unloadCache();
	virtual void revertRecord();
	virtual void pack();	
//@}

/// \name sorting public interface
//@{
	virtual void setSortOrder(dbSorter* adoptSorter);
	virtual dbSorter* sorter() const;
	virtual void sortSelectionNow(fieldNumT);
	virtual void sortSelectionInverseNow(fieldNumT);
	virtual void sortSelectionNow(const dbSorter*);
//@}

	virtual bool reloadRecord();
	virtual bool isRecordLoaded() const;

	virtual void *getFieldWriteDest(const dbField*);
	virtual void *getFieldReadFrom(fieldNumT);

/// \name reflective operators
//@{
	virtual unsigned long fieldDataLen(const dbField*) const;
	virtual bool fieldIsDirty(fieldNumT) const;
	virtual bool isNewRecord() const;
	virtual unsigned int numIndexes() const;
	virtual unsigned int numFiles() const;
	virtual unsigned long blobPointerReferenceSize() const;
	virtual bool allSelected() const;
	virtual bool contains(oidT) const;
//@}
	
	virtual bool addOIDtoSelection(oidT);
	virtual void dropRecordFromSelection(unsigned long);

/// \name getting & saving selections as lightweight objects
//@{
 	virtual dbSelection currentSelection() const;
	virtual dbSelection makeEmptySelection() const;
	virtual void setSelection(const dbSelection&);
	virtual void setSelection(const OOF_Selection*);
//@}

/// \name selection of specific contexts
//@{
	virtual bool selectJustOID(oidT);
	virtual void selectJustCurrent();
//@}
 
/// \name locking
//@{
	virtual void lockRecord();
	virtual void unlockRecord();
//@}
		
/// \name transactions  
//@{
	virtual void beginTransaction(short tMode);
	virtual void abortTransaction(short rMode);
	virtual void commitTransaction(short rMode);
//@}
		
/// debugging	
	void describeInvariant(ostream&) const; 
		
protected:
	bool Invariant(const char* optionalComment=0) const;
	oofString InvariantDescription() const;
	
	unsigned long state() const;

/// backend construction
	virtual void BuildBackendtables(bool rebuilding)=0;

	unsigned long PadFieldWidthForAlignment(const dbField* fld) const;
	unsigned int FieldAlignsTo(const dbField*) const;

	virtual void CachingContextChange();
	virtual void ContextChange();
	virtual bool CanUseIndex(const dbField*, dbQueryClause::BinaryQueryOps=dbQueryClause::equals) const;
	bool IgnoringDuplicateRecords() const;
	virtual void SaveContext()=0;


	unsigned long BlobLenFromBuffer(const dbField*, const char* theirBuffer) const;
	unsigned long BlobPosFromBuffer(const dbField* fld, const char* theirBuffer) const;
	unsigned long BlobLenFromBuffer(fieldNumT, const char* theirBuffer) const;
	unsigned long BlobPosFromBuffer(fieldNumT, const char* theirBuffer) const;
	void SetBlobLenInBuffer(fieldNumT, unsigned long len, const char* theirBuffer) const;
	void SetBlobPosInBuffer(fieldNumT, unsigned long pos, const char* theirBuffer) const;
	virtual void LoadRecordAtOffset(unsigned long offset)=0;
	bool MaybeLoadRecordFromCache(unsigned long offset);
	bool CommonSimpleDelete();
	bool GotoRecordForAllUnindexed(unsigned long relativeNum);

	void  AdoptBuffer(OOF_recordBufferedContext*);
	void DonateBlobBodiesTo(OOF_recordBufferedContext*);
	void CacheDirtyCurrentRecord();
#ifdef OOF_DebugDumpBlobState
	virtual void describeBlobs() const; 
#endif	
	void ResetBlobs();

/// \name high-level search redirectors
//@{
	virtual bool SearchBinaryFieldToLiteral(const dbQueryBinary*);
	virtual bool SearchTrinaryFieldToLiterals(const dbQueryTrinary*);
	virtual bool SearchCombineSubclauses(const dbQueryBinaryCombo*, bool searchSel);
	virtual bool SearchSelectionBinaryFieldToLiteral(const dbQueryBinary*);
//@}

/// \name Non-Indexed searches
//@{
	bool SearchEqualNonIndexed(const dbField*, const char*, bool matchEntireKey=true);
	bool SearchEqualNonIndexed(const dbField*, const void*);
	bool SearchWordNonIndexed(const dbField*, const char*, bool startsWith=false);
	bool SearchNotEqualNonIndexed(const dbField*, const char*, bool matchEntireKey=true);
	bool SearchNotEqualNonIndexed(const dbField*, const void*);
	bool SearchBetweenNonIndexed(const dbField*, const char*, const char*);
	bool SearchBetweenNonIndexed(const dbField*, const void*, const void*);
	bool SearchOutsideNonIndexed(const dbField*, const char*, const char*);
	bool SearchOutsideNonIndexed(const dbField*, const void*, const void*);
	bool SearchComparativeNonIndexed(const dbField*, const char*, dbQueryClause::BinaryQueryOps);
	bool SearchComparativeNonIndexed(const dbField*, const void*, dbQueryClause::BinaryQueryOps);
	bool SearchSelectionTrinaryFieldToLiterals(const dbQueryTrinary*);
//@}

/// \name Non-Indexed wildcard Searches
//@{
	bool SearchEqualNonIndexedWildcard(const dbField* schField, const char* schStr, bool matchEntireKey=true);
	bool SearchNotEqualNonIndexedWildcard(const dbField* schField, const char* schStr, bool matchEntireKey=true);
//@}
	
/// \name Sort implementations  
//@{
	void SortSelectionNow(const dbUlong* theFld);
	void SortSelectionNow(const dbLong* theFld);
	void SortSelectionNow(const dbUshort* theFld);
	void SortSelectionNow(const dbShort* theFld);
	void SortSelectionNow(const dbReal* theFld);
	void SortSelectionNow(const dbChar* theFld);
	void SortSelectionNow(const dbText* theFld);  
	void SortSelectionNow(const dbField* theFld, unsigned long fixedBinaryLength);  
  
	void SortSelectionInverseNow(const dbUlong* theFld);
	void SortSelectionInverseNow(const dbLong* theFld);
	void SortSelectionInverseNow(const dbUshort* theFld);
	void SortSelectionInverseNow(const dbShort* theFld);
	void SortSelectionInverseNow(const dbReal* theFld);
	void SortSelectionInverseNow(const dbChar* theFld);
	void SortSelectionInverseNow(const dbText* theFld);  
	void SortSelectionInverseNow(const dbField* theFld, unsigned long fixedBinaryLength);  
  
	void SubSort(const dbSorter *theSorter, const unsigned long level, unsigned long *theData, const unsigned long count);
//@}
    
// data storage
	OOF_recordSelection mSelection;
	OOF_recordFieldEntry *mFieldBufMap;  // owned semi-static member passed onto sub-collections
	unsigned int* mFieldBufMapRefCount;  // owned semi-static member passed onto sub-collections
	unsigned short mNumFiles, mNumIndexes, mOverheadLeadingBytes;


	OOF_ExpandableLongArray* mBlobFieldNums;  // owned
	OOF_recordCache* mDirtyRecordCache;  // owned
	bool mIgnoreDuplicateRecords;

#ifdef OOF_INVARIANT_DUMP_NAME
	void InvariantDump(const char* optionalComment=0) const;
	
	#ifndef _Macintosh
	// use Macsbug built-in debugger on Mac so we get stack traces as well
	// optional statics
	static oofDirectory sInvariantDumpDir;
	static ofstream sInvariantDump;
	#endif

#endif
 
	
friend class OOF_recordCacheCopier;
};

/**
	Lowest level in 3-layer abstraction of selection.
	Contains actual list of records as an array.
	Used by OOF_recordSelectionRep
	\ingroup oofInternals
*/
class OOF_recordSelRecsRep : public OOF_ExpandableLongArray, public OOF_mixRefCount {
private:
// construction
	OOF_recordSelRecsRep(unsigned long numSlots=0, unsigned int expandBySlots=10);
	// use default copy ctor
	virtual ~OOF_recordSelRecsRep() {};
							
	friend class OOF_recordSelectionRep;
};


/**
	Middle level in 3-layer abstraction of selection.
	Implements COW (Copy On Write) access to selections so using selections
	is lightweight operation until you need to change the selection via resorting
	or changing selection membership.
	\par
	This is the heart and soul of deferred evaluation in OOFILE and makes it very 
	efficient to use cloned tables as alternate iterators over a selection.
	\par
	Used by OOF_recordSelection and shares a OOF_recordSelectionRep.
	\warning if you change this code be very careful to fully test OOFILE.
	\ingroup oofInternals
*/
class OOF_recordSelectionRep : public OOF_mixRefCount, public OOF_SelectionStateDefiner {
private:
	enum {kDefExpansionChunk=10};
// construction
	OOF_recordSelectionRep(unsigned long numSlots=0, unsigned int expandBySlots=10);
	OOF_recordSelectionRep(const EselectionState, unsigned long numSlots=0);
	OOF_recordSelectionRep(const OOF_recordSelectionRep&);
	OOF_recordSelectionRep& operator=(const OOF_recordSelectionRep&);
	virtual ~OOF_recordSelectionRep();
							
	virtual void setSortOrder(dbSorter* adoptSorter);
	virtual dbSorter* sorter() const;

	void AboutToDirtySelection();
	unsigned long* ExposeArray();
	void MarkAsUsed(unsigned long);
	void CopyContents(const OOF_recordSelectionRep* rhs);
	void DropSelection();
	unsigned long* AllocSelection(unsigned long numSlots);
	unsigned long* AllocConsumedSelection(unsigned long numSlots);
	void ExpandToInclude(unsigned long indexToCover);
	
	void SelectionChanged();

// access operators
	unsigned long& item(unsigned long index);
	unsigned long value(unsigned long index) const;
	void Append(unsigned long n);
	void SetItemOffset(unsigned long index, unsigned long);
	void DeleteItem(unsigned long index);

// change state
	void SelectNone();
	void SelectOneRec(unsigned long rec);
	void SelectAll();
	void SelectSome();

// reflective operators  
	unsigned long Count() const;
	bool IsMember(unsigned long item) const;
	bool Find(unsigned long item, unsigned long& foundAt) const;
	bool Invariant() const;

// data storage
	unsigned long mCurrSingleRecOffset;
	EselectionState mState;
	OOF_recordSelRecsRep* mRecsRep;	// owned and shared COW
    dbSorter* mSorter;  // owned
	
	friend class OOF_recordSelection;
};


/**
	Used by OOF_simpleRecordBackend::saveRecord to save single cache item.
	Provides delay - responds to OOFmsg_SaveRecord after all the other
	listeners such as dbRelRefBase::receiveMsg.
	\par
	Self-deleting in OOF_recordCacheCopier::receiveMsg.
	\ingroup oofInternals
*/
class OOF_recordCacheCopier : public oofSingleListener {
public:
	OOF_recordCacheCopier(oofBroadcaster* listensTo, OOF_recordCache* adoptsCache, OOF_simpleRecordBackend* inBackend, OOF_recordBufferedContext*);
// use default copy ctor
	virtual ~OOF_recordCacheCopier();
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
protected:
// data storage
	OOF_recordCache* mCache;
    OOF_simpleRecordBackend* mBackend;
    OOF_recordBufferedContext* mSavedContext;
};



// -------------------------------------------------------
//    O O F _  r e c o r d B u f f e r e d C o n t e x t
// -------------------------------------------------------
inline bool
OOF_recordBufferedContext::isNew() const
{
	return (mRecordState==eNew);  
}


inline void 
OOF_recordBufferedContext::copyFrom(const OOF_recordBufferedContext* rhs)
{
	copyFrom(*rhs);
}


inline  
OOF_recordBufferedContext::operator unsigned long() const
{
	return mCurrLoadedRecOffset;
}


inline unsigned long
OOF_recordBufferedContext::currentOffset() const
{
	return mCurrLoadedRecOffset;
}


inline bool 
OOF_recordBufferedContext::operator==(unsigned long inOffset) const
{
	return (mCurrLoadedRecOffset==inOffset);
}


inline char*
OOF_recordBufferedContext::buffer() const
{
	return mBuffer;
}

// -------------------------------------------------------
//      O O F _ s i m p l e R e c o r d B a c k e n d
// -------------------------------------------------------

inline unsigned long 
OOF_simpleRecordBackend::BlobLenFromBuffer(const dbField* fld, const char* theirBuffer) const
{
		return BlobLenFromBuffer(fld->fieldNumber(), theirBuffer);  
}


inline unsigned long 
OOF_simpleRecordBackend::BlobPosFromBuffer(const dbField* fld, const char* theirBuffer) const
{
		return BlobPosFromBuffer(fld->fieldNumber(), theirBuffer);  
}


inline void 
OOF_simpleRecordBackend::gotoBOF()
{
	mCurrLoadedRecOffset = 0;
}


inline bool 
OOF_simpleRecordBackend::IgnoringDuplicateRecords() const
{
	return mIgnoreDuplicateRecords;
}



// -------------------------------------------------------
//           O O F _ d i r t y F i e l d M a p 
// -------------------------------------------------------
inline void 
OOF_dirtyFieldMap::markDirty(unsigned short n)
{
	assert(n<mNumFields);
	mDirtyMap[n] = true;
}
	

inline bool 
OOF_dirtyFieldMap::isDirty(unsigned short n) const
{
	assert(n<mNumFields);
	return mDirtyMap[n];
}
	


// -------------------------------------------------------
//            O O F _ r e c o r d C a c h e
// -------------------------------------------------------
inline unsigned long 
OOF_recordCache::count() const
{ 
	return mBuffers.count(); 
}


inline unsigned long 
OOF_recordCache::countOccupied() const
{ 
	return mBuffers.count() - mDeletedCells;  
}


inline bool 
OOF_recordCache::anyDeleted() const
{ 
	return  mDeletedCells > 0;  
}


// -------------------------------------------------------
//          O O F _ r e c o r d S e l e c t i o n R e p
// -------------------------------------------------------

inline unsigned long* 
OOF_recordSelectionRep::ExposeArray() 
{ 
	assert(mRecsRep);
	return mRecsRep->mBits; 
} 


inline void 
OOF_recordSelectionRep::MarkAsUsed(unsigned long numUsedAtFront)
{ 
	assert(mRecsRep);
	mRecsRep->mNextFreeEntry = numUsedAtFront; 
} 


inline void
OOF_recordSelectionRep::SelectionChanged()
{
	if (mSorter)
		mSorter->selectionChanged();
}


// -------------------------------------------------------
//           O O F _ r e c o r d S e l e c t i o n
// -------------------------------------------------------
inline OOF_recordSelection::EselectionState 
OOF_recordSelection::state() const
{
	return mRep->mState;
}


inline unsigned long 
OOF_recordSelection::count() const
{
	return mRep->Count();
} 


inline bool 
OOF_recordSelection::contains(oidT item) const
{
	bool ret = false;
	if (item)
		ret = mRep->IsMember(item);
	return ret;
}


inline bool 
OOF_recordSelection::find(oidT item, unsigned long& foundAt) const
{
	bool ret = false;
	if (item)
		ret = mRep->Find(item, foundAt);
	return ret;
}


inline void 
OOF_recordSelection::start() 
{
	mInternalIter = 0;
}


inline bool 
OOF_recordSelection::more() const
{
	assert(state() != allRecs);
	return mInternalIter < count();  // yes, this is more an "in range" than a "more"
}


inline void 
OOF_recordSelection::prev() 
{
	assert(mInternalIter>0);
	mInternalIter--;
}


inline void 
OOF_recordSelection::next() 
{
	mInternalIter++;
}


inline bool 
OOF_recordSelection::isEmpty() const
{
	return state()==empty;
}


inline bool 
OOF_recordSelection::isAllRecs() const
{
	return state()==allRecs;
}


inline unsigned long 
OOF_recordSelection::oneRecOffset() const
{
	return mRep->mCurrSingleRecOffset;
}


inline unsigned long
OOF_recordSelection::current() const  // can return 0 if allRecs state
{ 
	unsigned long ret=0;
	if (mRep && (mInternalIter < count()) )
		ret = mRep->value(mInternalIter);
	return ret;
}


inline unsigned long& 
OOF_recordSelection::operator()() // writable current value
{ 
	assert(mRep);
	assert(mInternalIter < count());  // guards against iter off end & allRecs state
	return mRep->item(mInternalIter);
}


inline unsigned long& 
OOF_recordSelection::operator[](long index)
{
	assert(index>=0);
	assert(mRep);
	return mRep->item ((unsigned int) index);
}


inline unsigned long& 
OOF_recordSelection::operator[](unsigned long index)
{
	assert(mRep);
	return mRep->item(index);
}


inline unsigned long 
OOF_recordSelection::value(unsigned long index) const
{
	assert(mRep);
	return mRep->value(index);
}


inline unsigned long 
OOF_recordSelection::iter() const
{
	return mInternalIter;
}


inline void 
OOF_recordSelection::gotoRecord(unsigned long index)
{
	mInternalIter=index;
	assert( (state()==allRecs) || (mInternalIter < count()) );
}


inline void 
OOF_recordSelection::selectNone()
{
	mRep->SelectNone();
}


inline void 
OOF_recordSelection::selectOneRec(unsigned long rec)
{
	mInternalIter = 0;
	mRep->SelectOneRec(rec);
}


inline void 
OOF_recordSelection::selectAll()
{
	mRep->SelectAll();
}

 
inline void 
OOF_recordSelection::selectSome()
{
	assert(mRep->Count() > 0);  // we have some sub-optimal code in SearchBetween that gets a selection of one rec
	mRep->SelectSome();
} 


inline void 
OOF_recordSelection::append(oidT oid)
{
	if(oid)	// bit of a hack to catch mistakes upline, and possible user mistakes
		mRep->Append(oid);
}


inline unsigned long* 
OOF_recordSelection::allocSelection(unsigned long numSlots) 
{
	return mRep->AllocSelection(numSlots);
} 


inline void
OOF_recordSelection::expandToInclude(unsigned long indexToCover) 
{
	mRep->ExpandToInclude(indexToCover);
} 


inline void
OOF_recordSelection::aboutToDirtySelection()
{
	mRep->AboutToDirtySelection();
}


inline unsigned long* 
OOF_recordSelection::allocConsumedSelection(unsigned long numSlots) 
{
	return mRep->AllocConsumedSelection(numSlots);
} 



inline void 
OOF_recordSelection::copyContents(const OOF_recordSelection& rhs)
{
	mRep->CopyContents(rhs.mRep);  
}
 

inline void 
OOF_recordSelection::resetFakeNewRecordOffSet()
{
	mNextFakeNewRecOffset = OOF_FIRST_SYNTHETIC_RECORD_OFFSET;

}

#endif


