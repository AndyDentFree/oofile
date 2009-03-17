// COPYRIGHT 1997 A.D. Software, All rights reserved

// Exceptions objects for OOFILE database

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif

// -------------------------------------------------------
//                o o f E _ B a s e
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, const oofE_Base& E)
{
	E.extract(os);
	return os;
}


// -------------------------------------------------------
//                o o f E _ T a b l e
// -------------------------------------------------------
oofE_Table::oofE_Table(dbTable* inTable) :
	oofSingleListener(inTable),	// so we know if it is destroyed
	mTable(inTable),
	mTableNumber(inTable->tableNumber()),
	mOriginalTableInScope(true),
	mConnection(inTable->mConnection)
{	
}


oofE_Table::oofE_Table(dbTable& inTable) :
	oofSingleListener(&inTable),	// so we know if it is destroyed
	mTable(&inTable),
	mTableNumber(inTable.tableNumber()),
	mOriginalTableInScope(true),
	mConnection(inTable.mConnection)
{	
}


oofE_Table::oofE_Table(const oofE_Table& rhs) :
	oofSingleListener(rhs),	
	mTable(rhs.mTable),
	mTableNumber(rhs.mTableNumber),
	mOriginalTableInScope(rhs.mOriginalTableInScope),
	mConnection(rhs.mConnection)
{
}


bool 
oofE_Table::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
{
	if (msg==OOFmsg_BroadcasterClosing) {	// dbTable going out of scope
		mOriginalTableInScope = false;
		mListensTo = 0;
		dbTable* protoTable = mConnection->table(mTableNumber); // now point to prototype
		if (protoTable==mTable)  // this exception thrown as connection being deleted!
			mTable = 0;
		else
			mTable = protoTable;
	}
	return true;
}


// -------------------------------------------------------
//              o o f E _ D u p l i c a t e R e c o r d
// -------------------------------------------------------
oofE_DuplicateRecord::oofE_DuplicateRecord(dbTable* T, long datno, const char* action, const char* where) :
	oofE_Table(T),
	mISAMdatno(datno),
	mAction(action),
	mWhere(where)
{}


oofE_DuplicateRecord::oofE_DuplicateRecord(dbTable& T, long datno, const char* action, const char* where) :
	oofE_Table(T),
	mISAMdatno(datno),
	mAction(action),
	mWhere(where)
{}


void 
oofE_DuplicateRecord::extract(ostream& os) const
{
	os	<< "duplicate record on unique key, attempting to " << mAction
		<< " in function " << mWhere
		<< " on table: " << table()->tableName()
		<< ", ISAM file no " << mISAMdatno;
}


// -------------------------------------------------------
//          o o f E _ R e c o r d L o c k e d
// -------------------------------------------------------
void 
oofE_RecordLocked::extract(ostream& os) const
{
	os
		<< "locked record on table: " << table()->tableName()
		<< "at record number: " << table()->recordNumber()
		<< "oid: " << table()->currentOID();
}


// -------------------------------------------------------
//              o o f E _ C t r e e E r r o r 
// -------------------------------------------------------
oofE_CtreeError::oofE_CtreeError(dbTable* T, long errNum, const char* action, const char* where) :
	oofE_Table(T),
	mErrorNumber(errNum),
	mAction(action),
	mWhere(where)
{}


oofE_CtreeError::oofE_CtreeError(dbTable& T, long errNum, const char* action, const char* where) :
	oofE_Table(T),
	mErrorNumber(errNum),
	mAction(action),
	mWhere(where)
{}


oofE_CtreeError::oofE_CtreeError(const oofE_CtreeError& rhs) : 
	oofE_Table(rhs),
	mErrorNumber(rhs.mErrorNumber),
	mAction(rhs.mAction),
	mWhere(rhs.mWhere)
{}


void
oofE_CtreeError::extract(ostream& os) const
{
	os	<< "c-tree plus error: " << mErrorNumber
		<< " - attempting to " << mAction
		<< " in function: " << mWhere;
	if (table())
		os << " on table: " << table();
}


