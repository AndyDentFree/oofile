// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofdbxml.cpp

/**
	\file oofdbxml.cpp
	support classes for XML output and parsing databases.

	\note this stuff was written primarily for a local project and under budget
	constraints that prevented efficiency or flexibility being considered
	in particular, I'd like more ease of efficiently creating tags!
*/
#include "oofpch_c.h"  // for precompilation of core files


#ifndef H_OOFXMLDB
	#include "oofxmldb.h"
#endif 
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFMACRO
	#include "oofmacro.h"
#endif


/**
	Contain fields parsed from an XML schema into a database schema.
	Subclass of dbTableOwningFields so can attach indeterminate number of fields.
	\todo remove class when add OOFILE_METHODS to dbTableOwningFields
	\ingroup oofInternals
*/
class dbTableOwningFieldsXML : public dbTableOwningFields {
	OOFILE_METHODS(dbTableOwningFieldsXML)
	dbTableOwningFieldsXML(const char* name=0) :  
		dbTableOwningFields(name)
	{};  
};


// -------------------------------------------------------
//            o o f X M L D B w r i t e r
// -------------------------------------------------------

oofXMLDBWriter::oofXMLDBWriter(ostream& os, dbView* inView, oofIDfactory& inFactory) :
	mOS(os),
	mView(inView),
	mIDfactory(inFactory)
{
}


void
oofXMLDBWriter::CalcNames()
{
	const oofString viewName = mView->name();
	if (viewName.isEmpty())
		RAISE_EXCEPTION( oofE_General("oofXMLDBWriter::CalcNames can't write name for un-named view") );			
	
	mTableElement = oofTagMaker::encodeName(viewName);
#ifdef OOF_WRITE_XML_RECORD_TAGS
// otherwise save the time creating them
	oofString recordName = viewTable->tableSingularName();
	if (recordName==viewName)  // eg: Progress would stay the same
		recordName += "_record";
	mRecordElement = oofTagMaker::encodeName(recordName);
#endif
}


void
oofXMLDBWriter::startDatabase()
{
	CalcNames();  // in case we changed the table name, eg: if report uniquifies when writing XML
	mOS << oofTagMaker::makeOpeningTag(mTableElement, mIDfactory.getIndentString()) << "\n";
	mIDfactory.enterLevel();  // fields and sub-table data will be indented
}


void
oofXMLDBWriter::endDatabase()
{
	mIDfactory.leaveLevel();  // nested fields
	mOS << oofTagMaker::makeClosingTag(mTableElement, mIDfactory.getIndentString());
}


void
oofXMLDBWriter::writeDatabase()
{
	startDatabase();
	dbTable* viewTable = mView->table();
	if (viewTable) {
		const unsigned long numRecs = viewTable->count();
		viewTable->start();
		for (unsigned i=0; i<numRecs; i++) {
			writeRecord();
			viewTable->next();
		}
	}
	else
		writeRecord();
	endDatabase();
}


void
oofXMLDBWriter::writeRecord()
{
	const unsigned int numflds = mView->count();	
#ifdef OOF_WRITE_XML_RECORD_TAGS	
		const oofString recordIndent = mIDfactory.getIndentString();	
		mOS << oofTagMaker::makeOpeningTag(mRecordElement, recordIndent) << "\n";
		mIDfactory.enterLevel();
#endif
			const oofString fieldIndent = mIDfactory.getIndentString();	
			for( unsigned int i=0; i < numflds; i++ ) {
				const dbField& fld = mView->field(i);
				const oofString fldName = oofTagMaker::encodeName(fld.fieldName());  // NOT YET IMPLEMENTED - cache these
				mOS << oofTagMaker::makeSimpleBoundedElement(fld.copyString(), fldName, fieldIndent);
			}	
#ifdef OOF_WRITE_XML_RECORD_TAGS	
		mIDfactory.leaveLevel();
		mOS << oofTagMaker::makeClosingTag(mRecordElement, recordIndent);
#endif
		mOS << '\n';  // extra blank line between records makes it a lot more readable
}


