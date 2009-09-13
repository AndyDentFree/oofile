// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST18

// This sample tests the wildcard searches.

// Simple stream I/O is used to interact with the user.

// NOTE the odd sizes in the fields below are to help trap alignment issues

#include "ooftest.h"
#include "ooftst01.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 18\n"
		 << "Test of wildcard searching\n";
	
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

#ifdef TESTING_RAM 	

if (dbConnect::fileExists(kExistsName)){
		theDB.openConnection(kDatabaseName);
		People.AddTestData();
	}
	else {
		theDB.newConnection(kDatabaseName);
		People.AddTestData();
	}

#else
	if (dbConnect::fileExists(kExistsName))
		theDB.openConnection(kDatabaseName);
	else {
		theDB.newConnection(kDatabaseName);
		People.AddTestData();
	}
#endif
	dbView justNames(People);
	justNames << People.LastName << People.OtherNames;
	
	People.setSortOrder(People.LastName);

	People.search(People.LastName=="*");
	cout << endl << "Testing pathological case '*' - should be all records" << endl 
		 << justNames << endl;
	
	cout << "Retrieving T*r: " << People[People.LastName=="T*r"].LastName << endl;

	People.search(People.LastName=="D??t");
	cout << "Listing two D??t records: " << endl << justNames << endl;

	People.search(People.LastName=="?mith");
	cout << "Listing leading '?' with ?mith: " << endl << justNames << endl;

	People.search(People.LastName=="De*");
	cout << "Listing trailing single '*' with De*: " << endl << justNames << endl;

	People.search(People.LastName!="De*");
	cout << "Testing not-equals wildcard with !De*: " << endl << justNames << endl;

	People.search(People.LastName.startsWith("D*n"));
	cout << "Listing startsWith D*n: " << endl << justNames << endl;

	People.search(People.LastName.startsWith("D*n*"));
	cout << "Listing startsWith D*n*: " << endl << justNames << endl;

	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

