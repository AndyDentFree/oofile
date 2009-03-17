// OOFILE RAM backend implementation
// 
// COPYRIGHT 1996 A.D. Software, All rights reserved

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"   // numeric fields
#endif
#ifndef OOF_EXCEP
	#include "oofexcep.h"
#endif
#ifndef H_OOFRAMPX
	#include "ooframpx.h"
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif


// -------------------------------------------------------
//         d b C o n n e c t _ r a m p
// -------------------------------------------------------
dbConnect_ramp::dbConnect_ramp(bool isTemporary) :
	dbConnect_ram(isTemporary),
	mWritesOOFSchema(false)
{}


dbConnect_ramp::~dbConnect_ramp()
{
	const bool wasOpen = mOpen;
	if (wasOpen)
		close(); // close our storage with virtual method

	if (mTempDatabase)
		deleteStorage();  // delete with virtual method

	mTempDatabase = false;  // specifically makes dbConnect_ram parent dtor ignore tables
	
	mOpen = wasOpen;  // dbConnect dtor needs to know we were open at dtor time!
}


OOF_tableBackend *
dbConnect_ramp::MakeTableBackend(dbTable* forTable) 
{
	#ifdef OOF_Demo
		if (mTables.count()==3) {
			raise("Sorry, in the demo you are only allowed to create a maximum of 2 tables");
			abort();
		}
	#endif
	OOF_tableBackend* ret = new OOF_rampBackend(forTable, this);
	return ret;
}


void 
dbConnect_ramp::newConnection(const char* connectionName) 
{
	assert(connectionName);
	if (mOpen) {
		oofE_RampConnectionError excp(this, "newConnection called on already open dbConnect_ramp object");
		RAISE_EXCEPTION(excp);
	}

	stSaveDirectory willReturnToCurrentDir;

	char* fileName = new char[strlen(connectionName)+1];	// expect file name on end of path
		
	mDatabaseDir = new oofDirectory(connectionName, fileName, true /* create dirs if necessary */);
	// oofDirectory parsing should have populated the filename
	if (strlen(fileName)==0) {
		oofE_RampConnectionError excp(this, stringstream() << flush << "File name missing: '" << connectionName << "'");
		RAISE_EXCEPTION(excp);
	}

	FinishNewConnection(fileName);	
	delete[] fileName;
}


void 
dbConnect_ramp::openConnection(const char* connectionName) 
{
	assert(connectionName);
	if (mOpen) {
		oofE_RampConnectionError excp(this, "openConnection called on already open dbConnect_ramp object");
		RAISE_EXCEPTION(excp);
	}

	stSaveDirectory willReturnToCurrentDir;

	char* fileName = new char[strlen(connectionName)+1];	// expect file name on end of path
		
	mDatabaseDir = new oofDirectory(connectionName, fileName, true /* create dirs if necessary */);
	// oofDirectory parsing should have populated the filename
	if (strlen(fileName)==0) {
		oofE_RampConnectionError excp(this, stringstream() << flush << "File name missing: '" << connectionName << "'");
		RAISE_EXCEPTION(excp);
	}

	FinishOpenConnection(fileName);	
	delete[] fileName;
}

bool
dbConnect_ramp::openOrCreateConnection(const char* connectionName) 
{
	assert(connectionName);
	if (mOpen) {
		oofE_RampConnectionError excp(this, "openOrConnection called on already open dbConnect_ramp object");
		RAISE_EXCEPTION(excp);
	}
	stSaveDirectory willReturnToCurrentDir;

	char* fileName = new char[strlen(connectionName)+1];	// expect file name on end of path
		
	mDatabaseDir = new oofDirectory(connectionName, fileName, true /* create dirs if necessary */);
	// oofDirectory parsing should have populated the filename
	if (strlen(fileName)==0) {
		oofE_RampConnectionError excp(this, stringstream() << flush << "File name missing: '" << connectionName << "'");
		RAISE_EXCEPTION(excp);
	}
	return  FinishOpenOrCreateConnection(fileName);	
	delete[] fileName;	
}


