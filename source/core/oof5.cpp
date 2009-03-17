// COPYRIGHT 1994-1997 A.D. Software, All rights reserved

// OOFILE database field-related classes
// date & time fields - see also oof3.cpp & oof4.cpp

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
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

#include <ctype.h>
#include <time.h>  // for dbDate stuff

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// token maps define the order in which we search for tokens
// and should really be sorted in decreasing order of being used
// by typical users, except this whole tokenising scheme is probably
// going to be rewritten

// define static variables
dbDate::dateFieldOrder dbDate::sDefaultDateOrder=orderDMY;
OOF_TOKENTYPE dbDate::tokenMap[dbDate::NUMDATETOKENS] = {eDDDD,eDDD,eDDTH,eDD,eDAYNAME,eDAY,eMMMM,eMMM,eMM,
				eMONTH,eMON,eYYYY,eYY,eYEAR,edddd,eddd,ezddth,edayname,eday,emmmm,emmm,ezmonth,emon,
				eDddd,eDdd,eDayname,eDay,eD,eMmmm,eMmm,eMonth,eMon,eM,eACT,eDEA};

unsigned short dbDate::sNextCenturyThreshold = 1970;  
unsigned short dbDate::sPrevCenturyThreshold = 2070;
	
oofString dbDate::sDefaultFormatMask;
oofString dbDateTime::sDefaultFormatMask;
oofString dbTime::sDefaultFormatMask;


// gross c stuff
// eMIN must come before eM or the parser confuses one for the other
OOF_TOKENTYPE dbDateTime::tokenMap[dbDateTime::NUMDATETIMETOKENS] = {
				eDDDD,eDDD,eDDTH,eDD,eDAYNAME,eDAY,eMMMM,eMMM,eMM,
				eMONTH,eMON,eYYYY,eYY,eYEAR,edddd,eddd,ezddth,edayname,eday,emmmm,emmm,ezmonth,emon,
				eHH,ehh,eMIN,eSEC,ePM,ePm,epm,
				eDddd,eDdd,eDayname,eDay,eD,eMmmm,eMmm,eMonth,eMon,eM,
				eACT,eDEA};

// must be within the 20th Century (19xx)
// AD 980504 I don't understand the above comment. AFAIK the only restriction is that it must
// be within about 130 years of the upper date you wish to record, unless there's a weak algorithm
unsigned int dbDateTime::sBaseYear = 1970;

#define NUMTOKENS 46
#define MAXTOKENS 50
#define MAXLINELEN 200

// token matching strings and lengths - in the same order as the OOF_TOKENTYPE enums defined in oof4.h

static char *vocab[NUMTOKENS]={"DDDD", "DDD", "DDTH", "DD", "DAYNAME", "DAY", "MMMM", "MMM","MM",
			      			   "MONTH", "MON", "YYYY", "YY", "YEAR",
			      			   "dddd", "ddd", "ddth", "dayname", "day", "mmmm", "mmm", "month", "mon",
			      			   "Dddd", "Ddd", "Dayname", "Day", "D", "Mmmm", "Mmm", "Month", "Mon", "M",
			      			   "HH", "hh", "MIN", "SEC", "PM", "Pm", "pm",
			      			   "#", "Z", ".", ",", "<", ">"};

static short vlen[NUMTOKENS]={4,3,4,2,7,3,4,3,2,
							  5,3,4,2,4,
							  4,3,4,7,3,4,3,5,3,
							  4,3,7,3,1,4,3,5,3,1,
							  2,2,3,3,2,2,2,
							  1,1,1,1,1,1};
	
// end gross c stuff

// global to source file oof4.cpp (used by dbDateTime,dbTime,and dbFullDateTime)

const unsigned long kMilliSecondsInDay = 24*60*60*1000;
const unsigned long	kSecondsInDay = 24*60*60;
const unsigned long	kSecondsInHour = 60*60;
const unsigned long kSecondsInMinute = 60;


bool
skipTillDigitAmPm( istream& is, hourT& hourMod, char fieldSep, char recSep)
{
	char ch, nextch;
	
	while( is.good() )					// not end of line
	{
		ch = is.peek();
		if ((ch==fieldSep) || (ch==recSep)) {
			return false;  // early exit - empty line
		}
		if( (ch>='0') && (ch<='9') ) {		// digit
			return true;
		}
		else
		{
			if( (toupper(ch)=='P') || (toupper(ch)=='A') )
			{
				is >> ch;
				nextch = is.peek();
				if( toupper(nextch)=='M' )	// "PM" or "AM"
				{
					if( toupper(ch)=='P' )
						hourMod = pm_hour;
					else
						hourMod = am_hour;
				}
			}
		}
		is >> ch;						// skip the char
	}
	return false;
}


// -------------------------------------------------------
//               d b N u m e r i c F i e l d  
// -------------------------------------------------------
TOKEN
dbNumericField::get_token(const char **s, const OOF_TOKENTYPE *m, unsigned int len)
/* Takes a string handle and checks if the current item is in the */
/* vocabulary and returns a token number of that vocab item.. It  */
/* also moves the string along past that token                    */
{
	TOKEN retval;
	const OOF_TOKENTYPE *map;
	
	map = m;
	
	if (isdigit(**s))
	{
		unsigned int num;

		sscanf(*s,"%i",&num);
		retval.token=eNUM;
		retval.value=num;
		for((*s)++;(isdigit(**s)||(**s=='x'));(*s)++)
			;
	} else {
		unsigned int i;

		for(i=0;i<len;i++)	// the number of tokens in the subset
		{
			if (strncmp(*s,vocab[map[i]],vlen[map[i]])==0)
			{
				*s+=vlen[map[i]];
				retval.token=map[i];  // using mapping table to match positions
//				retval.token=(OOF_TOKENTYPE)i;
				return(retval);
			}
		}
		retval.token=eOTHER;
		for(i=0;**s!='\0';)
		{
			retval.s[i++]=**s;
			(*s)++;
			for(unsigned int j=0;j<len;j++)
				if (strncmp(*s,vocab[map[j]],vlen[map[j]])==0){
					retval.s[i]='\0';
					return(retval);
				}
			assert(i<SLEN-1);
		}
		retval.s[i]='\0';
	}
	return(retval);
}


void
dbNumericField::parseMask(const char *s, TOKEN* r, const OOF_TOKENTYPE *map, unsigned int len)
{
	short index=0;
	const char *stringStart;
	stringStart = s;
	bool haveACT = false;
	while((*s!='\0')&&(index<MAXTOKENS-1)) {
		r[index]=get_token(&s,map,len);
		/* At this point we wish to enforce the following behaviour:
		If an ACT token is found, everything found before it is considered as
		the string equivalent only. If a DEA token is found, everything after
		it is considered as string equivalent only */
		if (r[index].token== eACT)
			haveACT = true;
		if (index && (r[index].token== eACT)) {	// ACT found, but not the first token
			index = 0;	// reset to the start
			r[index].token = eOTHER;	// make the first token an OTHER
			strncpy(r[index].s,stringStart,(s-stringStart-1));	// copy everything found so far into first token
			*(r[index].s+(s-stringStart-1)) ='\0';	// terminate the new string
			r[++index].token = eACT;
			index++;
			continue;
		}
		else
			if (haveACT&&(*s!='\0')&&(r[index].token == eDEA)) {	// DEA found, and there is more string to come
				r[++index].token = eOTHER;
				strcpy(r[index].s,s);
				index++;
				break;	// we're finished now
			}
		index++;
	}
	r[index].token=eEOT; // Just in Case
}


// -------------------------------------------------------
//                      d b D a t e
// -------------------------------------------------------
// prototype private func
void skipStreamPattern(istream&, const char *);

