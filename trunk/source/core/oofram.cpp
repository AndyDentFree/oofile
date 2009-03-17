// OOFILE RAM backend implementation
// 
// COPYRIGHT 1996 A.D. Software, All rights reserved

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFRAMX
	#include "ooframx.h"
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

unsigned long OOF_ramBackend::sSequenceNumber;


// -------------------------------------------------------
//                d b C o n n e c t _ r a m  
// -------------------------------------------------------
dbConnect_ram::dbConnect_ram(bool isTemporary) :
	dbConnect(isTemporary)
{}


dbConnect_ram::~dbConnect_ram()
{
	if (mTempDatabase && mOpen) {	// see dbConnect_ramp persistent subclass dtor
	// If we get here we are just a dbConnect_ram that's been used temporarily
	// to create tables.
	
	// "Temporary" for a dbConnect_ram means it is used to create a dbTable
	// which is separately deleted and has a lifespan beyond this connection.
	// I know it's a slightly weird idiom but people have found it very convenient
	// as all they have to do is hang onto the resulting dbTable pointer.
	
	// "non-temporary" for a dbConnect_ram means it owns its tables
	// but it is still not persistent (use dbConnect_ramp subclass for persistence)

	// To use dbConnect_ram as a normal database and have it delete owned tables
	// pass false into the ctor. This means the default for our ctor is "true"
	// which is opposite to the other tables but makes sense.
	

		// gigantic hack to make the connections 0 so the tables won't try to delete me
		// as ram connections violate common use - we create a temporary connection
		// and often just hang onto the dbTable within it
		// so tables are typically deleted AFTER the connection
		const unsigned int numTables=mTables.count();
		for (unsigned int i=0; i<numTables; i++) {
			dbTable* theTable = (dbTable*) mTables.value(i);	// safe downcast
			theTable->mConnection = 0;
		}
		mOpen=false;  // avoid dbConnect dtor destroying owned tables
	}
}


void
dbConnect_ram::close()
{ 
		mOpen = false;  // override because no file to close
}


void 
dbConnect_ram::newConnection(const char* connectionName) 
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("New ram", connectionName);
#endif

	SetupConnection(connectionName);
	MakeAllTablesValid();
}


void 
dbConnect_ram::openConnection(const char* connectionName) 
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("Open ram", connectionName);
#endif

	SetupConnection(connectionName);
	MakeAllTablesValid();
}

bool
dbConnect_ram::openOrCreateConnection(const char* connectionName) 
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("New ram", connectionName);
#endif

	SetupConnection(connectionName);
	MakeAllTablesValid();
	return false; //As it always creates  
}


#ifdef _Macintosh
void 
dbConnect_ram::newConnection(const FSSpec& inMacFSSpec) 
{
	newConnection(OOF_MacString(inMacFSSpec.name));
}


void 
dbConnect_ram::openConnection(const FSSpec& inMacFSSpec) 
{
	openConnection(OOF_MacString(inMacFSSpec.name));
}
#endif


void 
dbConnect_ram::SetupConnection(const OOF_String& connectionName)
{
	mConnectionName = connectionName;

	mTables.start();
	while (mTables.more()) {
		dbTable *theTable = (dbTable *) mTables();	// safe downcast
		theTable->buildSchema();
		mTables.next();
	}
	mOpen = true;
	sCurrentlyConstructing = 0;		// forces some asserts if define tables & relations after this
}

void
dbConnect_ram::writeOOFSchema()
{
	//null method as data is not saved

}
OOF_tableBackend *
dbConnect_ram::MakeTableBackend(dbTable* forTable) 
{
	#ifdef OOF_Demo
		if (mTables.count()==3) {
			raise("Sorry, in the demo you are only allowed to create a maximum of 2 tables");
			abort();
		}
	#endif
	return justMakeTableBackend(forTable);
}


OOF_tableBackend *
dbConnect_ram::justMakeTableBackend(dbTable* forTable) 
{
	return new OOF_ramBackend(forTable);
}


