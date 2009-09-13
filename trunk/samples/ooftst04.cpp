// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST4

// this sample just demonstrates the report-writer

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"
#include "oofrw.h"


#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes

	dbConnect_ram    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	

int main()
{
	cout << "OOFILE Validation Suite - Test 4\n"
		 << "Simple test to demonstrate use of the report-writer" << endl
		 << "using the database from ooftest2 and output to a file" << endl;

	theDB.newConnection();
	Patients.AddTestData();

	Patients.setSortOrder(Patients.LastName);
	cout << "Listing records to ooftst04.out...";
	ofstream fs("ooftst04.out");
	
	dbView theView(Patients, false); // pass in false to prevent iterator cloning - HACK until fix dbView copy ctor
	theView << Patients.PatientNo << Patients.LastName << Patients.Salary; 

	dbRepChar tempReport(dbRepSizer("Demo Columnar Report").pageHeight(80),
					dbRepColWidths() << 8 << 10 << 30, theView);
	tempReport.draw(fs);
 
 	fs << endl << endl << "Repeating the report with the same data view"<< endl << endl;
	
	dbRepChar tempReport2(dbRepSizer("Demo Columnar Report").pageHeight(80),
					 theView, dbRep::pageWise);
	tempReport2.draw(fs);
 
	fs.close();
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

