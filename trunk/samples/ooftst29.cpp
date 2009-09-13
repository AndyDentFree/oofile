// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST29

// This sample tests sort and query by method.

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

class OrderPriceCalculater : public dbRealCalculator{
public:
   virtual dbCalculator* clone() const {return new OrderPriceCalculater(*this);};
   virtual double	calc(const dbReal*);
};


class dbOrderItem : public dbTable {
   OOFILE_METHODS(dbOrderItem)
   dbReal   ItemPrice;
   dbUshort UnitsOrdered;
   dbReal   TotalOrderPrice;  // will be virtual

   dbOrderItem() :
      ItemPrice("Item Price"),
      UnitsOrdered("Units Ordered"),
      TotalOrderPrice("Total Price")
   {
      TotalOrderPrice.calculateWith( new OrderPriceCalculater );
   };
   
   void addTestData();
};



double   
OrderPriceCalculater::calc(const dbReal* theField)
{
   dbOrderItem* theTable = (dbOrderItem*) (theField->fieldTable());	// get the table on which we operate
   double ret = theTable->UnitsOrdered * theTable->ItemPrice;  
   return ret;
}


void
dbOrderItem::addTestData()
{
	newRecord();
	ItemPrice = 0.5;
	UnitsOrdered = 1000;
	saveRecord();

	newRecord();
	ItemPrice = 0.05;
	UnitsOrdered = 1000;
	saveRecord();

	newRecord();
	ItemPrice = 0.25;
	UnitsOrdered = 2000;
	saveRecord();

	newRecord();
	ItemPrice = 12.79;
	UnitsOrdered = 100;
	saveRecord();
}


int main()
{
	cout << "OOFILE Validation Suite - Test 29\n"
		 << "Use of methods in queries and sorting" << endl << endl;
	
	dbConnect_ram  theDB;
	dbOrderItem  testItems;
	theDB.newConnection();
	
	testItems.addTestData();
	
	cout << "Price, Qty & calculated Total Price:\n" 
		 << testItems << endl << endl;
	
	testItems.setSortOrder(testItems.TotalOrderPrice);
	cout << "Now list again, sorted by the calculated Total Price:\n" 
		 << testItems << endl << endl;

	testItems.search(testItems.TotalOrderPrice==500);
	cout << "Now list searching by the calculated Total Price = $500:\n" 
		 << testItems << endl << endl;

// test that calculators clone correctly
// our calculator above must point to the new table
	dbOrderItem anotherIterator = testItems;
	anotherIterator.selectAll();
	anotherIterator.setSortOrder(anotherIterator.UnitsOrdered);
	cout << "Now list a cloned iterator, to check calculators clone correctly:\n" 
		 << anotherIterator << endl << endl;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

