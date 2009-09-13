// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST41

// This sample tests the dBase backend's ability to open a schema with different field
// sizes and adjust the current schema to match the dBase file.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	dbConnect_dbase    theDB;	// may make this feature available in others later
	dbPatients     Patients;	
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);


// declare variant of Patients that has a few fields at different sizes
class dbPatients2 : public dbTable {
	OOFILE_METHODS(dbPatients2)
	dbChar		LastName, Othernames;
	dbLong		PatientNo;

	dbPatients2() : dbTable("Patients"),
				LastName(50, "Last Name", kIndexed),	// was 40
				Othernames(20, "Other names", kIndexed),	// was 80
				PatientNo("PatientNo", kIndexNoDups)
	{};
};


// declare subclass of Patients that has a few extra fields
class dbPatients3 : public dbPatients {
	OOFILE_METHODS(dbPatients3)
	dbChar		FirstExtra;
	dbLong		SecondExtra;

	dbPatients3() : 
				FirstExtra(40, "FirstExtra"),
				SecondExtra("SecondExtra")
	{};
};


class dbPatients4 : public dbTable {
	OOFILE_METHODS(dbPatients4)
	// no fields of its own
};
	
	
int main()
{
	cout << "OOFILE Validation Suite - Test 41\n"
		 << "tests the dBase backend's ability to open a schema with\n"
		 << "different field sizes and adjust the current schema to\n"
		 << "match the dBase file." << endl << endl;

	const char* kExistsName =  ":ooftst02:Patients.dbf";
	const char* kDatabaseName = ":ooftst02:";
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
		Patients.AddTestData();
	}

	Patients.setSortOrder(Patients.LastName);
	cout << theDB;
	theDB.close();
	
	
// now open a database with the different schema
	dbConnect_dbase    theDB2;	
	dbPatients2     Patients2;	

	cout << "Modified Patients schema (different widths & fewer fields) before opening\n";
	Patients2.describe(cout);

	theDB2.openConnection(kDatabaseName);
	cout << "Modified Patients schema after opening\n";
	Patients2.describe(cout);
	cout << endl << endl << Patients2 << endl << endl;
	theDB2.close();
	
// now open a 3rd database with MORE fields than the 
	dbConnect_dbase    theDB3;	
	dbPatients3     Patients3;	

	cout << "Modified Patients schema (MORE field) before opening\n";
	Patients3.describe(cout);

	theDB3.openConnection(kDatabaseName);
	cout << "Modified Patients schema after opening\n";
	Patients3.describe(cout);
	cout << endl <<endl << Patients3 << endl << endl;
	
	cout << "Now attempting to write to extra fields:\n";
	Patients3.start();
	Patients3.FirstExtra = "First Extra test";
	Patients3.SecondExtra = 999;
	Patients3.saveRecord();
	cout << endl <<endl << Patients3 << endl << endl;

	theDB3.close();


// open a totally unpopulated table, reading the entire schema in from the dbf file.
// NOTE: dbPatients4 used to have the class definition here but the g++ compiler doesn't
// support the local class declarations with friend function definitions 
// (from ABSTRACT_BASE_METHODS macro, via OOFILE_METHODS)

	dbConnect_dbase    theDB4;	
	dbPatients4	     	Patients4;	
	Patients4.setName("Patients");	// otherwise default table name is dbPatients4
	
	theDB4.openConnection(kDatabaseName);
	cout << "Patients schema after opening with no fields defined\n";
	Patients4.describe(cout);
	cout << endl <<endl << Patients4 << endl << endl;

	theDB4.close();



	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

