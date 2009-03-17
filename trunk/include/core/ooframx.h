// OOFILE RAM backend private headers
// 
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFRAMX
#define H_OOFRAMX

#ifndef H_OOFRAM
	#include "oofram.h"
#endif
#ifndef H_OOFRECS
	#include "oofrecs.h"
#endif

/**
	implementation of RAM-based database.
	Reuses OOF_recordCache to also manage the entire database store.
	Doesn't provide any indexing.
	Suitable for collections of hundreds, maybe thousands of records
	depending on available memory. Quite fast to iterate.
	\note often used to create a new temporary database for a report or list in GUI.
	\todo packing of records in the cache to avoid wasting space used
	for alignment and padding.
   \ingroup oofDatabase_ram oofInternals
*/
class OOF_ramBackend : public OOF_simpleRecordBackend
{
protected:
	OOF_ramBackend(dbTable*);
	OOF_ramBackend(const OOF_ramBackend&, dbTable*, dbTable::selSharingT);
	virtual ~OOF_ramBackend();
	
 public:
	virtual void addIndices(const char* newIDXname);
	virtual void suppressIndices();
	virtual void rebuild();
	virtual void close();
	virtual void createTableInConnection(const dbConnect*);
	virtual bool openTableInConnection(const dbConnect*,const bool createIfMissing = false);
	virtual OOF_tableBackend* clone(dbTable::selSharingT, dbTable*) const;

// recordwise access
	virtual void start();
	virtual void next();
	virtual unsigned long countAll() const;

// data access
	virtual void deleteRecord();
	
	virtual void loadBlob(const dbBLOB*);

// reflective operators
	virtual unsigned long sequenceNumber() const;

// public search functions
	virtual bool gotoRecord(unsigned long relativeNum);

protected:
	virtual void BuildBackendtables(bool rebuilding);
	virtual void LoadRecordAtOffset(unsigned long offset);
	virtual void SaveContext();
	void DeleteBlobBodies(char* inBuffer);	

// data storage
private:
	unsigned long mCacheCursor;
	unsigned short* mCacheRefCount;	// owned
	static unsigned long sSequenceNumber;
	OOF_recordCache*  mRamRecords;  // owned
	
	friend OOF_tableBackend *dbConnect_ram::justMakeTableBackend(dbTable*);
};

#endif

