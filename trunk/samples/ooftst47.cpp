// Copyright 1999 A.D. Software. All Rights Reserved

// OOFTEST47

// This sample tests the ability to buffer ALL changes to a database table
// as used in the report-writer to hide local editing in the preview window

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "oofios.h"	// an easy way to get your stream libraries, regardless of platform
#include "ooftst01.h"


int main()
{
	cout << "OOFILE Validation Suite - Test 47\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "while buffering access, so real database not touched.\n" << endl;

	TEST_CONNECT    theDB;
	dbPeople     People;

// bit of complicated filename logic to support different backends
// with this one test program

#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();

	#ifdef _Macintosh
// path handling only on the Mac at present
		const char* kExistsName =  ":ooftst01:People.dat";
		const char* kDatabaseName = ":ooftst01:";
	#else
		const char* kDatabaseName = "\\TestDir\\TestSubDir\\People.dat";
		const char* kExistsName =   kDatabaseName;
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

// open or create the database	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Just opened...\n" << theDB << endl << endl;
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	People.AddTestData();

	dbView justNames(People);  // clones its own iterator so will see original data
	// but has a shared selection so will be affected by the searches below
	justNames << People.LastName << People.OtherNames;
	
	People.setSaveOption(dbTable::bufferForever);
	People.setSortOrder(People.OtherNames);
	cout << "Retrieving Smith by Salary: ";
	People[People.Salary==0L];
	cout << People.LastName << endl << endl;
	
	cout << "Modifying Smith salary to 2000  - should locally buffer" << endl;
	People.Salary = 2000;
	
	
	cout << People << endl << endl;
	
	People.sortNow();// david see if still get buffered content when sort 991217
	cout << "just forced a sort" << endl; 
	cout << People << endl << endl;
	
	
	
	People.search(People.LastName=="Dent");
	
	People.LastName = "Wisdom";	
	cout 
		<< "Listing two Dent records, from view showing untouched data: " << endl 
		<< justNames << endl << endl;	
	cout 	<< "People after modifying one of the Dent names, showing local cache:" << endl 
		<< People << endl;
	
	People.search(People.LastName=="Dent");
	
	cout 
		<< "Search again - local cache used instead of index so only 1 found:" << endl 
		<< People << endl;
	
	//david extra changes of state 991217
		cout 
		<< "dump the whole of people:" << endl; 
			People.selectAll();//david debug
	
		People.search(People.LastName=="splat");//david debug 000127 empty selection
	
		People.selectAll();
		cout << People << endl;
	// end advid
	People.search(People.LastName=="Dent");
	
	cout 
		<< "Search again - local cache used instead of index so only 1 found:" << endl 
		<< People << endl;
	
	
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 
