// Copyright 1995 A.D. Software. All Rights Reserved

// OOFTEST12

// This sample tests the compound indexes by creating a single table
// and storing and retrieving indexed data.

// Simple stream I/O is used to interact with the user.

#include "ooftest.h"

DECLARE_CLASS(dbPeople)
	dbChar		LastName, OtherNames, DepartmentCode, ProjectCode;
	dbLong		Salary;
	dbCompoundField	CombinedNames, NameAndSalary, JobCode;
	dbUshort	StartedYear;
	

	dbPeople() :
				dbTable("People"),
				LastName(39, "Last Name"),
				OtherNames(79, "Other Names", kIndexCompress),
				DepartmentCode(3, "Dept. Code"), 
				ProjectCode(4, "Proj. Code"),
				Salary("Salary"),
				CombinedNames("Combined Names"),
				NameAndSalary("Name & Salary"),
				JobCode("Job Code"),
				StartedYear("Started Year")
				
{
	CombinedNames << LastName << OtherNames;
	CombinedNames.index(kIndexNoDups);  // order of these two lines is not significant
	
	NameAndSalary << LastName >> Salary;
	NameAndSalary.index(kIndexCompress);  
	
	JobCode << DepartmentCode << ProjectCode << StartedYear;
	JobCode.index(kIndexed);
};
	
// my own data entry procedures
	void Add(const char *lname, const char *oname, const long salary, const char *dept, const char *proj, const unsigned short year);
};

void dbPeople::Add(const char *lname, const char *oname, const long salary, const char *dept, const char *proj, const unsigned short year)
{
	newRecord();
	LastName = lname;
	OtherNames = oname;
	Salary = salary;
	DepartmentCode = dept;
	ProjectCode = proj;
	StartedYear = year;
	saveRecord();
}



int main()
{
	cout << "OOFILE Validation Suite - Test 12\n"
		 << "Simple test to build compound indexes\n"
		 << "and demonstrate sorting and searching by them and their components\n";
	
	dbConnect_ctree    theDB;
	dbPeople     People;
	
	if (dbConnect::fileExists("ooftst12.db"))
		theDB.openConnection("ooftst12.db");
	else {
		theDB.newConnection("ooftst12.db");
		People.Add("Smith", "John", 0, "Acc", "XXX", 1970);  // specifically to test zero searches
		People.Add("DENT", "Mildred", 5000, "Acc", "Rec", 1985);
		People.Add("Dent", "George", 25000, "Acc", "XXX", 1992);
		People.Add("Dent", "Andrew", 40000, "MIS", "Boss", 1982);  // should sort before George on CombinedNames but after on Salary
		People.Add("Denton", "Andrew", 39000, "Pur", "XXX", 1994);  
		People.Add("Taylor", "Ken", 75000, "MIS", "Cntr", 1994);
	}
	
	People.selectAll();
	People.setSortOrder(People.CombinedNames);
	cout << "Listing records by combined names\n" << People << endl;

	People.setSortOrder(People.NameAndSalary);
	cout << "Listing records by Name and reverse Salary\n" << People << endl;

	People.setSortOrder(People.OtherNames);
	cout << "Listing records in OtherNames order\n" << People << endl;

	People.search(People.LastName=="Dent");  // uses compound index
	cout << "Listing three Dent records: " << endl << People << endl;

	People.search(People.JobCode.startsWith("AccXXX"));  // uses compound index in direct query
	cout << "Listing two AccXXX records: " << endl << People << endl;

	People[People.LastName=="Dent" && People.Salary==25000];
	cout << "Listing George Dent by compound key search of LastName & Salary: " << endl << People << endl;
		
	People[People.LastName=="Dent" && People.Salary>=25000];
	cout << "Listing George or Andrew Dent by compound key search of LastName & Salary>=: " << endl << People << endl;

	People[People.Salary>=25000 && People.LastName=="Dent"];// david debug 00020
	cout << "Listing George or Andrew Dent by compound key search of LastName & Salary, swapped clause order" << endl << People << endl;

	
	People[People.LastName=="Dent" && People.OtherNames=="Andrew"];
	cout << "Listing Andrew Dent by compound key search on both names: " << endl << People << endl;

// combinatorial search
	People[ (People.LastName=="Dent" && People.Salary==25000) 
			| People.OtherNames=="Andrew"
	];
	cout << "Listing George Dent & Denton: " << endl << People << endl;

	cout << "Test Getting copy from read-only compound field" << endl;
	People.start();
	oofString str = People.JobCode.copyString();
	cout << "JobCode is: " << str << endl;

	dbPeople People2(People);
	People2.start();  // this or a search is ***essential*** after cloning a selection
	cout << endl << "Repeat above read-only compound field test on cloned selection" << endl;
	cout << "JobCode is: " << People2.JobCode << endl;

	cout << endl << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