dbDate::dbDate(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbDate::dbDate(const char *fieldName)  :
	dbNumericField(fieldName)
{
	formatMask(sDefaultFormatMask);
}


dbDate::dbDate(const char *fieldName, const OOF_IndexOptions indexOptions)  :
				dbNumericField(fieldName, indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbDate::dbDate(dbField::FieldIsStandaloneT k) : 
	dbNumericField(k) 
{
	formatMask(sDefaultFormatMask);
}


dbDate::dbDate(const dbDate& rhs) : 
	dbNumericField(rhs)
{}


dbDate::~dbDate()
{
}


dbField*
dbDate::clone() const 
{
	return new dbDate(*this);
}





dbDate&
dbDate::calculateWith(dbDateCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbDate::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	setDate( calculator()->calc(this) );
}


unsigned long 
dbDate::fieldStorageLen() const
{
	return sizeof(unsigned long); 
}


void 
dbDate::copyValueFrom(const dbField* srcField)
{
	if (srcField->fieldType()==dateField)
		*this = *(dbDate*)srcField;  // safe downcast
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbDate::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


void 
dbDate::copyValueIfDifferent(const dbField* srcField)
{
	if (srcField->fieldType()==dateField) {
		unsigned long rhs = *(dbDate*)srcField;  // safe downcast
		if (value() != rhs)
			this->setDate(rhs);
	}
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbDate::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


OOF_fieldTypes 
dbDate::fieldType() const
{
	return dateField; 
}


OOF_fieldTypes 
dbDate::nativeType() const
{
	return uLongField; 
}


void 
dbDate::extract(ostream& os) const
{
	const unsigned long binDate = (long)*this;
	if (binDate) {
		unsigned short day, month, year;
		long2ymd(binDate, year, month, day);
		if (mFormatMask.length())
			ymd2Stream(mFormatMask, year, month, day, os);
		else
			ymd2Stream(year, month, day, os);
	}
}


bool 
dbDate::insert(istream& is, char fieldSep, char recSep)
{
	// is the stream good?
	if (!is.good())
		return false;
	// read in the date
	unsigned short y,m,d;
	istream2ymd(is,y,m,d,fieldSep,recSep,sDefaultDateOrder);
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		setDate(y,m,d);
	return hitEndOfField;
}


ostream& 
dbDate::today(ostream& os)
{
	unsigned short day, month, year;
	currentDate2ymd(year, month, day);
	return ymd2Stream(year, month, day, os);
}


ostream& 
dbDate::ymd2Stream(unsigned short year, unsigned short month, unsigned short day, ostream& os)
{
	switch (sDefaultDateOrder) {
	case orderYMD :
		os << year << "/" << month << "/" << day;
		break;
		
	case orderDMY : 
		os << day << "/" << month << "/" << year;
		break;
		
	case orderMDY : 
		os << month << "/" << day << "/" << year;
		break;
	}
	return os;
}




ostream&
dbDate::ymd2Stream(const char* mask, unsigned short year, unsigned short month, unsigned short day, ostream& os)
{
	if(checkDate(year,month,day)) {
		// parse the mask
		TOKEN maskbits[MAXTOKENS];	// temporary !!!
		parseMask(mask,maskbits,dbDate::tokenMap,dbDate::NUMDATETOKENS);
		
		char somechars[10];	// to take a possible month name
		
		for(int index=0;maskbits[index].token!=eEOT;index++) {
			switch (maskbits[index].token) {
			case eD :
				// put day number as string into stream
				os << day;
				break;
			case eDD :
				// put day number as string into stream, padded with zeros
				if (day < 10)
					os << '0';	// pad with zeros
				os << day;
				break;
			case eDDD : case eDAY :
				// put month name as TLA into stream
				ymd2dayname(year, month, day,somechars,true,true);
				os << somechars;
				break;
			case eDDDD :case eDAYNAME :
				// put month as string into stream
				ymd2dayname(year, month, day,somechars,false,true);
				os << somechars;
				break;
			case eDdd : case eDay :
				// put month name as TLA into stream
				ymd2dayname(year, month, day,somechars);
				os << somechars;
				break;
			case eDddd : case eDayname :
				// put month as string into stream
				ymd2dayname(year, month, day,somechars,false);
				os << somechars;
				break;
			case eddd : case eday :
				// put month name as TLA into stream
				ymd2dayname(year, month, day,somechars);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case edddd : case edayname :
				// put month as string into stream
				ymd2dayname(year, month, day,somechars,false);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case eM :
				// put month number as string into stream
				os << month;
				break;
			case eMM :
				// put month number as string into stream, padded with zeros
				if (month < 10)
					os << '0';	// pad with zeros
				os << month;
				break;
			case eMMM : case eMON :
				// put month name as TLA into stream
				month2str(month,somechars,true,true);
				os << somechars;
				break;
			case eMMMM : case eMONTH :
				// put month as string into stream
				month2str(month,somechars,false,true);
				os << somechars;
				break;
			case eMmm : case eMon :
				// put month name as TLA into stream
				month2str(month,somechars);
				os << somechars;
				break;
			case eMmmm : case eMonth :
				// put month as string into stream
				month2str(month,somechars,false);
				os << somechars;
				break;
			case emmm : case emon :
				// put month name as TLA into stream
				month2str(month,somechars);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case emmmm : case ezmonth :
				// put month as string into stream
				month2str(month,somechars,false);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case eYY :
				// put short form year as string into stream
				os << (year%100);
				break;
			case eYYYY : case eYEAR :
				// put year as string into stream
				os << year;
				break;
			case eDDTH :
				ddth(day,somechars,true);
				os << somechars;
				break;
			case ezddth :
				ddth(day,somechars);
				os << somechars;
				break;
			case eACT : case eDEA :
				// do nothing
				break;
			case eOTHER:
				os << maskbits[index].s;
				break;
			default: // as much to stop gcc complaining as any other reason
				RAISE_EXCEPTION(oofE_FieldError(NULL, NULL,
					"Unhandled token type in dbDate::ymd2Stream indicates OOFILE programmer error"));
			}
		}
	} else
		os << "INVALID DATE!";
	
	return os;
}


unsigned short dbDate::ymd2dow(unsigned short y, unsigned short m, unsigned short d)
{
// From article by Kim Larsen, DDJ April 1995
// Jan and Feb are treated as months 13 and 14 of previous year
	if (m<3) {
		m += 12;
		y --;
	}
	unsigned short ret = (d + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400) % 7;
	ret +=1;	// 1-based
	return ret;
}


void dbDate::ymd2dayname(unsigned short year, unsigned short month, unsigned short day, char * dayname, bool isshort, bool isupper)
{
	unsigned short dow = ymd2dow(year, month, day);
	switch (dow) {
	case 1:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"MON");
			else
				strcpy(dayname,"Mon");
		}
		else {
			if (isupper)
				strcpy(dayname,"MONDAY");
			else
				strcpy(dayname,"Monday");
		}
		break;
	case 2:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"TUE");
			else
				strcpy(dayname,"Tue");
		}
		else {
			if (isupper)
				strcpy(dayname,"TUESDAY");
			else
				strcpy(dayname,"Tuesday");
		}
		break;
	case 3:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"WED");
			else
				strcpy(dayname,"Wed");
		}
		else {
			if (isupper)
				strcpy(dayname,"WEDNESDAY");
			else
				strcpy(dayname,"Wednesday");
		}
		break;
	case 4:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"THU");
			else
				strcpy(dayname,"Thu");
		}
		else {
			if (isupper)
				strcpy(dayname,"THURSDAY");
			else
				strcpy(dayname,"Thursday");
		}
		break;
	case 5:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"FRI");
			else
				strcpy(dayname,"Fri");
		}
		else {
			if (isupper)
				strcpy(dayname,"FRIDAY");
			else
				strcpy(dayname,"Friday");
		}
		break;
	case 6:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"SAT");
			else
				strcpy(dayname,"Sat");
		}
		else {
			if (isupper)
				strcpy(dayname,"SATURDAY");
			else
				strcpy(dayname,"Saturday");
		}
		break;
	case 7:
		if (isshort) {
			if (isupper)
				strcpy(dayname,"SUN");
			else
				strcpy(dayname,"Sun");
		}
		else {
			if (isupper)
				strcpy(dayname,"SUNDAY");
			else
				strcpy(dayname,"Sunday");
		}
		break;
	}
}


void
dbDate::month2str(unsigned short monthnum, char * monthname, bool isshort, bool isupper)
{
	switch (monthnum) {
	case 1:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"JAN");
			else
				strcpy(monthname,"Jan");
		}
		else {
			if (isupper)
				strcpy(monthname,"JANUARY");
			else
				strcpy(monthname,"January");
		}
		break;
	case 2:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"FEB");
			else
				strcpy(monthname,"Feb");
		}
		else {
			if (isupper)
				strcpy(monthname,"FEBRUARY");
			else
				strcpy(monthname,"February");
		}
		break;
	case 3:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"MAR");
			else
				strcpy(monthname,"Mar");
		}
		else {
			if (isupper)
				strcpy(monthname,"MARCH");
			else
				strcpy(monthname,"March");
		}
		break;
	case 4:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"APR");
			else
				strcpy(monthname,"Apr");
		}
		else {
			if (isupper)
				strcpy(monthname,"APRIL");
			else
				strcpy(monthname,"April");
		}
		break;
	case 5:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"MAY");
			else
				strcpy(monthname,"May");
		}
		else {
			if (isupper)
				strcpy(monthname,"MAY");
			else
				strcpy(monthname,"May");
		}
		break;
	case 6:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"JUN");
			else
				strcpy(monthname,"Jun");
		}
		else {
			if (isupper)
				strcpy(monthname,"JUNE");
			else
				strcpy(monthname,"June");
		}
		break;
	case 7:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"JUL");
			else
				strcpy(monthname,"Jul");
		}
		else {
			if (isupper)
				strcpy(monthname,"JULY");
			else
				strcpy(monthname,"July");
		}
		break;
	case 8:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"AUG");
			else
				strcpy(monthname,"Aug");
		}
		else {
			if (isupper)
				strcpy(monthname,"AUGUST");
			else
				strcpy(monthname,"August");
		}
		break;
	case 9:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"SEP");
			else
				strcpy(monthname,"Sep");
		}
		else {
			if (isupper)
				strcpy(monthname,"SEPTEMBER");
			else
				strcpy(monthname,"September");
		}
		break;
	case 10:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"OCT");
			else
				strcpy(monthname,"Oct");
		}
		else {
			if (isupper)
				strcpy(monthname,"OCTOBER");
			else
				strcpy(monthname,"October");
		}
		break;
	case 11:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"NOV");
			else
				strcpy(monthname,"Nov");
		}
		else {
			if (isupper)
				strcpy(monthname,"NOVEMBER");
			else
				strcpy(monthname,"November");
		}
		break;
	case 12:
		if (isshort) {
			if (isupper)
				strcpy(monthname,"DEC");
			else
				strcpy(monthname,"Dec");
		}
		else {
			if (isupper)
				strcpy(monthname,"DECEMBER");
			else
				strcpy(monthname,"December");
		}
		break;
	}
}


void
dbDate::ddth(unsigned short daynum, char * outDD, bool isupper)
{
	stringstream oss;
	
	oss << daynum;
	
	switch (daynum) {
	case 1: case 21: case 31: 
			if(isupper)
				oss << "ST";
			else
				oss << "st";
			break;
			
	case 2: case 22:
			if(isupper)
				oss << "ND";
			else
				oss << "nd";
			break;
			
	case 3: case 23:
			if(isupper)
				oss << "RD";
			else
				oss << "rd";
			break;
			
	default:if(isupper)
				oss << "TH";
			else
				oss << "th";

			break;

		
	}
	
	#if OOF_USE_ANSI_STREAMS
		string s = oss.str();	// ANSI string
		const char* s2 = s.c_str();
		strcpy(outDD, s2);
	#else
		unsigned slen=oss.pcount();
		memcpy(outDD,oss.str(),slen);
		oss.rdbuf()->freeze(0);
		outDD[slen]='\0';
	#endif
	return;
}


void 
dbDate::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
//	const long startDate = ymd2Long(1963, 5, 7);
//	const long endDate = ymd2Long(2001,1,1);
//	const long dateDiff = endDate - startDate;
//	setDate(startDate);
//  operator+=(rand()%dateDiff);   // don't have this operator yet!

// pretty sick alternative but at least it generates legal dates
	if (generateRandom) {
		setDate(ymd2Long( (rand()%(2010-1963) + 1963),
		                  (rand()%12), (rand()%28) ));
	}
	else {
		const long startDate = ymd2Long(1963, 5, 7);
		setDate(seedOrCount+startDate);
	}
}


dbQueryTrinary	
dbDate::between(const char* fromStr, const char* toStr) const
{
	return dbNumericField::between(str2Long(fromStr), str2Long(toStr));
}


dbQueryTrinary	
dbDate::outside(const char* fromStr, const char* toStr) const
{
	return dbNumericField::outside(str2Long(fromStr), str2Long(toStr));
}


dbQueryClause*	
dbDate::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbDate::operator==(const char* str) const
{
	return dbNumericField::operator==(str2Long(str));
}    


dbQueryBinary	
dbDate::operator!=(const char* str) const
{
	return dbNumericField::operator!=(str2Long(str));
}  


dbQueryBinary	
dbDate::operator<(const char* str) const
{
	return dbNumericField::operator<(str2Long(str));
}  


dbQueryBinary	
dbDate::operator<=(const char* str) const
{
	return dbNumericField::operator<=(str2Long(str));
}  


dbQueryBinary	
dbDate::operator>(const char* str) const
{
	return dbNumericField::operator>(str2Long(str));
}  


dbQueryBinary	
dbDate::operator>=(const char* str) const
{
	return dbNumericField::operator>=(str2Long(str));
}  


dbQueryTrinary	
dbDate::between(const dbDate& fromDate, const dbDate& toDate) const
{
	return dbNumericField::between((long)fromDate, (long)toDate);
}


dbQueryTrinary	
dbDate::outside(const dbDate& fromDate, const dbDate& toDate) const
{
	return dbNumericField::outside((long)fromDate, (long)toDate);
}


dbQueryBinary	
dbDate::operator==(const dbDate& rhs) const
{
	return dbNumericField::operator==((long)rhs);
}    


dbQueryBinary	
dbDate::operator!=(const dbDate& rhs) const
{
	return dbNumericField::operator!=((long)rhs);
}  


dbQueryBinary	
dbDate::operator<(const dbDate& rhs) const
{
	return dbNumericField::operator<((long)rhs);
}  


