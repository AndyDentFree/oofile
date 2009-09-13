// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST21

// This sample tests the multiple field sorting.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"
#include "oofios.h"

DECLARE_CLASS(dbTest21)
	dbChar		f1;
	dbUlong		f2;
	dbReal		f3;  
	static dbSorter sort123;
	
	dbTest21() : f1(20, "F1", kIndexed),
				f2("F2", kIndexed),
				f3("F3", kIndexed)
{};
	
// my own data entry procedures
	void Add(const char *tf1,const unsigned long tf2,const double tf3);
	void AddTestData();
};


void dbTest21::Add(const char *tf1,const unsigned long tf2,const double tf3)
{
	newRecord();
	f1=tf1;
	f2=tf2;
	f3=tf3;
	saveRecord();
}

void dbTest21::AddTestData()
{
	Add("Denton",2000,3.5);
	Add("Taylor",2000,4.5);
	Add("Taylor",3000,5.5);
	Add("Taylor",2000,3.5);
	Add("Denton",1000,1.5);
	Add("Underwood",1000,1.5);
	Add("Zaphod",5000,5.5);
}



// test some specific sort bugs reported by Liam
class dbArchive : public dbTable { 

	OOFILE_METHODS(dbArchive)  

public:  // table columns for all archives
	dbLong	Size; // in bytes
	dbDate	Date;
	dbChar	FileName;
	dbSorter arcSort;
	
	dbArchive() : 
		Size("Size", kIndexed),
		Date("Date Created"),
		FileName(80, "File Name", kIndexCompress)
	{
		arcSort >> Date << FileName;
	};
	void AddTestData();
};

void
dbArchive::AddTestData() {
	dbDate::sDefaultDateOrder = dbDate::orderMDY;
	
	ostrstream oss;
	oss << "153357\tMar 15 1996\tart/zine/real-macoy-11.hqx" << endl
		<< "288387\tMar 15 1996\tart/zine/real-macoy-12.hqx" << endl
		<< "205018\tMar 15 1996\tart/zine/real-macoy-13.hqx" << endl
		<< "448459\tMar 15 1996\tart/zine/real-macoy-14.hqx" << endl
		<< "6096\tMar 16 1996\tdev/cw LMouser.hqx" << endl
		<< "190561\tMar 16 1996\tdev/cw cw-pascal-tcl-port-10.hqx" << endl;
	istream is(oss.rdbuf());
	is >> *this;
}

// static sort member
	dbSorter		 dbTest21::sort123;

int main()
{
	cout << "OOFILE Validation Suite - Test 21\n"
		 << "This tests the string contains functions\n"
		 << "and the nested sorts\n\n";
	
	TEST_CONNECT  theDB;
	dbTest21     	 dbTest;
	dbArchive		 Archive;
	
// init static sort	
	dbTest21::sort123 << dbTest.f1 << dbTest.f2 << dbTest.f3;  

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst21:dbTest21.dbf";
			const char* kDatabaseName = ":ooftst21:";
		#else
			const char* kExistsName =   "dbTest21.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst21.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName))
		theDB.openConnection(kDatabaseName);
	else {
		theDB.newConnection(kDatabaseName);
		dbTest.AddTestData();
		Archive.AddTestData();
	}
	cout << "Listing All Records" << endl << endl;
	cout << dbTest;
	
	cout << endl << "Test 'Contains Any' Search - searching for 'ta', 'en' or 'wood'" << endl << endl;
	dbTest.searchSelContainsAnyDelimited(dbTest.f1, "ta,en,wood",',');
	cout << dbTest;
	
	cout << endl << "Test 'Contains All' Search - searching for 'wood', 'u' and 'de'" << endl << endl;
	dbTest.searchSelContainsAllDelimited(dbTest.f1, "wood,u,de",',');
	cout << dbTest;
	
	dbTest.selectAll();
	
	cout << endl <<"Testing Nested sort - sorting by all fields, 1 - 2 - 3" << endl << endl;
	dbTest.setSortOrder(dbTest.sort123);
	cout << dbTest;

	cout << endl <<"Testing Nested sort on 1st all same value - sorting by all fields, 1 - 2 - 3" << endl << endl;
	dbTest.search(dbTest.f1.startsWith("Tay"));
	cout << dbTest;

	dbTest.selectAll();
	
	cout << endl <<"Testing Nested sort - sorting by all fields, 2 - 1 - 3" << endl << endl;
	dbSorter sort213;
	sort213 << dbTest.f2 << dbTest.f1 << dbTest.f3;
	dbTest.setSortOrder(sort213);
	cout << dbTest;

	cout << endl << "now reversing the second sort item in the above test" << endl << endl;
	dbTest.setSortOrder(dbSorter() << dbTest.f2 << reversed(dbTest.f1) << dbTest.f3);
	cout << dbTest;
	
	cout << endl << "Testing suspended sorts and cloning selections (fields 123)" << endl;
	dbTest.setSortOrder(dbTest.sort123);
	dbTest.suspendSorting();
	dbTest.search(dbTest.f1.startsWith("Tay"));
	cout << "Unsorted..." << endl << dbTest << endl;

	dbTest21 cloned(dbTest);  // should have same sort order, still suspended

	dbTest.resumeSorting();
	cout << "Sorted..." << endl << dbTest << endl;
		
	cout << "Unsorted clone..." << endl << cloned << endl;
	cloned.resumeSorting();
	cout << "Sorted clone..." << endl << cloned << endl;
	

	cout << "Now testing some additional multiple field sort bugs" << endl;
	cout << "Unsorted" << endl << Archive << endl;
	
	Archive.setSortOrder(Archive.arcSort);
	cout << ">> Date << FileName" << endl << Archive << endl;

	cout << endl <<"Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

