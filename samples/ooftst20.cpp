// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST20

// This sample demonstrates how to create an ad-hoc query.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "ooftst01.h"

void anyName(const char* searchStrings[], bool isOr, unsigned short numSch);
void lastName(const char* searchStrings[], unsigned short numSch);

#define numEntries(A) sizeof(A)/sizeof(A[0])

	TEST_CONNECT    theDB;
	dbPeople     People;


int main()
{
	cout << "OOFILE Validation Suite - Test 20\n"
		 << "Demonstration of ad-hoc query building\n";
	
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
		const char* kExistsName =   "People.dat";
		const char* kDatabaseName = "";
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
	}
	else {
		theDB.newConnection(kDatabaseName);
		People.AddTestData();
	}

	People.selectAll();
	cout << "Listing records\n" << (dbView(People) << People.LastName << People.OtherNames);

	const char* userEntries[] = {"Dent", "George"};	
	anyName(userEntries, true, numEntries(userEntries));
	anyName(userEntries, false, numEntries(userEntries));

	const char* userEntries2[] = {"Dent", "Tay*"};	
	lastName(userEntries2, numEntries(userEntries));
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 


void
anyName(const char* searchStrings[], bool isOr, unsigned short numSch)
{
	dbQuery theQuery;
	if (isOr)
		cout << "OR";
	else
		cout << "AND";
	cout << " Querying either Last or Othernames = \n";
	
	for (unsigned short i=0; i<numSch; i++) {
		cout << "   " << searchStrings[i] << endl;

// construct clauses on the heap otherwise they are temporaries
// which won't exist by the time we do the search()
		
		dbQueryClause* term1 = new dbQueryBinary(
			People.LastName == searchStrings[i]
		);
		dbQueryClause* term2 = new dbQueryBinary(
			People.OtherNames == searchStrings[i]
		);

// the following is a bit subtle - the operator||
// which takes pointers creates a dbQueryBinaryComboOwner
// which will delete the sub-clauses
		if (isOr) {
//			theQuery |= (*term1 || term2); equivalent to the following
			theQuery |= term1;
			theQuery |= term2;
		}
		else
			theQuery &= (*term1 || term2);
	}
	People.search(theQuery);

	cout << "\nListing result of query\n" 
		 << (dbView(People) << People.LastName << People.OtherNames) << endl;

}



void
lastName(const char* searchStrings[], unsigned short numSch)
{
	dbQuery theQuery;
	cout << "Querying Lastname = any of\n";
	
	for (unsigned short i=0; i<numSch; i++) {
		cout << "   " << searchStrings[i] << endl;

// construct clauses on the heap otherwise they are temporaries
// which won't exist by the time we do the search()
		
		dbQueryClause* theTerm = new dbQueryBinary(
			People.LastName == searchStrings[i]
		);

		theQuery |= theTerm;
	}
	People.search(theQuery);

	cout << "\nListing result of query\n" 
		 << (dbView(People) << People.LastName << People.OtherNames) << endl;

}

