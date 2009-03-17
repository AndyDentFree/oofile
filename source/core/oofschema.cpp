// OOFILE  schema inport and connect
// 
// COPYRIGHT 2000 A.D. Software, All rights reserved
#include "oofpch_c.h"  // for precompilation of core files

#include "oofios.h"

#ifndef H_OOFSTR
	#include "oofStr.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOF1
	#include "oof1.h" 
#endif	
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif

//#include "oofCTRes.h"
#include "oofschema.h"




//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//		s c h e m a P a r s e r
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


/**Given an oofstring in srcString, this method parses and builds a schema in the current connection */
void
oofSchemaParser::parseSchemaText(const oofString& srcString)
{
	oofString tableString;
	mTableFound = true,
	mCurrentPos = 0;

	
	assert(!srcString.isEmpty());
	mParseBuffer = srcString;
	mParseBuffer.lineEndingsMac();// to make thing easy
	tableString = FindTableString();
	while(mTableFound){
		MakeTable(tableString);
		tableString = FindTableString();
	}
	mUnResolvedRels.resolveRelationships();
}

/**Given an oofstring in oofSchemaParser's internal buffer this method parses and builds a schema in the current connection */
void
oofSchemaParser::parseSchemaText()
{
	oofString tableString;
	mTableFound = true,
	mCurrentPos = 0;

	
	mParseBuffer.lineEndingsMac();// to make thing easy
	tableString = FindTableString();
	while(mTableFound){
		MakeTable(tableString);
		tableString = FindTableString();
	}
	mUnResolvedRels.resolveRelationships();
}


/**Load a oofschema string into the mParseBuffer from a file identified by  schemaFileName
It will attempt to figure out the file type from one of:
	 ramp; ctree (single or supewr file); dbase and load the data approrialtky
*/
void	
oofSchemaParser::loadSchema(const oofString &schemaFileName)
{
	stSaveDirectory willReturnToCurrentDir;
	//loadSchema(oofFileRef(schemaFileName));
	loadSchema(schemaFileName);
	
}


/**Load a oofschema string into the mParseBuffer from a ctree file  */

//void
//oofSchemaParser::loadSchema(const oofString &schemaFileName)
//oofSchemaParser::loadSchema(const oofString &schemaFile/* const oofString& ext, const oofString& iExt, dbConnect_ctree &theConnection*/)
//{
//	
//		OOFCTResource resourceInterface;
		
		//resourceInterface.openFile(schemaFile,theConnection.connectionName(),ext,iExt);
		//resourceInterface.loadResource('OFSC');
		//mParseBuffer.setChars(resourceInterface.resource(),resourceInterface.dataLength());
		//resourceInterface.closeFile();
//}


