// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST7.h
// included in ooftest7 and 17

// this sample demonstrates searching and sorting by 
// different types of fields, and how to parameterize
// calls, passing in table and field references

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


void doTest07(const char* existsName,const char* databaseName)
{
	TEST_CONNECT    theDB;
	dbPeople     People;

	if (dbConnect::fileExists(existsName)){
		theDB.openConnection(databaseName);
	}
	else {
		theDB.newConnection(databaseName);
		People.Add("Smith", "John", 2000, "23/11/94");
		People.Add("DENT", "Mildred", 9999, "22/11/94");
		People.Add("Denton", "George", 5000, "23-1-95");
		People.Add("Taylor", "Ken", 7500, "16,11,95");
		//david debug 000306 special yucky cases
		People.Add("Spod", "zrrrh", 7501, "16,11,95");
		People.Add("Spod", "apple", 7502, "16,11,95");
		People.Add("Spod", "Qworn", 7503, "16,11,95");
		People.Add("Spod", "toe", 7504, "16,11,95");
		People.Add("Spod", "pohl", 7505, "16,11,95");
		People.Add("Spod", "acorn", 7506, "16,11,95");
	
		
		
	}
// simple numeric test first, then use the parameterized versions
// Note the first few tests create temporary dbView's inline to output all records
// in a single cout << blah;

	People.selectAll();
	People.setSortOrder(People.LongUnsigned);
	
	cout << "Dumping database in key order by LongUnsigned (reverse of Salary)" << endl 
	     << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;

	cout << "Retrieving George Denton by ShortUnsigned == 25,000: " << endl;
	People[People.ShortUnsigned.equal(25000)];
	cout << (dbView(People) << People.LastName << People.Othernames << People.ShortUnsigned) << endl << endl;

	cout << "Finding LongUnsigned between " << ULONG_MAX-5001 << " and " << ULONG_MAX << endl;
	People[People.LongUnsigned.between(ULONG_MAX-5001, ULONG_MAX)];
	cout << (dbView(People) << People.LastName << People.Othernames << People.LongUnsigned) << endl << endl;


// test char searches
	testField(People, People.LastName, "Dent", "Denton", "DE");
	testField(People, People.Othernames, "George", "Kell", "J");

// test numeric searches
	testField(People, People.SalaryShort);
	testField(People, People.SalaryShortNoDup);
	testField(People, People.Salary);
	testField(People, People.SalaryNoDup);
	testField(People, People.SalaryReal);
	testField(People, People.SalaryRealNoDup);
	
// test date searches
	testField(People, People.LastVisit, "23/11/94", "23-1-95");
	testField(People, People.LastVisitNoDup, "23/11/94", "22-1-95");
	
	cout << "done" << endl;
}       

void testField(dbPeople& p, dbNumericField& searchBy)
{	
	p.selectAll();

// use a view to make it possible to dump just a few fields rather than the entire table
	dbView fewFields(p);
	fewFields << p.LastName << p.Othernames << searchBy;
	
	p.setSortOrder(searchBy);
	cout << "Dumping database in key order by " << searchBy.fieldName() << endl 
	     << fewFields << endl << endl;
	
	cout << "Retrieving George Denton by " << searchBy.fieldName() << " = 5000: " << endl;
	p[searchBy==5000];
	cout << fewFields << endl << endl;

	cout << "Retrieving John Smith by " << searchBy.fieldName() << " < 5000: " << endl;
	p[searchBy<5000];
	cout << fewFields << endl << endl;

	cout << "Retrieving Smith & George Denton by " << searchBy.fieldName() << " <= 5000: " << endl;
	p[searchBy<=5000];
	cout << fewFields << endl << endl;

	cout << "Retrieving Taylor & Mildred Dent by " << searchBy.fieldName() << " > 5000: " << endl;
	p[searchBy>5000];
	cout << fewFields << endl << endl;

	cout << "Same search with >=5000 (should also get George Denton): " << endl;
	p[searchBy>=5000];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " between 5000 and 7500 " << endl;
	p[searchBy.between(5000,7500)];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " outside 5000 to 7500 " << endl;
	p[searchBy.outside(5000,7500)];
	cout << fewFields << endl << endl;

	
	cout << "done" << endl;
}


void testField(dbPeople& p, dbChar& searchBy, const char* searchAgainst, const char* searchUpto, const char* searchAgainstPartial)
{	
	p.selectAll();

	dbView fewFields(p);
	fewFields << p.LastName << p.Othernames << searchBy;
	
	p.setSortOrder(searchBy);
	cout << "Dumping database in key order by " << searchBy.fieldName() << endl 
	     << fewFields << endl << endl;
	
	cout << "Finding " << searchBy.fieldName() << " starting with " << searchAgainstPartial << endl;
	p[searchBy.startsWith(searchAgainstPartial)];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " == " << searchAgainst << endl;
	p[searchBy==searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " < " << searchAgainst << endl;
	p[searchBy<searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " <= " << searchAgainst << endl;
	p[searchBy<=searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " > " << searchAgainst << endl;
	p[searchBy>searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " >= " << searchAgainst << endl;
	p[searchBy>=searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " between " 
		<< searchAgainst << " and " << searchUpto << endl;
	p[searchBy.between(searchAgainst,searchUpto)];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " outside " 
		<< searchAgainst << " to " << searchUpto << endl;
	p[searchBy.outside(searchAgainst,searchUpto)];
	cout << fewFields << endl << endl;

}


void testField(dbPeople& p, dbDateTime& searchBy, const char* searchAgainst, const char* searchUpto)
{	
	p.selectAll();
	
	dbView fewFields(p);
	fewFields << p.LastName << p.Othernames << searchBy;
	
	p.setSortOrder(searchBy);
	cout << "Dumping database in key order by " << searchBy.fieldName() << endl 
	     << fewFields << endl << endl;
	
	cout << "Finding " << searchBy.fieldName() << " == " << searchAgainst << endl;
	p[searchBy==searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " < " << searchAgainst << endl;
	p[searchBy<searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " <= " << searchAgainst << endl;
	p[searchBy<=searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " > " << searchAgainst << endl;
	p[searchBy>searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " >= " << searchAgainst << endl;
	p[searchBy>=searchAgainst];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " between " 
		<< searchAgainst << " and " << searchUpto << endl;
	p[searchBy.between(searchAgainst,searchUpto)];
	cout << fewFields << endl << endl;

	cout << "Finding " << searchBy.fieldName() << " outside " 
		<< searchAgainst << " to " << searchUpto << endl;
	p[searchBy.outside(searchAgainst,searchUpto)];
	cout << fewFields << endl << endl;

}

