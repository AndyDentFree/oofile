// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST17
// see also ooftst07

// this sample demonstrates searching and sorting by 
// different types of fields, and how to parameterize
// calls, passing in table and field references

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst07.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 17\n"
		 << "Simple test to demonstrate comparative searches" << endl
		 << "using a database identical to ooftst07 except without indices" << endl;
	
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst17:People.dbf";
			const char* kDatabaseName = ":ooftst17:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst17.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	doTest07(kExistsName, kDatabaseName);
	return EXIT_SUCCESS;
}