/**
	Examine file schema and load it into oofSchemaParser mParseBuffer.
*/
void	
oofSchemaParser::loadSchema(oofFileRef & /*schemaFile*/)
{
/*
	FILE* scFile = schemaFile.fopen( "rb");
	unsigned long bSize ;
	char buffer[64];
	char *fBuffer;
	unsigned long nRead = fread(buffer, sizeof(char),12, scFile);
	if(nRead != 12){
		oofE_oofSchemaParser err("oofSchemaParser::loadSchema. Schema file corrupt (to short)" );
		RAISE_EXCEPTION(err);
	}
	buffer[12] = '\0';
	
	if(strcmp("oofschemaVer",buffer)==0){//dbase
		bSize = schemaFile.fileSize(false)+1;
		fBuffer = new char[bSize];
		fseek(scFile, 0, SEEK_SET);
		nRead = fread(fBuffer, sizeof(char),bSize, scFile);
		if(nRead != bSize-1){
			oofE_oofSchemaParser err("oofSchemaParser::loadSchema. Failed to read full dbase schema file" );
			RAISE_EXCEPTION(err);
		}
		fBuffer[bSize] = '\0';
		mParseBuffer.adopt(fBuffer);
						//dbase or ctree
	}else
	{// ramp?
		//must guard against silly oofsets which we could get on super files etc
	
		fseek(scFile, -10,  SEEK_END);
		//nRead = fread(buffer, sizeof(char),8, scFile);
		unsigned long shemaPos;
		bool tryCtree = false;
		fscanf(scFile,"%lx", &shemaPos);
		unsigned long endFile = ftell(scFile);
		assert(endFile != -1);	
		if((shemaPos<0)|| (shemaPos> endFile))
			tryCtree = true;

		//assert( nRead==10);
		//buffer[9] = '\0';
		//unsigned long shemaPos = atol(buffer);
		if(!tryCtree){
			fseek(scFile, -shemaPos, SEEK_CUR);
			fBuffer = new char[shemaPos+1];
			nRead = fread(fBuffer, sizeof(char),shemaPos-8, scFile);// the -8 gets rid of the terminationg offset long
			assert(nRead == shemaPos-8);
			if(nRead != (shemaPos-8)){
				oofE_oofSchemaParser err("oofSchemaParser::loadSchema. Failed to read full ramp schema " );
				RAISE_EXCEPTION(err);
			}
	
			fBuffer[shemaPos+1] = '\0';
			mParseBuffer.adopt(fBuffer);
			if(mParseBuffer.find("oofschemaVer",0, true) != 0){//must be a ctree super file;	
				delete fBuffer;
				tryCtree = true;
			}
		
		}
		fclose(scFile);
		if(tryCtree){
		//could be ctree super or single file 
			
			OOFCTResource resourceInterface;
			//if(schemaFile.filename.contains(".osc",false) == oofString::kNotFound) //must change this to a function staticy thing
			//{
				resourceInterface.openFile(schemaFile);
				resourceInterface.loadResource('OFSC');
				mParseBuffer.setChars(resourceInterface.resource(),resourceInterface.dataLength());
				resourceInterface.closeFile();
			//}//else
			//{
				
		/// \todo	 no schema old oofile file?
		/// \todo   on server will not see file as dbase or ramp so we may have to chane order or some other clever logic
			
			//}
		}
		
	}//else	
		
*/
}
	
	
	
	
	
	
	
	

 
/**Attempt to load an OOFSchema from schemaFile and indicate if schema was found and loaded  */
bool 
oofSchemaParser::isOOFSchema(oofFileRef &schemaFile)
{
	loadSchema(schemaFile);
	long ret = mParseBuffer.find("oofschemaVer",0,true);
	if(ret ==  oofString::kNotFound)
		return false;
	else
		return true;
}

/*bool 
oofSchemaParser::isOOFSchema()
{
	loadSchema(schemaFile);
	long ret = mParseBuffer.find("oofschemaVer",0,true);
	if(ret =  oofString::kNotFound)
		return false;
	else
		return true;
}
*/

/** Given tableString, a valid oofSchema table description string, parse and build a table*/
void
oofSchemaParser::MakeTable(const oofString &tableString)
{
	dbTableOwningFields *buildingTable = new dbTableOwningFieldsSOoFSchema((const char*)FindTableName(tableString));  // append to connection we are building, before newConnection called
	oofString defaultExt = "";
	mCurrentFieldPos = 0;
	oofString fieldString = FindFieldString(tableString);
	while(mFieldFound){
		dbField *theField = makeField(fieldString);
		buildingTable->adoptField(theField); // apphend field
		fieldString = FindFieldString(tableString);
	}
	
}	

/**Search mParseBuffer for a oofSchema table description string from current search pos*/

oofString 		
oofSchemaParser::FindTableString()
{
	
	const int kNameLen = 7; // sizeof("table: ")
	
	long namePos;
	long endTable;
	
	namePos =  mParseBuffer.find("table: ",mCurrentPos,true);
	if(namePos == oofString::kNotFound){
			mTableFound = false;
	}
	//	now find the end of this Table
	endTable= mParseBuffer.find("table",namePos+1,true);// that +1 stops  it from finding finding the current string beginning as the end
	if(endTable == oofString::kNotFound){
		endTable = mParseBuffer.length();
	}
	mCurrentPos = endTable;
	return mParseBuffer.subString(namePos, endTable - namePos) ;
}

