//like ooftst02.h but vistis has a dbText field instead of dbChar field 
#include "oofios.h"

DECLARE_REF(dbPatients)
DECLARE_SET(dbVisits)

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
	dbDate		VisitDate;
	dbText		Why;
	dbVisits() : dbTable("Visits"),
				PatientNo("PatientNo", kIndexed),
				VisitDate("VisitDate", kIndexed),
				Why("Reason for Visit"/*, kIndexCompress*/)
	{
		Patient.joinField(PatientNo);
	};
};


void dbPatients::Add(const char *lname, const char *oname, const long salary)
{
	newRecord();
	LastName = lname;
	Othernames = oname;
	Salary = salary;
	PatientNo = (long) sequenceNumber();
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
		Add("Smith", "John", 20000);
			AddVisit("1/10/1994", "Sore Knee");
			AddVisit("14/10/1994", "Measles");
		saveRecord();
		
		Add("DENT", "Trissa", 99999);
			AddVisit("23-11-1994", "Flu");
			AddVisit("24-11-1994", "Hay fever");
		
		saveRecord();

		Add("Dent", "Andy", 50000);
			AddVisit("4.10.1994", "Flu");
			AddVisit("7.10.1994", "Tooth ache");
		saveRecord();

		Add("Taylor", "Ken", 75000);
}

