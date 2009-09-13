// included in ooftst42
#include "oofios.h"

DECLARE_SET(dbPeople)
DECLARE_REF(dbPeople)
DECLARE_SET(dbCompanies)
DECLARE_REF(dbCompanies)
DECLARE_SET(dbPurchaseLink)

/*
	This example shows the link table for the MN relationship as something private and 
	used just for the link. It of course may have a number of attributes of its own.

	We also show using a base class for a common ID attribute and override of newRecord;
*/


class baseTableWithID : public dbTable {
	OOFILE_METHODS(baseTableWithID)
protected:
	dbUlong				ID;
	baseTableWithID(const char* tblName) : dbTable(tblName),
		ID("ID", kIndexNoDups)
	{};
	virtual void newRecord();  // override that sets some fields
};


class dbPeople : public baseTableWithID {
	OOFILE_METHODS(dbPeople)
	dbChar		Name;
	dbCompaniesSet  buysFrom;
	dbPurchaseLinkSet	PLink;
	
public:
	dbPeople() : baseTableWithID("People"),
		Name(80, "Name", kIndexed)
	{
		buysFrom.joinField(ID);
		PLink.joinField(ID);
	};
		
// my own data entry procedures
	void add(const char *name);
	void linkToCompany(dbCompanies*, const char* CompanyName);  // link to existing
	void addTestData(dbCompanies*);
	
	void displayWithCompanies(ostream&);
};


class dbCompanies : public baseTableWithID {
	OOFILE_METHODS(dbCompanies)
	dbChar		Name;
	dbPeopleSet  purchaser;
	dbPurchaseLinkSet	PLink;
	
public:
	dbCompanies() : baseTableWithID("Companies"),
		Name(127, "Company Name", kIndexed)
	{
		purchaser.joinField(ID);
		PLink.joinField(ID);
	};

	unsigned long getID() const { return ID; };
	
// my own data entry procedures
	void add(const char *name);
	void addTestData();
};


// the link table that maps a given single record on each side
// we'll make its methods all private and access only through 
// friends on either side - this stops programmers fiddling 
// with the internals of relationships

// a future version of OOFILE would automatically generate this table
// so the only reason for you making a visible version would be if you
// needed extra attributes or to directly manipulate it

class dbPurchaseLink : public dbTable {
	OOFILE_METHODS(dbPurchaseLink)
	dbUlong				PersonID, CompanyID;
	dbPeopleRef	  		Person;
	dbCompaniesRef  	Company;
	
public:
	dbPurchaseLink() : dbTable("PurchaseLink"),
		PersonID("Person ID", kIndexed),
		CompanyID("Company ID", kIndexed)
	{
		Person.joinField(PersonID);
		Company.joinField(CompanyID);
	};
	
	friend class dbPeople;
};
	
	
// for complex databases it is usually best to have a single database
// class containing many tables. This also maps very well onto the Document model
// and is the pattern of code generated from AppMaker

// this database class shows how we can use an abstract base database connection
// so a runtime parameter to the database constructor could determine the actual
// type of database connection. This is a very common tecnique, eg: if you want
// to use the same database schema with both dBase and c-tree Plus

class CDatabase42 {
public:
	dbConnect*  	mDB;  // note that this is an abstract base connection type
			// the actual object could be varied by a parameter to the CDatabase42 ctor
	dbPeople*     	mPeople;
	dbCompanies*	mCompanies;
	dbPurchaseLink*	mLinker;

	CDatabase42();
	~CDatabase42();
	void openOrCreateDatabase(const char* kExistsName, const char* kDatabaseName);
	void dumpDatabase();
};	

// note the following methods are in this header for the convenience of our test
// programs - we don't normally advocate this layout, but would put them in
// another .cpp file.

void 
baseTableWithID::newRecord()
{
	dbTable::newRecord();
	ID = sequenceNumber();
}


//-------------------------------------
// d b P e o p l e
//-------------------------------------
void 
dbPeople::add(const char *name)
{
	newRecord();
	Name = name;
	saveRecord();
}


void 
dbPeople::linkToCompany(dbCompanies* company, const char *name)
{
// WARNING it's tempting to use buysFrom to get a set of Company records
// in this kind of routine, as it seems to be a straight dbCompanies*
// however, remember that busyFrom is a pointer to our set of related records
// for the current dbPeople record. Changing the state of buysFrom as shown
// below can have dangerous side-effects
//	buysFrom->search(buysFrom->Name==name);
	
	company->search(company->Name==name);
	if (!company->isEmpty()) {
		PLink->newRecord();
		PLink->PersonID = ID;
		PLink->CompanyID = company->getID();
		// we depend on a save being propagated from dbPeople
	}
	saveRecord();
}


void 
dbPeople::addTestData(dbCompanies* company)
{
	add("John Smith");
		linkToCompany(company, "Acme Ltd");
	add("Sally Adams");
		linkToCompany(company, "Acme Ltd");
		linkToCompany(company, "Wentworths");
	add("Phil Bloggs");
		linkToCompany(company, "Wentworths");
	add("Joan Whatnot");
	add("Zac Wayout");
		linkToCompany(company, "Bloggs Ltd");
}


void
dbPeople::displayWithCompanies(ostream& os)
{
	dbView relatedCompanies(buysFrom);
	relatedCompanies << buysFrom->Name;
	start(); 
	while (more()) {
		os << Name << endl;
		if (relatedCompanies.source()->isEmpty())
			os << "no companies" << endl;
		else 
			os << relatedCompanies << endl;
		os << endl;
		next();
	}
}


//-------------------------------------
// d b C o m p a n i e s
//-------------------------------------
void 
dbCompanies::add(const char *name)
{
	newRecord();
	Name = name;
	saveRecord();
}


void 
dbCompanies::addTestData()
{
	add("Acme Ltd");
	add("Wentworths");
	add("Bloggs Ltd");
}


//-------------------------------------
// C D a t a b a s e 4 2 
//-------------------------------------
CDatabase42::CDatabase42()   // would normally have a param determining type of connection
{
	mDB = new TEST_CONNECT;	// here we use the normal test macro instead of a switch
	mPeople = new dbPeople;
	mCompanies = new dbCompanies;
	mLinker = new dbPurchaseLink;
// the direct relationship	
	dbRelationship People_Companies(mPeople->buysFrom, mCompanies->purchaser);

// the supporting MN stuff
	dbRelationship People_PLink(mPeople->PLink, mLinker->Person);
	dbRelationship Company_PLink(mCompanies->PLink, mLinker->Company);
	People_Companies.linkMNvia(mLinker->Person, mLinker->Company);
}


CDatabase42::~CDatabase42()
{
	delete mPeople;
	delete mCompanies;
	delete mLinker;
	delete mDB;
}


void
CDatabase42::openOrCreateDatabase(const char* kExistsName, const char* kDatabaseName)
{
	if (dbConnect::fileExists(kExistsName)) {
		mDB->openConnection(kDatabaseName);
		mPeople->deleteAll();
		mCompanies->deleteAll();
	}
	else {
		mDB->newConnection(kDatabaseName);
	}
	mCompanies->addTestData();
	mPeople->addTestData(mCompanies);
	mPeople->setSortOrder(mPeople->Name);
}


void
CDatabase42::dumpDatabase()
{
	mDB->extract(cout);
	cout << endl << endl;

	cout << "Description of database schema: " << endl;
	mDB->describe(cout);
}