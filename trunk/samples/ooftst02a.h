// included in ooftst2, 3, 4, 6, 9, 15, 26, 41
#include "oofios.h"

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

DECLARE_SET(dbFood)
DECLARE_REF(dbCustomer)

DECLARE_REF(dbFood)
DECLARE_REF(SaladData)
DECLARE_REF(MainCourseData)
DECLARE_REF(DessertData)

enum FoodDataType
{
   kSalad,
   kMainCourse,
   kDessert
};

DECLARE_CLASS(dbFood)
	dbCustomerRef Customer;
	dbChar		Name;
	dbUlong     CustomerNumber;
	dbUlong		FoodId;
	dbUlong		Cost;

   dbUshort    fFoodDataType;
   dbUlong     fFoodDataId;
   SaladDataRef      fSaladData;
   MainCourseDataRef fMainCourseData;
   DessertDataRef    fDessertData;

	dbFood() : dbTable("Food"),
				Name(40, "Food Name", kIndexed),
				FoodId("FoodId", kIndexNoDups),
				Cost("Cost", kIndexed),
            CustomerNumber("CustomerNumber")
	{
//		Customer.joinField(CustomerNumber);
      fSaladData.joinField(fFoodDataId);
      fMainCourseData.joinField(fFoodDataId);
      fDessertData.joinField(fFoodDataId);
	};
	
// my own data entry procedures
	void Add(const char *foodname, unsigned long cost, FoodDataType foodType);
	void AddCustomer(const char* name, const char* phone);
	void AddTestData();
	
};


DECLARE_CLASS(dbCustomer)
	dbFoodSet	Food;
	dbUlong		CustomerNum;
   dbChar      CustName;
	dbChar		phoneNumber;

	dbCustomer() : dbTable("Customer"),
				CustomerNum("CustomerNum", kIndexed),
				CustName(60, "CustName", kIndexed),
				phoneNumber(20, "phoneNumber")
	{
//		Food.joinField(CustomerNum);
	};

	virtual void newRecord()
	{
		dbTable::newRecord();
		CustomerNum = sequenceNumber();
	}
};

DECLARE_CLASS(SaladData)
   dbUlong  fId;
   dbChar   fColor;
   dbChar   fLettuceSource;
	dbFoodRef fFood;

   SaladData() : dbTable("SaladData"),
    fId("Id"),
    fColor(20, "Color"),
    fLettuceSource(60, "LettuceSource")
   {
      fFood.joinField(fId);
   }
};

DECLARE_CLASS(MainCourseData)
   dbUlong  fId;
   dbChar   fPlateStyle;
   dbChar   fSpiceLevel;
	dbFoodRef fFood;

   MainCourseData() : dbTable("MainCourseData"),
    fId("Id"),
    fPlateStyle(40, "PlateStyle"),
    fSpiceLevel(40, "SpiceLevel")
   {
      fFood.joinField(fId);
   }
};

DECLARE_CLASS(DessertData)
   dbUlong  fId;
   dbChar   fTemperature;
   dbChar   fSweetnesCategory;
	dbFoodRef fFood;

   DessertData() : dbTable("DessertData"),
    fId("Id"),
    fTemperature(40, "Temperature"),
    fSweetnesCategory(40, "SweetnesCategory")
   {
      fFood.joinField(fId);
   }
};

void dbFood::Add(const char *foodname, unsigned long cost, FoodDataType foodType)
{
	newRecord();
	Name = foodname;
	Cost = cost;
	FoodId = sequenceNumber();

   fFoodDataType = foodType;
   switch (foodType)
   {
      case kSalad: 
      fSaladData->newRecord();
      fFoodDataId = fSaladData->fId = fSaladData->sequenceNumber();
      fSaladData->fColor = "Green";
      fSaladData->fLettuceSource = "Jim's Greens";
      break;

      case kMainCourse:
      fMainCourseData->newRecord();
      fFoodDataId = fMainCourseData->fId = fMainCourseData->sequenceNumber();
      fMainCourseData->fPlateStyle = "Normal";
      fMainCourseData->fSpiceLevel = "Normal";
      break;

      case kDessert:
      fDessertData->newRecord();
      fFoodDataId = fDessertData->fId = fDessertData->sequenceNumber();
      fDessertData->fTemperature = "Hot";
      fDessertData->fSweetnesCategory = "Customer Sweetened";
      break;
   }

	saveRecord();
}


void dbFood::AddCustomer(const char* name, const char* phone)
{
   if (!Customer .setRelatedRecordMatching (Customer->CustName == name))  //*1
   {
      Customer->newRecord();
      CustomerNumber = Customer->CustomerNum = Customer->sequenceNumber(); // *2
      Customer->CustName = name;
      Customer->phoneNumber = phone;
   }
	return;
#if 1
 	dbCustomer* globalTable = (dbCustomer*)(Customer->prototype());  // ugly cast will go away in next OOFILE
   globalTable->search(globalTable->CustName == name);
   if (globalTable->count() == 0)
   {
	   Customer->newRecord();
      CustomerNumber = Customer->CustomerNum = Customer->sequenceNumber();
	   Customer->CustName = name;
	   Customer->phoneNumber = phone;
   }
   else
   {
  /*
  	accessing Customer->CustomerNum; will call OOF_simpleRecordBackend::getFieldReadFrom
  	that in turn invokes dbTable::ensureRecordLoaded to load the actual instance
  	and so whatever you found by a search above is now flushed.
  	*/
	   CustomerNumber = globalTable->CustomerNum;
   }
#else
   Customer->search(Customer->CustName == name);
   if (Customer->count() == 0)
   {
	   Customer->newRecord();
      Customer->CustomerNum = Customer->sequenceNumber();
	   Customer->CustName = name;
	   Customer->phoneNumber = phone;
   }
   
   // Why does the next line only get a non-zero value when the if
   // expression above is true and the new record code above is executed?

   cout << Customer->CustName;
   unsigned long cn = Customer->CustomerNum;
   CustomerNumber = cn;
#endif
}


void dbFood::AddTestData()
{
		Add("Eggs", 1500, kMainCourse);
			AddCustomer("Ron", "769-8449");
		saveRecord();
		
		Add("Pots", 450, kMainCourse);
			AddCustomer("Tim", "697-1234");
		saveRecord();

		Add("Spam", 395, kMainCourse);
			AddCustomer("Ron", "769-8449");
		saveRecord();

		Add("Coffee", 150, kDessert);
			AddCustomer("Ron", "769-8449");
		saveRecord();

		Add("Parsley", 70, kSalad);
			AddCustomer("Tom", "323-2205");
		saveRecord();

		Add("Sage", 70, kSalad);
			AddCustomer("Tim", "697-1234");
		saveRecord();
}