oofString 
oofXMLDBWriter::schemaHeader(const char* indentString)
{
	oofString ret =  // use C string constant concantenation to make the following lines easier to read
		"<!-- \n"
		"     The following is based largely on the XML Schema working group draft \n"
		"     http://www.w3.org/TR/xmlschema-1/ with our own datatypes (eg: ui2) defined below\n" 
		"     being compatible with Microsoft's data types XML-Data, except format of date, time etc.\n"
		"     defined in http://msdn.microsoft.com/xml/schema/reference/datatypes.asp\n\n"
		"     Our dbFields are <elementType>s nested within the <elementType> of the table, rather than the\n"
		"     MS approach of declaring an <elementType> then using an IDRef in the <element> type attribute\n"
		"     we use length attributes in the <datatype> to indicate string and base64 fixed length fields\n"
		"     following the facets discussed in http://www.w3.org/TR/xmlschema-2/\n"
		"-->\n"
	;
	ret += indentString;
		ret += "<schema\n";
	ret += indentString;
		ret += "   xmlns:dt='http://www.w3.org/xmlschemas/datatypes'>\n";
	ret += indentString;
		ret += "   <datatype name='ui2'><basetype name='dt:integer'/><minInclusive>0</minInclusive><maxInclusive>65,535</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='i2'><basetype name='dt:integer'/><minInclusive>-32,768</minInclusive><maxInclusive>32,767</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='ui4'><basetype name='dt:integer'/><minInclusive>0</minInclusive><maxInclusive>4,294,967,295</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='i4'><basetype name='dt:integer'/><minInclusive>-2,147,483,648</minInclusive><maxInclusive>2,147,483,647</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='double'><basetype name='dt:real'/><minInclusive>1.7E-308</minInclusive><maxInclusive>1.7E308</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='date'><basetype name='dt:integer'/><minInclusive>0</minInclusive><maxInclusive>4,294,967,295</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='dateTime'><basetype name='dt:integer'/><minInclusive>0</minInclusive><maxInclusive>4,294,967,295</maxInclusive></datatype>\n";
	ret += indentString;
		ret += "   <datatype name='time'><basetype name='dt:integer'/><minInclusive>0</minInclusive><maxInclusive>65,535</maxInclusive></datatype>\n";
// NOT YET IMPLEMENTED - definition for bin.base64
	return ret;
}


void 
oofXMLDBWriter::endSchema()
{
	mIDfactory.leaveLevel();  // nested fields
	mOS << oofTagMaker::makeClosingTag("elementType", mIDfactory.getIndentString());
}


/**
	creates a string containing the schema for a view, excluding the end tag, in case we nest other data.
	
	flattens output so all data in a dbView appears to have been in the view's table
	which may continue to be a sufficient solution.

	\todo cope better with data on related tables
*/
void 
oofXMLDBWriter::startSchema()
{
	CalcNames();
	const oofString tableAtts("name=", oofString::quoteString(mTableElement)); 
	mOS << "\n"
		<< oofTagMaker::makeOpeningTag("elementType", mIDfactory.getIndentString(), 0 /* no ID */, tableAtts) << "\n";
	const unsigned int numflds = mView->count();	
	mIDfactory.enterLevel();
		const oofString fieldIndent = mIDfactory.getIndentString();
		for( unsigned int i=0; i < numflds; i++ ) {
			const dbField& fld = mView->field(i);
			const oofString fieldAtts("name=", oofString::quoteString( oofTagMaker::encodeName(fld.fieldName()) ));
			mOS << oofTagMaker::makeOpeningTag("elementType", fieldIndent, 0 /* no ID */, fieldAtts);
			mOS << " <datatypeRef" << fieldtype2Attributes(&fld) << "/> ";
			mOS << oofTagMaker::makeClosingTag("elementType");
		}	
		// leave level open as nested data may be declared within this schema element
}


/**
	used when you want to write a single dbView's schema to a stream.
	eg: if you were writing a separate schema document
	assumes Schema at outermost level
*/
void 
oofXMLDBWriter::writeSchema()
{
	mOS << schemaHeader(mIDfactory.getIndentString()) << "\n";
	startSchema();
	endSchema();
	mOS << oofTagMaker::makeClosingTag("Schema");
}


