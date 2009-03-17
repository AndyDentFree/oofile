// COPYRIGHT 1994 A.D. Software, All rights reserved

// OOFILE database queries

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h" 
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#include <assert.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// Note: these consts are for future upgrades
char dbQueryClause::sWildcardMultiple = '*';
char dbQueryClause::sWildcardSingle = '?';


// -------------------------------------------------------
//                 d b Q u e r y C l a u s e
// -------------------------------------------------------
void
dbQueryClause::getSearchTables(const dbTable* /*inSchTable*/, dbTable*& outDiffFieldTable, dbTable*& outRelatedTable) const
{
	outRelatedTable = outDiffFieldTable = 0;
}


dbQueryClause* 
dbQueryClause::cloneReplacingTable(const dbTable* /*inTable*/, const dbTable* /*repTable*/) const
{
	return 0;
}

	
bool
dbQueryClause::wildcardsInString(const char* schStr) 
{
	for(int i=0;schStr[i];i++) 
		if((schStr[i]==sWildcardMultiple)||(schStr[i]==sWildcardSingle))
			return true;
	return false;
}


bool 
dbQueryClause::isSimpleIndexedSearch() const
{
	return false;
}


bool 
dbQueryClause::isWordSearch(dbQueryClause::BinaryQueryOps inOp)
{
	const bool ret = (inOp >= hasWord && inOp <= hasAllWordsStartsWith);
	return ret;
}

 
dbQueryBinaryCombo 
dbQueryClause::operator&(const dbQueryClause& rhs) const
{
	return dbQueryBinaryCombo(this, dbQueryClause::oofAND, &rhs);
}


dbQueryBinaryCombo 
dbQueryClause::operator&&(const dbQueryClause& rhs) const
{
	return dbQueryBinaryCombo(this, dbQueryClause::oofAND, &rhs);
}


dbQueryBinaryCombo 
dbQueryClause::operator|(const dbQueryClause& rhs) const
{
	return dbQueryBinaryCombo(this, dbQueryClause::oofOR, &rhs);
}


dbQueryBinaryCombo 
dbQueryClause::operator||(const dbQueryClause& rhs) const
{
	return dbQueryBinaryCombo(this, dbQueryClause::oofOR, &rhs);
}


dbQueryBinaryComboOwner*
dbQueryClause::operator|(dbQueryClause* adoptRHS) 
{
   return new dbQueryBinaryComboOwner(this, dbQueryClause::oofOR, adoptRHS);
}


dbQueryBinaryComboOwner*
dbQueryClause::operator||(dbQueryClause* adoptRHS) 
{
   return new dbQueryBinaryComboOwner(this, dbQueryClause::oofOR, adoptRHS);
}


dbQueryBinaryComboOwner*
dbQueryClause::operator&(dbQueryClause* adoptRHS)
{
   return new dbQueryBinaryComboOwner(this, dbQueryClause::oofAND,adoptRHS);
}


dbQueryBinaryComboOwner*
dbQueryClause::operator&&(dbQueryClause* adoptRHS)
{
   return new dbQueryBinaryComboOwner(this, dbQueryClause::oofAND,adoptRHS);
}


// -------------------------------------------------------
//                 d b Q u e r y 
// -------------------------------------------------------
dbQuery::dbQuery() : mQuery(0)
{
}


dbQuery::~dbQuery()
{
	delete mQuery;
}


dbQueryClause* 
dbQuery::clone() const
{
	return new dbQuery(*this);
}


void 
dbQuery::describe(ostream& os) const
{
	os << "dbQuery (built query):\t";
	mQuery->describe(os);
}


dbQueryClause::QueryClauseTypes  
dbQuery::queryClauseType()  const
{
	return builtQuery;
}


void 
dbQuery::getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const
{
	mQuery->getSearchTables(inSchTable, outDiffFieldTable, outRelatedTable);
}


/**
	conditionally clone if embedded query requires replacement.
*/
dbQueryClause* 
dbQuery::cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const
{
	dbQuery* orphanedReturn = 0;
	dbQueryClause* embeddedNew = mQuery->cloneReplacingTable(inTable, repTable);
	if (embeddedNew) {
		orphanedReturn = new dbQuery();
		*orphanedReturn |= embeddedNew;
	}
	return orphanedReturn;
}


