#ifndef H_OOF4
#define H_OOF4

// COPYRIGHT 1994 A.D. Software, All rights reserved

// OOFILE database numeric field-related classes
// see also oof3.h

#ifndef H_OOF3
	#include "oof3.h"
#endif

// WAS #if defined (_Windows) && !OOF_USE_ANSI_STREAMS
// need headers to define tm
	#ifndef _INC_TIME
		#include <time.h>
	#endif	
//#endif

// used in format parser for output (currently just dbDate)
// enumerated values explicity assigned for the purposes of readability

// coarse C code that will be cleaned up after 1.3

#define SLEN 50
typedef enum {eDDDD=0,eDDD=1,eDDTH=2,eDD=3,eDAYNAME=4,eDAY=5,eMMMM=6,eMMM=7,eMM=8,
			  eMONTH=9,eMON=10,eYYYY=11,eYY=12,eYEAR=13,
			  edddd=14,eddd=15,ezddth=16,edayname=17,eday=18,emmmm=19,emmm=20,ezmonth=21,emon=22,
			  eDddd=23,eDdd=24,eDayname=25,eDay=26,eD=27,eMmmm=28,eMmm=29,eMonth=30,eMon=31,eM=32,
			  eHH=33,ehh=34,eMIN=35,eSEC=36,ePM=37,ePm=38,epm=39,
			  eOPT=40,eIMP=41,ePNT=42,eCMA=43,eACT=44,eDEA=45,
	      	  eEOT=46, eNUM=47, eOTHER=48} OOF_TOKENTYPE;

/**
	Token used in parsing date or time strings.
	\ingroup oofInternals
*/
typedef struct {
	OOF_TOKENTYPE token;
	int value;
	char s[SLEN];
} TOKEN;

enum hourT { hour_24, am_hour, pm_hour };



// utility functions
bool skipTillDigitAmPm(istream&, hourT&, char fieldSep=dbTable::kFieldSep, char recSep=dbTable::kRecSep);

class dbReal;
class dbShort;
class dbLong;
class dbDate;
class dbUshort;
class dbUlong;
class dbQueryLiteral;
class dbBool;
class dbTime;
class dbDateTime;

