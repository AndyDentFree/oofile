// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST53

#include "ooftst53.h"
#include "oofexcep.h"

// -------------------------------------------------------
//             d b P a r t y
// -------------------------------------------------------

/**
	quick add function mainly for test data.
	\param inAddress and other details will propagate to related tables later
*/
void 
dbParty::add(
	const char* inName, 
	const char* inAddress, 
	const char* inPhone, 
	const char* inPostCode)
{
	newRecord();
	Name = inName;
	Address = inAddress;
	Phone = inPhone;
	PostCode = inPostCode;
}


// -------------------------------------------------------
//             d b P e r s o n
// -------------------------------------------------------

/**
	quick add function mainly for test data.
	overrides dbParty::add()
	\param inEmail is added to provide further details
*/
void 
dbPerson::add(
	const char* inName, 
	const char* inEmail, 
	const char* inAddress, 
	const char* inPhone, 
	const char* inPostCode)
{
	dbParty::add(inName, inAddress, inPhone, inPostCode);
	Email = inEmail;
}




// -------------------------------------------------------
//             d b R o l e
// -------------------------------------------------------

/**
	AddRole adds role in context of relationship.
	it is meant to be called on related tables only by Person
	so our person relationship is taken care of automatically by OOFILE.
*/
void 
dbRole::addRole(const char* inDesc, const char* inOptionalOrg)
{
	assert(isRelatedClone());
	assert(inDesc);
	newRecord();
	Desc = inDesc;
	if (inOptionalOrg)	{
/*
		if (Org->search(Org->Name==inOptionalOrg))
 			Org->start();  // load record to get the OID!
			Org.setOID(Org->currentOID());
*/
		Org.setRelatedRecordMatching(Org->Name==inOptionalOrg);
	}
	// caller will save as cascade when they do
}



void
dbOrg::addTestData()
{	
	add("Acme Inc.");
	saveRecord();
	add("Acme Widgets Inc.");
	saveRecord();
	add("Bloggs Enterprises");
	saveRecord();
	add("Highway1 Pty Ltd");
	saveRecord();
}

	
void
dbPerson::addTestData()
{	
	add("Bill Bloggs");
		Roles->addRole("Individual");
	saveRecord();	
	
	add("Bill Smith", "", "", "555 9999");
		Roles->addRole("Individual");
	saveRecord();	
	
	add("Jan Shadders", "", "12 Anywhere Lane, Yokine", "555 1234", "6060");
		Roles->addRole("Individual");
	saveRecord();	
	
	add("Jack Aunty", "", "9 Bill St\nYokine", "555 1234", "6060");
		Roles->addRole("Individual");
	saveRecord();	
	
	add("Jan Bloggs", "jan@bloggs.net.au");
		Roles->addRole("Individual");
	saveRecord();	
	
	add("William Bloggs III");
		Roles->addRole("Individual");
		Roles->addRole("Webmaster", "Bloggs Enterprises");
	saveRecord();	
	
	add("Sally Waters");
		Roles->addRole("System Manager", "Bloggs Enterprises");
	saveRecord();	
	
	add("TANG lee", "tang@acme.com.au");
		Roles->addRole("Webmaster", "Acme Widgets Inc.");
		Roles->addRole("System Manager", "Acme Inc.");
	saveRecord();	
}	



int main()
{
	cout << "OOFILE Validation Suite - Test 53\n"
		 << "Simple test to store some data and retrieve it" << endl
		 << "using a relation joining over a field and showing" << endl
		 << "iterators on related tables and 1-many relations" << endl << endl;

	try {
		TEST_CONNECT    theDB;
		dbRole			Role;
		dbPerson		Person;
		dbOrg			Org;
		
	// declare relationships with temporary objects
	// next rev of OOFILE will make most of this unnecessary as relationships
	// will be resolved at new/openConnection time unless there is more than one
	// relationship between two tables	
		dbRelationship( Person.Roles, Role.Person );
		dbRelationship( Org.Roles, Role.Org );
		
	// MN relationship directly between Person and Org uses the two above
	// A single Person may relate to many Orgs and vice versa	
		dbRelationship PersonOrg( Person.Orgs, Org.People );
		PersonOrg.linkMNvia( Role.Person, Role.Org );
		
		
		#ifdef TESTING_DBASE
			#ifdef _Macintosh
				const char* kExistsName =  ":ooftst53:Person.dbf";
				const char* kDatabaseName = ":ooftst53:";
			#else
				const char* kExistsName =   "Person.dbf";
				const char* kDatabaseName = "";
			#endif	
	
		#else
			const char* kDatabaseName = "ooftst53.db";
			const char* kExistsName = kDatabaseName;
		#endif
		
		if (dbConnect::fileExists(kExistsName)) {
			theDB.openConnection(kDatabaseName);
		}
		else {
			theDB.newConnection(kDatabaseName);
			Org.addTestData();
			Person.addTestData();
		}
	
		cout << theDB;
		
		// specific output to show relationship traversal working
		// note shorthand use of [] to do a search
		Person[Person.Name=="Sally Waters"];
		cout << endl << "Sally Waters relates to Orgs" << endl;
		Person.Orgs->extract(cout);
		
		Person[Person.Name=="TANG lee"];
		cout << endl << "TANG lee relates to Orgs" << endl;
		Person.Orgs->extract(cout);
		
		Person.search(Person.Name=="Jan Shadders");
		cout << endl << "Jan Shadders shouldn't have any Org" << endl;
		Person.Orgs->extract(cout);

		Person[Person.hasWord("Bill")];  
		cout << endl << endl << "People matching 'Bill' somehow" << endl 
			<< Person << endl;
		
	}
	catch (oofE_Base& whyOOFfailed) {
		std::cout << "OOFILE Error" << endl;
		whyOOFfailed.extract(std::cout);
	}	
	cout << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

	