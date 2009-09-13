// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST39

// This sample tests the database backend behaviour after deleting a record.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "oofios.h"	// an easy way to get your stream libraries, regardless of platform
#include "ooftst01.h"


int main()
{
	cout << "OOFILE Validation Suite - Test 39\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "after deleting just one record\n"
		 << "in a single-table database, with no relations\n" << endl;

	TEST_CONNECT    theDB;
	dbPeople     People;

// bit of complicated filename logic to support different backends
// with this one test program

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

// open or create the database	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
		People.AddTestData();
		People.deleteAll();
	}
// Always want to add records after deleting some, for this test
// but leave at least one record deleted, from the original 4	
	People.Add("Smith", "John", 0);  // specifically to test zero searches
	People.Add("Dent", "Mildred", 5000);
	People.Add("Dent", "George", 25000);

	dbView justNames(People, false);
	justNames << People.LastName << People.OtherNames;
// dump stuff like a GUI retrieval
	People.gotoRecord(0);
	cout << "Record 0 is: " << People.LastName << ", " << People.OtherNames << endl;
	People.gotoRecord(1);
	cout << "Record 1 is: " << People.LastName << ", " << People.OtherNames << endl << endl;

	People.setSortOrder(People.OtherNames);
	People.gotoRecord(0);
	cout << "Record 0 is: " << People.LastName << ", " << People.OtherNames << endl;
	People.gotoRecord(1);
	cout << "Record 1 is: " << People.LastName << ", " << People.OtherNames << endl << endl;

//	cout << "Listing records in OtherNames order\n" << justNames << endl;

	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