ostream& 
operator<<(ostream& os, dbConnect_ram& connect)
{
//	connect.describe(os);
	connect.dumpData(os);
	return os;
}


// -------------------------------------------------------
//           O O F _ r a m  B a c k e n d 
// -------------------------------------------------------
OOF_ramBackend::OOF_ramBackend(dbTable* inTable) :
	OOF_simpleRecordBackend(inTable),
	mCacheCursor(0)
{
	mCacheRefCount = new unsigned short(1);
	mRamRecords = new OOF_recordCache();
}


OOF_ramBackend::OOF_ramBackend(const OOF_ramBackend& rhs, dbTable* inTable, dbTable::selSharingT selSharing) :
	OOF_simpleRecordBackend(rhs, selSharing, inTable),
	mCacheCursor(rhs.mCacheCursor),
	mCacheRefCount(rhs.mCacheRefCount)
{
// unlike the mDirtyCache of simpleRecordBackend, we share our record cache
// as it is a repository of records
	
	mRamRecords = rhs.mRamRecords;
	(*mCacheRefCount)++;
}


OOF_ramBackend::~OOF_ramBackend()
{
	if (mRamRecords) {
		if (*mCacheRefCount == 1) {
		// before deleting cached records, delete their attached blobs
			const unsigned long numCached = mRamRecords->count();
			for (unsigned long i=0; i<numCached; i++) {
				OOF_recordBufferedContext* ctx = mRamRecords->item(i);  
				if (ctx) {
					char* ctxBuffer = ctx->buffer();
					if (ctxBuffer)
						DeleteBlobBodies(ctxBuffer);
				}
			}
			mRamRecords->deleteAll();  // still call this as it does other cleanup as well as deleting ctx which we could do above
			delete mRamRecords;
			delete mCacheRefCount;
		}
		else {
			(*mCacheRefCount)--;
			mRamRecords = 0;		// so our parent doesn't delete it!!!
		}		
	}
}


void
OOF_ramBackend::DeleteBlobBodies(char* inBuffer)
{
	assert( Invariant("ramB::DeleteBlobBodies-entry") );

	if (mBlobFieldNums) {  // cleanup blobs we still own
		const unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			const fieldNumT fldNum = mBlobFieldNums->item(i);
			char* blobBuf = (char*) BlobPosFromBuffer(fldNum, inBuffer);
			delete[] blobBuf;
			SetBlobPosInBuffer(fldNum, 0, inBuffer);
			// len was already set when last updated blob
		}
	}
	assert( Invariant("ramB::DeleteBlobBodies-exit") );
}


void 
OOF_ramBackend::BuildBackendtables(bool rebuilding)
{
	assert( Invariant("ramB::BuildBackendtables-entry") );

	assert(!mFieldBufMap);
	assert(!rebuilding);	// NOT YET IMPLEMENTED
	const fieldNumT numFields = mFields.count();
	mDirtyMap.finishMap(numFields);

	mFieldBufMap = new OOF_recordFieldEntry[numFields];
	assert(mFieldBufMap);
	mFieldBufMapRefCount = new unsigned int(1);
	assert(mFieldBufMapRefCount);
	
	mRecBufLen = 0;
	for (unsigned int i = 0; i < numFields; i++) {
		dbField  *fld = (dbField  *) mFields[i];  // safe downcast
#ifdef OOF_DEBUG
		if (mRecBufLen%2==1)
			dbConnect::raise(stringstream() << flush << "OOF_ramBackend::BuildBackendtables odd buffer size at field:" << fld->fieldName());
#endif
// work out if need to pad field alignment, by inserting dummy field (just skip space)
		const unsigned int alignTo = FieldAlignsTo(fld);
		if (alignTo>2) {
			unsigned int alignRemainder = mRecBufLen%alignTo;
			if (alignRemainder) {
				mRecBufLen += (alignTo - alignRemainder);
			}
		}
		if (fld->fieldIsBlob()) {
			if (!mBlobFieldNums)
				mBlobFieldNums = new OOF_ExpandableLongArray(0,2,2); // 0 def value, alloc 2 slots & grow by same
			mBlobFieldNums->append(i);
		}

		mFieldBufMap[i].mOffset = mRecBufLen;
		unsigned long len;
		if (fld->fieldIsVirtual()) {
			len = 0;
		}
		else {
			len = PadFieldWidthForAlignment(fld);
			mRecBufLen += len;  // watch the order here - inc AFTER copy to mOffset
		}
		mFieldBufMap[i].mLength = len;
	}
		
	assert( Invariant("ramB::BuildBackendtables-exit") );
}


