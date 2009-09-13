// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST36

// This sample tests the database backend handling of duplicate records

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

// global variables that define the database using the ooftst02 classes

	class dbNoBlob : public dbTable {
		OOFILE_METHODS(dbNoBlob)
		dbChar	Title;		
		dbNoBlob() :
			Title(80, "Title", kIndexNoDups)
		{};
	};


	class dbHasBlob : public dbNoBlob {
		OOFILE_METHODS(dbHasBlob)
		dbText		Notes;			
	};

	TEST_CONNECT    theDB;
	dbNoBlob     	noBlob;
	dbHasBlob		hasBlob;
	
int main()
{
	cout << "OOFILE Validation Suite - Test 36\n"
		 << "Test ways to cope with duplicate records" << endl << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst36:noBlob.dbf";
			const char* kDatabaseName = ":ooftst36:";
		#else
			const char* kExistsName =   "noBlob.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst36.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		noBlob.deleteAll();
		hasBlob.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	noBlob.newRecord();
	noBlob.Title = "George";
	noBlob.saveRecord();

// try a duplicate	
	noBlob.ignoreDuplicateRecords();
	noBlob.newRecord();
	noBlob.Title = "George";
	noBlob.saveRecord();	// will not be added
	noBlob.noticeDuplicateRecords();

// try another duplicate	
	{
		stIgnoreDuplicateRecords blobDups(noBlob);	// use stack class for same effect
		noBlob.newRecord();
		noBlob.Title = "George";
		noBlob.saveRecord();	// will not be added
	}
		
	hasBlob.newRecord();
	hasBlob.Title = "Dent";
	hasBlob.Notes = "This is the first record with a blob";
	hasBlob.saveRecord();

// try a duplicate	
	hasBlob.ignoreDuplicateRecords();
	hasBlob.newRecord();
	hasBlob.Title = "Dent";
	hasBlob.Notes = "Try to save a duplicate record with a blob";
	hasBlob.saveRecord();	// will not be added
	hasBlob.noticeDuplicateRecords();

	cout << theDB;
	

// try duplicate detection
	hasBlob.newRecord();
	hasBlob.Title = "Dent";
	if (hasBlob.Title.valueIsDuplicate())
		cout << "It works! - 'Dent' is a duplicate";
	else
		cout << "Duplicate checking doesn't work without indexes";
	cout << endl << endl;
		
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

