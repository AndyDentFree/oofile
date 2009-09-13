// included in ooftst2, 3, 4, 6, 9, 15, 26, 41
#include "oofios.h"

DECLARE_REF(dbPatients)
DECLARE_SET(dbVisits)
DECLARE_REF(dbVisits)
DECLARE_REF(dbVisitDate)

DECLARE_CLASS(dbPatients)
	dbChar		LastName, Othernames;
	dbVisitsSet	Visits;
	dbLong		PatientNo;
	dbLong		Salary;

	dbPatients() : dbTable("Patients"),
				LastName(40, "Last Name", kIndexed),
				Othernames(80, "Other names", kIndexed),
				PatientNo("PatientNo", kIndexNoDups),
				Salary("Salary", kIndexed)
	{
		Visits.joinField(PatientNo);
	};
	
// my own data entry procedures
	void Add(const char *lname, const char *oname, const long salary);
	void AddVisit(const char* visitDate, const char* why);
	void AddTestData();
	
};


DECLARE_CLASS(dbVisits)
	dbPatientsRef	Patient;
	dbLong		PatientNo;
//	dbDate		VisitDate;

	dbLong			visitNo;
	dbVisitDateRef	dateRef;

	dbChar		Why;
	dbVisits() : dbTable("Visits"),
				PatientNo("PatientNo", kIndexed),
			
				//VisitDate("VisitDate", kIndexed),
				visitNo("VisitNo", kIndexNoDups),

				Why(200, "Reason for Visit", kIndexCompress)
	{
		Patient.joinField(PatientNo);
		dateRef.joinField(visitNo);
	};
};

DECLARE_CLASS(dbVisitDate)
	dbVisitsRef	visit;
	dbDate		VisitDate;
	dbLong		visitNo;

	dbVisitDate() : dbTable("VisitDate"), 
		VisitDate("VisitDate", kIndexed)
//		,visitNo("VisitNo", kIndexed)
	{
		visit.joinField(visitNo);
	};
				
};

void dbPatients::Add(const char *lname, const char *oname, const long salary)
{
	newRecord();
	LastName = lname;
	Othernames = oname;
	Salary = salary;
	PatientNo = (long) sequenceNumber();

	// we need to save the main record because we need the PrimaryKey
	// to be used in creating new visit records.
	saveRecord();
}


void dbPatients::AddVisit(const char* visitDate, const char* why)
{
	Visits->newRecord();
//	Visits->VisitDate = visitDate;
	Visits->Why = why;
	Visits->visitNo = (long) Visits->sequenceNumber();
	saveRecord();

	Visits->dateRef->newRecord();
	Visits->dateRef->VisitDate = visitDate;
	Visits->dateRef->visitNo = Visits->visitNo;
}


void dbPatients::AddTestData()
{
// yes, if using SmartHeap debugging the following can take long enough to make folks wonder
// there are a *lot* of places where OOFILE calls SmartHeap to check memory
		Add("Smith", "John", 20000);
			AddVisit("1/10/1994", "Sore Knee");
		saveRecord();										//!!!???
			AddVisit("14/10/1994", "Measles");
		saveRecord();
		
		Add("DENT", "Trissa", 99999);
			AddVisit("23-11-1994", "Flu");
		saveRecord();

		Add("Dent", "Andy", 50000);
			AddVisit("4.10.1994", "Flu");
		saveRecord();

		Add("Taylor", "Ken", 75000);
}


