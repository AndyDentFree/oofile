// COPYRIGHT 1994 A.D. Software, All rights reserved

// OOFILE database field-related classes
// see also oof4.cpp

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef OOF_EXCEP
	#include "oofexcep.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h" 
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#ifndef H_OOFRELX
	#include "oofrelx.h"
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef _MSC_VER
// suppress warning 'this' used in base member init list
	#pragma warning( disable : 4355 )	
#endif

bool dbText::sConvertsLineEndingsOnLoad = false;

// -------------------------------------------------------
//                      d b F i e l d 
// -------------------------------------------------------

dbField::dbField(const OOF_IndexOptions indexOptions) :
	mBackend(0),
	mTable(0),
	mFieldNumber(0),
	mCalculator(0),
	mIndexOptions(indexOptions),
	mPartialKeyLength(kKeyLengthAutoCalculated),
	mCalculateOnlyDefaults(false),
	mStoreCalculatedValues(false)
{
#ifdef OOF_DEBUG
	if(dbTable::sCurrentlyConstructing==0) {
		dbConnect::raise(stringstream() << flush << "un-named field type"
		<< " is defined AFTER the call to newConnection or openConnection, probably by accident, and is not part of a table", false);   
	} else
#endif
		dbTable::sCurrentlyConstructing->attachfield(this);
}


dbField::dbField(const char* fieldName) :
	mBackend(0),
	mTable(0),
	mFieldNumber(0),
	mCalculator(0),
	mIndexOptions(kNotIndexed),
	mPartialKeyLength(kKeyLengthAutoCalculated),
	mFieldName(fieldName),
	mCalculateOnlyDefaults(false),
	mStoreCalculatedValues(false)
{
#ifdef OOF_DEBUG
	if(dbTable::sCurrentlyConstructing==0) {
		dbConnect::raise(stringstream() << flush << "field '" << fieldName << "' of unknown type"
		<< " is defined AFTER the call to newConnection or openConnection, probably by accident, and is not part of a table", false);   
	} else
#endif
	dbTable::sCurrentlyConstructing->attachfield(this);
}


dbField::dbField(const char *fieldName, const OOF_IndexOptions indexOptions) :
	mBackend(0),
	mTable(0),
	mFieldNumber(0),
	mCalculator(0),
	mIndexOptions(indexOptions),
	mPartialKeyLength(kKeyLengthAutoCalculated),
	mFieldName(fieldName),
	mCalculateOnlyDefaults(false),
	mStoreCalculatedValues(false)
{
#ifdef OOF_DEBUG
	if(dbTable::sCurrentlyConstructing==0) {
		dbConnect::raise(stringstream() << flush << "field: '" << fieldName
		<< "' is defined AFTER the call to newConnection or openConnection, probably by accident, and is not part of a table", false);   
	} else 
#endif
	dbTable::sCurrentlyConstructing->attachfield(this);
}


dbField::dbField(const dbField& rhs) :
	OOF_PublicBase(rhs),
	mBackend(rhs.mBackend),
	mTable(0),
	mFieldNumber(rhs.mFieldNumber),
	mCalculator(0),
	mIndexOptions(rhs.mIndexOptions),
	mPartialKeyLength(rhs.mPartialKeyLength),
	mFieldName(rhs.mFieldName),
	mCalculateOnlyDefaults(rhs.mCalculateOnlyDefaults),
	mStoreCalculatedValues(rhs.mStoreCalculatedValues)
{
	if (!rhs.fieldIsStandalone()) {
		dbTable* theTable = dbTable::sCurrentlyConstructing;
		theTable->attachfield(this);
	}
	if (rhs.mCalculator)
		mCalculator = rhs.mCalculator->cloneOwned(this);
}


dbField::dbField(dbField::FieldIsStandaloneT) :
	mBackend(0),
	mTable(0),
	mFieldNumber(0),
	mCalculator(0),
	mIndexOptions(kNotIndexed),
	mPartialKeyLength(kKeyLengthAutoCalculated),
	mCalculateOnlyDefaults(false),
	mStoreCalculatedValues(false)
{
} 


dbField::~dbField()
{
	delete mCalculator;
}


oofString 
dbField::name() const
{
	return fieldName();
}


void 
dbField::CopyCalculatedValueToBuffer()
{
	assert(0);	// dummy base method for virtual fields and BLOB
}


bool 
dbField::isEmpty() const
{ 
	return false; 	// overriden for dbBLOB and dbChar
}


bool 
dbField::validateContents() 
{ 
	return true; 
}


dbQueryClause*	
dbField::valueAsQueryLiteral() const
{
#ifdef OOF_DEBUG
	dbConnect::raise("You must override dbField::valueAsQueryLiteral() for field types that may be uniquely indexed");
#endif
	assert(0);	// must override for searchable fields
	return 0;	
}


void 
dbField::setName(const char* name)
{ 
	mFieldName = name; 
}


const char*
dbField::tableName() const
{ 
	if (fieldIsStandalone())
		return "";
	else
		return  mTable->tableName(); 
}


void 
dbField::index(const OOF_IndexOptions indexOpt) 
{
// allowed to turn off existing index
//	assert((indexOpt==kNotIndexed)  || (mIndexOptions==kNotIndexed) || (mIndexOptions==indexOpt));		// NOT ALREADY SPECIFIED as different index
  mIndexOptions=indexOpt;
} 


 
unsigned long 
dbField::fieldMinWrapWidth() const
{
	const unsigned long kArbitraryWrapThreshold = 12;
	unsigned long ret = fieldStorageLen();
	if (ret==0)  // may happen with empty standalone oofChar
		ret = 1;
	else 	if (ret > kArbitraryWrapThreshold)
		ret = kArbitraryWrapThreshold;
	return ret;
}


unsigned long 
dbField::fieldUsualWrapWidth() const
{
	unsigned long ret = fieldMinWrapWidth();
	return ret;
}


const char* 
dbField::asChars() const
{
	assert(!"dbField::asChars is never meant to be called, use copyString instead as a virtual method");
	return"";
}


oofWordParser* 
dbField::words() const
{
	return 0;
}


const void* 
dbField::binaryContents() const
{
	assert(!fieldIsStandalone());
	{
		return mBackend->getFieldReadFrom(mFieldNumber);
	}
}


bool
dbField::usingCalculator() const
{
	bool ret = false;
	if (mCalculator) {
		if (mCalculateOnlyDefaults)
			ret = mTable->isNewRecord() && !isDirty();	// ie: new, untouched, unsaved
		else
			ret = true;
	}
	return ret;
}


void
dbField::saveDefaultCalculatedValue()
{
	assert(mTable->isNewRecord());
	if (mCalculateOnlyDefaults && !isDirty()) {
		CopyCalculatedValueToBuffer();
	}
}


void
dbField::saveStoredCalculatedValue()
{
	if (mStoreCalculatedValues) {
		CopyCalculatedValueToBuffer();
	}
}