oofString 
oofXMLDBWriter::fieldtype2Attributes(const dbField* fld)
{
// see oofXMLschemaParser::attributes2fieldtype as our opposite	
	oofString ret = " name='";

	const OOF_fieldTypes fldType = fld->fieldType();
	switch (fldType) {
	case charField : {
			const unsigned long maxLen = fld->fieldStorageLen();
			char buf[40];
			sprintf(buf, "string' length='%lu'", maxLen);  // eg: name='string'  length='80'
			ret += buf;
		}
		break;
		
	case shortField :
		ret += "i2'";
		break;

	case uShortField :
		ret += "ui2'";
		break;

	case timeField :// NOT YET IMPLEMENTED - convert our date and time types to ISO 8601
		ret += "time'";  
		break;
		
	case longField :
		ret += "i4'";
		break;

	case dateField :// NOT YET IMPLEMENTED - convert our date and time types to ISO 8601
		ret += "date'";  
		break;
		
	case dateTimeField :
		ret += "dateTime'";  
		break;
		
	case uLongField :
		ret += "ui4'";  
		break;
		
	case realField :
		ret += "double'";
		break;

	case blobField :
		ret += "bin.base64'";  // varying binary
		break;

	case fixedBinaryField : {
			char buf[40];
			sprintf(buf, "bin.base64' length='%lu'", fld->fieldStorageLen()); 
			ret += buf;
		}
		break;

	default :
		ret += "string'";  // varying string, eg: dbText
	}	
	return ret;
}





// -------------------------------------------------------
//         o o f S e r i a l i z a b l e 
// -------------------------------------------------------
oofString 
oofSerializable::serialize() const  // default returns empty
{
	return oofString();
}


oofString 
oofSerializable::makeSerializableElement(oofSerializable* inSer, const char* tag, oofIDfactory* idFactory, bool writeID, const char* otherAttributes)
{
	
	oofString idString;
	if (writeID)
		idString = idFactory->getIDstring();
	const oofString outerIndent = idFactory->getIndentString();
	oofString ret = oofTagMaker::makeOpeningTag(tag, outerIndent, idString, otherAttributes);
	ret += '\n';
	idFactory->enterLevel(); {
		const oofString innerIndent = idFactory->getIndentString();
		ret += oofTagMaker::makeSimpleBoundedElement(inSer->signature(), "signature", innerIndent);
		const oofString dataBody = inSer->serialize();
		if (!dataBody.isEmpty())
			ret += oofTagMaker::makeSimpleBoundedElement(dataBody, "data", innerIndent);
	}
	idFactory->leaveLevel();
	ret += oofTagMaker::makeClosingTag(tag, outerIndent);
	return ret;
}


#ifdef OOF_READ_XML
// so people aren't forced to include expatpp in their projects

// -------------------------------------------------------
//         o o f X M L s c h e m a P a r s e r
// -------------------------------------------------------
oofXMLschemaParser::oofXMLschemaParser() :
	mBuildingTable(0),
	mBuildingField(false),
	mInDatatype(false)
{
}


oofXMLschemaParser::oofXMLschemaParser(expatppNesting* parent) :
	expatppNesting(parent),
	mBuildingTable(0),
	mBuildingField(false),
	mInDatatype(false)
{
}


