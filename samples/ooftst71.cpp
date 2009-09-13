// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST71
// This sample tests the functioning of the fast assignment operator<<

// Simple stream I/O is used to interact with the user.

/**
	Helper class for dbTable to easily assign field values;
	Provides idiom of 
	People << "Andy" << "Dent" << "37";
	instead of
	People.newRecord();
	People.FirstName = "Andy";
	People.LastName = "Dent";
	People.Age = 37;
	People.saveRecord();
*/
#include "ooftest.h"
#include "oofios.h"	// an easy way to get your stream libraries, regardless of platform
#include "ooftst01.h"


int main()
{
	TEST_CONNECT    theDB;
	dbPeople     People;

	cout << "OOFILE Validation Suite - Test 71\n"
		 << "simple test to entering data into tables with <<" << endl;

		
// bit of complicated filename logic to support different backends
// with this one test program

#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();
	const char* kDatabaseName =  "../ooftst71/People.dat";
#else
	#ifdef TESTING_DBASE
		const char* kDatabaseName =  "../ooftst71/People.dbf";
	#else
		// Persistent RAM, no option of single file
		const char* kDatabaseName = "../ooftst71/ooftst71.db";
		const char* kExistsName = kDatabaseName;
	#endif
#endif
	if(theDB.openOrCreateConnection(kDatabaseName)) {
		cout << "Just opened...\n" << theDB << endl << endl;
		People.deleteAll();
	}
	else { 
		cout << "Just created...\n" << theDB << endl << endl;
	}
		
	People << "Andy" << "Dent" << "37";
	People << "Rosalie" << "Miles";
	People << "Tanith" << "Dent" << "11";
	People << "Ryan" << "Dent" << "8";
		

	People.setSortOrder(People.LastName);
	cout << "Listing records\n" << People;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

