// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST10

// this sample uses stream operators to set records

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"


DECLARE_CLASS(dbPeople)
	dbChar		LastName, OtherNames;
	dbDate		Birthday;
	dbShort		Short;
	dbUshort	Ushort;
	dbLong		Long;
	dbUlong		Ulong;
	dbReal		Real;
	dbText		Description;

	dbPeople() :
				dbTable("People"),
				LastName(19, "Last Name"),	// use short name to test long string import
				OtherNames(79, "Other Names"),
				Birthday("Birthday"),
				Short("Test Short"),
				Ushort("Test UShort"),
				Long("Test Long"),
				Ulong("Test ULong"),
				Real("Test Real"),
				Description("Description")
				{};
};

TEST_CONNECT theDB;
dbPeople	People;

int main()
{
	cout << "OOFILE Validation Suite - Test 10\n"
		 << "Simple test to demonstrate creating records" << endl
		 << "using input streams" << endl;
		 
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst10:People.dbf";
			const char* kDatabaseName = ":ooftst10:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst10.db";
		const char* kExistsName = kDatabaseName;
	#endif
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	dbDate::sDefaultDateOrder=dbDate::orderMDY;

// fake setting up an input stream, with known quantities.
// in normal life, we'd have an input stream straight from a file
// but we want predictable input so we can use the output for regression testing

	People.setSortOrder(People.OtherNames);
//	People.setReverseSortOrder(People.OtherNames);  shows known bug in reverse char sorts

	theDB.truncateTrailingSpaces();
		
	stringstream oss;

	oss << "Dent" << '\t' << "George" << '\t' << "Jan 22nd 65"
		<< '\t' << -88 << '\t' << 21
		<< '\t' << -6500000 << '\t' << 7200000 << '\t' << 0.5
		<< '\t' << "George is the boss, and by the way none of the figures or dates in these test programs are real." 
		<< endl;
		
	oss << "Dent" << '\t' << "Mildred" << '\t' << "4-15-1971"
		<< '\t' << -7 << '\t' << 670
		<< '\t' << -19100000 << '\t' << 3400000 << '\t' << 1.81 
		<< '\t' << "Beyond Description" << endl;
		
// try with trailing spaces, and with field separators after the last field
// and a really long field to trigger truncation on the LastName
	oss << "Dent           " << '\t' << "Alice" << '\t' <<endl;
	oss << "SmithXXXXXXXXXXXXXXXXXXXX" << '\t' << "Billy" << '\t' << endl;

// try with an extra field that we don't import	
	oss << "Dent" << '\t' << "Annalise" << '\t' << "Jan 13 1990"
		<< '\t' << -88 << '\t' << 21
		<< '\t' << 0 << '\t' << 0 << '\t' << 0.5
		<< '\t' << "" 
		<< '\t' << "IGNORED EXTRA FIELD" 
		<< endl;
		
		
	
	istream is(oss.rdbuf());  // normally something like ifstream is("filename");

	cout << "Entire database  - should be empty" << endl << theDB << endl;
	
	// change the following setting, or comment out this line
	// to see the error message from reading a short record
	People.setShortImports(true);
	
	is >> People;  // ********* Yes, this is the import!!! ********
	
	cout << "Entire database  - after importing from the stream" << endl << theDB << endl;
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