/**
	Calculator attached to a dbShort field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbShortCalculator : public dbCalculator {
public:
	virtual ~dbShortCalculator() {};
	virtual short	calc(const dbShort*)=0;
};


/**
	Calculator attached to a dbUshort field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbUshortCalculator : public dbCalculator {
public:
	virtual ~dbUshortCalculator() {};
	virtual unsigned short	calc(const dbUshort*)=0;
};


/**
	Calculator attached to a dbBool field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbBoolCalculator : public dbCalculator {
public:
	virtual ~dbBoolCalculator() {};
	virtual bool	calc(const dbBool*)=0;
};


/**
	Calculator attached to a dbLong field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbLongCalculator : public dbCalculator {
public:
	virtual ~dbLongCalculator() {};
	virtual long	calc(const dbLong*)=0;
};


/**
	Calculator attached to a dbUlong field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbUlongCalculator : public dbCalculator {
public:
	virtual ~dbUlongCalculator() {};
	virtual unsigned long	calc(const dbUlong*)=0;
};


/**
	Calculator attached to a dbDate field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbDateCalculator : public dbCalculator {
public:
	virtual ~dbDateCalculator() {};
	virtual unsigned long	calc(const dbDate*)=0;	// internal rep of dbDate is unsigned long
};


/**
	Calculator attached to a dbReal field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRealCalculator : public dbCalculator {
public:
	virtual ~dbRealCalculator() {};
	virtual double	calc(const dbReal*)=0;
};


/**
	Calculator attached to a dbTime field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTimeCalculator : public dbCalculator {
public:
	virtual ~dbTimeCalculator() {};
	virtual long	calc(const dbTime*)=0;
};


/**
	Calculator attached to a dbDateTime field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbDateTimeCalculator : public dbCalculator {
public:
	virtual ~dbDateTimeCalculator() {};
	virtual unsigned long	calc(const dbDateTime*)=0;
};


/**
	Persistent base for fields which can be represented as numbers.
	Provides extra search and bases for common numeric operators.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbNumericField : public dbField {
protected:  // only use subclasses
	dbNumericField(const OOF_IndexOptions); 
  	dbNumericField(const char* fieldName=0); 
  	dbNumericField(const char* fieldName, const OOF_IndexOptions);
 	dbNumericField(dbField::FieldIsStandaloneT k) : dbField(k) {}; 
	// use default copy ctor
	virtual ~dbNumericField() {};
public:
	
	virtual unsigned long fieldMinWrapWidth() const;
	const oofString& formatMask() const;
	void formatMask(const char*);

	virtual void clear();  
	
// search factories
	dbQueryBinary	operator==(long) const;
	dbQueryBinary	operator<(long) const;
	dbQueryBinary	operator<=(long) const;
	dbQueryBinary	operator>=(long) const;
	dbQueryBinary	operator>(long) const;
	dbQueryBinary	operator!=(long) const;

#if defined(_MSC_VER)  || defined(__MWERKS__)
// extra operators because Visual C++ has always been stupid about
// claiming ambiguities when choosing between integer RHS conversions and creating temporaries
// Oct 2001 - Microsoft limitations made it into the C++ standard and environments like CodeWarrior
// as of about Pro6 therefore became less smart about ambiguity
	dbQueryBinary	operator==(short) const;
	dbQueryBinary	operator<(short) const;
	dbQueryBinary	operator<=(short) const;
	dbQueryBinary	operator>=(short) const;
	dbQueryBinary	operator>(short) const;
	dbQueryBinary	operator!=(short) const;

	dbQueryBinary	operator==(int) const;
	dbQueryBinary	operator<(int) const;
	dbQueryBinary	operator<=(int) const;
	dbQueryBinary	operator>=(int) const;
	dbQueryBinary	operator>(int) const;
	dbQueryBinary	operator!=(int) const;

	dbQueryBinary	operator==(unsigned long) const;
	dbQueryBinary	operator<(unsigned long) const;
	dbQueryBinary	operator<=(unsigned long) const;
	dbQueryBinary	operator>=(unsigned long) const;
	dbQueryBinary	operator>(unsigned long) const;
	dbQueryBinary	operator!=(unsigned long) const;

	dbQueryBinary	operator==(unsigned short) const;
	dbQueryBinary	operator<(unsigned short) const;
	dbQueryBinary	operator<=(unsigned short) const;
	dbQueryBinary	operator>=(unsigned short) const;
	dbQueryBinary	operator>(unsigned short) const;
	dbQueryBinary	operator!=(unsigned short) const;

	dbQueryBinary	operator==(unsigned int) const;
	dbQueryBinary	operator<(unsigned int) const;
	dbQueryBinary	operator<=(unsigned int) const;
	dbQueryBinary	operator>=(unsigned int) const;
	dbQueryBinary	operator>(unsigned int) const;
	dbQueryBinary	operator!=(unsigned int) const;

#endif  // defined(_MSC_VER)  || defined(__MWERKS__)

	dbQueryBinary	equal(long) const;
	dbQueryBinary	lessThan(long) const;
	dbQueryBinary	lessThanOrEqual(long) const;
	dbQueryBinary	greaterThanOrEqual(long) const;
	dbQueryBinary	greaterThan(long) const;
	dbQueryBinary	notEqual(long) const;

	dbQueryTrinary	between(long, long) const;
	dbQueryTrinary	outside(long, long) const;

	void operator+=(long);
	void operator-=(long);
	void operator*=(long);
	void operator/=(long);

	void operator+=(double);
	void operator-=(double);
	void operator*=(double);
	void operator/=(double);
	
// data storage
	OOF_String	mFormatMask;	// will use shared string one day
	
	virtual dbQueryLiteral* makeQueryLiteral(long) const;

protected:
	static void CheckRange(double d, long minL, long maxL, const char* file, int line);
	static void CheckRange(long l, long minL, long maxL, const char* file, int line);
	static void CheckRange(unsigned long l, unsigned long max, const char* file, int line);
	
public:
	static TOKEN get_token(const char **s, const OOF_TOKENTYPE *m, unsigned int len);
	static void parseMask(const char *s, TOKEN* r, const OOF_TOKENTYPE *map, unsigned int len);
};


/**
	Persistent field used to store a short.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbShort : public dbNumericField  {
public:
	dbShort(const char* fieldName=0); 
	dbShort(const OOF_IndexOptions); 
	dbShort(const char* fieldName, const OOF_IndexOptions);
 	dbShort(dbField::FieldIsStandaloneT k); 
	dbShort(const dbShort&);
	virtual ~dbShort();
	virtual dbField* clone() const;

	dbShort& calculateWith( dbShortCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbShortCalculator* calculator() const;
	
// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;

// data access
	short value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbShort& operator=(short);  
	dbShort& operator=(long);  
	dbShort& operator=(int);  
	dbShort& operator=(const dbShort&); // value copy 
	dbShort& operator=(double);  
	operator short() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	
	
	virtual dbQueryLiteral* makeQueryLiteral(long) const;
protected:
	virtual void CopyCalculatedValueToBuffer();
};


/**
	Persistent field used to store an unsigned short.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbUshort : public dbNumericField  {
public:
	dbUshort(const char* fieldName=0);
	dbUshort(const OOF_IndexOptions);
	dbUshort(const char* fieldName, const OOF_IndexOptions);
 	dbUshort(dbField::FieldIsStandaloneT k); 
	dbUshort(const dbUshort&);
	virtual ~dbUshort();
	virtual dbField* clone() const;

	dbUshort& calculateWith( dbUshortCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbUshortCalculator* calculator() const;
	
// search factories
	dbQueryBinary	operator==(unsigned short) const;

// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;

// data access
	unsigned short value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbUshort& operator=(unsigned short);  
	dbUshort& operator=(long);  
	dbUshort& operator=(int);  
	dbUshort& operator=(const dbUshort&); // value copy 
	dbUshort& operator=(double);  
	operator unsigned short() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	
	
	virtual dbQueryLiteral* makeQueryLiteral(long) const;
protected:
	virtual void CopyCalculatedValueToBuffer();
};


/**
	Persistent field used to store a boolean.
	Pre-ANSI could interpret 'y' and 'n' as true/false but now ANSI cast rules
	treat char values as integer so they must be non-zero/zero for true/false.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbBool : public dbNumericField  {
public:
	dbBool(const char* fieldName=0);
	dbBool(const OOF_IndexOptions);
	dbBool(const char* fieldName, const OOF_IndexOptions);
 	dbBool(dbField::FieldIsStandaloneT k); 
	dbBool(const dbBool&);
	virtual ~dbBool();
	virtual dbField* clone() const;

	dbBool& calculateWith( dbBoolCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbBoolCalculator* calculator() const;
	
	dbQueryBinary	operator==(bool) const;
	dbQueryBinary	operator==(const char*) const;
	
	operator dbQueryBinary() const;

// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual OOF_fieldTypes nativeType() const;
	virtual unsigned long fieldStorageLen() const;

// data access
	bool value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	bool operator!() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbBool& operator=(bool);  
	  
	dbBool& operator=(const dbBool&); // value copy 

#ifdef OOF_HAS_BOOL
	dbBool& operator=(int);  
	operator int() const;
#else
	dbQueryBinary	operator==(char) const;
	dbBool& operator=(char);  
#endif

	operator bool() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	
	
	virtual dbQueryLiteral* makeQueryLiteral(bool) const;	// additional version
	virtual dbQueryLiteral* makeQueryLiteral(long) const;	// to prevent hiding inherited
	
protected:
	virtual void CopyCalculatedValueToBuffer();

public:
	static char bool2char(bool);
	static bool char2bool(char);
	static bool str2bool(const char*);
};


/**
	Persistent field used to store a long.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbLong : public dbNumericField  {
public:
	dbLong(const char* fieldName=0);
	dbLong(const OOF_IndexOptions);
 	dbLong(const char* fieldName, const OOF_IndexOptions);
   	dbLong(dbField::FieldIsStandaloneT k);
   	dbLong(const dbLong&); 
	virtual ~dbLong();
	virtual dbField* clone() const;

	dbLong& calculateWith( dbLongCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbLongCalculator* calculator() const;

// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
	
// data access
	long value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbLong& operator=(long);  
	dbLong& operator=(int);  
	dbLong& operator=(const dbLong&);  // value copy
	dbLong& operator=(double);  
	operator long() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	
protected:
	virtual void CopyCalculatedValueToBuffer();
};


/**
	Persistent field used to store an unsigned long.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbUlong : public dbNumericField  {
public:
	dbUlong(const char* fieldName=0);
	dbUlong(const OOF_IndexOptions);
	dbUlong(const char* fieldName, const OOF_IndexOptions);
 	dbUlong(dbField::FieldIsStandaloneT k); 
	dbUlong(const dbUlong&);
	virtual ~dbUlong();
	virtual dbField* clone() const;

	dbUlong& calculateWith( dbUlongCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbUlongCalculator* calculator() const;
	
	dbQueryBinary	operator==(unsigned long) const;

// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
	
// data access
	unsigned long value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbUlong& operator=(unsigned long);  
	dbUlong& operator=(unsigned int);  
	dbUlong& operator=(const dbUlong&);  // value copy
	dbUlong& operator=(double);  
	operator unsigned long() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	
	virtual dbQueryLiteral* makeQueryLiteral(long n) const;
	virtual dbQueryLiteral* makeQueryLiteral(unsigned long n) const;
protected:
	virtual void CopyCalculatedValueToBuffer();
};


/**
	Persistent field used to store a double.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbReal : public dbNumericField  {
public:
	dbReal(const char* fieldName=0); 
	dbReal(const OOF_IndexOptions indexOptions);
	dbReal(const char* fieldName, const OOF_IndexOptions indexOptions);
	dbReal(const dbReal&);
 	dbReal(dbField::FieldIsStandaloneT k);
	virtual ~dbReal();
	virtual dbField* clone() const;

	dbReal& calculateWith( dbRealCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbRealCalculator* calculator() const;
	
// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;

// search factories
	dbQueryBinary	operator==(double) const;
	dbQueryBinary	operator<(double) const;
	dbQueryBinary	operator<=(double) const;
	dbQueryBinary	operator>=(double) const;
	dbQueryBinary	operator>(double) const;
	dbQueryBinary	operator!=(double) const;
	dbQueryTrinary	between(double, double) const;
	dbQueryTrinary	outside(double, double) const;

// data access
	double value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	dbReal& operator=(double);
	dbReal& operator=(const dbReal&);  // value copy
	operator double() const;
	virtual  void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);	

	virtual dbQueryLiteral* makeQueryLiteral(long) const;
	dbQueryLiteral* makeQueryLiteral(double) const;

protected:
	virtual void CopyCalculatedValueToBuffer();
	void CompleteInit();
};


/**
	Persistent field used to store a date.
	Flexible parser is very sensitive to setting dbDate::sDefaultDateOrder
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbDate : public dbNumericField {
public:
	enum dateFieldOrder {orderYMD, orderDMY, orderMDY};
	enum { NUMDATETOKENS = 35 };	dbDate(const OOF_IndexOptions indexOptions); 
	dbDate(const char* fieldName=0);
	dbDate(const char* fieldName, const OOF_IndexOptions indexOptions);  
 	dbDate(const dbDate&); 
 	dbDate(dbField::FieldIsStandaloneT); 
	virtual ~dbDate();
	virtual dbField* clone() const;

	dbDate& calculateWith( dbDateCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbDateCalculator* calculator() const;
	
// arithmetic operators
	virtual dbDate& operator++ ();
	virtual dbDate& operator-- ();
	dbDate& operator+= (long);
	dbDate& operator-= (long);
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbDate& operator+= (unsigned long);
	dbDate& operator-= (unsigned long);
	dbDate& operator+= (short);
	dbDate& operator-= (short);
	dbDate& operator+= (unsigned short);
	dbDate& operator-= (unsigned short);
	dbDate& operator+= (int);
	dbDate& operator-= (int);
	dbDate& operator+= (unsigned int);
	dbDate& operator-= (unsigned int);
#endif	//  defined(_MSC_VER)  || defined(__MWERKS__)

// days between two dates
	long operator-(const dbDate&) const;

// operators returning concrete dates
	dbDate operator-(long) const;
	dbDate operator+(long) const;
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbDate operator-(unsigned long) const;
	dbDate operator+(unsigned long) const;
	dbDate operator-(short) const;
	dbDate operator+(short) const;
	dbDate operator-(unsigned short) const;
	dbDate operator+(unsigned short) const;
	dbDate operator-(int) const;
	dbDate operator+(int) const;
	dbDate operator-(unsigned int) const;
	dbDate operator+(unsigned int) const;
#endif  //  defined(_MSC_VER)  || defined(__MWERKS__)
	
// search factories
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
	dbQueryTrinary	between(const char*, const char*) const;
	dbQueryTrinary	outside(const char*, const char*) const;

	dbQueryBinary	operator==(const dbDate&) const;
	dbQueryBinary	operator<(const dbDate&) const;
	dbQueryBinary	operator<=(const dbDate&) const;
	dbQueryBinary	operator>=(const dbDate&) const;
	dbQueryBinary	operator>(const dbDate&) const;
	dbQueryBinary	operator!=(const dbDate&) const;
	dbQueryTrinary	between(const dbDate&, const dbDate&) const;
	dbQueryTrinary	outside(const dbDate&, const dbDate&) const;
	

// reflective operators
	virtual unsigned long fieldStorageLen() const;
	virtual OOF_fieldTypes fieldType() const;
	virtual OOF_fieldTypes nativeType() const;
	bool isEmpty() const;
	bool isValid() const;

// data access

// getters
	void getDate(unsigned short& year, unsigned short& month, unsigned short& day) const;
	unsigned long value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	unsigned short day() const;
	unsigned short month() const;
	unsigned short year() const;
	operator unsigned long() const;
#ifdef _Macintosh
	void getStr255(Str255 s) const;
#endif
	virtual  void extract(ostream& os) const;  // as "YYYY/MM/DD"
	unsigned long daynum() const;

// setters
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);
	virtual void setString(const char*);  
	virtual void setNumber(long);  
#ifdef _Macintosh
	void setStr255(const Str255 s);
#endif
	dbDate& operator=(const char* str);  // "YYYY MM DD" 
	dbDate& operator=(const dbDate&);
	dbDate& operator=(const tm&);
	void setDate(unsigned long);
	virtual bool insert(istream&, char fieldSep, char recSep);
	void setDate(unsigned short year, unsigned short month, unsigned short day);
	void setDate(const tm&);
	void setDateToCurrentDate();
	void setDaynum(unsigned long);

	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);
	
public:
	static dateFieldOrder sDefaultDateOrder;
	static void long2ymd(unsigned long binaryDate, unsigned short&, unsigned short&, unsigned short&);
	static bool istream2ymd(istream& is, unsigned short&, unsigned short&, unsigned short&, dateFieldOrder theOrder=dbDate::sDefaultDateOrder);	
	static bool istream2ymd(istream& is, unsigned short&, unsigned short&, unsigned short&, char fieldSep, char recordSep,  dateFieldOrder theOrder=dbDate::sDefaultDateOrder);	
	static bool str2ymd(const char*, unsigned short&, unsigned short&, unsigned short&, dateFieldOrder theOrder=dbDate::sDefaultDateOrder);
	static void daynum2ymd(unsigned long, unsigned short&,unsigned short&,unsigned short&);

	static unsigned long str2Long(const char*, dateFieldOrder theOrder=dbDate::sDefaultDateOrder);
	static unsigned long ymd2Long(unsigned short, unsigned short, unsigned short);
	static unsigned long ymd2daynum(unsigned short,unsigned short,unsigned short);
	static ostream& ymd2Stream(unsigned short, unsigned short, unsigned short, ostream& os);
	static void ddth(unsigned short daynum, char * ddth, bool isupper=false);
	static unsigned short ymd2dow(unsigned short, unsigned short, unsigned short);
	static void ymd2dayname(unsigned short, unsigned short, unsigned short, char * dayname, bool isshort=true, bool isupper=false);
	static void month2str(unsigned short monthnum, char * monthname, bool isshort=true, bool isupper=false);
	static ostream& ymd2Stream(const char* mask, unsigned short year, unsigned short month, unsigned short day, ostream& os);
	static unsigned short istream2month(istream&);
	static unsigned short str2month (const char *);
	
	static bool checkDate(unsigned short year, unsigned short month, unsigned short day);
	static bool checkDate(const char*, dateFieldOrder theOrder=dbDate::sDefaultDateOrder);
	static bool isLeapYear(unsigned short year);
	
// current date
	static dbDate currentDate();
	static void currentDate2ymd(unsigned short&, unsigned short&, unsigned short&);
	static ostream& today(ostream& os);
	static unsigned short currentCentury();
	static unsigned short currentDay();
	static unsigned short currentMonth();
	static unsigned short currentYear();
	
// Y2K (and later centuries!) 2-digit year threshold
	static unsigned short nextCenturyThreshold();
	static void nextCenturyThreshold(unsigned short);
	static unsigned short prevCenturyThreshold();  // upper bound, eg: enter 92 in 2001 will be 1992, not 2092
	static void prevCenturyThreshold(unsigned short);
	static unsigned short adjustShortYear(unsigned short inYear);
	
	static const oofString& defaultFormatMask();
	static void defaultFormatMask(const oofString&);
	

protected:
	virtual void CopyCalculatedValueToBuffer();
// token set
	static OOF_TOKENTYPE tokenMap[NUMDATETOKENS];
	static unsigned short sNextCenturyThreshold;
	static unsigned short sPrevCenturyThreshold;
	static oofString sDefaultFormatMask;
};




/**
	Persistent field used to store a time without date.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTime : public dbNumericField {
public:
	
// constructors
	dbTime(const char* fieldName=0);
	dbTime(const OOF_IndexOptions indexOptions); 
	dbTime(const char* fieldName, const OOF_IndexOptions indexOptions);  
 	dbTime(const dbTime& rhs); 
 	dbTime(dbField::FieldIsStandaloneT k); 
	virtual ~dbTime();
	virtual dbField* clone() const;
	
/// \name arithmetic operators
//@{
	virtual dbTime& operator++ ();
	virtual dbTime& operator-- ();
	dbTime& operator+= (long);
	dbTime& operator-= (long);
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbTime& operator+= (unsigned long);
	dbTime& operator-= (unsigned long);
	dbTime& operator+= (short);
	dbTime& operator-= (short);
	dbTime& operator+= (unsigned short);
	dbTime& operator-= (unsigned short);
	dbTime& operator+= (int);
	dbTime& operator-= (int);
	dbTime& operator+= (unsigned int);
	dbTime& operator-= (unsigned int);
#endif	//  defined(_MSC_VER)  || defined(__MWERKS__)
//@}
	
// time (in seconds) between two times
	long operator-(const dbTime&) const;

// \name operators returning concrete times
//@{
	dbTime operator-(long) const;
	dbTime operator+(long) const;
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbTime operator-(unsigned long) const;
	dbTime operator+(unsigned long) const;
	dbTime operator-(short) const;
	dbTime operator+(short) const;
	dbTime operator-(unsigned short) const;
	dbTime operator+(unsigned short) const;
	dbTime operator-(int) const;
	dbTime operator+(int) const;
	dbTime operator-(unsigned int) const;
	dbTime operator+(unsigned int) const;
#endif  //  defined(_MSC_VER)  || defined(__MWERKS__)
//@}	
	
/// \name search factories
//@{
	dbQueryTrinary	between(const char*, const char*) const;
	dbQueryTrinary	outside(const char*, const char*) const;
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator>=(const char*) const;

	dbQueryTrinary	between(const dbTime&, const dbTime&) const;
	dbQueryTrinary	outside(const dbTime&, const dbTime&) const;
	dbQueryBinary	operator==(const dbTime&) const;
	dbQueryBinary	operator!=(const dbTime&) const;
	dbQueryBinary	operator<(const dbTime&) const;
	dbQueryBinary	operator<=(const dbTime&) const;
	dbQueryBinary	operator>(const dbTime&) const;
	dbQueryBinary	operator>=(const dbTime&) const;
//@}
	
/// \name reflective operators
//@{
	virtual unsigned long fieldStorageLen() const;
	virtual OOF_fieldTypes fieldType() const;
	virtual OOF_fieldTypes nativeType() const;
//@}

// utilities
	dbTime& calculateWith( dbTimeCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbTimeCalculator* calculator() const;	// inlined

	static bool checkTime(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond);

	static void long2dhmsm(long binaryTime, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond);
	static long dhmsm2long(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond);

	static long str2long(const char* str);
	static bool str2dhmsm(const char* str, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond);
	
	static bool istream2dhmsm(istream& is, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond);
	static bool istream2dhmsm(istream& is, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond, char fieldSep, char recSep);

	static ostream& hms2stream(unsigned short hour, unsigned short minute, unsigned short second, ostream& os);
	static ostream& hms2stream(unsigned short hour, unsigned short minute, ostream& os);
	//static ostream& hms2Stream(const char* mask, unsigned short hour, unsigned short minute, unsigned short second, ostream& os);

	
	
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);
	
// setters
	void setTime(long);
	void setTime(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond);
	void setTime(short day, unsigned short hour, unsigned short minute, unsigned short second);
	void setTime(unsigned short hour, unsigned short minute, unsigned short second);
	void setTime( const tm& unixTime );
	
	void setTimeToCurrentTime();

	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	dbTime& operator=(const char* str);  // "HH MM SS"
	dbTime& operator=(const dbTime& rhs);
	dbTime& operator=(const tm&);

	virtual bool insert(istream&, char fieldSep, char recSep);

// getters
	operator long() const;
	long getTime() const;		// inlined
	void getTime( short& days, unsigned short& hours, unsigned short& minutes, unsigned short& seconds, unsigned short& milliseconds ) const;
	void getTime( short& days, unsigned short& hours, unsigned short& minutes, unsigned short& seconds ) const;
	void getTime( unsigned short& hours, unsigned short& minutes, unsigned short& seconds) const;
	void getTime( tm& unixTime ) const;
	
	long value() const;			// inlined
	virtual dbQueryClause* valueAsQueryLiteral() const;
	short day() const;
	unsigned short hour() const;
	unsigned short minute() const;
	unsigned short second() const;
	unsigned short millisecond() const;	
	
	virtual  void extract(ostream& os) const;  // as "HH:MM:SS"

/// \name current time
//@{
	static ostream& now(ostream& os);
	static dbTime currentTime();
	static void currentTime2hms(unsigned short&, unsigned short&, unsigned short&);

	static unsigned short currentSecond();
	static unsigned short currentMinute();
	static unsigned short currentHour();
//@}
	
	static const oofString& defaultFormatMask();
	static void defaultFormatMask(const oofString&);
	
	
//protected:
//	//token set
//	enum { NUMDATETOKENS = 38 };
//	static OOF_TOKENTYPE tokenMap[NUMDATETOKENS];
protected:
	virtual void CopyCalculatedValueToBuffer();
	static oofString sDefaultFormatMask;
};

// AD 980728  long operator- (const dbTime&, const dbTime&);




/**
	Persistent field used to store a date and time.
	Stores as offset from base date with range of about 139 years.
	Default base date is 1/1/1900 or 1904 if call setMacCompatibleBaseYear
	Flexible parser is very sensitive to setting dbDate::sDefaultDateOrder.
	\todo confirm if affected by CodeWarrior 7EA changing tm in \<time.h\>
	to use 1900 base year.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbDateTime : public dbNumericField {
public:
	enum { NUMDATETIMETOKENS = 42 };

	static unsigned int			sBaseYear;
	
// constructors
	dbDateTime(const char* fieldName=0);
	dbDateTime(const OOF_IndexOptions indexOptions); 
	dbDateTime(const char* fieldName, const OOF_IndexOptions indexOptions);  
 	dbDateTime(const dbDateTime& rhs); 
 	dbDateTime(dbField::FieldIsStandaloneT);
	virtual ~dbDateTime();
	virtual dbField* clone() const;
	
	dbDateTime& calculateWith( dbDateTimeCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbDateTimeCalculator* calculator() const;	// inlined

// arithmetic operators
	virtual dbDateTime& operator++ ();  // inc in days
	virtual dbDateTime& operator-- ();
	dbDateTime& operator+= (long numDays);
	dbDateTime& operator-= (long numDays);
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbDateTime& operator+= (unsigned long);
	dbDateTime& operator-= (unsigned long);
	dbDateTime& operator+= (short);
	dbDateTime& operator-= (short);
	dbDateTime& operator+= (unsigned short);
	dbDateTime& operator-= (unsigned short);
	dbDateTime& operator+= (int);
	dbDateTime& operator-= (int);
	dbDateTime& operator+= (unsigned int);
	dbDateTime& operator-= (unsigned int);
#endif	
	
// operators returning concrete date-times
	unsigned long operator+ (const dbDateTime&) const;
	unsigned long operator- (const dbDateTime&) const;

	dbDateTime operator+ (long numDays) const;
	dbDateTime operator- (long numDays) const;
#if defined(_MSC_VER)  || defined(__MWERKS__)
	dbDateTime operator-(unsigned long) const;
	dbDateTime operator+(unsigned long) const;
	dbDateTime operator-(short) const;
	dbDateTime operator+(short) const;
	dbDateTime operator-(unsigned short) const;
	dbDateTime operator+(unsigned short) const;
	dbDateTime operator-(int) const;
	dbDateTime operator+(int) const;
	dbDateTime operator-(unsigned int) const;
	dbDateTime operator+(unsigned int) const;
#endif
	
//	unsigned long operator+ (const dbTime&) const;
//	unsigned long operator- (const dbTime&) const;
	
// search factories
	dbQueryTrinary	between(const char*, const char*) const;
	dbQueryTrinary	outside(const char*, const char*) const;
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator>=(const char*) const;

	dbQueryTrinary	between(const dbDateTime&, const dbDateTime&) const;
	dbQueryTrinary	outside(const dbDateTime&, const dbDateTime&) const;
	dbQueryBinary	operator==(const dbDateTime&) const;
	dbQueryBinary	operator!=(const dbDateTime&) const;
	dbQueryBinary	operator<(const dbDateTime&) const;
	dbQueryBinary	operator<=(const dbDateTime&) const;
	dbQueryBinary	operator>(const dbDateTime&) const;
	dbQueryBinary	operator>=(const dbDateTime&) const;
	
// reflective operators
	virtual unsigned long fieldStorageLen() const;
	virtual OOF_fieldTypes fieldType() const;
	virtual OOF_fieldTypes nativeType() const;
	bool isEmpty() const;
	bool isValid() const;

// utilities
	static bool checkTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second);
	static bool checkTime(const char*);

	static void ulong2ymdhms( long binaryTime, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second );
	static unsigned long ymdhms2ulong(unsigned short year, unsigned short month, unsigned short day, unsigned short hour=0, unsigned short minute=0, unsigned short second=0);

	static bool istream2ymdhms(istream& is, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, dbDate::dateFieldOrder);
	static bool istream2ymdhms(istream& is, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, char fieldSep, char recSep, dbDate::dateFieldOrder);

	static ostream& ymdhms2stream(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second, ostream& os);
	static ostream& ymdhms2stream(const char* mask, unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second, ostream& os);

	static unsigned long str2ulong(const char* str);
	static bool str2ymdhms(const char* str, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second);

	static unsigned long secsInYear( unsigned short year );
	static unsigned long secsInMonth( unsigned short year, unsigned short month );		// inlined
	static unsigned short daysInMonth( unsigned short year, unsigned short month );
	
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);

// setters
	virtual void setNumber(long);  
	virtual void setString(const char*);
	void setDateTime(unsigned long numberSecondsSinceBaseDate);
	void setDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second);
	void setDateTime( const tm& unixTime );
	void setTimeToCurrentTime();

	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);

	dbDateTime& operator=(const char* str);  // "DD MM YY HH MM SS"
	dbDateTime& operator=(const tm&);
	dbDateTime& operator=(const dbDateTime& rhs);
#ifdef _Macintosh
	void setStr255(const Str255 s);
#endif
	virtual bool insert(istream&, char fieldSep, char recSep);

// getters
	operator unsigned long() const;
	unsigned long getDateTime() const;		// inlined
	void getDateTime( unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second ) const;	
	void getDateTime( tm& unixTime ) const;
	
	unsigned long value() const;			// inlined
	virtual dbQueryClause* valueAsQueryLiteral() const;
	unsigned short year() const;
	unsigned short month() const;
	unsigned short day() const;
	unsigned short hour() const;
	unsigned short minute() const;
	unsigned short second() const;
		
#ifdef _Macintosh
	void getStr255(Str255 s) const;
#endif
	virtual  void extract(ostream& os) const;  // as "DD/MM/YY HH:MM:SS"

protected:
	virtual void CopyCalculatedValueToBuffer();

public:
// current time
	static ostream& now(ostream& os);
	static void currentTime2ymdhms(unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second);

	static dbDateTime currentTime();

	static unsigned short currentYear();
	static unsigned short currentMonth();
	static unsigned short currentDay();
	static unsigned short currentHour();
	static unsigned short currentMinute();
	static unsigned short currentSecond();
	
	static const oofString& defaultFormatMask();
	static void defaultFormatMask(const oofString&);
	
	
#ifdef _Macintosh
	static void setMacCompatibleBaseYear();
#endif
	
protected:
	//token set
	static OOF_TOKENTYPE tokenMap[NUMDATETIMETOKENS];
	static oofString sDefaultFormatMask;
};
// AD 980728 unsigned long operator- (const dbDateTime&, const dbDateTime&);


/**
	\defgroup standaloneFields Local variables compatible with database fields.
	\anchor standaloneFields_anchor
	Non-persistent subclasses used to declare local variables of type compatible 
	with the database fields. 
	\par
	Implemented by a flag in dbField so any method returning a field
	type will actually return a non-persistent object when called on such.
	eg: calling dbDate::currentDate effectively returns an oofDate.
	\see dbField::eFieldIsStandalone used in dbField and subclass ctors
	\ingroup oofDatabase
*/

