// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST58

// This sample tests the database III backend by creating a table with blobs
// used to test blob handling
// This test should be run twice to verify that it can read an existing dbIII data base
// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst58.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	
	dbPatients     Patients;
int main()
{
	theDB.useDbase3Memos();

	cout << "OOFILE Validation Suite - Test 58\n"
		 << "Simple test to store some data and retrieve it" << endl
		 << "To test db III blob handling" << endl;

	/*#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst58:Patients.dbf";
			const char* kDatabaseName = ":ooftst58:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst58.db";
		const char* kExistsName = kDatabaseName;
	#endif
*/	
	#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();

	#ifdef _Macintosh
// path handling only on the Mac at present
		const char* kExistsName =  ":ooftst58:Patients.dat";
		const char* kDatabaseName = ":ooftst58:";
	#else
		const char* kDatabaseName = "\\TestDir\\TestSubDir\\Patients.dat";
		const char* kExistsName =   kDatabaseName;
	#endif	

#else
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst58:Patients.dbf";
			const char* kDatabaseName = ":ooftst58:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		// Persistent RAM, no option of single file
		#ifdef _Macintosh
			const char* kDatabaseName = ":ooftst58:ooftst58.db";
		#else
			const char* kDatabaseName = "ooftst58.db";
		#endif	
		const char* kExistsName = kDatabaseName;
	#endif
#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << " just loaded and now deleting: " << endl;
		cout << theDB;
	
		Patients.deleteAll();
		Patients.AddTestData();
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
	}

	Patients.setSortOrder(Patients.LastName);
	cout << theDB;

	Patients.search(Patients.LastName == "Smith");
	Patients.MemoF = "spot";
	Patients.saveRecord();
	
	cout << " The data base with Smith memo edit (smaller string)" << endl;
	
	cout << Patients << endl;//991004
	Patients.search(Patients.LastName == "Smith");
	Patients.MemoF = kHugeString;
	Patients.saveRecord();
	
	//cout << "Finding Patient No 3 : " << Patients << endl;
	cout << " The data base with Smith memo edit (huge string)" << endl;
		cout << Patients << endl;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