dbQueryBinary	
dbDate::operator<=(const dbDate& rhs) const
{
	return dbNumericField::operator<=((long)rhs);
}  


dbQueryBinary	
dbDate::operator>(const dbDate& rhs) const
{
	return dbNumericField::operator>((long)rhs);
}  


dbQueryBinary	
dbDate::operator>=(const dbDate& rhs) const
{
	return dbNumericField::operator>=((long)rhs);
}  


dbDate& 
dbDate::operator=(const char* str)
{
	if (!str || ::strlen(str)==0)
		setDate(0);
	else {
		unsigned short day, month, year;
		if(str2ymd(str, year, month, day))
			setDate(year, month, day);
		else
			setDate(ULONG_MAX);
	}
	return *this;
}


dbDate& 
dbDate::operator=(const dbDate& rhs)
{
	if (this == &rhs)
		return *this;
	
	setDate((long)rhs);
	return *this;
}


void 
dbDate::setDate(unsigned long n)
{
	if (fieldIsStandalone()) 
		mBackend = (OOF_tableBackend*) n;
	else {
		unsigned long *writeTo = (unsigned long *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
}


void
dbDate::setDate(const tm& unixTime )
{
	const unsigned short day = unixTime.tm_mday;
	const unsigned short month = unixTime.tm_mon + 1;  // months since Jan
	const unsigned short year = unixTime.tm_year + 1900;

	setDate(year, month, day);
}



#ifdef _Macintosh
void 
dbDate::getStr255(Str255 s) const
{
	oofString str = copyString();
	str.getStr255(s);
}


void 
dbDate::setStr255(const Str255 s)
{
	if (s[0]==0) 
		setDate(0);
	else {
		size_t sLen = s[0];
		char* cs = new char[sLen+1];
		memcpy(cs, &s[1], sLen);
		cs[sLen] = '\0';
		*this = cs;
	}
}



// end of Mac-specific extra conversions
#endif


dbDate::operator unsigned long() const
{
	unsigned long ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {
		if (fieldIsStandalone()) 
			ret = (unsigned long) mBackend;
		else {
			const unsigned long *readFrom = (unsigned long *)mBackend->getFieldReadFrom(mFieldNumber);
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}


void
dbDate::setNumber(long n)
{
	setDaynum(n);
}


void 
dbDate::setString(const char* str)
{
	dbDate::operator=(str);
}


void 
dbDate::setDate(unsigned short year, unsigned short month, unsigned short day)
{
	unsigned long binDate = ymd2Long(year, month, day);
	setDate(binDate);
}


void 
dbDate::setDateToCurrentDate()
{
	unsigned short year, month, day;
	currentDate2ymd(year, month, day);
	setDate(year, month, day);
}


void 
dbDate::nextCenturyThreshold(unsigned short inYear)
{
	if (inYear < 100) // will accept short years but add century
		inYear += currentCentury();
	sNextCenturyThreshold = inYear;
	if (sPrevCenturyThreshold<sNextCenturyThreshold)
		sPrevCenturyThreshold = sNextCenturyThreshold+100;
}


void 
dbDate::prevCenturyThreshold(unsigned short inYear)
{
// defines upper bound in current century - 2 digit dates boosted
// beyond this should be put back to the previous century
	if (inYear < 100) // will accept short years but add century
		inYear += currentCentury();
	sPrevCenturyThreshold = inYear;
	assert (sPrevCenturyThreshold>sNextCenturyThreshold);
}


unsigned short
dbDate::adjustShortYear(unsigned short inYear)
{
// can set thresholds to 0 to disable them
	assert(sNextCenturyThreshold==0 || sNextCenturyThreshold>100);
	assert(sPrevCenturyThreshold==0 || sPrevCenturyThreshold>100);
	assert(inYear<100);
	inYear += currentCentury();
	if (sNextCenturyThreshold>0 && inYear<sNextCenturyThreshold)
		inYear += 100;  // boosts into next century, eg: 30 entered in 1999
	else if (sPrevCenturyThreshold>0 && inYear>sPrevCenturyThreshold)
		inYear -= 100;  // put back into prev century, eg: 92 entered in 2001
	return inYear;
}



void 
dbDate::getDate(unsigned short& year, unsigned short& month, unsigned short& day) const
{
	const unsigned long binDate = (long) *this;
	long2ymd(binDate, year, month, day);
}


unsigned long 
dbDate::ymd2Long(unsigned short year, unsigned short month, unsigned short day)
{
	if (checkDate(year, month, day)) {
		if ((year<100) && (month >0) && (day>0))	// not null date, eg 21/5/00
			year = adjustShortYear(year);
		return (( ( (( unsigned long)year << 8) + (unsigned long)month) << 8) + day);	// use discrete bytes for ease of reading
	}
	else {
		return ULONG_MAX;
	}
}


void 
dbDate::long2ymd(unsigned long binaryDate, unsigned short& year, unsigned short& month, unsigned short& day)
{
	day = binaryDate & 0xFF;
	unsigned long shifted = binaryDate >> 8;
	month = shifted & 0xFF;
	shifted = shifted >> 8;
	year = shifted;
}


long 
dbDate::operator-(const dbDate& rhs) const
{
	long ret = daynum() - rhs.daynum();
	return ret;
}



dbDate 
dbDate::operator-(long rhs) const
{
	dbDate ret = *this;
	ret -= rhs;
	return ret;
}





dbDate 
dbDate::operator+(long rhs) const
{
	dbDate ret = *this;
	ret += rhs;
	return ret;
}


dbDate
dbDate::currentDate()
{
	dbDate ret = dbDate(dbField::eFieldIsStandalone);  // g++ 2.7 disliked just ret(blah)
	ret.setDateToCurrentDate();
	return ret;
}


void 
dbDate::currentDate2ymd(unsigned short& year, unsigned short& month, unsigned short& day)
{
	const time_t numTime = time(0);
	if (numTime == -1) {
#ifdef OOF_DEBUG
		dbConnect::raise("System time not available");
#endif
		year = month = day = 0;
	}
	else {
		const tm* timeBits = localtime(&numTime);
		day = timeBits->tm_mday;
		month = timeBits->tm_mon+1;  // months since Jan
		year = timeBits->tm_year+1900;
	}
}


unsigned short 
dbDate::currentCentury()
{
	const time_t numTime = time(0);
	if (numTime == -1) {
#ifdef OOF_DEBUG
		dbConnect::raise("System time not available");
#endif
		return 1900;  // coarse default
	}
	else {
		const tm* timeBits = localtime(&numTime);
		return (timeBits->tm_year/100)*100+1900;
	}
}


bool 
dbDate::str2ymd(const char* str, unsigned short& year, unsigned short& month, unsigned short& day, dateFieldOrder theOrder)
{
	if (!str) {
		year = month = day = 0;
		return false;
	}
	istringstream s((char*)str);
	return istream2ymd(s, year, month, day, theOrder);
}


unsigned long 
dbDate::str2Long(const char* str, dateFieldOrder theOrder)
{
	if (!str)
		return 0;

	unsigned short year, month, day;
	istringstream s((char*)str);
	unsigned long ret;
	if (istream2ymd(s, year, month, day, theOrder))
		ret = ymd2Long(year, month, day);
	else
		ret = ULONG_MAX;
	return ret;
}


bool 
dbDate::checkDate(const char* str, dateFieldOrder theOrder)
{
	if (!str || strlen(str)==0)
		return true;	// early exit with trivial case - empty string
		
	unsigned short year, month, day;
	istringstream s((char*)str);
	istream2ymd(s, year, month, day, theOrder);
	const bool ret = checkDate(year, month, day);
	return ret;
}


bool 
dbDate::checkDate(unsigned short year, unsigned short month, unsigned short day)
{
	if (day==0)
		if ((year==0) && (month==0))
			return true;	// valid null date
		else
			return false;
		
		
	// check for valid year
	if ((year>9999)||(year>99)&&(year<=999))
		return false;	// note that 3 digit years are considered invalid

	bool ret;
	// check for valid month and year
	switch (month) {
	case 2:
		if (day > 28)
			ret = ((day==29) && isLeapYear(year));
		else
			ret = true;
		break;
				
	case 4: case 6: case 9: case 11:
		ret = (day <= 30);
		break;
			
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		ret = (day <= 31);
		break;
			
	default :
		ret = false;	// bad month
	}
	return ret;
}


bool
dbDate::isLeapYear(unsigned short year)
{
	bool isLeap = false;
	if (!(year%4)) {	// divisible by 4
		if (!(year%100))	// by 100
			isLeap = !(year%400);	// divisible by 400
		else
			isLeap = true;
	}
	return isLeap;
}


bool 
dbDate::istream2ymd(istream& is, unsigned short& year, unsigned short& month, unsigned short& day, char fieldSep, char recSep,
				dateFieldOrder theOrder)
{
	char ch = is.peek();
	if (ch==recSep) {
		year = month = day = 0;
		return false;  // early exit - empty line
	}
	if (ch==fieldSep) {
		year = month = day = 0;
		return true;  // early exit - empty field
	}
	
	if (is.good()) {
		is.flags(is.flags() & ~ios::skipws | ios::dec);  // we look at the white space!
		switch (theOrder) {
		case orderYMD :
			// skip non-digits
			skipTillDigit(is, fieldSep, recSep);
			// get year
			is >> year;
			// skip non-alphanumericals
			ch = is.peek();
			while (!isalnum(ch) && is.good()) {
				is >> ch;	// skip char
				ch = is.peek();
			}
			// get month
			month = istream2month(is);
			// skip non-digits
			skipTillDigit(is, fieldSep, recSep);
			// get day
			is >> day;
			break;
				
		case orderDMY :
			// skip non-digits
			skipTillDigit(is, fieldSep, recSep);
			// get day
			is >> day;
			// skip post numerical characters (st,nd,rd,th)
			skipStreamPattern(is,"st");
			skipStreamPattern(is,"nd");
			skipStreamPattern(is,"rd");
			skipStreamPattern(is,"th");
			// skip non-alphanumericals
			ch = is.peek();
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			// get month
			month = istream2month(is);
			// get next character
			ch = is.peek();
			if ((ch==fieldSep)||(ch==recSep)) {
				year = currentYear();
				break;
			}
			// skip non-alphanumericals
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			// get year
			is >> year;
			break;
				
		case orderMDY :
			// skip non-alphanumericals
			ch = is.peek();
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			// get month
			month = istream2month(is);
			// skip non-digits
			skipTillDigit(is, fieldSep, recSep);
			// get day
			is >> day;
			// skip post numerical characters (st,nd,rd,th)
			skipStreamPattern(is,"st");
			skipStreamPattern(is,"nd");
			skipStreamPattern(is,"rd");
			skipStreamPattern(is,"th");
			// get next character
			ch = is.peek();
			if ((ch==fieldSep)||(ch==recSep)) {
				year = currentYear();
				break;
			}
			// skip non-alphanumericals
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			// get year
			is >> year;
			break;
		}
		skipRestOfField(is, fieldSep, recSep);	// mainly for trailing times, eg: 0:00:00 from VB/Access
		if ((year<100) && (month >0) && (day>0))	// not null date, eg 21/5/00
			year = adjustShortYear(year);
		return checkDate(year, month, day);
	}
	else
		return false;
}


bool 
dbDate::istream2ymd(istream& is, unsigned short& year, unsigned short& month, unsigned short& day, dateFieldOrder theOrder)
{
	return istream2ymd(is, year, month, day, dbTable::kFieldSep, dbTable::kRecSep, theOrder);
}


unsigned short
dbDate::istream2month(istream& is)
{
	is.flags(is.flags() & ~ios::skipws | ios::dec);  // we look at the white space!
	char buf[4];
	char ch = is.peek();
	unsigned short monthNum=0;
	if (is.good()) {
		// is month as character or digit?
		if (isalpha(ch)) {
			is >> ch;
			buf[0] = ch;
			is.get(buf[1]);
			is.get(buf[2]);
			buf[3]='\0';
			monthNum = str2month(buf);
		}
		else {
			is >> monthNum;
		}
	} // stream good
	// return month as digit
	return monthNum;
}


unsigned short
dbDate::str2month (const char * s)
{
	char month[4];
	strncpy(month,s,3);
	month[3] = '\0';
	for (int i=0; month[i]; ++i)
		month[i]=toupper(month[i]);
	if (!strcmp(month,"JAN"))
		return 1;
	else if (!strcmp(month,"FEB"))
		return 2;
	else if (!strcmp(month,"MAR"))
		return 3;
	else if (!strcmp(month,"APR"))
		return 4;
	else if (!strcmp(month,"MAY"))
		return 5;
	else if (!strcmp(month,"JUN"))
		return 6;
	else if (!strcmp(month,"JUL"))
		return 7;
	else if (!strcmp(month,"AUG"))
		return 8;
	else if (!strcmp(month,"SEP"))
		return 9;
	else if (!strcmp(month,"OCT"))
		return 10;
	else if (!strcmp(month,"NOV"))
		return 11;
	else if (!strcmp(month,"DEC"))
		return 12;
	else
		return 0;	// error
}


unsigned long
dbDate::ymd2daynum( unsigned short y,unsigned short m,unsigned short d)
/*
Convert Gregorian calendar date to the corresponding Julian day number
j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
(Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
This function not valid before that.
*/
{
	unsigned long c, ya;
	if (m > 2)
		m -= 3;
	else {
		m += 9;
		y--;
	} /* else */
	c = y / 100;
	ya = y - 100*c;
	return ((146097*c)>>2) + ((1461*ya)>>2) + (153*m + 2)/5 + d + 1721119;
}


void
dbDate::daynum2ymd(unsigned long dn,unsigned short& y,unsigned short& m,unsigned short& d)
/*
Convert a Julian day number to its corresponding Gregorian calendar
date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
(Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
This function not valid before that.
*/
{
	unsigned long j = dn - 1721119;
	unsigned long dl, yl, ml;
	yl = ((j<<2) - 1) / 146097;
	j = (j<<2) - 1 - 146097*yl;
	dl = j>>2;
	j = ((dl<<2) + 3) / 1461;
	dl = (dl<<2) + 3 - 1461*j;
	dl = (dl + 4)>>2;
	ml = (5*dl - 3)/153;
	dl = 5*dl - 3 - 153*ml;
	dl = (dl + 5)/5;
	yl = 100*yl + j;
	if (ml < 10)
		ml += 3;
	else {
		ml -= 9;
		yl++;
	} /* else */
	assert( (dl <= USHRT_MAX) && (ml <= USHRT_MAX) && (yl <= USHRT_MAX));
	d = dl;
	m = ml;
	y = yl;
}


dbDate&
dbDate::operator++ ()
{
	// increment date
	setDaynum(daynum()+1);
	return *this;
}


dbDate&
dbDate::operator-- ()
{
	// decrement date
	setDaynum(daynum()-1);
	return *this;
}


dbDate&
dbDate::operator+= (long numDays)
{
	// add days to date
	setDaynum(daynum()+numDays);
	return *this;
}


dbDate&
dbDate::operator-= (long numDays)
{
	return (*this) += -numDays;
}


unsigned long
dbDate::daynum() const
{
	unsigned short day, month, year;
	getDate(year, month, day);
	return ymd2daynum(year, month, day);
}


void
dbDate::setDaynum(unsigned long dn)
{
	unsigned short day, month, year;
	daynum2ymd(dn, year, month, day);
	setDate(year,month,day);
}


unsigned short
dbDate::day() const
{
	return ((unsigned long) *this) & 0xFF;
}


unsigned short
dbDate::month() const
{
	return (((unsigned long) *this) >> 8) & 0xFF;
}


unsigned short
dbDate::year() const
{
	return ((unsigned long) *this) >> 16;
}


unsigned short
dbDate::currentDay()
{
	unsigned short y,m,d;
	currentDate2ymd(y,m,d);
	return d;
}


unsigned short
dbDate::currentMonth()
{
	unsigned short y,m,d;
	currentDate2ymd(y,m,d);
	return m;
}

unsigned short
dbDate::currentYear()
{
	unsigned short y,m,d;
	currentDate2ymd(y,m,d);
	return y;
}

// private helper for dbDate
void
skipStreamPattern(istream& is, const char * str)
{
	const unsigned short patternLen = strlen(str);
	for (unsigned short i=0; i<patternLen; i++) {
		char c = toupper(is.peek());
		char nextPat = toupper(str[i]);
		if (c != nextPat)
			return;
		is >> c;
	}
		
	return;
}


// -------------------------------------------------------
//                      d b T i m e
// -------------------------------------------------------

#ifdef __MWERKS__
#pragma mark ## DBTIME_CONSTRUCTORS
#endif

dbTime::dbTime(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbTime::dbTime(const char *fieldName)  :
	dbNumericField(fieldName)
{
	formatMask(sDefaultFormatMask);
}


dbTime::dbTime(const char *fieldName, const OOF_IndexOptions indexOptions)  :
	dbNumericField(fieldName, indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbTime::dbTime(dbField::FieldIsStandaloneT k) : 
	dbNumericField(k) 
{
	formatMask(sDefaultFormatMask);
}



dbTime::dbTime(const dbTime& rhs) : 
	dbNumericField(rhs)
{}


dbTime::~dbTime()
{}


dbField*
dbTime::clone() const 
{
	return new dbTime(*this);
}





#ifdef __MWERKS__
#pragma mark ## DBTIME_ARITHMETIC_OPERATORS
#endif

dbTime&
dbTime::operator++ ()
{
	setTime( value()+1000 );	// add 1 second
	return *this;
}

dbTime&
dbTime::operator-- ()
{
	setTime( value()-1000 );	// subtract 1 second
	return *this;
}

dbTime&
dbTime::operator+= (long numMilliseconds)
{
	setTime( value()+numMilliseconds );		// add milliseconds to time
	return *this;
}

dbTime&
dbTime::operator-= (long numMilliseconds)
{
	setTime( value()-numMilliseconds );		// subtract milliseconds to time
	return *this;
}

long 
dbTime::operator-(const dbTime& rhs) const		// return number in milliseconds
{
	long ret = value() - rhs.value();
	return ret;
}

dbTime 
dbTime::operator-(long rhs) const
{
	dbTime ret = *this;
	ret -= rhs;
	return ret;
}

dbTime 
dbTime::operator+(long rhs) const
{
	dbTime ret = *this;
	ret += rhs;
	return ret;
}

#ifdef __MWERKS__
#pragma mark ## DBTIME_SEARCH_FACTORIES
#endif

dbQueryTrinary	
dbTime::between(const char* fromStr, const char* toStr) const
{
	return dbNumericField::between(str2long(fromStr), str2long(toStr));
}


dbQueryTrinary	
dbTime::outside(const char* fromStr, const char* toStr) const
{
	return dbNumericField::outside(str2long(fromStr), str2long(toStr));
}


dbQueryClause*	
dbTime::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbTime::operator==(const char* str) const
{
	return dbNumericField::operator==(str2long(str));
}    


dbQueryBinary	
dbTime::operator!=(const char* str) const
{
	return dbNumericField::operator!=(str2long(str));
}  


dbQueryBinary	
dbTime::operator<(const char* str) const
{
	return dbNumericField::operator<(str2long(str));
}  


dbQueryBinary	
dbTime::operator<=(const char* str) const
{
	return dbNumericField::operator<=(str2long(str));
}  


dbQueryBinary	
dbTime::operator>(const char* str) const
{
	return dbNumericField::operator>(str2long(str));
}  


dbQueryBinary	
dbTime::operator>=(const char* str) const
{
	return dbNumericField::operator>=(str2long(str));
}

// -- search factories using dbTime operands ---------

dbQueryTrinary	
dbTime::between(const dbTime& fromDate, const dbTime& toDate) const
{
	return dbNumericField::between((long)fromDate, (long)toDate);
}


dbQueryTrinary	
dbTime::outside(const dbTime& fromDate, const dbTime& toDate) const
{
	return dbNumericField::outside((long)fromDate, (long)toDate);
}


dbQueryBinary	
dbTime::operator==(const dbTime& rhs) const
{
	return dbNumericField::operator==((long)rhs);
}    


dbQueryBinary	
dbTime::operator!=(const dbTime& rhs) const
{
	return dbNumericField::operator!=((long)rhs);
}  


dbQueryBinary	
dbTime::operator<(const dbTime& rhs) const
{
	return dbNumericField::operator<((long)rhs);
}  


dbQueryBinary	
dbTime::operator<=(const dbTime& rhs) const
{
	return dbNumericField::operator<=((long)rhs);
}  


dbQueryBinary	
dbTime::operator>(const dbTime& rhs) const
{
	return dbNumericField::operator>((long)rhs);
}  


dbQueryBinary	
dbTime::operator>=(const dbTime& rhs) const
{
	return dbNumericField::operator>=((long)rhs);
}

#ifdef __MWERKS__
#pragma mark ## DBTIME_REFECTIVE_OPERATORS
#endif

unsigned long 
dbTime::fieldStorageLen() const
{
	return sizeof(long); 
}

OOF_fieldTypes 
dbTime::fieldType() const
{
	return timeField; 
}

OOF_fieldTypes 
dbTime::nativeType() const
{
	return longField; 
}

#ifdef __MWERKS__
#pragma mark ## DBTIME_UTILITIES
#endif

dbTime&
dbTime::calculateWith(dbTimeCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbTime::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	setTime( calculator()->calc(this) );
}


bool
dbTime::checkTime(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond)
{
	if( (day>23) || (day<-23) )
		return false;

	// checking for < 0 superfluous for unsigned values
	if( hour>23 )
		return false;

	if( minute>59 )
		return false;

	if( second>59 )
		return false;

	if( millisecond>999 )
		return false;

	return true;
}

void 
dbTime::long2dhmsm(long binaryTime, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond)
{
	long	remainder = binaryTime;
	
	day = (short)( remainder / (1000*60*60*24) );
	remainder = remainder % (1000*60*60*24);

	hour = (unsigned short)( remainder / (1000*60*60) );
	remainder = remainder % (1000*60*60);

	minute = (unsigned short)( remainder / (1000*60) );
	remainder = remainder % (1000*60);

	second = (unsigned short)( remainder / 1000 );

	millisecond = remainder % 1000;
}

long 
dbTime::dhmsm2long(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond)
{
	long totalMilliseconds = 0;
	
	if( checkTime(day, hour, minute, second, millisecond) ) {
	
		totalMilliseconds = (long)millisecond;
		totalMilliseconds += 1000 * (long)second;
		totalMilliseconds += 1000 * 60 * (long)minute;
		totalMilliseconds += 1000 * 60 * 60 * (long)hour;
		totalMilliseconds += 1000 * 60 * 60 * 24 * (long)day;
	}
	else
		totalMilliseconds= ULONG_MAX;
	return totalMilliseconds;
}

bool
dbTime::istream2dhmsm(istream& is, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond)
{
	return istream2dhmsm(is, day, hour, minute, second, millisecond, dbTable::kFieldSep, dbTable::kRecSep);
}

bool 
dbTime::istream2dhmsm(istream& is, short& day, unsigned short& hour, unsigned short& minute,
					unsigned short& second, unsigned short& millisecond, char fieldSep, char recSep)
{
	hourT hourModifier = hour_24;		// assume 24 hour clock
	const char ch = is.peek();
	
	day = hour = minute = second = millisecond = 0;

	if( ch==recSep )
		return false;  // early exit - empty line

	if( ch==fieldSep )
		return true;  // early exit - empty field
	
	if( is.good() ) {
		is.flags( is.flags() & ~ios::skipws | ios::dec);	// we look at the white space
		
		if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
			is >> hour;
			
			if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
				is >> minute;
				
				if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> second;
					
					if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
						is >> millisecond;
					}
				}
			}
		}
		skipTillDigitAmPm(is,hourModifier, fieldSep, recSep);
		if( hourModifier==am_hour )
		{
			if( hour==12 )
				hour=0;
		}	
		
		if( hourModifier==pm_hour )
		{
			if( hour<12 )
				hour+=12;
		}
	} // good stream
	return checkTime(day, hour, minute, second, millisecond);
}

ostream& 
dbTime::hms2stream(unsigned short hour, unsigned short minute, unsigned short second, ostream& os)
{
	os << hour << ':';
	
	if( minute < 10 )
		os << '0';	// pad with zeros
	os << minute << ':';

	if( second < 10 )
		os << '0';	// pad with zeros
	os << second;

	return os;
}

ostream& 
dbTime::hms2stream(unsigned short hour, unsigned short minute, ostream& os)
{
	os << hour << ':';
	
	if( minute < 10 )
		os << '0';	// pad with zeros
	os << minute << ':';

	return os;
}

long 
dbTime::str2long(const char* str)
{
	if (!str)
		return 0;
		
	short day;
	unsigned short hour, minute, second, millisecond;
	
	istringstream s((char*)str);
	long ret;
	if (istream2dhmsm(s, day, hour, minute, second, millisecond))
		ret = dhmsm2long(day, hour, minute, second, millisecond);
	else
		ret = LONG_MAX;
	return ret;
}

bool 
dbTime::str2dhmsm(const char* str, short& day, unsigned short& hour, unsigned short& minute, unsigned short& second, unsigned short& millisecond)
{
	if (!str) {
		day = hour = minute = second = millisecond = 0;
		return false;
	}
	istringstream s((char*)str);
	return istream2dhmsm(s, day, hour, minute, second, millisecond);
}



void 
dbTime::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom)
		setTime( (rand()%24), (rand()%60), (rand()%60) );
	else
		setTime(seedOrCount);
}

