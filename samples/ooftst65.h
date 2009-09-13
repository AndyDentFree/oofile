// ooftst65.h
// 
#pragma once

#include "oofile.h"

// Define the creator type and file type for your application:
#define kSignature		'????'
#define kFileType		'????'

class dbContacts : public dbTable {
  OOFILE_METHODS(dbContacts)
		dbChar		Name;
		dbChar		Phone;
		dbChar		Fax;
		dbText		Address;
		dbText		Notes;

	dbContacts() : dbTable("Contacts") ,
		Name(120, "Name"),
		Phone(19, "Phone"),
		Fax(19, "Fax"),
	Address("Address"),
	Notes("Notes")
	{
	Name.index(kIndexCompress);
	setSortOrder(dbSorter() << Name << Phone);
	};
};

