// OOFILE record-oriented abstract backend implementation
// COPYRIGHT 1996 A.D. Software, All rights reserved

// this layer provides implementation classes for record-oriented
// backends such as c-tree Plus, dBase and RAM-based tables

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFRECS
	#include "oofrecs.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//    O O F I L E _ s i m p l e R e c o r d B a c k e n d
//         see also oofrec2.cpp & oofrec3.cpp
// -------------------------------------------------------
OOF_simpleRecordBackend::OOF_simpleRecordBackend(dbTable* theTable):
	OOF_tableBackend(theTable),
	mSelection(theTable->prototype()),
	mFieldBufMap(0),
	mFieldBufMapRefCount(0),
	mNumFiles(0),
	mNumIndexes(0),
	mOverheadLeadingBytes(0),
	mBlobFieldNums(0),
	mDirtyRecordCache(0),
	mIgnoreDuplicateRecords(false)
{};
														
														
OOF_simpleRecordBackend::OOF_simpleRecordBackend(const OOF_simpleRecordBackend& rhs, dbTable::selSharingT selSharing, dbTable* theTable) :
	OOF_tableBackend(rhs, theTable),
	OOF_recordBufferedContext(rhs),
	mSelection(theTable->prototype()),
	mFieldBufMap(rhs.mFieldBufMap),
	mFieldBufMapRefCount(rhs.mFieldBufMapRefCount),
	mNumFiles(rhs.mNumFiles),
	mNumIndexes(rhs.mNumIndexes),
	mOverheadLeadingBytes(rhs.mOverheadLeadingBytes),
	mDirtyRecordCache(0),
	mIgnoreDuplicateRecords(rhs.mIgnoreDuplicateRecords)
{
	if (rhs.mBlobFieldNums) {
		mBlobFieldNums = new OOF_ExpandableLongArray(*(rhs.mBlobFieldNums));
		assert(mBlobFieldNums);
	}
	else
		mBlobFieldNums = 0;
	*mFieldBufMapRefCount +=1;

	if (rhs.sorter())
		setSortOrder( rhs.sorter()->clone());		
	if (selSharing == dbTable::selNotShared) { // explicitly called from cloneTableWithoutSelection
		mRecordState = eUnloaded;
		mCurrLoadedRecOffset = 0;
		memset(mBuffer, 0, mRecBufLen);
// don't need to copy mSelection as init'ed to empty state by copy
	}
	else {		// called from clone or just default  copy ctor on user table
		mSelection.copyFrom(rhs.mSelection, selSharing);  // ref counted, copy on write or maybe shared
		mRecordState = rhs.mRecordState ;
		mCurrLoadedRecOffset = rhs.mCurrLoadedRecOffset;
		memcpy(mBuffer, rhs.mBuffer, mRecBufLen);
	}	
}
 
 
OOF_simpleRecordBackend::~OOF_simpleRecordBackend()
{
	if (mFieldBufMapRefCount) {	// in case never created database
		if (*mFieldBufMapRefCount <= 1) {	
			delete[] mFieldBufMap;
			delete mFieldBufMapRefCount;
		}
		else
			*mFieldBufMapRefCount -= 1;
	}
	delete mBlobFieldNums;  
	delete mDirtyRecordCache;
}


unsigned long 
OOF_simpleRecordBackend::PadFieldWidthForAlignment(const dbField* fld) const
{
	unsigned long len = fld->fieldStorageLen();
	switch(fld->fieldType()) {
	
	case charField : 
				len += 1;  // null
				// fall through for same treatment as binary

	case fixedBinaryField : 
		if (len%2==1)
			len += 1; 
		break;

	default:
	;  // stop g++ complaining
	}
	return len;
}


unsigned int 
OOF_simpleRecordBackend::FieldAlignsTo(const dbField* fld) const
{
	if (fld->fieldIsBlob())
		return 4;  // uses longs
		
	if (fld->fieldStorageLen()==0)
		return 0;  // cope with relationships & other virtual fields
		
	switch(fld->nativeType()) {
	
	case uLongField : 
	case longField : 
		return 4;

	case realField : 
		return 8;

	case fixedBinaryField : 
		return 4;  // this is just a GUESS! needs testing with 8 byte structures to see
								// NOT YET IMPLEMENTED
				
	case compoundField : 
		return 1;  // aligns to anything as doesn't physically exist in buffer

	default:
		return 2;
	}
}


void 
OOF_simpleRecordBackend::buildSchema(bool rebuilding) 
{
	assert( Invariant("simpleB::buildSchema-entry") );
	
	BuildBackendtables(rebuilding);
	assert( Invariant("simpleB::buildSchema-after build") );
	
	selectAll();
	delete[] mBuffer;	// in case reallocing
	AllocBuffer();
	memset(mBuffer, 0, mRecBufLen);

	assert( Invariant("simpleB::buildSchema-exit") );
}

