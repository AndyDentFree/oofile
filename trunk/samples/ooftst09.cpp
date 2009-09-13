// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST9

// this sample deletes records including related data

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"


#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	

void testRecDeletes();

void testRecDeletes()
{
	Patients.start();
	cout << "Deleting individual record - "
		<< Patients.LastName << ", "
		<< Patients.Othernames  
		<< endl << endl;	
	Patients.deleteRecord();
	
	cout << "Should be on next record after delete, which is now first record in file - "
		<< Patients.LastName << ", "
		<< Patients.Othernames  
		<< endl << endl;	

	Patients.last();
	cout << "Goto last record in file - "
		<< Patients.LastName << ", " 
		<< Patients.Othernames  
		<< endl << endl;	

	Patients.deleteRecord();
	cout << "Should be on first record after deleting last - " 
		<< Patients.LastName << ", " 
		<< Patients.Othernames  
		<< endl << endl;	

	Patients.selectAll();
	cout << "Patients  - should be 2 recs" << endl << Patients << endl << endl;
	
}


int main()
{
	cout << "OOFILE Validation Suite - Test 9\n"
		 << "Simple test to demonstrate deleting records" << endl
		 << "using the database from ooftst02" << endl;
		 
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst09:Patients.dbf";
			const char* kDatabaseName = ":ooftst09:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst09.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	Patients.AddTestData();  // always add data - file is empty from last run if it exists

	cout << "Entire database before deletions" << endl << theDB << endl;

	dbView smithVisits(Patients.Visits); 
	smithVisits << Patients.Visits->VisitDate << Patients.Visits->Why; 

	Patients.search(Patients.LastName=="Smith");
	cout << "Dumping Smith and his visits: " << endl 
	     << Patients << endl 
		 << smithVisits << endl;
		
	cout << "Deleting Smith" << endl;
	Patients.deleteRecord();

	cout << "Entire database without Smith" << endl << theDB << endl;
	
	cout << "Deleting all remaining records" << endl;
	Patients.deleteAll();

	cout << "Entire database  - should be empty" << endl << theDB << endl << endl;
	
	Patients.AddTestData();  
	cout << "Testing record position after deletes, with all recs & no sort order"<< endl;
	testRecDeletes();
	Patients.deleteAll();

	Patients.AddTestData();  
	cout << "Test again, with all recs & sort by othername"<< endl << endl;
	Patients.setSortOrder(Patients.Othernames);
	testRecDeletes();
	Patients.deleteAll();
	
	Patients.AddTestData();  
	cout << "Test again, with all recs & non-indexed sort by reverse othername"<< endl << endl;
	Patients.setReverseSortOrder(Patients.Othernames);
	testRecDeletes();
	Patients.deleteAll();
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

