// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST37

// This sample tests copying from one database to another.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes
// also shows the kind of wrapping into an overall class that is commonly used
// to have a document class to pass around a GUI interface

	class myDB {
	public:
		TEST_CONNECT    theDB;
		dbPatients    	Patients;
		dbVisits		Visits;

		myDB();
	};
	
	myDB::myDB()
	{
	// declarative variable only - don't need to give it a name
		dbRelationship(Patients.Visits, Visits.Patient);
	}

	myDB firstDB, secondDB;
		
int main()
{
	cout << "OOFILE Validation Suite - Test 37\n"
		 << "Simple test to store some data and copy to another DB" << endl << endl;

// CREATE OR OPEN THE FIRST DATABASE
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst37:Patients.dbf";
			const char* kDatabaseName = ":ooftst37:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst37.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		firstDB.theDB.openConnection(kDatabaseName);
	}
	else {
		firstDB.theDB.newConnection(kDatabaseName);
		firstDB.Patients.AddTestData();
	}

// CREATE OR OPEN THE SECOND DATABASE
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName2 =  ":ooftsx37:Patients.dbf";
			const char* kDatabaseName2 = ":ooftsx37:";
		#else
			const char* kExistsName2 =   "Patients.dbf";
			const char* kDatabaseName2 = "";
		#endif	

	#else
		const char* kDatabaseName2 = "ooftsx37.db";
		const char* kExistsName2 = kDatabaseName2;
	#endif
	
	if (dbConnect::fileExists(kExistsName2)) {
		secondDB.theDB.openConnection(kDatabaseName2);
		secondDB.Patients.deleteAll();
	}
	else {
		secondDB.theDB.newConnection(kDatabaseName2);
	}

	cout << "Second DB is currently empty\n" << secondDB.theDB;
	
// copy from first to second
	secondDB.theDB.copyAllFrom(&firstDB.theDB);		

	cout << "Second DB is now a copy of first\n" << secondDB.theDB;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