bool	
dbField::valueIsDuplicate() const
{
	dbQueryClause* rhsLiteral = valueAsQueryLiteral();
	dbQueryBinary dupQuery(new dbQueryField(this), dbQueryClause::equals, rhsLiteral);
	bool ret = mBackend->checkDuplicate(&dupQuery);
	return ret;
}


OOF_fieldTypes 
dbField::nativeType() const
{
	return fieldType();  // only override for things like dbDate which can be treated as a different native type (ulong)
}


dbTable* 
dbField::fieldTable() const
{
	return mTable;  
}



/**
Checks if any relationships in field's table use field as join key.
Relatively expensive because it's iterating over all fields but all the
function calls below are inline queries so will compile down well in release mode.

WARNING Doesn't fan out - if any other tables use this field as a join key then
they are doing really weird stuff and the application programmer must add their
own checks. 

This feature originally added to support OOF_repEditSetupVisitor::VisitViewBandBody
*/
bool
dbField::fieldIsJoinKey() const
{
	if (fieldIsStandalone())
		return false;
	
	// couple of local const cached members for faster lookup
	const dbTable* ourTable = mTable;
	const fieldNumT thisFieldNumber = mFieldNumber;  
	
	const fieldNumT fieldsInTable = ourTable->numFields();
	for (fieldNumT i=0; i<fieldsInTable; i++) {
		if (i!=thisFieldNumber) {
			const dbField* anotherField = ourTable->field(i);
			if (anotherField->fieldType()==relationshipField) {
				const dbRelRefBase* asRel = (const dbRelRefBase*) anotherField;   // safe downcast
				if (asRel->joinField()==this)
					return true;
			}  // is rel
		}  // skip current field
	} // loop fields
	return false;
}


dbField* 
dbField::equivalentFieldFromTable(dbTable* baseTable) const
{
//get the equivalent field in the baseTable, or related table to baseTable
// NOT YET IMPLEMENTED - cope with circular relationships! 
	dbField* ret = 0;
	
	if (fieldIsStandalone())
		ret = clone();
	else {
		const tableNumT baseTableNum = baseTable->tableNumber();
		const tableNumT currentTableNum = mTable->tableNumber();
		if (baseTableNum == currentTableNum)
			ret = baseTable->field(mFieldNumber);	// common, trivial case
		else {  // assume baseTable somewhere to our left, need to trace exact relationship chain
			dbRelRefBase* relToLHS = mTable->controllingRelationship();
			if (relToLHS) {
				dbTable* lhsTable = relToLHS->relatedTable();
				dbRelRefBase* pointerToOurTable = relToLHS->inverse();
				dbField* equivField = pointerToOurTable->equivalentFieldFromTable(baseTable);  // RECURSE
				assert(equivField->fieldType()==relationshipField);
				dbRelRefBase* pointerToEquivTable = (dbRelRefBase*) equivField;  // safe downcast
				dbTable* wantedTable = pointerToEquivTable->relatedTable();
				assert(currentTableNum == wantedTable->tableNumber());
				ret = wantedTable->field(mFieldNumber);
			}
			else
				dbConnect::raise("dbField::equivalentFieldFromTable failed for unrelated fields",false);
		}
	}
	return ret;
}


bool 
dbField::fieldIsBlob() const
{
	return false;  
}


bool 
dbField::fieldIsBinary() const
{
	return true;  
}


bool 
dbField::fieldIsVirtual() const
{
	bool ret = (mCalculator && !mCalculateOnlyDefaults);	// a calculator replaces our need for storage
	return ret;
}
	
	
bool 
dbField::fieldIsIndexed() const
{
	return mIndexOptions!=kNotIndexed;
} 


bool 
dbField::fieldIsKeywordIndexed() const
{
	return false;
} 


bool 
dbField::fieldIsUniqueIndexed() const
{
	bool ret = ((mIndexOptions & kIndexNoDups)!=0);
	return ret;
} 


unsigned short 
dbField::fieldKeyLen() const
{
	unsigned short ret = 0;
	if (fieldIsIndexed()) {
		if (fieldIsPartiallyIndexed())
			ret = mPartialKeyLength;
		else
			ret = fieldStorageLen();
	}
	return ret;
}


void
dbField::CalculateWith(dbCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
// NOTE we can't check indexed status here as the index() method can be called
// later, so we don't set mStoreCalculatedValues
	if (mCalculator) {
		if (!adoptedCalculator) {
			if (!mCalculateOnlyDefaults) {
				oofE_FieldError excp(fieldTable(), this,
					"You can't remove a calculator by passing in a nil parameter unless the calculator is used only for defaults");
				RAISE_EXCEPTION(excp);
			}
		}
		delete mCalculator;
	}
	else {
// only set mCalculateOnlyDefaults if we don't have a calculator
// as can't change this behaviour once set - eg: too dangerous to try to convert
// a calc field into a calcDefaultsOnly field
		if (useOnlyForDefaults) {
			useCalculatorOnlyForDefaults();
			mBackend->fieldHasDefaultCalculator(this);
		}
	}	
	mCalculator = adoptedCalculator;
}
 

oofString
dbField::copyString() const
{
	stringstream oss;
	extract(oss);
	return oofString(oss);
}


void 
dbField::getChars(char* ioBuff, unsigned long& ioLen) const
{
#if OOF_USE_ANSI_STREAMS
	ostringstream oss;
	extract(oss);
	string ossStr = oss.str();	// copied 
	
	const unsigned long ossLen = ossStr.size();
	if (ossLen<ioLen)  // less data than user buffer
		ioLen = ossLen;  // so return accurate length
	// else just fill the buffer they gave us
	memcpy(ioBuff, ossStr.c_str(), ioLen);		// NOT assuming null terminated strings
// the stream is not constructed using our buffer that we pass in,
// so we need to copy the output

#else
// old streams directly use our buffer
	ostringstream oss(ioBuff, ioLen);
	extract(oss);
	ioLen = oss.pcount();	// now that we've appended our null on the end 
#endif
}


void 
dbField::extract(ostream& os) const
{
	os << "";
}


bool 
dbField::insert(istream& is, char fieldSep, char recSep)
{
	char c = is.peek();
	while (is.good()) {
		if ((c==fieldSep) || (c==recSep)) {
			return true;  // OK - hit a separator
		}
		is >> c;  // just swallowing this character
		c = is.peek();
	}
	return false;  // fell through without hitting separator
}


istream& 
operator>>(istream& is, dbField* fld)
{
	fld->insert(is, '\t', '\n');
	return is;
}


istream& 
operator>>(istream& is, dbField& fld)
{
	fld.insert(is, '\t', '\n');
	return is;
}


void 
dbField::describe(ostream& os) const
{
	if (mFieldName.isEmpty())
		os << "Un-named field";
	else
		os << "field: " << mFieldName;
	os << "\t type: " << fieldType() << endl;
	if (fieldIsIndexed())
		os << "\tindex: " << fieldIndexOptions() << endl;
}


bool 
dbField::fieldIsSameTypeAs(const dbField* rhs) const
{
	return (fieldType()==rhs->fieldType());
}