/**
	Use to have local non-persistent variable of type dbChar.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofChar : public dbChar
{
public:
 	oofChar(unsigned long fieldWidth = 80);
 	oofChar(const char* initialString, unsigned long fieldWidth = 0);
 	oofChar(const dbChar&);
 	oofChar(const oofChar&);
	virtual ~oofChar() {};
	virtual dbField* clone() const;
	
	oofChar& operator=(const char*);
	oofChar& operator=(char);
	oofChar& operator=(const oofChar&);
	oofChar& operator=(const dbChar&);
};


/**
	Use to have local non-persistent variable of type dbText.
*/
class OOFILE_EXPORT oofText : public dbText
{
public:
 	oofText();
 	oofText(const char* initialString);
 	oofText(const oofText&);
 	oofText(const dbText&);
	virtual ~oofText() {};
	virtual dbField* clone() const;
	
	oofText& operator=(const char*);
	oofText& operator=(char);
	oofText& operator=(const oofText&);
	oofText& operator=(const dbText&);
};


/**
	Use to have local non-persistent variable of type dbBLOB.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofBLOB : public dbBLOB
{
public:
 	oofBLOB();
 	oofBLOB(const oofBLOB&);
 	oofBLOB(const dbBLOB&);
	virtual ~oofBLOB() {};
	virtual dbField* clone() const;
	
	oofBLOB& operator=(const oofBLOB&);
	oofBLOB& operator=(const dbBLOB&);
};


/**
	Use to have local non-persistent variable of type dbDate.
	\ingroup oofStandaloneFields
	\see dbDate::currentDate
*/
class OOFILE_EXPORT oofDate : public dbDate
{
public:
 	oofDate();
	oofDate(const char* initialDate);
 	oofDate(const dbDate&);
 	oofDate(const oofDate&);
 	oofDate(const tm&);
	virtual ~oofDate() {};
	virtual dbField* clone() const;
	