void 
OOF_simpleRecordBackend::AdoptBuffer(OOF_recordBufferedContext* buf)
{	
	assert( Invariant("OOF_simpleRecordBackend::AdoptBuffer-entry") );

	assert(buf);
	assert(mRecBufLen==buf->mRecBufLen);
	mCurrLoadedRecOffset = buf->mCurrLoadedRecOffset;
	
	if (!mSelection.isAllRecs()) {
	// make sure what we adopt is in the selection, and make it the current rec!
		unsigned long currOffsetInSelection = mSelection.current();
		if (currOffsetInSelection != mCurrLoadedRecOffset) {	// selection iter is out of whack, eg: saving cached dirty records
			unsigned long foundAt;
			bool selContainsCachedOID = mSelection.find(mCurrLoadedRecOffset, foundAt);
			assert(selContainsCachedOID);	// should not be adopting something not in our selection!!!!
			mSelection.gotoRecord(foundAt);	// now selection matches us!
		}
	}
	mRecordState = buf->mRecordState;
	assert(mRecordState != eUnloaded);  // we couldn't have cached an unloaded record
	delete[] mBuffer;
	mBuffer = buf->mBuffer;
	buf->mBuffer = 0;
	if (mBlobFieldNums && buf->mBlobFieldBodies) {  // we've blobs & there are buffers out there
		unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			dbBLOB* fld = (dbBLOB *) mFields[fldNum];	// safe downcast
			char* bodyPtr = (char*) (buf->mBlobFieldBodies->item(i));
			if (bodyPtr){
				fld->adoptBody(bodyPtr, BlobLenFromBuffer(fld, mBuffer));
				buf->mBlobFieldBodies->item(i) = 0;
			}
			else
				fld->reset();
		}
	}
	
	delete buf;
	assert( Invariant("OOF_simpleRecordBackend::AdoptBuffer-exit") );
}


void 
OOF_simpleRecordBackend::DonateBlobBodiesTo(OOF_recordBufferedContext* rhs)
{
	if (mBlobFieldNums) {
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
		delete rhs->mBlobFieldBodies;  // usually 0
		mBlobFieldBodies = 0;  // OOF_ctreeBackend never has bodies, it's blobs point to their own
		unsigned int numBlobs = mBlobFieldNums->count();
		rhs->mBlobFieldBodies = new OOF_ExpandableLongArray(0, numBlobs);
		assert(rhs->mBlobFieldBodies);
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			dbBLOB* fld = (dbBLOB *) mFields[fldNum];	// safe downcast
			if (fld->isDirty())
				rhs->mBlobFieldBodies->item(i) = (unsigned long) (fld->orphanBody());
		}
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	}
}


unsigned long 
OOF_simpleRecordBackend::BlobLenFromBuffer(fieldNumT fldNum, const char* theirBuffer) const
{
		unsigned long* at = (unsigned long *) (theirBuffer + mFieldBufMap[fldNum].mOffset+sizeof(unsigned long));
		return *at;  
}


unsigned long 
OOF_simpleRecordBackend::BlobPosFromBuffer(fieldNumT fldNum, const char* theirBuffer) const
{
		unsigned long* at = (unsigned long *) (theirBuffer + mFieldBufMap[fldNum].mOffset);
		return *at;  
}


void OOF_simpleRecordBackend::SetBlobLenInBuffer(fieldNumT fldNum, unsigned long len, const char* theirBuffer) const
{
	unsigned long *at = (unsigned long *)(theirBuffer + mFieldBufMap[fldNum].mOffset + sizeof(unsigned long));
	*at = len;
}


void OOF_simpleRecordBackend::SetBlobPosInBuffer(fieldNumT fldNum, unsigned long pos, const char* theirBuffer) const
{
	unsigned long *at = (unsigned long *)(theirBuffer + mFieldBufMap[fldNum].mOffset);
	*at = pos;
}


void 
OOF_simpleRecordBackend::setBlobLength(fieldNumT fldNum, unsigned long len)
{
	mTable->ensureRecordLoaded();
	unsigned long *at = (unsigned long *)(mBuffer + mFieldBufMap[fldNum].mOffset + sizeof(unsigned long));
	*at = len;
}


void 
OOF_simpleRecordBackend::ResetBlobs()
{
	if (mBlobFieldNums) {
		unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			dbBLOB* fld = (dbBLOB *) mFields[fldNum];	// safe downcast
			fld->reset();
		}
	}
}

#ifdef OOF_DebugDumpBlobState
void 
OOF_simpleRecordBackend::describeBlobs() const
{
	ofstream DGebugStream("blobState",ios::app); 
		
	if (mBlobFieldNums) {
					
		unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			dbBLOB* fld = (dbBLOB *) mFields[fldNum];	// safe downcast
			fld->blobState(DGebugStream);
		}
				
	}
		DGebugStream.close();
		
}
#endif

