

// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST42

// This sample tests the database backend by creating a pair of tables
// and storing and retrieving indexed data via traversal paths.

// it shows also the use of an overall database class corresponding to a 
// Document, and how we typically pass around a pointer to that database
// class rather than using global variables as in earlier samples.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst42.h"

void testRelatedSearches(CDatabase42*);
	
int main()
{
	cout << "OOFILE Validation Suite - Test 42\n"
		 << "Simple test to store some data and retrieve it" << endl
		 << "using a MN relation joining over a field" << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst42:People.dbf";
			const char* kDatabaseName = ":ooftst42:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = ""; 
		#endif	

	#else
		const char* kDatabaseName = "ooftst42.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	CDatabase42* DB = new CDatabase42;  // this would normally be a member of an app framework's Document
	DB->openOrCreateDatabase(kExistsName, kDatabaseName);
	DB->dumpDatabase();
	testRelatedSearches(DB);
	cout << "Test Completed" << endl;
	delete DB;
	
	return EXIT_SUCCESS;
}       


void
testRelatedSearches(CDatabase42* DB)
{
// NOTE the following more intuitive form doesn't currently work
// so you have to go via the MN link table as shown after
//	DB->mPeople->search (DB->mPeople->buysFrom->Name == "Acme Ltd");
	DB->mPeople->search (DB->mPeople->PLink->Company->Name == "Acme Ltd");
	cout << "People related to Acme Ltd" << endl;
	DB->mPeople->displayWithCompanies(cout);

	cout << endl << endl << "All people and related companies" << endl;
	DB->mPeople->selectAll();
	DB->mPeople->displayWithCompanies(cout);
}
