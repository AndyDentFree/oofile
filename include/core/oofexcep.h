#ifndef H_OOFEXCEP
#define H_OOFEXCEP
// COPYRIGHT 1997 A.D. Software, All rights reserved

// Exceptions objects for OOFILE database
#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif

/**
	\def RAISE_EXCEPTION
	Macro to allow us to either throw an exception or call dbConnect::raise.
	Must be used throughout OOFILE instead of 'throw' to continue to compile
	without exceptions, on older compilers.
	\ingroup oofMacros
*/

#ifdef OOF_USING_EXCEPTIONS
	#define RAISE_EXCEPTION(E) throw(E)
#else
	#define RAISE_EXCEPTION(E) dbConnect::raise(E)
#endif

/**
	\defgroup oofExceptions OOFILE exception hierarchy to provide structured
	error reporting.
	\ingroup appAuthor
*/


class dbConnect;

/**
	Base class for exception hierarchy.
	Declares pure virtual extract() method and with related
	operator<<(ostream& can be used to dump exception to stream
	for easy reporting.
	\ingroup oofExceptions
*/
class OOFILE_EXPORT oofE_Base {
public:
	virtual void extract(ostream&) const=0;
	virtual ~oofE_Base() {};
};
/// \relates oofE_Base
ostream& operator<<(ostream&, const oofE_Base&);


/**
	Exception for database connection.
	\ingroup oofExceptions
*/
class OOFILE_EXPORT oofE_Connection : public oofE_Base {
public:
	oofE_Connection(dbConnect* C) : mConnection(C) {};
	dbConnect* connection() const;
	
private:
	dbConnect*	mConnection;
};


/**
	Exception for database connection to c-tree Plus backend.
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeConnectionError : public oofE_Connection {
public:
	oofE_CtreeConnectionError(dbConnect* C, long errNum, const char* action, const char* where);
	oofE_CtreeConnectionError(dbConnect* C, ostream& os);
	oofE_CtreeConnectionError(dbConnect* C, const char* str);
	long errorNumber() const;

	virtual void extract(ostream& os) const;

private:
	long mErrorNumber;
	oofString mAction;
	oofString mWhere;
	oofString mErrorString;
};


/**
	Exception for database connection to dBase-compatible backend
	\ingroup oofExceptions oofDatabase_dbase
*/
class OOFILE_EXPORT oofE_dBaseConnectionError : public oofE_Connection {
public:
	oofE_dBaseConnectionError(dbConnect* C, long errNum, const char* action, const char* where);
	oofE_dBaseConnectionError(dbConnect* C, ostream& os);
	oofE_dBaseConnectionError(dbConnect* C, const char* str);
	long errorNumber() const;

	virtual void extract(ostream& os) const;

private:
	long mErrorNumber;
	oofString mAction;
	oofString mWhere;
	oofString mErrorString;
};