void
dbQuery::operator|=(dbQueryClause* adoptedBit) 
{
	if (adoptedBit) {
		if (mQuery)
			mQuery = *mQuery || adoptedBit;
		else
			mQuery = adoptedBit;
	}
}


void
dbQuery::operator&=(dbQueryClause* adoptedBit) 
{
	if (adoptedBit) {
		if (mQuery)
			mQuery = *mQuery && adoptedBit;
		else
			mQuery = adoptedBit;
	}
}


// -------------------------------------------------------
//        d b Q u e r y B i n a r y N o F i e l d
// -------------------------------------------------------
dbQueryBinaryNofield::dbQueryBinaryNofield(BinaryQueryOps op, dbQueryClause* litOrFld) :
				mRhs(litOrFld),
				mBinOp(op)
{
}


dbQueryBinaryNofield::dbQueryBinaryNofield(const dbQueryBinaryNofield& rhs) :
	mBinOp(rhs.mBinOp)
{
	mRhs = rhs.mRhs->clone();
}


dbQueryBinaryNofield::~dbQueryBinaryNofield()
{
	delete mRhs;
}


dbQueryClause* 
dbQueryBinaryNofield::clone() const
{
	return new dbQueryBinaryNofield(*this);
}


dbQueryBinaryNofield& 
dbQueryBinaryNofield::operator=(dbQueryBinaryNofield&) 
{ 
	assert(0);  // should never be called
	return *this; 
};


void 
dbQueryBinaryNofield::describe(ostream& os) const
{
	os << "dbQueryBinaryNofield:\t";
	os << mBinOp << '\t';
	mRhs->describe(os);
}


dbQueryClause::QueryClauseTypes  
dbQueryBinaryNofield::queryClauseType()  const
{
	return entireTable;
}



dbQueryLiteralStr* 
dbQueryBinaryNofield::literalStrClause() const
{
	if (mRhs->queryClauseType()==atomicLiteralStr) 
		return (dbQueryLiteralStr*) mRhs;  // safe downcast
	else
		return 0;
}


dbQueryLiteralStrMultiValue* 
dbQueryBinaryNofield::literalMultiStrClause() const
{
	if (mRhs->queryClauseType()==multiLiteralStr) 
		return (dbQueryLiteralStrMultiValue*) mRhs;  // safe downcast
	else
		return 0;
}


dbQueryLiteral* 
dbQueryBinaryNofield::literalClause() const
{
// NOTE the following assumes that logic to handle binary logical clauses is a superset of
// that handling strings, and if the difference is important we have tried literalStrClause()
// BEFORE trying this routine
	if ((mRhs->queryClauseType()==atomicLiteral) || (mRhs->queryClauseType()==atomicLiteralStr))
		return (dbQueryLiteral*) mRhs;  // safe downcast, actually to a parent of the exact type (a dbQueryLiteralLong etc.)
	else
		return 0;
}


// -------------------------------------------------------
//                 d b Q u e r y B i n a r y 
// -------------------------------------------------------
dbQueryBinary::dbQueryBinary(dbQueryField* fld, BinaryQueryOps op, dbQueryClause* litOrFld) :
				dbQueryBinaryNofield(op,litOrFld),
				mLhs(fld)
{
	assert(fld->field() && !fld->field()->fieldIsStandalone());
}
 

dbQueryBinary::dbQueryBinary(const dbQueryBinary& rhs) :
	dbQueryBinaryNofield(rhs)
{
	mLhs = (dbQueryField*) rhs.mLhs->clone(); // safe downcast
}


dbQueryBinary::~dbQueryBinary()
{
	delete mLhs;
}


dbQueryClause* 
dbQueryBinary::clone() const
{
	return new dbQueryBinary(*this);
}


dbQueryBinary& 
dbQueryBinary::operator=(dbQueryBinary&) 
{ 
	assert(0);  // should never be called
	return *this; 
};


void 
dbQueryBinary::describe(ostream& os) const
{
	os << "dbQueryBinary:\t";
	mLhs->describe(os);
	os << '\t' << mBinOp << '\t';
	mRhs->describe(os);
}


dbQueryClause::QueryClauseTypes  
dbQueryBinary::queryClauseType()  const
{
	if (mRhs->queryClauseType()==atomicfield)
		return binaryfieldTofield;
	else
		return binaryfieldToLiteral;
}


