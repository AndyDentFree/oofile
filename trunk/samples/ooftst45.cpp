// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST45

// Simple stream I/O is used to interact with the user.
// to demonstrate indexed calculated fields
// only useful for OOFILE Professional - c-tree backend

#include "ooftest.h"

class dbTest45 : public dbTable {
	OOFILE_METHODS(dbTest45)
	dbChar	Phone;
	dbChar	Name;
	dbChar	Code;
	
	dbTest45() : 
		Phone(23, "Phone"),
		Name(80, "Name"),
		Code(23, "Compressed Code", kIndexed)
	{
		Code.calculateWith(new dbCharStripperCalculator(Phone));
		setSortOrder(Code);
	};
	
	void addRecord(const char* name, const char* phone);
	void addTestData();
};


void
dbTest45::addRecord(const char* inName, const char* inPhone)
{
	newRecord();
		Name = inName;
		Phone = inPhone;
		// Code is calculated
	saveRecord();
}


void
dbTest45::addTestData()
{
	addRecord("Victor Morris First National Real Estate",  "0011652218888");
	addRecord("Ray White Kent Prier Real Estate Ltd",  "0064033794120");
	addRecord("Remo Romeo Landscaping",  "014 465729");
	addRecord("Mike Stanley's Realty Options",  "014 662962");
	addRecord("Cathy's Creative Gardens",  "014 9911195");
	addRecord("Mitchelhill Dyann Real Estate",  "015 004500");
	addRecord("Wiltshire & Company",  "015 115085");
	addRecord("Beachside North Shore Realty",  "015 137 755");
	addRecord("Alpha Realty Sushine Coast",  "015 158 218");
	addRecord("Kuranda Real Estate",  "015 160 659");
	addRecord("Levi's Property Shop",  "015 249396");
	addRecord("Guy Higgins Resort Sales",  "015 260071");
	addRecord("Yvonne Boyle Pty Ltd",  "015 291413");
	addRecord("John Boyle Real Estate",  "015 333811");
	addRecord("Pegg Real Estate",  "015 345847");
	addRecord("Kearnan & Co",  "015 348360");
	addRecord("Fletcher Real Estate",  "015 353898");
	addRecord("David Phillips & Co",  "015 504181");

}

int main()
{
	cout << "OOFILE Validation Suite - Test 45\n"
		 << "Shows a calculated field being indexed and used for searching\n";

	dbConnect_ctree theDB;
	dbTest45     	Phones;

	const char* kDatabaseName = "ooftst45.db";
	const char* kExistsName = kDatabaseName;

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
		Phones.addTestData();
	}
	
	cout << "Entire database" << endl << theDB << endl << endl;  // just to see the calculated Code field
	
	// test that the calculator clones correctly
	dbTest45 somePhones = Phones;
	somePhones.search(somePhones.Code=="015004500");
	cout << "Only 015004500" << endl << somePhones << endl << endl;
	
	somePhones.search(somePhones.Code.startsWith("015"));
	cout << "Starting with  015" << endl << somePhones << endl << endl;
	
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