/**
	Exception for database connection to RAM-based backend
	\ingroup oofExceptions oofDatabase_ram
*/
class OOFILE_EXPORT oofE_RamConnectionError : public oofE_Connection {
public:
	oofE_RamConnectionError(dbConnect* C, const char* str);
	oofE_RamConnectionError(dbConnect* C, ostream& os);
	const oofString& errorString() const;

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Exception for database connection to persistent version of RAM backend
	\ingroup oofExceptions oofDatabase_ramp
*/
class OOFILE_EXPORT oofE_RampConnectionError : public oofE_RamConnectionError {
public:
	oofE_RampConnectionError(dbConnect* C, const char* str);
	oofE_RampConnectionError(dbConnect* C, ostream& os);
	virtual void extract(ostream& os) const;
};



/**
	Base exception for database table operations
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_Table : public oofE_Base, public oofSingleListener {
public:
	oofE_Table(dbTable*);
	oofE_Table(dbTable&);
	oofE_Table(const oofE_Table&);
	virtual ~oofE_Table() {};
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	dbTable* table() const;
	bool originalTableStillInScope() const;
	
private:
	dbTable*	mTable;	// may go out of scope
	tableNumT	mTableNumber;
	bool		mOriginalTableInScope;
	dbConnect*	mConnection;
};


/**
	Exception for database table operation resulting in duplicate record error.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_DuplicateRecord : public oofE_Table {
public:
	oofE_DuplicateRecord(dbTable* T, long datno, const char* action, const char* where);
	oofE_DuplicateRecord(dbTable& T, long datno, const char* action, const char* where);

	virtual void extract(ostream& os) const;
	virtual ~oofE_DuplicateRecord() {};

private:
	long mISAMdatno;
	oofString mAction;
	oofString mWhere;
};


/**
	Exception for database table operation failure due to locking conflict.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_RecordLocked : public oofE_Table {
public:
	oofE_RecordLocked(dbTable* T) : oofE_Table(T) {};
	oofE_RecordLocked(dbTable& T) : oofE_Table(T) {};
	virtual void extract(ostream&) const;
	virtual ~oofE_RecordLocked() {};
};


/**
	Base exception for database field operations.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_Field : public oofE_Base {
public:
	oofE_Field(dbTable* T, dbField* field);
	oofE_Field(dbTable& T, dbField* field);
	virtual ~oofE_Field() {};

	dbTable* table() const;
	dbField* field() const;

private:
	dbTable* mTable;
	dbField* mField;
};


/**
	Exception for database field operations with descriptive error numbers.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_FieldError : public oofE_Field {
public:
	oofE_FieldError(dbTable* T, dbField* field, const char* str);
	oofE_FieldError(dbTable& T, dbField* field, const char* str);
	oofE_FieldError(dbTable* T, dbField* field, ostream& os);
	oofE_FieldError(dbTable& T, dbField* field, ostream& os);
	virtual ~oofE_FieldError() {};

	const oofString& errorString() const;
	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Exception for database field operation violating field data numeric range.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_RangeError : public oofE_Base {
public:
	oofE_RangeError(const char* str);
	oofE_RangeError(ostream& os);
	virtual ~oofE_RangeError() {};

	const oofString& errorString() const;
	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Base class for any GUI exception.
	\ingroup oofExceptions oofGUI
*/
class OOFILE_EXPORT oofE_GUI : public oofE_Base {
public:
	oofE_GUI(dbGUIFactory* gui);
	virtual ~oofE_GUI() {};

private:
	dbGUIFactory* mGUI;
};


/**
	Base class for any GUI exception able to describe error.
	\ingroup oofExceptions oofGUI
*/
class OOFILE_EXPORT oofE_GUIError : public oofE_GUI {
public:
	oofE_GUIError(dbGUIFactory* gui, const char* str);
	oofE_GUIError(dbGUIFactory* gui, ostream& os);
	virtual ~oofE_GUIError() {};

	const oofString& errorString() const;
	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Exception for MFC GUI on MS Windows.
	\ingroup oofExceptions oofGUI
*/
class OOFILE_EXPORT oofE_MFCGUIError : public oofE_GUI {
public:
	oofE_MFCGUIError(dbGUIFactory* gui, dbTable* tbl, const char* str);
	oofE_MFCGUIError(dbGUIFactory* gui, dbTable* tbl, ostream& os);
	virtual ~oofE_MFCGUIError() {};

