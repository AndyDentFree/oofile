// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST65

// this sample uses shows an easy way to generate test data
// this is similar to ooftest 11, but it connects to the data used by contacts gui example in the server
// this is a client test and is currently hardwired to file spod

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"
#include "ooftst65.h"
	

extern "C" {
	#include "ctstdr.h"
	#include "ctoptn.h"
	#include "ctdecl.h"
	#include "cterrc.h"
}

//#define TRNLOG		0x0030

dbConnect_FServ theDB;
dbContacts	contacts;

int main()
{

#pragma message("******YOU MUST set the following two lines to match your server ****")
const char* kDatabaseName  ="thing 1:spod2:spod3:noTransactions";
const char* kServerName  ="FAIRCOMS@bannana";

	dbUser me("ADMIN","ADMIN",kServerName);


	cout << "OOFILE Validation Suite - Test 65\n"
		 << "Simple test to demonstrate using the inbuilt test data generator" << endl
		 << "similar to test 11 but connects to the contacts data base on the server"
		 <<" to make nice big test data bases you would to generate random" << endl
		 <<  "data for a real project" << endl << endl;
		 	
	cout << endl << "Number of test records: " << flush;
	unsigned long numRecs;
	cin >> numRecs;

	if (numRecs==0)
		return EXIT_SUCCESS;
		
	cout << endl << "Do you want random data? (Y/N) " << flush;
	char randomYN = 'N';
	cin >> randomYN;
	
	cout << endl << "Do you want to display the test data? (Y/N) " << flush;
	char displayYN = 'N';
	cin >> displayYN;
	
	if(theDB.openOrCreateConnection(kDatabaseName,&me))
		cout << "Just opened...\n" << theDB << endl << endl;
	else 
		cout << "Just created...\n" << theDB << endl << endl;
	cout << endl << "Generating " << numRecs << " test records..." << endl;
	
	theDB.generateTestData(numRecs, (randomYN=='y') || (randomYN=='Y'));
	
	if ((displayYN=='y') || (displayYN=='Y'))
		cout << theDB << endl;
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