/** Given tableString, a valid oofSchema table description string, extract table name*/
oofString
oofSchemaParser::FindTableName(const oofString &tableString)
{ 
	
	const int kNameLen = 7;  // sizeof("table: ")
	
	long namePos;
	long endName;
	
	
	namePos=tableString.find("table: ",0,true);
	if(namePos == oofString::kNotFound
 		)
		//mNoTableFound = true;//assert error corrupt schema
		mTableFound = false;
	namePos +=kNameLen;
	endName = tableString.find("contains fields:",namePos,true);
	if(endName == oofString::kNotFound){
			oofE_oofSchemaParser err("oofSchemaParser::FindTableName. Corrupt schema table string" );
			RAISE_EXCEPTION(err);
		}
		 
	//while(tableString[--endName] == ' ');// find the end of name
	endName++;
	return tableString.subString(namePos, endName - namePos) ;
}


/** Given fieldString, a valid oofSchema field description string, extract build a field*/
dbField	*	
oofSchemaParser::makeField(const oofString &fieldString)
{		
		
	
	dbField *retField;
	if(!IsRel(fieldString)){
	switch (FindType(fieldString)) {   
		case charField:
			if(FieldIsIndexed(fieldString))
				retField = new dbChar(FindFieldLength(fieldString), FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbChar(FindFieldLength(fieldString), FindFieldName(fieldString));
			break;
		
		case textField:
			if(FieldIsIndexed(fieldString))
				retField = new dbText(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbText(FindFieldName(fieldString));
			break;
		
		case shortField:
			if(FieldIsIndexed(fieldString))
				retField = new dbShort(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbShort(FindFieldName(fieldString));
			break;
		
		case uShortField:
			if(FieldIsIndexed(fieldString))
				retField = new dbUshort(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbUshort(FindFieldName(fieldString));
			break;
		
		case longField:
			if(FieldIsIndexed(fieldString))
				retField = new dbLong(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbLong(FindFieldName(fieldString));
			break;
		
		case uLongField:
			if(FieldIsIndexed(fieldString))
				retField = new dbUlong(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbUlong(FindFieldName(fieldString));
			break;
		
		case realField:
			if(FieldIsIndexed(fieldString))
				retField = new dbReal(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbReal(FindFieldName(fieldString));
			break;
		
		case dateField:
			if(FieldIsIndexed(fieldString))
				retField = new dbDate(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbDate(FindFieldName(fieldString));
			break;
		
		case dateTimeField:
			if(FieldIsIndexed(fieldString))
				retField = new dbDateTime(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbDateTime(FindFieldName(fieldString));
			break;
		
		case timeField:
			if(FieldIsIndexed(fieldString))
				retField = new dbTime(FindFieldName(fieldString),ParseIndexType(fieldString));
			else
				retField = new dbTime(FindFieldName(fieldString));
			
			break;
			
		default:
			{
				//oofE_General excp(stringstream() << flush 
				//	<< "Can't yet handle field type"
				//	<< mFieldType
				//);
				//RAISE_EXCEPTION(excp);
		// NOT YET IMPLEMENTED
			}
		}
	
	}else{
		unsigned long maxLinks; 
		switch(FindRelType(fieldString)){
			case e1_1:
			case eM_1:
			case eN_1:
				maxLinks = 1;
			break;
			default:
				maxLinks = 0;
			break;
		}
		dbRelRefBase *relRef =  new dbRelRefBase(FindFieldName(fieldString),maxLinks); 
		relRef->propagateRelatedDeletes(PropagatesDeletes(fieldString));
	
		//retField = new dbRelRefBase(FindFieldName(fieldString),maxLinks); 
		  retField = relRef;        
		//(dbRelRefBase*) retField->propagateRelatedDeletes(PropagatesDeletes(fieldString));
		//dbTable::field(const oofString& fieldName)

		//unResolvedRelationship *toResolve = new unResolvedRelationship(FindFieldName(fieldString),FindJoinFieldName(fieldString),(dbRelRefBase*)retField,true /*do i need this?*/);
 		unResolvedRelationship *toResolve = new unResolvedRelationship(FindFieldName(fieldString),FindJoinFieldName(fieldString),(dbRelRefBase*)retField,true /*do i need this?*/);
 		
		mUnResolvedRels.append(toResolve);
	
		//	unResolvedRelationship(oofString rhstableName,oofString rhsFieldName,retField,true /*do i need this?*/);
 		//unResolvedRelationship(FindFieldName(fieldString),FindJoinFieldName(fieldString),(dbRelRefBase*)retField,true /*do i need this?*/);
 		//check the abicve casts to (dbRelRefBase*)
	}	
	
	return retField;
		

}








/*oofString 		
oofSchemaParser::FindFieldString(oofString &TableString)
{
	
	const int kNameLen = 7;  // sizeof("Field: ")
	
	long FieldPos;
	long endName;
	char theChar;
	long endTable;
	
	long FieldPos=  mParseBuffer.find("Field: ",mCurrentPos,true);
	if(namePos == oofString::kNotFound)
 		{
 				mNoTableFound = true;
	
	//	now find the end of this Type
		endTable= long mParseBuffer.find("table",mCurrentPos,true);
		if(endTable == oofString::kNotFound
 		){
			endTable = mParseBuffer.length();
		}
	}
	mNexTable = endTable;
	return subString(namePos, endTable) ;
}


*/
/** Given tableString, a valid oofSchema table description string, extract a field
 */

oofString
oofSchemaParser::FindFieldString(const oofString &tableString)
{ 
	
	const int kNameLen = 7;  // sizeof("field: ")
	
	long fieldPos;
	long endField;
	unsigned long relPos;
	mFieldFound = true;
	
	fieldPos =  tableString.find("field: ",mCurrentFieldPos,true);
	if(fieldPos == oofString::kNotFound)
 	{
		mFieldFound = false;
		return "";
	}else{
		fieldPos += kNameLen; //jump over field: "
		//now find the end of this field
		endField =  tableString.find("field: ",fieldPos,true);//yes
		if(endField == oofString::kNotFound){
			endField = tableString.length();
		}
		relPos = tableString.find("is a",fieldPos,true);
		if((relPos != oofString::kNotFound)&&(relPos < endField)) // is this a relation
			endField = tableString.find(13,relPos,true);//it's a reField
	}
	mCurrentFieldPos = endField;
	return tableString.subString(fieldPos, endField - fieldPos) ;
}



/** Given tableString, a valid oofSchema field description string, extract field name*/
oofString
oofSchemaParser::FindFieldName(const oofString &fieldString)
{ 
	
	const int kNameLen = 7;  // sizeof("table: ")
	
	//long namePos;
	long endName;
	
	endName = fieldString.find("type:",0,true);
	if(endName == oofString::kNotFound){
		endName = fieldString.find("is a",0,true);
		if(endName == oofString::kNotFound){
			oofE_oofSchemaParser err("oofSchemaParser::FindFieldName. Corrupt field table string" );
			RAISE_EXCEPTION(err);
		} 
	}
	//while(fieldString[--endName] == ' ');// find the end of name
	//while(isspace(fieldString[--endName]));//dg debug 2001023
	endName++;
	//return fieldString.subString(namePos, endName - namePos) ;
	return fieldString.subString(0, endName) ;

}

/** Given fieldString, a valid oofSchema join field  description string, extract join field name*/
oofString  
oofSchemaParser::FindJoinFieldName(const oofString &fieldString)
{
	const int kNameLen = 22;  // sizeof("joining across field: ")
	unsigned long endName;
	long pos = fieldString.find("joining across field: ",0,true);
	
	if(pos == oofString::kNotFound)
		return "";
	
		
	pos +=kNameLen;	
	endName = fieldString.find("and prop",pos,true);
	if(endName == oofString::kNotFound)
 		endName = fieldString.length();
	//while(fieldString[--endName] == ' ');// find the end of name
	while(isspace(fieldString[--endName]))
		;//dg debug 2001023

	endName++;
	return fieldString.subString(pos, endName - pos) ;
}


/** Given tableString, a valid oofSchema field field description string, extract field type string*/
oofString
oofSchemaParser::FindTypeString(const oofString &fieldString)
{ 
	
	const int ktypeLen = 6; // sizeof("type: ")
	
	long typePos;
	long endType;
	
	typePos=  fieldString.find("type: ",0,true);
	if(typePos == oofString::kNotFound){
 			oofE_oofSchemaParser err("oofSchemaParser::FindTypeString. corrupt schema missing type" );
			RAISE_EXCEPTION(err);
	}
	//now find the end of this type
	endType = fieldString.find("index: ",typePos,true);
	if(endType == oofString::kNotFound){
			endType = fieldString.length();
	}
	//mCurrentFieldPos = endType;
	return fieldString.subString(typePos+ktypeLen, typePos - endType ) ;
}

/** Given tableString, a valid oofSchema field field description string, extract field type*/

OOF_fieldTypes
oofSchemaParser::FindType(const oofString &fieldString)
{ 
	OOF_fieldTypes returnType;
	oofString typeString = FindTypeString(fieldString);
	switch(typeString[0]){
		
		case 'b':
			if(typeString[1] =='l')
				returnType = blobField;
			else
				returnType = boolField;
		break;
	
		case 'c':
				returnType = compoundField;
		break;
		
		case 'd':
			if(typeString[5] =='i')
				returnType = dateTimeField;
			else
				returnType = dateField;
		break;
		
		case 'f':
			if(typeString[5] == 'B')
				returnType = fixedBinaryField;
			else
				returnType = charField;
				
		break;
		
		case'l':
			returnType = longField;
		break;
		
		case 'r':
		if(typeString[2] =='a')
			returnType = realField;
		else
			returnType = relationshipField;
		break;
		
		case 's':
			returnType = shortField;
		break;
			
		case 't' :
		if(typeString[1] =='e')
			returnType = textField;
		else
			returnType = timeField;
		break;
		
		case 'u':
		if(typeString[1] =='L')
			returnType = uLongField;
		else
			returnType = uShortField;
		break;

}
	return returnType;
}





/** Given fieldString, check if this field is indexed */
bool
oofSchemaParser::FieldIsIndexed(const oofString &fieldString)
{
	long indexpos = fieldString.find("index",0,true);
	if(indexpos == oofString::kNotFound)
		return false;
	else
		return true;
}	


/** Given fieldString, extract the index descriptor string */
oofString
oofSchemaParser::FindIndexString(const oofString &fieldString)
{ 
	long indexPos,endIndex;
	indexPos=  fieldString.find("index: ",0,true);
	if(indexPos == oofString::kNotFound)
		assert(0);//something is wrong FieldIsIndexed call should gaurd against this 
	indexPos += 7;
	// now find the end of this index
	endIndex = fieldString.find(13,indexPos);
	if(endIndex == oofString::kNotFound){
		oofE_oofSchemaParser err("oofSchemaParser::FindIndexString. Corrupt schema" );
		RAISE_EXCEPTION(err);
	}
	return fieldString.subString(indexPos, endIndex - indexPos);
}


/** Given fieldString, find the index type*/
OOF_IndexOptions
oofSchemaParser::ParseIndexType(const oofString &fieldString)
{

	OOF_IndexOptions theIndexOpt;
		
	oofString indexStr = FindIndexString(fieldString);
	switch(indexStr.length()){
		case 32:
			if(indexStr=="indexed, no dups & ignoring case")
				theIndexOpt = kIndexNoDups;
			else
				theIndexOpt = kIndexNoNulls;
		break;
	
		case 38://"indexed, allowing dups & ignoring case";
			theIndexOpt = kIndexed;
		break;
	
		case 40://"indexed, ignoring case, no dups no nulls";
			theIndexOpt = kIndexNoDupsNoNulls;
		break;
		
		case 41://indexed, ignoring case, compressing front
			if(indexStr == "indexed, ignoring case, compressing front")
				theIndexOpt = kIndexCompressLeading;
			else
				theIndexOpt = kIndexCaseSensitive;//"indexed, allowing dups but case-sensitive"
		break;
		
		case 43:
			theIndexOpt = kIndexCompressPadding;//"indexed, ignoring case, compressing padding"
		break;
		
		case 48:
			theIndexOpt = kIndexCompress;//"indexed, ignoring case, compress front & padding";
		break;
		
		case 51:
			theIndexOpt = kIndexCompressLeadingNoDups;//"indexed, no dups & ignoring case, compressing front"
		break;
		
		case 53:
			theIndexOpt = kIndexCompressPaddingNoDups;//indexed, no dups & ignoring case, compressing padding
		break;
	
		case 58:
			theIndexOpt = kIndexCompressNoDups;//ndexed, no dups & ignoring case, compress front & padding
		break;
		
		case 60:
			theIndexOpt = kIndexCompressNoDupsNoNulls;//indexed, ignoring case, no nulls, no dups, compressing front
		break;
		
		case 61:
			theIndexOpt = kIndexCompressNoNulls;//indexed, ignoring case, no nulls, compressing front & padding
		break;
		
		case 62:
			theIndexOpt = kIndexCompressPaddingNoDupsNoNulls; //indexed, ignoring case, no nulls, no dups, compressing padding
	
		break;
		
		case 70:
			theIndexOpt = kIndexCompressNoDupsNoNulls ;//indexed, ignoring case, no nulls, no dups, compressing front & padding
	
		break;
	}
	return theIndexOpt;
}


/** Given fieldString, extract the relationship type */
oofSchemaParser::eRelType
oofSchemaParser::FindRelType(const oofString& /*fieldString*/)
{
	eRelType ret = eRelError;
/*	long pos =  fieldString.find("is a ",0,true);
	pos += 5;
	switch(fieldString[pos])
	{
		case '1':
			switch(fieldString[pos+2]){
				case '1':
					ret = e1_1;
				break;
			
				case'N':
					ret = e1_N;
				break;
				
				default:
					ret = eRelError;
				break;
			}
		break;
		
		case 'M':
			switch(fieldString[pos+2]){
			
				case '1':
					ret = eM_1;
				break;
				
				case 'N':
					ret = eM_N;
				break;
				
				default:
					ret = eRelError;
				break;
		}
		break;
		
		case 'N':
		switch(fieldString[pos+2]){
			
			case '1':
				ret = eN_1;
			break;
			
			case 'N':
				ret = eN_N;
			break;
		
			default:
				ret = eRelError;
			break;
		}
		break;
		
	
		}
	if(ret==eRelError){
		oofE_oofSchemaParser err("oofSchemaParser::FindRelType. Relationship error" );
		RAISE_EXCEPTION(err);
	}
	
	*/
	return ret;

}


/** Given fieldString, fiemd the length of the field */
unsigned long
oofSchemaParser::FindFieldLength(const oofString &fieldString)
{
	
	const int kNameLen = 26;  // sizeof("and a maximum length of : ")
	
	long lengthPos;
	long endlength;
	oofString lengthStr;
	lengthPos = fieldString.find("and a maximum length of : ",0,true);
	if(lengthPos == oofString::kNotFound){
		mFieldHasLength = false;
	}else{
		//mCurrentFieldPos = lengthPos;
		mFieldHasLength = true;
	}
	//convert input to mac type
	// now find the end of this index
	endlength = fieldString.find(13,lengthPos);
	if(endlength == oofString::kNotFound){
		oofE_oofSchemaParser err("oofSchemaParser::FindFieldLength. Corrupt field string" );
		RAISE_EXCEPTION(err);
	}
	lengthStr = fieldString.subString(lengthPos+kNameLen, endlength);
		if(mFieldHasLength )
		return	lengthStr.asUnsignedLong();
	else
		return 0;
}
			  


/** Given fieldString, check if this field is a join field */
bool
oofSchemaParser::IsJoinField(const oofString &fieldString)
{
	long pos = fieldString.find("joining across field: ",0,true);
	if(pos == oofString::kNotFound)
		return false;
	else
		return true;


}


/** Given fieldString, check if this field is a rel rield */
bool
oofSchemaParser::IsRel(const oofString &fieldString)
{
	long relationshipPos = fieldString.find(" is a ",0,true);
	if(relationshipPos == oofString::kNotFound)
		return false;
	else
		return true;


}

/** Given fieldString, check if this field propagates deletes */
bool
oofSchemaParser::PropagatesDeletes(const oofString &fieldString)
{
	long pos = fieldString.find(" and propagating deletes to related records",0,true);
	if(pos == oofString::kNotFound)
		return false;
	else
		return true;


}
	
/*bool 
oofSchemaParser::isOOFSchema(oofFileRef &schemaFile)
{
	loadSchema(schemaFile);
	long ret = mParseBuffer.find("oofschemaVer",0,true);
	if(ret == oofString::kNotFound)
		return false; 
	else
		return true;
}


*/
	
	

	
	
	
/*bool 
schemaParser::isOOFSchema()
{
	loadSchema(schemaFile);
	long ret = mParseBuffer.find("oofschemaVer",0,true);
	if(ret =  oofString::kNotFound)
		return false;
	else
		return true;
}*/

/*void	
oofSchemaParser::loadSchema(oofString &inFile, oofString& ext, oofString& iExt)
{
	
		OOFCTResource resourceInterface;
		resourceInterface.openFile(inFile,connectionName,ext,iExt);
		resourceInterface::loadResource("OFSC",RES_TYPE);
		mParseBuffer.setChars(resourceInterface.resource(),resourceInterface.dataLength)
}
*/

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//		u n R e s o l v e d R e l a t i o n s h i p 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	
/** This object resolves unresolved relationships on schemas just parsed from an oofSchema*/
unResolvedRelationship::unResolvedRelationship(const oofString rhstableName,oofString joinFieldName, dbRelRefBase *lhsField, bool isSet)
{
	mJoinFieldName = joinFieldName;
	mRhsTableName = rhstableName,
 	/*mRhsFieldName = rhsFieldName,we can get this from lhsfieldTablename*/
 	mIsLhs = true;
 	mLhsField = lhsField;
 	mIsSet = isSet;
 	
}
 	
 	
 	
 	/*unResolvedRelationship::unResolvedRelationship(oofString rhsFieldName, dbField *lhsField,bool isSet):
 	mRhsFieldName(rhsFieldName),
 	mIsLhs(true),
 	mLhsField(lhsField)
 	{
 	
 	
 	
 	}*/
 	
 /*	unResolvedRelationship::unResolvedRelationship(oofString rhstableName,oofString	 rhsFieldName, dbField *lhsField,bool isSet):
 	mRhstableName(rhstableName),
 	mRhsFieldName(rhsFieldName),
 	mIsLhs(true),
 	mLhsField(lhsField)
 	{
 	
 	
 	
 	}*/

 void	
 unResolvedRelationship::rhsTableName(const oofString &tName )
 {
 	mRhsTableName = tName;
 
 }
 
const oofString& 
unResolvedRelationship::rhsTableName()
{
	return mRhsTableName;
}

const oofString& 	
unResolvedRelationship::rhsFieldName()
{
	return mRhsFieldName;

}



dbRelRefBase*
unResolvedRelationship::rhsField()
 {
 	return mRhsField;
 }		
 
 void	
 unResolvedRelationship::rhsField(dbRelRefBase* rhsField)
 {
 	mRhsField = rhsField;
 }		



dbRelRefBase * 	
unResolvedRelationship::lhsField()
{
	return	mLhsField;
} 		
 
bool
unResolvedRelationship::isSet()
{
	return mIsSet;
}

bool		
unResolvedRelationship::isLhs()
{
	return mIsLhs;

} 	

void
unResolvedRelationship::setIsRhs()
{
	mIsLhs = false;
}

bool
unResolvedRelationship::isRhs()
{
	return !mIsLhs;
}

	

/**Resolves the relationships  */
void
unResolvedRelationship::resolve()
{
	
		//unResolvedRelationship *theRel = (unResolvedRelationship*) mList[searchIndex];
		//dbRelRefBase *theField = theRel->lhsField();
		if(mJoinFieldName==""){
			//dbRelationship(lhs, rhs);  // declarative functor finishes the linking between actual tables
			dbRelationship(mLhsField,mRhsField);
		}else{
			dbTable *lhsTable = mLhsField->fieldTable();
			dbField *lhsJoinField = lhsTable->field(mJoinFieldName);
			
			//dbField *lhsJoinField = mLhsField->fieldTable()->field(mJoinFieldName);
			dbField *rhsJoinField= mRhsField->fieldTable()->field(mJoinFieldName);
			
			//dbRelationship relationShip(mLhsField,mLhsJoinField,mRhsField,mRhsField,mLhsJoinField);
			dbRelationship (mLhsField, mRhsField,lhsJoinField,rhsJoinField);
			
		
		}
		
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//		r e l a t i o n s h i p s T o I n s t a n t i a t e 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

/**
	Destructor of a class to contain a list of relationships to be instantiated.
*/
relationshipsToInstantiate::~relationshipsToInstantiate()
{
	const unsigned long numRelationships = mList.count();
	for (unsigned long i=0; i<numRelationships; i++) {
		unResolvedRelationship* theRelationShip = (unResolvedRelationship*)mList.value(i);
		delete theRelationShip;
	}

}

/**
	A Class to contain a list of relationships to be instantiated.
*/
void 
relationshipsToInstantiate::append(unResolvedRelationship *relToResolve)
{
	mList.append((unsigned long)relToResolve);
}
 		

/**
	Given a table and field name find a matching unresolved relationship.
*/
void
relationshipsToInstantiate::findMatch(const oofString& tableName, const oofString& fieldName)
{
	
	unsigned long searchIndex;
	
	
	for(searchIndex = mList.index()+1;searchIndex < mList.count();searchIndex++)
	{
		unResolvedRelationship *theRel = (unResolvedRelationship*) mList[searchIndex];
		dbRelRefBase *theField = theRel->lhsField();
		
		oofString	lhsFieldName = theField->fieldName();
		oofString	lhsTableName = theField->fieldTable()->tableName();
		if( (tableName == lhsTableName)&&(fieldName ==lhsFieldName) ){
			//we have a match
			theRel->setIsRhs();// stopp outeer loop  method processing this
			//unResolvedRelationship *parent = (unResolvedRelationship*) mList[searchIndex];
			unResolvedRelationship *parent = (unResolvedRelationship*) mList[mList.index()];
			
			parent->rhsField(theField);
		}
	}
} 	


/**
	Iterate through the list and instantiate all the relationships 
*/
void	
relationshipsToInstantiate::resolveRelationships()
{
	
	unResolvedRelationship *theRel; //= (unResolvedRelationship*) mList.value(searchIndex);
	
	for (mList.start();mList.more();mList.next() )
	{
		theRel = (unResolvedRelationship*) mList.value(mList.index());
		if(theRel->isLhs()){
			//findMatch(theRel->rhsTableName(), theRel->rhsFieldName());
			findMatch(theRel->rhsTableName(), theRel->lhsField()->fieldTable()->tableName());
			
			//no the rhs field name will the name of this table
			theRel->resolve();
		}
	}
}
 	