void 
OOF_simpleRecordBackend::ignoreDuplicateRecords()
{
	mIgnoreDuplicateRecords = true;
}


void 
OOF_simpleRecordBackend::noticeDuplicateRecords()
{
	mIgnoreDuplicateRecords = false;
}


bool 
OOF_simpleRecordBackend::allSelected() const
{
	return (mSelection.state() == OOF_recordSelection::allRecs);
}


bool 
OOF_simpleRecordBackend::atFirst() const
{
	return (isRecordLoaded() && (mSelection.iter()==0) );
}


bool 
OOF_simpleRecordBackend::atLast() const
{
	return (isRecordLoaded() && (mSelection.iter()==(count()-1)) );
}


bool 
OOF_simpleRecordBackend::atRelative(unsigned long rel)  const
{
	return (isRecordLoaded() && (mSelection.iter()==rel) );
}


bool 
OOF_simpleRecordBackend::more() const
{
	switch (mSelection.state()) {

	case OOF_recordSelection::oneRec : 
	case OOF_recordSelection::someRecs : 
		return mSelection.more();

	case OOF_recordSelection::allRecs :
		return mSelection.iter() < countAll();
		
	default :
		return false;
	}
}


void 
OOF_simpleRecordBackend::prev() 
{
	unsigned long currentIter = mSelection.iter();
	switch (currentIter) {
	case 0:
		if (!isRecordLoaded())
			reloadRecord();
		break;
	case ULONG_MAX:
		gotoRecord(0);
		break;
	default:
		gotoRecord(currentIter-1);  
		break;
	}

// ES
/*	unsigned long currentIter = mSelection.iter();
	if (currentIter==0) {
		if (!isRecordLoaded())
			reloadRecord();
	}
	else
		gotoRecord(currentIter-1);  
*/
}


/**
	\see countAll
*/
unsigned long 
OOF_simpleRecordBackend::count() const
{
	
	switch (mSelection.state()) {
	
	case OOF_recordSelection::empty :
		return 0;

	case OOF_recordSelection::oneRec :
		return 1;
		
	case OOF_recordSelection::someRecs :
	{
		unsigned long ret = mSelection.count();  // rely on new records going into cache as well
		if (isNew() && !tableCachesDirtyRecords())  // new record won't be in Selection
			ret++;
		return ret;
	}

	case OOF_recordSelection::allRecs :
		return countAll();
	}
	return 0;  // default just to keep compilers happy
}

oidT 
OOF_simpleRecordBackend::currentOID()
{
	if (mSelection.isEmpty())
		return 0;
	mTable->ensureRecordLoaded();
	assert (isRecordLoaded());
	return mCurrLoadedRecOffset;  // NOT YET IMPLEMENTED - handle true OID depending on dbTable settings when created
}


oidT 
OOF_simpleRecordBackend::oidOfRecord(unsigned long recno)
{
	oidT ret = 0;
	switch (mSelection.state()) {

	case OOF_recordSelection::empty : 
		assert(0);
		
	case OOF_recordSelection::oneRec : 
		assert(recno==0);
		ret = mCurrLoadedRecOffset;
		break;
		
	case OOF_recordSelection::someRecs : 
		assert(recno<=mSelection.count());
		ret = mSelection.value(recno);
		break;

	case OOF_recordSelection::allRecs :
		gotoRecord(recno);
		ret = mCurrLoadedRecOffset;
		break;
		
	default :
		;
	}
	return ret;
}


unsigned long 
OOF_simpleRecordBackend::fieldDataLen(const dbField* fld) const
{
	unsigned long dataLen;
	
	switch (fld->fieldType()) {
	case charField : {
		if (mTable->ensureRecordLoaded(false, false)) {	// don't auto-create, allow failure
			char* at = mBuffer + mFieldBufMap[fld->fieldNumber()].mOffset;
			dataLen = strlen(at);  // the string length <= fixed width
		}
		else
			dataLen = 0;
		break;
	}
		
	case blobField : 
		if (mTable->ensureRecordLoaded(false, false)) {	// don't auto-create, allow failure
			dataLen = BlobLenFromBuffer(fld, mBuffer);  
		}
		break;

	case textField : 
		if (mTable->ensureRecordLoaded(false, false)) {	// don't auto-create, allow failure
			dataLen = BlobLenFromBuffer(fld, mBuffer);  // NOTE includes trailing null
			if (dataLen)
				dataLen -= 1;	// if there IS a blob, deduct the null
								// it is too dangerous to expect our caller to understand!!!
								// text field stores string AND terminating null in a blob!
		}
		break;

	default :
		dataLen = fld->fieldStorageLen();  // the fixed width
	}
	return dataLen;
}