oofXMLschemaParser::~oofXMLschemaParser()
{
	assert(
		(mParser && XML_GetErrorCode()!=0) || // !mParser if cleanly exit nested parser 
		(!mBuildingTable && !mInDatatype)
	);  // completed parsing or error exit
}
	
	
/**
	expatpp startElement call back.
*/
void 
oofXMLschemaParser::startElement(const char* name, const char** atts)
{
	if (mInDatatype)
		return;  // early exit to speedup skipping standard declarations
		
// following ordered in decreasing freqency
	if(strcmp(name,"datatype")==0){
		assert(!mInDatatype);
		mInDatatype = true;
	}

	else if(strcmp(name,"elementType")==0){  // could be table or field
// NOT YET IMPLEMENTED - more traditional schema where fields defined outside 
// and we see <elementTypeRef> inside <elementType> tags for tables

		const char* nameAtt = atts[1];
		assert(atts[0] && nameAtt);  // must have at least one attribute
		assert(strcmp(atts[0],"name")==0);  // bit suspect, but order-dependent assumption guarded here
		if (!mBuildingTable) {  // is topmost table
			assert(dbConnect::currentlyConstructing());  // caller must have created a dbConnect
			const oofString tableName = oofTagMaker::decodeName(nameAtt);
			mBuildingTable = new dbTableOwningFieldsXML(tableName);  // append to connection we are building, before newConnection called
		}
		else { // is field OR nested table==relationship field
		// creating new field, unless we pass in FieldIsStandaloneT, will attach to the current table
			if (mBuildingField) {
			// we should have encountered a <datatypeRef> inside an <elementType> for a plain field
			// so this is the first field inside a nested table
			// so the nested table needs creating from the saved field details in the previous <elementType>
				// lHS relationship field created before nested table -> many records in rhs table 
				dbRelSet* lhs = new dbRelSet(mFieldName);  // rel field is same name as related table
				mBuildingTable->adoptField(lhs);
				dbTableOwningFields* nestedTable = new dbTableOwningFieldsXML(mFieldName);  // append to connection we are building, before newConnection called
				// recurse - we've started a new table
				// RHS relationship field points back to parent table
				dbRelRef* rhs = new dbRelRef(mBuildingTable->tableName());  // point back to single parent
				nestedTable->adoptField(rhs);
				// add a relationship field to point to the nested table
				 // NOT YET IMPLEMENTED other relationships - always assume could be 1-N
				mNestedTables.append(mBuildingTable);
				mBuildingTable = nestedTable;  // effectively makes this recursive
				dbRelationship(lhs, rhs);  // declarative functor finishes the linking between actual tables
			}
			// field (or nested relationship) inside existing table
			mBuildingField = true;  // don't know its type yet!
			mFieldName = oofTagMaker::decodeName(nameAtt);  // save for construction in endElement
		}
	}

	else if(strcmp(name,"datatypeRef")==0){  // look at attributes to see type
		assert(mBuildingTable);
		mFieldType = attributes2fieldtype(atts, mFieldMaxLen);  // will complete field in endElement
	}

	else if(strcmp(name,"elementTypeRef")==0){  
		assert(!"OOFILE doesn't yet support elementTypeRef");
	}

	else if(strcmp(name,"schema")==0){
		// ignore the schema tag
		assert(mBuildingTable==0);
	}

	// should not encounter following starts - should be shielded by test at top
	else if(strcmp(name,"basetype")==0){
		assert(mInDatatype);  
	}
	else if(strcmp(name,"minInclusive")==0){
		assert(mInDatatype);  
	}
	else if(strcmp(name,"maxInclusive")==0){
		assert(mInDatatype);  
	}
}// end start element


void
oofXMLschemaParser::endElement(const char* name)
{
// following ordered in decreasing freqency
	if (mInDatatype) {
		if(strcmp(name,"datatype")==0){
			assert(mInDatatype);
			mInDatatype = false;  // clear optimisation flag
		}
		return;  // early exit to speedup skipping standard declarations
	}
	
	if(strcmp(name,"elementType")==0){  // could be table or field
		assert(mBuildingTable);
		if (mBuildingField) {  // all storage types, relationship fields added in startElement()
			mBuildingField = false;
			dbField* theField;
			// we finally have all the info we need to create the field
			switch (mFieldType) {   // see oofXMLDBWriter::fieldtype2Attributes
			case charField:
				theField = new dbChar(mFieldMaxLen, mFieldName);
				break;
			
			case textField:
				theField = new dbText(mFieldName);
				break;
			
			case shortField:
				theField = new dbShort(mFieldName);
				break;
			
			case uShortField:
				theField = new dbUshort(mFieldName);
				break;
			
			case longField:
				theField = new dbLong(mFieldName);
				break;
			
			case uLongField:
				theField = new dbUlong(mFieldName);
				break;
			
			case realField:
				theField = new dbReal(mFieldName);
				break;
			
			case dateField:
				theField = new dbDate(mFieldName);
				break;
			
			case dateTimeField:
				theField = new dbDateTime(mFieldName);
				break;
			
			case timeField:
				theField = new dbTime(mFieldName);
				break;
				
			default:
				{
					oofE_General excp(stringstream() << flush 
						<< "oofXMLschemaParser::endElement can't yet handle field type"
						<< mFieldType
					);
					RAISE_EXCEPTION(excp);
			// NOT YET IMPLEMENTED
				}
			}
			mBuildingTable->adoptField(theField);

		}
		else {
			mBuildingTable=(dbTableOwningFields*)mNestedTables.pop();  // safe downcast, return from nested table, 0 if topmost
			if (mBuildingTable)  // have we returned to top or was previous the top?
				mBuildingTable->stillCurrentlyConstructing();
		}
	}
	else if(strcmp(name,"schema")==0){
		// ignore the schema tag
		assert(mBuildingTable==0);  // should have finished table
	}
}// end element