bool
dbField::isDirty() const
{
	if (fieldIsStandalone())
		return true;
	return mBackend->fieldIsDirty(mFieldNumber);
}


// -------------------------------------------------------
//                  d b F i x e d B i n a r y
// -------------------------------------------------------

dbFixedBinary::dbFixedBinary(long fieldWidth, const OOF_IndexOptions indexOptions) :
	dbField(indexOptions),
	mBodyLength(fieldWidth)
{}


dbFixedBinary::dbFixedBinary(long fieldWidth, const char *fieldName)  :
	dbField(fieldName),
	mBodyLength(fieldWidth)
{}


dbFixedBinary::dbFixedBinary(long fieldWidth, const char *fieldName, 
								const OOF_IndexOptions indexOptions)  :
	dbField(fieldName, indexOptions),
	mBodyLength(fieldWidth)
{}


dbField*
dbFixedBinary::clone() const 
{
	return new dbFixedBinary(*this);
}


OOF_fieldTypes 
dbFixedBinary::fieldType() const
{
	return fixedBinaryField; 
}


unsigned long 
dbFixedBinary::fieldStorageLen() const
{
	return mBodyLength; 
}


dbFixedBinary& 
dbFixedBinary::operator=(const dbFixedBinary& rhs)
{
	if (this == &rhs)  // MUST use this form to avoid invoking the operator== used for searches
		return *this;
	
	setValue(rhs.binaryContents());
	return *this;
}


void 
dbFixedBinary::clear()
{
	assert(!fieldIsStandalone());
	char *writeTo = (char*)mBackend->getFieldWriteDest(this);
	if (writeTo) {
		memset(writeTo, 0, mBodyLength);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}	
	else
		;  // error NOT YET IMPLEMENTED
	mTable->markDirty();
}


void 
dbFixedBinary::setString(const char* str)
{
	dbFixedBinary::operator=(str);
}


dbFixedBinary&  
dbFixedBinary::operator=(const char* str)
{
	if (!str)
		clear();
	else {
		assert(strlen(str) <= mBodyLength);
		setValue(str);
	}
	return *this;
}



void 
dbFixedBinary::setValue(const void* str)
{
	assert(!fieldIsStandalone());
	char *writeTo = (char*)mBackend->getFieldWriteDest(this);
	if (writeTo) {
		memcpy(writeTo, str, mBodyLength);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}	
	else
		;  // error NOT YET IMPLEMENTED
	mTable->markDirty();
}


void 
dbFixedBinary::setByte(char c, unsigned long offset)
{
	assert(!fieldIsStandalone());
	char *writeTo = (char*) mBackend->getFieldWriteDest(this);
	if (writeTo) {
		writeTo[offset] = c;
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}	
	else
		;  // error NOT YET IMPLEMENTED
	mTable->markDirty();
}


void 
dbFixedBinary::extract(ostream& os) const
{
	const char *asChars = (char*)binaryContents();
	os << hex;
	for (unsigned int i=0; i<mBodyLength; i++)
		os << asChars[i] << ' ';
}



void 
dbFixedBinary::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom) {
		OOF_String theStr;
		const unsigned long randLen = rand()%mBodyLength;
		theStr.makeRandomChars(randLen);
		dbFixedBinary::setString(theStr);
	}
	else {
		OOF_CheckString theStr(seedOrCount, '-');
		dbFixedBinary::setString(theStr);
	}
}


void 
dbFixedBinary::describe(ostream& os) const
{
	dbField::describe(os);
	os << "    and a length of : " << mBodyLength << endl;
}


bool 
dbFixedBinary::fieldIsSameTypeAs(const dbField* rhs) const
{
	if (dbField::fieldIsSameTypeAs(rhs))
		return (mBodyLength == ((const dbFixedBinary*)rhs)->mBodyLength);	// safe downcast
	else
		return false;
}



// -------------------------------------------------------
//                      d b C h a r
// -------------------------------------------------------
dbChar::dbChar(unsigned long fieldWidth, const char* fieldName):
				dbField(fieldName),
				OOF_mixKeywordableField(this),
				mMaxLength(fieldWidth)
{
	CompleteInit();
}


dbChar::dbChar(unsigned long fieldWidth, const OOF_IndexOptions indexOptions) :
				dbField(indexOptions),
				OOF_mixKeywordableField(this),
				mMaxLength(fieldWidth)
{
	CompleteInit();
}



dbChar::dbChar(unsigned long fieldWidth, const char* fieldName, const OOF_IndexOptions indexOptions)  :
				dbField(fieldName, indexOptions),
				OOF_mixKeywordableField(this),
				mMaxLength(fieldWidth)
{
	CompleteInit();
}


dbChar::dbChar(const dbChar& rhs) : 
				dbField(rhs), 
				OOF_mixKeywordableField(rhs, this),
				mMaxLength(rhs.mMaxLength)
{
	CompleteInit();
	if (fieldIsStandalone()) {
		strcpy((char*) mBackend, (const char*) rhs.mBackend);
	}
}
 

dbChar::dbChar(dbField::FieldIsStandaloneT k, unsigned long fieldWidth) :
				dbField(k),
				OOF_mixKeywordableField(this),
				mMaxLength(fieldWidth)
{
	CompleteInit();
}

 
void
dbChar::CompleteInit()
{
	if (fieldIsStandalone()) {
		mBackend = (OOF_tableBackend*) (new char[mMaxLength+1]);
		((char*)mBackend)[0] = '\0';
	}
}
 
 
dbChar::~dbChar()
{
	 if (fieldIsStandalone())
		delete[] (char*) mBackend;
}


dbField*
dbChar::clone() const 
{
	return new dbChar(*this);
}


dbChar&
dbChar::calculateWith(dbCharCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbChar::getChars(char* ioBuff, unsigned long& ioLen) const
{
	const char* from = (const char*) *this;
	unsigned long outLen = strlen(from);
	if (ioLen < outLen)
		outLen = ioLen;
	else
		ioLen = outLen;
	memcpy(ioBuff, from, outLen);
}


oofString 
dbChar::copyString() const
{
	return oofString( (const char*) *this);
}


void 
dbChar::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==charField)
		*this = *(const dbChar*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbChar::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbChar::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==charField) {
		const char* rhs = *(const dbChar*)srcField;  // safe downcast
		if (strcmp(*this, rhs))
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbChar::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbChar::fieldType() const
{
	return charField; 
}


bool 
dbChar::fieldIsKeywordIndexed() const
{
	return (mParser!=0);
} 


bool 
dbChar::fieldIsBinary() const
{
	return false;  
}


unsigned long 
dbChar::fieldUsualWrapWidth() const
{
	unsigned long ret = mMaxLength;	// NOT YET IMPLEMENTED - settable default
	if (ret==0)  // may happen with empty standalone oofChar
		ret = 1;
	return ret;
}


unsigned long 
dbChar::fieldStorageLen() const
{
	return mMaxLength; 
}


unsigned short 
dbChar::length() const
{
	return strlen(*this);
}


bool 
dbChar::isEmpty() const
{ 
	return (length()==0);
}


const char*
dbChar::asChars() const
{
	return (const char*) *this;
}


oofWordParser* 
dbChar::words() const
{
	return wordParser();
}


dbChar::operator const char*() const
{
	const char* ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) {
			ret = (const char*) mBackend;
		}
		else {
			ret = (const char*) (mBackend->getFieldReadFrom(mFieldNumber) );
			if (!ret)
				ret = "";
		}
	}
	return ret;
}


