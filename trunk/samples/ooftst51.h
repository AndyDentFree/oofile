// Copyright 1999 A.D. Software. All Rights Reserved
// ooftst51.h
// included in ooftest51 and for user debugging

// provides a function which iterates a database connection
// to dump all names of fields and tables
// and test their encoding/decoding for XML


#ifndef H_OOFXML
	#include "oofxml.h"
#endif


void testXMLnames(dbConnect*);
void testName(const char* prompt, const oofString& inStr);

void 
testXMLnames(dbConnect* theDB)
{
	cout << "Test some special cases:\n";
	testName("leading XML", "xml_whatever");
	testName("leading spaces & special chars", "  something *is* out-there!!!");
	testName("leading digit", "3dun");
	
	cout << "Test the database:\n\n";
	
	const int numTables = theDB->numTables();
	for (int ixT=0; ixT<numTables; ixT++) {
		dbTable* theTable = theDB->table(ixT);
		const oofString tableName = theTable->tableName();
/* to limit test to specific table*/
		if (tableName!="KD")
			continue;

		testName("Table", tableName);
		
		const int numFields = theTable->numFields();
		for (int ixF=0; ixF<numFields; ixF++) {
			dbField* theField = theTable->field(ixF);
			testName("   Field", theField->fieldName());
		}  // table loop
	}  // table loop
}



void 
testName(const char* prompt, const oofString& inStr)
{
	const oofString encName = oofXMLwriter::encodeName(inStr);
	const oofString decName = oofXMLwriter::decodeName(encName);
	cout << prompt << '\'' << inStr 
		<< "'  enc: '" << encName 
		<< "'  dec: '" << decName;
	if (decName==inStr)
		cout << '\'' << endl;
	else 
		cout << "\' FAILED" << endl;
}
