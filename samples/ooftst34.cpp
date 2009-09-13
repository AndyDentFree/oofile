// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST34

// This sample tests the Persistent RAM database backend 
// ability to save and revert the whole database.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	dbConnect_ramp    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	
int main()
{
	cout << "OOFILE Validation Suite - Test 34\n"
		 << "Simple test of Persistent RAM (ramp) backend" << endl
		 << "save and revert facility" << endl << endl;

	const char* kDatabaseName = "ooftst34.db";
	const char* kExistsName = kDatabaseName;
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
		theDB.save();	// so we have a database on disk from which to Revert
	}

	cout << theDB;

	Patients.search(Patients.PatientNo==3);
	cout << "Finding Patient No 3 : " << Patients << endl;
	
	cout << "Changing name: " << endl;
	Patients.LastName = "Whatnot";
	Patients.saveRecord();
	Patients.selectAll();// so we can see that data went in and not just in buffer
	cout << Patients << endl << endl;

	theDB.revert();
	cout << "After revert\n" << theDB << endl << endl;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