void 
dbQueryBinary::getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const
{
	mLhs->getSearchTables(inSchTable, outDiffFieldTable, outRelatedTable);
}


/**
	conditionally clone if LHS clause requires replacement.
	\see dbQueryTrinary::cloneReplacingTable
*/
dbQueryClause* 
dbQueryBinary::cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const
{
	dbQueryClause* orphanedReturn = 0;
	assert(mLhs->queryClauseType()==atomicfield);
	dbQueryField* lhsNew = (dbQueryField*) (mLhs->cloneReplacingTable(inTable, repTable));  // safe downcast
	if (lhsNew) {
		orphanedReturn = new dbQueryBinary(lhsNew, mBinOp, mRhs->clone());
	}
	return orphanedReturn;
}


bool 
dbQueryBinary::isSimpleIndexedSearch() const
{
	if (queryClauseType() != binaryfieldToLiteral)
		return false;

	const dbField* schField = lhsField();
	if (!schField->fieldIsIndexed())
		return false;
		
	if (schField->fieldType() != charField)
		return true;  // binary indexed field!
	
	if (binaryOperator() != equals && 
		binaryOperator() != startsWith && 
		binaryOperator() != notEquals 
	)
		return true;  // indexed and can't be wildcard

	dbQueryLiteralStr* strRHS = dbQueryBinary::literalStrClause();
	assert(strRHS);  // as lhs is char field
	return strRHS->hasWildcards();
}

 
// -------------------------------------------------------
//           d b Q u e r y B i n a r y C o m b o
// -------------------------------------------------------
dbQueryBinaryCombo::dbQueryBinaryCombo(const dbQueryClause* lhs, QueryCombinatorialOps op, const dbQueryClause* rhs) :
				mLhs(lhs),
				mRhs(rhs),
				mComboOp(op),
				mTempKeyStoreToDispose(0),
				mTempKeyStoreToDispose2(0)
				{}
				
				
 dbQueryBinaryCombo::~dbQueryBinaryCombo()
{
	delete[] mTempKeyStoreToDispose;
	delete[] mTempKeyStoreToDispose2;
}


dbQueryClause* 
dbQueryBinaryCombo::clone() const
{
	return new dbQueryBinaryCombo(*this);
}


dbQueryBinaryCombo::dbQueryBinaryCombo(const dbQueryBinaryCombo& rhs) :
				mLhs(rhs.mLhs),
				mRhs(rhs.mRhs),
				mComboOp(rhs.mComboOp),
				mTempKeyStoreToDispose(0),
				mTempKeyStoreToDispose2(0)
{}


/**
	conditionally clone if LHS or RHS clause requires replacement.
*/
dbQueryClause* 
dbQueryBinaryCombo::cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const
{
	dbQueryClause* orphanedReturn = 0;
	dbQueryClause* lhsNew =  mLhs->cloneReplacingTable(inTable, repTable); 
	dbQueryClause* rhsNew =  mRhs->cloneReplacingTable(inTable, repTable); 
	if (lhsNew || rhsNew) {
		if (!lhsNew)
			lhsNew = mLhs->clone();
		if (!rhsNew)
			rhsNew = mRhs->clone();
		orphanedReturn = new dbQueryBinaryCombo(lhsNew, mComboOp, rhsNew);
	}
	return orphanedReturn;
}

dbQueryBinaryCombo& 
dbQueryBinaryCombo::operator=(dbQueryBinaryCombo&) 
{ 
	assert(0);  // should never be called
	return *this; 
};


void 
dbQueryBinaryCombo::describe(ostream& os) const
{
	os << "dbQueryBinaryCombo:\t";
	mLhs->describe(os);
	os << '\t' << mComboOp << '\t';
	mRhs->describe(os);
}


dbQueryClause::QueryClauseTypes  
dbQueryBinaryCombo::queryClauseType()  const
{
	return binaryCombination;
}

	
const dbQueryClause* 
dbQueryBinaryCombo::item(unsigned int i) const
{
	assert(i<2);  // for now, later will have a list of items
	if (i==0)
		return mLhs;
	else
		return mRhs;
}


