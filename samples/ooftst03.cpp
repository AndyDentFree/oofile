// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST3

// this sample tries a number of random updates to test m/u

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	dbConnect_ctree    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	

#ifdef _Windows
#include <windows.h>
void yieldTime();
void yieldTime() {
	MSG msg;
	PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
}
#endif

int main()
{
	cout << "OOFILE Validation Suite - Test 3\n"
		 << "Simple random updates testing multi-user" << endl
		 << "using a copy of the database from ooftest2" << endl;

// get probably remote file path
	char remotePath[255];
	cout << "Enter full file path, probably a remote disk: " << endl 
	     << "(eg: Mac Remote Disk:oofile:ooftst03.db  OR  E:\\OOFILE\\OOFTEST3.DB)" << endl;
	cin.getline(remotePath, 255);
	
	cout << endl << "Are we running as a Writer or Reader (R/W)? ";
	char writeORread;
	cin >> writeORread;
	bool asWriter = ( (writeORread=='W') || (writeORread=='w') );

//	if (dbConnect::fileExists("ooftest3.db"))
// JUST ASSUME IT DOES - this function doesn't work if file already open!
		theDB.openConnection(remotePath);
//	else {
//		dbConnect::raise("Please copy ooftest2.db (from running ooftest2) to ooftest3.db");
//	}
	
	if (asWriter) {
		cout << "Repeatedly updating the first record..." << endl;
		unsigned long numRecs = Patients.count();
		for (unsigned long i=0; i<100000 ; i++) {
			unsigned long theRec = rand()%numRecs;
			
	// if turning on locking, uncomment here and at end of this block
	// but remember 1 writer/many readers doesn't require you to do anything
			theDB.enterWriteLocking();
			Patients.start(); // comment out this line if you want random updates
// IF YOU WANT RANDOM					Patients[theRec];	// jump to a random record
			cout << "Writing rec: " << theRec << '\t' << Patients.LastName << " " << Patients.Salary;
			Patients.Salary = Patients.Salary + 1;
			Patients.saveRecord();	
			Patients.unloadRecord();  // force read from disk
			cout << " ... " << Patients.Salary << endl;
			theDB.exitLocking();
#ifdef _Windows
			yieldTime();
#endif
		}
	}
	else {
		cout << "Randomly reading records..." << endl;
		unsigned long numRecs = Patients.count();
		for (unsigned long i=0; i<100000 ; i++) {
			unsigned long theRec = rand()%numRecs;
			
			Patients[theRec];	// jump to a random record
			cout << "rec: " << theRec << '\t' << Patients.LastName << " " << Patients.Salary << endl;
#ifdef _Windows
			yieldTime();
#endif
		}
	}

	cout << "Database after the random updates" << endl << Patients << endl;
	cout << "done" << endl;

	return EXIT_SUCCESS;
}

