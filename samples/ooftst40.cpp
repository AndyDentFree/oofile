// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST40

// this sample uses generated data to create a reasonable sized database
// for testing with c-tree to compare indexed vs non-indexed
// recordNumber(query) 

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"


class dbTest : public dbTable {
	OOFILE_METHODS(dbTest)
	dbChar		IndexedChar;
	dbChar		PlainChar;
	dbUlong		IndexedLong;
	dbUlong		PlainLong;
	
	dbTest() :
		IndexedChar(60, "IndexedChar", kIndexCompress),
		PlainChar(60, "Plain Char"),
		IndexedLong("Indexed Long", kIndexNoDups),
		PlainLong("Plain Long")
	{};

};


dbConnect_ctree theDB;
dbTest	Test;

int main()
{

	cout << "OOFILE Validation Suite - Test 40\n"
		 << "Simple test to demonstrate recordNumber(query) on various selections" << endl
		 << "using the inbuilt test data generator to generate a decent size DB" << endl << endl;
		 	
	
	if (dbConnect::fileExists("ooftst40.db")) {
		theDB.openConnection("ooftst40.db");
	}
	else {
		theDB.newConnection("ooftst40.db");
		Test.generateTestData(10000, false);	// 10,000 non-random
	}
//	cout << theDB << endl;

	unsigned long recNo;
//	Test.search(Test.IndexedLong==211);
//	cout << Test << endl;
	
	recNo = Test.recordNumber(Test.IndexedChar>="two-one-one");
	cout << "tried to get rec 21: recNo = "	<< recNo << endl << endl;
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