#ifdef _Macintosh
void 
dbConnect_ramp::newConnection(const FSSpec& inMacFSSpec) 
{
	if (mOpen) {
		oofE_RampConnectionError excp(this, "newConnection called on already open dbConnect_ramp object");
		RAISE_EXCEPTION(excp);
	}

	stSaveDirectory willReturnToCurrentDir;

	mDatabaseDir = new oofDirectory(inMacFSSpec);
	FinishNewConnection(OOF_MacString(inMacFSSpec.name));
}


void 
dbConnect_ramp::openConnection(const FSSpec& inMacFSSpec) 
{
	if (mOpen) {
		oofE_RampConnectionError excp(this, "openConnection called on already open dbConnect_ramp object");
		RAISE_EXCEPTION(excp);
	}

	stSaveDirectory willReturnToCurrentDir;

	mDatabaseDir = new oofDirectory(inMacFSSpec);
	FinishOpenConnection(OOF_MacString(inMacFSSpec.name));
}
#endif


void
dbConnect_ramp::FinishNewConnection(const char* connectionName) 
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("New persistent ram", connectionName);
#endif

	SetupConnection(connectionName);
	MakeAllTablesValid();
	mFP = ::fopen(connectionName,"wb+");
	if(mFP)	{
		// nothing to do, all table backends are empty
	} 
	else {
		oofE_RampConnectionError excp(this, stringstream() << flush 
			<< "error in dbConnect_ramp::FinishNewConnection() "
			<< "failed to create file: " << connectionName 
			<< endl);
		RAISE_EXCEPTION(excp);
	}
}


void
dbConnect_ramp::FinishOpenConnection(const char* connectionName)
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("Open persistent ram", connectionName);
#endif

	SetupConnection(connectionName);
	MakeAllTablesValid();
	mFP = fopen(connectionName,"rb+");
	if(mFP)	{
		ReadAllTables();
	} 
	else {
		oofE_RampConnectionError excp(this, stringstream() << flush 
			<< "error in dbConnect_ramp::FinishNewConnection() "
			<< "failed to open file: " << connectionName 
			<< endl);
		RAISE_EXCEPTION(excp);
	}
}

bool
dbConnect_ramp::FinishOpenOrCreateConnection(const char* connectionName)
{
#ifdef OOF_DEBUG_LOG_NAME
	LogConnection("Open persistent ram", connectionName);
#endif
	bool opened;
	
	SetupConnection(connectionName);
	MakeAllTablesValid();
	mFP = fopen(connectionName,"rb+");
	if(mFP)	{
		opened = true;
		ReadAllTables();
	} 
	else {// notthere so create it
		mFP = ::fopen(connectionName,"wb+");
		if(mFP)	{
			opened = false; 
			// nothing to do, all table backends are empty
		} 
		else {
			oofE_RampConnectionError excp(this, stringstream() << flush 
				<< "error in dbConnect_ramp::FinishNewConnection() "
				<< "failed to create file: " << connectionName 
				<< endl);
			RAISE_EXCEPTION(excp);
		}
	}
	return opened;
}


void 
dbConnect_ramp::SetupConnection(const OOF_String& connectionName)
{
	assert(mDatabaseDir);  // should be setup when goto destination in open or newConnection
	mDatabaseDir->gotoDirectory();
	dbConnect_ram::SetupConnection(connectionName);
}


void
dbConnect_ramp::revert()
{
	for (mTables.start(); mTables.more(); mTables.next()) {
		dbTable *theTable = (dbTable *) mTables();		// safe downcast
		theTable->deleteAll();	// NOT YET IMPLEMENTED - more efficient delete all at this level
	}
	ReadAllTables();
}


