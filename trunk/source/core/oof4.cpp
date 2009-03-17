// COPYRIGHT 1994-1997 A.D. Software, All rights reserved

// OOFILE database field-related classes
// numeric fields - see also oof3.cpp & oof5.cpp

#include "oofpch_c.h"  // for precompilation of core files

#include <ctype.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif


// -------------------------------------------------------
//               d b N u m e r i c F i e l d  
// -------------------------------------------------------
dbNumericField::dbNumericField(const OOF_IndexOptions indexOptions) :
	dbField(indexOptions)
{}


dbNumericField::dbNumericField(const char* fieldName)  :
	dbField(fieldName)
{}


dbNumericField::dbNumericField(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbField(fieldName, indexOptions)
{}



void
dbNumericField::clear()
{
	setNumber(0);
}


dbQueryTrinary	
dbNumericField::between(long fromNum, long toNum) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::between,
		makeQueryLiteral(fromNum), makeQueryLiteral(toNum));
}


dbQueryTrinary	
dbNumericField::outside(long fromNum, long toNum) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::outside,
		makeQueryLiteral(fromNum), makeQueryLiteral(toNum));
}


dbQueryBinary	
dbNumericField::operator==(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


#if defined(_MSC_VER)  || defined(__MWERKS__)
dbQueryBinary	
dbNumericField::operator==(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator==(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator==(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator==(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator==(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::operator!=(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator<=(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::operator>=(unsigned int n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


#endif  // #if defined(_MSC_VER)  || defined(__MWERKS__)


dbQueryBinary	
dbNumericField::equal(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}    


dbQueryBinary	
dbNumericField::notEqual(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::lessThan(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::lessThanOrEqual(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::greaterThan(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, makeQueryLiteral(n));
}  


dbQueryBinary	
dbNumericField::greaterThanOrEqual(long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, makeQueryLiteral(n));
}  


void 
dbNumericField::operator/=(long)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator-=(long)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator+=(long)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator*=(long)

{
// NOT YET IMPLEMENTED
;
}


void 
dbNumericField::operator/=(double)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator-=(double)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator+=(double)

{
// NOT YET IMPLEMENTED
}



void 
dbNumericField::operator*=(double)

{
// NOT YET IMPLEMENTED
;
}



void 
dbNumericField::CheckRange(double d, long minL, long maxL, const char* file, int line)
{
	if (d<minL) {
		oofE_RangeError excp(stringstream() << flush << file << "line: " << line
			<< "Range conversion error: double " << d 
			<< " below min of " << minL << endl);
		RAISE_EXCEPTION(excp);
	}
	else
		if (d>maxL) {
			oofE_RangeError excp(stringstream() << flush << file << "line: " << line
				<< "Range conversion error: double " << d 
				<< " above max of " << maxL << endl);
			RAISE_EXCEPTION(excp);
		}
}
 
 

void 
dbNumericField::CheckRange(long l, long minL, long maxL, const char* file, int line)
{
	if (l<minL) {
		oofE_RangeError excp(stringstream() << flush << file << "line: " << line
			<< "Range conversion error: long " << l 
			<< " below min of " << minL << endl);
		RAISE_EXCEPTION(excp);
	}
	else
		if (l>maxL) {
			oofE_RangeError excp(stringstream() << flush << file << "line: " << line
				<< "Range conversion error: long " << l
				<< " above max of " << maxL << endl);
			RAISE_EXCEPTION(excp);
		}
}
 
 
void 
dbNumericField::CheckRange(unsigned long l, unsigned long maxL, const char* file, int line)
{
	if (l>maxL) {
		oofE_RangeError excp(stringstream() << flush << file << "line: " << line
			<< "Range conversion error: ulong " << l
			<< " above max of " << maxL << endl);
		RAISE_EXCEPTION(excp);
	}
}
 
 
dbQueryLiteral* 
dbNumericField::makeQueryLiteral(long n) const
{
	return new dbQueryLiteralLong(n);
}


unsigned long 
dbNumericField::fieldMinWrapWidth() const
{
	unsigned long ret = mFormatMask.length();
	if (ret==0)
		ret = fieldStorageLen()*2+2;	// just an arbitrary number that will make most dates & longs etc. fit

	return ret;
}
// -------------------------------------------------------
//                   d b S h o r t
// -------------------------------------------------------
dbShort::dbShort(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{}


dbShort::dbShort(const char* fieldName)  :
	dbNumericField(fieldName)
{}


dbShort::dbShort(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{}


dbShort::dbShort(dbField::FieldIsStandaloneT k) :
				dbNumericField(k)
{}

 
dbShort::~dbShort()
{
}


dbField*
dbShort::clone() const 
{
	return new dbShort(*this);
}


dbShort&
dbShort::calculateWith(dbShortCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbShort::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}

dbShort& 
dbShort::operator=(const dbShort& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (short)rhs;
	return *this;
}


void 
dbShort::setNumber(long n)
{
	*this = n;
}


void 
dbShort::setString(const char* str)
{
	short n;
	istringstream is((char*)str);  // cast away const for Borland
	is >> n;
	*this = n;
}


dbShort::operator short() const
{
	short ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) {
			ret = (long) mBackend;
		}
		else {
			const short *readFrom = (short *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}


dbShort::dbShort(const dbShort& rhs) : 
	dbNumericField(rhs)
{
}

		
dbShort& 
dbShort::operator=(short n)
{
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) n;
	else {
		short *writeTo = (short *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbShort::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==shortField)
		*this = *(dbShort*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbShort::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbShort::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==shortField) {
		const long rhs = *(dbShort*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbShort::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbShort::fieldType() const
{
	return shortField;
}


unsigned long 
dbShort::fieldStorageLen() const
{
	return sizeof(short); 
}


void 
dbShort::extract(ostream& os) const
{
	os << (short) *this;
}


bool 
dbShort::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the short
	short n=0;
	if (skipTillNumber(is, fieldSep, recSep))
		is >> n;
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		*this = n;
	return hitEndOfField;
}


void 
dbShort::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		*this = (short)(rand()-rand()/2);
	else
		*this = (short)seedOrCount;
}


dbQueryLiteral* 
dbShort::makeQueryLiteral(long n) const
{
	return new dbQueryLiteralShort((short)n);
}


dbQueryClause*	
dbShort::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


// -------------------------------------------------------
//                   d b U s h o r t
// -------------------------------------------------------
 
dbUshort::dbUshort(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{}

 
dbUshort::dbUshort(const char* fieldName)  :
	dbNumericField(fieldName)
{}

 
dbUshort::dbUshort(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{}



dbUshort::dbUshort(dbField::FieldIsStandaloneT k) :
				dbNumericField(k)
{}

 
dbUshort::~dbUshort()
{
}


dbField*
dbUshort::clone() const 
{
	return new dbUshort(*this);
}


dbUshort&
dbUshort::calculateWith(dbUshortCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbUshort::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


dbUshort& 
dbUshort::operator=(const dbUshort& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (unsigned short)rhs;
	return *this;
}


void 
dbUshort::setNumber(long n)
{
	*this = n;
}


void 
dbUshort::setString(const char* str)
{
	unsigned short n;
	istringstream is((char*)str);  // cast away const for Borland
	is >> n;
	*this = n;
}


dbUshort::operator unsigned short() const
{
	unsigned short ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) {
			ret = (unsigned long) mBackend;
		}
		else {
			const unsigned short *readFrom = (unsigned short *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}


dbUshort::dbUshort(const dbUshort& rhs) : 
	dbNumericField(rhs)
{
}

		
dbUshort& 
dbUshort::operator=(unsigned short n)
{
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) n;
	else {
		unsigned short *writeTo = (unsigned short *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbUshort::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==uShortField)
		*this = *(dbUshort*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbUshort::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbUshort::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==uShortField) {
		const long rhs = *(dbUshort*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbUshort::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbUshort::fieldType() const
{
	return uShortField;
}


unsigned long 
dbUshort::fieldStorageLen() const
{
	return sizeof(unsigned short); 
}


void 
dbUshort::extract(ostream& os) const
{
	os << (unsigned short)*this;
}


bool 
dbUshort::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the unsigned short
	unsigned short n=0;
	if (skipTillDigit(is, fieldSep, recSep))
		is >> n;
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		*this = n;
	return hitEndOfField;
}



void 
dbUshort::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		*this = (unsigned short)(rand());
	else
		*this = (const unsigned short)seedOrCount;
}


dbQueryLiteral* 
dbUshort::makeQueryLiteral(long n) const
{
	return new dbQueryLiteralUshort((unsigned short)n);
}


dbQueryBinary	
dbUshort::operator==(unsigned short n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}


dbQueryClause*	
dbUshort::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


// -------------------------------------------------------
//                   d b B o o l
// -------------------------------------------------------
 
dbBool::dbBool(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{}

 
dbBool::dbBool(const char* fieldName)  :
	dbNumericField(fieldName)
{}

 
dbBool::dbBool(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{}



dbBool::dbBool(dbField::FieldIsStandaloneT k) :
				dbNumericField(k)
{}

 
dbBool::~dbBool()
{
}


dbField*
dbBool::clone() const 
{
	return new dbBool(*this);
}


dbBool&
dbBool::calculateWith(dbBoolCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbBool::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


dbBool& 
dbBool::operator=(const dbBool& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (bool)rhs;
	return *this;
}


void 
dbBool::setNumber(long n)
{
	bool b = (n!=0);
	*this = b;
}


void 
dbBool::setString(const char* str)
{
	bool b = str2bool(str);
	*this = b;
}


dbBool::operator bool() const
{
	bool ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) {
			ret = ((unsigned long) mBackend!=0);
		}
		else {
			const unsigned short *readFrom = (unsigned short *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = (*readFrom!=0);
			else
				ret = false;
		}
	}
	return ret;
}


dbBool::dbBool(const dbBool& rhs) : 
	dbNumericField(rhs)
{
}

		
dbBool& 
dbBool::operator=(bool n)
{
	if (fieldIsStandalone()) {
		long forConversion = n;
		mBackend = (OOF_tableBackend*) forConversion;
	}
	else {
		unsigned short* writeTo = (unsigned short*)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbBool::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==boolField)
		*this = *(dbBool*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbBool::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbBool::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==boolField) {
		bool rhs = *(dbBool*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbBool::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbBool::fieldType() const
{
	return boolField;
}


OOF_fieldTypes 
dbBool::nativeType() const
{
	return uShortField; 
}


unsigned long 
dbBool::fieldStorageLen() const
{
	return sizeof(unsigned short); 
}


void 
dbBool::extract(ostream& os) const
{
	os << bool2char((bool)*this);
}


bool 
dbBool::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the char
	char c;
	c = is.peek();
	if ((c==fieldSep) || (c==recSep)) 
		return true;	// early exit, blank field defaults to false
	
	is >> c;
		*this = c;

	bool hitEndOfField = skipRestOfField(is, fieldSep, recSep);
	return hitEndOfField;
}



void 
dbBool::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	bool b;
	if (generateRandom)
		b = ((rand()%1)!=0);
	else
		if ((seedOrCount/2)*2==seedOrCount)	// even
			b = true;
		else
			b = false;
	*this = b;
}


dbQueryLiteral* 
dbBool::makeQueryLiteral(bool b) const
{
	unsigned short n = 0;
	if (b!=0)
		n = 1;
	return new dbQueryLiteralUshort(n);
}


dbQueryLiteral* 
dbBool::makeQueryLiteral(long n) const
{
	if (n!=0)
		n = 1;
	return new dbQueryLiteralUshort(n);
}




dbBool::operator dbQueryBinary() const
{
	const bool b = true;
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(b));
}


dbQueryClause*	
dbBool::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbBool::operator==(bool b) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(b));
}

#ifdef OOF_HAS_BOOL
#else
// causes ambiguity in CodeWarrior 10
dbQueryBinary	
dbBool::operator==(char c) const
{
	const bool b = char2bool(c);
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(b));
}


dbBool& 
dbBool::operator=(char c)
{
	const bool b = char2bool(c);
	*this = b;
	return *this;
}
#endif  // OOF_HAS_BOOL

dbQueryBinary	
dbBool::operator==(const char* str) const
{
	const bool b = str2bool(str);
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(b));
}


char
dbBool::bool2char(bool b)
{
	if (b)
		return 'T';
	else
		return 'F';
}


bool
dbBool::char2bool(char c)
{
	const char uc = toupper(c);
	if ((uc=='T') || (uc=='Y') || (uc=='1'))	// check 1 in case get char const instead of number
		return true;
	else
		return false;
}


bool
dbBool::str2bool(const char* str)
{
// crude implementation for now!
	if (strlen(str))
		return char2bool(str[0]);
	else
		return false;
}


// -------------------------------------------------------
//                      d b L o n g
// -------------------------------------------------------

dbLong::dbLong(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{}


dbLong::dbLong(const char* fieldName)  :
	dbNumericField(fieldName)
{}


dbLong::dbLong(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{}


dbLong::dbLong(dbField::FieldIsStandaloneT k) :
				dbNumericField(k)
{}

 
dbLong::~dbLong()
{
}


dbField*
dbLong::clone() const 
{
	return new dbLong(*this);
}


dbLong&
dbLong::calculateWith(dbLongCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbLong::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


dbLong& 
dbLong::operator=(const dbLong& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (long)rhs;
	return *this;
}


dbLong::dbLong(const dbLong& rhs) : 
	dbNumericField(rhs)
{
}

		
dbLong& 
dbLong::operator=(long n)
{
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) n;
	else {
		long *writeTo = (long *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbLong::setNumber(long n)
{
	*this = n;
}



void 
dbLong::setString(const char* str)
{
	long n;
	istringstream is((char*)str);  // cast away const for Borland
	is >> n;
	*this = n;
}


dbLong::operator long() const
{
	long ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) 
			ret = (long) mBackend;
		else {
			const long *readFrom = (long *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}


void 
dbLong::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==longField)
		*this = *(dbLong*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbLong::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbLong::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==longField) {
		long rhs = *(dbLong*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbLong::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbLong::fieldType() const
{
	return longField; 
}


unsigned long 
dbLong::fieldStorageLen() const
{
	return sizeof(long); 
}


void 
dbLong::extract(ostream& os) const
{
	os << (long) *this;
}


bool 
dbLong::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the long
	long n=0;
	if (skipTillNumber(is, fieldSep, recSep))
		is >> n;
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		*this = n;
	return hitEndOfField;
}

dbQueryClause*	
dbLong::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


void 
dbLong::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		*this = (long)(rand()-rand());
	else
		*this = (long)seedOrCount;
}


// -------------------------------------------------------
//                      d b U l o n g
// -------------------------------------------------------

dbUlong::dbUlong(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{}


dbUlong::dbUlong(const char* fieldName)  :
	dbNumericField(fieldName)
{}


dbUlong::dbUlong(const char* fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{}


dbUlong::dbUlong(dbField::FieldIsStandaloneT k) :
				dbNumericField(k)
{}

 
dbUlong::~dbUlong()
{
}


dbField*
dbUlong::clone() const 
{
	return new dbUlong(*this);
}


dbUlong::dbUlong(const dbUlong& rhs) : 
	dbNumericField(rhs)
{
}

		
dbUlong&
dbUlong::calculateWith(dbUlongCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbUlong::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


dbUlong& 
dbUlong::operator=(const dbUlong& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (unsigned long)rhs;
	return *this;
}


dbUlong& 
dbUlong::operator=(unsigned long n)
{
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) n;
	else {
		unsigned long *writeTo = (unsigned long *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbUlong::setNumber(long n)
{
	*this = (unsigned long) n;
}


void 
dbUlong::setString(const char* str)
{
	unsigned long n;
	istringstream is((char*)str);  // cast away const for Borland
	is >> n;
	*this = n;
}


dbUlong::operator unsigned long() const
{
	unsigned long ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) 
			return (unsigned long) mBackend;
		else {
			const unsigned long *readFrom = (const unsigned long *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}


void 
dbUlong::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==uLongField)
		*this = *(dbUlong*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbUlong::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbUlong::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==uLongField) {
		const unsigned long rhs = *(dbUlong*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbUlong::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbUlong::fieldType() const
{
	return uLongField; 
}


unsigned long 
dbUlong::fieldStorageLen() const
{
	return sizeof(unsigned long); 
}


void 
dbUlong::extract(ostream& os) const
{
	os << (unsigned long)*this;
}


bool 
dbUlong::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the unsigned long
	unsigned long n=0;
	if (skipTillDigit(is, fieldSep, recSep))
		is >> n;
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		*this = n;
	return hitEndOfField;
}


void 
dbUlong::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		*this = (unsigned long)(rand());
	else
		*this = seedOrCount;
}


dbQueryLiteral* 
dbUlong::makeQueryLiteral(unsigned long n) const
{
	return new dbQueryLiteralUlong(n);
}


dbQueryLiteral* 
dbUlong::makeQueryLiteral(long n) const
{
	return new dbQueryLiteralUlong((unsigned long) n);
}


dbQueryClause*	
dbUlong::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbUlong::operator==(unsigned long n) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, makeQueryLiteral(n));
}



// -------------------------------------------------------
//                      d b R e a l
// -------------------------------------------------------

dbReal::dbReal(dbField::FieldIsStandaloneT k) : 
	dbNumericField(k)
{
	CompleteInit();
}


dbReal::dbReal(const OOF_IndexOptions indexOptions) :
				dbNumericField(indexOptions)
{
	CompleteInit();
}


dbReal::dbReal(const char* fieldName)  :
				dbNumericField(fieldName)
{
	CompleteInit();
}


dbReal::dbReal(const char* fieldName, const OOF_IndexOptions indexOptions)  :
				dbNumericField(fieldName, indexOptions)  
{
	CompleteInit();
}


dbReal::dbReal(const dbReal& rhs) : 
	dbNumericField(rhs)
{
	CompleteInit();
	if (fieldIsStandalone())
		*((double*) mBackend)  = *((double*) rhs.mBackend) ;
}

 
void
dbReal::CompleteInit()
{
	if (fieldIsStandalone())
		mBackend = (OOF_tableBackend*) (new double);
}


dbReal::~dbReal()
{
	if (fieldIsStandalone())
		delete (double*) mBackend;
}


dbField*
dbReal::clone() const 
{
	return new dbReal(*this);
}


dbReal&
dbReal::calculateWith(dbRealCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbReal::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	*this = calculator()->calc(this);
}


dbReal& 
dbReal::operator=(const dbReal& rhs)
{
	if (this == &rhs)
		return *this;
	
	*this = (double)rhs;
	return *this;
}


dbReal& 
dbReal::operator=(double n)
{
	if (fieldIsStandalone())
		*((double*) mBackend) = n;
	else {
		double *writeTo = (double *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
	return *this;
}


void 
dbReal::setNumber(long n)
{
	*this = n;
}


void 
dbReal::setString(const char* str)
{
	double n;
	istringstream is((char*)str);  // cast away const for Borland
	is >> n;
	*this = n;
}


dbReal::operator double() const
{
	double ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {	
		if (fieldIsStandalone()) 
			ret = *((double*) mBackend);
		else {
			const double *readFrom = (const double *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0.0;
		}
	}
	return ret;
}


void 
dbReal::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==realField)
		*this = *(dbReal*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbReal::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbReal::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==realField) {
		long rhs = *(dbReal*)srcField;  // safe downcast
		if (value() != rhs)
			*this = rhs;
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbReal::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbReal::fieldType() const
{
	return realField; 
}


unsigned long 
dbReal::fieldStorageLen() const
{
	return sizeof(double); 
}


void 
dbReal::extract(ostream& os) const
{
	os << (double)*this;
}


bool 
dbReal::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the real
	double n=0;
	if (skipTillNumber(is, fieldSep, recSep, true))
		is >> n;
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		*this = n;
	return hitEndOfField;
}


void 
dbReal::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		*this = (1.7*rand()-rand()*1.5);
	else
		*this = (double)seedOrCount;
}


dbQueryTrinary	
dbReal::between(double fromNum, double toNum) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::between,
		new dbQueryLiteralDouble(fromNum), new dbQueryLiteralDouble(toNum));
}


dbQueryTrinary	
dbReal::outside(double fromNum, double toNum) const
{
	return dbQueryTrinary(new dbQueryField(this), dbQueryClause::outside,
		new dbQueryLiteralDouble(fromNum), new dbQueryLiteralDouble(toNum));
}


dbQueryClause*	
dbReal::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbReal::operator==(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::equals, new dbQueryLiteralDouble(d));
}  


 dbQueryBinary	
 dbReal::operator!=(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::notEquals, new dbQueryLiteralDouble(d));
}  


 dbQueryBinary	
 dbReal::operator<(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThan, new dbQueryLiteralDouble(d));
}  


 dbQueryBinary	
 dbReal::operator<=(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::lessThanOrEqual, new dbQueryLiteralDouble(d));
}  


dbQueryBinary	
dbReal::operator>(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThan, new dbQueryLiteralDouble(d));
}  


dbQueryBinary	
dbReal::operator>=(double d) const
{
	return dbQueryBinary(new dbQueryField(this), dbQueryClause::greaterThanOrEqual, new dbQueryLiteralDouble(d));
}  

 
dbQueryLiteral* 
dbReal::makeQueryLiteral(long n) const
{
	return new dbQueryLiteralDouble(n);
}

 
dbQueryLiteral* 
dbReal::makeQueryLiteral(double n) const
{
	return new dbQueryLiteralDouble(n);
}


// -------------------------------------------------------
//                  o o f C h a r
// -------------------------------------------------------
oofChar::oofChar(unsigned long fieldWidth) : 
	dbChar(dbField::eFieldIsStandalone, fieldWidth) 
{
}


oofChar::oofChar(const char* initialString, unsigned long fieldWidth) :
	dbChar(dbField::eFieldIsStandalone, fieldWidth ? fieldWidth : strlen(initialString)) 
{
	assert((fieldWidth==0) || strlen(initialString) <= fieldWidth);
	dbChar::operator=(initialString); 
}


oofChar::oofChar(const dbChar& rhs) : 
	dbChar(dbField::eFieldIsStandalone, rhs.fieldStorageLen()) 
{ 
	dbChar::operator=(rhs); 
}


oofChar::oofChar(const oofChar& rhs) : 
	dbChar(dbField::eFieldIsStandalone, rhs.fieldStorageLen()) 
{ 
	dbChar::operator=(rhs); 
}


dbField*
oofChar::clone() const 
{
	return new oofChar(*this);
}


oofChar& 
oofChar::operator=(const oofChar& rhs) 
{ 
	dbChar::operator=(rhs); 
	return *this; 
}


oofChar& 
oofChar::operator=(const dbChar& rhs) 
{ 
	dbChar::operator=(rhs); 
	return *this; 
}


oofChar& 
oofChar::operator=(const char* rhs)
{
	dbChar::operator=(rhs); 
	return *this; 
}


oofChar& 
oofChar::operator=(char rhs)
{
	dbChar::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f T e x t
// -------------------------------------------------------
oofText::oofText() : 
	dbText(dbField::eFieldIsStandalone) 
{
	dbText::operator=(""); 
}

 
oofText::oofText(const char* initialString) :
	dbText(dbField::eFieldIsStandalone) 
{
	dbText::operator=(initialString); 
}


oofText::oofText(const dbText& rhs) : 
	dbText(dbField::eFieldIsStandalone) 
{ 
	dbText::operator=(rhs); 
} 


oofText::oofText(const oofText& rhs) : 
	dbText(dbField::eFieldIsStandalone) 
{ 
	dbText::operator=(rhs); 
} 


dbField*
oofText::clone() const 
{
	return new oofText(*this);
}


oofText& 
oofText::operator=(const char* rhs)
{ 
	dbText::operator=(rhs); 
	return *this; 
}


oofText& 
oofText::operator=(char rhs)
{ 
	dbText::operator=(rhs); 
	return *this; 
}


oofText&
oofText::operator=(const oofText& rhs) 
{ 
	dbText::operator=(rhs); 
	return *this; 
}


oofText& 
oofText::operator=(const dbText& rhs) 
{ 
	dbText::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f B L O B
// -------------------------------------------------------
oofBLOB::oofBLOB() : 
	dbBLOB(dbField::eFieldIsStandalone) 
{
}


oofBLOB::oofBLOB(const dbBLOB& rhs) : 
	dbBLOB(dbField::eFieldIsStandalone) 
{ 
	dbBLOB::operator=(rhs); 
} 


oofBLOB::oofBLOB(const oofBLOB& rhs) : 
	dbBLOB(dbField::eFieldIsStandalone) 
{ 
	dbBLOB::operator=(rhs); 
} 


oofBLOB& 
oofBLOB::operator=(const oofBLOB& rhs) 
{ 
	dbBLOB::operator=(rhs); 
	return *this; 
}


oofBLOB& 
oofBLOB::operator=(const dbBLOB& rhs) 
{ 
	dbBLOB::operator=(rhs); 
	return *this; 
}


dbField*
oofBLOB::clone() const 
{
	return new oofBLOB(*this);
}


// -------------------------------------------------------
//                  o o f L o n g
// -------------------------------------------------------
oofLong::oofLong() : 
	dbLong(dbField::eFieldIsStandalone) 
{
}


oofLong::oofLong(long initialValue) : 
	dbLong(dbField::eFieldIsStandalone) 
{ 
	dbLong::operator=(initialValue); 
}


oofLong::oofLong(const dbLong& rhs) : 
	dbLong(dbField::eFieldIsStandalone) 
{ 
	dbLong::operator=(rhs); 
}


oofLong::oofLong(const oofLong& rhs) : 
	dbLong(dbField::eFieldIsStandalone) 
{ 
	dbLong::operator=(rhs); 
}


dbField*
oofLong::clone() const 
{
	return new oofLong(*this);
}


oofLong& 
oofLong::operator=(long rhs)
{ 
	dbLong::operator=(rhs); 
	return *this; 
}


oofLong& 
oofLong::operator=(const oofLong& rhs) 
{ 
	dbLong::operator=(rhs); 
	return *this; 
}


oofLong& 
oofLong::operator=(const dbLong& rhs) 
{ 
	dbLong::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f U l o n g
// -------------------------------------------------------
oofUlong::oofUlong() : 
	dbUlong(dbField::eFieldIsStandalone) 
{
}

 
oofUlong::oofUlong(unsigned long initialValue) : 
	dbUlong(dbField::eFieldIsStandalone) 
{ 
	dbUlong::operator=(initialValue); 
}


oofUlong::oofUlong(const dbUlong& rhs) : 
	dbUlong(dbField::eFieldIsStandalone) 
{ 
	dbUlong::operator=(rhs); 
}


oofUlong::oofUlong(const oofUlong& rhs) : 
	dbUlong(dbField::eFieldIsStandalone) 
{ 
	dbUlong::operator=(rhs); 
}


dbField*
oofUlong::clone() const 
{
	return new oofUlong(*this);
}


oofUlong& 
oofUlong::operator=(unsigned long rhs)
{ 
	dbUlong::operator=(rhs); 
	return *this; 
}


oofUlong& 
oofUlong::operator=(const oofUlong& rhs) 
{ 
	dbUlong::operator=(rhs); 
	return *this; 
}


oofUlong& 
oofUlong::operator=(const dbUlong& rhs) 
{ 
	dbUlong::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f S h o r t
// -------------------------------------------------------
oofShort::oofShort() : 
	dbShort(dbField::eFieldIsStandalone) 
{
}


oofShort::oofShort(short initialValue) :
	dbShort(dbField::eFieldIsStandalone) 
{
	dbShort::operator=(initialValue); 
}


oofShort::oofShort(const dbShort& rhs) : 
	dbShort(dbField::eFieldIsStandalone) 
{ 
	dbShort::operator=(rhs); 
}


oofShort::oofShort(const oofShort& rhs) : 
	dbShort(dbField::eFieldIsStandalone) 
{ 
	dbShort::operator=(rhs); 
}


dbField*
oofShort::clone() const 
{
	return new oofShort(*this);
}


oofShort& 
oofShort::operator=(short rhs)
{ 
	dbShort::operator=(rhs); 
	return *this; 
}


oofShort& 
oofShort::operator=(const oofShort& rhs) 
{ 
	dbShort::operator=(rhs); 
	return *this; 
}


oofShort& 
oofShort::operator=(const dbShort& rhs) 
{ 
	dbShort::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f U s h o r t
// -------------------------------------------------------
oofUshort::oofUshort() : 
	dbUshort(dbField::eFieldIsStandalone) 
{
}

 
oofUshort::oofUshort(unsigned short initialValue) :
	dbUshort(dbField::eFieldIsStandalone) 
{ 
	dbUshort::operator=(initialValue); 
}


oofUshort::oofUshort(const dbUshort& rhs) : 
	dbUshort(dbField::eFieldIsStandalone) 
{ 
	dbUshort::operator=(rhs); 
}


oofUshort::oofUshort(const oofUshort& rhs) : 
	dbUshort(dbField::eFieldIsStandalone) 
{ 
	dbUshort::operator=(rhs); 
}


dbField*
oofUshort::clone() const 
{
	return new oofUshort(*this);
}


oofUshort& 
oofUshort::operator=(unsigned short rhs)
{ 
	dbUshort::operator=(rhs); 
	return *this; 
}


oofUshort& 
oofUshort::operator=(const oofUshort& rhs) 
{ 
	dbUshort::operator=(rhs); 
	return *this; 
}


oofUshort& 
oofUshort::operator=(const dbUshort& rhs) 
{ 
	dbUshort::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f R e a l
// -------------------------------------------------------
oofReal::oofReal() : 
	dbReal(dbField::eFieldIsStandalone) 
{
}


oofReal::oofReal(double initialValue) : 
	dbReal(dbField::eFieldIsStandalone) 
{ 
	dbReal::operator=(initialValue); 
}

 
oofReal::oofReal(const dbReal& rhs) : 
	dbReal(dbField::eFieldIsStandalone) 
{ 
	dbReal::operator=(rhs); 
}

 
oofReal::oofReal(const oofReal& rhs) : 
	dbReal(dbField::eFieldIsStandalone) 
{ 
	dbReal::operator=(rhs); 
}


dbField*
oofReal::clone() const 
{
	return new oofReal(*this);
}

 
oofReal& 
oofReal::operator=(double rhs)
{ 
	dbReal::operator=(rhs); 
	return *this; 
}


oofReal& 
oofReal::operator=(const oofReal& rhs) 
{ 
	dbReal::operator=(rhs); 
	return *this; 
}


oofReal& 
oofReal::operator=(const dbReal& rhs) 
{ 
	dbReal::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                   o o f B o o l
// -------------------------------------------------------
oofBool::oofBool() : 
	dbBool(dbField::eFieldIsStandalone) 
{
}

 
oofBool::oofBool(bool initialValue) :
	dbBool(dbField::eFieldIsStandalone) 
{ 
	dbBool::operator=(initialValue); 
}


oofBool::oofBool(char initialValue) :
	dbBool(dbField::eFieldIsStandalone) 
{ 
	dbBool::operator=(initialValue); 
}


oofBool::oofBool(const char* initialValue) :
	dbBool(dbField::eFieldIsStandalone) 
{ 
	setString(initialValue); 
}


oofBool::oofBool(const dbBool& rhs) : 
	dbBool(dbField::eFieldIsStandalone) 
{ 
	dbBool::operator=(rhs); 
}


oofBool::oofBool(const oofBool& rhs) : 
	dbBool(dbField::eFieldIsStandalone) 
{ 
	dbBool::operator=(rhs); 
}


dbField*
oofBool::clone() const 
{
	return new oofBool(*this);
}


oofBool& 
oofBool::operator=(bool rhs)
{ 
	dbBool::operator=(rhs); 
	return *this; 
}



oofBool& 
oofBool::operator=(const oofBool& rhs) 
{ 
	dbBool::operator=(rhs); 
	return *this; 
}


oofBool& 
oofBool::operator=(const dbBool& rhs) 
{ 
	dbBool::operator=(rhs); 
	return *this; 
}


#ifdef OOF_HAS_BOOL
oofBool& 
oofBool::operator=(int n)
{
	const bool b = n;
	*this = b;
	return *this;
}
#else
oofBool& 
oofBool::operator=(char rhs)
{ 
	dbBool::operator=(rhs); 
	return *this; 
}

#endif
