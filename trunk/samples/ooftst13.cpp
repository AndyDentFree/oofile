// Copyright 1995 A.D. Software. All Rights Reserved

// OOFTEST13

// This sample tests the database backend by creating a single table
// and storing and retrieving NON-indexed data.

// Simple stream I/O is used to interact with the user.

// it is very nearly a copy of ooftst01.cpp


#include "ooftest.h"
#define demoNoIndexes  // just fiddles with ooftst01.inc definitions
#include "ooftst01.h"


int main()
{
	cout << "OOFILE Validation Suite - Test 13\n"
		 << "Simple test to store some data and retrieve it\n"
		 << "in a single-table database, with no relations and no indices !\n";
	
	dbConnect_ctree    theDB;
	dbPeople     People;

// Note: although using the same schema (Nearly) we can't use the
// files from ooftst01 & ooftst14 as they have indexes on several fields
	if (dbConnect::fileExists("ooftst13.db"))
		theDB.openConnection("ooftst13.db");
	else {
		theDB.newConnection("ooftst13.db");
		People.AddTestData();
	}
	
	cout << "Retrieving Smith: " << People[People.LastName=="Smith"].LastName << endl;

// create a dbView which we will use over and over, unlike ooftst01
// where we create the dbView temporarily in the cout

	dbView justNames(People);
	justNames << People.LastName << People.OtherNames;
	dbView NamesAndSalaries = justNames;
	NamesAndSalaries << People.Salary;

	People.search(People.LastName=="Dent");	
	cout << "Listing two Dent records: " << endl << justNames << endl;

	People.search(People.OtherNames<"M");
	cout << "Listing records with other names < M: " << endl << justNames << endl;

	People.search(People.LastName>="Smith");
	
	cout << "Listing records with surnames >= Smith: " << endl << justNames << endl;

	People.search(People.Salary.between(5000,25000));	
	cout << "Listing records with salaries between 5000 and 25000 inclusive: " 
		 << endl << NamesAndSalaries << endl;

	People.search(People.Salary>10000);	
	cout << "Listing records with salaries greater than 10000: " << endl << NamesAndSalaries << endl;
			
	People.searchSelection(People.LastName=="Dent");	
	cout << "Refining this selection to the single Dent record: " << endl << justNames << endl;
			
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