unsigned long 
dbQueryBinaryCombo::pairFieldsIfCouldUseCompoundIndex() const
{
	if (mComboOp != oofAND)
		return 0;  // early failure - they're not ANDed
		
	if (mLhs->queryClauseType() != binaryfieldToLiteral)
		return 0;
		
	if (mRhs->queryClauseType() != binaryfieldToLiteral)
		return 0;
		
	dbQueryBinary* lhs = (dbQueryBinary*) mLhs;  // safe downcast due to above tests 
	if (lhs->binaryOperator() != equals)
		return 0;
		
	dbQueryBinary* rhs = (dbQueryBinary*) mRhs;  // safe downcast due to above tests 
	if (rhs->binaryOperator() == notEquals) 
		return 0;
		
// if get here have an expression that CAN be used with a compound index
// ie:  A==a && B==b -> == ab
// A==a && B.startsWith(b) -> startsWith(ab)
// A==a && B>=b -> between(ab, a+)

	const dbField* fld = lhs->lhsField();
	unsigned long ret = fld->fieldNumber();
	fld = rhs->lhsField();
	ret = (ret << 16) + fld->fieldNumber();
	return ret;
}


dbQueryClause* 
dbQueryBinaryCombo::makeCompoundSearch(const dbCompoundField* fld)
{
// this gets a little complicated:
// we make this look like a string query, BUT 
// either of our components can really be binary values
// and we also must pad out the LHS to its full key width

// WARNING this is only partially implemented, mainly for 2nd being > dbDate
	assert(fld->fieldStorageLen()<USHRT_MAX);
	const unsigned short keyLen = fld->fieldStorageLen();
	const unsigned short firstFieldLen = fld->field(0)->fieldStorageLen();
	char* target = new char[keyLen];  // uh oh - we need to delete this after the query succeeds!
	assert(target);
	delete[] mTempKeyStoreToDispose;  // just in case we are reusing a query object somehow
	delete[] mTempKeyStoreToDispose2;
	mTempKeyStoreToDispose = 0;
	mTempKeyStoreToDispose2 = 0;
	
	assert(mLhs->queryClauseType() == binaryfieldToLiteral);
	dbQueryLiteral* lit = ((dbQueryBinary*) mLhs)->literalClause();  // safe downcast
	assert(lit);
	memset(target, 0, keyLen);  // clear the entire compound key width
// NOT YET IMPLEMENTED - would be a loop for more than 2 segments
	memcpy(target, lit->binaryContents(), lit->literalLen());	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif	
	
	assert(mRhs->queryClauseType() == binaryfieldToLiteral);
	dbQueryBinary* rhs = (dbQueryBinary*) mRhs;  // safe downcast
	dbQueryLiteral* finalLit = rhs->literalClause();
	
	BinaryQueryOps theOp = rhs->binaryOperator();
	if (fld->segment(1)->isReversed()) // need to swap the sense of some ops
		switch(theOp) {
		case greaterThan:
			theOp = lessThan;
			break;
			
		case greaterThanOrEqual:
			theOp = lessThanOrEqual;
			break;
			
		case lessThan:
			theOp = greaterThan;
			break;
			
		case lessThanOrEqual:
			theOp = greaterThanOrEqual;
			break;
			
		default:
			assert(theOp==equals || theOp==startsWith);
			break;
		}
		
	switch(theOp) {
	case greaterThan:
	case greaterThanOrEqual:
		mTempKeyStoreToDispose2 = target;  // and fill 2nd field with 0xFF
		target += firstFieldLen;
		memset(target, 0xFF, keyLen-firstFieldLen);  
		
		target = new char[keyLen]; 
		assert(target);
		mTempKeyStoreToDispose = target;
		memset(target, 0, keyLen);  // clear the entire compound key width
	// NOT YET IMPLEMENTED - would be a loop for more than 2 segments
		memcpy(target, lit->binaryContents(), lit->literalLen());	
		#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
			OOF_MEM_DEBUG_FORCE_POOL_CHECK
		#endif
		target += firstFieldLen;
		memcpy(target, finalLit->binaryContents(), finalLit->literalLen());	
		if (theOp == greaterThan)  // HORRIBLE HACK THAT ONLY WORKS ON DATES
			mTempKeyStoreToDispose2[keyLen-1] = (char) 0xFF;  // EFFECTIVE INCREMENT may now work on int, real etc.
		return new dbQueryTrinary(new dbQueryField(fld), dbQueryClause::between,
			new dbQueryLiteralBLOB(mTempKeyStoreToDispose, keyLen), 
			new dbQueryLiteralBLOB(mTempKeyStoreToDispose2, keyLen));
		break;
		
		
	default:  // for now
		mTempKeyStoreToDispose = target;
		target += firstFieldLen;
		memcpy(target, finalLit->binaryContents(), finalLit->literalLen());	
		#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
			OOF_MEM_DEBUG_FORCE_POOL_CHECK
		#endif
		return new dbQueryBinary(new dbQueryField(fld), 
			rhs->binaryOperator(), 
			new dbQueryLiteralBLOB(mTempKeyStoreToDispose, keyLen)
		);
	}
}



