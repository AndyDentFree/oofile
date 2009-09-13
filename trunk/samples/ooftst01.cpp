// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST1

// This sample tests the database backend by creating a single table
// and storing and retrieving indexed data.

// Simple stream I/O is used to interact with the user.

// NOTE the odd sizes in the fields below are to help trap alignment issues

#include "ooftest.h"
#include "oofios.h"	// an easy way to get your stream libraries, regardless of platform
#include "ooftst01.h"


int main()
{
	cout << "OOFILE Validation Suite - Test 1\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "in a single-table database, with no relations\n" << endl;

	TEST_CONNECT    theDB;
	dbPeople     People;

// bit of complicated filename logic to support different backends
// with this one test program

#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();

	#ifdef _Macintosh
		const char* kExistsName =  ":ooftst01:People.dat";
		const char* kDatabaseName = ":ooftst01:";
	#else
		const char* kDatabaseName = "\\TestDir\\TestSubDir\\";
		const char* kExistsName =   "\\TestDir\\TestSubDir\\People.dat";
	#endif	

#else
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst01:People.dbf";
			const char* kDatabaseName = ":ooftst01:";
		#else
			const char* kDatabaseName = "\\TestDir\\TestSubDir\\";
			const char* kExistsName =   "\\TestDir\\TestSubDir\\People.dbf";
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

// open or create the database	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Just opened...\n" << theDB << endl << endl;
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	unsigned long x = People.sequenceNumber();
	cout << "Seq: " << x << endl;
	
	People.AddTestData();

//	People.setSortOrder(People.LastName);
	cout << "Listing records\n" << People;
// NOTE we dumped all the fields of People this time, from now on we will
// use dbViews to restrict the fields listed

		
	dbView justNames(People);
	justNames << People.LastName << People.OtherNames;
	
	People.setSortOrder(People.OtherNames);
	cout << "Listing records in OtherNames order\n" << justNames << endl;

	cout << "Now retrieving by index\n";
	cout << "Retrieving Taylor: " << People[People.LastName=="Taylor"].LastName << endl;

	cout << "Retrieving Smith by Salary: ";
	People[People.Salary==0L];
	cout << People.LastName << endl << endl;

	People.search(People.LastName=="Dent");
	cout << "Listing two Dent records: " << endl << justNames << endl;

	cout << endl << "now producing a subset of Taylor & Dent" << endl;
	dbPeople entrepeneurs = People;	
	entrepeneurs.search(entrepeneurs.LastName=="Taylor");
	entrepeneurs += People;
	cout << (dbView(entrepeneurs) << entrepeneurs.LastName << entrepeneurs.OtherNames) << endl;

	dbPeople savedEnts = entrepeneurs;
	cout << "Just cloned a selection of " << savedEnts.count() << " records" << endl;
	
	cout << endl << "now reducing original via Intersection to Dent" << endl;
	entrepeneurs &= People;      // Dent, Taylor & Dent
	cout << entrepeneurs;

	cout << endl << "now Inverting to Smith & Taylor" << endl;
	~entrepeneurs;
	cout << (dbView(entrepeneurs) << entrepeneurs.LastName << entrepeneurs.OtherNames) << endl;   
	// Dent inverted

	cout << "Show the cloned selection is still Dent & Taylor" << endl
	<< (dbView(savedEnts) << savedEnts.LastName << savedEnts.OtherNames) << endl;
	
	cout << endl << "now reducing via Difference to Dent" << endl;
	savedEnts -= entrepeneurs;  // Dent, Taylor - Smith, Taylor
	cout << (dbView(savedEnts) << savedEnts.LastName << savedEnts.OtherNames) << endl;
	
// demonstrate going to a specific relative record in a selection
// then going to the previous record, as would be common in a GUI
// by double-clicking a line in a browser, then pressing Prev Record
	People.selectAll();
	People.setSortOrder(People.LastName);
	cout << "Retrieving Taylor by relative record number: ";
	People.gotoRecord(3);

	cout << People.LastName << endl << endl;
	cout << "Retrieving previous record Smith: ";
	People.prev();
	cout << People.LastName << endl << endl;
	
	cout << "Now finding people who've been paid in the last 2 weeks " << endl;
	People.search(People.LastPaid > dbDate::currentDate()-14L);
	cout << (dbView(People) << People.LastName << People.OtherNames << People.LastPaid) << endl;

	cout << endl << "Test the fast bug dangerous union_with_no_overlap" << endl;
	People.search(People.LastName=="Dent");
	dbPeople overlapTester = People;	
	overlapTester.search(overlapTester.LastName=="Taylor");
	overlapTester.union_with_no_overlap(People);
	cout << (dbView(overlapTester) << overlapTester.LastName << overlapTester.OtherNames) << endl;
	// now break it
	cout << endl << "Show side effects of misuse of union_with_no_overlap - selection with repeats" << endl;
	overlapTester.union_with_no_overlap(People);
	cout << (dbView(overlapTester) << overlapTester.LastName << overlapTester.OtherNames) << endl;

//*******************************
// demonstrate the value() call which is mainly for use in the lhs of if() statements
// instead of cast operators

// Find people with the same salary as the first entrepeneur
// (OK, this is a silly way to do it - a search would be faster, but it's an example!)

// NOTE: the value() on the rhs of the if() is redundant as the 'operator long()'
// would be invoked automatically by the context, but it looks nice and symmetrical

	entrepeneurs.start();
	People.selectAll();
	for (People.start(); People.more(); People.next()) {
		if (People.Salary.value() == entrepeneurs.Salary.value())	//******
			cout << "Matching Salaries of : " 
				 << People.Salary << " for "
				 << People.LastName << ", "
				 << entrepeneurs.LastName
				 << endl << endl;
	}
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

