// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST60.h
// included in ooftest60 

// this sample Test sorting of char fields with buffer forever cache ond

// Simple stream I/O is used to interact with the user.


DECLARE_CLASS(dbPeople)
	dbChar		LastName, Othernames;
	dbLong		Salary, SalaryNoDup;
	dbShort		SalaryShort, SalaryShortNoDup;
	dbReal		SalaryReal, SalaryRealNoDup;
	dbDateTime	LastVisit, LastVisitNoDup;
	dbUshort	ShortUnsigned;
	dbUlong		LongUnsigned;

	dbPeople() :
				dbTable("People"),
				LastName(40, "Last Name"),
				Othernames(80, "Other names"),
				Salary("Salary"),
				SalaryNoDup("Salary NoDup"),
				SalaryShort("SalaryShort"),
				SalaryShortNoDup("SalaryShortNoDup"),
				SalaryReal("SalaryReal"),
				SalaryRealNoDup("SalaryRealNoDup"),
				LastVisit("LastVisit"),
				LastVisitNoDup("LastVisitNoDup"),
				ShortUnsigned("ShortUnsigned"),
				LongUnsigned("LongUnsigned")				
	{
#ifdef TEST_INDEXES
			LastName.index(kIndexed);
			Othernames.index(kIndexNoDups);
			Salary.index(kIndexed);
			SalaryNoDup.index(kIndexNoDups);
			SalaryShort.index(kIndexed);
			SalaryShortNoDup.index(kIndexNoDups);
			SalaryReal.index(kIndexed);
			SalaryRealNoDup.index(kIndexNoDups);
			LastVisit.index(kIndexed);
			LastVisitNoDup.index(kIndexNoDups);
			ShortUnsigned.index(kIndexed);
			LongUnsigned.index(kIndexed);				
#endif
	};
	
// my own data entry procedure
	void Add(const char *lname, const char *oname, const long salary, const char* visitDate);
};



void dbPeople::Add(const char *lname, const char *oname, const long salary, const char* visitDate)
{
	newRecord();
	LastName = lname;
	Othernames = oname;
	Salary = salary;
	SalaryNoDup = salary;
	SalaryReal = salary;
	SalaryRealNoDup = salary;
	SalaryShort = salary;
	SalaryShortNoDup = salary;
	LastVisit = visitDate;
	LastVisitNoDup = visitDate;
	
	ShortUnsigned = salary*5;  // numbers 10,000 to 49,999
	LongUnsigned = ULONG_MAX - salary;  
	
	saveRecord();
}

void testField(dbPeople& searchTable, dbNumericField& searchBy);
void testField(dbPeople& searchTable, dbChar& searchBy, const char* searchAgainst, const char* searchUpto, const char* searchAgainstPartial);
void testField(dbPeople& searchTable, dbDateTime& searchBy, const char* searchAgainst, const char* searchUpto);
void doTest07(const char* existsName,const char* databaseName);


void doTest60(const char* existsName,const char* databaseName)
{
	TEST_CONNECT    theDB;
	dbPeople     People;
	theDB.useSeparateFiles();

	if (dbConnect::fileExists(existsName)){
		theDB.openConnection(databaseName);
	}
	else {
		theDB.newConnection(databaseName);
		People.Add("Smith", "John", 2000, "23/11/94");
		People.Add("DENT", "Trissa", 9999, "22/11/94");
		People.Add("Denton", "Andy", 5000, "23-1-95");
		People.Add("Taylor", "Ken", 7500, "16,11,95");
		//david debug 000306 special yucky cases
		People.Add("Spoda", "zrrrh", 7501, "16,11,95");
		People.Add("Spodaab", "apple", 7502, "16,11,95");
		People.Add("Spodaaz", "Qworn", 7503, "16,11,95");
		People.Add("Spodz", "toe", 7504, "16,11,95");
		People.Add("Spodmno", "pohl", 7505, "16,11,95");
		People.Add("Spodcza", "acorn", 7506, "16,11,95");
	
		
		
	}
	// FIRST TEST THE ORIGONAL SORT ALGORITHM NO  BUFFER FOREVER
	//	CASE 1. ALL SELECTED
	//	CASE 2. SOME SELECTED
	// SECOND TEST 	TEST BUFFER FOREVER WITH NO DIRTY CACHE
	
	
	//	THEN TEST BUFFER FOREVER WITH DIRTY CACHE
	//	case 4. All selected
	//  CASE 5. Some selected
	// what buffer forever no dirty cACHE NO WILL JUST FALL INTO ORIGONAL BUT SHOULD STILL CHECK
	People.selectAll();
	People.setSortOrder(People.LongUnsigned); // to scramble the list
	cout << "CASE 1. all selected not buffer forever" << endl << endl;
	cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName << "  " << People.Othernames << "  " << People.LongUnsigned)  << endl;
	cout << endl;
	// now sort on lastname
	People.setSortOrder(People.LastName);
	People.selectAll();

	cout << "Dumping database in key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << People.LongUnsigned << " "  << endl ;
		cout << endl;
cout << endl << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;

	// now teston case = some selected
	cout << "CASE 2. some selected not buffer forever" << endl << endl;
	
	People.search(People.LastName.startsWith("Spod"));
		People.setSortOrder(People.LongUnsigned); // to scramble the list
		cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName << People.Othernames<< People.LongUnsigned) << endl;
	     cout << endl;
		
		cout << "Dumping database in key order by LastName"  << endl;
		People.setSortOrder(People.LastName);

		 for(People.start();People.more();People.next())
		cout << People.LastName << "  " << People.Othernames << "  "  << People.LongUnsigned  << endl;

		cout << endl;



cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;
	
	//
	People.setSaveOption(dbTable::bufferForever); 
	//no wtest with a dirty buffer
		cout << "CASE 3. All selected &  buffer forever NO dirty cache" << endl << endl;
	
	People.setSortOrder(People.LongUnsigned);
	People.selectAll();
	cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames  << People.LongUnsigned) << endl;
	
	cout << endl;
	People.setSortOrder(People.LastName);
	
	People.selectAll();
	//cout << "Dumping database in key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << " " <<  People.LongUnsigned << endl ;
		cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;


	
	//
	People.setSaveOption(dbTable::bufferForever); 
	//no wtest with a dirty buffer
		cout << "CASE 4. All selected &  buffer forever with dirty cache" << endl << endl;
	//dirty the cache
	People.search(People.Othernames=="toe");
	People.Othernames = "toetea";
	People.sortNow();
	People.selectAll(); //to test if lose cached chanages
	
	People.setSortOrder(People.LongUnsigned);
	
	cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames  << People.LongUnsigned) << endl << endl;
	
	People.setSortOrder(People.LastName);
	
	People.selectAll();
	//cout << "Dumping database in key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << " " <<  People.LongUnsigned  << endl;
		cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;

////////000000
	
	cout << "CASE 5. some selected &  buffer forever with dirty cache" << endl << endl;
	
	
	People.search(People.LastName.startsWith("Spod"));
	People.setSortOrder(People.LongUnsigned);
	
	cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames << People.LongUnsigned) << endl;
	cout << endl;
	People.setSortOrder(People.LastName);
	
		//cout << "Dumping database in key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName <<  " " <<  People.Othernames << " "  << People.LongUnsigned << endl;
	cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;
cout << endl << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;
cout << endl << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;
cout << endl << endl;
cout << "Now testing reverse sorts." << endl << endl;
	
	// Now check reverse ordersorts
	// FIRST TEST THE ORIGONAL SORT ALGORITHM NO  BUFFER FOREVER
	//	CASE 6. ALL SELECTED
	//	CASE 7. SOME SELECTED
	// SECOND TEST 	TEST BUFFER FOREVER WITH NO DIRTY CACHE
	
	
	//	THEN TEST BUFFER FOREVER WITH DIRTY CACHE
	//	case 9. All selected
	//  CASE 10. Some selected
	// what buffer forever no dirty cACHE NO WILL JUST FALL INTO ORIGONAL BUT SHOULD STILL CHECK
	
	People.setSaveOption(dbTable::requireExplicit); //will dump dirty cache
	
	People.selectAll();
	People.setReverseSortOrder(People.LongUnsigned); // to scramble the list
	cout << "CASE 6. all selected not buffer forever" << endl << endl;
	cout << "Dumping database in reverse key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName << "  " << People.Othernames << "  " << People.LongUnsigned)  << endl;
	cout << endl;
	// now sort on lastname
	People.setReverseSortOrder(People.LastName);
	People.selectAll();

	cout << "Dumping database in reverse key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << People.LongUnsigned << " "  << endl ;
		cout << endl;
cout << endl << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;

	// now teston case = some selected
	cout << "CASE 7. some selected not buffer forever" << endl << endl;
	
	People.search(People.LastName.startsWith("Spod"));
		People.setReverseSortOrder(People.LongUnsigned); // to scramble the list
		cout << "Dumping database in reverse key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName << People.Othernames<< People.LongUnsigned) << endl;
	     cout << endl;
		
		cout << "Dumping database in reverse key order by LastName"  << endl;
		People.setReverseSortOrder(People.LastName);

		 for(People.start();People.more();People.next())
		cout << People.LastName << "  " << People.Othernames << "  "  << People.LongUnsigned  << endl;

		cout << endl;



cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;
	
	//
	People.setSaveOption(dbTable::bufferForever); 
	//no wtest with a dirty buffer
		cout << "CASE 8. All selected &  buffer forever NO dirty cache" << endl << endl;
	
	People.setReverseSortOrder(People.LongUnsigned);
	People.selectAll();
	cout << "Dumping database in reverse key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames  << People.LongUnsigned) << endl;
	
	cout << endl;
	People.setReverseSortOrder(People.LastName);
	
	People.selectAll();
	//cout << "Dumping database in reverse key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in reverse reverse key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << " " <<  People.LongUnsigned << endl ;
		cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;


	
	//
	People.setSaveOption(dbTable::bufferForever); 
	//no wtest with a dirty buffer
		cout << "CASE 9. All selected &  buffer forever with dirty cache" << endl << endl;
	//dirty the cache
	People.search(People.Othernames=="toe");
	People.Othernames = "toetea";
	People.sortNow();
	People.selectAll(); //to test if lose cached chanages
	
	People.setReverseSortOrder(People.LongUnsigned);
	
	cout << "Dumping database in reverse key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames  << People.LongUnsigned) << endl << endl;
	
	People.setReverseSortOrder(People.LastName);
	
	People.selectAll();
	//cout << "Dumping database in reverse key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in reverse key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName << " " << People.Othernames << " " <<  People.LongUnsigned  << endl;
		cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;

////////000000
	
	cout << "CASE 10. some selected &  buffer forever with dirty cache" << endl << endl;
	
	
	People.search(People.LastName.startsWith("Spod"));
	People.setReverseSortOrder(People.LongUnsigned);
	
	cout << "Dumping database in reverse key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName  << People.Othernames << People.LongUnsigned) << endl;
	cout << endl;
	People.setReverseSortOrder(People.LastName);
	
		//cout << "Dumping database in reverse key order by LastName " << endl 
	//     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Dumping database in reverse key order by LastName " << endl;
	 for(People.start();People.more();People.next())
		cout << People.LastName <<  " " <<  People.Othernames << " "  << People.LongUnsigned << endl;
	cout << endl;
cout << "+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=" << endl;



	cout << "done" << endl;
}       
