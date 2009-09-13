// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST8

// This sample just tests the dbConnect::raise() method of error reporting.
// which should use a message box on the Windows and Mac environments
// and console output elsewhere

#include "ooftest.h"


int main()
{
	cout << "Testing user-specified error reporter" << endl << flush;
	
// use a reporter designed for CGI programs to report errors back to web pages	
	oofErrorReporter::setDefaultReporter(new oofCGIErrorReporter(cout));
	dbConnect::raise( "Simple error without aborting, to see the HTML generated",false);
	
	oofErrorReporter::setDefaultReporter(0);	// clear our custom reporter
	dbConnect::raise( stringstream() 
	   << "This is a longer string to see if I can get the Alert to handle it or not"
	   << endl
	   << "and just what happens with embedded newlines, also to see if the wrapping works OK "
	   << "for very long lines which I am sure are now exceeding the 255 character limit, or "
	   << "if not yet will soon be and so it will be interesting to see just what the compiler "
	   << "complains of if indeed it does complain!");
	cout << "Finished" << endl << flush;
	
	return EXIT_SUCCESS;
}       