#ifdef __MWERKS__
#pragma mark ## DBTIME_SETTERS
#endif

/**	
	Stores the value in the mBackend pointer for use in the lightweight
	subclass oofTime when the field storage isn't connected to a database.
*/ 
void 
dbTime::setTime(long n)
{
	if (fieldIsStandalone()) {
	
		mBackend = (OOF_tableBackend*) n;
	}
	else {
	
		long *writeTo = (long *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
}

void 
dbTime::setTime(short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short millisecond)
{
	setTime( dhmsm2long(day, hour, minute, second, millisecond) );
}

void 
dbTime::setTime(short day, unsigned short hour, unsigned short minute, unsigned short second)
{
	setTime( dhmsm2long(day, hour, minute, second, 0) );
}

void 
dbTime::setTime(unsigned short hour, unsigned short minute, unsigned short second)
{
	setTime( dhmsm2long(0, hour, minute, second, 0) );
}

void
dbTime::setTime(const tm& unixTime )
{
	const unsigned short second = unixTime.tm_sec;
	const unsigned short minute = unixTime.tm_min;
	const unsigned short hour = unixTime.tm_hour;
	
	setTime(hour, minute, second);
}

void 
dbTime::setTimeToCurrentTime()
{
	unsigned short hour, minute, second;
	
	currentTime2hms(hour, minute, second);
	setTime(hour, minute, second);
}

void 
dbTime::copyValueFrom(const dbField* srcField)
{
	if( srcField->fieldType()==timeField )
		setTime( ((dbTime*)srcField)->value() );

#ifdef OOF_DEBUG
	else
		dbConnect::raise(
			stringstream() << flush << "wrong field type passed in to dbTime::copyValueFrom, copying to " 
			<< fieldName() << " from " << srcField->fieldName()
		);
#endif
}

void 
dbTime::copyValueIfDifferent(const dbField* srcField)
{
	if( srcField->fieldType()==timeField ) {
		
		const long rhs = ((dbTime*)srcField)->value();
		if( value() != rhs )
			setTime( rhs );
	}
	
#ifdef OOF_DEBUG
	else
		dbConnect::raise(
			stringstream() << flush << "wrong field type passed in to dbTime::copyValueIfDifferent, copying to " 
			<< fieldName() << " from " << srcField->fieldName()
		);
#endif
}

dbTime& 
dbTime::operator=(const char* str)
{
	if (!str || ::strlen(str)==0)
		setTime(0);
	else {
		short day;
		unsigned short hour, minute, second, millisecond;
		str2dhmsm(str, day, hour, minute, second, millisecond);
		setTime(day, hour, minute, second, millisecond);
	}
	return *this;
}


dbTime& 
dbTime::operator=(const dbTime& rhs)
{
	if (this == &rhs)
		return *this;
	
	setTime( (long)rhs );
	return *this;
}


bool 
dbTime::insert(istream& is, char fieldSep, char recSep)
{
	if( !is.good() )	// is the stream good?
		return false;
		
	// read in the time
	short			day;
	unsigned short	hour, minute, second, millisecond;
	istream2dhmsm(is,day,hour,minute,second,millisecond,fieldSep,recSep);
	
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		setTime(day,hour,minute,second,millisecond);
	return hitEndOfField;
}


#ifdef __MWERKS__
#pragma mark ## DBTIME_GETTERS
#endif

dbTime::operator long() const
{
	long ret;
	if (usingCalculator())
		ret = calculator()->calc(this);
	else {
		if (fieldIsStandalone()) 
			ret = (long) mBackend;
		else {
			const long *readFrom = (long *)mBackend->getFieldReadFrom(fieldNumber());
			if (readFrom)
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}

void
dbTime::getTime( short& days, unsigned short& hours, unsigned short& minutes, unsigned short& seconds, unsigned short& milliseconds ) const
{
	long2dhmsm( getTime(), days, hours, minutes, seconds, milliseconds );
}

void
dbTime::getTime( short& days, unsigned short& hours, unsigned short& minutes, unsigned short& seconds ) const
{
	unsigned short milliseconds;
	
	long2dhmsm( getTime(), days, hours, minutes, seconds, milliseconds );
}

void
dbTime::getTime( unsigned short& hours, unsigned short& minutes, unsigned short& seconds) const
{
	short days;
	unsigned short milliseconds;
	
	long2dhmsm( getTime(), days, hours, minutes, seconds, milliseconds );
}

void
dbTime::getTime( tm& unixTime ) const
{
	short 			days;
	unsigned short 	hours, minutes, seconds, milliseconds;

	long2dhmsm( getTime(), days, hours, minutes, seconds, milliseconds );

	unixTime.tm_sec = seconds;
	unixTime.tm_min = minutes;
	unixTime.tm_hour = hours;
}

short
dbTime::day() const
{
	short 			day;
	unsigned short 	hour, minute, second;
	
	getTime( day, hour, minute, second );
	
	return day;
}

unsigned short
dbTime::hour() const
{
	unsigned short 	hour, minute, second;
	
	getTime( hour, minute, second );
	
	return hour;
}

unsigned short
dbTime::minute() const
{
	unsigned short 	hour, minute, second;
	
	getTime( hour, minute, second );
	
	return minute;
}

unsigned short
dbTime::second() const
{
	unsigned short 	hour, minute, second;
	
	getTime( hour, minute, second );
	
	return second;
}

unsigned short
dbTime::millisecond() const
{
	short 			day;
	unsigned short 	hour, minute, second, millisecond;
	
	getTime( day, hour, minute, second, millisecond );
	
	return millisecond;
}

void 
dbTime::extract(ostream& os) const
{
	const long binTime = (long)*this;
	
	if( binTime ) {
		short day;
		unsigned short hour, minute, second, millisecond;
		
		long2dhmsm(binTime, day, hour, minute, second, millisecond);
		
		//if( mFormatMask.length() )
		//	hms2stream(mFormatMask, year, month, day, os);
		//else
			hms2stream(hour, minute, second, os);
	}
}

#ifdef __MWERKS__
#pragma mark ## DBTIME_CURRENT_TIME
#endif

ostream& 
dbTime::now(ostream& os)
{
	unsigned short hour, minute, second;
	currentTime2hms(hour, minute, second);
	return hms2stream(hour, minute, second, os);
}

void 
dbTime::currentTime2hms(unsigned short& hour, unsigned short& minute, unsigned short& second)
{
	const time_t numTime = time(0);
	if (numTime == -1) {
#ifdef OOF_DEBUG
		dbConnect::raise("System time not available");
#endif
		hour = minute = second = 0;
	}
	else {
		const tm* timeBits = localtime(&numTime);
		second = timeBits->tm_sec;
		minute = timeBits->tm_min;
		hour = timeBits->tm_hour;
	}
}

dbTime
dbTime::currentTime()
{
	dbTime ret = dbTime(dbField::eFieldIsStandalone);  // g++ 2.7 disliked just ret(blah)
	ret.setTimeToCurrentTime();
	return ret;
}

unsigned short
dbTime::currentHour()
{
	unsigned short h,m,s;
	currentTime2hms(h,m,s);
	return h;
}

unsigned short
dbTime::currentMinute()
{
	unsigned short h,m,s;
	currentTime2hms(h,m,s);
	return m;
}

unsigned short
dbTime::currentSecond()
{
	unsigned short h,m,s;
	currentTime2hms(h,m,s);
	return s;
}

/* AD 980728
long
operator- (const dbTime& time1, const dbTime& time2)
{
	// return number of milliseconds
	return time1.value() - time2.value();
}

*/


// -------------------------------------------------------
//                   d b D a t e T i m e
// -------------------------------------------------------

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_CONSTRUCTORS
#endif

dbDateTime::dbDateTime(const OOF_IndexOptions indexOptions) :
	dbNumericField(indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbDateTime::dbDateTime(const char *fieldName) :
	dbNumericField(fieldName)
{
	formatMask(sDefaultFormatMask);
}


dbDateTime::dbDateTime(const char *fieldName, const OOF_IndexOptions indexOptions) :
	dbNumericField(fieldName, indexOptions)
{
	formatMask(sDefaultFormatMask);
}


dbDateTime::dbDateTime(dbField::FieldIsStandaloneT k) : 
	dbNumericField(k) 
{
	formatMask(sDefaultFormatMask);
}


dbDateTime::dbDateTime(const dbDateTime& rhs) : 
	dbNumericField(rhs)
{
	formatMask(sDefaultFormatMask);
}


dbDateTime::~dbDateTime()
{}


dbField*
dbDateTime::clone() const 
{
	return new dbDateTime(*this);
}





#ifdef __MWERKS__
#pragma mark ## DBDATETIME_ARITHMETIC_OPERATORS
#endif

dbDateTime&
dbDateTime::operator+= (long numDays)
{
	setDateTime( value() + numDays*kSecondsInDay );		
	return *this;
}


dbDateTime&
dbDateTime::operator++()
{
	*this += 1;
	return *this;
}


dbDateTime&
dbDateTime::operator--()
{
	*this -= 1;
	return *this;
}


dbDateTime&
dbDateTime::operator-= (long numDays)
{
	setDateTime( value() - numDays*kSecondsInDay );
	return *this;
}

unsigned long 
dbDateTime::operator+(const dbDateTime& rhs) const		// return number in seconds
{
	const unsigned long ret = value() + rhs.value();
	return ret;
}

unsigned long 
dbDateTime::operator-(const dbDateTime& rhs) const		// return number in seconds
{
	const unsigned long ret = value() - rhs.value();
	return ret;
}

dbDateTime 
dbDateTime::operator+(long numDays) const
{
	dbDateTime ret = *this;
	ret += numDays;
	return ret;
}

dbDateTime 
dbDateTime::operator-(long numDays) const
{
	dbDateTime ret = *this;
	ret -= numDays;
	return ret;
}

//unsigned long
//dbDateTime::operator+(const dbTime& rhs) const
//{
//	unsigned long ret = value() + rhs.value();
//	return ret;
//}

//unsigned long
//dbDateTime::operator-(const dbTime& rhs) const
//{
//	unsigned long ret = value() - rhs.value();
//	return ret;
//}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_SEARCH_FACTORIES
#endif

dbQueryTrinary	
dbDateTime::between(const char* fromStr, const char* toStr) const
{
	return dbNumericField::between(str2ulong(fromStr), str2ulong(toStr));
}

dbQueryTrinary	
dbDateTime::outside(const char* fromStr, const char* toStr) const
{
	return dbNumericField::outside(str2ulong(fromStr), str2ulong(toStr));
}


dbQueryClause*	
dbDateTime::valueAsQueryLiteral() const
{
	dbQueryClause* ret = makeQueryLiteral(value());
	return ret;
}


dbQueryBinary	
dbDateTime::operator==(const char* str) const
{
	return dbNumericField::operator==(str2ulong(str));
}    

dbQueryBinary	
dbDateTime::operator!=(const char* str) const
{
	return dbNumericField::operator!=(str2ulong(str));
}  

dbQueryBinary	
dbDateTime::operator<(const char* str) const
{
	return dbNumericField::operator<(str2ulong(str));
}  

dbQueryBinary	
dbDateTime::operator<=(const char* str) const
{
	return dbNumericField::operator<=(str2ulong(str));
}  

dbQueryBinary	
dbDateTime::operator>(const char* str) const
{
	return dbNumericField::operator>(str2ulong(str));
}  

dbQueryBinary	
dbDateTime::operator>=(const char* str) const
{
	return dbNumericField::operator>=(str2ulong(str));
}

// -- search factories using dbDateTime operands ---------

dbQueryTrinary	
dbDateTime::between(const dbDateTime& fromDate, const dbDateTime& toDate) const
{
	return dbNumericField::between(fromDate.value(), toDate.value());
}

dbQueryTrinary	
dbDateTime::outside(const dbDateTime& fromDate, const dbDateTime& toDate) const
{
	return dbNumericField::outside(fromDate.value(), toDate.value());
}

dbQueryBinary	
dbDateTime::operator==(const dbDateTime& rhs) const
{
	return dbNumericField::operator==(rhs.value());
}    

dbQueryBinary	
dbDateTime::operator!=(const dbDateTime& rhs) const
{
	return dbNumericField::operator!=(rhs.value());
}  

dbQueryBinary	
dbDateTime::operator<(const dbDateTime& rhs) const
{
	return dbNumericField::operator<(rhs.value());
}  

dbQueryBinary	
dbDateTime::operator<=(const dbDateTime& rhs) const
{
	return dbNumericField::operator<=(rhs.value());
}  

dbQueryBinary	
dbDateTime::operator>(const dbDateTime& rhs) const
{
	return dbNumericField::operator>(rhs.value());
}  

dbQueryBinary	
dbDateTime::operator>=(const dbDateTime& rhs) const
{
	return dbNumericField::operator>=(rhs.value());
}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_REFECTIVE_OPERATORS
#endif

unsigned long 
dbDateTime::fieldStorageLen() const
{
	return sizeof(unsigned long);
}

OOF_fieldTypes 
dbDateTime::fieldType() const
{
	return dateTimeField; 
}

OOF_fieldTypes 
dbDateTime::nativeType() const
{
	return uLongField; 
}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_UTILITIES
#endif

dbDateTime&
dbDateTime::calculateWith(dbDateTimeCalculator* adoptedCalculator, bool useOnlyForDefaults)
{
	CalculateWith(adoptedCalculator, useOnlyForDefaults);
	return *this;
}


void 
dbDateTime::CopyCalculatedValueToBuffer()
{
	assert(mCalculator);
	setDateTime( calculator()->calc(this) );
}


bool
dbDateTime::checkTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second)
{
	if (day==0)
		if ((year==0) && (month==0))
			return true;	// valid null date
		else
			return false;

	if( year < dbDateTime::sBaseYear )
		return false;
	
	if( (month > 12) || (month == 0) )
		return false;
	
	if( day > daysInMonth( year, month ))
		return false;
	
	if( hour>23 )
		return false;

	if( minute>59 )
		return false;

	if( second>59 )
		return false;

	return true;
}


bool 
dbDateTime::checkTime(const char* str)
{
	if (!str || strlen(str)==0)
		return true;	// early exit with trivial case - empty string
		
	unsigned short year, month, day, hour, minute, second;
	
	istringstream s((char*)str);
	istream2ymdhms(s, year, month, day, hour, minute, second, dbDate::sDefaultDateOrder);
	bool ret = checkTime(year, month, day, hour, minute, second);
	return ret;
}


void
dbDateTime::ulong2ymdhms( long binaryTime, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second )
{
	unsigned long remainder = binaryTime;
	
	if (binaryTime==0) {	// special case - null date
		year = month = day = hour = minute = second = 0;
	}
	else {
		// count off the number of years from dbDateTime::sBaseYear
		unsigned short	tempYear = dbDateTime::sBaseYear;
		
		while( secsInYear( tempYear ) <= remainder ) {
			remainder -= secsInYear( tempYear );
			tempYear++;
		}
		year = tempYear;
	
		// with the remainder figure out the month number (using the year number)
		unsigned short	tempMonth = 1;
		
		while( secsInMonth( year, tempMonth ) <= remainder ) {
			remainder -= secsInMonth( year, tempMonth );
			tempMonth++;
		}
		month = tempMonth;
		
		// then the day number
		unsigned short	tempDay = 1;
		
		while( kSecondsInDay <= remainder ) {
			remainder -= kSecondsInDay;
			tempDay++;
		}
		day = tempDay;
		
		// the hour number
		unsigned short	tempHour = 0;
		
		while( kSecondsInHour <= remainder ) {
			remainder -= kSecondsInHour;
			tempHour++;
		}
		hour = tempHour;
		
		// the minute number
		unsigned short	tempMinute = 0;
		
		while( kSecondsInMinute <= remainder ) {
			remainder -= kSecondsInMinute;
			tempMinute++;
		}
		minute = tempMinute;
		
		// the second number (whatever is remaining)
		second = remainder;
		
	#ifdef OOF_DEBUG
		if( checkTime(year,month,day,hour,minute,second) == false ) {
			dbConnect::raise(stringstream() << flush << "dbDateTime::ulong2ymdhms produced a bad time: long = " << binaryTime <<
			" YMDHMS = " << year << '\t' << month << '\t' << day << '\t' << hour << '\t' << minute << '\t' << second, false);
		}
	#endif	
	} // non-zero core number	
}

unsigned long 
dbDateTime::ymdhms2ulong(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second)
{
	unsigned long totalSeconds = 0;
	
	if( checkTime(year, month, day, hour, minute, second) ) {
		if (year) {
			assert(month && day && " If year is non-zero, so must month and day be.");
		totalSeconds = (unsigned long)second;
		totalSeconds += kSecondsInMinute * (unsigned long)minute;
		totalSeconds += kSecondsInHour * (unsigned long)hour;
		totalSeconds += kSecondsInDay * (unsigned long)(day-1);
		
		for(unsigned short tempMonth=(month-1); tempMonth>=1; tempMonth-- ) {
			totalSeconds += secsInMonth( year, tempMonth );
		}
		
		for(unsigned short tempYear=(year-1); tempYear>=dbDateTime::sBaseYear; tempYear-- ) {
			totalSeconds += secsInYear( tempYear );
		}
	}
	}
	else 
		totalSeconds = ULONG_MAX;
	
	return totalSeconds;
}

bool
dbDateTime::istream2ymdhms(istream& is, unsigned short& year, unsigned short& month, unsigned short& day,
		unsigned short& hour, unsigned short& minute, unsigned short& second, dbDate::dateFieldOrder theOrder)
{
	return istream2ymdhms(is, year, month, day, hour, minute, second, dbTable::kFieldSep, dbTable::kRecSep, theOrder);
}


bool 
dbDateTime::istream2ymdhms(istream& is, unsigned short& year, unsigned short& month,
	unsigned short& day, unsigned short& hour, unsigned short& minute,
	unsigned short& second, char fieldSep, char recSep,
	dbDate::dateFieldOrder theOrder)
{
	hourT hourModifier = hour_24;		// assume 24 hour clock
	char ch = is.peek();

	day = month = year = hour = minute = second = 0;

	if( ch==recSep )
		return false;  // early exit - empty line

	if( ch==fieldSep )
		return true;  // early exit - empty field
	
	if( is.good() ) {
		is.flags( is.flags() & ~ios::skipws | ios::dec );	// we look at the white space

		switch (theOrder) {
		case dbDate::orderYMD :
		
			skipTillDigit(is, fieldSep, recSep);		// skip non-digits
			if (is.good())
				is >> year;									// get year
			
			ch = is.peek();								// skip non-alphanumericals
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			month = dbDate::istream2month(is);					// get month
			
			skipTillDigit(is, fieldSep, recSep);		// skip non-digits
			if (is.good())
				is >> day;									// get day
			
			if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
				is >> hour;

				if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> minute;
			
					if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
						is >> second;
					}
				}
			}
			break;
				
		case dbDate::orderDMY :
			
			if (skipTillDigit(is, fieldSep, recSep))	// skip non-digits
				is >> day;								// get day
			
			skipStreamPattern(is,"st");				// skip post numerical characters (st,nd,rd,th)
			skipStreamPattern(is,"nd");
			skipStreamPattern(is,"rd");
			skipStreamPattern(is,"th");
			
			ch = is.peek();							// skip non-alphanumericals
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			month = dbDate::istream2month(is);				// get month
		
			ch = is.peek();							// skip non-digits, check for eof
			while (!isdigit(ch)) {
				is >> ch;
				if (!is.good()) {
					year = currentYear();
					break;
				}
				ch = is.peek();
			}
			if (is.good()) {
				is >> year;							// get year
			
				if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> hour;
	
					if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> minute;
				
						if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
						is >> second;
						}
					}
				}
			} // still good for year
			break;
			
				
		case dbDate::orderMDY :
			
			ch = is.peek();							// skip non-alphanumericals
			while (!isalnum(ch) && is.good()) {
				is >> ch;
				ch = is.peek();
			}
			
			month = dbDate::istream2month(is);				// get month
			
			if (skipTillDigit(is, fieldSep, recSep))	// skip non-digits
				is >> day;								// get day
				
			skipStreamPattern(is,"st");				// skip post numerical characters (st,nd,rd,th)
			skipStreamPattern(is,"nd");
			skipStreamPattern(is,"rd");
			skipStreamPattern(is,"th");

			ch = is.peek();							// skip non-digits, check for eof
			while (!isdigit(ch)) {
				is >> ch;
				if (!is.good()) {
					year = currentYear();
					break;
				}
				ch = is.peek();
			}
			if (is.good()) {
				is >> year;					// get year

				if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> hour;
	
					if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
					is >> minute;
				
						if( skipTillDigitAmPm(is,hourModifier, fieldSep, recSep) ) {
						is >> second;
						}
					}
				}
			} // still good for year
			break;	
		}
		
		skipRestOfField(is, fieldSep, recSep);
		
		if( (year<100) && (month>0) && (day>0) )	// not null date, eg 21/5/00
			year = dbDate::adjustShortYear(year);
			if (year<dbDateTime::sBaseYear)
				year += 100;  // if our base year in next century is only case that can trigger this 
		
		if( hourModifier==am_hour )
		{
			if( hour==12 )
				hour=0;
		}	

		if( hourModifier==pm_hour )
		{
			if( hour<12 )
				hour+=12;
		}

		return checkTime(year, month, day, hour, minute, second);
	}
	else
		return false;
}

