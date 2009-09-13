// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST28

// This sample tests the keyword indexing.
// see also ooftst27 for a simple test of the word parser.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"
#include "ooftst01.h"


int main()
{
	cout << "OOFILE Validation Suite - Test 28\n"
		 << "Use keyword indexing to retrieve by words within fields." << endl;

	dbConnect_ctree    theDB;
	dbPeople     People;

	theDB.useSeparateFiles();	
	theDB.useExclusiveAccess();	// force this despite filesharing library (FPUTFGET option)
								// so we can rebuild the indexes
							
// this test creates People.dat, People.idx & Blobs
// in the same directory

	#define kExistsName ":test28:People.dat"
	#define kDatabaseName ":test28"

	// ****** attach a keyword index to a couple of fields
	// BEFORE the openConnection or newConnection - it's part of the field declaration
	
	// The keyword index can also be searched as a table-level index, searching for
	// words regardless of which field they come from
		People.LastName.indexWords();
		People.Description.indexWords();

	// NOTE: we are using the default word parser here. You could also pass
	// your own specialised parser in as a parameter to the above, eg:
	//    People.LastName.indexWords(new MyFancyParser)



	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		cout << "Deleting previous test data\n";
		People.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	People.suppressIndices();	
	cout << "Adding test data\n\n";
	People.AddTestData();

//	People.setSortOrder(People.LastName);
//	cout << "Listing entire database\n" << theDB << endl << endl;
// NOTE we dumped all the fields of People this time, from now on we will
// use dbViews to restrict the fields listed

		
	dbView justNames(People, false);	// BUG searchSelection doesn't work if don't specify false = not a shared selection
	justNames << People.LastName << People.OtherNames;
	
	People.search(People.Description.hasWord("bloke"));
	cout << "People with 'bloke' in their Description (keyword file non-indexed):\n"
		 << justNames << endl;

	People.search(People.Description.hasWordStartsWith("blo"));
	cout << "People with word starting with 'blo' in their Description (John Smith):\n"
		 << justNames << endl;

	cout << "Now rebuilding the database indices, to see if above faster\n";
	People.rebuild();

	People.search(People.Description.hasWord("bloke"));
	cout << "People with 'bloke' in their Description (John Smith):\n"
		 << justNames << endl;

	People.search(People.Description.hasWord("blo"));
	cout << "People with 'blo' in their Description (nobody):\n"
		 << justNames << endl;

	People.search(People.Description.hasWordStartsWith("blo"));
	cout << "People with word starting with 'blo' in their Description (John Smith):\n"
		 << justNames << endl;

	People.search(People.Description.hasWord("George"));
	cout << "People with 'George' in their Description (George & Mildred Dent):\n"
		 << justNames << endl;

	People.searchSelection(People.Description.hasWord("married"));
	cout << "People with 'George' in their Description and also (via searchSelection) 'married' (Mildred Dent):\n"
		 << justNames << endl;

	People.start();	// after the above output via cout
	People.Description += " BTW George's not a bad bloke either.";
	People.saveRecord();	// will re-index
	
	People.search(People.Description.hasWord("bloke"));
	cout << "People with 'bloke' in their Description (John Smith & Mildred Dent):\n"
		 << justNames << endl;

	People.search(People.Description.hasAnyWordsDelimited("bloke/software", '/'));
	cout << "People with 'bloke' or 'software' in their Description (John Smith, George & Mildred Dent):\n"
		 << justNames << endl;

	People.search(People.Description.hasAllWordsDelimited("bloke/software", '/'));
	cout << "People with 'bloke' and 'software' in their Description (nobody):\n"
		 << justNames << endl;

	People.search(People.Description.hasAllWordsStartsWithDelimited("blo/so", '/'));
	cout << "People with 'blo' and 'so' in their Description (John Smith):\n"
		 << justNames << endl;

	const char* schWords[2] = {"giraffe", "boots"};
	People.search(People.Description.hasAnyWordsOf(schWords, 2));
	cout << "People with 'giraffe' or 'boots' in their Description (John Smith):\n"
		 << justNames << endl;

	People.search(People.Description.hasAllWordsOf(schWords, 2));
	cout << "People with 'giraffe' and 'boots' in their Description (nobody):\n"
		 << justNames << endl;

	cout << "\n\n******** Now search table, regardless of field *********\n\n";

	People.search(People.hasWord("bloke"));
	cout << "People with 'bloke' (John Smith & Mildred Dent):\n"
		 << justNames << endl;

	People.search(People.hasAnyWordsDelimited("bloke/software", '/'));
	cout << "People with 'bloke' or 'software' (John Smith, George & Mildred Dent):\n"
		 << justNames << endl;

	People.search(People.hasAllWordsDelimited("bloke/software", '/'));
	cout << "People with 'bloke' and 'software' (nobody):\n"
		 << justNames << endl;

	People.search(People.hasAllWordsStartsWithDelimited("blo/so", '/'));
	cout << "People with 'blo' and 'so' (John Smith):\n"
		 << justNames << endl;

	People.search(People.hasAnyWordsOf(schWords, 2));
	cout << "People with 'giraffe' or 'boots' (John Smith):\n"
		 << justNames << endl;

	People.search(People.hasAllWordsOf(schWords, 2));
	cout << "People with 'giraffe' and 'boots' (nobody):\n"
		 << justNames << endl;

	const char* schWords2[2] = {"Mildred", "married"};
	People.search(People.hasAllWordsOf(schWords2, 2));
	cout << "People with 'Mildred' and 'married' (Mildred Dent):\n"
		 << justNames << endl;

	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

