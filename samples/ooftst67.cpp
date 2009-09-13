// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST67

// This sample is used to check the functioning of the dbUserObject and associated error handling
// Simple stream I/O is used to interact with the user.
#include "ooftest.h"
#include "dbUser.h"
#include "oofexcep.h"

int main()
{


	//const char* kDatabaseName  ="spod";
	const char* kServerName  ="FAIRCOMS@Davidzone";
	cout << "OOFILE Validation Suite - Test 67\n"
			 << "Simple test to  obtain check functioning of the dbUser object and associated error handling" << endl << endl;
			
	cout << "First create a user with correct length password and userID" << endl << endl; 
	
	dbUser me("FEFI","FOFUM",kServerName);
	
	cout << "Object me has userID of " << me.ID() << " whose password is " << me.passWord() <<"." << endl << endl; 
	
	cout << "Now attempt to change the user ID to a string whose length is greater than 32"  << endl << endl; 
	try{
		me.ID("she sells sea shells by the sea shore");
	
	}
	catch(oofE_dbUser& err){
		err.extract(cout);	
		cout << "The error number is " << err.errorNumber() << endl << endl;
	}
	
	
	
	cout << "Now attempt to change the user password to a string whose length is greater than 10"  << endl << endl; 
	try{
		me.passWord("splat 2000a");
	
	}
	catch(oofE_dbUser& err){
		err.extract(cout);	
		cout << "The error number is " << err.errorNumber() << endl << endl;
		
	}
	
	
	cout << "Finally check that we can actually set the id and password \"accounts\" & \"trident2\" "  << endl << endl; 
	me.passWord("trident2");
	me.ID("accounts");
	cout << "Object me now has userid of " << me.ID() << " and password " << me.passWord() <<"." << endl << endl; 
	
	
	cout << "done" << endl << endl;
	
	
	return EXIT_SUCCESS;
}       