// -------------------------------------------------------
//       d b Q u e r y B i n a r y C o m b o O w n e r
// -------------------------------------------------------
dbQueryBinaryComboOwner::dbQueryBinaryComboOwner(dbQueryClause* lhs, QueryCombinatorialOps op, dbQueryClause* rhs) :
				dbQueryBinaryCombo(lhs, op, rhs),
				mOurLhs(lhs),
				mOurRhs(rhs)
				{}
				
				
dbQueryBinaryComboOwner::~dbQueryBinaryComboOwner()
{
	delete mOurLhs;
	delete mOurRhs;
}


dbQueryClause* 
dbQueryBinaryComboOwner::clone() const
{
	return new dbQueryBinaryComboOwner(*this);
}


void 
dbQueryBinaryComboOwner::describe(ostream& os) const
{
	os << "dbQueryBinaryComboOwner:\t";
	mOurLhs->describe(os);
	os << '\t' << mComboOp << '\t';
	mOurRhs->describe(os);
}


// -------------------------------------------------------
//                 d b Q u e r y T r i n a r y 
// -------------------------------------------------------
// NOTE there is a lot of horrible code in this class
// it is offensively coarse, but reasonably efficient and a long way
// down the priority list for a rewrite.
dbQueryTrinary::dbQueryTrinary(const dbQueryTrinary& rhs) :
	mTrinOp(rhs.mTrinOp)
{
	mLhs = (dbQueryField*) rhs.mLhs->clone(); // safe downcast
	mFrom = rhs.mFrom->clone();
	mTo = rhs.mTo->clone();
}


dbQueryTrinary::~dbQueryTrinary()
{
	delete mLhs;
	delete mFrom;
	delete mTo;
}


dbQueryClause* 
dbQueryTrinary::clone() const
{
	return new dbQueryTrinary(*this);
}


dbQueryTrinary& 
dbQueryTrinary::operator=(dbQueryTrinary&) 
{ 
	assert(0);  // should never be called
	return *this; 
};


void 
dbQueryTrinary::describe(ostream& os) const
{
	os << "dbQueryTrinary:\t";
	mLhs->describe(os);
	os << '\t';
	mFrom->describe(os);
	os << '\t' << mTrinOp << '\t';
	mTo->describe(os);
}


dbQueryClause::QueryClauseTypes  
dbQueryTrinary::queryClauseType()  const
{
		return trinaryFieldToLiterals;
}


void 
dbQueryTrinary::getSearchTables(const dbTable* inSchTable, 
		dbTable*& outDiffFieldTable, 
		dbTable*& outRelatedTable) const
{
	mLhs->getSearchTables(inSchTable, outDiffFieldTable, outRelatedTable);
}



/**
	conditionally clone if LHS clause requires replacement.
	\see dbQueryBinary::cloneReplacingTable
*/
dbQueryClause* 
dbQueryTrinary::cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const
{
	dbQueryClause* orphanedReturn = 0;
	assert(mLhs->queryClauseType()==atomicfield);
	dbQueryField* lhsNew = (dbQueryField*) (mLhs->cloneReplacingTable(inTable, repTable));  // safe downcast
	if (lhsNew) {
		orphanedReturn = new dbQueryTrinary(lhsNew, mTrinOp, mFrom->clone(), mTo->clone());
	}
	return orphanedReturn;
}

	
dbQueryLiteralStr* 
dbQueryTrinary::literalStrFromClause() const
{
	if (mFrom->queryClauseType()==atomicLiteralStr) 
		return (dbQueryLiteralStr*) mFrom;  // safe downcast
	else
		return 0;
}


