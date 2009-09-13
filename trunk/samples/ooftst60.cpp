// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST60

// this sample demonstrates searching and sorting by 
// different types of fields, and how to parameterize
// calls, passing in table and field references

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

//#define TEST_INDEXES

#include "ooftst60.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 60\n"
		 << "Simple test to demonstrate comparative searches" << endl
		 << "using a database similar to ooftest2 but with lots of indexed fields" << endl;
	
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst60:People.dbf";
			const char* kDatabaseName = ":ooftst60:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst60.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	doTest60(kExistsName, kDatabaseName);
	return EXIT_SUCCESS;
}

