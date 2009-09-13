// Copyright 1994 A.D. Software. All Rights Reserved

// included in ooftst16
// copy of ooftst02.h but without the PatientNo fields for the join relationships

DECLARE_REF(dbPatients)
DECLARE_SET(dbVisits)

DECLARE_CLASS(dbPatients)
	dbChar		LastName, Othernames;
	dbVisitsSet	Visits;
	dbLong		Salary;

	dbPatients() :
				LastName(40, "Last Name", kIndexed),
				Othernames(80, "Other names", kIndexed),
				Salary("Salary", kIndexed)
	{};
	
// my own data entry procedures
	void Add(const char *lname, const char *oname, const long salary);
	void AddVisit(const char* visitDate, const char* why);
	void AddTestData();
	
};


DECLARE_CLASS(dbVisits)
	dbPatientsRef	Patient;
	dbDate		VisitDate;
	dbChar		Why;
	dbVisits() : 
				VisitDate("VisitDate", kIndexed),
				Why(200, "Reason for Visit", kIndexCompress)
	{};
};


void dbPatients::Add(const char *lname, const char *oname, const long salary)
{
	newRecord();
	LastName = lname;
	Othernames = oname;
	Salary = salary;
	saveRecord();
}


void dbPatients::AddVisit(const char* visitDate, const char* why)
{
	Visits->newRecord();
	Visits->VisitDate = visitDate;
	Visits->Why = why;
}


void dbPatients::AddTestData()
{
// yes, if using SmartHeap debugging the following can take long enough to make folks wonder
// there are a *lot* of places where OOFILE calls SmartHeap to check memory
		cout << "Generating new test records..." << flush;
		Add("Smith", "John", 20000);
			AddVisit("1/10/1994", "Sore Knee");
			AddVisit("14/10/1994", "Measles");
		saveRecord();
		
		Add("DENT", "Mildred", 99999);
			AddVisit("23-11-1994", "Flu");
		saveRecord();

		Add("Dent", "George", 50000);
			AddVisit("4.10.1994", "Flu");
		saveRecord();

		Add("Taylor", "Ken", 75000);
		cout << endl << endl;
}