dbQueryLiteralStr* 
dbQueryTrinary::literalStrToClause() const
{
	if (mTo->queryClauseType()==atomicLiteralStr) 
		return (dbQueryLiteralStr*) mTo;  // safe downcast
	else
		return 0;
}


dbQueryLiteral* 
dbQueryTrinary::literalFromClause() const
{
	if (mFrom->queryClauseType()==atomicLiteral) 
		return (dbQueryLiteral*) mFrom;  // safe downcast
		return 0;
}

	
dbQueryLiteral* 
dbQueryTrinary::literalToClause() const
{
	if (mTo->queryClauseType()==atomicLiteral) 
		return (dbQueryLiteral*) mTo;  // safe downcast
	else
		return 0;
}

	
// -------------------------------------------------------
//                 d b Q u e r y F i e l d
// -------------------------------------------------------
dbQueryClause* 
dbQueryField::clone() const
{
	return new dbQueryField(*this);
}


dbQueryClause::QueryClauseTypes  
dbQueryField::queryClauseType()  const
{
	return atomicfield;
}


void 
dbQueryField::describe(ostream& os) const
{
	os << "dbQueryField:\t" << mField->fieldTable()->tableName() << '\t' << mField->fieldName();
}


/**
	Find tables on which we are searching, if not simply the field's table.
	Copes with two situations:
	1) a search is being performed given a clause written on a different
	  table, and we just need to map tables across
	  
	2) searches on related tables
*/
void 
dbQueryField::getSearchTables(const dbTable* inSchTable, dbTable*& outDiffFieldTable, dbTable*& outRelatedTable) const
{
	outRelatedTable = outDiffFieldTable = 0;
	dbTable* maybeTable = mField->fieldTable();
	if (maybeTable==inSchTable)
		return;	// NOT on related table, it's on the search table (the usual simple case)
	
	// field is not same table as searching
	outDiffFieldTable = maybeTable;	
	dbTable* relatedTable = maybeTable->lhsTableInRelationshipSpec();
	if (relatedTable && 
		(relatedTable->baseTableOfRelationChain()==inSchTable) // for MN and other complex related searches
	){
		outRelatedTable = relatedTable;
		return;
	}
	else {
		if (maybeTable->tableNumber() == inSchTable->tableNumber())
			return;  // still not right but caller can cope
	}
		
// NOT YET IMPLEMENTED - work out if table is related to inTable anyway
// but they have not qualified it, eg: Patients.search(Visits.Date=="1/1/96");
	dbConnect::raise(stringstream() << flush 
		<< "dbQueryField::relatedTable on field " << mField->fieldName()
		<< " in table " << maybeTable->tableName()
		<< " is not related to target table " << inSchTable->tableName(),
		false	/* not necessarily fatal */
	);		
}


/**
	conditionally clone if embedded query requires replacement.
*/
dbQueryClause* 
dbQueryField::cloneReplacingTable(const dbTable* inTable, const dbTable* repTable) const
{
	dbQueryClause* orphanedReturn = 0;
	dbTable* maybeTable = mField->fieldTable();
	if (maybeTable->tableNumber() == inTable->tableNumber())
	{
		dbField* fieldInRepTable = repTable->field (mField->fieldNumber());
		orphanedReturn = new dbQueryField(fieldInRepTable);
	}
	return orphanedReturn;
}





// -------------------------------------------------------
//                 d b Q u e r y L i t e r a l
// -------------------------------------------------------
dbQueryClause::QueryClauseTypes  
dbQueryLiteral::queryClauseType()  const
{
	return atomicLiteral;
}


const void* 
dbQueryLiteral:: binaryContents() const
{
	return 0;
}


unsigned short 
dbQueryLiteral::literalLen() const
{
	return 0;
}


// -------------------------------------------------------
//          d b Q u e r y L i t e r a l L o n g
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralLong::clone() const
{
	return new dbQueryLiteralLong(*this);
}


const void* 
dbQueryLiteralLong:: binaryContents() const
{
	return &mNum;
}


void 
dbQueryLiteralLong::describe(ostream& os) const
{
	os << "dbQueryLiteralDouble:\t" << mNum;
}


unsigned short 
dbQueryLiteralLong::literalLen() const
{
	return sizeof(long);
}


	
// -------------------------------------------------------
//          d b Q u e r y L i t e r a l U l o n g
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralUlong::clone() const
{
	return new dbQueryLiteralUlong(*this);
}


