#ifndef H_OOFRAM
#define H_OOFRAM

// OOFILE RAM backend implementation
// EXPORTED header - this is the one included in client programs
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOF1
	#include "oof1.h"
#endif

/**
	\defgroup oofDatabase_ram OOFILE Database backend for simple streamable RAM databases.
	Included in the Open Source OOFILE.
	\ingroup oofDatabase
*/


/**
	Database backend for a single RAM-resident table.
	\see dbConnect_ram
	\ingroup oofDatabase_ram
*/
class OOFILE_EXPORT dbConnect_ram : public dbConnect {
public:
	dbConnect_ram(bool isTemporary=true);	// see dtor for explanation of why default=true
	virtual ~dbConnect_ram();
private:
	dbConnect_ram(const dbConnect_ram&) { assert(0); };  // NEVER CALLED

public:	
	virtual void newConnection(const char* connectionName=0);
	virtual void openConnection(const char* connectionName=0);
	virtual bool openOrCreateConnection(const char* connectionName=0);// just for symmetry with other backends 
	virtual void writeOOFSchema();

#ifdef _Macintosh
	virtual void newConnection(const FSSpec&);
	virtual void openConnection(const FSSpec&);
#endif
	virtual void close();
	
// locking
	virtual void exitLocking() {};
	virtual void enterWriteLocking() {};
	virtual void enterReadLocking() {};

	static OOF_tableBackend *justMakeTableBackend(dbTable*);

protected:
	void SetupConnection(const OOF_String& connectionName);
	virtual OOF_tableBackend *MakeTableBackend(dbTable*);
};
OOFILE_EXPORT ostream& operator<<(ostream& os, dbConnect_ram& connect);
#endif

