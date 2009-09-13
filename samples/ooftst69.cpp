// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST69

// This sample tests conditional indexing using a calculator.
// it is an extension of ooftst29.cpp

// Simple stream I/O is used to interact with the user.
#include "ooftest.h"

class OrderPriceCalculater : public dbUlongCalculator{
public:
   virtual dbCalculator* clone() const {return new OrderPriceCalculater(*this);}
   virtual unsigned long	calc(const dbUlong*);
};


class dbOrderItem : public dbTable {
   OOFILE_METHODS(dbOrderItem)
   dbReal   ItemPrice;
   dbUshort UnitsOrdered;
   dbUlong   TotalOrderPrice;  // will be virtual but because indexed gets stored in database
   dbBool	ItemSuspended;	// used to hide items.

   dbOrderItem() :
      ItemPrice("Item Price"),
      UnitsOrdered("Units Ordered"),
      TotalOrderPrice("Total Price", kIndexNoNulls)  // ***** NOTE THE no nulls clause!
   {
      TotalOrderPrice.calculateWith( new OrderPriceCalculater );
   };
   virtual void newRecord();
   void addTestData();
};



unsigned long   
OrderPriceCalculater::calc(const dbUlong* theField)
{
   dbOrderItem* theTable = (dbOrderItem*) (theField->fieldTable());	// get the table on which we operate
   if (theTable->ItemSuspended)
   	return 0;  // will hide items due to being indexed with no null
   const unsigned long ret = theTable->UnitsOrdered * theTable->ItemPrice;  
   return ret;
}


void
dbOrderItem::newRecord()
{
	dbTable::newRecord();
	ItemSuspended = false;
}

void
dbOrderItem::addTestData()
{
	deleteAll();
	
	newRecord();
	ItemPrice = 0.5;
	UnitsOrdered = 1000;
	saveRecord();

	newRecord();
	ItemPrice = 0.5;
	UnitsOrdered = 0;  // will end up hidden as no units
	saveRecord();

	newRecord();
	ItemPrice = 0.05;
	UnitsOrdered = 1000;
	saveRecord();

	newRecord();
	ItemPrice = 0.25;
	UnitsOrdered = 2000;
	ItemSuspended = true;  // will hide this one
	saveRecord();

	newRecord();
	ItemPrice = 12.79;
	UnitsOrdered = 100;
	ItemSuspended = true;  // will hide this one
	saveRecord();
}


int main()
{
	cout << "OOFILE Validation Suite - Test 69\n"
		 << "Use of calculated index to provide conditional indexing in queries and sorting" << endl << endl;
	
	dbConnect_ctree  theDB;  // indexing only available in ctree
	dbOrderItem  testItems;
	theDB.openOrCreateConnection("ooftst69.dat");
	
	testItems.addTestData();
	testItems.selectAll();
	
	cout << "Price, Qty & calculated Total Price:\n" 
		 << testItems << endl << endl;
	
	testItems.setSortOrder(testItems.TotalOrderPrice);
	cout << "Now list again, sorted by the calculated Total Price, notice missing items:\n" 
		 << testItems << endl << endl;

	testItems.search(testItems.TotalOrderPrice==500UL);
	cout << "Now list searching by the calculated Total Price = $500:\n" 
		 << testItems << endl << endl;
	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

