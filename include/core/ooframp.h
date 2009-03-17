#ifndef H_OOFramp
#define H_OOFramp

// OOFILE RAM Persistent backend implementation
// EXPORTED header - this is the one included in client programs
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFRAM
	#include "oofram.h"
#endif

/**
	\defgroup oofDatabase_ramp OOFILE Database backend for RAM-resident databases.
	\ingroup oofDatabase
*/


/**
	Database connection to a RAM database that can be persisted out to a single file.
	\see OOF_rampBackend
	\warning files can't be shared cross-platform as they are endian-specific
	\ingroup oofDatabase_ramp
*/
class OOFILE_EXPORT dbConnect_ramp : public dbConnect_ram {
public:
	dbConnect_ramp(bool isTemporary=false);	
	virtual ~dbConnect_ramp();
	
private:
	dbConnect_ramp(const dbConnect_ramp&) { assert(0); };  // NEVER CALLED

public:	
	virtual void newConnection(const char* connectionName);
	virtual void openConnection(const char* connectionName);
	virtual bool openOrCreateConnection(const char* connectionName); 
	virtual void writeOOFSchema();

#ifdef _Macintosh
	virtual void newConnection(const FSSpec&);
	virtual void openConnection(const FSSpec&);
#endif
	virtual void close();
	virtual void deleteStorage();
	void save();
	void revert();

private:
	virtual OOF_tableBackend *MakeTableBackend(dbTable*);
	void FinishNewConnection(const char* connectionName);
	void FinishOpenConnection(const char* connectionName);
	bool FinishOpenOrCreateConnection(const char* connectionName); 
	virtual void SetupConnection(const OOF_String& connectionName);
	void ReadAllTables();
	void writeRampSchema();
// data storage
	FILE*	mFP;	// owned
	bool mWritesOOFSchema;
	friend class OOF_rampBackend;
};
OOFILE_EXPORT ostream& operator<<(ostream& os, dbConnect_ramp& connect);
#endif

