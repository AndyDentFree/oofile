// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST31

// Simple stream I/O is used to interact with the user.
// to demonstrate the storage of blobs in a c-tree database

#include "ooftest.h"

DECLARE_CLASS(dbImageViewer)
        dbChar          FileName;
        dbDate	        FileModDate;
        dbChar          Description;
        dbBLOB          Thumbnail;

        dbImageViewer() : dbTable("ImagView") ,
                                FileName(50, "File Name"),
                                FileModDate("Mod Date"),
                                Description(100, "Description"),
                                Thumbnail("Thumbnail")
        {
                FileName.index(kIndexCompress);
                FileModDate.index(kIndexCompress);
                Description.index(kIndexCompress);
                setSortOrder(FileName);
        };
        
        void addTestData();
};

void
dbImageViewer::addTestData()
{
	long testBuf[] = {0, 2, 4, 8, 16, 65535, 0xADADADAD};
	long bufLen = sizeof(testBuf);
	long* copiedBuf = new long[bufLen/sizeof(long)];
	memcpy(copiedBuf, testBuf, bufLen);
	
// just add two records
	newRecord();
	FileName = "first blob";
	FileModDate = dbDate::currentDate();
	Description = "0, 2, 4, 8, 16, 65535, 0xADADADAD";
	Thumbnail.adoptBody(copiedBuf, bufLen);	// transfer ownership of this memory, not copy!
	saveRecord();
	
	newRecord();
	FileName = "second blob";
	FileModDate = dbDate::currentDate()+1;
	Description = "0, 2, 4, 8, 16, 65535, 0xADADADAD repeated";
	Thumbnail.setBytes(testBuf, bufLen);
// would cause assertion failure	Thumbnail += Thumbnail;
	Thumbnail.append(testBuf, bufLen);
	saveRecord();
	
}


int 
main()
{
	cout << "OOFILE Validation Suite - Test 31\n"
		 << "Store blobs in database"
		 << endl << endl;

	TEST_CONNECT    theDB;
	dbImageViewer     Images;	// not really - just arbitrary blobs for now!

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst31:ImagView.dbf";
			const char* kDatabaseName = ":ooftst31:";
		#else
			const char* kExistsName =   "ImagView.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst31.db";
		const char* kExistsName = kDatabaseName;
	#endif
	

	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);
		Images.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}
	Images.addTestData();
	
	cout << "Dump of entire database\n" << Images << endl;
	
	Images.start();
	Images.Thumbnail.setBytes("ABC", 3);
	Images.saveRecord();
	cout << "Modify BLOB in first record to 'ABC' (hex 41, 42, 43)\n" << Images << endl;
	
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

