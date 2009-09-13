// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST53

/**
	 Simple stream I/O is used to interact with the user.
	to demonstrate complex OOFILE M:N pointer relationships
	and how to use subclassing - Party provides the data
	and Person and Org the relationships with each other.
	this sample is also marked up with Doxygen-compatible comments
*/
#include "ooftest.h"

#ifndef H_ooftst53
#define H_ooftst53

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
	Normally we would have Roles refer to another description in a 
	separate lookup table. However, to keep this example simpler they
	have been flattened so we redundantly store the Desc.
*/
class dbRole : public dbTable {
	OOFILE_METHODS(dbRole)

/// @name database fields
//@{
	dbChar		Desc;
//@}

	dbRole() :
		Desc(40, "Description")
	{}

/// @name relationships	
//@{
	dbPersonRef	Person;
	dbOrgRef	Org;
//@}

/// @name data manipulation functions
//@{
	void addRole(const char* inDesc, const char* inOptionalOrg=0);
//@}

};



#endif  // H_ooftst53
	