	oofDate& operator=(const char*);
	oofDate& operator=(const oofDate&);
	oofDate& operator=(const dbDate&);
	oofDate& operator=(const tm&);
};



/**
	Use to have local non-persistent variable of type dbLong.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofLong : public dbLong
{
public:
 	oofLong();
 	oofLong(long);
 	oofLong(const dbLong&);
 	oofLong(const oofLong&);
	virtual ~oofLong() {};
	virtual dbField* clone() const;
	
	oofLong& operator=(long);
	oofLong& operator=(const oofLong&);
	oofLong& operator=(const dbLong&);
};


/**
	Use to have local non-persistent variable of type dbUlong.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofUlong : public dbUlong
{
public:
 	oofUlong();
 	oofUlong(unsigned long);
 	oofUlong(const dbUlong&);
 	oofUlong(const oofUlong&);
	virtual ~oofUlong() {};
	virtual dbField* clone() const;
	
	oofUlong& operator=(unsigned long);
	oofUlong& operator=(const oofUlong&);
	oofUlong& operator=(const dbUlong&);
};


/**
	Use to have local non-persistent variable of type dbShort.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofShort : public dbShort
{
public:
 	oofShort();
 	oofShort(short);
 	oofShort(const dbShort&);
 	oofShort(const oofShort&);
	virtual ~oofShort() {};
	virtual dbField* clone() const;
	
	oofShort& operator=(short);
	oofShort& operator=(const oofShort&);
	oofShort& operator=(const dbShort&);
};


/**
	Use to have local non-persistent variable of type dbUshort.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofUshort : public dbUshort
{
public:
 	oofUshort(); 
 	oofUshort(unsigned short); 
 	oofUshort(const dbUshort&); 
 	oofUshort(const oofUshort&); 
	virtual ~oofUshort() {};
	virtual dbField* clone() const;
	
	oofUshort& operator=(unsigned short);
	oofUshort& operator=(const oofUshort&);
	oofUshort& operator=(const dbUshort&);
};


/**
	Use to have local non-persistent variable of type dbReal.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofReal : public dbReal
{
public:
 	oofReal(); 
 	oofReal(double); 
 	oofReal(const dbReal&); 
 	oofReal(const oofReal&); 
	virtual ~oofReal() {};
	virtual dbField* clone() const;
	
	oofReal& operator=(double);
	oofReal& operator=(const oofReal&);
	oofReal& operator=(const dbReal&);
};


/**
	Use to have local non-persistent variable of type dbBool.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofBool : public dbBool
{
public:
 	oofBool(); 
 	oofBool(bool); 
 	oofBool(char); 
 	oofBool(const char*); 
 	oofBool(const dbBool&); 
 	oofBool(const oofBool&); 
	virtual ~oofBool() {};
	virtual dbField* clone() const;
	
	oofBool& operator=(bool);

#ifdef OOF_HAS_BOOL
	oofBool& operator=(int);  
#else
	oofBool& operator=(char);	// ambiguous with the bool type
#endif
	
	oofBool& operator=(const oofBool&);
	oofBool& operator=(const dbBool&);
};


/**
	Use to have local non-persistent variable of type dbTime.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofTime : public dbTime
{
public:
 	oofTime();
	oofTime(const char* initialTime);
 	oofTime(const dbTime&);
 	oofTime(const oofTime&);
	oofTime(const tm&);
	virtual ~oofTime() {};
	virtual dbField* clone() const;
	
	oofTime& operator=(const char*);
	oofTime& operator=(const oofTime&);
	oofTime& operator=(const dbTime&);
	oofTime& operator=(const tm&);
};


/**
	Use to have local non-persistent variable of type dbDateTime.
	\ingroup oofStandaloneFields
*/
class OOFILE_EXPORT oofDateTime : public dbDateTime
{
public:
 	oofDateTime();
	oofDateTime(const char* initialTime);
	oofDateTime(const tm&);
 	oofDateTime(const dbDateTime&);
 	oofDateTime(const oofDateTime&);
	virtual ~oofDateTime() {};
	virtual dbField* clone() const;
	