	const oofString& errorString() const;
	dbTable* table() const;

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
	dbTable*  mTable;
};


/**
	Exception for database selections.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_Selection : public oofE_Base {
public:
	oofE_Selection(const char* str);
	oofE_Selection(ostream& os);
	virtual ~oofE_Selection() {};

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Exception for unclassified use when nothing else is better.
	\ingroup oofExceptions oofUtils
*/
class OOFILE_EXPORT oofE_General : public oofE_Base {
public:
	oofE_General(const oofString&);
	oofE_General(ostream& os);
	virtual ~oofE_General() {};

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Base exception for database operations on the c-tree Plus backend.
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeError : public oofE_Table {
public:
	oofE_CtreeError(dbTable* T, long errNum, const char* action, const char* where);
	oofE_CtreeError(dbTable& T, long errNum, const char* action, const char* where);
	oofE_CtreeError(const oofE_CtreeError&);
	virtual ~oofE_CtreeError() {};

	virtual void extract(ostream& os) const;
	long errorNumber() const;

private:
	long mErrorNumber;	
	oofString mAction;
	oofString mWhere;
};


/**
	Exception where we get a c-tree error number back for diagnosis.
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeISAMError : public oofE_CtreeError {
public:
	oofE_CtreeISAMError(dbTable* T, long datno, long errNum, const char* action, const char* where);
	oofE_CtreeISAMError(dbTable& T, long datno, long errNum, const char* action, const char* where);
	virtual ~oofE_CtreeISAMError() {};

	virtual void extract(ostream& os) const;

private:
	long mISAMdatno;
};


/**
	Exception where we get a c-tree error number and know the record address.
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeISAMErrorAtOffset : public oofE_CtreeISAMError {
public:
	oofE_CtreeISAMErrorAtOffset(dbTable* T, long datno, long offset, long errNum,
		const char* action, const char* where);
	oofE_CtreeISAMErrorAtOffset(dbTable& T, long datno, long offset, long errNum,
		const char* action, const char* where);
	virtual ~oofE_CtreeISAMErrorAtOffset() {};

	virtual void extract(ostream& os) const;

private:
	long mOffset;
};


/**
	Exception where we get a c-tree error number and have the key value causing the error.
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeKeyError : public oofE_CtreeError {
public:
	oofE_CtreeKeyError(dbTable* T, long keyNo, long errNum, const char* action, const char* where);
	oofE_CtreeKeyError(dbTable& T, long keyNo, long errNum, const char* action, const char* where);
	virtual ~oofE_CtreeKeyError() {};

	virtual void extract(ostream& os) const;

private:
	long mKeyNo;
};


/**
	Exception where we get a c-tree error number for an operation on a variable-length BLOB.
	\see dbBLOB
	\ingroup oofExceptions oofDatabase_ctree
*/
class OOFILE_EXPORT oofE_CtreeBlobError : public oofE_CtreeError {
public:
	oofE_CtreeBlobError(dbTable* T, fieldNumT fldNum, long errNum,
		const char* action, const char* where);
	oofE_CtreeBlobError(dbTable& T, fieldNumT fldNum, long errNum,
		const char* action, const char* where);
	oofE_CtreeBlobError(dbTable* T, fieldNumT fldNum, long errNum, long blobPos, long blobLen,
		const char* action, const char* where);
	oofE_CtreeBlobError(dbTable& T, fieldNumT fldNum, long errNum, long blobPos, long blobLen,
		const char* action, const char* where);
	virtual ~oofE_CtreeBlobError() {};

	virtual void extract(ostream& os) const;

private:
	fieldNumT	mFieldNum;
	long		mBlobPos;
	long		mBlobLen;
};


/**
	Base exception for table operations that can report more details.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_TableError : public oofE_Table {
public:
	oofE_TableError(dbTable* T, ostream& os);
	oofE_TableError(dbTable& T, ostream& os);
	oofE_TableError(dbTable* T, const char* str);
	oofE_TableError(dbTable& T, const char* str);
	virtual ~oofE_TableError() {};

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};



/**
	Exception from the OOF_ramBackend database backend.
	\ingroup oofExceptions oofDatabase_ram
*/
class OOFILE_EXPORT oofE_RamError : public oofE_Table {
public:
	oofE_RamError(dbTable* T, ostream& os);
	oofE_RamError(dbTable& T, ostream& os);
	oofE_RamError(dbTable* T, const char* str);
	oofE_RamError(dbTable& T, const char* str);
	const oofString& errorString() const;

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
};


/**
	Base class for exceptions returning MacOS error numbers.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_RampError : public oofE_RamError {
public:
	oofE_RampError(dbTable* T, ostream& os);
	oofE_RampError(dbTable& T, ostream& os);
	oofE_RampError(dbTable* T, const char* str);
	oofE_RampError(dbTable& T, const char* str);

	virtual void extract(ostream& os) const;
};


#ifdef _Macintosh
/**
	Base class for exceptions returning MacOS error numbers.
	\ingroup oofExceptions oofDatabase
*/
class OOFILE_EXPORT oofE_MacOSErr : public oofE_Base {
public:
	oofE_MacOSErr(const OSErr e, const char* extraMsg=0) : 
		mErrNum(e),
		mErrorString(extraMsg)
	{};
	const OSErr& errorNumber() const;