ostream& 
dbDateTime::ymdhms2stream(
	unsigned short year, unsigned short month, unsigned short day, 
	unsigned short hour, unsigned short minute, unsigned short second, ostream& os
)
{
	switch (dbDate::sDefaultDateOrder) {
	case dbDate::orderYMD :			
		os << year << '/' << month << '/' << day << ' ';
		break;
		
	case dbDate::orderMDY :			
		os << month << '/' << day << '/' << year << ' ';
		break;

	case dbDate::orderDMY :			
		os << day << '/' << month << '/' << year << ' ';
		break;
	};		

	os << hour << ':';
	
	if( minute < 10 )
		os << '0';	// pad with zeros
	os << minute << ':';

	if( second < 10 )
		os << '0';	// pad with zeros
	os << second;

	return os;
}


ostream& 
dbDateTime::ymdhms2stream(
	const char* mask, unsigned short year, unsigned short month, unsigned short day, 
	unsigned short hour, unsigned short minute, unsigned short second, ostream& os
)
{
	if(checkTime(year,month,day,hour,minute,second)) {
		// parse the mask
		TOKEN maskbits[MAXTOKENS];	// temporary !!!
		parseMask(mask,maskbits,dbDateTime::tokenMap,dbDateTime::NUMDATETIMETOKENS);
		
		char somechars[10];	// to take a possible month name
		
		for(int index=0;maskbits[index].token!=eEOT;index++) {
			switch (maskbits[index].token) {
			
			case eHH:			// 24 hour clock
				os << hour;
				break;
			case ehh:			// 12 hour clock
				if( hour == 0 ) {
					os << "12";
				} else {
					if( hour > 12 ) {
						os << (hour-12);
					} else {
						os << hour;
					}
				}
				break;
			case eMIN:			// minutes
				if( minute < 10 )
					os << '0';	// pad with zeros
				os << minute;
				break;
			case eSEC:			// seconds
				if( second < 10 )
					os << '0';	// pad with zeros
				os << second;
				break;
			case ePM:			// hour indicator (all caps)
				if( hour < 12 ) {
					os << "AM";
				} else {
					os << "PM";
				}
				break;
			case ePm:			// hour indicator (part caps)
				if( hour < 12 ) {
					os << "Am";
				} else {
					os << "Pm";
				}
				break;
			case epm:			// hour indicator (no caps)
				if( hour < 12 ) {
					os << "am";
				} else {
					os << "pm";
				}
				break;
			case eD :
				// put day number as string into stream
				os << day;
				break;
			case eDD :
				// put day number as string into stream, padded with zeros
				if (day < 10)
					os << '0';	// pad with zeros
				os << day;
				break;
			case eDDD : case eDAY :
				// put month name as TLA into stream
				dbDate::ymd2dayname(year, month, day,somechars,true,true);
				os << somechars;
				break;
			case eDDDD :case eDAYNAME :
				// put month as string into stream
				dbDate::ymd2dayname(year, month, day,somechars,false,true);
				os << somechars;
				break;
			case eDdd : case eDay :
				// put month name as TLA into stream
				dbDate::ymd2dayname(year, month, day,somechars);
				os << somechars;
				break;
			case eDddd : case eDayname :
				// put month as string into stream
				dbDate::ymd2dayname(year, month, day,somechars,false);
				os << somechars;
				break;
			case eddd : case eday :
				// put month name as TLA into stream
				dbDate::ymd2dayname(year, month, day,somechars);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case edddd : case edayname :
				// put month as string into stream
				dbDate::ymd2dayname(year, month, day,somechars,false);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case eM :
				// put month number as string into stream
				os << month;
				break;
			case eMM :
				// put month number as string into stream, padded with zeros
				if (month < 10)
					os << '0';	// pad with zeros
				os << month;
				break;
			case eMMM : case eMON :
				// put month name as TLA into stream
				dbDate::month2str(month,somechars,true,true);
				os << somechars;
				break;
			case eMMMM : case eMONTH :
				// put month as string into stream
				dbDate::month2str(month,somechars,false,true);
				os << somechars;
				break;
			case eMmm : case eMon :
				// put month name as TLA into stream
				dbDate::month2str(month,somechars);
				os << somechars;
				break;
			case eMmmm : case eMonth :
				// put month as string into stream
				dbDate::month2str(month,somechars,false);
				os << somechars;
				break;
			case emmm : case emon :
				// put month name as TLA into stream
				dbDate::month2str(month,somechars);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case emmmm : case ezmonth :
				// put month as string into stream
				dbDate::month2str(month,somechars,false);
				somechars[0]=tolower(somechars[0]);
				os << somechars;
				break;
			case eYY :
				// put short form year as string into stream
				os << (year%100);
				break;
			case eYYYY : case eYEAR :
				// put year as string into stream
				os << year;
				break;
			case eDDTH :
				dbDate::ddth(day,somechars,true);
				os << somechars;
				break;
			case ezddth :
				dbDate::ddth(day,somechars);
				os << somechars;
				break;
			case eACT : case eDEA :
				// do nothing
				break;
			case eOTHER:
				os << maskbits[index].s;
				break;
			default: // as much to stop gcc complaining as any other reason
				RAISE_EXCEPTION(oofE_FieldError(NULL, NULL,
					"Unhandled token type in dbDateTime::ymd2Stream indicates OOFILE programmer error"));
			}
		}
	} else
		os << "INVALID DATE-TIME!";
	
	return os;
}



