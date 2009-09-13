// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST26

// This sample tests searching by related files.

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
	cout << "OOFILE Validation Suite - Test 26\n"
		 << "Simple test of counting related data" << endl
		 << "and intersecting related data" << endl
		 << "as is often used in producing reports or graphs" << endl << endl;
	
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
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
	}

	cout << theDB << endl;
	
	Patients.search(Patients.LastName == "Dent");
	cout << "Number of 'Dent' Patients  = " << Patients.count() << endl;
	
	Visits.search(Visits.VisitDate < "1-11-1994");
	cout << "Number of Visits prior 1-11-1994 = " << Visits.count() << endl;
	
	cout << "Number of Visits related to selected Patients = " 
		 << Patients.Visits.countAllRelated() << endl;

	cout << "Number of Visits related to selected Patients prior 1-11-1994 = " 
		 << Patients.Visits.countAllRelatedIn(Visits) << endl;

	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