bool 
OOF_simpleRecordBackend::fieldIsDirty(fieldNumT fldNum) const
{
	bool ret = 	mDirtyMap.isDirty(fldNum);
	return ret;
}


void * 
OOF_simpleRecordBackend::getFieldWriteDest(const dbField* inField)
{
	assert( Invariant("simpleB::getFieldWriteDest-entry") );

	assert(inField);
	void* ret;
	if (inField->fieldIsVirtual() && !inField->storesCalculatedValues())   // NOT YET IMPLEMENTED - smarter test for this in the field itself??
		ret = 0;
	else {
		mTable->ensureRecordLoaded();
		fieldNumT fldNum = inField->fieldNumber();
		mDirtyMap.markDirty(fldNum);
		ret = mBuffer + mFieldBufMap[fldNum].mOffset;
	}
	assert( Invariant("simpleB::getFieldWriteDest-exit") );
	return ret;
}


void * 
OOF_simpleRecordBackend::getFieldReadFrom(fieldNumT fldNum)
{
	void* ret = 0;
	if (mTable->ensureRecordLoaded(false, false))	// don't auto-create, allow failure
		ret = mBuffer + mFieldBufMap[fldNum].mOffset;
	return ret;
}


bool 
OOF_simpleRecordBackend::GotoRecordForAllUnindexed(unsigned long relativeNum)
{
// if we have just been at a rec, assume we've returned to it for a purpose,
// maybe to reload after someone else modifies
	if (mCurrLoadedRecOffset && (mSelection.iter() == relativeNum)) {
		LoadRecordAtOffset(mCurrLoadedRecOffset);
	}
	else {
		if ((mCurrLoadedRecOffset==0) || mSelection.iter() > relativeNum)  
			start();
// WEAK IMPLEMENTATION FOR NOW but the internal iterator makes it OK if successive calls
		while (mSelection.iter() < relativeNum)
			next();
	}
	return true;
}


/**
	Save current main record and any cached dirty records.
	Broadcasts to related records and other listeners so may re-dirty records in the process.
	Loops creating OOF_recordCacheCopier dependant on our dbTable which will repeatedly pull items out of the 
	cache and get the table to save them.
	This yo-yoing is necessary to get the table to broadcast changes, in case the 
	cached records have dependancies such as relationships hanging off them.
*/
void 
OOF_simpleRecordBackend::saveRecord()
{
// Note: this saves all items in the cache but leaves them there
// for explicit removal with unloadCache
	if (isDirty() || isNew()) {
		assert(isRecordLoaded());
		const bool savingDefaultCalculators = (isNew() && mHasDefaultCalculators);
		const bool savingStoredCalculators = mHasStoredCalculators;  // copy to local const for speed in loop
		if (savingDefaultCalculators || savingStoredCalculators) {
			unsigned long kNumFields = mFields.count();
			for (unsigned long i=0; i<kNumFields; i++) {
				dbField* theField = (dbField*) mFields.value(i);	// safe downcast
				if (savingDefaultCalculators)
					theField->saveDefaultCalculatedValue();
				if (savingStoredCalculators)
					theField->saveStoredCalculatedValue();
			}
		}
		SaveContext();
		mDirty = false;
		mDirtyMap.reset();	// reset even if ignore dups because behave as if we saved a record
	}
	if (mDirtyRecordCache) {
// save our current context
		OOF_recordBufferedContext* buf = new OOF_recordBufferedContext(*this);
		assert(buf);
		DonateBlobBodiesTo(buf);
		
// Note: the following will delete itself when it is finished
		new OOF_recordCacheCopier(mTable, mDirtyRecordCache, this, buf);
		mDirtyRecordCache = 0;
	}
// maybe trigger resort etc.	
	dbSorter* mySorter = sorter();
	if (mySorter)
		mySorter->backendSavedRecord(this);
}


void 
OOF_simpleRecordBackend::unloadRecord()
{
/*	if (isNew())	// hasn't been saved or cached - it's just being dumped
		mSelection.deleteCurrent();	// otherwise selection keeps fake pointer
*/		
	assert( Invariant("simpleB::unloadRecord-entry") );

	const bool recordIsNew = isNew();
	
	// Andy puzzled 10 Mar 2000 - why not test tableCachesDirtyRecords() so also happens
	// for other buffering modes?
	if ( (mTable->tableSaveOption() == dbTable::bufferForever) && (recordIsNew || isDirty()) ) 
		CacheDirtyCurrentRecord();
		
	if (recordIsNew)
		mCurrLoadedRecOffset=0;  // can't reload!
	mRecordState = eUnloaded;
   	mDirty = false;
	ResetBlobs();
	mSelection.resetFakeNewRecordOffSet();
	assert( Invariant("simpleB::unloadRecord-exit") );
}