void
dbConnect_ramp::ReadAllTables()
{
	assert(mFP);
	int err = fseek(mFP, 0L, SEEK_SET);	// always start from beginning of file
	assert(err==0);
	for (mTables.start(); mTables.more(); mTables.next()) {
		dbTable *theTable = (dbTable *) mTables();		// safe downcast
		theTable->openTableInConnection(this);	// streams data from us
	}
}



void
dbConnect_ramp::save()
{ 
	assert (mOpen);
	int err = ::fseek(mFP, 0L, SEEK_SET);	// always start from beginning of file
	assert(err==0);
	for (mTables.start(); mTables.more();mTables.next()) {
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
		dbTable *theTable = (dbTable *) mTables();		// safe downcast
		//The following is grotty, but safe because we are downcasting to
		//	an OOF_rampBackend inside a dbConnect_ramp
		((OOF_rampBackend*)theTable->mBackend)->save();
	}	 
	writeOOFSchema();
}


void
dbConnect_ramp::writeOOFSchema()
{
	mWritesOOFSchema = true;
}


/**
	Write a textual schema into the database file.
	\ingroup experimental
*/
void
dbConnect_ramp::writeRampSchema()
{
	// append to end of ramp file
	assert(mOpen);
	FILE* fp = mFP;
	assert(fp);
	const oofString schemaStr = MakeOOFSchemaString();
	if (schemaStr.length()>0)
	{
		const unsigned long numWritten =  fprintf(fp,"%s",schemaStr.chars());

		if (numWritten != schemaStr.length()) {
			oofE_RampConnectionError excp(this, stringstream() << flush 
				<< "error in dbConnect_ramp::writeRampSchema() "
				<< "failed to write schema: " << mConnectionName 
				<< endl);
			RAISE_EXCEPTION(excp);
		}
	}
}


void
dbConnect_ramp::close()
{ 
	
	if (mOpen) {
		assert (mOpen);
		save();
		if(mWritesOOFSchema)
			writeRampSchema();
		
	//for (mTables.start(); mTables.more();mTables.next()) {
	//	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	//		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	//	#endif
	//	dbTable *theTable = (dbTable *) mTables();		// safe downcast
	//	theTable->close();
	//}
		mOpen = false;
		::fclose(mFP);
	}
}


void 
dbConnect_ramp::deleteStorage()
{
	assert(mDatabaseDir);
	oofFileRef theFile(*mDatabaseDir, mConnectionName);
	theFile.deleteFile();
}


ostream& 
operator<<(ostream& os, dbConnect_ramp& connect)
{
//	connect.describe(os);
	connect.dumpData(os);
	return os;
}



// -------------------------------------------------------
//           O O F _ r a m p B a c k e n d 
// -------------------------------------------------------
OOF_rampBackend::OOF_rampBackend(dbTable* inTable, dbConnect_ramp* inConnect) :
	OOF_ramBackend(inTable),
	mConnection(inConnect)
{
}


OOF_rampBackend::OOF_rampBackend(const OOF_rampBackend& rhs, dbTable* inTable, dbTable::selSharingT selSharing) :
	OOF_ramBackend(rhs, inTable, selSharing)
{}


OOF_rampBackend::~OOF_rampBackend()
{}


OOF_tableBackend* 
OOF_rampBackend::clone(dbTable::selSharingT selSharing, dbTable* theTable) const 
{
	return new OOF_rampBackend(*this, theTable, selSharing);
}


bool
OOF_rampBackend::openTableInConnection(const dbConnect* inConnect,const bool) 
{
	assert( Invariant("rampB::openTableInConnection-entry") );

	// stream in all data from connection
	assert(inConnect==mConnection);
	FILE* fp = mConnection->mFP;
	assert(fp);
	unsigned long numRecs=0;
	unsigned long numRead = fread(&numRecs,sizeof(numRecs), 1, fp);
	if (numRead==0) {
		RAISE_EXCEPTION(oofE_RamError(mTable, stringstream() << flush 
			<< "OOF_rampBackend::openTableInConnection empty file: "
			<< inConnect->connectionName())
		);
	}
	for (unsigned long i=0; i<numRecs; i++) {
		ReadRecordFromFile(fp);
	}
	selectAll();
	assert( Invariant("rampB::openTableInConnection-exit") );
	return true;
}



