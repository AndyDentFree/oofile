// OOFILE RAM backend private headers
// 
// COPYRIGHT 1996 A.D. Software, All rights reserved
#ifndef H_OOFRAMPX
#define H_OOFRAMPX

#ifndef H_OOFramp
	#include "ooframp.h"
#endif
#ifndef H_OOFRAMX
	#include "ooframx.h"
#endif
#ifndef H_OOFRECS
	#include "oofrecs.h"
#endif

/**
	Database backend for a single RAM-resident table.
	\see dbConnect_ramp
	\ingroup oofDatabase_ramp
*/
class OOF_rampBackend : public OOF_ramBackend
{
protected:
	OOF_rampBackend(dbTable*, dbConnect_ramp*);
	OOF_rampBackend(const OOF_rampBackend&, dbTable*, dbTable::selSharingT);
	virtual ~OOF_rampBackend();
	
 public:
	virtual bool openTableInConnection(const dbConnect*,const bool createIfMissing = false);
	virtual OOF_tableBackend* clone(dbTable::selSharingT, dbTable*) const;

	virtual void close();
	virtual void save();

private:
	void ReadRecordFromFile(FILE*);
	void WriteRecordToFile(FILE*);
	
// data storage
	dbConnect_ramp*	mConnection;	// for I/O
	friend OOF_tableBackend *dbConnect_ramp::MakeTableBackend(dbTable*);
};

#endif