void 
OOF_simpleRecordBackend::unloadCache()
{
	assert( Invariant("simpleB::unloadCache-entry") );

	if (mDirtyRecordCache && 
	(mTable->tableSaveOption() != dbTable::bufferForever) ) {
		mDirtyRecordCache->deleteAll();
	}

	assert( Invariant("simpleB::unloadCache-exit") );
}


bool 
OOF_simpleRecordBackend::reloadRecord()
{
	assert( Invariant("simpleB::reloadRecord-entry") );
	
	bool ret = false;
	if (mCurrLoadedRecOffset) {
		LoadRecordAtOffset(mCurrLoadedRecOffset);
		ResetBlobs();
		ret = true;
	}
	else {
		start();
		ret = isRecordLoaded();
	}
	assert( Invariant("simpleB::reloadRecord-exit") );
	return ret;
}


void 
OOF_simpleRecordBackend::revertRecord()
{
	assert( Invariant("simpleB::revertRecord-entry") );
	if (!isRecordLoaded()) {
		dbConnect::raise(stringstream() << flush 
			<< "Attempt to revert a record which is not loaded, in table "
			<< mTable->tableName()
			, false
		);
	}
	else {
		LoadRecordAtOffset(mCurrLoadedRecOffset);
		ResetBlobs();
	}
	assert( Invariant("simpleB::revertRecord-exit") );
}


void 
OOF_simpleRecordBackend::ContextChange()
{
	assert( Invariant("simpleB::ContextChange-entry") );

	dbSorter* mySorter = sorter();
	if (mySorter)
		mySorter->backendChangedContext(this);

	assert( Invariant("simpleB::ContextChange-exit") );
}


void 
OOF_simpleRecordBackend::CachingContextChange()
{
	assert( Invariant("simpleB::CachingContextChange-entry") );
	
	if (isDirty() || isNewRecord()) {
		CacheDirtyCurrentRecord();
		mDirty = false;
	}
	ResetBlobs();
	ContextChange();  // triggers sorts so must occur AFTER we've stashed a dirty record

	assert( Invariant("simpleB::CachingContextChange-exit") );
}


void 
OOF_simpleRecordBackend::CacheDirtyCurrentRecord()
{
	assert( Invariant("simpleB::CacheDirtyCurrentRecord-entry") );
		OOF_recordBufferedContext* ctx = new OOF_recordBufferedContext(*this);
		assert(ctx);
		DonateBlobBodiesTo(ctx);
		// at this point ctx contains all the persistent fields from the record
		if (!mDirtyRecordCache)
			mDirtyRecordCache = new OOF_recordCache(ctx);
		else
			mDirtyRecordCache->append(ctx);
	assert( Invariant("simpleB::CacheDirtyCurrentRecord-exit") );
}


void 
OOF_simpleRecordBackend::newRecord() 
{
	assert( Invariant("simpleB::newRecord-entry") );

	mSelection.aboutToDirtySelection();
	mRecordState = eNew;
	mDirty = false;
	memset(mBuffer, 0, mRecBufLen);
	mCurrLoadedRecOffset = mSelection.appendNewRecord(tableCachesDirtyRecords());

	assert( Invariant("simpleB::newRecord-exit") );
}


bool 
OOF_simpleRecordBackend::CommonSimpleDelete()
{
	assert( Invariant("simpleB::CommonSimpleDelete-entry") );

	if (mSelection.state()==OOF_recordSelection::empty){
		assert( Invariant("simpleB::CommonSimpleDelete-exit empty") );
		return true;
	}
	if (mDirtyRecordCache) 
		mDirtyRecordCache->deleteRecord(mCurrLoadedRecOffset);

	mSelection.aboutToDirtySelection();

	if (isNewRecord()) {
	// selection MAY contain this pointer under certain conditions, like related cached records
		if (mSelection.current()==mCurrLoadedRecOffset) // most likely
			mSelection.deleteCurrent();
		else {
			unsigned long foundAt;
			if (mSelection.find(mCurrLoadedRecOffset, foundAt))
				mSelection.deleteItem(foundAt);
		}
		unloadRecord();
		start();  // leaves in reasonably consistent state
		assert( Invariant("simpleB::CommonSimpleDelete-exit complete") );
		return true;  // simple case we know how to handle
	}

	assert( Invariant("simpleB::CommonSimpleDelete-exit") );
	return false;  // didn't wholly process delete!
}


