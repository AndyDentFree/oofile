// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST6

// this sample modifies related data
// including demonstrating the caching of related records
// as would typically be used in a GUI environment

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
	cout << "OOFILE Validation Suite - Test 6\n"
		 << "Simple test to demonstrate updating related fields" << endl
		 << "using the database from ooftst02" << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst06:Patients.dbf";
			const char* kDatabaseName = ":ooftst06:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst06.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		Patients.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	Patients.AddTestData();

	dbView currentVisits(Patients.Visits); 
	currentVisits << Patients.Visits->VisitDate << Patients.Visits->Why; 

	Patients.search(Patients.LastName=="Smith");
	cout << "Dumping Smith and his visits: " << endl 
	     << Patients << endl 
		 << currentVisits << endl;

	cout << "changing the first reason to 'Computer-Induced Sanity' " << endl;
	Patients.Visits->gotoRecord(0);
	Patients.Visits->Why = "Computer-Induced Sanity";
	
	cout << "changing the second reason to 'Funny Views' " << endl;
	currentVisits.source()->gotoRecord(1);  // test navigating via the view
	Patients.Visits->Why = "Funny Views";

	Patients.saveRecord();  // save both changes

	cout << "Dumping Smith and changed visits: " << endl 
	     << Patients << endl 
		 << currentVisits << endl;

	Patients.AddVisit("14/2/1995", "Anxiety Attacks");
	
// now change to another related record - our new one should be cached
	currentVisits.source()->gotoRecord(1);  
	Patients.Visits->Why = "Changed Again";

// return to the new record (in the cache) and update it	
	currentVisits.source()->gotoRecord(2);  
	Patients.Visits->VisitDate = "15/2/1994";
	
	Patients.saveRecord(); 

	cout << "Dumping Smith and visits with added visit: " << endl 
	     << Patients << endl 
		 << currentVisits << endl;
		 
	cout << "Now dumping the entire Visits file: " << endl << Visits << endl;

// demonstrate changing ownership of a related record

/*
Initial attempt - doesn't work as we propagate changes in the parent join field
down to related records unconditionally!

	Patients.search(Patients.LastName=="Taylor"); 
	assert(Patients.count()==1);
	long TaylorPatientNo = Patients.PatientNo;	// destination ID
	Patients.search(Patients.LastName=="Smith");	// got the source
	Patients.Visits->gotoRecord(0);
	Patients.Visits->PatientNo = TaylorPatientNo;
	Patients.saveRecord();
*/	

// edit the Visits record directly
	Patients.search(Patients.LastName=="Taylor"); 
	assert(Patients.count()==1);
	long TaylorPatientNo = Patients.PatientNo;	// destination ID
	Patients.search(Patients.LastName=="Smith");	// got the source
	Patients.Visits->gotoRecord(0);
	oidT VisitsOID = Patients.Visits->currentOID();
	Visits.selectJustOID(VisitsOID);
	Visits.PatientNo = TaylorPatientNo;
	Visits.saveRecord();
	
	cout << "Dumping Smith and changed visits (one removed): " << endl 
	     << Patients << endl 
		 << currentVisits << endl;
	
	Patients.search(Patients.LastName=="Taylor"); 
	cout << "Dumping Taylor and added visit: " << endl 
	     << Patients << endl 
		 << currentVisits << endl;

	cout << "done" << endl;
	
	return EXIT_SUCCESS;
}       

