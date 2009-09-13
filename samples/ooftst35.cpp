// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST35

// This sample tests the database backend relative access patterns
// similarly to a GUI listbox being scrolled by a user.
//
// Also shows how you override saveRecord;

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"


class	dbNumbers : public dbTable {
OOFILE_METHODS(dbNumbers)
	dbLong	Integer;
	dbLong	UnindexedInt;
	dbChar	IntName;
	
	dbNumbers() :
		Integer("Integer", kIndexed),
		UnindexedInt("UnindexedInt"),
		IntName(9, "Integer Name", kIndexCompressPadding)
	{};
	
	virtual void saveRecord();
	void DisplayTest();
	void AddTestData();
	void Display(unsigned int recsToDisplay);
};


void
dbNumbers::saveRecord()
{
	char str[10];
	sprintf(str, "%4d", Integer.value());
	IntName = str;
	UnindexedInt = Integer;

	dbTable::saveRecord();
}


void
dbNumbers::AddTestData()
{
	cout << "\nAdding test data...\n\n";
	
	for (long i=0; i<1000; i++) {
		newRecord();
		Integer = i;
		saveRecord();
	}
}	// AddTestData


void
dbNumbers::DisplayTest()
{
	start();
	Display(25);	// first 25 as if a listbox
	
	cout << "\nPretend we scrolled down a chunk to record 400 (in order)\n" ;
	gotoRecord(400);
	Display(25);	

	cout << "\nPretend we scrolled back three lines to 397th record\n" ;
	gotoRecord(397);
	Display(5);	

	cout << "\nNow try going to the previous record (from 401)\n" ;
	prev();
	Display(5);	
}	// DisplayTest

	
void 
dbNumbers::Display(unsigned int recsToDisplay)
{
// mimic the listbox abstraction class behaviours
// which typically put an array-like layer on top of the database
// thus refreshing each cell in order becomes a succession of 
// gotoRecords, that know nothing of a wider context

	unsigned long rec = recordNumber();
	for (unsigned int i=0; i<recsToDisplay; i++)  {
		gotoRecord(rec);
		cout << Integer << "   '" <<  IntName << "'" << endl;
		rec++;
	}
}


int main()
{
	cout << "OOFILE Validation Suite - Test 35\n"
		 << "Simple test to store some regular ascending data and retrieve it\n"
		 << "in pattern like a user scrolling a listbox around\n" << endl;

	TEST_CONNECT    theDB;
	dbNumbers    Numbers;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst35:Numbers.dbf";
			const char* kDatabaseName = ":ooftst35:";
		#else
			const char* kExistsName =   "Numbers.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst35.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		dbConnect::raise("Must delete databases before this test for unsorted test to be valid");
	}
	else {
		theDB.newConnection(kDatabaseName);
		Numbers.AddTestData();
	}

	cout << "First few records unsorted - results may vary\n" ;
	Numbers.DisplayTest();
	
	Numbers.setSortOrder(Numbers.Integer);
	cout << "\n\nNow sorted by integer\n" ;
	Numbers.DisplayTest();
	
	Numbers.setSortOrder(Numbers.UnindexedInt);
	cout << "\n\nNow sorted by Unindexed integer (only makes difference on c-tree)\n" ;
	Numbers.DisplayTest();
	
	Numbers.setSortOrder(Numbers.IntName);
	Numbers.selectAll();	//*** work around bug, if have all records selected and sort by non-indexed field
							// fails to correctly change state when sorting by indexed field later
	cout << "\n\nNow sorted by integer name\n" ;
	Numbers.DisplayTest();
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

