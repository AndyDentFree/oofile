// included in ooftest1, 13, 14, 15, 18, 20, 24, 25, 37 and 47

DECLARE_CLASS(dbPeople)
	dbChar		LastName, OtherNames;
	dbLong		Salary;
	dbText		Description;
	dbDate		LastPaid;

#ifndef demoNoIndexes
	dbPeople() :
				dbTable("People"),
				LastName(39, "Last Name", kIndexCompress),
				OtherNames(79, "Other Names", kIndexCompress),
				Salary("Salary", kIndexed),
				Description("Description"),
				LastPaid("Last Paid", kIndexed)
				{};
#else
	dbPeople() :
				dbTable("People"),
				LastName(39, "Last Name"),
				OtherNames(79, "Other Names"),
				Salary("Salary"),
				Description("Description"),
				LastPaid("Last Paid", kIndexed)
				{};
#endif
	
// my own data entry procedures
	void Add(const char *lname, const char *oname, const long salary);
	void AddTestData();
};

void dbPeople::Add(const char *lname, const char *oname, const long salary)
{
	newRecord();
	LastName = lname;
	OtherNames = oname;
	Salary = salary;
	LastPaid.setDateToCurrentDate();
	saveRecord();
}


void dbPeople::AddTestData()
{
	Add("Smith", "John", 0);  // specifically to test zero searches
	Description = "John is a plain sort of bloke, not the kind to stand-out in a crowd\n";
	Description += "and in fact you'd probably say he's the classic Mr Average. However ";
	Description += "he harbours secret dreams of being a brain surgeon and a bloke 'wot ";
	Description += "goes down the sewers in big rubber boots'\n";
	LastPaid -= 21;  // 3 weeks ago
	saveRecord();

	Add("Dent", "Mildred", 5000);
	Description = "Mildred was married to George and is mother of Annalise and Berkely";
	saveRecord();

	Add("DENT", "George", 25000);// david 001026 Dent to DENT for debug ease
	Description = "George has a Don Quixote complex but his Sancho ";
	Description += "Panza stops him tilting at too many software windmills";
	LastPaid -= 7;  // 1 week ago
	saveRecord();

	Add("Taylor", "Ken", 75000);
}

