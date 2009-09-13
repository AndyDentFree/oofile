// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST24

// This sample tests opening two databases at once.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "ooftst01.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 24\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "in 2 simultaneously open databases.\n";
	
	TEST_CONNECT    theDB;
	dbPeople     People;

#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();

	#ifdef _Macintosh
// path handling only on the Mac at present
		const char* kExistsName =  ":ooftst01:People.dat";
		const char* kDatabaseName = ":ooftst01:";
	#else
		const char* kExistsName =   "People.dat";
		const char* kDatabaseName = "";
	#endif	

#else
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst01:People.dbf";
			const char* kDatabaseName = ":ooftst01:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		// Persistent RAM, no option of single file
		#ifdef _Macintosh
			const char* kDatabaseName = ":ooftst01:ooftst01.db";
		#else
			const char* kDatabaseName = "ooftst01.db";
		#endif	
		const char* kExistsName = kDatabaseName;
	#endif
#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	People.AddTestData();
	People.setSortOrder(People.LastName);
	cout << "Listing records\n" << People << endl;



	TEST_CONNECT    theDB2;	// won't use separate files
	dbPeople     People2;
	People2.setName("People2");

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			kExistsName =  ":ooftst24:People2.dbf";
			kDatabaseName = ":ooftst24:";
		#else
			kExistsName =   "People2.dbf";
			kDatabaseName = "";
		#endif	

	#else
		kDatabaseName = "ooftst24.db";
		kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		cout << "Opening second database\n";
		theDB2.openConnection(kDatabaseName);
		People2.deleteAll();
	}
	else {
		cout << "Creating second database\n";
		theDB2.newConnection(kDatabaseName);
	}
	People2.AddTestData();
	People.setSortOrder(People.OtherNames);
	cout << "Listing records in 2nd database\n" << People2 << endl;

	cout << "Proving 1st database is still active\n" << People << endl;

	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

