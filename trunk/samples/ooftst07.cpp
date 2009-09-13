// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST07

// this sample demonstrates sorting by 
// different types of fields, and how to parameterize
// calls, passing in table and field references

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#define TEST_INDEXES

#include "ooftst07.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 7\n"
		 << "Simple test to demonstrate comparative searches" << endl
		 << "using a database similar to ooftest2 but with lots of indexed fields" << endl;
	
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst07:People.dbf";
			const char* kDatabaseName = ":ooftst07:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst07.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	doTest07(kExistsName, kDatabaseName);
	return EXIT_SUCCESS;
}