#ifdef _Macintosh
void 
dbChar::getStr255(Str255 s) const
{
	const char* from = (const char*) *this;
	s[0] = strlen(from);
	memcpy(s+1, from, s[0]);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}

void 
dbChar::setStr255(const Str255 s)
{
	long charLen = s[0];
	const char* strStart = (const char*) s+1;
	setChars(strStart, charLen);
}



// end of Mac-specific extra conversions
#endif


void 
dbChar::setNumber(long n)
{
	OOF_CheckString numberAsWords(n);
	dbChar::setString(numberAsWords);
}


dbChar& 
dbChar::operator=(const dbChar& rhs)
{
	if (this == &rhs)  // MUST use this form to avoid invoking the operator== used for searches
		return *this;
	
	*this = (const char*)rhs;
	return *this;
}


dbChar&  
dbChar::operator=(const char* str)
{
	if (!str)
		clear();
	else
		setChars(str, strlen(str));
	return *this;
}


dbChar&   
dbChar::operator=(char ch)
{
	char	toStr[2];
	toStr[0] = ch;
	toStr[1] = '\0';
	setChars(toStr, 1);
	return *this;
}


void 
dbChar::operator+=(char ch)
{
	char	toStr[2];
	toStr[0] = ch;
	toStr[1] = '\0';
	dbChar::operator+=(toStr);
}


void 
dbChar::operator+=(const char* str)
{
	const unsigned long appendLen = (str==0) ? 0 : strlen(str);
	if (appendLen==0)
		return;
		
	if (usingCalculator())
		return;
		
	char *writeTo = 0;
	{	
		if (fieldIsStandalone()) {
			writeTo = (char*) mBackend;
		}
		else {
			writeTo = (char*) (mBackend->getFieldWriteDest(this) );
		}
	}
	assert(writeTo);
	
	unsigned long currLen = strlen(writeTo);
	assert(currLen <= mMaxLength);
	
	unsigned long totalLen = currLen + appendLen;
	if (totalLen > mMaxLength)
		dbConnect::raise((stringstream() << flush 
			<< "attempt to append string of " << totalLen << " to dbChar of only " << mMaxLength << " chars:\n"
			<< str)
			, false
		);
	else
	{
		if (fieldIsStandalone()) {
			char* oldStore = writeTo;
			writeTo = new char[totalLen+1];
			memcpy(writeTo, oldStore, currLen);
			delete[] oldStore;			
			mBackend = (OOF_tableBackend*) writeTo;
		}
		memcpy(writeTo+currLen, str, appendLen);
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
		writeTo[totalLen]='\0';
	}
}


void 
dbChar::setString(const char* str)
{
	dbChar::operator=(str);
}


void 
dbChar::clear()
{
	setChars(0,0);
}


void 
dbChar::setChars(const char* str , unsigned long charLen)
{
	if (charLen > mMaxLength) {
#ifdef OOF_DEBUG
		dbConnect::raise((stringstream() << flush 
			<< "attempt to assign string of " << charLen << " to dbChar of only " << mMaxLength << " chars:\n"
			<< str)
			, false
		);
#endif
		charLen = mMaxLength;  // bloody-minded truncation
	}
	// assignment was previously an else condition!
	{
		char *writeTo = 0;
		if (fieldIsStandalone()) {
			delete[] (char*) mBackend;
			writeTo = new char[charLen+1];
			mBackend = (OOF_tableBackend*) writeTo;
			if (writeTo) {
				if (str && charLen>0)
				memcpy(writeTo, str, charLen);
		#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
			OOF_MEM_DEBUG_FORCE_POOL_CHECK
		#endif			
				writeTo[charLen]='\0';
			}
		}
		else {
			writeTo = (char*)mBackend->getFieldWriteDest(this);
			mTable->markDirty();
			if (writeTo) {
				if (str && charLen>0)
					memcpy(writeTo, str, charLen);
		#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
			OOF_MEM_DEBUG_FORCE_POOL_CHECK
		#endif			
// don't just write a terminating null, but also fill the rest of the field with nulls
// so indexed searches work. - only case is if we just rewrote field with shorter value.
				writeTo[mMaxLength]='\0';
				for(;writeTo[charLen]!='\0';charLen++) {	// safe because of extra null at end of fields
					writeTo[charLen]='\0';
				}
			}
		}
	}
}



char& 
dbChar::operator[](unsigned short charIndex)
{
	char *writeTo = 0;
	if (fieldIsStandalone()) {
		writeTo =  (char*) mBackend;
	}
	else {
		writeTo = (char*)mBackend->getFieldWriteDest(this);
	}
	return writeTo[charIndex];
}


void 
dbChar::extract(ostream& os) const
{
	os << (const char*)*this;
}



bool 
dbChar::insert(istream& is, char fieldSep, char recSep)
{
// this is NOT very efficient, but it *is* safe
	if (!is.good()) 
		return false;
		
	const unsigned long maxLen = fieldStorageLen();
	assert(maxLen);  // brief flash of paranoia in case they get this far with a field of 0 len
	char* buildBuf = new char[maxLen+1];
	assert(buildBuf);
	unsigned int charsRead = 0;
	bool hitEndOfField = false;
	while (is.good() && (charsRead<=maxLen)) {  // allow read one char past, in case it's terminator
		char c = is.peek();
		if ((c==fieldSep) || (c==recSep)) {
			hitEndOfField = true;
			break;  // OK - hit a separator
		}
		is >> c;
		if ( (c>=' ') && (charsRead<maxLen)) { // printable & in field length
			buildBuf[charsRead] = c;
			charsRead++;
		}
	} // loop reading char at a time

	bool gotDataOK = false;
	if (hitEndOfField)  // everybody happy
		gotDataOK = true;
	else {  // some form of problems along the way
		if (charsRead>maxLen) {// too wide for DB field, store as short anyway
			gotDataOK = true;
			dbField::insert(is, fieldSep, recSep);  // consume the remainder
#ifdef OOF_DEBUG
			dbConnect::raise(stringstream() << flush << "dbChar::insert() of field " 
																		<< tableName() << "." << fieldName()
			                              << " has filled its length of " << maxLen 
																		<< " before reaching the end of the field"
											);   
#endif
		}
	}
	
	if (gotDataOK) { 
		if (dbConnect::willTruncateTrailingSpaces()) {
			while ((charsRead > 0) && (buildBuf[charsRead-1] ==' '))
				--charsRead;
		}
		buildBuf[charsRead] = '\0';  // now make it into a C string!
		*this = buildBuf;  // finally, assign to the record
	}
	delete[] buildBuf;
	return gotDataOK;  // if general stream problem, let caller report it
}


