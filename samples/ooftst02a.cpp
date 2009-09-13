// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST2

// This sample tests the database backend by creating a pair of tables
// and storing and retrieving indexed data via traversal paths.

// Simple stream I/O is used to interact with the user.
#include "oofile.h"

#include "ooftst02a.h"

// global variables that define the database using the ooftst02 classes
class ATestDb
{
public:

	dbConnect_ctree    theDB;
	dbFood     gDbFoods;
	dbCustomer	gDbCustomers;
   SaladData   gDbSaladData;
   MainCourseData gDbMainCourseData;
   DessertData gDbDessertData;


   ATestDb();
   ~ATestDb();
};

ATestDb::ATestDb()
{
	dbRelationship(gDbFoods.Customer, gDbCustomers.Food);  // reverse this or, prevent propagation
	gDbFoods.Customer.propagateRelatedDeletes(false);
	
	dbRelationship(gDbFoods.fSaladData, gDbSaladData.fFood);
	dbRelationship(gDbFoods.fMainCourseData, gDbMainCourseData.fFood);
	dbRelationship(gDbFoods.fDessertData, gDbDessertData.fFood);

   const char* kDatabaseName = "foods02.db";
	const char* kExistsName = kDatabaseName;

   if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		gDbFoods.deleteAll();
		gDbFoods.AddTestData();
	}
	else {
		theDB.newConnection(kDatabaseName);
		gDbFoods.AddTestData();
	}
}

ATestDb::~ATestDb()
{
   theDB.close();
}

int main()
{

   cout << "Ed's test program" << endl << endl;
   {	
      ATestDb theDB;

	   theDB.gDbFoods.setSortOrder(theDB.gDbFoods.FoodId);
	   theDB.gDbCustomers.setSortOrder(theDB.gDbCustomers.CustomerNum);
//	   cout << theDB.theDB;
   }
   if (0) {
      ATestDb theDB;  // exception occurs in deleteAll called by ATestDb ctor

	   cout << theDB.theDB;
      theDB.gDbFoods.search(theDB.gDbFoods.Customer->CustName == "Ron");
     cout << "Finding " << theDB.gDbFoods.count() << " Customer == Ron records: " << endl << theDB.gDbFoods << endl;
   }
#ifdef OOF_MEM_DEBUG_REPORT_LEAKS
	OOF_MEM_DEBUG_REPORT_LEAKS
#endif
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

 