// -------------------------------------------------------
//            o o f E _ C t r e e I S A M E r r o r
// -------------------------------------------------------
oofE_CtreeISAMError::oofE_CtreeISAMError(dbTable* T, long datno, long errNum,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mISAMdatno(datno)
{}


oofE_CtreeISAMError::oofE_CtreeISAMError(dbTable& T, long datno, long errNum,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mISAMdatno(datno)
{}


void
oofE_CtreeISAMError::extract(ostream& os) const
{
	oofE_CtreeError::extract(os);
	os << ", ISAM file no: " << mISAMdatno;
}


// -------------------------------------------------------
//  o o f E _ C t r e e I S A M E r r o r A t O f f s e t
// -------------------------------------------------------
oofE_CtreeISAMErrorAtOffset::oofE_CtreeISAMErrorAtOffset(dbTable* T, long datno, long offset, long errNum,
									const char* action, const char* where) :
	oofE_CtreeISAMError(T, datno, errNum, action, where),
	mOffset(offset)
{}


oofE_CtreeISAMErrorAtOffset::oofE_CtreeISAMErrorAtOffset(dbTable& T, long datno, long offset, long errNum,
									const char* action, const char* where) :
	oofE_CtreeISAMError(T, datno, errNum, action, where),
	mOffset(offset)
{}


void
oofE_CtreeISAMErrorAtOffset::extract(ostream& os) const
{
	oofE_CtreeISAMError::extract(os);
	os << " offset: " << mOffset;
}


// -------------------------------------------------------
//            o o f E _ C t r e e K e y E r r o r
// -------------------------------------------------------
oofE_CtreeKeyError::oofE_CtreeKeyError(dbTable* T, long keyNo, long errNum, const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mKeyNo(keyNo)
{}


oofE_CtreeKeyError::oofE_CtreeKeyError(dbTable& T, long keyNo, long errNum, const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mKeyNo(keyNo)
{}


void
oofE_CtreeKeyError::extract(ostream& os) const
{
	oofE_CtreeError::extract(os);	// use base method
	os << " index file number: " << mKeyNo;
}


// -------------------------------------------------------
//            o o f E _ C t r e e B l o b E r r o r
// -------------------------------------------------------
oofE_CtreeBlobError::oofE_CtreeBlobError(dbTable* T, fieldNumT fldNum, long errNum,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mFieldNum(fldNum),
	mBlobPos(-1),
	mBlobLen(-1)
{}


oofE_CtreeBlobError::oofE_CtreeBlobError(dbTable& T, fieldNumT fldNum, long errNum,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mFieldNum(fldNum),
	mBlobPos(-1),
	mBlobLen(-1)
{}


oofE_CtreeBlobError::oofE_CtreeBlobError(dbTable* T, fieldNumT fldNum, long errNum,
										 long blobPos, long blobLen,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mFieldNum(fldNum),
	mBlobPos(blobPos),
	mBlobLen(blobLen)
{}


oofE_CtreeBlobError::oofE_CtreeBlobError(dbTable& T, fieldNumT fldNum, long errNum,
										 long blobPos, long blobLen,
										 const char* action, const char* where) :
	oofE_CtreeError(T, errNum, action, where),
	mFieldNum(fldNum),
	mBlobPos(blobPos),
	mBlobLen(blobLen)
{}


void
oofE_CtreeBlobError::extract(ostream& os) const
{
	oofE_CtreeError::extract(os);	// use base method

	os << " in field number: " << mFieldNum;

	dbField* theField = table()!=0 ? table()->field(mFieldNum) : 0;
	if (theField != 0) {
		const oofString fieldName = theField->fieldName();
		if (fieldName.length() > 0)
			os << " (" << fieldName << ")";
		if (mBlobPos > -1 || mBlobLen > -1) {
			os << endl;
			if (mBlobPos > -1)
				os << "record offset: " << mBlobPos << (mBlobLen > -1 ? ", " : "");
			if (mBlobLen > -1)
				os << " blob length: ", mBlobLen;
		}
	}
}


// -------------------------------------------------------
//              o o f E _ T a b l e E r r o r
// -------------------------------------------------------
oofE_TableError::oofE_TableError(dbTable* T, ostream& os) :
	oofE_Table(T),
	mErrorString(os)
{}


oofE_TableError::oofE_TableError(dbTable& T, ostream& os) :
	oofE_Table(T),
	mErrorString(os)
{}


oofE_TableError::oofE_TableError(dbTable* T, const char* str) :
	oofE_Table(T),
	mErrorString(str)
{}


oofE_TableError::oofE_TableError(dbTable& T, const char* str) :
	oofE_Table(T),
	mErrorString(str)
{}


void
oofE_TableError::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ G U I
// -------------------------------------------------------
oofE_GUI::oofE_GUI(dbGUIFactory* gui) :
	oofE_Base(),
	mGUI(gui)
{}


// -------------------------------------------------------
//              o o f E _ G U I E r r o r
// -------------------------------------------------------
oofE_GUIError::oofE_GUIError(dbGUIFactory* gui, const char* str) :
	oofE_GUI(gui),
	mErrorString(str)
{}


oofE_GUIError::oofE_GUIError(dbGUIFactory* gui, ostream& os) :
	oofE_GUI(gui),
	mErrorString(os)
{}


void
oofE_GUIError::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ M F C G U I E r r o r
// -------------------------------------------------------
oofE_MFCGUIError::oofE_MFCGUIError(dbGUIFactory* gui, dbTable* tbl, const char* str) :
	oofE_GUI(gui),
	mErrorString(str),
	mTable(tbl)
{}


oofE_MFCGUIError::oofE_MFCGUIError(dbGUIFactory* gui, dbTable* tbl, ostream& os) :
	oofE_GUI(gui),
	mErrorString(os),
	mTable(tbl)
{}


void
oofE_MFCGUIError::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ S e l e c t i o n
// -------------------------------------------------------
oofE_Selection::oofE_Selection(const char* str) :
	oofE_Base(),
	mErrorString(str)
{}


oofE_Selection::oofE_Selection(ostream& os) :
	oofE_Base(),
	mErrorString(os)
{}


void
oofE_Selection::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//         o o f E _ N o t I m p l e m e n t e d
// -------------------------------------------------------
oofE_General::oofE_General(const oofString& str) :
	oofE_Base(),
	mErrorString(str)
{}


oofE_General::oofE_General(ostream& os) :
	oofE_Base(),
	mErrorString(os)
{}


void
oofE_General::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ F i e l d
// -------------------------------------------------------
oofE_Field::oofE_Field(dbTable* T, dbField* field) :
	oofE_Base(),
	mTable(T),
	mField(field)
{}


oofE_Field::oofE_Field(dbTable& T, dbField* field) :
	oofE_Base(),
	mTable(&T),
	mField(field)
{}


// -------------------------------------------------------
//              o o f E _ F i e l d E r r o r
// -------------------------------------------------------
oofE_FieldError::oofE_FieldError(dbTable* T, dbField* field, const char* str) :
	oofE_Field(T, field),
	mErrorString(str)
{}


oofE_FieldError::oofE_FieldError(dbTable& T, dbField* field, const char* str) :
	oofE_Field(T, field),
	mErrorString(str)
{}


oofE_FieldError::oofE_FieldError(dbTable* T, dbField* field, ostream& os) :
	oofE_Field(T, field),
	mErrorString(os)
{}


oofE_FieldError::oofE_FieldError(dbTable& T, dbField* field, ostream& os) :
	oofE_Field(T, field),
	mErrorString(os)
{}


void
oofE_FieldError::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ R a n g e E r r o r
// -------------------------------------------------------
oofE_RangeError::oofE_RangeError(const char* str) :
	oofE_Base(),
	mErrorString(str)
{}


oofE_RangeError::oofE_RangeError(ostream& os) :
	oofE_Base(),
	mErrorString(os)
{}


void
oofE_RangeError::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//              o o f E _ R a m E r r o r
// -------------------------------------------------------
oofE_RamError::oofE_RamError(dbTable* T, ostream& os) :
	oofE_Table(T),
	mErrorString(os)
{}


oofE_RamError::oofE_RamError(dbTable& T, ostream& os) :
	oofE_Table(T),
	mErrorString(os)
{}


oofE_RamError::oofE_RamError(dbTable* T, const char* str) :
	oofE_Table(T),
	mErrorString(str)
{}


oofE_RamError::oofE_RamError(dbTable& T, const char* str) :
	oofE_Table(T),
	mErrorString(str)
{}


void
oofE_RamError::extract(ostream& os) const
{
	os << "Ram error: " << mErrorString;
}

// -------------------------------------------------------
//              o o f E _ R a m p E r r o r
// -------------------------------------------------------
oofE_RampError::oofE_RampError(dbTable* T, ostream& os) :
	oofE_RamError(T, os)
{}


oofE_RampError::oofE_RampError(dbTable& T, ostream& os) :
	oofE_RamError(T, os)
{}


oofE_RampError::oofE_RampError(dbTable* T, const char* str) :
	oofE_RamError(T, str)
{}


oofE_RampError::oofE_RampError(dbTable& T, const char* str) :
	oofE_RamError(T, str)
{}


void
oofE_RampError::extract(ostream& os) const
{
	os << "Ramp error: " << errorString();
}


// -------------------------------------------------------
//    o o f E _ C t r e e C o n n e c t i o n E r r o r
// -------------------------------------------------------
oofE_CtreeConnectionError::oofE_CtreeConnectionError(dbConnect* C, long errNum,
													 const char* action, const char* where) :
	oofE_Connection(C),
	mErrorNumber(errNum),
	mAction(action),
	mWhere(where)
{}


oofE_CtreeConnectionError::oofE_CtreeConnectionError(dbConnect* C, ostream& os) :
	oofE_Connection(C),
	mErrorNumber(0),
	mErrorString(os)
{}


oofE_CtreeConnectionError::oofE_CtreeConnectionError(dbConnect* C, const char* str) :
	oofE_Connection(C),
	mErrorNumber(0),
	mErrorString(str)
{}


void
oofE_CtreeConnectionError::extract(ostream& os) const
{
	if (mErrorNumber != 0)
		os	<< "c-tree Plus connection error: " << mErrorNumber
			<< ", connection name: " << connection()->connectionName()
			<< " - attempting to " << mAction
			<< " in function: " << mWhere;
	else
		os	<< "c-tree Plus connection error: " << mErrorString;
}


// -------------------------------------------------------
//    o o f E _ d B a s e C o n n e c t i o n E r r o r
// -------------------------------------------------------
oofE_dBaseConnectionError::oofE_dBaseConnectionError(dbConnect* C, long errNum,
													 const char* action, const char* where) :
	oofE_Connection(C),
	mErrorNumber(errNum),
	mAction(action),
	mWhere(where)
{}


oofE_dBaseConnectionError::oofE_dBaseConnectionError(dbConnect* C, ostream& os) :
	oofE_Connection(C),
	mErrorNumber(0),
	mErrorString(os)
{}


oofE_dBaseConnectionError::oofE_dBaseConnectionError(dbConnect* C, const char* str) :
	oofE_Connection(C),
	mErrorNumber(0),
	mErrorString(str)
{}


void
oofE_dBaseConnectionError::extract(ostream& os) const
{
	if (mErrorNumber != 0)
		os	<< "c-tree Plus connection error: " << mErrorNumber
			<< ", connection name: " << connection()->connectionName()
			<< " - attempting to " << mAction
			<< " in function: " << mWhere;
	else
		os	<< "dbase connection error: " << mErrorString;
}

// -------------------------------------------------------
//    o o f E _ R a m C o n n e c t i o n E r r o r
// -------------------------------------------------------
oofE_RamConnectionError::oofE_RamConnectionError(dbConnect* C, const char* str) :
	oofE_Connection(C),
	mErrorString(str)
{}


oofE_RamConnectionError::oofE_RamConnectionError(dbConnect* C, ostream& os) :
	oofE_Connection(C),
	mErrorString(os)
{}


void
oofE_RamConnectionError::extract(ostream& os) const
{
	oofString connectionName(connection()->connectionName());

	os	<< "Ram connection error: " << mErrorString;
	if (connectionName.length() > 0)
		os << ", connection name: " << connectionName;
}


// -------------------------------------------------------
//    o o f E _ R a m p C o n n e c t i o n E r r o r
// -------------------------------------------------------
oofE_RampConnectionError::oofE_RampConnectionError(dbConnect* C, const char* str) :
	oofE_RamConnectionError(C, str)
{}


oofE_RampConnectionError::oofE_RampConnectionError(dbConnect* C, ostream& os) :
	oofE_RamConnectionError(C, os)
{}


void
oofE_RampConnectionError::extract(ostream& os) const
{
	oofString connectionName(connection()->connectionName());

	os	<< "Ramp connection error: " << errorString();
	if (connectionName.length() > 0)
		os << ", connection name: " << connectionName;
}


// -------------------------------------------------------
//    o o f E _ d b U s e r
// -------------------------------------------------------


oofE_dbUser::oofE_dbUser(const oofString& str, long errorNumber):
	mErrorString(str),
	mErrorNumber(errorNumber)
{}

oofE_dbUser::oofE_dbUser( ostream& os) :
	mErrorString(os),
	mErrorNumber(0)
{}

void
oofE_dbUser::extract(ostream& os) const
{
	os << mErrorString;
}


// -------------------------------------------------------
//    o o f E _ o o f S c h e m a P a r s e r
// -------------------------------------------------------


oofE_oofSchemaParser::oofE_oofSchemaParser(const oofString& str):
	mErrorString(str)
	{}

oofE_oofSchemaParser::oofE_oofSchemaParser( ostream& os) :
	mErrorString(os)
{}

void
oofE_oofSchemaParser::extract(ostream& os) const
{
	os << mErrorString;
}

// -------------------------------------------------------
//   o o f E _ o o f C T R e s  
// -------------------------------------------------------


oofE_oofCTRes::oofE_oofCTRes(const oofString& str):
	mErrorString(str)
	{}

oofE_oofCTRes::oofE_oofCTRes( ostream& os) :
	mErrorString(os)
{}

void
oofE_oofCTRes::extract(ostream& os) const
{
	os << mErrorString;
}


#ifdef _Macintosh
// -------------------------------------------------------
//    o o f E _ M a c O S E r r
// -------------------------------------------------------
void
oofE_MacOSErr::extract(ostream& os) const
{
	os	<< "Mac OS Error number: " << mErrNum;
	if (mErrorString.length()>0)
		os << " " << mErrorString;
}

#endif

