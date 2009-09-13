// Copyright 1995 A.D. Software. All Rights Reserved

// OOFTEST14 - Mixture of OOFTest1 and 4 with HTML Reports

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"
#include "oofhtml.h"
#include "ooftst01.h"

int main()
{
	cout << "OOFILE Validation Suite - Test HTML\n"
		 << "Simple test of the HTML Report Writer\n";
	
	dbConnect_ram    theDB;
	dbPeople     People;

// bit of complicated filename logic to support different backends
// with this one test program

	theDB.newConnection();
	People.AddTestData();
	People.setSortOrder(People.LastName);
	
	cout << "This test produces the columnar and pagewise reports in two variants," << endl
	<< "first using plain HTML then using the Netscape tables" << endl << endl	
	<< "Producing simple HTML Columnar report to oofts14a.htm...";
	
	dbView theView(People, false); // faster to avoid cloning an iterator HACK for this relesae only
	theView << People.LastName << People.OtherNames << People.Description; 

	ofstream fs("oofts14a.htm");
	dbRepHTML tempReport(dbRepSizer("Demo Columnar Report").pageHeight(80),
					dbRepColWidths() << 10 << 15 << 50, theView);
	tempReport.draw(fs);
	fs.close();
	
 	cout << endl << "Creating a PageWise report to oofts14b.htm"<< endl;	
	ofstream fs2("oofts14b.htm");
	dbRepHTML tempReport2(dbRepSizer("Demo Page-wise Report").pageHeight(80),
					 dbRepColWidths() << 15 << 60,theView, dbRep::pageWise);
	tempReport2.draw(fs2);
	fs2.close();
	

 	cout << endl << "Creating a Columnar report to oofts14c.htm"<< endl;
	ofstream fs3("oofts14c.htm");
	dbRepHTMLTable tempReport3(dbRepSizer("Demo Columnar Report").pageHeight(80),
					dbRepColWidths() << 10 << 15 << 50, theView);
	tempReport3.draw(fs3);
 	fs3.close();
	
 	cout << endl << "Creating a PageWise report to oofts14d.htm"<< endl;
		ofstream fs4("oofts14d.htm");
	dbRepHTMLTable tempReport4(dbRepSizer("Demo Page-wise Report").pageHeight(80),
					 dbRepColWidths() << 15 << 60,theView, dbRep::pageWise);
	tempReport4.draw(fs4);
 	fs4.close();
	
	cout << "done" << endl;

	return EXIT_SUCCESS;
}       

