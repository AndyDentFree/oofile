// Copyright 1995 A.D. Software. All Rights Reserved

// OOFTST15

// demonstrates opening two databases sequentially

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst01.h"
#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	

int main()
{
	cout << "OOFILE Validation Suite - Test 15\n"
		 << "Simple test to demonstrate opening two databases" << endl
		 << "at different times" << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst15:Patients.dbf";
			const char* kDatabaseName = ":ooftst15:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst15.db";
		const char* kExistsName = kDatabaseName;
	#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
	}
	
	cout << "Dumping the first database, Patients & Visits" << endl << endl 
		 << theDB
		 << endl << endl;
	
	theDB.close();
	
	
// now create the second database
	TEST_CONNECT    theDB2;
	dbPeople     People;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			kExistsName =  "::ooftsx15:People.dbf";
			kDatabaseName = ":ooftsx15:";
		#else
			kExistsName =   "People.dbf";
			kDatabaseName = "";
		#endif	

	#else
		kDatabaseName = "ooftsx15.db";
		kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB2.openConnection(kDatabaseName);
	}
	else {
		theDB2.newConnection(kDatabaseName);
		People.AddTestData();
	}
		
	cout << "Dumping the second database, People" << endl << endl << theDB2;
	
	cout << "Done" << endl;
	
	return EXIT_SUCCESS;
}       

