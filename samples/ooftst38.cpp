#include "ooftest.h"
// Copyright 1997 A.D. Software. All Rights Reserved

// OOFTEST38

// this sample tests the use of indices which ignore nulls
// it also shows a trick to use a compound index to get
// effectively multiple indexes on the same field
// with different index settings

// Simple stream I/O is used to interact with the user.

DECLARE_CLASS(dbTest38)
	dbChar			name;
	dbUlong			f2;
	dbReal			f3; 
	dbCompoundField	secondName; 
	
	dbTest38() : 
		name(20, "name", kIndexCompress),
		f2("F2", kIndexNoDupsNoNulls),
		f3("F3"),
		secondName("second name", kIndexCompressNoNulls)
{
	secondName << name;	// not really a compound index, just a different kind of index
};
	};


// show a common technique, using our own class to contain all the database
class test38DB : public dbConnect_ctree
{
public:
	void	addTestData(unsigned long startNum, unsigned long endNum);
	void	displaySorted(dbField& sortField);

// tables
	dbTest38     	 mTable;

};

void
test38DB::addTestData(unsigned long startNum, unsigned long endNum)
{
	const char* names[] = {
		"George",
		"Fred",
		"Sally",
		"Zach",
		""  // null entries will not be indexed by secondName
	};
	const unsigned short kNumNames =  sizeof(names)/sizeof(const char*);
	for (unsigned long i=startNum; i<endNum; i++) {
		mTable.newRecord();
		const unsigned short iName = i%kNumNames;
		mTable.name = names[iName];	// one of 10 names
		mTable.f2 = i;
		mTable.f3 = i+0.5;
		mTable.saveRecord();
	}
	mTable.selectAll();  // if this is not performed, we don't use the index to iterate
				  // and hence the secondName and f2 displaySorted will still show data
}

void
test38DB::displaySorted(dbField& sortField)
{
	mTable.setSortOrder(sortField);
	cout 
		<< "Sorted by " 
		<< sortField.fieldName() 
		<< " has " << mTable.count() << " records\n"
		<< mTable
		<< endl << endl;
}


int main()
{
	cout << "OOFILE Validation Suite - Test 38\n"
		 << "This tests the effect of adding indices to a database\n\n";
	
	test38DB  theDB;

	const char* kDatabaseName = "ooftst38.db";
	if (dbConnect::fileExists(kDatabaseName)) {
		theDB.openConnection(kDatabaseName);
		theDB.deleteAll();
	}
	else {
		cout << "Creating a new database\n";
		theDB.newConnection(kDatabaseName);
	}

	test38DB  aDB;

	cout << "Adding 5 test records\n";
	theDB.addTestData(0, 5);
	theDB.displaySorted(aDB.mTable.name);
	theDB.displaySorted(aDB.mTable.secondName); // shouldn't see any records due to noNull index
	theDB.displaySorted(aDB.mTable.f2);
	theDB.displaySorted(aDB.mTable.f3);

	cout << endl <<"Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