bool 
OOF_simpleRecordBackend::MaybeLoadRecordFromCache(unsigned long offset)
{
	assert( Invariant("simpleB::MaybeLoadRecordFromCache-entry") );

	if (mDirtyRecordCache) {
// maybe this record is cached and we should reload from the cache rather than the file.
		unsigned long cachedIndex = mDirtyRecordCache->containsOffset(offset);
		if (cachedIndex!=OOF_recordCache::kNotFound) {
			OOF_recordBufferedContext* buf = mDirtyRecordCache->orphanBuffer(cachedIndex);
			assert(buf);
			AdoptBuffer(buf);
			mDirty = true;
			assert( Invariant("simpleB::MaybeLoadRecordFromCache-exit-true") );
			return true;    //**** SUCCESSFUL EXIT *****
		}
	}
	assert( Invariant("simpleB::MaybeLoadRecordFromCache-exit-false") );
	return false;
}


unsigned long 
OOF_simpleRecordBackend::recordNumber() const
{
	if (isNewRecord())
		return 0;
	unsigned long ret = mSelection.iter();
	if (ret == ULONG_MAX)
		ret = 0;	// HACK FOR NOW
	return ret;
}


/**
	Implement dbTable::recordNumberOfOID.
	\return zero-based record number or ULONG_MAX if not found
	\todo abstract the allRecs case so can implement efficiently in c-tree.
*/
unsigned long 
OOF_simpleRecordBackend::recordNumberOfOID(oidT inOID)
{
	const unsigned long kNotFound = ULONG_MAX;
	
	oidT ret;
	switch (mSelection.state()) {

	case OOF_recordSelection::empty : 
		ret = kNotFound;
		
	case OOF_recordSelection::oneRec : 
		if (mCurrLoadedRecOffset==inOID)
			ret = 0;
		else
			ret = kNotFound;
		break;
		
	case OOF_recordSelection::someRecs : 
		if (!mSelection.find(inOID, ret))
			ret = kNotFound;
		break;

	case OOF_recordSelection::allRecs :
		{ // nest for Visual C++
			unsigned long numRecs = countAll();
			start();
			ret = kNotFound;
			for (unsigned long i=0; i<numRecs; i++) {
				if (mCurrLoadedRecOffset==inOID) {
					ret = i;
					break;
				}
				next();
			}
		}
		break;
		
	default :
		;
	}
	return ret;
}


unsigned long 
OOF_simpleRecordBackend::recordNumberMatching(const dbQueryClause* inClause, bool* outFoundIt) const
{
	assert( Invariant("simpleB::recordNumberMatching-entry") );

// UGGH!
// have to clone the database as we are const!!!!

// NOT YET IMPLEMENTED
	if (outFoundIt)
		*outFoundIt = false;

	unsigned long ret = 0;
// LATER TO BE HEAVILY REWRITTEN - ONLY SUPPORTS A VERY MINOR CASE OF STRING SEARCHING
// FOR >= MATCH

	if (inClause->queryClauseType()==dbQueryClause::binaryfieldToLiteral) {
		dbQueryBinary* qClause =  (dbQueryBinary*) inClause;	// safe downcast
		dbQueryLiteralStr* strRHS = qClause->literalStrClause();
		dbQueryClause::BinaryQueryOps theOp = qClause->binaryOperator();
		#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
			OOF_MEM_DEBUG_FORCE_POOL_CHECK
		#endif
	
		const dbField* originalSchField = qClause->lhsField();
		assert(originalSchField);
		dbTable* schTable = originalSchField->fieldTable()->cloneTableSharingSelection();
		assert(schTable);
		dbField* schField = schTable->field(originalSchField->fieldNumber());
		if (strRHS) {
			const char* schStr = strRHS->str();
			assert(schStr); 
			switch(theOp) {
			case (dbQueryClause::startsWith) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support startsWith");
				break;
			
			case (dbQueryClause::hasWord) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support hasWord");
				break;
			
			case (dbQueryClause::hasWordStartsWith) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support hasWordStartsWith");
				break;
			
			case (dbQueryClause::equals) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support equals");
				break;
			
			case (dbQueryClause::notEquals) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support notEquals");
				break;
				
			case (dbQueryClause::lessThan) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support lessThan");
					break;
					
			case (dbQueryClause::lessThanOrEqual) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support lessThanOrEqual");
					break;
								
			case (dbQueryClause::greaterThan) :
				assert(!"OOF_simpleRecordBackend::recordNumberMatching doesn't yet support greaterThan");
					break;
					
			case (dbQueryClause::greaterThanOrEqual) :
				if (schField->fieldType()==charField || schField->fieldType()==textField) {
					for(schTable->start();schTable->more();schTable->next()){
						if(OOF_stricmp(schField->asChars(), schStr)>=0) {
							if (outFoundIt) 
								*outFoundIt = true;
							ret = schTable->recordNumber();
							break;
						} // found match
					} // loop looking for match
				}
				else {  // use less efficient copyString
					for(schTable->start();schTable->more();schTable->next()){
						if(OOF_stricmp(schField->copyString(), schStr)>=0) {
							if (outFoundIt) 
								*outFoundIt = true;
							ret = schTable->recordNumber();
							break;
						} // found match
					} // loop looking for match
				}
				break;
					
			default:
				;  // nothing - already set false return value up top
			}
			delete schTable;
		} 
		else {
			assert(!"OOF_simpleRecordBackend::recordNumberMatching only supports string searches");
		} // not string rhs
	}
	else {
		assert(!"OOF_simpleRecordBackend::recordNumberMatching only supports binary queries");
	} // not binary query

	assert( Invariant("simpleB::recordNumberMatching-exit") );
	return ret;
}