void 
OOF_ramBackend::createTableInConnection(const dbConnect*) 
{
	// null method - no physical table created
}


bool 
OOF_ramBackend::openTableInConnection(const dbConnect*,const bool createIfMissing) 
{
	return createIfMissing;
	// null method - no physical table to open
}


OOF_tableBackend* 
OOF_ramBackend::clone(dbTable::selSharingT selSharing, dbTable* theTable) const 
{
	return new OOF_ramBackend(*this, theTable, selSharing);
}



void 
OOF_ramBackend::start() 
{
	assert( Invariant("ramB::start-entry") );
	
	mSelection.start();
	assert( Invariant("ramB::start-selStart") );

	switch (mSelection.state()) {
	
	case OOF_recordSelection::empty :
		unloadRecord();
		break;	

	case OOF_recordSelection:: oneRec :
		LoadRecordAtOffset(mSelection.oneRecOffset());
		break;	
		
	case OOF_recordSelection:: someRecs : {
		LoadRecordAtOffset(mSelection());
		break;	
	}
		
	case OOF_recordSelection:: allRecs : {
		if (countAll()) {
			mCacheCursor=0;
			for(mCacheCursor=0; ;mCacheCursor++) {	// we're guaranteed a record by the countAll() above, so no terminating check
				if(mDirtyRecordCache){
					unsigned long cachedIndex = mDirtyRecordCache->containsOffset(mCacheCursor+1);
						if (cachedIndex!=OOF_recordCache::kNotFound) 
							break;	
				}
				const OOF_recordBufferedContext* ctx = mRamRecords->item(mCacheCursor);
				if (ctx)
					break;
			}
			LoadRecordAtOffset(mCacheCursor+1);	// 1-based offset
		}
		else
			unloadRecord();
		break;
	}
	}
	assert( Invariant("ramB::start-exit") );
}


void 
OOF_ramBackend::next() 
{
	assert( Invariant("ramB::next-entry") );

	if (mSelection.iter() == ULONG_MAX) {
		start();
		assert( Invariant("ramB::next-exit invalid iter") );
		return;
	}

	switch (mSelection.state()) {

	case OOF_recordSelection::empty :
		unloadRecord();
		break;	

	case OOF_recordSelection:: oneRec :
		unloadRecord();
		mSelection.next();
		break;	

	case OOF_recordSelection::someRecs : 
		mSelection.next();
		if (mSelection.more())
			LoadRecordAtOffset(mSelection());
		else
			unloadRecord();
		break;	
		

	case OOF_recordSelection::allRecs : 
		mSelection.next();
		if (more()) {
			for(mCacheCursor++; ;mCacheCursor++) {	// we're guaranteed a record by the more() above, so no terminating check
				const OOF_recordBufferedContext* ctx = mRamRecords->item(mCacheCursor);
				if (ctx)	// skip deleted records!!!!
					break;
			}
			LoadRecordAtOffset(mCacheCursor+1);	// 1-based offset
		}
		else
			unloadRecord();
		break;
// NOT YET IMPLEMENTED - error trapping & flushing dirty buffer
	}

	assert( Invariant("ramB::next-exit") );
}


unsigned long 
OOF_ramBackend::countAll() const 
{
// we keep all records in the dirty cache
	if (!mRamRecords)
		return 0;
	unsigned long ret = mRamRecords->countOccupied();
	if (isNew())
		ret++;
	return ret;
}