unsigned long 
dbDateTime::str2ulong(const char* str)
{
	if (!str)
		return 0;
		
	unsigned short year, month, day, hour, minute, second;
	
	istringstream s((char*)str);
	unsigned long ret;
	if (istream2ymdhms(s, year, month, day, hour, minute, second, dbDate::sDefaultDateOrder))
		ret = ymdhms2ulong(year, month, day, hour, minute, second);
	else
		ret = ULONG_MAX;
	return ret;
}

bool 
dbDateTime::str2ymdhms(const char* str, unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second)
{
	if (!str) {
		year = month = day = hour = minute = second = 0;
		return false;
	}
	istringstream s((char*)str);
	return istream2ymdhms(s, year, month, day, hour, minute, second, dbDate::sDefaultDateOrder);
}

unsigned long
dbDateTime::secsInYear(unsigned short year )
{
	if( dbDate::isLeapYear(year) ) {
		return( 366*kSecondsInDay );
	}
	else {
		return( 365*kSecondsInDay );
	}
}


unsigned long
dbDateTime::secsInMonth(unsigned short year, unsigned short month )
{
	const unsigned long returnSecs = daysInMonth( year, month ) * kSecondsInDay;	
	return returnSecs;
}


unsigned short
dbDateTime::daysInMonth(unsigned short year, unsigned short month )
{
	unsigned long	returnDays;
	
	switch( month ) {
		
		case 2:			// February
			if( dbDate::isLeapYear(year) ) {
				returnDays = 29;
			}
			else {
				returnDays = 28;
			}
			break;
		
		case 4:			// April
		case 6:			// June
		case 9:			// September
		case 11:		// November
			returnDays = 30;
			break;
		
		case 1:			// January
		case 3:			// March
		case 5:			// May
		case 7:			// July
		case 8:			// August
		case 10:		// October
		case 12:		// December
			returnDays = 31;
			break;
		
#ifdef OOF_DEBUG
		default:
			dbConnect::raise(stringstream() << flush << "Unrecognised month passed to dbDateTime::daysInMonth: month = " << month, false);
			break;
#endif

	}
	return( returnDays );
}

