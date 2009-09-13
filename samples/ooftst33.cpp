// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST33

// Simple stream I/O is used to interact with the user.
// to demonstrate use of calculated fields and date format masks

#include "ooftest.h"

class overdueCalc : public dbBoolCalculator{
public:
   virtual dbCalculator* clone() const {return new overdueCalc(*this);};
   virtual bool	calc(const dbBool*);
};


class statusCalc : public dbCharCalculator{
public:
   virtual dbCalculator* clone() const {return new statusCalc(*this);};
   virtual const char*	calc(const dbChar*);
};



DECLARE_CLASS(dbInvoicing)
        dbUlong			InvoiceNumber;
        dbDate			DateDue;
        
        dbBool	        IsOverdue;
        dbChar          Status;

        dbInvoicing() : dbTable() ,
                                InvoiceNumber("Invoice Number"),
                                DateDue("Date Due"),
                                IsOverdue("Is Overdue"),
                                Status(0, "Status")	// use 0 length as doesn't matter for calc field
        {
        	IsOverdue.calculateWith(new overdueCalc);
        	Status.calculateWith(new statusCalc);
        	setSortOrder(dbSorter() << DateDue << InvoiceNumber);
        };
        
        void addTestData();
        void addInvoice(unsigned long invNumber, const char* due);
};


bool   
overdueCalc::calc(const dbBool* theField)
{
	dbInvoicing* theTable = (dbInvoicing*) (theField->fieldTable());	// get the table on which we operate
	long daysDue = oofDate("7th May 1996") - theTable->DateDue;
	bool ret = daysDue > 31;  
// normally would use a calculation like the following, but we've fixed a date
// to make our test results predictable
//	bool ret = (theTable->DateDue - dbDate::currentDate()) > 31;  
	return ret;
}


const char*   
statusCalc::calc(const dbChar* theField)
{
	dbInvoicing* theTable = (dbInvoicing*) (theField->fieldTable());	// get the table on which we operate
	if (theTable->IsOverdue)
		return "Overdue";
	else
		return "";
}


void 
dbInvoicing::addInvoice(unsigned long invNumber, const char* due)
{
	newRecord();
	InvoiceNumber = invNumber;
	DateDue = due;
	saveRecord();
}


void
dbInvoicing::addTestData()
{
	addInvoice(12345, "5th May 1996");
	addInvoice(12349, "5th May 1996");
	addInvoice(12346, "5th Feb 1996");
	addInvoice(12347, "4-1-1996");
	addInvoice(12348, "4-1-1996");
	addInvoice(12342, "4-1-1996");
	addInvoice(12343, "6th May 1996");
}


int 
main()
{
	cout << "OOFILE Validation Suite - Test 33\n"
		 << "Show date formatting and char and bool calculators"
		 << endl << endl;

	dbConnect_ram	theDB;
	dbInvoicing		Invoicing;	
	theDB.newConnection();
	
	Invoicing.addTestData();	
	Invoicing.DateDue.formatMask("ddth Month YYYY");
	cout << "All test records\n" << Invoicing;
	
// NOTE the following is for demonstrating the bool calculator
// a real database search would be better with an indexed search on the dbDate	
	cout << "\n\nSearching For Overdue records, by a calculated bool field\n";
	Invoicing.search(Invoicing.IsOverdue);

	Invoicing.DateDue.formatMask("Day, DD-MMM-YY");
	cout << Invoicing << endl;

	Invoicing.DateDue.formatMask("DD/MM/YYYY");
	cout << Invoicing << endl;

	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