bool 
OOF_simpleRecordBackend::addOIDtoSelection(oidT inOID)
{
	assert( Invariant("simpleB::addOIDtoSelection-entry") );

	bool ret = false;
	if (!contains(inOID)) {
		ret = true;
		mSelection.append(inOID);
	}
	assert( Invariant("simpleB::addOIDtoSelection-exit") );
	return ret;
}


bool 
OOF_simpleRecordBackend::contains(oidT inOID) const
{
	bool ret = mSelection.contains(inOID);
	return ret;
}


void 
OOF_simpleRecordBackend::dropRecordFromSelection(unsigned long inRecNum)
{
	assert( Invariant("simpleB::dropRecordFromSelection-entry") );
	
	mSelection.deleteItem(inRecNum);

	assert( Invariant("simpleB::dropRecordFromSelection-exit") );
}

 



/**
	\param inOID record address, zero is accepted but causes selectNone()
	dbTable::selectJustOID will react similarly to zero oidT.
	\todo validate that inOID is valid existing record
*/
bool 
OOF_simpleRecordBackend::selectJustOID(oidT inOID)
{
	assert( Invariant("simpleB::selectJustOID-entry") );

	bool ret = true;
	if (inOID) {
		mSelection.selectOneRec(inOID);
		start();
	}
	else {
	#ifdef OOF_DEBUG
		dbConnect::raise("selectJustOID() called with 0 recordAddress", true /* continue */);
	#endif
		selectNone();
		ret = false;
	}
	assert( Invariant("simpleB::selectJustOID-exit") );

	return ret;
}


void 
OOF_simpleRecordBackend::selectJustCurrent()
{
	assert( Invariant("simpleB::selectJustCurrent-entry") );

	selectJustOID(mCurrLoadedRecOffset);

	assert( Invariant("simpleB::selectJustCurrent-exit") );
}

bool 
OOF_simpleRecordBackend::isRecordLoaded() const
{
	return (mRecordState!=eUnloaded);
}


bool 
OOF_simpleRecordBackend::isNewRecord() const
{
	return isNew();
}


void 
OOF_simpleRecordBackend::setSortOrder(dbSorter* adoptSorter)
{
	assert( Invariant("simpleB::setSortOrder-entry") );

	mSelection.setSortOrder(adoptSorter);

	assert( Invariant("simpleB::setSortOrder-exit") );
}


dbSorter* 
OOF_simpleRecordBackend::sorter() const
{
	return mSelection.sorter();
}


dbSelection 
OOF_simpleRecordBackend::currentSelection() const
{
	assert( Invariant("OOF_simpleRecordBackend::currentSelection") );
	return dbSelection(&mSelection);
}


dbSelection
OOF_simpleRecordBackend::makeEmptySelection() const
{
	return dbSelection(new OOF_recordSelection(mTable->prototype()));
}


void 
OOF_simpleRecordBackend::setSelection(const dbSelection& rhs)
{
	setSelection(rhs.internalSelection());
}


void 
OOF_simpleRecordBackend::setSelection(const OOF_Selection* rhs)
{
	assert( Invariant("simpleB::setSelection-oofsel-entry") );

// dangerous downcast if user is stupid enough to mix selections but highly unlikely
	const OOF_recordSelection* rhsSel = (const OOF_recordSelection*) 	rhs;
	mSelection = *rhsSel;
	const unsigned long recsInSelection = mSelection.count();
	const unsigned long currentRec = mSelection.iter();
	if (recsInSelection)
		if (currentRec < recsInSelection)	// haven't iterated off end
			gotoRecord(currentRec);	// goto current record in selection
		else
			start();

	assert( Invariant("simpleB::setSelection-oofsel-exit") );
}


void 
OOF_simpleRecordBackend::selectAll()
{
	assert( Invariant("simpleB::selectAll-entry") );

	mSelection.selectAll();
	unloadRecord();
	mCurrLoadedRecOffset=0;
	mSelection.gotoRecord(0);	// reset iterator to reasonable value

	assert( Invariant("simpleB::selectAll-exit") );
}


