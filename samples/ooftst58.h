//like ooftst02.h but visits has a dbText field instead of dbChar field 
#include "oofios.h"

DECLARE_REF(dbPatients)

DECLARE_CLASS(dbPatients)
	dbChar		LastName, Othernames;
	dbLong		PatientNo;
	dbLong		Salary;
	dbText		MemoF;

	dbPatients() : dbTable("Patients"),
				LastName(40, "Last Name", kIndexed),
				Othernames(80, "Other names", kIndexed),
				PatientNo("PatientNo", kIndexNoDups),
				Salary("Salary", kIndexed),
				MemoF("Reason for Visit"/*, kIndexCompress*/)
			{};
				
	
// my own data entry procedures
	void Add(const char *lname, const char *oname, const long salary, const char *);
	void AddTestData();
	
};




void dbPatients::Add(const char *lname, const char *oname, const long salary, const char * aMemo)
{
	newRecord();
	LastName = lname;
	Othernames = oname;
	Salary = salary;
	PatientNo = (long) sequenceNumber();
	MemoF = aMemo;
	saveRecord();
}




void dbPatients::AddTestData()
{
// yes, if using SmartHeap debugging the following can take long enough to make folks wonder
// there are a *lot* of places where OOFILE calls SmartHeap to check memory
		Add("Smith", "John", 20000, "lippy loopy triangles");
		
		Add("DENT", "Trissa", 99999, "she sells see shells by the see shore");
			
	
		Add("Dent", "Andy", 50000, "Boss");
		
		Add("Spod", "thrinnn", 100,"");
		Add("Taylor", "Ken", 75000, "Web man not spider");
			
}


#define kHugeString "This is a huge string, to test blob handling it has > 512 bytes and should use two blocks of dbt storage aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqwertyuiop splat the quick brown fox jumps over the lazy dog1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog ! + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? qwertyuiop splat the quick brown fox jumps over the lazy dog 1 + 1 = 2 2 + 2 = 4 + 1 = 5 ~!@#$%^&*()_+`1234567890[]{}\|<>? "
