// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST85

// This tests
// building schema from stored oofscema and then connecting to the data base usig the schema

#include "oofctrex.h"
#include "ooftest.h"
#define TESTING_CTREE
	TEST_CONNECT    theDB;


// Simple stream I/O is used to interact with the user.
//#include "oofctrex.h"
//#include "ooftest.h"
//#include "ooftst02.h"
#include "oofschema.h"
//void anyName(const char* searchStrings[], bool isOr, unsigned short numSch);
//void lastName(const char* searchStrings[], unsigned short numSch);

//#define numEntries(A) sizeof(A)/sizeof(A[0])//
	//#define TESTING_CTREE
	//TEST_CONNECT    theDB;


int main()
{
	cout << "OOFILE Validation Suite - Test 85\n"
	 << "building schema from oofscema dump \n"
		 <<"	building schema from oofscema dump" << endl << endl;
		 
#define TESTING_SINGLE_FILES
// we can use separate files, or a single container file
// and this sample uses separate files
	//theDB.useSeparateFiles();
//#ifdef _Macintosh

// path handling only on the Mac at present
	//	const char* kExistsName =  ":ooftst01:People.dat";
	//	const char* kDatabaseName = ":ooftst01:";

	//#ifdef _Macintosh
		#ifdef TESTING_DBASE
			const char* kSchemaFileName = ":ooftst86:ooftst86.osc";
			const char* kDatabaseName = ":ooftst86:";
			const char* kExistsName = ":ooftst86:Patients.dbf";
		#else
			#ifdef TESTING_SINGLE_FILES
				const char* kExistsName =  ":ooftst86:People.dat";
				const char* kDatabaseName = ":ooftst86:";
				//const char* kSchemaFileName = "ooftst86.osc";
				const char* kSchemaFileName = ":ooftst86:ooftst86.osc";
		
			#else
				const char* kDatabaseName = "ooftst86.db";
				const char* kExistsName = kDatabaseName;
				const char* kSchemaFileName = "ooftst86.osc";
			#endif
	
		
		#endif
		
	//#else
	//	const char* kDatabaseName = "\\TestDir\\TestSubDir\\People.dat";
	//	const char* kExistsName =   kDatabaseName;
	//	oofString extension = "";
	//	oofString theFile = "People";
	//	oofString thedir = "\\TestDir\\TestSubDir\\"; // dg i think this is correct

	//#endif	
	
//oofString schemaString;
//schemaString =  "table: Patients contains fields:\nfield: Last Name     type: fixed-length chars\n    index: indexed, allowing dups & ignoring case\n    and a maximum length of : 40\nfield: Other names   type: fixed-length chars\n    index: indexed, allowing dups & ignoring case\n    and a maximum length of : 80\nfield: Visits is a 1:N relationship joining across field: PatientNo and propagating deletes to related records\nfield: PatientNo     type: long integer\n    index: indexed, no dups & ignoring case\nfield: Salary    type: long integer\n    index: indexed, allowing dups & ignoring case\n\ntable: Visits contains fields:\nfield: Patients is a N:1 relationship joining across field: PatientNo\nfield: PatientNo     type: long integer\n    index: indexed, allowing dups & ignoring case\nfield: VisitDate     type: date\n    index: indexed, allowing dups & ignoring case\nfield: Reason for Visit  type: fixed-length chars\n    index: indexed, ignoring case, compress front & padding\n    and a maximum length of : 200\n";

oofSchemaParser parser;

//oofFileRef theSource(kSchemaFileName);
//parser.loadSchema(theSource);
parser.loadSchema(kSchemaFileName);

parser.parseSchemaText();
// 

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Just opened...\n" << theDB << endl << endl;
	}

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

