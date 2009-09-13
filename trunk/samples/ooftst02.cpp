// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST2

// This sample tests the database backend by creating a pair of tables
// and storing and retrieving indexed data via traversal paths.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	
int main()
{
	cout << "OOFILE Validation Suite - Test 2\n"
		 << "Simple test to store some data and retrieve it" << endl
		 << "using a relation joining over a field and showing" << endl
		 << "iterators on related tables and 1-many relations" << endl << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst02:Patients.dbf";
			const char* kDatabaseName = ":ooftst02:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst02.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		//Patients.deleteAll();
		//Patients.AddTestData();
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
	}

	Patients.setSortOrder(Patients.LastName);
	cout << theDB;
	
	Patients.start(); 
	while (Patients.more()) {
		cout << Patients.PatientNo << '\t' 
			 << Patients.LastName << endl;
		if (Patients.Visits->count()==0)
			cout << "no visits" << endl;
		else {
			Patients.Visits->start(); 
			while (Patients.Visits->more()) {
				cout << '\t' << Patients.Visits->VisitDate << '\t'
			      	<< Patients.Visits->Why << endl;
				Patients.Visits->next();
			}
		}
		cout << endl;
		Patients.next();
	}

	cout << endl << "Now repeating the process using a dbView instead of explicitly" 
	     << endl << "iterating over the related file." << endl;
	dbView relatedVisits(Patients.Visits);
	relatedVisits << Patients.Visits->VisitDate << Patients.Visits->Why;
	Patients.start(); 
	while (Patients.more()) {
		cout << Patients.PatientNo << '\t' 
			 << Patients.LastName << endl;
		if (relatedVisits.source()->isEmpty())
			cout << "no visits" << endl;
		else 
			cout << relatedVisits << endl;
		cout << endl;
		Patients.next();
	}

	Patients.search(Patients.PatientNo==3);
	cout << "Finding Patient No 3 : " << Patients << endl;
	
	cout << "Now finding the Flu sufferers: " << endl;
	Patients.search(Patients.Visits->Why=="Flu");
	cout << Patients << endl << endl;

	Patients.setSortOrder(Patients.Othernames);
	cout << "Now dumping the entire database, with patients sorted by Othernames: " << endl << theDB;
	
	cout << "Description of database schema: " << endl;
	theDB.describe(cout);
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