const void* 
dbQueryLiteralUlong:: binaryContents() const
{
	return &mNum;
}


void 
dbQueryLiteralUlong::describe(ostream& os) const
{
	os << "dbQueryLiteralUlong:\t" << mNum;
}


unsigned short 
dbQueryLiteralUlong::literalLen() const
{
	return sizeof(unsigned long);
}


	
// -------------------------------------------------------
//          d b Q u e r y L i t e r a l S h o r t
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralShort::clone() const
{
	return new dbQueryLiteralShort(*this);
}


const void* 
dbQueryLiteralShort:: binaryContents() const
{
	return &mNum;
}


void 
dbQueryLiteralShort::describe(ostream& os) const
{
	os << "dbQueryLiteralShort:\t" << mNum;
}


unsigned short 
dbQueryLiteralShort::literalLen() const
{
	return sizeof(short);
}


	
// -------------------------------------------------------
//          d b Q u e r y L i t e r a l U s h o r t
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralUshort::clone() const
{
	return new dbQueryLiteralUshort(*this);
}


const void* 
dbQueryLiteralUshort:: binaryContents() const
{
	return &mNum;
}


void 
dbQueryLiteralUshort::describe(ostream& os) const
{
	os << "dbQueryLiteralUshort:\t" << mNum;
}


unsigned short 
dbQueryLiteralUshort::literalLen() const
{
	return sizeof(unsigned short);
}


	
// -------------------------------------------------------
//         d b Q u e r y L i t e r a l D o u b l e
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralDouble::clone() const
{
	return new dbQueryLiteralDouble(*this);
}


const void* 
dbQueryLiteralDouble:: binaryContents() const
{
	return &mNum;
}


void 
dbQueryLiteralDouble::describe(ostream& os) const
{
	os << "dbQueryLiteralDouble:\t" << mNum;
}


unsigned short 
dbQueryLiteralDouble::literalLen() const
{
	return sizeof(double);
}


	
// -------------------------------------------------------
//         d b Q u e r y L i t e r a l B L O B
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralBLOB::clone() const
{
	return new dbQueryLiteralBLOB(*this);
}


const void* 
dbQueryLiteralBLOB:: binaryContents() const
{
	return mBLOB;
}


void 
dbQueryLiteralBLOB::describe(ostream& os) const
{
	os << "dbQueryLiteralBLOB:\n";
}


unsigned short 
dbQueryLiteralBLOB::literalLen() const
{
	return mBLOBlen;
}


	
// -------------------------------------------------------
//            d b Q u e r y L i t e r a l S t r
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralStr::clone() const
{
	return new dbQueryLiteralStr(*this);
}


dbQueryClause::QueryClauseTypes  
dbQueryLiteralStr::queryClauseType()  const
{
	return atomicLiteralStr;
}

 
const void* 
dbQueryLiteralStr::binaryContents() const
{
	return mStr;
}


void 
dbQueryLiteralStr::describe(ostream& os) const
{
	os << "dbQueryLiteralStr:\t" << mStr;
}


unsigned short 
dbQueryLiteralStr::literalLen() const
{
	if (!mStr)
		return 0;
	return strlen(mStr);
}


bool
dbQueryLiteralStr::hasWildcards()
{
	if (mCheckedForWildcards == neverChecked) {		
		assert(mStr);
		if (dbQueryClause::wildcardsInString(mStr))
			mCheckedForWildcards = hasWild;
		else
			mCheckedForWildcards = hasNone;
	}
	return (mCheckedForWildcards == hasWild);
	
}


// -------------------------------------------------------
//   d b Q u e r y L i t e r a l S t r M u l t i V a l u e 
// -------------------------------------------------------
dbQueryClause::QueryClauseTypes  
dbQueryLiteralStrMultiValue::queryClauseType()  const
{
	return multiLiteralStr;
}


// -------------------------------------------------------
//      d b Q u e r y L i t e r a l S t r A r r a y
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralStrArray::clone() const
{
	return new dbQueryLiteralStrArray(*this);
}


void 
dbQueryLiteralStrArray::start()
{
	mIter = 0;	
}


bool 
dbQueryLiteralStrArray::more()
{
	return mIter < mCount;
}


void 
dbQueryLiteralStrArray::next()
{
	mIter++;	
}