void 
OOF_simpleRecordBackend::selectNone()
{
	assert( Invariant("simpleB::selectNone-entry") );

	unloadRecord();
	mSelection.selectNone();
	mCurrLoadedRecOffset=0;

	assert( Invariant("simpleB::selectNone-exit") );
}


void 
OOF_simpleRecordBackend::pack()
{
// null method - most backends ignore
}

 
unsigned int 
OOF_simpleRecordBackend::numIndexes() const 
{
	return mNumIndexes;
}


unsigned int 
OOF_simpleRecordBackend::numFiles() const 
{
	return mNumFiles;
}


bool 
OOF_simpleRecordBackend::CanUseIndex(const dbField* /*theFld*/, dbQueryClause::BinaryQueryOps) const
{
	return false;
}


unsigned long 
OOF_simpleRecordBackend::blobPointerReferenceSize() const
{
// NOT YET IMPLEMENTED handle inline blobs
	return sizeof(long)+sizeof(long);
}


void 
OOF_simpleRecordBackend::describeState(ostream& os) const
{
	os 	<< "RecState: " << mRecordState << "\t" 
		<< "Offset: " << mCurrLoadedRecOffset << "\t" ;
		if (mDirty)
			os << "Dirty\t";
		else
			os << "Clean\t";
	mSelection.describeState(os);
	os << endl;
}
// -------------------------------------------------------
//     O O F _ r e c o r d  C a c h e C o p i e r
// -------------------------------------------------------
OOF_recordCacheCopier::OOF_recordCacheCopier(oofBroadcaster* listensTo, OOF_recordCache* adoptsCache, OOF_simpleRecordBackend* inBackend, OOF_recordBufferedContext* savedContext) :
		oofSingleListener(listensTo),
		mCache(adoptsCache),
		mBackend(inBackend),
		mSavedContext(savedContext)
{
}


OOF_recordCacheCopier::~OOF_recordCacheCopier()
{
	delete mCache;
}


#ifdef NDEBUG	
bool 
OOF_recordCacheCopier::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
#else
bool 
OOF_recordCacheCopier::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
#endif
{
	if (msg==OOFmsg_BroadcasterClosing)  // rare but possible in some exception handling
		return true;
		
// while we have cached records, make each one current and dirty
// so the save loop will continue
	assert(msg==OOFmsg_SaveRecord);  // we are only called from a tight loop
	unsigned int numCached = mCache->count();
	if (numCached) {
		mBackend->AdoptBuffer(mCache->orphanBuffer(numCached-1));
		mBackend->markDirty();
	}
	else { // all finished!
		mBackend->AdoptBuffer(mSavedContext);	
		delete this;
	}
	return true;
}


// -------------------------------------------------------
//           O O F _ d i r t y F i e l d M a p 
// -------------------------------------------------------
OOF_dirtyFieldMap::OOF_dirtyFieldMap() :
	mNumFields(0),
	mDirtyMap(0)
{
}
	

OOF_dirtyFieldMap::OOF_dirtyFieldMap(const OOF_dirtyFieldMap& rhs) :
	mNumFields(rhs.mNumFields),
	mDirtyMap(0)
{
	finishMap(mNumFields);
}
	

OOF_dirtyFieldMap::~OOF_dirtyFieldMap()
{
	delete[] mDirtyMap;
}
	

	
void 
OOF_dirtyFieldMap::finishMap(unsigned short numFields)
{
	mNumFields = numFields;
	delete[] mDirtyMap;
	mDirtyMap = new bool[numFields];
	reset();
}
	

bool 
OOF_dirtyFieldMap::anyDirty() const
{
	for (unsigned short i=0; i<mNumFields; i++) {
		if (mDirtyMap[i])
			return true;
	}
	return false;
}
	

void 
OOF_dirtyFieldMap::reset()
{
	for (unsigned short i=0; i<mNumFields; i++) {
		mDirtyMap[i] = false;
	}
}
	


// -------------------------------------------------------
//            E s e l e c t i o n S t a t e
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, OOF_SelectionStateDefiner::EselectionState st)
{
	switch (st) {
	case (OOF_SelectionStateDefiner::empty) :
		os << "empty";
		break;
		
	case (OOF_SelectionStateDefiner::oneRec) :
		os << "oneRec";
		break;
		
	case (OOF_SelectionStateDefiner::someRecs) :
		os << "someRecs";
		break;

	default :
		os << "allRecs";
	}
	return os;
}


// -------------------------------------------------------
//            E R e c o r d S t a t e
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, OOF_recordBufferedContext::ERecordState st)
{
	switch (st) {
	case (OOF_recordBufferedContext::eNew) :
		os << "eNew";
		break;
		
	case (OOF_recordBufferedContext::eLoaded) :
		os << "eLoaded";
		break;
		
	default :
		os << "eUnloaded";
	}
	return os;
}


