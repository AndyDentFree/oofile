#include "ooftest.h"
#include "oofios.h"
// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST23

// this sample creates a file then adds indices

// Simple stream I/O is used to interact with the user.

DECLARE_CLASS(dbTest23)
	dbChar		f1;
	dbUlong		f2;
	dbReal		f3;  
	
	dbTest23() : f1(20, "F1", kIndexCompress),
				f2("F2", kIndexNoDups),
				f3("F3")
{};
	};


// show a common technique, using our own class to contain all the database
class test23DB : public dbConnect_ctree
{
public:
	test23DB();
	
	void	addTestData(unsigned long startNum, unsigned long endNum);
	void	displaySubset();

// tables
	dbTest23     	 mTable;

};

test23DB::test23DB()
{
	useSeparateFiles();
	useExclusiveAccess();	// force this despite filesharing library (FPUTFGET option)
}

void
test23DB::addTestData(unsigned long startNum, unsigned long endNum)
{
	const char* names[] = {
		"George",
		"Fred",
		"Sally",
		"Zach",
		"William",
		"Sue",
		"Pamela",
		"Annalise",
		"Jane",
		"Frederick"
	};
	const unsigned short kNumNames = 10;  // sizeof(names);
	for (unsigned long i=startNum; i<endNum; i++) {
		mTable.newRecord();
		const unsigned short iName = i%kNumNames;
		mTable.f1 = names[iName];	// one of 10 names
		mTable.f2 = i;
		mTable.f3 = i+0.5;
		mTable.saveRecord();
	}
}

void
test23DB::displaySubset()
{
	mTable.search(mTable.f1=="George");
	cout << mTable << endl << endl;
}

int main()
{
	cout << "OOFILE Validation Suite - Test 23\n"
		 << "This tests the effect of adding indices to a database\n\n";
	
	test23DB  theDB;

	if (dbConnect::fileExists("dbtest23.dat")) {
		theDB.openConnection("");
		theDB.deleteAll();
	}
	else {
		cout << "Creating a new database\n";
		theDB.newConnection("");
	}

	test23DB  aDB;

	if (dbConnect::fileExists(":adb:dbtest23.dat")) {
		aDB.openConnection("adb");
		aDB.deleteAll();
	}
	else {
		cout << "Creating a new database\n";
		aDB.newConnection("adb");
	}

	cout << "Adding 500 test records with indexing active\n";
	theDB.addTestData(0, 51);
	theDB.displaySubset();

	theDB.mTable.suppressIndices();	
	cout << "Adding 500 test records with indexing disabled\n";
	theDB.addTestData(51, 101);
	
	aDB.mTable.newRecord();
	aDB.mTable.f1 = "testing";
	aDB.mTable.saveRecord();

	//theDB.displaySubset();
	theDB.mTable.rebuild();

	cout << "Displaying records after rebuild\n";	
	theDB.displaySubset();

	cout << endl <<"Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