void 
dbDateTime::generateTestData(bool generateRandom, unsigned long seedOrCount)
{
	if (generateRandom) {
		setDateTime(((rand()%50)+dbDateTime::sBaseYear),	// year
				((rand()%11)+1),	// month
				((rand()%27)+1),	// day
				(rand()%24),		// hour
				(rand()%60),		// minute
				(rand()%60));		// second
	}
	else {
		const unsigned long startDate = ymdhms2ulong(1963, 5, 7);
		setDateTime(seedOrCount+startDate);
	}
}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_SETTERS
#endif


void 
dbDateTime::setNumber(long n)
{
	setDateTime(n);
}



void 
dbDateTime::setDateTime(unsigned long n)
{
	/*	Stores the value in the mBackend pointer for use in the lightweight
		subclass oofDateTime when the field storage isn't connected to a database.
	*/ 
	if( fieldIsStandalone() ) {

		mBackend = (OOF_tableBackend*) n;
	}
	else {
	
		unsigned long *writeTo = (unsigned long *)mBackend->getFieldWriteDest(this);
		if (writeTo)
			*writeTo = n;	
		mTable->markDirty();
	}
}

void 
dbDateTime::setDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second)
{
	setDateTime( ymdhms2ulong(year, month, day, hour, minute, second) );
}