void
OOF_rampBackend::save()
{
	assert( Invariant("rampB::save-entry") );
	
	// stream out all data to connection
	FILE* fp = mConnection->mFP;
	assert(fp);
	
	mSelection.selectAll();
	unsigned long numRecs=countAll();
	unsigned long numWritten = fwrite(&numRecs,sizeof(numRecs), 1,fp);	// write number of records before streaming them out
	if (numWritten==0) {
		RAISE_EXCEPTION(oofE_RamError(mTable, stringstream() << flush 
			<< "OOF_rampBackend::close unable to write numRecs to file: "
			<< mConnection->connectionName())
		);
	}
	
	OOF_ExpandableLongArray* saveFieldNums = mBlobFieldNums;
	mBlobFieldNums = 0;
	// HORRIBLE hack for now, to suppress start(), next() etc. from resetting blobs
	// remember, by the time the dtor that calls this is invoked, our blobs are deleted
	
	start();
	for (unsigned long i=0; i<numRecs; i++) {
		mBlobFieldNums = saveFieldNums;
		WriteRecordToFile(fp);	
		mBlobFieldNums = 0;
		unloadRecord();	// so blobs get loaded
		next();
	}
	assert( Invariant("rampB::save-exit") );
}

void 
OOF_rampBackend::close()
{
	assert(0);//save();
}


void
OOF_rampBackend::ReadRecordFromFile(FILE* fp)
{
	assert( Invariant("rampB::ReadRecordFromFile-entry") );

	newRecord();
	unsigned long numRead = fread(mBuffer,sizeof(char), mRecBufLen, fp);
// as saveRecord copies back values from the dbBlobs we don't want to
// get our blobs until now, as we'll directly set the blob values in the buffer

	if (mBlobFieldNums) {
		unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			unsigned long blobLen = BlobLenFromBuffer(fldNum, mBuffer);
			if (blobLen) {
				char* blobAddress = new char[blobLen];
				assert (blobAddress);
				numRead = fread(blobAddress,sizeof(char), blobLen,fp);
				SetBlobPosInBuffer(fldNum, (unsigned long) blobAddress, mBuffer);
			} // blob has data
		}  // loop all blobs
	} // has any blobs
	OOF_ExpandableLongArray* saveFieldNums = mBlobFieldNums;
	// HORRIBLE hack for now, to suppress saveRecord from iterating blobs
	mBlobFieldNums = 0;
	mDirty = true;
	saveRecord();
	mBlobFieldNums = saveFieldNums;
	assert( Invariant("rampB::ReadRecordFromFile-exit") );
}


void
OOF_rampBackend::WriteRecordToFile(FILE* fp)
{
	assert( Invariant("rampB::WriteRecordToFile-entry") );
	size_t numWritten = fwrite(mBuffer,sizeof(char), mRecBufLen,fp);

	if (mBlobFieldNums) {
		unsigned int numBlobs = mBlobFieldNums->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			fieldNumT fldNum = mBlobFieldNums->item(i);
			unsigned long blobLen = BlobLenFromBuffer(fldNum, mBuffer);
			if (blobLen) {
				void* blobAddress = (void*) BlobPosFromBuffer(fldNum, mBuffer);
				assert (blobAddress);	// if has len must have storage
				numWritten = fwrite(blobAddress,sizeof(char), blobLen,fp);
			} // blob has data
		}  // loop all blobs
	} // has any blobs
	assert( Invariant("rampB::WriteRecordToFile-exit") );
}