void 
OOF_ramBackend::deleteRecord() 
{
	assert( Invariant("ramB::deleteRecord-entry") );
	
	if (CommonSimpleDelete()){
		assert( Invariant("ramB::deleteRecord-exit commonSimpleHandled") );
		return;
	}				
	mRamRecords->deleteRecord(mCurrLoadedRecOffset);
	unloadRecord();  // gets rid of in-mem

	switch (mSelection.state()) {	// state probably changed by above delete
	
	case OOF_recordSelection::empty : {
#ifdef OOF_DEBUG
		dbConnect::raise(stringstream() << flush << "deleteRecord() called on empty selection in table: " << mTable->tableName(), false);
#endif
		break;	
	}
	
	case OOF_recordSelection::oneRec : {
		selectNone();
		break;	
	}

	case OOF_recordSelection::someRecs : {
		mSelection.deleteCurrent();	// takes care of position if delete last
		LoadRecordAtOffset(mSelection());
// ES
/*
		const unsigned long currRec = recordNumber();
		mSelection.deleteCurrent();	// takes care of position if delete last
		LoadRecordAtOffset(mSelection[currRec]);
*/
		break;	
	}
		
	case OOF_recordSelection::allRecs :  {
		if (countAll() == 0) { // just deleted the last record
			selectNone();
			mCacheCursor = 0;
		}
		else  {
			// Elemir pointed out this returns false if (atLast())
			if (mSelection.iter() >= countAll())
				start();
			else  {
				if (mSelection.iter()==0) 	// reload first record (which has just been deleted, so will skip to next)
					start();
				else {
					mSelection.prev();
					next();	// effectively go to where we are in rel rec numbers
					// but with skipping over empty records, ours and maybe following	
				}
			}
		}
	}
	}
	assert( Invariant("ramB::deleteRecord-exit") );
}


void 
OOF_ramBackend::SaveContext() 
{
	assert( Invariant("ramB::SaveContext-entry") );

	if (mBlobFieldNums) {
	   	const unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			const fieldNumT fldNum = mBlobFieldNums->item(i);
			dbBLOB* fld = (dbBLOB *) mFields[fldNum];	// safe downcast
			if (fld->isDirty()) {
				unsigned long blobAddress = (unsigned long) (fld->orphanBody());
				SetBlobPosInBuffer(fldNum, blobAddress, mBuffer);
			}
			// len was already set when last updated blob
		}
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	}
	
	if (isNew()) {	// add a cache entry
		bool reusingSlot = false;
		assert (mRamRecords);
		const unsigned long cachedIndex = mRamRecords->firstFree();
		if (cachedIndex == OOF_recordCache::kNotFound) 
			mCurrLoadedRecOffset = mRamRecords->count()+1;	// OFFSETS ARE 1-BASED SO NO CHANCE OF 0 RECORD VALUE
		else {	// found slot to re-use
			mCurrLoadedRecOffset = cachedIndex+1;
			reusingSlot = true;
		}

		// got correct record offset by now and valid cache		
		if (mSelection.state()==OOF_recordSelection::allRecs)
			mSelection.gotoRecord(ULONG_MAX);	// invalid!
		else {
			if (tableCachesDirtyRecords())
				mSelection.updateCurrent(mCurrLoadedRecOffset);
			else
				mSelection.append(mCurrLoadedRecOffset);
		}
		mRecordState = eLoaded;

		OOF_recordBufferedContext* ctx = new OOF_recordBufferedContext(*this);
		// now put it in the cache
		if (reusingSlot) 
			mRamRecords->reuseDeletedItem(mCurrLoadedRecOffset-1, ctx);
		else
			mRamRecords->append(ctx);
	}
	else {	// copy existing record to store
		OOF_recordBufferedContext* ctx = mRamRecords->item(mCurrLoadedRecOffset-1);	// convert 1-based offset to 0-based index
		assert(ctx);
		ctx->copyFrom(this);
	}	
	mDirty = false;

	assert( Invariant("ramB::SaveContext-exit") );
}


