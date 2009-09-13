// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST80

// This tests connection to an existing ctree data base and 
// building schema from doda

// Simple stream I/O is used to interact with the user.
#include "oofctrex.h"
#include "ooftest.h"
//#include "ooftst01.h"

void anyName(const char* searchStrings[], bool isOr, unsigned short numSch);
void lastName(const char* searchStrings[], unsigned short numSch);

#define numEntries(A) sizeof(A)/sizeof(A[0])
#define TESTING_CTREE
	TEST_CONNECT    theDB;


int main()
{
	cout << "OOFILE Validation Suite - Test 80\n"
		 << "existing ctree data base and \n"
		 <<"	building schema from doda" << endl << endl;
	cout << "MUST Thave created the test database with Test 80a.\n";
		 


// we can use separate files, or a single container file
// and this sample uses separate files
	theDB.useSeparateFiles();

//#ifdef _Macintosh

// path handling only on the Mac at present
	//	const char* kExistsName =  ":ooftst01:People.dat";
	//	const char* kDatabaseName = ":ooftst01:";

	#ifdef _Macintosh
		const char* kExistsName =  ":ooftst80:People.dat";
		const char* kDatabaseName = ":ooftst80:";
		oofString extension = "";
		oofString theFile = "People";
		oofString thedir = ":ooftst80:";

	#else
		const char* kDatabaseName = "\\TestDir\\TestSubDir\\People.dat";
		const char* kExistsName =   kDatabaseName;
		oofString extension = "";
		oofString theFile = "People";
		oofString thedir = "\\TestDir\\TestSubDir\\"; // dg i think this is correct

	#endif	


DODAInterface theDoda;

theDoda.loadDODA(theFile,thedir, extension, extension);
theDoda.makeTable();

// open or create the database	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Just opened...\n" << theDB << endl << endl;
	}
	//else {
	//	theDB.newConnection(kDatabaseName);
	//	People.AddTestData();
	//}

	//People.selectAll();
	//cout << "Listing records\n" << (dbView(People) << People.LastName << People.OtherNames);

	//const char* userEntries[] = {"Dent", "Andy"};	
	//anyName(userEntries, true, numEntries(userEntries));
	//anyName(userEntries, false, numEntries(userEntries));

	//const char* userEntries2[] = {"Dent", "Tay*"};	
	//lastName(userEntries2, numEntries(userEntries));
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 


//void
//anyName(const char* searchStrings[], bool isOr, unsigned short numSch)
//{
//	dbQuery theQuery;
//	if (isOr)
	//	cout << "OR";
	//else
	//	cout << "AND";
	//cout << " Querying either Last or Othernames = \n";
	
	///for (unsigned short i=0; i<numSch; i++) {
	//	cout << "   " << searchStrings[i] << endl;

// construct clauses on the heap otherwise they are temporaries
// which won't exist by the time we do the search()
		
		//dbQueryClause* term1 = new dbQueryBinary(
		//	People.LastName == searchStrings[i]
		//);
		//dbQueryClause* term2 = new dbQueryBinary(
		//	People.OtherNames == searchStrings[i]
		//);

// the following is a bit subtle - the operator||
// which takes pointers creates a dbQueryBinaryComboOwner
// which //will delete the sub-clauses
	//	i//f (isOr) {
//	//		//theQuery |= (*term1 || term2); equivalent to the following
	//		theQuery |= term1;
	//		theQuery |= term2;
	//	}
	//	else
	//		theQuery &= (*term1 || term2);
	//}
	//People.search(theQuery);

//	cout << "\nListing result of query\n" 
//		 << (dbView(People) << People.LastName << People.OtherNames) << endl;

//}


/*
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

}*/

