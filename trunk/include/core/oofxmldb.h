// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofdbxml.h

// support classes for XML output and parsing databases

#ifndef H_OOFXMLDB
#define H_OOFXMLDB

#ifndef H_OOFXML
	#include "oofxml.h"
#endif 
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifdef OOF_READ_XML
	#ifndef H_EXPATPP
		#include "expatpp.h"
	#endif
#endif

/**
	Base class for objects that can be serialised out to XML
	Serialisation allows you to have custom calcs save a signature and data
	and (if your code is linked to the reader) have these objects instantiated
	on reading back the XML. Most commonly used with oofRepXMLEnv.
	\ingroup oofXML
*/	
class OOFILE_EXPORT oofSerializable {
public:
	virtual ~oofSerializable() {};
	
	virtual oofString serialize() const;  // default returns empty
	virtual oofString signature() const=0;

// create an element to write to an XML file
	static oofString makeSerializableElement(oofSerializable*, const char* tag, oofIDfactory* idFactory, bool writeID=false, const char* otherAttributes=0);
};


#ifdef OOF_READ_XML

/**
	Parser for oofSerializable objects which have been written to XML.
	\see oofSerializable::makeSerializableElement for the tags we parse
	\ingroup oofXML
*/
class OOFILE_EXPORT oofXMLserializableParser : public expatppNesting {
public:
	oofXMLserializableParser(expatppNesting*, oofSerializable** returnValue);  // like a function return, but only when our callbacks complete
	virtual ~oofXMLserializableParser();
	virtual void startElement(const XML_Char* name, const XML_Char** atts);
	virtual void endElement(const XML_Char* name);
	virtual void charData(const XML_Char* s, int len);
	
protected:
// data storage
	oofSerializable** mReturnValue;
	bool mInSignature, mInData;
	oofString	mSignature, mData;	
};


/**
	Parse an OOFILE database schema from XML.
	\ingroup oofXML
*/
class OOFILE_EXPORT oofXMLschemaParser : public expatppNesting {
private:
	oofXMLschemaParser(const oofXMLschemaParser&) {};  // can't copy
public:
	oofXMLschemaParser();
	oofXMLschemaParser(expatppNesting*);
	virtual ~oofXMLschemaParser();
	virtual void startElement(const XML_Char* name, const XML_Char** atts);
	virtual void endElement(const XML_Char* name);
	virtual void charData(const XML_Char* s, int len);
	
protected:
// data storage
	dbTableOwningFields* mBuildingTable;  // use special dbTable subclass that manages dynamic field attachment
	bool mInDatatype, mBuildingField;
	oofString mFieldName;
	OOF_fieldTypes mFieldType;
	unsigned long mFieldMaxLen;
	OOF_Dictionary mNestedTables;
	
public:
	static OOF_fieldTypes attributes2fieldtype(const char**, unsigned long& maxLen)	;
};


/**
	Parse subtree of XML assuming all tags are fields or tables.
	Assumes nested data is related data to the enclosing table.
	\warning will add redundant nested data if multiple top-level records 
	pointed to the same related records.
	\todo cope by being able to lookup related data to see if should just point
	to a common record.
	\ingroup oofXML
*/
class OOFILE_EXPORT oofXMLdataParser : public expatppNesting {
private:
	oofXMLdataParser(const oofXMLdataParser&) {};  // can't copy
public:
	oofXMLdataParser(dbConnect*);
	oofXMLdataParser(expatppNesting*, dbConnect*);
	oofXMLdataParser(expatppNesting*, dbConnect*, dbTable*);
	virtual ~oofXMLdataParser();
	virtual void startElement(const XML_Char* name, const XML_Char** atts);
	virtual void endElement(const XML_Char* name);
	virtual void charData(const XML_Char* s, int len);
	
protected:
	void EnterNestedData(dbField*);
	
// data storage	
	dbConnect* mDB;
	dbTable* mTable;  // owned, temp
	bool mInRecord;
	fieldNumT mNextFieldNo;
	dbField* mField;
	fieldNumT mNumFields;  // cached from table
	bool mIsTopTable;
	oofString mFieldData;
};


/**
	 define static factory function taking a const oofString& and returning an oofSerializable.
	 \ingroup oofXML
 */
typedef oofSerializable* (*oofSerializableFactoryFunction) (const oofString&);

/// default factory handles registering a factory function
class OOFILE_EXPORT oofSerializableFactory : public OOF_PublicBase {
public:
	oofSerializableFactory(const oofString& inSignature, oofSerializableFactoryFunction);

	static void registerDefaultMake(const oofString& signature, oofSerializableFactoryFunction);

/// return a factory if want to lookup factories and invoke extra functions
	static oofSerializableFactory* factoryMatching(const oofString&);
	
/// just make object if using default factories
	static oofSerializable* makeObjectMatching(const oofString&, const oofString& inData);

/// custom factories may override makeObject	
	virtual oofSerializable*  makeObject(const oofString& inData); 
	
protected:
	virtual bool MatchesSignature(const oofString&);
	
// data storage
	oofString	mSignature;  // base factory has string to match
	oofSerializableFactoryFunction  mFactoryFunction;

	static OOF_Dictionary	sFactories;  // singleton, owns factory objects
};

#endif  // OOF_READ_XML


class dbView;


/**
	Write a database to XML on iostream with included <schema> tag.
	Writes fields as elements and related tables as nested elements.
	Does not generate a DTD.
	Writes the schema as our variant of XMLSchema.
	\todo reconcile our schema with "standards" from Microsoft
	\ingroup oofXML
*/
class OOFILE_EXPORT oofXMLDBWriter {
public:
	oofXMLDBWriter(ostream&, dbView*, oofIDfactory&);
	void startDatabase();
	void endDatabase();
	void writeDatabase();
	void writeRecord();
	void writeSchema();
	void startSchema();
	void endSchema();

private:
	void CalcNames();
	
// data storage
	ostream& mOS;
	dbView* mView;
	oofIDfactory& mIDfactory;
	oofString mTableElement, mRecordElement;

public:
	static oofString schemaHeader(const char* indentString);
	static oofString fieldtype2Attributes(const dbField*);
};


#endif

