// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST 16

// this sample modifies related data
// including demonstrating the caching of related records
// as would typically be used in a GUI environment

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst16.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	
int main()
{
	cout << "OOFILE Validation Suite - Test 16\n"
		 << "Simple test to demonstrate updating related fields" << endl
		 << "using a similar database to ooftst06 but with direct OID relationships" << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst16:dbPatients.dbf";
			const char* kDatabaseName = ":ooftst16:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst16.db";
		const char* kExistsName = kDatabaseName;
	#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		Patients.deleteAll();
		cout << "Now dumping the entire database should be empty: " << endl << theDB;// david debug 990825
	
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	Patients.AddTestData();
	
	cout << "Now finding the Flu sufferers: " << endl;
	Patients.search(Patients.Visits->Why=="Flu");
	cout << Patients << endl << endl;

	cout << "Now finding the poorly-paid Flu sufferers: " << endl;
	Patients.search(Patients.Salary < 60000 && Patients.Visits->Why=="Flu");
	cout << Patients << endl << endl;

	Patients.selectAll();
	dbView smithVisits(Patients.Visits); 
	smithVisits << Patients.Visits->VisitDate << Patients.Visits->Why; 

	Patients.search(Patients.LastName=="Smith");
	cout << "Dumping Smith and his visits: " << endl 
	     << Patients << endl 
		 << smithVisits << endl;
	
	cout << "changing the first reason to 'Computer-Induced Sanity' " << endl;
	Patients.Visits->gotoRecord(0);
	Patients.Visits->Why = "Computer-Induced Sanity";
	
	cout << "changing the second reason to 'Funny Views' " << endl;
	smithVisits.source()->gotoRecord(1);  // test navigating via the view
	Patients.Visits->Why = "Funny Views";

	Patients.saveRecord();  // save both changes

	cout << "Dumping Smith and changed visits: " << endl 
	     << Patients << endl 
		 << smithVisits << endl;

	Patients.AddVisit("14/2/1995", "Anxiety Attacks");
	
// now change to another related record - our new one should be cached
	smithVisits.source()->gotoRecord(1);  
	Patients.Visits->Why = "Changed Again";

// return to the new record (in the cache) and update it	
	smithVisits.source()->gotoRecord(2);  
	Patients.Visits->VisitDate = "15/2/1994";
	
	Patients.saveRecord(); 
	cout << "Dumping Smith and visits with added visit: " << endl 
	     << Patients << endl 
		 << smithVisits << endl;
		 
	cout << "Now dumping the entire database: " << endl << theDB;
	
	cout << "Description of database schema: " << endl;
	theDB.describe(cout);
	
	cout << "done" << endl;
	return EXIT_SUCCESS;
}       

