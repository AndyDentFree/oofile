// Copyright 1999 A.D. Software. All Rights Reserved

// OOFTEST55

// This sample tests the database backend by creating a pair of tables
// and storing and retrieving indexed data via traversal paths.
// just like two except the  why field of vists table is a dbText
// used to test blob handling and caching across relationships
// a variation of ooftest 48 that tests related blob caching
// designed to test a particular bug in dbase backends
// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

#include "ooftst55.h"

// global variables that define the database using the ooftst02 classes

	TEST_CONNECT    theDB;
	dbPatients     Patients;
	dbVisits	Visits;
	dbRelationship PatientVisits(Patients.Visits, Visits.Patient);
	
int main()
{
	cout << "OOFILE Validation Suite - Test 55\n"
		 << "Simple test to store some data and retrieve it" << endl
		 << "using a relation joining over a field and showing" << endl
		 << "iterators on related tables and 1-many relations" << endl 
		 << "similar to ooftst2 but blobs are used instead of dbText"  << endl << endl;

	/*#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst055:Patients.dbf";
			const char* kDatabaseName = ":ooftst048:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst048.db";
		const char* kExistsName = kDatabaseName;
	#endif
*/	
//	#ifdef TESTING_CTREE
// we can use separate files, or a single container file
// and this sample uses separate files
//	theDB.useSeparateFiles();

/*	#ifdef _Macintosh
// path handling only on the Mac at present
		const char* kExistsName =  ":ooftst55:People.dat";
		const char* kDatabaseName = ":ooftst55:";
	#else
		const char* kDatabaseName = "\\TestDir\\TestSubDir\\People.dat";
		const char* kExistsName =   kDatabaseName;
	#endif	

#else
	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst55:People.dbf";
			const char* kDatabaseName = ":ooftst55:";
		#else
			const char* kExistsName =   "People.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		// Persistent RAM, no option of single file
		#ifdef _Macintosh
			const char* kDatabaseName = ":ooftst55:ooftst55.db";
		#else
			const char* kDatabaseName = "ooftst55.db";
		#endif	
		const char* kExistsName = kDatabaseName;
	#endif
#endif*/
#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst55:Patients.dbf";
			const char* kDatabaseName = ":ooftst55:";
		#else
			const char* kExistsName =   "Patients.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst55.db";
		const char* kExistsName = kDatabaseName;
	#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		Patients.deleteAll();
		Patients.AddTestData();
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
	// test db blob bug
	Patients.search(Patients.PatientNo==2);
	
	Patients.Visits->VisitDate = "9/9/1999"; 
	Patients.Visits->next();
	//iterate vistits and force caching
	Patients.AddVisit("1/1/2001", "Dengue Fever");
	//Patients.Visits->saveRecord();
	// end dbblobs bug
	Patients.Visits->next();
		
	cout << " now dumping current record visits record should get cached record" << endl;
		Patients.Visits->start(); 
			while (Patients.Visits->more()) {
				cout << '\t' << Patients.Visits->VisitDate << '\t'
			      	<< Patients.Visits->Why << endl;
				Patients.Visits->next();
			}
		cout << endl;
	Patients.unloadRecord();
	
	cout << "the whole data base after dumping cache: " << endl;
	cout << theDB;
	

		cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