void 
oofXMLschemaParser::charData(const XML_Char *s, int len)
{
	if (emptyCharData(s, len))
		return;
	
}


OOF_fieldTypes 
oofXMLschemaParser::attributes2fieldtype(const char** atts, unsigned long& outMaxLen)
{
// see oofXMLDBWriter::fieldtype2Attributes as our opposite	
	OOF_fieldTypes ret;
	const char* typeAtt = atts[1];
	assert(atts[0] && typeAtt);  // must have at least one attribute
	assert(strcmp(atts[0],"name")==0);  // assumption guarded that 1st attribute is name of type

	if(strcmp(typeAtt,"string")==0){
		if (atts[2]) {
			ret = charField;
			assert(atts[3]);  // always paired
			if(strcmp(atts[2],"length")==0) { // just in case later have extra data type atts
				const oofString lenAtt = atts[3];
				outMaxLen = lenAtt.asUnsignedLong();
			}   // 2nd attribute is length
		}  // got 2nd attribute
		else
			ret = textField;
	}
	else if(strcmp(typeAtt,"i2")==0){
		ret = shortField;
	}
	else if(strcmp(typeAtt,"ui2")==0){
		ret = uShortField;
	}
	else if(strcmp(typeAtt,"i4")==0){
		ret = longField;
	}
	else if(strcmp(typeAtt,"ui4")==0){
		ret = uLongField;
	}
	else if(strcmp(typeAtt,"double")==0){
		ret = realField;
	}
	else if(strcmp(typeAtt,"date")==0){
		ret = dateField;
	}
	else if(strcmp(typeAtt,"dateTime")==0){
		ret = dateTimeField;
	}
	else if(strcmp(typeAtt,"time")==0){
		ret = timeField;
	}
	else {
		stringstream os;
		os << "oofXMLschemaParser::attributes2fieldtype received unknown attributes:";
		for (unsigned int i=0; atts[i]; i++)
			os << " " << atts[i];
		oofE_General excp(os);
		RAISE_EXCEPTION(excp);
// NOT YET IMPLEMENTED
	}

	return ret;
}


// -------------------------------------------------------
//         o o f X M L d a t a P a r s e r
// -------------------------------------------------------
oofXMLdataParser::oofXMLdataParser(dbConnect* inDB) :
	mDB(inDB),
	mTable(0),
	mInRecord(false),
	mNextFieldNo(0),
	mField(0),
	mIsTopTable(true)
{
// not yet used - we have only tested data parsing when called from OOF_XMLrepParser
// which is a nesting parser so invokes the ctor oofXMLdataParser(expatppNesting*, dbConnect*, dbTable*)
}


oofXMLdataParser::oofXMLdataParser(expatppNesting* parent, dbConnect* inDB) :
	expatppNesting(parent),
	mDB(inDB),
	mTable(0),
	mInRecord(false),
	mNextFieldNo(0),
	mField(0),
	mIsTopTable(true)  // only false for ctor with nestedTable param
{
}


oofXMLdataParser::oofXMLdataParser(expatppNesting* parent, dbConnect* inDB, dbTable* nestedTable) :
	expatppNesting(parent),
	mDB(inDB),
	mTable(nestedTable),
	mInRecord(false),
	mNextFieldNo(0),
	mField(0),
	mIsTopTable(false)
{
	assert(mTable);  // in case of caller error
	mNumFields = mTable->numFields();
}