	virtual void extract(ostream& os) const;
	
protected:
	OSErr mErrNum;
	oofString mErrorString;
};

#endif


/**
	Base class for exceptions from login.
	\ingroup oofExceptions experimental oofDatabase
*/
class OOFILE_EXPORT oofE_dbUser : public oofE_Base {
public:
	oofE_dbUser(const oofString&,long errNum);
	oofE_dbUser(ostream& os);
	virtual ~oofE_dbUser() {};
	long errorNumber() const;

	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;
	long mErrorNumber;	

};

inline long
oofE_dbUser::errorNumber() const
{
	return mErrorNumber;
}


/**
	Base class for exceptions from parsing schemae.
	\ingroup oofExceptions experimental
*/
class OOFILE_EXPORT oofE_oofSchemaParser : public oofE_Base {
public:
	oofE_oofSchemaParser(const oofString&);
	oofE_oofSchemaParser(ostream& os);
	virtual ~oofE_oofSchemaParser() {};
	
	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;

};




/**
	Exceptions related to reading c-tree Plus resources.
	\ingroup oofExceptions experimental
*/
class OOFILE_EXPORT oofE_oofCTRes : public oofE_Base {
public:
	oofE_oofCTRes(const oofString&);
	oofE_oofCTRes(ostream& os);
	virtual ~oofE_oofCTRes() {};
	
	virtual void extract(ostream& os) const;

private:
	oofString mErrorString;

};

// -------------------------------------------------------
//                  O O F E _ T a b l e
// -------------------------------------------------------
inline dbTable*
oofE_Table::table() const
{
	return mTable;
}


inline bool
oofE_Table::originalTableStillInScope() const
{
	return mOriginalTableInScope;
}


// -------------------------------------------------------
//              o o f E _ C t r e e E r r o r 
// -------------------------------------------------------
inline long
oofE_CtreeError::errorNumber() const
{
	return mErrorNumber;
}


// -------------------------------------------------------
//    o o f E _ C o n n e c t i o n 
// -------------------------------------------------------
inline dbConnect*
oofE_Connection::connection() const
{
	return mConnection;
}


// -------------------------------------------------------
//    o o f E _ R a m E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_RamError::errorString() const
{
	return mErrorString;
}


// -------------------------------------------------------
//    o o f E _ R a m C o n n e c t i o n E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_RamConnectionError::errorString() const
{
	return mErrorString;
}

// -------------------------------------------------------
//    o o f E _ C t r e e C o n n e c t i o n E r r o r
// -------------------------------------------------------
inline long
oofE_CtreeConnectionError::errorNumber() const
{
	return mErrorNumber;
}


// -------------------------------------------------------
//    o o f E _ R a n g e E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_RangeError::errorString() const
{
	return mErrorString;
}


// -------------------------------------------------------
//    o o f E _ F i e l d
// -------------------------------------------------------
inline dbField*
oofE_Field::field() const
{
	return mField;
}


inline dbTable*
oofE_Field::table() const
{
	return mTable;
}


// -------------------------------------------------------
//    o o f E _ F i e l d E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_FieldError::errorString() const
{
	return mErrorString;
}


// -------------------------------------------------------
//    o o f E _ G U I E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_GUIError::errorString() const
{
	return mErrorString;
}


// -------------------------------------------------------
//    o o f E _ M F C G U I E r r o r
// -------------------------------------------------------
inline const oofString&
oofE_MFCGUIError::errorString() const
{
	return mErrorString;
}


inline dbTable*
oofE_MFCGUIError::table() const
{
	return mTable;
}


#ifdef _Macintosh
// -------------------------------------------------------
//    o o f E _ M a c O S E r r
// -------------------------------------------------------
inline const OSErr& 
oofE_MacOSErr::errorNumber() const
{
	return mErrNum;
}
#endif


#endif
