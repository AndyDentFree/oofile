// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST32

// Simple stream I/O is used to interact with the user.
// to demonstrate use of bools in a database

#include "ooftest.h"

DECLARE_CLASS(dbBoolTest)
        dbUlong			Number;
        dbBool	        IsEven;
        dbBool          IsOdd;

        dbBoolTest() : dbTable("booltest") ,
                                Number("Number"),
                                IsEven("IsEven"),
                                IsOdd("IsOdd")
        {
                Number.index(kIndexNoDups);
                IsEven.index(kIndexed);
        };
        
        void addTestData();
        
        virtual void saveRecord();
};

void
dbBoolTest::saveRecord()
{
	IsEven = ((Number%2) == 0);
	IsOdd = !IsEven;
	dbTable::saveRecord();
}


void
dbBoolTest::addTestData()
{
	const unsigned long numRecords = 200;
	cout << "Adding " << numRecords << " test records\n";
	for (unsigned long i=0; i<numRecords; i++) {
		newRecord();
		Number = i;
		saveRecord();
	}
}


int 
main()
{
	cout << "OOFILE Validation Suite - Test 32\n"
		 << "Store bools in database"
		 << endl << endl;

	TEST_CONNECT	theDB;
	dbBoolTest		BoolTest;	

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst32:booltest.dbf";
			const char* kDatabaseName = ":ooftst32:";
		#else
			const char* kExistsName =   "booltest.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst32.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Deleting old test data\n";
		BoolTest.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	BoolTest.addTestData();
	
	cout << "Searching indexed True IsEven\n";
	BoolTest.search(BoolTest.IsEven);
	cout << BoolTest << endl;
	
	cout << "Searching nonindexed True IsOdd\n";
	BoolTest.search(BoolTest.IsOdd == true);
	cout << BoolTest << endl;
	
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

