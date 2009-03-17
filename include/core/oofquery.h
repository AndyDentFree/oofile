#ifndef H_OOFQUERY
#define H_OOFQUERY

// COPYRIGHT 1994 A.D. Software, All rights reserved

/**
	\file oofquery.h
	Declare query classes.
*/


/**
	\defgroup oofQuery OOFILE Database Query
	Classes that allow you to build queries either:
	- declaratively using operator overloading, eg: People.search(People.name=="Andy");
	- by assembling objects at runtime in your code, or
	- using higher level query interfaces parsing strings, XML or GUI query interfaces.	
	Included in the Open Source OOFILE.
	\ingroup oofDatabase
*/



#ifndef H_OOF1
	#include "oof1.h"
#endif

class dbQueryField ;
class dbQueryLiteral;
class dbQueryLiteralStr;
class dbQueryLiteralStrMultiValue;
class dbQueryBinaryCombo;
class dbQueryBinaryComboOwner;
class dbField;
class dbCompoundField;
class dbTable;
class oofWordParser;


/** 
       Highest level used to assemble queries.
	   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryClause {
public:
	enum BinaryQueryOps {equals, notEquals, lessThan, lessThanOrEqual, greaterThan, greaterThanOrEqual, startsWith, 
                          hasWord, hasAnyWord, hasAllWords, hasWordStartsWith, hasAnyWordStartsWith, hasAllWordsStartsWith};
	enum TrinaryQueryOps {between, outside};
	enum QueryCombinatorialOps {oofAND, oofOR};
	enum QueryClauseTypes {binaryfieldToLiteral, binaryfieldTofield, trinaryFieldToLiterals, entireTable, atomicfield, atomicLiteral, atomicLiteralStr, multiLiteralStr, binaryCombination, builtQuery};

	// use default copy ctor
	virtual ~dbQueryClause() {};
	virtual dbQueryClause* clone() const=0;
	
	virtual dbQueryClause::QueryClauseTypes  queryClauseType() const=0;
	bool isSimpleIndexedSearch() const;
	virtual void getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const;
	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;
	
// combinatorial operators
	dbQueryBinaryCombo operator&(const dbQueryClause&) const;
	dbQueryBinaryCombo operator&&(const dbQueryClause&) const;
	dbQueryBinaryCombo operator|(const dbQueryClause&) const;
	dbQueryBinaryCombo operator||(const dbQueryClause&) const;

	dbQueryBinaryComboOwner* operator|(dbQueryClause* adoptRHS);
	dbQueryBinaryComboOwner* operator||(dbQueryClause* adoptRHS);
	dbQueryBinaryComboOwner* operator&(dbQueryClause* adoptRHS);
	dbQueryBinaryComboOwner* operator&&(dbQueryClause* adoptRHS);

	virtual void describe(ostream&) const=0;
	
protected:
	dbQueryClause() {};
	
// other protocol
// part of testing	dbTable *AppliesToTable();

public:
	static bool wildcardsInString(const char*);
	static char sWildcardMultiple;  // not const so user can change!
	static char sWildcardSingle;
	static bool isWordSearch(dbQueryClause::BinaryQueryOps);
};
//operator=(dbTable lhs, dbQueryClause& rhs);
OOFILE_EXPORT ostream& operator<<(ostream&, dbQueryClause::BinaryQueryOps);
OOFILE_EXPORT ostream& operator<<(ostream&, dbQueryClause::TrinaryQueryOps);
OOFILE_EXPORT ostream& operator<<(ostream&, dbQueryClause::QueryCombinatorialOps);
OOFILE_EXPORT ostream& operator<<(ostream&, dbQueryClause::QueryClauseTypes);


/**
	class for building queries of arbitrary complexity with application logic.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQuery : public dbQueryClause {
public:
	dbQuery();
	// use default copy ctor
	virtual ~dbQuery();
	virtual dbQueryClause* clone() const;
	
	virtual dbQueryClause::QueryClauseTypes  queryClauseType() const;
	virtual void getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const;
	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;

	void operator|=(dbQueryClause* adoptedPortion);
	void operator&=(dbQueryClause* adoptedPortion);
	
	const dbQueryClause* query() const;
	
	virtual void describe(ostream&) const;

private:
	dbQueryClause* mQuery;  // recursively built as binary combos
};


/**
	Binary query where LHS is table, eg: dbTable::hasAllWordsDelimited.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryBinaryNofield : public dbQueryClause {
public:
	dbQueryBinaryNofield(BinaryQueryOps op, dbQueryClause* litOrFld);
	dbQueryBinaryNofield(const dbQueryBinaryNofield&);
	virtual ~dbQueryBinaryNofield();
	virtual dbQueryClause* clone() const;
private:
	dbQueryBinaryNofield& operator=(dbQueryBinaryNofield&);

public:
	virtual dbQueryClause::QueryClauseTypes queryClauseType()  const;
	dbQueryLiteralStr* literalStrClause() const;
	dbQueryLiteralStrMultiValue* literalMultiStrClause() const;
	dbQueryLiteral* literalClause() const;
	dbQueryClause::BinaryQueryOps binaryOperator() const;
	
	virtual void describe(ostream&) const;

protected:
	
// other protocol
// part of testing	dbTable *AppliesToTable();

// data storage
protected:
	dbQueryClause* mRhs;  // owned
	BinaryQueryOps  mBinOp;
};


/**
	Common binary query for field, eg: People.Salary > 90000.
	Simple class returned by C++ overloads like dbChar::operator==
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryBinary : public dbQueryBinaryNofield {
public:
	dbQueryBinary(dbQueryField* fld, BinaryQueryOps op, dbQueryClause* litOrFld);
	dbQueryBinary(BinaryQueryOps op, dbQueryClause* litOrFld);	// for table searches
	dbQueryBinary(const dbQueryBinary&);
	virtual ~dbQueryBinary();
	virtual dbQueryClause* clone() const;
private:
	dbQueryBinary& operator=(dbQueryBinary&);

public:
	virtual dbQueryClause::QueryClauseTypes queryClauseType()  const;
	virtual void getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const;
	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;
	
	const dbField* lhsField() const;
	bool isSimpleIndexedSearch() const;
	
	virtual void describe(ostream&) const;

protected:
	
// other protocol
// part of testing	dbTable *AppliesToTable();

// data storage
protected:
	dbQueryField  *mLhs;  // owned
};


/**
	Binary query combining two sub-queries.
	Simple class returned by C++ overloads like dbQueryClause::operator&&
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryBinaryCombo : public dbQueryClause {
public:
// NOT YET IMPLEMENTED extend to any number of clauses with same op
	dbQueryBinaryCombo(const dbQueryClause*, QueryCombinatorialOps, const dbQueryClause*);
	dbQueryBinaryCombo(const dbQueryBinaryCombo&);
	virtual ~dbQueryBinaryCombo();
	virtual dbQueryClause* clone() const;
private:
	dbQueryBinaryCombo& operator=(dbQueryBinaryCombo&);

public:
	virtual dbQueryClause::QueryClauseTypes queryClauseType()  const;
	dbQueryClause::QueryCombinatorialOps binaryOperator() const;
	const dbQueryClause* item(unsigned int) const;
	unsigned long pairFieldsIfCouldUseCompoundIndex() const;
	dbQueryClause* makeCompoundSearch(const dbCompoundField*);

	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;
	virtual void describe(ostream&) const;

protected:
	
// other protocol
// part of testing	dbTable *AppliesToTable();

// data storage
protected:
// NOTE unlike the other Binary and Trinary, we do NOT own our lhs & rhs
	const dbQueryClause  *mLhs, *mRhs;
	QueryCombinatorialOps  mComboOp;
	char *mTempKeyStoreToDispose, *mTempKeyStoreToDispose2;
};


/**
	Builder class used to combine sub-queries when constructing runtime queries.
	It can delete heap objects unlike the normal combinatorial dbQueryBinaryCombo
	which is used by C++ operator overloads.
	\see dbQuery
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryBinaryComboOwner : public dbQueryBinaryCombo {
public:
	dbQueryBinaryComboOwner(dbQueryClause*, QueryCombinatorialOps, dbQueryClause*);
	// use default copy ctor
	virtual ~dbQueryBinaryComboOwner();		
	virtual dbQueryClause* clone() const;

	virtual void describe(ostream&) const;

private:
	dbQueryClause  *mOurLhs, *mOurRhs;  // owned
};


/**
	Common trinary query for field, eg: People.Salary.between(50000, 90000);
	Very similar to dbQueryBinary but takes two RHS arguments, typically to specify a range.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryTrinary : public dbQueryClause {
public:
	dbQueryTrinary(dbQueryField* fld, TrinaryQueryOps op, dbQueryClause* litOrFldFrom, dbQueryClause* litOrFldTo) :
				mLhs(fld),
				mFrom(litOrFldFrom),
				mTo(litOrFldTo),
				mTrinOp(op)
				{};		
	dbQueryTrinary(const dbQueryTrinary&);
	virtual ~dbQueryTrinary();
	virtual dbQueryClause* clone() const;
private:
	dbQueryTrinary& operator=(dbQueryTrinary&);

public:
	virtual dbQueryClause::QueryClauseTypes queryClauseType()  const;
	virtual void getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const;
	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;

	dbQueryLiteralStr* literalStrFromClause() const;
	dbQueryLiteralStr* literalStrToClause() const;
	dbQueryLiteral* literalFromClause() const;
	dbQueryLiteral* literalToClause() const;
	dbQueryClause::TrinaryQueryOps trinaryOperator() const;
	const dbField* lhsField() const;

	virtual void describe(ostream&) const;

protected:
	
// data storage
protected:
	dbQueryField  *mLhs;  // owned
	dbQueryClause *mFrom, *mTo;  // owned
	TrinaryQueryOps  mTrinOp;
};


/**
	Base for the RHS arguments to combinatorial queries dbQueryBinary, dbQueryTrinary.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteral : public dbQueryClause {
public:
	virtual dbQueryClause::QueryClauseTypes  queryClauseType()  const;
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;
	virtual ~dbQueryLiteral() {};		
	
protected:
	dbQueryLiteral() {};		
	// use default copy ctor
};


/**
	RHS String argument to queries on fields like dbChar.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralStr : public dbQueryLiteral {
public:
	dbQueryLiteralStr(const char* str) :
	mStr(str),
	mCheckedForWildcards(neverChecked)
	{};
	// use default copy ctor
	virtual ~dbQueryLiteralStr() {};
	virtual dbQueryClause* clone() const;

	virtual dbQueryClause::QueryClauseTypes  queryClauseType()  const;
	const char* str() const;
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;
	
	bool hasWildcards();

	virtual void describe(ostream&) const;

// data storage
private:
	OOF_String mStr;
	enum WildcardCheckT { neverChecked, hasNone, hasWild };
	WildcardCheckT mCheckedForWildcards;
}; 


/**
	Abstract base for string fragment queries like OOF_mixKeywordableField::hasAnyWordsOf.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralStrMultiValue : public dbQueryLiteral {
public:
	virtual void start()=0;
	virtual bool more()=0;
	virtual void next()=0;
	virtual const char* current()=0;
	virtual dbQueryClause::QueryClauseTypes  queryClauseType()  const;
};


/**
	String fragment arguments passed in as single string with a delimiter character.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralStrDelimited : public dbQueryLiteralStrMultiValue {
public:
	dbQueryLiteralStrDelimited(const char* str, char delim) :
		mStr(str),
		mDelim(delim) 
	{};
	// use default copy ctor
	virtual ~dbQueryLiteralStrDelimited() {};
	virtual dbQueryClause* clone() const;
	
	virtual void start();
	virtual bool more();
	virtual void next();
	virtual const char* current();

	virtual void describe(ostream&) const;

private:
	void CalcWordLen();
	
// data storage
	const char* mStr;
	const char* mReadFrom;
	const char* mEndStr;
	char mDelim;
	OOF_String mCurrent;	// must copy because items not null terminated
	unsigned long mStrLen, mWordLen;
};


/**
	String fragment arguments passed in as array of C strings.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralStrArray : public dbQueryLiteralStrMultiValue {
public:
	dbQueryLiteralStrArray(const char **schStrs, unsigned long count) :
		mSchStrs(schStrs),
		mCount(count) 
	{};
	// use default copy ctor
	virtual ~dbQueryLiteralStrArray() {};
	virtual dbQueryClause* clone() const;
	
	virtual void start();
	virtual bool more();
	virtual void next();
	virtual const char* current();
 
	virtual void describe(ostream&) const;

// data storage
private:
	const char **mSchStrs;
	unsigned long mCount, mIter;
};


/**
	RHS double argument to queries on fields like dbReal.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralDouble : public dbQueryLiteral {
public:
	dbQueryLiteralDouble(double d) :
	mNum(d) {};
	// use default copy ctor
	virtual ~dbQueryLiteralDouble() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const double mNum;
}; 


/**
	Argument mainly used to support compound key queries ending in binary number.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralBLOB : public dbQueryLiteral {
public:
	dbQueryLiteralBLOB(void* blob, unsigned short len) :
										mBLOB(blob),
										mBLOBlen(len)
										{};
	// use default copy ctor
	virtual ~dbQueryLiteralBLOB() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const void* mBLOB;
	unsigned short mBLOBlen;
}; 


/**
	RHS long argument to queries on fields like dbLong.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralLong : public dbQueryLiteral {
public:
	dbQueryLiteralLong(long l) :
	mNum(l) {};
	// use default copy ctor
	virtual ~dbQueryLiteralLong() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const long mNum;
}; 


/**
	RHS unsigned long argument to queries on fields like dbUlong.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralUlong : public dbQueryLiteral {
public:
	dbQueryLiteralUlong(unsigned long l) :
	mNum(l) {};
	// use default copy ctor
	virtual ~dbQueryLiteralUlong() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const unsigned long mNum;
};


/**
	RHS short argument to queries on fields like dbShort.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralShort : public dbQueryLiteral {
public:
	dbQueryLiteralShort(short s) :
	mNum(s) {};
	// use default copy ctor
	virtual ~dbQueryLiteralShort() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const short mNum;
};


/**
	RHS unsigned short argument to queries on fields like dbUshort.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryLiteralUshort : public dbQueryLiteral {
public:
	dbQueryLiteralUshort(unsigned short s) :
	mNum(s) {};
	// use default copy ctor
	virtual~dbQueryLiteralUshort() {};
	virtual dbQueryClause* clone() const;
	
	virtual const void* binaryContents() const;
	virtual unsigned short literalLen() const;

	virtual void describe(ostream&) const;

// data storage
private:
	const unsigned short mNum;
};


/**
	LHS argument to queries on fields.
   \ingroup oofQuery
*/
class OOFILE_EXPORT dbQueryField : public dbQueryClause {
public:
// constructors
	dbQueryField(const dbField* fld) :
				mField(fld) {};
	// use default copy ctor
	virtual~dbQueryField() {};
	virtual dbQueryClause* clone() const;
	
