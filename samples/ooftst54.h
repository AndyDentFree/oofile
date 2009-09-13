// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST54

/**
	Simple stream I/O is used to interact with the user.
	to demonstrate complex OOFILE M:N pointer relationships
	and how to use subclassing - Party provides the data
	and Person and Org the relationships with each other.
	this sample is also marked up with Doxygen-compatible comments
	
	Copy of ooftst53 extended to provide additional related Role Description
	as bugs have been encountered in adding related data 2 levels down.
	
	This example also shows use of a more generic lookup table which
	would often have multiple instances in a database. Thus, users
	of the lookup table have a typed dbLookupRef pointing at it but
	the generic dbRelSet is used in the other side.	
*/
#ifndef H_ooftst54
#define H_ooftst54

#include "ooftest.h"

class dbLookup; // forward

/**
	Declare relationship class for usrs of dbLookup.
	Unlike the normal convention we don't declare a paired relationship class
	but will use the generic base dbRelSet as the dbLookupDesc instance will
	be related back to any one of a number of table classes.
	
	\note how we add a method addOrFind invoked directly on the relationship
*/
class dbLookupRef : public dbRelRef {
	REL_REF_BODY(dbLookup) 
	
	void addOrFind(const char* inLookupDesc);
};


/**
	dbLookup provides lookup tables for separately maintained descriptions.
	Several instances of this table might exist in a database.
	
	Note that the name of the Desc field may be overriden by the user
	\see dbField::setName
	
	The width of the description field is oriented toward simple GUI descriptions
	where it might be used in a popup or scrolling list.
*/
class dbLookup : public dbTable {
	OOFILE_METHODS(dbLookup)
	dbChar		ShortDesc;
	dbRelSet	LookupUser; ///> generic user of lookup table
	
	dbLookup() :
		ShortDesc(30, "Description", kIndexCompressPaddingNoDups)
	{};
};

/**
	dbParty represents an actual person or organisation.
*/
class dbParty : public dbTable {
	ABSTRACT_BASE_METHODS(dbParty)
	
/// @name database fields
//@{
	dbChar		Name;
	dbChar		Email;
	dbChar		Phone;
	dbChar		PostCode;
	dbText		Address;
//@}

	dbParty() :
		Name(200, "Name", kIndexCompress),
		Phone(40, " Phone Number", kIndexCompressPadding),
		PostCode(12, " PostCode", kIndexCompressPadding),
		Address(" Address")
	{
		Name.indexWords();  // also indexed normally for fast sorting & startsWith search
		Address.indexWords();
	}

/// @name data manipulation functions
//@{
	void add(
		const char* inName, 
		const char* inAddress=0, 
		const char* inPhone=0, 
		const char* inPostCode=0
	);
//@}
};


/**
	Generate classes used in the type-safe relationship declarations in dbTables.
	eg: dbPerson relating via dbRoleSet Roles to dbRole.

*/
DECLARE_REF(dbPerson)
DECLARE_SET(dbPerson)

DECLARE_REF(dbRole)
DECLARE_SET(dbRole)

DECLARE_REF(dbOrg)
DECLARE_SET(dbOrg)


/**
	dbPerson represents an actual person or named position.
	
	We don't use the term Client because that implies a
	relationship which may not exist. Person is neutral.
	
	We use a single dbParty::Name field which caters for
	likely Asian format names written in reverse.
	
	\todo add more personal details?
*/
class dbPerson : public dbParty {
	OOFILE_METHODS(dbPerson)
/// @name database fields
//@{
	dbChar		Email;  // add one field to make it easier to see records!
//@}

/// @name relationships	
//@{
	dbRoleSet	Roles;
	dbOrgSet	Orgs;
//@}

	dbPerson() :
		Email(200, " Email Address", kIndexCompress)
	{
		Name.setName("Person Full Name");  
	}

/// @name data manipulation functions
//@{
	void add(
		const char* inName, 
		const char* inEmail=0, 
		const char* inAddress=0, 
		const char* inPhone=0, 
		const char* inPostCode=0
	);
	void addTestData();
//@}
};


/**
	dbOrg represents an external Org.
*/
class dbOrg : public dbParty {
	OOFILE_METHODS(dbOrg)

/// @name relationships	
//@{
	dbRoleSet	Roles; ///> Roles relate to People
	dbPersonSet	People;
//@}

	dbOrg()
	{
		Name.setName("Organisation Name");
	}

/// @name data manipulation functions
//@{
	void addTestData();
//@}
};


/**
	A dbRole defines the context in which we interact with a Person.
	dbRole maps a Person to an Org.
	
	We differ from ooftst53 in adding a relationship to point to the
	description, rather than an embedded field.
*/
class dbRole : public dbTable {
	OOFILE_METHODS(dbRole)

/// @name relationships	
//@{
	dbPersonRef	Person;
	dbOrgRef	Org;
	dbLookupRef	Desc;
//@}

/// @name data manipulation functions
//@{
	void addRole(const char* inDesc, const char* inOptionalOrg=0);
//@}

};



#endif  // H_ooftst53
	