void 
dbChar::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
// nearly identical to dbText and dbFixedBinary
	if (generateRandom) {
		OOF_String theStr;
		const unsigned long randLen = rand()%mMaxLength;
		theStr.makeRandomChars(randLen);
		dbChar::setString(theStr);
	}
	else {
		OOF_CheckString theStr(seedOrCount, '-');
		dbChar::setString(theStr);
	}
}


dbQueryBinary	
dbChar::operator==(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralStr(str));
}  


dbQueryClause*	
dbChar::valueAsQueryLiteral() const
{
	dbQueryClause* ret = new dbQueryLiteralStr(value());
	return ret;
}  


dbQueryBinary	
dbChar::startsWith(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::startsWith, new dbQueryLiteralStr(str));
}  


/*

// DOESN'T WORK JUST CREATING STRING IN HERE WITH WILDCARDS
// WILL NEED MORE COMPLEX OPTION

dbQueryBinary	
dbChar::contains(const char* str) const
{
	bool ret;
	char* strPlus;
	unsigned short len = strlen(str);
	if (len==0) {
		strPlus = new char[2];
		strPlus[0] = '*';
		strPlus[1] = '\0';
	}
	else {
		strPlus = new char[len+3];
		assert(strPlus);
		strPlus[0] = '*';
		memcpy(&strPlus[1], str, len);
		strPlus[len+1] = '*';
		strPlus[len+2] = '\0';
	}
	ret = dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralStr(strPlus));
	delete[] strPlus;
	return ret;

} 
*/ 


dbQueryTrinary	
dbChar::between(const char* fromStr, const char* toStr) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::between,
		new dbQueryLiteralStr(fromStr), new dbQueryLiteralStr(toStr));
}


dbQueryTrinary	
dbChar::outside(const char* fromStr, const char* toStr) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::outside,
		new dbQueryLiteralStr(fromStr), new dbQueryLiteralStr(toStr));
}


dbQueryBinary	
dbChar::operator!=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, new dbQueryLiteralStr(str));
}  


 dbQueryBinary	
 dbChar::operator<(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, new dbQueryLiteralStr(str));
}  


 dbQueryBinary	
 dbChar::operator<=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, new dbQueryLiteralStr(str));
}  


 dbQueryBinary	
 dbChar::operator>(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, new dbQueryLiteralStr(str));
}  


 dbQueryBinary	
 dbChar::operator>=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, new dbQueryLiteralStr(str));
}  


void 
dbChar::describe(ostream& os) const
{
	dbField::describe(os);
	os << "    and a maximum length of : " << mMaxLength << endl;
}


bool 
dbChar::fieldIsSameTypeAs(const dbField* rhs) const
{
	if (dbField::fieldIsSameTypeAs(rhs))
		return (mMaxLength == ((const dbChar*)rhs)->mMaxLength);	// safe downcast
	else
		return false;
}


void 
dbChar::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


// -------------------------------------------------------
//         d b C a l c u l a t o r
// -------------------------------------------------------
dbCalculator* 
dbCalculator::cloneOwned(dbField*) const  // has default implementation as not necessary to override
{
	return clone();	// calls the pure virtual
}


// -------------------------------------------------------
//  d b C h a r S t r i p p e r C a l c u l a t o r
// -------------------------------------------------------
dbCharStripperCalculator::dbCharStripperCalculator(dbChar& stripsField, char toStrip) : 
	mStripsField(&stripsField),
	mStripChar(toStrip) 
{}

		
const char*
dbCharStripperCalculator::calc(const dbChar*)
{
	mStrippedStr = mStripsField->asChars();
	mStrippedStr.stripChar(mStripChar);
	return mStrippedStr;
}


dbCalculator* 
dbCharStripperCalculator::clone() const
{
	return new dbCharStripperCalculator(*this);
}


dbCalculator* 
dbCharStripperCalculator::cloneOwned(dbField* newOwner) const
{
	dbTable* newOwnerTable = newOwner->fieldTable();
	dbField* newStripsField = newOwnerTable->field(mStripsField->fieldNumber());
	return new dbCharStripperCalculator(*(dbChar*)newStripsField, mStripChar);
}



// -------------------------------------------------------
//                      d b T e x t
// -------------------------------------------------------
dbText::dbText(const OOF_IndexOptions opt) :
	dbBLOB(opt),
	OOF_mixKeywordableField(this)
{
//				assert( opt == kNotIndexed);  // more later
	mIsTextField = true;		// override dbBLOB init
}


dbText::dbText(dbField::FieldIsStandaloneT k) :
	dbBLOB(k),
	OOF_mixKeywordableField(this)
{
	mIsTextField = true;		// override dbBLOB init
}


dbText::dbText(const char* fieldName)  :
	dbBLOB(fieldName),
	OOF_mixKeywordableField(this)
{
	mIsTextField = true;		// override dbBLOB init
}

 
dbText::dbText(const char* fieldName, const OOF_IndexOptions opt)  :
	dbBLOB(fieldName, opt),
	OOF_mixKeywordableField(this)
{
//assert(opt == kNotIndexed);
	mIsTextField = true;		// override dbBLOB init
}


dbText::dbText(const dbText& rhs) :
	dbBLOB(rhs),
	OOF_mixKeywordableField(rhs, this)
{
	mIsTextField = true;		// override dbBLOB init
}


dbField*
dbText::clone() const 
{
	return new dbText(*this);
}


