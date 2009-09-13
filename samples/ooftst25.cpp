// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST25

// This sample tests the database RAM backend by creating a single table
// similarly to a ctree table.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "ooftst01.h"

	DECLARE_CLASS(littleDB)
		dbChar	f1;
		dbLong	f2;
		littleDB() :
			f1(80, "f1"),
			f2("f2")
		{};
	};


int main()
{
	cout << "OOFILE Validation Suite - Test 25\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "in a single-table database, and then use a RAM-based equivalent\n";
	
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
	
// same as ooftst01 up to here	

	dbConnect_ram	tempDB;
	littleDB simpleRAMtable;
	tempDB.newConnection();	// make the database in RAM
	
	simpleRAMtable.newRecord();
	simpleRAMtable.f1 = "George Dent";
	simpleRAMtable.f2 = 99;
	simpleRAMtable.saveRecord();
	simpleRAMtable.newRecord();
	simpleRAMtable.f1 = "Mildred Dent";
	simpleRAMtable.f2 = 100;
	simpleRAMtable.saveRecord();
	
	cout << "Listing simple table\n" << simpleRAMtable << endl;

	cout << "Defining a table in RAM from an existing ctree table\n";
	
	dbPeople* PR = People.cloneEmptyInRAM();
	cout << "Listing empty table in RAM\n" << PR << endl;
	PR->AddTestData();	// Yep, we just call the same method as the c-tree table!
	cout << "Listing populated table in RAM\n" << PR << endl;
	delete PR;

// now create and populate in one hit	
	dbPeople* PR2 = People.cloneInRAM();
	cout << "Listing populated table in RAM\n" << PR2 << endl;
	delete PR2;

	cout << "Test Completed" << endl;
		
	return EXIT_SUCCESS;
} 

