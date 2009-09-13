#include "ooftest.h"
// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST22

// this sample tests some common error conditions that should generate runtime errors
// Note: #define OOF_DEBUG to ensure all errors are trapped

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

DECLARE_CLASS(dbTest22)
	dbChar		f1;
	dbUlong		f2;
	dbReal		f3;  
	
	dbTest22() : f1(20, "F1", kIndexed),
				f2("F2", kIndexed),
				f3("F3", kIndexed)
{};
	};


// show a common technique, using our own class to contain all the database
class myDatabase : public TEST_CONNECT
{
public:
	myDatabase();
	~myDatabase();
	
	dbTest22*     	 dbTest;
};

myDatabase::myDatabase()
{
	dbTest = new dbTest22;
}


myDatabase::~myDatabase()
{
	delete dbTest;
}



int main()
{
	cout << "OOFILE Validation Suite - Test 22\n"
		 << "This tests various common errors that should be picked up with\n"
		 << "runtime errors.\n\n";
	
	cout << "Test defining a dbTable object without a preceding dbConnect. \n"
		 <<	"This will also generate complaints from each field in the dbTest22\n\n";
	dbTest22    	 shouldGenerateRuntimeError;
	
	cout << "Now test defining a dbChar object without a preceding dbTable:\n\n";
	dbChar    	 shouldAlsoGenerateRuntimeError(20, "dbChar to fail");
	
	
	cout << "Now test defining a oofChar and saying it has a keyword index:\n\n";
	oofChar	theDate;
	theDate.indexWords();

	cout << "This tests side effects in declaring a database\n"
		 << "but never opening it. This is legal, but has caused problems in the past\n\n";
	myDatabase*  theDB = new myDatabase;
	
	delete theDB;
	
	cout << endl <<"Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