dbText&
dbText::calculateWith(dbTextCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


dbText::operator const char*() const
{
	const char* ret;
	if (usingCalculator()) 
		ret = calculator()->calc(this);
	else {	
		if (length())
			ret = (const char *)bodyAddress();
		else
			ret = "";
	}
	return ret;	
}


const char*
dbText::asChars() const
{
	return (const char*) *this;
}


oofWordParser* 
dbText::words() const
{
	return wordParser();
}


unsigned long 
dbText::length() const
{
	if (fieldIsStandalone()) {
		unsigned long ret =  *((long*) mBackend);
		if (ret)
			ret -= 1;	// deduct trailing null
		return ret;
	}
	else
		return fieldDataLen();
}


void 
dbText::getChars(char* ioBuff, unsigned long& outLen) const
{
	const char* from = (const char*) *this;
	outLen = strlen(from);
	memcpy(ioBuff, from, outLen);
}


oofString 
dbText::copyString() const
{
	return oofString((const char*) *this);
}


void 
dbText::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==textField)
		*this = *(dbText*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbText::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbText::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==textField) {
		const char* rhs = *(const dbText*)srcField;  // safe downcast
		if (strcmp(*this, rhs))
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbText::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbText::fieldType() const
{
	return textField; 
}


OOF_fieldTypes 
dbText::nativeType() const
{
	return charField; 
}


bool 
dbText::fieldIsKeywordIndexed() const
{
	return (mParser!=0);
} 


bool 
dbText::fieldIsBinary() const
{
	return false;  
}


unsigned long 
dbText::fieldUsualWrapWidth() const
{
	unsigned long ret = 60;	// NOT YET IMPLEMENTED - settable default
	return ret;
}




 
dbText& 
dbText::operator=(const dbText& rhs)
{
	if (this == &rhs)  // MUST use this form to avoid invoking the operator== used for searches
		return *this;
	
	*this = (const char*)rhs;
	return *this;
}


dbText& 
dbText::operator=(char ch)
{
	char	toStr[2];
	toStr[0] = ch;
	toStr[1] = '\0';
	dbText::operator=(toStr);
	return *this;
}


dbText& 
dbText::operator=(const char* str)
{
	unsigned long charLen;
	if (str)
		charLen = strlen(str);
	else
		charLen = 0;
	setChars(str, charLen);
	return *this;
}


void 
dbText::setNumber(long n)
{
	OOF_CheckString numberAsWords(n);
	dbText::setString(numberAsWords);
}


void 
dbText::setString(const char* str)
{
	dbText::operator=(str);
}


void 
dbText::setChars(const char* str , unsigned long charLen)
{
	if (!fieldIsStandalone()) {
		mTable->ensureRecordLoaded();
		mTable->markDirty();
	}
	mStorage->setChars(str, charLen);
}


void 
dbText::clear()
{
	setChars(0, 0);
} 


void 
dbText::operator+=(char ch)
{
	char	toStr[2];
	toStr[0] = ch;
	toStr[1] = '\0';
	dbText::operator+=(toStr);
}


void 
dbText::operator+=(const char* str)
{
	unsigned long charLen = strlen(str);
	if (charLen==0)
		return;
	
	dbBLOB::append(str, charLen+1);	// copy the null as well!
}  // operator+=


void
dbText::adoptString(oofString& adoptedString) const
{
	const unsigned long inLen = adoptedString.length();  // BEFORE the orphan
	adoptBody(adoptedString.orphan(), inLen, inLen);
}


void 
dbText::append(const void* appendBody, unsigned long bodyLen, unsigned long usedLen)
{
	if (usedLen==0 || bodyLen==0)
		return;
		
	if (usedLen>bodyLen)
		usedLen=bodyLen;	// probably optional param
	else
		if (bodyLen==ULONG_MAX)
			usedLen = strlen((const char*)appendBody);
		
	dbBLOB::append(appendBody, usedLen+1);	// copy the null as well!
}  // appendChars


void 
dbText::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom) {
		OOF_String theStr;
		const unsigned long randLen = rand()%10240;	// max of 10kb test data
		theStr.makeRandomChars(randLen);
		dbText::setString(theStr);
	}
	else {
		OOF_CheckString theStr(seedOrCount, '-');
		dbText::setString(theStr);
	}
}


void 
dbText::extract(ostream& os) const
{
	os << (const char*)*this;
}


bool 
dbText::insert(istream& is, char fieldSep, char recSep)
{
// nearly straight copy of dbChar - we really need to do something to combine these fields!

// this is NOT very efficient, but it *is* safe
	if (!is.good()) 
		return false;

// this is not yet complete - it needs a cleaner way to handle large imports		
	const unsigned long maxLen = 16000;  // KEEP UNDER 64k to avoid Win3.1 bugs
	char* buildBuf = new char[maxLen+1];
	assert(buildBuf);
	unsigned int charsRead = 0;
	bool hitEndOfField = false;
	while (is.good() && (charsRead<=maxLen)) {  // allow read one char past, in case it's terminator
		char c = is.peek();
		if ((c==fieldSep) || (c==recSep)) {
			hitEndOfField = true;
			break;  // OK - hit a separator
		}
		is >> c;
		if ( (c>=' ') && (charsRead<maxLen)) { // printable & in field length
			buildBuf[charsRead] = c;
			charsRead++;
		}
	} // loop reading char at a time

	bool gotDataOK = false;
	if (hitEndOfField)  // everybody happy
		gotDataOK = true;
	else {  // some form of problems along the way
		if (charsRead>maxLen) {// too wide for DB field, store as short anyway
			gotDataOK = true;
			dbField::insert(is, fieldSep, recSep);  // consume the remainder
#ifdef OOF_DEBUG
			dbConnect::raise(stringstream() << flush << "dbText::insert() of field " 
																		<< tableName() << "." << fieldName()
			                              << " has filled its length of " << maxLen 
																		<< " before reaching the end of the field"
											);   
#endif
		}
	}
	
	if (gotDataOK) { 
		if (dbConnect::willTruncateTrailingSpaces()) {
			while ((charsRead > 0) && (buildBuf[charsRead-1] ==' '))
				--charsRead;
		}
		buildBuf[charsRead] = '\0';  // now make it into a C string!
		*this = buildBuf;  // finally, assign to the record
	}
	else
		*this = "";
	delete[] buildBuf;
	return gotDataOK;  // if general stream problem, let caller report it
}





dbQueryClause*	
dbText::valueAsQueryLiteral() const
{
	dbQueryClause* ret = new dbQueryLiteralStr(value());
	return ret;
}


dbQueryBinary	
dbText::operator==(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralStr(str));
}  

	
dbQueryBinary	
dbText::operator!=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbText::operator<(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbText::operator<=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbText::operator>(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbText::operator>=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, new dbQueryLiteralStr(str));
}  


void 
dbText::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


// -------------------------------------------------------
//                      d b B L O B s t o r a g e
// -------------------------------------------------------
dbBLOBstorage::dbBLOBstorage(dbBLOB* owner)  :
				mBody(0),
				mBodyLength(0),
				mDirty(false),
				mLoaded(false),
				mOwner(owner)
	{};  
	
dbBLOBstorage::~dbBLOBstorage()
{
	delete[] mBody;
};


char*
dbBLOBstorage::bodyAddress() const
{
	return mBody;
}


bool 
dbBLOBstorage::isDirty() const
{
	return mDirty;
}

void
dbBLOBstorage::markDirty()
{
	mDirty = true;
	mLoaded = true;
}

#ifdef OOF_DebugDumpBlobState
void
	dbBLOBstorage::blobState(ostream& os) const
	{
		
		os << "dbBLOBstorage: "<< this << endl
		<< " mBody: " << (char*) mBody << endl
		 << "mDirty: ";
		
		if(mDirty)
			os << "True ";
		else
			os << "false ";
		os << endl
		<< "mLoaded: ";
			if(mLoaded)
			
			os << "True ";
		else
			os << "false ";
		os << endl << endl;
	
}
#endif 

void 
dbBLOBstorage::reset()
{
	mDirty = false;
	mLoaded = false;
}


char* 
dbBLOBstorage::orphanBody()
{
	
	char* ret = mBody;
	reset();
	mBody = 0;
	mBodyLength = 0;
	return ret;
}


void 
dbBLOBstorage::adoptBody(char* body, unsigned long bodyLen)
{
	delete[] mBody;
	mBody = body;
	mBodyLength = bodyLen;
	mLoaded = mDirty = true;
}