void 
OOF_ramBackend::loadBlob(const dbBLOB* blob) 
{
	assert( Invariant("ramB::loadBlob-entry") );

	void* blobAddress = (void*) BlobPosFromBuffer(blob, mBuffer);
	if (blobAddress==0) {
		blob->reset();
		return;   // EXIT FOR NO BLOB
	}
	
	unsigned long len = BlobLenFromBuffer(blob, mBuffer);
	if (len==0) {
		blob->reset();
		return;   // EXIT FOR NO BLOB (was a blob and shrunk to nothing)
	}
	
	void *body = blob->allocRoomFor(len);
	assert(body);
	memcpy(body, blobAddress, len);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	assert( Invariant("ramB::loadBlob-exit") );
}


unsigned long 
OOF_ramBackend::sequenceNumber() const 
{
	sSequenceNumber++;
	return sSequenceNumber;
}


bool 
OOF_ramBackend::gotoRecord(unsigned long relativeNum) 
{
	assert( Invariant("ramB::gotoRecord-entry") );

#ifdef OOF_DEBUG
	const unsigned long numRecs = count();
	if (relativeNum >= numRecs) {
		dbConnect::raise(stringstream() << flush << "gotoRecord - attempt to go to rec: " <<
			relativeNum << " in selection of " << numRecs);

		assert( Invariant("ramB::gotoRecord-exit beyond range") );
		return false;
	}
#endif

	switch (mSelection.state()) {

	case OOF_recordSelection::oneRec :{
// this may seem screwy but is legit if that rec has just been pushed on the stack
// by some other code - there are inefficiencies in the automatic ContextChange
// calls that can cause this push/pop behaviour
		LoadRecordAtOffset(mSelection.oneRecOffset());
		assert( Invariant("ramB::gotoRecord-exit oneRec") );
		return true;
	}

	case OOF_recordSelection::someRecs :{
		LoadRecordAtOffset(mSelection[relativeNum]);
		mSelection.gotoRecord(relativeNum);
		assert( Invariant("ramB::gotoRecord-exit someRecs") );
		return true;
	}

	case OOF_recordSelection::allRecs :{
		if (mRamRecords->anyDeleted()){
			const bool ret = GotoRecordForAllUnindexed(relativeNum);	// must skip empty slots
			assert( Invariant("ramB::gotoRecord-exit allRecsWithDeleted") );
			return ret;
		}
		else	{
			LoadRecordAtOffset(relativeNum+1);	// offsets are 1-based, records & selections 0-based
			mSelection.gotoRecord(relativeNum);
			mCacheCursor = relativeNum;	// extra iterator used for all recs
			assert( Invariant("ramB::gotoRecord-exit allRecs") );
			return true;
		}
	}

	default :
		assert( Invariant("ramB::gotoRecord-exit") );
		return false;
	}
}

 
void 
OOF_ramBackend::LoadRecordAtOffset(unsigned long offset) 
{
	assert( Invariant("ramB::LoadRecordAtOffset-entry") );
	
	if(mDirty)
		CacheDirtyCurrentRecord();
			
	if (!MaybeLoadRecordFromCache(offset)) {
		assert(offset <= mRamRecords->count());	// offset is 1-based in ram
		const OOF_recordBufferedContext* ctx = mRamRecords->item(offset-1);
		assert(ctx);
		copyFrom(ctx);
		mCurrLoadedRecOffset = offset;
		mRecordState = eLoaded;
		mDirty = false;
	}

	assert( Invariant("ramB::LoadRecordAtOffset-exit") );
}


// STUBBED FUNCTIONS NOT AVAILABLE IN THIS BACKEND
void OOF_ramBackend::addIndices(const char* /*newIDXname*/) { assert(0); }
void OOF_ramBackend::suppressIndices() { assert(0); }
void OOF_ramBackend::rebuild() { assert(0);}
void OOF_ramBackend::close() { assert(0);}

