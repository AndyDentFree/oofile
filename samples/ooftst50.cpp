// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST50

// This sample tests the database backends for all current backends which are
// persistent and can create temporary files by creating a pair of tables
// which should be deleted at exit.

// The basic definition that it worked is that
// you should see no ooftst50 files left after completion!

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"
#include "oofexcep.h"

// global variables that define the database using the ooftst02 classes

class CTest50 {
public:
	CTest50(dbConnect* adoptedConnection);
	~CTest50();
	
// public data members
	dbConnect*	mDB;  // owned, adopted
	dbPatients*     Patients;  // owned, auto-deleted
	dbVisits*	Visits;  // owned, auto-deleted
		
};


CTest50::CTest50(dbConnect* adoptedConnection) :
	mDB(adoptedConnection)
{
	assert(adoptedConnection && adoptedConnection==dbConnect::currentlyConstructing());
// this assertion guarantees the incoming connection has not yet had
// open/newConnection called

// create new table objects which will automatically attach themselves
// to the most recently created dbConnect, which was hopefully just passed in
	Patients = new dbPatients;
	Visits = new dbVisits;
	dbRelationship PatientVisits(Patients->Visits, Visits->Patient);
	
	mDB->newConnection("ooftst50");
	Patients->AddTestData();
	
	cout << endl << "dumping newly created database" << endl;
	mDB->dumpData(cout);
}


CTest50::~CTest50()
{
	delete mDB;  // will delete all the tables attached to it
}

	
int main()
{
	cout << "OOFILE Validation Suite - Test 50\n"
		 << "Test temporary databases in a range of backends" << endl << endl;

	try {
	cout << "testing persistent RAM" << endl;
		CTest50(new dbConnect_ramp(true  /* is temporary*/));
	
	// comment out the following if you only have the free OOFILE
		cout << "testing dbase" << endl;
		CTest50(new dbConnect_dbase(true) );
		
	// comment out the following if you only have Personal OOFILE
		cout << "testing ctree with a single file" << endl;
		CTest50(new dbConnect_ctree(true) );
		
		cout << "testing ctree with many files" << endl;
		dbConnect_ctree* orphanedDB = new dbConnect_ctree(true);
		orphanedDB->useSeparateFiles();
	
		CTest50* anotherTemp = new CTest50(orphanedDB);  
		delete anotherTemp;  // use new/delete to show that it's not just stack objects
		// will cleanup properly
	}
	catch (const oofE_Base& theErr)
	{
		cout << theErr;
	}
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       