oofXMLdataParser::~oofXMLdataParser()
{
	assert(
		(mParser && XML_GetErrorCode()!=0) ||  // !mParser if cleanly exit nested parser 
		(!mInRecord)  // mTable may still point to a table in nested situations
	);  // completed parsing or error exit
}
	
	
/**
	expatpp startElement call back.
*/
void 
oofXMLdataParser::startElement(const char* inName, const char** atts)
{
	const oofString name = oofTagMaker::decodeName(inName);

	if (mTable) {
	// assume start of field in record
	// but could be a nested table, eg:
/*
      <dbStudents1>
         <LastName>Carey</LastName>
         <FirstName>Marion</FirstName>

         <dbProgress>
            <CurricID>58</CurricID>
*/	
	
	// get the next data field we expect from the schema
	// (model assumes empty fields written as empty elements, NOT skipped!)

		const long numFieldsThisTable = mNumFields;  // local const is minor optimisation for loop calcs
		for (;mNextFieldNo<numFieldsThisTable;) { // skip rel fields
			mField = mTable->field(mNextFieldNo);
			if (mField->fieldType()==relationshipField) { 
				if (mField->fieldName()==name) {  // we hit nested table matching relationship field
					EnterNestedData(mField);
					return;  // ***** early exit
				}
				else { // skip embedded rel fields
					mNextFieldNo++;
					mField=0;
				}
			}
			else
				break;
		}
		// logic to handle having reached end of fields in record is in endElement()
		if (!mInRecord) {  // may be 1st field in new rec, or start of related table
			// first real field is field 0 or follows a skipped relationship field
			assert((mNextFieldNo==0) || (mTable->field(mNextFieldNo-1)->fieldType()==relationshipField));  
			if (mField->fieldName()==name)  {  // element name matches 1st field fieldname
				mInRecord=true;  // so it was start of a new record at our current level
				mTable->newRecord();
			}
			else { // field doesn't match - must be nested table or error
				// lookup assumes field names unique within table, have to search for relationship field
				// because related data will probably be at the end of a parent record, NOT in the same
				// position necessarily as the relationship field itself
				// This would happen if relationship field was skipped in loop above, with nested data
				// occurring at end of parent record
				dbField* theField = mTable->field(name);  // rel field has same name as table
				assert(theField->fieldType()==relationshipField);
				EnterNestedData(theField);
			}
		} // !mInRecord
		if (mField)
			mFieldData.clear();
	}
	else {  // not in table, so name should be top-level table name
		assert(!mInRecord && mNextFieldNo==0);
		mTable = mDB->table(name);
		assert(mTable);  // table element fails to match table in schema
		mNumFields = mTable->numFields();
	}
}// end start element


void
oofXMLdataParser::EnterNestedData(dbField* theField)
{
	dbRelRefBase* rel2rhs = (dbRelRefBase*)theField;  // safe downcast
	dbTable* nestedTable = rel2rhs->relatedTable();  // RECURSE into nested table
	// asserts based on table names being unique in schema
	dbTable* otherTable;  // just used in asserts
	assert( (otherTable = mDB->table(theField->fieldName()) )!=0);  // confirm can lookup table name  - the assignment in the assert is intentional
	assert(otherTable->tableNumber()==nestedTable->tableNumber());  // confirms rel points to correct type
	
	oofXMLdataParser* dataParser = new oofXMLdataParser(this, mDB, nestedTable);  // self-deleting sub parser
	// sub-parser will start with the record-level elements
}


void
oofXMLdataParser::endElement(const char* name)
{
	if (mInRecord) {
		assert(mTable);
		assert(mField);  // can't get to endElement without hitting a field
		// this is end of field tag OR end of nested data 
		
		mField->setString(mFieldData);  // having accumulated data with possible multiple charData calls
		
		mNextFieldNo++;	
		if (mNextFieldNo>=mNumFields) { // end of record - ended last field
			mNextFieldNo=0;
			mInRecord = false;
			if (mIsTopTable)
				mTable->saveRecord();  // will cascade to save related
			// if we don't avoid saving related records here will have a 
			// save invoked on the parent which prevents isNewRecord being used
			// on our return from the nested table, below				
		}
	}
	else { // assume end of table
		assert(mTable);
// taken out - name incoming may have periods instead of spaces		assert(strcmp(mTable->tableName(), name)==0);
		// NOT YET IMPLEMENTED (and maybe never) allow for fewer fields than in table
		assert(!mTable->isNewRecord());  // should have saved at end of last field
		mTable=0;
	} // end of table
}// end element