const char* 
dbQueryLiteralStrArray::current()
{
	return mSchStrs[mIter];
}


void 
dbQueryLiteralStrArray::describe(ostream& os) const
{
	os << "dbQueryLiteralStrArray:\t";
	for (unsigned long i=0; i<mCount; i++) {
		os << mSchStrs[i] << '\t';
	}
}


// -------------------------------------------------------
//   d b Q u e r y L i t e r a l S t r D e l i m i t e d 
// -------------------------------------------------------
dbQueryClause* 
dbQueryLiteralStrDelimited::clone() const
{
	return new dbQueryLiteralStrDelimited(*this);
}


void 
dbQueryLiteralStrDelimited::start()
{
	mReadFrom = mStr;
	mStrLen = strlen(mStr);
	mEndStr = mStr+mStrLen;
	CalcWordLen();
}


bool 
dbQueryLiteralStrDelimited::more()
{
	return mReadFrom < mEndStr;
}


void 
dbQueryLiteralStrDelimited::next()
{
	mReadFrom += mWordLen;	// should be pointing at delimiter
	assert((mReadFrom>=mEndStr) || (*mReadFrom==mDelim));
	mReadFrom++;	// skip delimiter
	CalcWordLen();
}


const char* 
dbQueryLiteralStrDelimited::current()
{
	mCurrent.setChars(mReadFrom, mWordLen);
	return mCurrent;
}


void 
dbQueryLiteralStrDelimited::CalcWordLen()
{
	mWordLen=0;
	for (const char* ip = mReadFrom; ip < mEndStr && *ip!=mDelim; ip++)
		mWordLen++;
}


void 
dbQueryLiteralStrDelimited::describe(ostream& os) const
{
	os << "dbQueryLiteralStrDelimited:\t" << mStr;
}


// -------------------------------------------------------
//            B i n a r y Q u e r y O p s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, dbQueryClause::BinaryQueryOps st)
{
	switch (st) {
	case (dbQueryClause::equals) :
		os << "equals";
		break;

	case (dbQueryClause::notEquals) :
		os << "notEquals";
		break;

	case (dbQueryClause::lessThan) :
		os << "lessThan";
		break;

	case (dbQueryClause::lessThanOrEqual) :
		os << "lessThanOrEqual";
		break;

	case (dbQueryClause::greaterThan) :
		os << "greaterThan";
		break;

	case (dbQueryClause::greaterThanOrEqual) :
		os << "greaterThanOrEqual";
		break;

	case (dbQueryClause::startsWith) :
		os << "startsWith";
		break;

	case (dbQueryClause::hasWord) :
		os << "hasWord";
		break;

	case (dbQueryClause::hasAnyWord) :
		os << "hasAnyWord";
		break;

	case (dbQueryClause::hasAllWords) :
		os << "hasAllWords";
		break;

	case (dbQueryClause::hasWordStartsWith) :
		os << "hasWordStartsWith";
		break;

	case (dbQueryClause::hasAnyWordStartsWith) :
		os << "hasAnyWordStartsWith";
		break;

	default :
		os << "hasAllWordsStartsWith";
	}
	return os;
}
 

// -------------------------------------------------------
//            T r i n a r y Q u e r y O p s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, dbQueryClause::TrinaryQueryOps st)
{
	switch (st) {
	case (dbQueryClause::between) :
		os << "between";
		break;

	default :
		os << "outside";
	}
	return os;
}


// -------------------------------------------------------
//         Q u e r y C o m b i n a t o r i a l O p s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, dbQueryClause::QueryCombinatorialOps st)
{
	switch (st) {
	case (dbQueryClause::oofAND) :
		os << "and";
		break;

	default :
		os << "or";
	}
	return os;
}




// -------------------------------------------------------
//           Q u e r y C l a u s e T y p e s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, dbQueryClause::QueryClauseTypes st)
{
	switch (st) {
	case (dbQueryClause::binaryfieldToLiteral) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::binaryfieldTofield) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::trinaryFieldToLiterals) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::entireTable) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::atomicfield) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::atomicLiteral) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::atomicLiteralStr) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::multiLiteralStr) :
		os << "binaryfieldToLiteral";
		break;

	case (dbQueryClause::binaryCombination) :
		os << "binaryfieldToLiteral";
		break;

	default :
		os << "builtQuery";
	}
	return os;
}
