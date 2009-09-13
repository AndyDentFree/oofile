// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST19

// this sample demonstrates using an abstract base class
// and using mixin inheritance

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

class dbSomeFields
{
public:
	dbChar Recipient;
	dbSomeFields() :
		Recipient(80, "Recipient")
	{};
};


DECLARE_ABSTRACT_CLASS(dbFileBase)
public:
	dbChar		FileName;
	dbFileBase(const char* tableName) : dbTable(tableName),
		FileName(255, "File Name") 
	{};
	virtual void FormattedName(ostream&)=0;
};


class dbIncoming : public dbFileBase
{
public:
// as we inherit from other than dbTable,
// must use the following macro instead of DECLARE_CLASS
	OOFILE_METHODS(dbIncoming)
	virtual void FormattedName(ostream&);
	dbIncoming() : dbFileBase("Incoming") {};
};


class dbOutgoing : public dbFileBase, public dbSomeFields
{
public:
// as we use multiple inheritance, must use the following macro instead of DECLARE_CLASS
	OOFILE_METHODS(dbOutgoing)
	virtual void FormattedName(ostream&);
	dbOutgoing() : dbFileBase("Outgoing"), dbSomeFields() {};
};


void 
dbIncoming::FormattedName(ostream& os)
{
	os << FileName << endl;
}


void 
dbOutgoing::FormattedName(ostream& os)
{
	os << FileName << " sent to " << Recipient << endl;
}


int main()
{
	TEST_CONNECT   	theDB;
	dbIncoming     		Incoming;
	dbOutgoing			Outgoing;

	cout << "OOFILE Validation Suite - Test 19\n"
		 << "Simple test to demonstrate inheritance" << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst19:Incoming.dbf";
			const char* kDatabaseName = ":ooftst19:";
		#else
			const char* kExistsName =   "Incoming.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst19.db";
		const char* kExistsName = kDatabaseName;
	#endif

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
	}
	else {
		theDB.newConnection(kDatabaseName);
		Incoming.newRecord();
		Incoming.FileName = "An Incoming File";
		Incoming.saveRecord();
		
		Outgoing.newRecord();
		Outgoing.FileName = "An Outgoing File";
		Outgoing.Recipient = "XXX";
		Outgoing.saveRecord();
		
	}

	cout << "Invoking a virtual function on each in turn" << endl;
	Incoming.FormattedName(cout);
	Outgoing.FormattedName(cout);

	cout << endl << "Dumping entire database" << theDB;
	
	cout << "Test Completed" << endl;

	return EXIT_SUCCESS;
}