void 
oofXMLdataParser::charData(const XML_Char *s, int len)
{
// remember it may be called repeatedly if embedded newlines!
	if (!mField)
		return;  // only interested if in a field

	oofString decodedString = oofString::utf8To8BitAscii(s,len);// convert from utf8 format
	mFieldData.append(decodedString, decodedString.length());  
}


// -------------------------------------------------------
//   o o f X M L s e r i a l i z a b l e P a r s e r
// -------------------------------------------------------
oofXMLserializableParser::oofXMLserializableParser(expatppNesting* parent, oofSerializable** returnValue) :
	expatppNesting(parent),
	mReturnValue(returnValue),
	mInSignature(false),
	mInData(false)
{
}


oofXMLserializableParser::~oofXMLserializableParser()
{
	assert(!mInSignature && !mInData);  // may never have been in Data
	*mReturnValue = oofSerializableFactory::makeObjectMatching(mSignature, mData);
	// update the reference to a pointer we received in our ctor
}
	
		
void 
oofXMLserializableParser::startElement(const char* name, const char** atts)
{
	if(strcmp(name,"signature")==0){  
		mInSignature = true;
		mSignature.clear();
	}
	else {
		assert(strcmp(name,"data")==0);
		mInData = true;
		mData.clear();
	}
}


void
oofXMLserializableParser::endElement(const char* name)
{
	if(strcmp(name,"signature")==0){  
		mInSignature = false;
	}
	else {
		assert(strcmp(name,"data")==0);
		mInData = false;
	}
}


void 
oofXMLserializableParser::charData(const XML_Char *s, int len)
{
	if (mInSignature)
		mSignature.append(s, len);
	else if (mInData)
		mData.append(s, len);
}


// -------------------------------------------------------
//         o o f S e r i a l i z a b l e  F a c t o r y 
// -------------------------------------------------------
// 990529 due to static init order bug, temp hack is for this 
// to be declared in oofrep3 so it precedes the oofRepDefaultSettings ctor
//OOF_Dictionary oofSerializableFactory::sFactories(true);  // owns objects

oofSerializableFactory::oofSerializableFactory(const oofString& inSignature, oofSerializableFactoryFunction inFactFunc) :
	mSignature(inSignature),
	mFactoryFunction(inFactFunc)
{}


void 
oofSerializableFactory::registerDefaultMake(const oofString& inSignature, oofSerializableFactoryFunction inFactFunc)
{
	oofSerializableFactory* orphanedFactory = new oofSerializableFactory(inSignature, inFactFunc);
	sFactories.append(orphanedFactory);
}


oofSerializableFactory* 
oofSerializableFactory::factoryMatching(const oofString& inSignature)
{
	const unsigned long numFactories = sFactories.count();
	for (unsigned long i=0; i<numFactories; i++) {
		oofSerializableFactory*  theFactory = (oofSerializableFactory*) sFactories.item(i);  // safe downcast
		assert(theFactory);
		if (theFactory->MatchesSignature(inSignature))
			return theFactory;
	}
	return 0;
}
	

oofSerializable* 
oofSerializableFactory::makeObjectMatching(const oofString& inSignature, const oofString& inData)
{
	oofSerializable*  ret = 0;
	oofSerializableFactory*  theFactory = factoryMatching(inSignature);
	if (theFactory)
		ret = theFactory->makeObject(inData);
	return ret;
}


oofSerializable* 
oofSerializableFactory::makeObject(const oofString& inData)
{
	assert(mFactoryFunction);
	oofSerializable* ret = mFactoryFunction(inData);
	return ret;
}


bool 
oofSerializableFactory::MatchesSignature(const oofString& inSignature)
{
// virtual function allows subclassers to 
// add their own qualifiers in the signature for ultimate flexibility.
// or even do a hashed match
	return mSignature==inSignature;
}



#endif // OOF_READ_XML