void
dbDateTime::setDateTime(const tm& unixTime )
{
	const unsigned short second = unixTime.tm_sec;
	const unsigned short minute = unixTime.tm_min;
	const unsigned short hour = unixTime.tm_hour;
	const unsigned short day = unixTime.tm_mday;
	const unsigned short month = unixTime.tm_mon + 1;  // months since Jan
	const unsigned short year = unixTime.tm_year + 1900;

	setDateTime(year, month, day, hour, minute, second);
}


void 
dbDateTime::setTimeToCurrentTime()
{
	unsigned short year, month, day, hour, minute, second;
	
	currentTime2ymdhms(year, month, day, hour, minute, second);
	setDateTime(year, month, day, hour, minute, second);
}

void 
dbDateTime::copyValueFrom(const dbField* srcField)
{
	if( srcField->fieldType()==dateTimeField )
		setDateTime( ((dbDateTime*)srcField)->value() );

#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbDateTime::copyValueFrom, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}

void 
dbDateTime::copyValueIfDifferent(const dbField* srcField)
{
	if( srcField->fieldType()==dateTimeField ) {
		
		unsigned long rhs = ((dbDateTime*)srcField)->value();  // safe downcast due to above check
		if( value() != rhs )
			setDateTime( rhs );
	}
	
#ifdef OOF_DEBUG
	else
		dbConnect::raise(stringstream() << flush << "wrong field type passed in to dbDateTime::copyValueIfDifferent, copying to " << fieldName() << " from " << srcField->fieldName());
#endif
}


dbDateTime& 
dbDateTime::operator=(const char* str)
{
	
	if (!str || ::strlen(str)==0)
		setDateTime(0);
	else {
		unsigned short year, month, day, hour, minute, second;
		
		if (str2ymdhms(str, year, month, day, hour, minute, second))
			setDateTime(year, month, day, hour, minute, second);
		else
			setDateTime(ULONG_MAX);	// invalid
	}
	return *this;
}


dbDateTime& 
dbDateTime::operator=(const dbDateTime& rhs)
{
	if (this == &rhs)
		return *this;
	
	setDateTime( rhs.value() );
	return *this;
}


bool 
dbDateTime::insert(istream& is, char fieldSep, char recSep)
{
	if( !is.good() )	// is the stream good?
		return false;
		
	// read in the time
	unsigned short	year, month, day, hour, minute, second;
	istream2ymdhms(is,year,month,day,hour,minute,second,fieldSep,recSep, dbDate::sDefaultDateOrder);
	
	// read in a potential field/record separator
	const char c = is.peek();
	bool hitEndOfField = false;
	if ((c==fieldSep) || (c==recSep)) 
		hitEndOfField = true;
	if (hitEndOfField)
		setDateTime(year,month,day,hour,minute,second);
	return hitEndOfField;
}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_GETTERS
#endif


#ifdef _Macintosh
void 
dbDateTime::getStr255(Str255 s) const
{
	oofString str = copyString();
	str.getStr255(s);
}


void 
dbDateTime::setStr255(const Str255 s)
{
	if (s[0]==0) 
		setDateTime(0);
	else {
		const size_t sLen = s[0];
		char* cs = new char[sLen+1];
		memcpy(cs, &s[1], sLen);
		cs[sLen] = '\0';
		*this = cs;
	}
}



// end of Mac-specific extra conversions
#endif


void 
dbDateTime::setString(const char* str)
{
	dbDateTime::operator=(str);
}


dbDateTime::operator unsigned long() const
{
	unsigned long ret;
	
	if( usingCalculator() )
		ret = calculator()->calc(this);
	else {
		if( fieldIsStandalone() ) 
			ret = (unsigned long) mBackend;
		else {
			const unsigned long *readFrom = (unsigned long *)mBackend->getFieldReadFrom(fieldNumber());
			if( readFrom )
				ret = *readFrom;
			else
				ret = 0;
		}
	}
	return ret;
}

void
dbDateTime::getDateTime( unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second ) const
{
	ulong2ymdhms( getDateTime(), year, month, day, hour, minute, second );
}

void
dbDateTime::getDateTime( tm& unixTime ) const
{
	unsigned short	year, month, day, hour, minute, second;

	getDateTime(year, month, day, hour, minute, second );

	unixTime.tm_sec = second;
	unixTime.tm_min = minute;
	unixTime.tm_hour = hour;
	unixTime.tm_mday = day;
	unixTime.tm_mon = month - 1;  // months since Jan
	unixTime.tm_year = year - 1900;
}

unsigned short
dbDateTime::year() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return year;
}

unsigned short
dbDateTime::month() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return month;
}

unsigned short
dbDateTime::day() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return day;
}

unsigned short
dbDateTime::hour() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return hour;
}

unsigned short
dbDateTime::minute() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return minute;
}

unsigned short
dbDateTime::second() const
{
	unsigned short	year, month, day, hour, minute, second;
	getDateTime( year, month, day, hour, minute, second );
	return second;
}

void 
dbDateTime::extract(ostream& os) const
{
	const unsigned long binTime = (unsigned long)*this;
	
	if( binTime ) {
		unsigned short	year,month,day,hour,minute,second;

		ulong2ymdhms(binTime,year,month,day,hour,minute,second);
		
		if( mFormatMask.length() )
			ymdhms2stream(mFormatMask,year,month,day,hour,minute,second,os);
		else
			ymdhms2stream(year,month,day,hour,minute,second,os);
	}
}

#ifdef __MWERKS__
#pragma mark ## DBDATETIME_CURRENT_TIME
#endif

ostream& 
dbDateTime::now(ostream& os)
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return ymdhms2stream(year, month, day, hour, minute, second, os);
}

void 
dbDateTime::currentTime2ymdhms(unsigned short& year, unsigned short& month, unsigned short& day, unsigned short& hour, unsigned short& minute, unsigned short& second)
{
	const time_t numTime = time(0);
	if (numTime == -1) {
#ifdef OOF_DEBUG
		dbConnect::raise("System time not available");
#endif
		year = dbDateTime::sBaseYear;
		month = day = 1;
		hour = minute = second = 0;
	}
	else {
		const tm* timeBits = localtime(&numTime);
		second = timeBits->tm_sec;
		minute = timeBits->tm_min;
		hour = timeBits->tm_hour;
		day = timeBits->tm_mday;
		month = timeBits->tm_mon + 1;  // months since Jan

		assert( dbDateTime::sBaseYear < (unsigned int)(timeBits->tm_year + 1900) );
		year = timeBits->tm_year + 1900;
	}
}

dbDateTime
dbDateTime::currentTime()
{
	dbDateTime ret = dbDateTime(dbField::eFieldIsStandalone);  // g++ 2.7 disliked just ret(blah)
	ret.setTimeToCurrentTime();
	return ret;
}

unsigned short
dbDateTime::currentYear()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return year;
}

unsigned short
dbDateTime::currentMonth()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return month;
}

unsigned short
dbDateTime::currentDay()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return day;
}

unsigned short
dbDateTime::currentHour()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return hour;
}

unsigned short
dbDateTime::currentMinute()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return minute;
}

unsigned short
dbDateTime::currentSecond()
{
	unsigned short year, month, day, hour, minute, second;
	currentTime2ymdhms(year, month, day, hour, minute, second);
	return second;
}


#ifdef _Macintosh
void 
dbDateTime::setMacCompatibleBaseYear()
{
	sBaseYear = 1904;
	// using this base year means our internal representation will be the same
	// as the long values returned by Mac calls such as ::GetDateTime()
	// or in file modification dates
}
#endif


/* AD980728
unsigned long
operator- (const dbDateTime& time1, const dbDateTime& time2)
{
	// return number of seconds
	const unsigned long ret = time1.value() - time2.value();
	return ret;
}
*/



// -------------------------------------------------------
//                  o o f D a t e
// -------------------------------------------------------
oofDate::oofDate() : 
	dbDate(dbField::eFieldIsStandalone) 
{}


oofDate::oofDate(const char* initialDate) :
	dbDate(dbField::eFieldIsStandalone) 
{	
	dbDate::operator=(initialDate); 
}



oofDate::oofDate(const tm& unixTime ) :
	dbDate(dbField::eFieldIsStandalone) 
{
	setDate(unixTime);
}



oofDate::oofDate(const dbDate& rhs) : 
	dbDate(dbField::eFieldIsStandalone) 
{
	dbDate::operator=(rhs); 
}


oofDate::oofDate(const oofDate& rhs) : 
	dbDate(dbField::eFieldIsStandalone) 
{
	dbDate::operator=(rhs); 
}


dbField*
oofDate::clone() const 
{
	return new oofDate(*this);
}


#ifdef __MWERKS__
#pragma mark ## OOFTIME
#endif

// -------------------------------------------------------
//                  o o f T i m e
// -------------------------------------------------------
oofTime::oofTime() : 
	dbTime(dbField::eFieldIsStandalone) 
{}


oofTime::oofTime(const char* initialTime) :
	dbTime(dbField::eFieldIsStandalone) 
{	
	dbTime::operator=(initialTime); 
}


oofTime::oofTime(const dbTime& rhs) : 
	dbTime(dbField::eFieldIsStandalone) 
{
	dbTime::operator=(rhs); 
}


oofTime::oofTime(const oofTime& rhs) : 
	dbTime(dbField::eFieldIsStandalone) 
{
	dbTime::operator=(rhs); 
}


dbField*
oofTime::clone() const 
{
	return new oofTime(*this);
}

#ifdef __MWERKS__
#pragma mark ## OOFDATETIME
#endif

// -------------------------------------------------------
//                  o o f D a t e T i m e
// -------------------------------------------------------
oofDateTime::oofDateTime() : 
	dbDateTime(dbField::eFieldIsStandalone) 
{}


oofDateTime::oofDateTime(const char* initialTime) :
	dbDateTime(dbField::eFieldIsStandalone) 
{	
	dbDateTime::operator=(initialTime); 
}


oofDateTime::oofDateTime(const tm& unixTime ) :
	dbDateTime(dbField::eFieldIsStandalone) 
{
	setDateTime(unixTime);
}


oofDateTime::oofDateTime(const dbDateTime& rhs) : 
	dbDateTime(dbField::eFieldIsStandalone) 
{
	dbDateTime::operator=(rhs); 
}


oofDateTime::oofDateTime(const oofDateTime& rhs) : 
	dbDateTime(dbField::eFieldIsStandalone) 
{
	dbDateTime::operator=(rhs); 
}


dbField*
oofDateTime::clone() const 
{
	return new oofDateTime(*this);
}