	oofDateTime& operator=(const char*);
	oofDateTime& operator=(const tm&);
	oofDateTime& operator=(const oofDateTime&);
	oofDateTime& operator=(const dbDateTime&);
};

// -------------------------------------------------------
//              d b N u m e r i c F i e l d
// -------------------------------------------------------
inline const oofString& 
dbNumericField::formatMask() const
{
	return mFormatMask;
}


inline void 
dbNumericField::formatMask(const char* str)
{
	mFormatMask = str;
}


// -------------------------------------------------------
//                   d b S h o r t 
// -------------------------------------------------------
inline dbShort& 
dbShort::operator=(long rhs)
{
#ifdef OOF_DEBUG
	CheckRange(rhs, SHRT_MIN, SHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (short)rhs;
	return *this;
}


inline dbShort& 
dbShort::operator=(int rhs)  // assume same as long
{
#ifdef OOF_DEBUG
	CheckRange((long)rhs, SHRT_MIN, SHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (short)rhs;
	return *this;
}


inline dbShort& 
dbShort::operator=(double rhs)
{
#ifdef OOF_DEBUG
	CheckRange(rhs, SHRT_MIN, SHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (short)rhs;
	return *this;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbShort& fld)
{
	fld.extract(os);
	return os;
}


inline short
dbShort::value() const
{
	return (short)*this;	
}


inline dbShortCalculator* 
dbShort::calculator() const
{
	return (dbShortCalculator*) mCalculator;	// safe downcast
}


// -------------------------------------------------------
//                   d b U s h o r t 
// -------------------------------------------------------
inline dbUshort& 
dbUshort::operator=(long rhs)
{
#ifdef OOF_DEBUG
	CheckRange(rhs, USHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (unsigned short)rhs;
	return *this;
}


inline dbUshort& 
dbUshort::operator=(int rhs)  // assume same as long
{
#ifdef OOF_DEBUG
	CheckRange((unsigned long)rhs, USHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (unsigned short)rhs;
	return *this;
}


inline dbUshort& 
dbUshort::operator=(double rhs)
{
#ifdef OOF_DEBUG
	CheckRange((unsigned long)rhs, USHRT_MAX, __FILE__, __LINE__);
#endif
	*this = (unsigned short)rhs;
	return *this;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbUshort& fld)
{
	fld.extract(os);
	return os;
}


inline unsigned short
dbUshort::value() const
{
	return (unsigned short)*this;	
}


inline dbUshortCalculator* 
dbUshort::calculator() const
{
	return (dbUshortCalculator*) mCalculator;	// safe downcast
}


// -------------------------------------------------------
//                   d b B o o l
// -------------------------------------------------------
OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbBool& fld)
{
	fld.extract(os);
	return os;
}


inline dbBoolCalculator* 
dbBool::calculator() const
{
	return (dbBoolCalculator*) mCalculator;	// safe downcast
}


inline bool
dbBool::value() const
{
	return (bool)*this;	
}


#ifdef OOF_HAS_BOOL
inline 
dbBool::operator int() const
{
	int i = (bool)*this;
	return i;
}


inline dbBool& 
dbBool::operator=(int n)
{
	bool b = n;
	*this = b;
	return *this;
}
#endif


inline bool 
dbBool::operator!() const
{
	bool ret = (bool)*this;
	return !ret;
}


// -------------------------------------------------------
//                      d b L o n g
// -------------------------------------------------------
inline dbLong& 
dbLong::operator=(int rhs)
{
	*this = (long)rhs;
	return *this;
}


inline dbLong& 
dbLong::operator=(double rhs)
{
#ifdef OOF_DEBUG
	CheckRange(rhs, LONG_MIN, LONG_MAX, __FILE__, __LINE__);
#endif
	*this = (long)rhs;
	return *this;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbLong& fld)
{
	fld.extract(os);
	return os;
}


inline long
dbLong::value() const
{
	return (long)*this;	
}


inline dbLongCalculator* 
dbLong::calculator() const
{
	return (dbLongCalculator*) mCalculator;	// safe downcast
}



// -------------------------------------------------------
//                      d b U l o n g
// -------------------------------------------------------
inline dbUlong& 
dbUlong::operator=(unsigned int rhs)
{
#ifdef OOF_DEBUG
	CheckRange(rhs, ULONG_MAX, __FILE__, __LINE__);
#endif
	*this = (unsigned long)rhs;
	return *this;
}


inline dbUlong& 
dbUlong::operator=(double rhs)
{
#ifdef OOF_DEBUG
	CheckRange((unsigned long)rhs, ULONG_MAX, __FILE__, __LINE__);
#endif
	*this = (unsigned long)rhs;
	return *this;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbUlong& fld)
{
	fld.extract(os);
	return os;
}


inline unsigned long
dbUlong::value() const
{
	return (unsigned long)*this;	
}


inline dbUlongCalculator* 
dbUlong::calculator() const
{
	return (dbUlongCalculator*) mCalculator;	// safe downcast
}



// -------------------------------------------------------
//                      d b D a t e
// -------------------------------------------------------
inline const oofString&
dbDate::defaultFormatMask()
{
	return sDefaultFormatMask;
}


inline void
dbDate::defaultFormatMask(const oofString& inMask) 
{
	sDefaultFormatMask = inMask;
}


inline dbDate&
dbDate::operator=(const tm& rhs)
{ 
	setDate(rhs); 
	return *this;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbDate& fld)
{
	fld.extract(os);
	return os;
}


inline dbDateCalculator* 
dbDate::calculator() const
{
	return (dbDateCalculator*) mCalculator;	// safe downcast
}


inline unsigned long
dbDate::value() const
{
	return (unsigned long)*this;	
}

inline bool 
dbDate::isEmpty() const
{
	bool ret = ((unsigned long)*this == 0);
	return ret;
}

inline bool 
dbDate::isValid() const
{
	bool ret = ((unsigned long)*this != ULONG_MAX);
	return ret;
}


#if defined(_MSC_VER)  || defined(__MWERKS__)
// extra operators because Visual C++ has always been stupid about
// claiming ambiguities when choosing between integer RHS conversions and creating temporaries
inline dbDate& 
dbDate::operator+= (unsigned long n)
{
	return *this += (long)n;
}


inline dbDate& 
dbDate::operator-= (unsigned long n)
{
	return *this -= (long)n;
}


inline dbDate& 
dbDate::operator+= (short n)
{
	return *this += (long)n;
}


inline dbDate& 
dbDate::operator-= (short n)
{
	return *this -= (long)n;
}


inline dbDate& 
dbDate::operator+= (unsigned short n)
{
	return *this += (long)n;
}


inline dbDate& 
dbDate::operator-= (unsigned short n)
{
	return *this -= (long)n;
}


inline dbDate& 
dbDate::operator+= (int n)
{
	return *this += (long)n;
}


inline dbDate& 
dbDate::operator-= (int n)
{
	return *this -= (long)n;
}


inline dbDate& 
dbDate::operator+= (unsigned int n)
{
	return *this += (long)n;
}


inline dbDate& 
dbDate::operator-= (unsigned int n)
{
	return *this -= (long)n;
}


inline dbDate 
dbDate::operator-(unsigned long n) const
{
	return *this - (long)n;
}


inline dbDate 
dbDate::operator+(unsigned long n) const
{
	return *this + (long)n;
}


inline dbDate 
dbDate::operator-(short n) const
{
	return *this - (long)n;
}


inline dbDate 
dbDate::operator+(short n) const
{
	return *this + (long)n;
}


inline dbDate 
dbDate::operator-(unsigned short n) const
{
	return *this - (long)n;
}


inline dbDate 
dbDate::operator+(unsigned short n) const
{
	return *this + (long)n;
}


inline dbDate 
dbDate::operator-(int n) const
{
	return *this - (long)n;
}


inline dbDate 
dbDate::operator+(int n) const
{
	return *this + (long)n;
}


inline dbDate 
dbDate::operator-(unsigned int n) const
{
	return *this - (long)n;
}


inline dbDate 
dbDate::operator+(unsigned int n) const
{
	return *this + (long)n;
}

#endif	// Visual-C++ only


inline unsigned short 
dbDate::nextCenturyThreshold()
{
	return sNextCenturyThreshold;
}


inline unsigned short 
dbDate::prevCenturyThreshold()
{
	return sPrevCenturyThreshold;
}


// -------------------------------------------------------
//                      d b R e a l
// -------------------------------------------------------
OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbReal& fld)
{
	fld.extract(os);
	return os;
}


inline double
dbReal::value() const
{
	return (double)*this;	
}



inline dbRealCalculator* 
dbReal::calculator() const
{
	return (dbRealCalculator*) mCalculator;	// safe downcast
}


// -------------------------------------------------------
//                      d b T i m e
// -------------------------------------------------------
inline dbTime& 
dbTime::operator=(const tm& rhs) 
{ 
	setTime(rhs); 
	return *this; 
}


inline const oofString&
dbTime::defaultFormatMask()
{
	return sDefaultFormatMask;
}


inline void
dbTime::defaultFormatMask(const oofString& inMask) 
{
	sDefaultFormatMask = inMask;
}


inline dbTimeCalculator* 
dbTime::calculator() const
{
	return (dbTimeCalculator*) mCalculator;	// safe downcast
}

inline long
dbTime::getTime() const
{
	return (long)*this;
}

inline long
dbTime::value() const
{
	return (long)*this;	
}

#if defined(_MSC_VER)  || defined(__MWERKS__)
inline dbTime& 
dbTime::operator+= (unsigned long n)
{
	return *this += (long)n;
}


inline dbTime& 
dbTime::operator-= (unsigned long n)
{
	return *this -= (long)n;
}


inline dbTime& 
dbTime::operator+= (short n)
{
	return *this += (long)n;
}


inline dbTime& 
dbTime::operator-= (short n)
{
	return *this -= (long)n;
}


inline dbTime& 
dbTime::operator+= (unsigned short n)
{
	return *this += (long)n;
}


inline dbTime& 
dbTime::operator-= (unsigned short n)
{
	return *this -= (long)n;
}


inline dbTime& 
dbTime::operator+= (int n)
{
	return *this += (long)n;
}


inline dbTime& 
dbTime::operator-= (int n)
{
	return *this -= (long)n;
}


inline dbTime& 
dbTime::operator+= (unsigned int n)
{
	return *this += (long)n;
}


inline dbTime& 
dbTime::operator-= (unsigned int n)
{
	return *this -= (long)n;
}


inline dbTime 
dbTime::operator-(unsigned long n) const
{
	return *this - (long)n;
}


inline dbTime 
dbTime::operator+(unsigned long n) const
{
	return *this + (long)n;
}


inline dbTime 
dbTime::operator-(short n) const
{
	return *this - (long)n;
}


inline dbTime 
dbTime::operator+(short n) const
{
	return *this + (long)n;
}


inline dbTime 
dbTime::operator-(unsigned short n) const
{
	return *this - (long)n;
}


inline dbTime 
dbTime::operator+(unsigned short n) const
{
	return *this + (long)n;
}


inline dbTime 
dbTime::operator-(int n) const
{
	return *this - (long)n;
}


inline dbTime 
dbTime::operator+(int n) const
{
	return *this + (long)n;
}


inline dbTime 
dbTime::operator-(unsigned int n) const
{
	return *this - (long)n;
}


inline dbTime 
dbTime::operator+(unsigned int n) const
{
	return *this + (long)n;
}

#endif	// defined(_MSC_VER)  || defined(__MWERKS__)

OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbTime& fld)
{
	fld.extract(os);
	return os;
}


// -------------------------------------------------------
//                    d b D a t e T i m e
// -------------------------------------------------------
inline const oofString&
dbDateTime::defaultFormatMask()
{
	return sDefaultFormatMask;
}


inline void
dbDateTime::defaultFormatMask(const oofString& inMask) 
{
	sDefaultFormatMask = inMask;
}


inline dbDateTime& 
dbDateTime::operator=(const tm& rhs) 
{ 
	setDateTime(rhs); 
	return *this; 
}


inline dbDateTimeCalculator* 
dbDateTime::calculator() const
{
	return (dbDateTimeCalculator*) mCalculator;	// safe downcast
}


inline unsigned long
dbDateTime::getDateTime() const
{
	return (unsigned long)*this;
}


inline unsigned long
dbDateTime::value() const
{
	return (unsigned long)*this;	
}


inline bool 
dbDateTime::isEmpty() const
{
	bool ret = ((unsigned long)*this == 0);
	return ret;
}


inline bool 
dbDateTime::isValid() const
{
	bool ret = ((unsigned long)*this != ULONG_MAX);
	return ret;
}

#if defined(_MSC_VER)  || defined(__MWERKS__)
inline dbDateTime& 
dbDateTime::operator+= (unsigned long n)
{
	return *this += (long)n;
}


inline dbDateTime& 
dbDateTime::operator-= (unsigned long n)
{
	return *this -= (long)n;
}


inline dbDateTime& 
dbDateTime::operator+= (short n)
{
	return *this += (long)n;
}


inline dbDateTime& 
dbDateTime::operator-= (short n)
{
	return *this -= (long)n;
}


inline dbDateTime& 
dbDateTime::operator+= (unsigned short n)
{
	return *this += (long)n;
}


inline dbDateTime& 
dbDateTime::operator-= (unsigned short n)
{
	return *this -= (long)n;
}


inline dbDateTime& 
dbDateTime::operator+= (int n)
{
	return *this += (long)n;
}


inline dbDateTime& 
dbDateTime::operator-= (int n)
{
	return *this -= (long)n;
}


inline dbDateTime& 
dbDateTime::operator+= (unsigned int n)
{
	return *this += (long)n;
}


inline dbDateTime& 
dbDateTime::operator-= (unsigned int n)
{
	return *this -= (long)n;
}


inline dbDateTime 
dbDateTime::operator-(unsigned long n) const
{
	return *this - (long)n;
}


inline dbDateTime 
dbDateTime::operator+(unsigned long n) const
{
	return *this + (long)n;
}


inline dbDateTime 
dbDateTime::operator-(short n) const
{
	return *this - (long)n;
}


inline dbDateTime 
dbDateTime::operator+(short n) const
{
	return *this + (long)n;
}


inline dbDateTime 
dbDateTime::operator-(unsigned short n) const
{
	return *this - (long)n;
}


inline dbDateTime 
dbDateTime::operator+(unsigned short n) const
{
	return *this + (long)n;
}


inline dbDateTime 
dbDateTime::operator-(int n) const
{
	return *this - (long)n;
}


inline dbDateTime 
dbDateTime::operator+(int n) const
{
	return *this + (long)n;
}


inline dbDateTime 
dbDateTime::operator-(unsigned int n) const
{
	return *this - (long)n;
}


inline dbDateTime 
dbDateTime::operator+(unsigned int n) const
{
	return *this + (long)n;
}

#endif	// defined(_MSC_VER)  || defined(__MWERKS__)
	

OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbDateTime& fld)
{
	fld.extract(os);
	return os;
}



// -------------------------------------------------------
//                  o o f D a t e T i m e
// -------------------------------------------------------
inline oofDateTime& 
oofDateTime::operator=(const char* rhs) 
{ 
	dbDateTime::operator=(rhs); 
	return *this; 
}


inline oofDateTime& 
oofDateTime::operator=(const tm& rhs) 
{ 
	dbDateTime::operator=(rhs); 
	return *this; 
}


inline oofDateTime& 
oofDateTime::operator=(const oofDateTime& rhs) 
{ 
	dbDateTime::operator=(rhs); 
	return *this; 
}


inline oofDateTime& 
oofDateTime::operator=(const dbDateTime& rhs) 
{ 
	dbDateTime::operator=(rhs); 
	return *this; 
}


// -------------------------------------------------------
//                  o o f D a t e
// -------------------------------------------------------
inline oofDate& 
oofDate::operator=(const oofDate& rhs) 
{ 
	dbDate::operator=(rhs); 
	return *this; 
}


inline oofDate& 
oofDate::operator=(const dbDate& rhs) 
{ 
	dbDate::operator=(rhs); 
	return *this; 
}


inline oofDate& 
oofDate::operator=(const tm& rhs) 
{ 
	dbDate::operator=(rhs); 
	return *this; 
}


inline oofDate& 
oofDate::operator=(const char* rhs) 
{ 
	dbDate::operator=(rhs); 
	return *this; 
}




// -------------------------------------------------------
//                  o o f T i m e
// -------------------------------------------------------
inline oofTime& 
oofTime::operator=(const char* rhs) 
{ 
	dbTime::operator=(rhs); 
	return *this; 
}


inline oofTime& 
oofTime::operator=(const oofTime& rhs) 
{ 
	dbTime::operator=(rhs); 
	return *this; 
}


inline oofTime& 
oofTime::operator=(const dbTime& rhs) 
{ 
	dbTime::operator=(rhs); 
	return *this; 
}


inline oofTime& 
oofTime::operator=(const tm& rhs) 
{ 
	dbTime::operator=(rhs); 
	return *this; 
}

#endif