char* 
dbBLOBstorage::allocRoomFor(unsigned long len)
{
	if (len > mBodyLength) {
		if (mLoaded) {	// assume we must preserve the buffer
			char *newBody = new char[len];
			assert(newBody);
			if (mBody) {
				memcpy(newBody, mBody, mBodyLength);
				delete[] mBody;
			}
			mBody = newBody;
		}
		else {			
			delete[] mBody;
			mBody = new char[len];
			assert(mBody);
		}
		mBodyLength = len;
	}
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	return mBody;
}


void 
dbBLOBstorage::setBytes(const char* buf , unsigned long bufLen)
{
	if (buf && bufLen) {
		allocRoomFor(bufLen);
		memcpy(mBody, buf, bufLen);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
	else
		bufLen = 0;  // cope with obscure zero buf & non-zero length
	mOwner->SetLength(bufLen);
	mOwner->markDirty();
}


void 
dbBLOBstorage::setChars(const char* str, unsigned long charLen)
{
	if (str && charLen) {
		allocRoomFor(charLen+1);
		memcpy(mBody, str, charLen);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
		((char*)mBody)[charLen]='\0';
		mOwner->SetLength(charLen+1);
	}
	else {
		mOwner->SetLength(0);	// allowed to have empty body without null terminator
	}
	mOwner->markDirty();
}


void 
dbBLOBstorage::convertLineEndings()
{
	if (mBody && mBodyLength) {
		// knock 1 off our mBodyLength as that includes the null, assuming storing text
		unsigned long strLen = mBodyLength-1;
#if defined(_Macintosh)
		char* newBody = OOF_String::lineEndingsMac((char*)mBody, strLen);
#elif defined(_MSDOS)  // implies _Windows
		char* newBody = OOF_String::lineEndingsDOS((char*)mBody, strLen);
#elif defined(_Unix)
		char* newBody = OOF_String::lineEndingsUnix((char*)mBody, strLen);
#else
	ERROR "must have _Macintosh _Windows or _Unix defined"
#endif
		if (newBody && newBody!=mBody) {
			delete[] mBody;
			mBody = newBody;
		}
		mBodyLength = strLen+1;  // may get non-zero result without realloc, if shortened
	}
}


// -------------------------------------------------------
//                      d b B L O B
// -------------------------------------------------------
dbBLOB::dbBLOB(const OOF_IndexOptions indexOptions) :
				dbField(indexOptions),
				mIsTextField(false)
{
	mStorage = new dbBLOBstorage(this);
}


dbBLOB::dbBLOB(const char* fieldName)  :
				dbField(fieldName),
				mIsTextField(false)
{
	mStorage = new dbBLOBstorage(this);
}


dbBLOB::dbBLOB(const char* fieldName, const OOF_IndexOptions indexOptions)  :
				dbField(fieldName, indexOptions),
				mIsTextField(false)
{
	mStorage = new dbBLOBstorage(this);
}


dbBLOB::dbBLOB(const dbBLOB& rhs) : 
	dbField(rhs),
	mIsTextField(rhs.mIsTextField)
{
	mStorage = new dbBLOBstorage(this);
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) (new long(0));
}


dbBLOB::dbBLOB(dbField::FieldIsStandaloneT k) : 
	dbField(k),
	mIsTextField(false)
{
	mStorage = new dbBLOBstorage(this);
	mBackend = (OOF_tableBackend*) (new long(0));
}


dbBLOB::~dbBLOB()
{
	delete mStorage;
	if (fieldIsStandalone())
		delete (long*) mBackend;
};


dbField*
dbBLOB::clone() const 
{
	return new dbBLOB(*this);
}


OOF_fieldTypes 
dbBLOB::fieldType() const
{
	return blobField; 
}


unsigned long 
dbBLOB::fieldStorageLen() const
{
	if (fieldIsStandalone())
		if (mStorage)
			return mStorage->mBodyLength;
		else
			return 0;
	else
		return mBackend->blobPointerReferenceSize();  
}


unsigned long 
dbBLOB::length() const
{
	if (fieldIsStandalone())
		return *((long*) mBackend);
	else
		return fieldDataLen();
}

 
bool 
dbBLOB::isEmpty() const
{ 
	return (length()==0);
}


bool 
dbBLOB::fieldIsBlob() const
{
	return true;  
}


void 
dbBLOB::SetLength(unsigned long len) const
{
	if (fieldIsStandalone()) {
		long* lenP = (long*)mBackend;
		*lenP = len;	// store long in mBackend
	}
	else{
		mBackend->setBlobLength(mFieldNumber, len);
	}
}


void 
dbBLOB::adoptBody(void* body, unsigned long bodyLen, unsigned long usedLen) const
{
	mStorage->adoptBody((char*)body, bodyLen);
	SetLength(usedLen);
}


void
dbBLOB::adoptBody(void* body, unsigned long bodyLen) const
{
	adoptBody(body, bodyLen, bodyLen);
}


char* 
dbBLOB::bodyAddress() const
{
	if (!fieldIsStandalone()) {
		mTable->ensureRecordLoaded();
		if (!loaded())
			LoadField();
	}
	return mStorage->bodyAddress();
}


void 
dbBLOB::append(const dbBLOB& rhs)
{
	append(rhs.bodyAddress(), rhs.length());
}


void 
dbBLOB::append(const void* appendBody, unsigned long bodyLen, unsigned long usedLen)
{
	if (usedLen==0)
		return;
		
	if (usedLen>bodyLen)
		usedLen=bodyLen;	// probably optional param
		
	if (!fieldIsStandalone()) {
		mTable->ensureRecordLoaded();
		if (!loaded())
			LoadField();
		mTable->markDirty();
	}
	const unsigned long currentFldLen = length();	// virtual call, will skip null if dbText field
	const unsigned long newLen = currentFldLen+bodyLen;
	allocRoomFor(newLen);	// content-preserving
	const unsigned long newUsedLen = currentFldLen+usedLen;
	SetLength(newUsedLen);  // may be shorter than that set in allocRoomFor, if have slop already
	markDirty();		
	char* const body = (char*)bodyAddress();
	char* const appendTo = body+currentFldLen;
	memcpy(appendTo, appendBody, bodyLen);	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif		
}


void 
dbBLOB::operator+=(const dbBLOB& rhs)
{
	assert(this != &rhs);	// can't append ourself
	append(rhs);
}


dbBLOB& 
dbBLOB::operator=(const dbBLOB& rhs)
{
	if (this == &rhs)  // MUST use this form to avoid invoking the operator== used for searches
		return *this;
	
	setBytes(rhs.bodyAddress(), rhs.length());
	return *this;
}


void 
dbBLOB::LoadField() const
{
	assert(!fieldIsStandalone());
	mBackend->loadBlob(this);
	mStorage->mLoaded = true;
	if (mIsTextField && dbText::convertsLineEndingsOnLoad())
		mStorage->convertLineEndings();
}