	const dbField*  field()  const {return mField;};
	virtual dbQueryClause::QueryClauseTypes  queryClauseType()  const;
	virtual void getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const;
	virtual dbQueryClause* cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const;
	
	virtual void describe(ostream&) const;

// data storage
	const dbField* mField;
};



// -------------------------------------------------------
//                        d b Q u e r y 
// -------------------------------------------------------
inline const dbQueryClause*
dbQuery::query() const 
{
	return mQuery;
}


// -------------------------------------------------------
//        d b Q u e r y B i n a r y N o F i e l d
// -------------------------------------------------------
inline dbQueryClause::BinaryQueryOps 
dbQueryBinaryNofield::binaryOperator() const
{
		return mBinOp;
}


// -------------------------------------------------------
//                 d b Q u e r y B i n a r y 
// -------------------------------------------------------
inline const dbField* 
dbQueryBinary::lhsField() const
{
		return mLhs->field();
}


// -------------------------------------------------------
//           d b Q u e r y B i n a r y C o m b o
// -------------------------------------------------------
inline dbQueryClause::QueryCombinatorialOps 
dbQueryBinaryCombo::binaryOperator() const
{
		return mComboOp;
}


// -------------------------------------------------------
//                 d b Q u e r y T r i n a r y 
// -------------------------------------------------------
inline dbQueryClause::TrinaryQueryOps 
dbQueryTrinary::trinaryOperator() const
{
		return mTrinOp;
}


inline const dbField* dbQueryTrinary::lhsField() const
{
		return mLhs->field();
}


// -------------------------------------------------------
//                 d b Q u e r y F i e l d
// -------------------------------------------------------
//inline const dbField*  
//dbQueryField::field()  const
//{
//	return mField;
//}
 
 
// -------------------------------------------------------
//            d b Q u e r y L i t e r a l S t r
// -------------------------------------------------------
inline const char* 
dbQueryLiteralStr::str()  const
{
	return mStr;
}


#endif