/**
	Copy the incoming buffer into our local buffer in mStorage.
	Use adoptBody to avoid copying.
*/
void 
dbBLOB::setBytes(const void* buf , unsigned long bufLen)
{
	if (!fieldIsStandalone()) {
		mTable->ensureRecordLoaded();
		mTable->markDirty();
	}
	mStorage->setBytes((const char*)buf, bufLen);
}


void 
dbBLOB::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom) {
		OOF_String theStr;
		const unsigned long randLen = rand()%10240;	// max of 10kb test data
		theStr.makeRandomChars(randLen);
		setBytes(theStr, theStr.length());
	}
	else {
		OOF_CheckString theStr(seedOrCount, '-');
		setBytes(theStr, theStr.length());
	}
}


void 
dbBLOB::extract(ostream& os) const
{
// extract as hex dump
	unsigned long lenToDump = length();
#if defined __MWERKS__  & __MWERKS__ >= 0x2301	
	std::ios_base::fmtflags savedFlags = os.flags(ios::right | ios::hex);
#else
	long savedFlags = os.flags(ios::right | ios::hex);
#endif

	int savedFill = os.fill('0');
//	int savedWidth = os.width(2);	not needed as widths not sticky
	char* body = (char*) bodyAddress();
	unsigned short numInRow=0;
	for (unsigned long i=0; i<lenToDump; i++) {
		if (numInRow==16) {
			os << endl;
			numInRow = 0;
		}
		else
			numInRow++;
		os.width(2);	// for some stupid reason CW lacks stickywidth (is this dropped from Standard?)
		unsigned short asNum = (unsigned char)body[i];
		os << asNum << ' ';	
	}
	os.flags(savedFlags);
	os.fill(savedFill);
//	os.width(savedWidth);	not needed as widths not sticky
	os << endl;
}

#ifdef OOF_DebugDumpBlobState
void
dbBLOB::blobState(ostream &os) 
	{
		mStorage->blobState(os);
	}
#endif

// -------------------------------------------------------
//               d b C o m p o u n d F i e l d 
// -------------------------------------------------------
						
dbCompoundField::dbCompoundField(const OOF_IndexOptions indexOptions) :
	dbField(indexOptions),
	mTotalLength(0)
{}
	
								
dbCompoundField::dbCompoundField(const char* fieldName) :
	dbField(fieldName),
	mTotalLength(0)
{}
	
								
dbCompoundField::dbCompoundField(const char* fieldName, const OOF_IndexOptions indexOptions) :
	dbField(fieldName, indexOptions),
	mTotalLength(0)
{}
	
								
dbCompoundField::dbCompoundField(const dbCompoundField& rhs) : 
	dbField(rhs), 
	OOF_FieldSegOwner(rhs),  
	mTotalLength(rhs.mTotalLength)
{}


dbField*
dbCompoundField::clone() const 
{
	return new dbCompoundField(*this);
}


OOF_fieldTypes 
dbCompoundField::fieldType() const
{
	return compoundField;
}


unsigned long 
dbCompoundField::fieldStorageLen() const
{
	return mTotalLength;   
}


bool 
dbCompoundField::fieldIsVirtual() const
{
	return true;
}


void 
dbCompoundField::addSegment(dbField& segField) 
{
	assert(segField.fieldType() != textField);
	OOF_FieldSegOwner::addSegment(segField);
	mTotalLength += segField.fieldStorageLen();
}


const dbField* 
dbCompoundField::field(unsigned int n) const
{
	const dbField* theField = mTable->field(OOF_FieldSegOwner::fieldNumberOfSegment(n));
	return theField;
}


dbQueryBinary	
dbCompoundField::operator==(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::operator!=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::operator<(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::operator<=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::operator>(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::operator>=(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, new dbQueryLiteralStr(str));
}  


dbQueryBinary	
dbCompoundField::startsWith(const char* str) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::startsWith, new dbQueryLiteralStr(str));
}  


void 
dbCompoundField::extract(ostream& os) const
{
	const unsigned int numSegments = countSegments();
	for (unsigned int i=0; i<numSegments; i++) {
		const dbField* fld = field(i);
		fld->extract(os);
	}
}

 
bool 
dbCompoundField::fieldIsSameTypeAs(const dbField* rhs) const
{
	if (dbField::fieldIsSameTypeAs(rhs))
		return (mTotalLength == ((const dbCompoundField*)rhs)->mTotalLength);	// safe downcast
	else
		return false;
}


// -------------------------------------------------------
//                  O O F I L E _ f i e l d T y p e s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, OOF_fieldTypes ft)
{
	switch (ft) {
	case (charField) :
		os << "fixed-length chars";
		break;
		
	case (textField) :
		os << "variable-length text";
		break;
		
	case (dateField) :
		os << "date";
		break;
		
	case (dateTimeField) :
		os << "date time";
		break;
		
	case (timeField) :
		os << "time";
		break;
		
	case (shortField) :
		os << "short integer";
		break;
		
	case (uShortField) :
		os << "unsigned short integer";
		break;
		
	case (longField) :
		os << "long integer";
		break;
		
	case (uLongField) :
		os << "unsigned long integer";
		break;
		
	case (realField) :
		os << "real";
		break;
		
	case (blobField) :
		os << "binary blob";
		break;
		
	case (compoundField) :
		os << "compound";
		break;
		
	case (fixedBinaryField) :
		os << "fixedBinary";
		break;
		
	case (boolField) :
		os << "bool";
		break;
		
	case (relationshipField) :
		os << "relationship traversal pointer";
		break;
		
	}
	return os;
}



// -------------------------------------------------------
//                d b F i e l d C o p i e r
// -------------------------------------------------------
dbFieldCopier::dbFieldCopier(oofBroadcaster* listensTo, unsigned short copyOnMessage, dbField* copyFrom, dbField* copyTo) :
		oofSingleListener(listensTo),
		mCopyFrom(copyFrom),
		mCopyTo(copyTo),
		mCopyOnMessage(copyOnMessage)
{}


bool
dbFieldCopier::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	if (msg==mCopyOnMessage)
		mCopyTo->copyValueFrom(mCopyFrom);
	else
		oofSingleListener::receiveMsg(msg, senderDefined);
	return true;
}


// -------------------------------------------------------
//               dbCloneCleanup
// -------------------------------------------------------
dbCloneCleanup::dbCloneCleanup(dbTable* listensTo) :
	oofSingleListener(listensTo),
	mTable(listensTo)
{}


bool
dbCloneCleanup::receiveMsg(OOFmsgT msg, unsigned long /* senderDefined */)
{
	if (msg!=OOFmsg_BroadcasterClosing)  
		mTable->postCloneTableCleanup();
	delete this;
	return true;
}


// -------------------------------------------------------
//               O O F _ S e g m e n t  
// -------------------------------------------------------

OOF_Segment::OOF_Segment(const dbField& fld) : 
		mFieldNo(fld.fieldNumber()), 
		mReversed(false) 
{}


OOF_Segment::OOF_Segment(const dbField* fld) : 
		mFieldNo(fld->fieldNumber()), 
		mReversed(false) 
{}


