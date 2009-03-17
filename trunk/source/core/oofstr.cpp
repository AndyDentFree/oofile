// COPYRIGHT 1994-1998 A.D. Software, All rights reserved

// string classes
#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFSTR
	#include "oofstr.h"
#endif
#ifndef _CTYPE_H
	#include <ctype.h>
#endif

// Mac toolbox includes if not using precompiled header
#ifdef _Macintosh
	#ifndef __RESOURCES__
		#include <resources.h>  // for ReleaseResource
	#endif
	#ifndef __TEXTUTILS__
		#include <TextUtils.h>  // for GetString
	#endif
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//             s t a t i c   v a r s
// -------------------------------------------------------
char oofString::sEmptyChar = '\0';


// -------------------------------------------------------
//                        U T I L I T I E S
// -------------------------------------------------------
// OOF_stricmp - compare strings ignoring case of text

int 
OOF_stricmp (const char* s1, const char* s2)
{
	int i;
	for (i=0;s1[i]&&s2[i]&&(toupper(s1[i])==toupper(s2[i]));++i)
		;
	return (toupper(s1[i])-toupper(s2[i]));
}


int 
OOF_strnicmp (const char* s1, const char* s2, const unsigned int n)
{
	unsigned int i;
	for (i=0;s1[i]&&s2[i]&&(toupper(s1[i])==toupper(s2[i]))&&i<n;i++)
		;
	if(i==n) // We got past the number we were meant to compare
		return 0;
	else
		return (toupper(s1[i])-toupper(s2[i]));
}


// -------------------------------------------------------
//              O O F _ S t r i n g
// -------------------------------------------------------
oofString::oofString(const oofString& rhs)
{
	mLen = rhs.mLen;
	if (mLen) {
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		strcpy(mBody, rhs.mBody);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
	else
		mBody=0;
}


oofString::oofString(const oofString& str1, const char *chars, const oofString& str3)
{
	unsigned int charLen = 0;
	if (chars) 
		charLen = strlen(chars);
	mLen = str1.mLen + charLen + str3.mLen;
	if (mLen==0)
		mBody = 0;
	else {
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		if (str1.mBody)
			strcpy(mBody, str1.mBody);
		else
			mBody[0]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
		if (charLen)
			strcat(mBody, chars);
		if (str3.mBody)
			strcat(mBody, str3.mBody);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
}


oofString::oofString(char inChar, int repeatCount)
{
	mLen = repeatCount;
	mBody = new char[repeatCount+1];  // room for null
	assert(mBody);
	for (int i=0; i<repeatCount; i++)
		mBody[i]=inChar;
	mBody[repeatCount]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


oofString::oofString(const oofString& str1, const oofString& str2)
{
	mLen = str1.mLen + str2.mLen;
	if (mLen==0)
		mBody = 0;
	else {
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		assert((str1.mBody==0) ? (str1.mLen==0) : 1);
		if (str1.mBody) {
			memcpy(mBody, str1.mBody, str1.mLen);
		}
		assert((str2.mBody==0) ? (str2.mLen==0) : 1);
		if (str2.mBody) {
			memcpy(mBody+str1.mLen, str2.mBody, str2.mLen);
		}
		mBody[mLen]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
}


oofString::oofString(char inChar1, const oofString& str1, char inChar2)
{
	mLen = str1.mLen + 2;
	mBody = new char[mLen + 1];  // room for null
	assert(mBody);
	mBody[0]=inChar1;
	assert((str1.mBody==0) ? (str1.mLen==0) : 1);
	if (mLen>2) {
		memcpy(mBody+1, str1.mBody, str1.mLen);
	}
	mBody[mLen-1]=inChar2;
	mBody[mLen]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


oofString::oofString(const oofString& str1, const char *chars)
{
	unsigned int charLen = 0;
	if (chars) 
		charLen = strlen(chars);
	mLen = str1.mLen + charLen;
	if (mLen==0)
		mBody = 0;
	else {
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		if (str1.mBody)
			strcpy(mBody, str1.mBody);
		else
			mBody[0]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
		if (charLen)
			strcat(mBody, chars);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
}


oofString::oofString(const oofString& str1, const char ch, const char *chars)
{
	const unsigned int len1 = str1.mLen;
	unsigned int charLen = 0;
	if (chars) 
		charLen = strlen(chars);
	mLen = len1 + charLen + 1;

	mBody = new char[mLen + 1];  // room for null
	assert(mBody);
	if (str1.mBody)
		strcpy(mBody, str1.mBody);
	else
	mBody[len1]=ch;
	mBody[len1+1]='\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	if (charLen)
		strcat(mBody, chars);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


oofString::oofString(const char *chars) :
	mBody(0),
	mLen(0)
{
	if (chars)
		FinishConstruction(chars, strlen(chars));
}


oofString::oofString(const char *chars, unsigned long numChars) :
	mBody(0)
{
	FinishConstruction(chars, numChars);
}


oofString::oofString(char *chars, unsigned long numChars, bool adopt) :
	mBody(0)
{
	if (adopt) {  // dunno why anyone would invoke this variant with false, but allow
		mBody = chars;
		mLen = numChars;
		assert(strlen(chars)==numChars);
	}
	else
		FinishConstruction(chars, numChars);
}


#ifndef OOF_NO_STDLIB
	/**
		ctor taking an output stream.
		\note you will call this constructor even from an expression like
		oofString jack(ostringstream() << flush << "first line\n" << "second\n");
		because it is the type of the return value from the operator<< which is last
		evaluated, NOT the type of temporary stream object you start with.
	*/
	oofString::oofString(ostream& os) :
		mBody(0),
		mLen(0)
	{

	#if OOF_USE_ANSI_STREAMS
		ostringstream oss;
		oss << os.rdbuf();
		string s = oss.str();	// copy into ANSI string
		FinishConstruction(s.c_str(), s.size());
	#else
		ostringstream oss;
		#ifdef __MWERKS__
			oss << *(os.rdbuf());	// old Metrowerks lacked an operator<<(streambuf*)
		#else
			oss << os.rdbuf();
		#endif
		oss << ends;	// not null terminated until we add one
		FinishConstruction(oss.str(), oss.pcount()-1);	// pcount includes the null byte!
		oss.rdbuf()->freeze(0);  // leave buffer for stream to delete
	 #endif
	}
	
	
	#if OOF_USE_ANSI_STREAMS
	oofString::oofString(stringstream& os) :	// ostringstream is typedef'd as ostrstream if using old libs
		mBody(0),
		mLen(0)
	{
		string s = os.str();	// copy into ANSI string
		FinishConstruction(s.c_str(), s.size());
	}
	#endif
	
	
	#if OOF_HAS_ANSI_STRINGS
	oofString::oofString(const string& s) :
		mBody(0),
		mLen(0)
	{
		FinishConstruction(s.c_str(), s.size());
	}
	#endif
#endif // OOF_NO_STDLIB

#if defined(_Macintosh)
oofString::oofString(const Str255 str) : // _Macintosh
	mBody(0),
	mLen(0)
{
	FinishConstruction((const char*)&(str[1]), str[0]);
}
#endif


oofString::~oofString()
{
	delete [] mBody;
}


void
oofString::FinishConstruction(const char *chars, unsigned long numChars)
{
	if (chars) {
		mLen = numChars;
		if (numChars) {
			char* newBody = new char[numChars + 1];  // room for null
			assert(newBody);
			memcpy(newBody, chars, numChars); 
			newBody[numChars] = '\0';
			mBody = newBody;
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
		}
		else
			mBody = 0;
	}  // if pointer to chars not null
	else {
		mBody = 0;  // guarantee regardless of calling ctor
		mLen = 0;
	}
}


#if defined(_Macintosh)
void 
oofString::getStr255(Str255 s) const // _Macintosh
{
	s[0] = mLen;
	if (mLen)
		memcpy(s+1, mBody, mLen);
}

 
void 
oofString::setStr255(const Str255 s)
{
	const long charLen = s[0];
	const char* strStart = (char*) s+1;
	setChars(strStart, charLen);
}

#endif
// _Macintosh

#if OOF_HAS_ANSI_STRINGS
oofString& 
oofString::operator=(const string& s)
{
	setChars(s.c_str(), s.size());
	return *this;
}


oofString::operator string() const
{
	return string(mBody, mLen);
}
#endif
// OOF_HAS_ANSI_STRINGS




oofString& 
oofString::operator=(const oofString& rhs)
{
	if (*this==rhs)
		return *this;
		
	delete[] mBody;
	mLen = rhs.mLen;
	if (mLen==0)
		mBody = 0;
	else {
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		strcpy(mBody, rhs.mBody);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
	return *this;
}


bool 
oofString::operator==(const oofString& rhs) const
{
	if (this==&rhs)
		return true;	// trivial - same object
		
	if (mLen!=rhs.mLen)
		return false;	// different lengths
		
	for (unsigned long i=0; i<mLen; i++) {
		if (mBody[i]!=rhs.mBody[i])
			return false;
	}		
	return true;			
}


#ifdef _Macintosh
bool 
oofString::operator==(const Str255 s) const  //  _Macintosh
{
	if (s==0 || s[0]==0) 
		return (mBody==0 || mLen==0);
		
	const unsigned long rhsLen = s[0];
	if (mLen!=rhsLen)
		return false;	// different lengths
		
	for (unsigned long i=0; i<rhsLen; i++) {
		if (mBody[i]!=s[i+1])
			return false;
	}		
	return true;			
}


void
oofString::loadResource(unsigned long strID)
{
	StringHandle inStr = ::GetString(strID);
	if (inStr) {
		::HLock((Handle)inStr);
		setStr255(*((const Str255*)inStr));  // ugly cast for Carbon/CodeWarrior Pro6
		::ReleaseResource((Handle)inStr);
	}
	else
		setChars(0,0);
}
#elif defined _Windows
void
oofString::loadResource(unsigned long strID)
{
/*
NOT YET IMPLEMENTED
AD 990426 - don't want to use implementation just yet that has oofString requiring CStrings
as that introduces an MFC dependency we are avoiding.

	CString s;
	if (s.LoadString(strID)) {		
		const char* loadedString = s;
		*this = loadedString;
	}
	else
		setChars(0,0);
*/
}
#else
void
oofString::loadResource(unsigned long /*strID*/)
{
	assert(!"oofString::loadResource only implemented on Mac and Windows");
}
#endif


bool 
oofString::operator==(const char* chars) const
{
	if (chars==0) 
		return (mBody==0 || mLen==0);
		
	const unsigned long rhsLen = strlen(chars);
	if (mLen!=rhsLen)
		return false;	// different lengths
		
	for (unsigned long i=0; i<rhsLen; i++) {
		if (mBody[i]!=chars[i])
			return false;
	}		
	return true;			
}

oofString& 
oofString::operator=(const char* chars)
{
	delete[] mBody;
	if (chars) {
		mLen = strlen(chars);
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		strcpy(mBody, chars); 
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
	else {
		mLen = 0;
		mBody = 0;
	}
	return *this;
}


void 
oofString::setChars(const char* chars, unsigned long charLen)
{
// NOT YET IMPLEMENTED - avoid reallocs if mBody within certain size range 
// that will fit chars - would require keeping alloc length as well as mLen
	delete[] mBody;
	if (chars && charLen) {
		mLen = charLen;
		mBody = new char[mLen + 1];  // room for null
		assert(mBody);
		memcpy(mBody, chars, charLen); 
		mBody[charLen] = '\0';
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
	else {
		mLen = 0;
		mBody = 0;
	}
	return;
}


void 
oofString::insertChars(const char* chars, unsigned long charLen, unsigned long beforeChar)
{
	if (!chars || charLen==0)
		return;
		
	unsigned long totalLen = mLen + charLen;
	char* oldStore = mBody;
	mBody = new char[totalLen+1];
	char* insertAt;
	if (beforeChar>=mLen) {  // appending
		memcpy(mBody, oldStore, mLen);
		insertAt = mBody+mLen;
	}
	else if (beforeChar<2) {  // at front of string
		insertAt = mBody;
		memcpy(insertAt+charLen, oldStore, mLen);  // entire old body after the inserted string
	}
	else {
		insertAt = mBody+beforeChar;
		memcpy(mBody, oldStore, beforeChar);  // bit before the inserted string
		memcpy(insertAt+charLen, oldStore+beforeChar, mLen-beforeChar);  // after the inserted string
	}

	delete[] oldStore;			
	memcpy(insertAt, chars, charLen);
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	mBody[totalLen]='\0';
	mLen = totalLen;
}


void 
oofString::padTo(unsigned short paddedLength, char padWith, bool rightAlign)
{
	if (paddedLength <= mLen)
		return;
		
	const size_t charsToFill = paddedLength - mLen;
	char* oldStore = mBody;
	mBody = new char[paddedLength+1];
	char* insertAt;
	if (rightAlign) {
		memcpy(&mBody[charsToFill], oldStore, mLen);
		insertAt = mBody;
	}
	else{
		memcpy(mBody, oldStore, mLen);
		insertAt = mBody+mLen;
	}
	delete[] oldStore;			
	memset(insertAt, padWith, charsToFill);
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	mBody[paddedLength]='\0';
	mLen = paddedLength;	
}


void 
oofString::getChars(char* ioBuff, unsigned long& ioLen) const
{
	unsigned long outLen = mLen;
	if (ioLen < outLen)
		outLen = ioLen;
	else
		ioLen = outLen;
	memcpy(ioBuff, mBody, outLen);
}


oofString::operator const char*() const
{		
	if (mBody)
		return mBody;
	else
		return "";
}


char&
oofString::operator[](unsigned long index) const
{
	if ((index >= mLen) || !mBody)		
		return sEmptyChar;
	else
		return mBody[index];
}


void 
oofString::operator+=(char c)
{
	unsigned long totalLen = mLen + 1;
	if (mLen && mBody) {
		char* oldStore = mBody;
		mBody = new char[totalLen+1];
		memcpy(mBody, oldStore, mLen);
		delete[] oldStore;	
	}
	else {		
		delete[] mBody;		// shortening could reduce mLen but still have mBody
		mBody = new char[totalLen+1];
	}
	mBody[mLen] = c;
	mBody[totalLen]='\0';
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	mLen = totalLen;
}


void 
oofString::operator+=(const char* str)
{
	const unsigned long appendLen = strlen(str);
	if (appendLen>0)
		AppendSome(str, appendLen);
}


void 
oofString::operator+=(const oofString& src)
{
	const unsigned long appendLen = src.length();
	if (appendLen>0)
		AppendSome(src.chars(), appendLen);
}


void 
oofString::append(const char* str, unsigned long appendLen)
{
	if (appendLen>0)
		AppendSome(str, appendLen);
}


void 
oofString::AppendSome(const char* str, unsigned long appendLen)
{
	assert(appendLen>0);
	const unsigned long totalLen = mLen + appendLen;
	if (mLen && mBody) {
		char* oldStore = mBody;
		mBody = new char[totalLen+1];
		memcpy(mBody, oldStore, mLen);
		delete[] oldStore;		
	}	
	else {		
		delete[] mBody;		// shortening could reduce mLen but still have mBody
		mBody = new char[totalLen+1];
	}
	memcpy(mBody+mLen, str, appendLen);
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	mBody[totalLen]='\0';
	mLen = totalLen;
}


void 
oofString::convertNumber(int n, const char* printfMask, bool appendIt)
{
	char buf[80];
	if (printfMask)
		sprintf(buf, printfMask, n);
	else
		sprintf(buf, "%i", n);
	if (appendIt)
		*this += buf;
	else
		*this = buf;
}


void 
oofString::convertNumber(unsigned long n, const char* printfMask, bool appendIt)
{
	char buf[80];
	if (printfMask)
		sprintf(buf, printfMask, n);
	else
		sprintf(buf, "%lu", n);
	if (appendIt)
		*this += buf;
	else
		*this = buf;
}


void 
oofString::convertNumber(double n, const char* printfMask, bool appendIt)
{
	char buf[80];
	if (printfMask)
		sprintf(buf, printfMask, n);
	else
		sprintf(buf, "%g", n);
	if (appendIt)
		*this += buf;
	else
		*this = buf;
}


unsigned long 
oofString::replaceChar(char fromChar, char toChar, unsigned long startAt)
{
	unsigned long numReplaced = 0;
	const unsigned long kLen = mLen;
	for (unsigned long i=startAt; i<kLen; i++) {
		if (mBody[i]==fromChar) {
			mBody[i] = toChar;
			numReplaced++;
		}
	}
	return numReplaced;
}


oofString 
oofString::subString(unsigned long fromChars, unsigned long numChars) const
{
	if (!mBody || fromChars >= mLen)
		return oofString();  // return empty string if original empty, or start past its end
		
	assert(mLen > fromChars);  // handle other case above, returning empty
	const unsigned long lenAvailToCopy = mLen - fromChars;
	if (numChars > lenAvailToCopy)  // default is copy all remaining
		numChars = lenAvailToCopy;
		
	const char* copyFrom = &mBody[fromChars];
	return oofString(copyFrom, numChars);  // return string of <= length requested	
}


oofString 
oofString::subStringBetween(const char* leftStr, const char* rightStr) const
{
	const long leftAt = findEnd(leftStr);	
	if (leftAt!=kNotFound) {
		const long rightAt = find(rightStr, leftAt  /* startFrom */);	
		if (rightAt!=kNotFound) {
			const unsigned long subLen = rightAt-leftAt;
			return subString(leftAt, subLen);
		}
	}
	return oofString();
}


oofString 
oofString::operator-(const char* inTail) const
{
	const unsigned int inLen = strlen(inTail);
	if (endsWith(inTail, inLen)) {
		return subString(0, mLen-inLen);
	}
	else
		return oofString();
}


oofString 
oofString::subStringBetween(char leftChar, char rightChar) const
{
	const long leftStartsAt = find(leftChar);	
	if (leftStartsAt!=kNotFound) {
		const long leftAt = leftStartsAt+1;  // skip the actual char, like using findEnd in substringBetween(const char*...
		const long rightAt = find(rightChar, leftAt  /* startFrom */);	
		if (rightAt!=kNotFound) {
			const unsigned long subLen = rightAt-leftAt;
			return subString(leftAt, subLen);
		}
	}
	return oofString();
}


long 
oofString::find(char inChar, unsigned long startFrom, bool caseSensitive) const
{
	unsigned long foundAt = kNotFound;
	const unsigned long kLen = mLen;
	if (isalpha(inChar)) {
		if (!caseSensitive)
			inChar = toupper(inChar);
	}
	else
		caseSensitive = false;  // only worth hassle if alpha argument!
	
	if (caseSensitive) {
		for (unsigned long i=startFrom; i<kLen; i++) {
			if (mBody[i]==inChar) {  // guarantees is not null
				foundAt = i;
				break;	
			} // found match
		} // find loop - comparing strings
	}
	else {
		for (unsigned long i=startFrom; i<kLen; i++) {
			if (toupper(mBody[i])==inChar) {  // guarantees is not null
				foundAt = i;
				break;	
			} // found match
		} // find loop - comparing strings
	}
	return foundAt;
}


long 
oofString::findLast(char inChar, bool caseSensitive) const
{
	if (mLen==0)
		return kNotFound;
		
	unsigned long foundAt = kNotFound;
	if (caseSensitive) {
		if (isalpha(inChar))
			inChar = toupper(inChar);
		else
			caseSensitive = false;  // only worth hassle if alpha argument!
	}
	
	if (caseSensitive) {
		for (long i=mLen-1; i>=0; i--) {
			if (toupper(mBody[i])==inChar) {  // guarantees is not null
				foundAt = i;
				break;	
			} // found match
		} // find loop - comparing strings
	}
	else {
		for (long i=mLen-1; i>=0; i--) {
			if (mBody[i]==inChar) {  // guarantees is not null
				foundAt = i;
				break;	
			} // found match
		} // find loop - comparing strings
	}
	return foundAt;
}


long 
oofString::find(const char* inStr, unsigned long startFrom, bool caseSensitive, unsigned long inLen) const
{
// NOT YET IMPLEMENTED - Boyer-Moore search for longer strings
	unsigned long foundAt = kNotFound;
	const unsigned long kLen = mLen;
	const unsigned long schLen = (inLen==ULONG_MAX) ? strlen(inStr) : inLen;
	if (schLen==0)
		return 0;  // empty input string - matches beginning
	
	if (schLen==1)
		return find(inStr[0], startFrom, caseSensitive);  // use optimal single char search

	if (caseSensitive) {
		for (unsigned long i=0; i<schLen; i++) {
			if (!isalpha(inStr[i])) {
				caseSensitive = false;  // only worth hassle if wholly alpha argument!
				break;
			}
		}  // loop sch argument
	}
		
	if (caseSensitive) {  // I know this repeats a lot of logic but that's efficient!
		const char* schStr = inStr;
		const char firstChar = schStr[0];			
		for (unsigned long i=startFrom; i<kLen; i++) {
			if (mBody[i]==firstChar) {  // guarantees is not null
				if ((i+schLen) > kLen) {  // hit end of source, partway through matching
					foundAt = kNotFound;
					break;		// comparison failed
				}
				const char* startMatch = &mBody[i];
				foundAt = i;  // anticipate rest of string matches
				// note start at 1 not 0 so offset both search and origin - compared [0] above
				for (unsigned long k=1; startMatch[k] && schStr[k]; k++) {
					if (schStr[k] != startMatch[k]) {
						foundAt = kNotFound;
						break;		// comparison failed
					}
				} // loop comparing chars in a string
				if (foundAt != kNotFound)  // successful match of entire search str
					break;  // out of main loop matching first char to source
			} // if first char matches
		} // find loop - comparing strings
	}
	else {  // INSENSITIVE
		const unsigned long localBufLen = 20;  // should be able to use stack buf for most search args
		char localBuf[localBufLen];
		char* schStr;
		if (schLen > localBufLen) 
			schStr = new char[schLen+1];
		else
			schStr = localBuf;  // for small search arg, avoid hitting the heap!
			
		for (unsigned long iSch=0; iSch<schLen; iSch++)   // optimise by pre-creating uppercased sch string
			schStr[iSch] = toupper(inStr[iSch]);
		schStr[schLen] = '\0';
		
		const char firstChar = schStr[0];			
		for (unsigned long i=startFrom; i<kLen; i++) {
			if (toupper(mBody[i])==firstChar) {  // guarantees is not null
				if ((i+schLen) > kLen) {  // hit end of source, partway through matching
					foundAt = kNotFound;
					break;		// comparison failed
				}
				const char* startMatch = &mBody[i];
				foundAt = i;  // anticipate rest of string matches
				// note start at 1 not 0 so offset both search and origin - compared [0] above
				for (unsigned long k=1; startMatch[k] && schStr[k]; k++) {
					if (schStr[k] != toupper(startMatch[k])) {  //** insensitive test
						foundAt = kNotFound;
						break;		// comparison failed
					}
				} // loop comparing chars in a string
				if (foundAt != kNotFound)  // successful match of entire search str
					break;  // out of main loop matching first char to source
			} // if first char matches
		} // find loop - comparing strings
		
		if (schStr != localBuf)
			delete[] schStr;  // go out of our way to avoid copy as that's a slow heap hit!
	}  // case sensitive or not
	return foundAt;
}


long 
oofString::findEnd(const char* schStr, unsigned long startFrom, bool caseSensitive) const
{
	long foundAt = find(schStr, startFrom, caseSensitive);
	if (foundAt!=kNotFound)
		foundAt+=strlen(schStr);
	return foundAt;
}


void 
oofString::replaceAt(unsigned long replaceAtOffset, unsigned long replaceLen, const char* newStr, unsigned long newLen)
{
	if (!newStr)
		return;
		
	const unsigned long insertLen = (newLen==ULONG_MAX) ? strlen(newStr) : newLen;
		
	if (replaceLen==insertLen) {   // lucky trivial case - it just fits
		memcpy(&mBody[replaceAtOffset], newStr, replaceLen);
	}
	else if (replaceLen>insertLen) {  // copy over then move end up, wasting some space
		memcpy(&mBody[replaceAtOffset], newStr, insertLen);
		const unsigned long startTrailer = replaceAtOffset+replaceLen; 
		if (startTrailer<mLen) { // might have found string at end so no trailer
			const unsigned long trailingLen = mLen - startTrailer;
			memmove(&mBody[replaceAtOffset+insertLen], &mBody[startTrailer], trailingLen);
		}
		mLen = mLen - (replaceLen-insertLen);
		mBody[mLen] = '\0';
	}
	else {
		char* oldBody = mBody;
		const unsigned long expandedBy = insertLen - replaceLen;
		assert(expandedBy>0);
		const unsigned long newLen = mLen + expandedBy;
		mBody = new char[newLen+1];
		if (replaceAtOffset>0)
			memcpy(mBody, oldBody, replaceAtOffset);  // copy preceding chunk
		memcpy(&mBody[replaceAtOffset], newStr, insertLen);
		const unsigned long startTrailer = replaceAtOffset+replaceLen;
		if (startTrailer<mLen) { // might have found string at end so no trailer
			const unsigned long trailingLen = mLen - startTrailer;
			memcpy(&mBody[replaceAtOffset+insertLen], &oldBody[startTrailer], trailingLen);
		}
		mLen = newLen;
		mBody[newLen] = '\0';
		delete[] oldBody;
	}
}


unsigned long 
oofString::replaceFirstString(const char* replaceStr, const char* newStr,bool caseSensitive)
{
	unsigned long ret = 0;
	long foundAt = find(replaceStr, caseSensitive);	
	if (foundAt!=kNotFound) {
		ret = strlen(replaceStr);
		replaceAt(foundAt, ret, newStr);
	}
	return ret;
}


/**
	Search and replace a substring with optional starting offset and case sensitivity.
	\todo add an OOFILE sample to exercise edge conditions where input strings match
	\todo add an OOFILE sample to test if original string exactly matches replacement
	\todo check if David's increment is needed - startAt = foundAt + strlen(newStr);
	\warning too much optimisation will may break - you may get substrings where the replace string
	in combination with surroundings causes a recurrence of the original
*/
unsigned long 
oofString::replaceString(const char* replaceStr, const char* newStr, unsigned long startAt, bool caseSensitive)
{
	unsigned long numReplaced = 0;
	if (!replaceStr)
		return 0;
	
	//  if all strings and body are the same this is an endless loop so stop it
	if (caseSensitive){
		if (strcmp(replaceStr,newStr)==0)
			return 0;
	}
	else {
		if (OOF_stricmp(replaceStr,newStr)==0)
			return 0;
	}
	const unsigned long replaceLen = strlen(replaceStr);
	if (replaceLen==0)
		return 0;  // pathological case of replacing null string
			
	const unsigned long  newStrLen = strlen(newStr);
	// don't care if zero length replacement as that is a common case to remove a string
	for (;;) {
		long foundAt = find(replaceStr, startAt, caseSensitive, replaceLen);	// pass in length we're searching for to optimise find
		if (foundAt==kNotFound)
			break;  // exit - went to end of string without finding any more

		numReplaced++;
		replaceAt(foundAt, replaceLen, newStr, newStrLen);
// dangerous			startAt = foundAt;  // starts from beginning of place replaced, in case combination of replacement & existing string is replaceable
		// brute force fix for case when replace string with larger containing target
		// eg: replace "oof" with "oofile"
		 startAt = foundAt + newStrLen;
	}
	return numReplaced;
}

/**
	Search and replace a substring with optional starting offset and case sensitivity.
	Similar to 	replaceString except this only replaces whole word matches of sub string
	This to prevent calls like replaceStringWholeWord("htm", html) pm "html htm"
	giving htmll html
	A word is defined a s a sequence of matching chars, delimited by a non  alpha/numeric char 
	\todo add an OOFILE sample to exercise edge conditions where input strings match
	\todo add an OOFILE sample to test if original string exactly matches replacement
	\todo check if David's increment is needed - startAt = foundAt + strlen(newStr);
	\warning too much optimisation will may break - you may get substrings where the replace string
	in combination with surroundings causes a recurrence of the original
*/
unsigned long 
oofString::replaceStringWholeWord(const char* replaceStr, const char* newStr, unsigned long startAt,  bool caseSensitive)
{
	unsigned long numReplaced = 0;
	if (!replaceStr)
		return 0;
	
	//  if all strings and body are the same this is an endless loop so stop it
	if (caseSensitive){
		if (strcmp(replaceStr,newStr)==0)
			return 0;
	}
	else {
		if (OOF_stricmp(replaceStr,newStr)==0)
			return 0;
	}
	const unsigned long replaceLen = strlen(replaceStr);
	if (replaceLen==0)
		return 0;  // pathological case of replacing null string
			
	const unsigned long  newStrLen = strlen(newStr);
	// don't care if zero length replacement as that is a common case to remove a string
	for (;;) {
		long foundAt = find(replaceStr, startAt, caseSensitive, replaceLen);	// pass in length we're searching for to optimise find
		if (foundAt==kNotFound)
			break;  // exit - went to end of string without finding any more

		//char theNextOne = mBody[foundAt+replaceLen] ;
		if(!isalnum(mBody[foundAt+replaceLen]) && !isalnum(mBody[foundAt-1])){// is this the end of a word
				
			numReplaced++;
			replaceAt(foundAt, replaceLen, newStr, newStrLen);
	// dangerous			startAt = foundAt;  // starts from beginning of place replaced, in case combination of replacement & existing string is replaceable
			// brute force fix for case when replace string with larger containing target
			// eg: replace "oof" with "oofile"
		}
		startAt = foundAt + newStrLen;
		
	}
	return numReplaced;
}


void 
oofString::encodeURL()
{
// NOT YET IMPLEMENTED - anything more complex than replacing spaces with %20
	replaceString(" ", "%20");
}


oofString
oofString::decodeEntity(const oofString& srcString)
{
	const char * theString;
	char  currentChar;
	int   aNumber,hNumber,index=0;
	theString = srcString;
	oofString res;
	char * end;
	char tempStr[5];
	tempStr[0] = '0';
	
	while((currentChar=theString[index++])!='\0'){
		if(currentChar=='&'){
			if(theString[index] == '#'){
				//it's a number so jump over #
				index++;
				//assert(theString[index]=='x');
				bool isHex = (theString[index]=='x');
				
				aNumber = 0;
				// make a proper hex sequence for strtol
				char* strP = tempStr+1;
				while((currentChar = theString[index++])!=';'){
					*strP++ = currentChar;
							}
				*strP= currentChar;
				//aNumber = atoi(theString+index); // terminates on ';'
				if(isHex)
					aNumber = (int)strtol(tempStr,&end,16); // terminates on ';'
				else
					aNumber = (int)strtol(tempStr,&end,10); 
				assert(aNumber < 256);
				//index = index + (end-theString);
				//while((currentChar = theString[index++])!=';'); // jump over the encoded sequence
				res += char(aNumber);					
			}
			else{
				//it's an embbeded sequence
				hNumber = 0;
				while((currentChar = theString[index++])!=';'){
					hNumber += currentChar;
				}
					
				// make the compiler do the work to calc case constants, in case of prog error!	
				switch(hNumber){
					case (unsigned long) 'l'+'t':   // 224
						res += '<';	
					break;
					case (unsigned long) 'g'+'t':   // 219:
						res += '>';
					break;
					case (unsigned long) 'q'+'u'+'o'+'t':   // 558:
						res += '"';
					break;
					case (unsigned long) 'a'+'m'+'p':   // 318:
						res +='&' ;
					break;
					case (unsigned long) 'a'+'p'+'o'+'s':   // 435:
						res += '\'';
					break;
				}
				
				
			}
						
		}
		else{
			//plain char	
			res += currentChar;
		}
	
	}
	return res;
}

oofString 
oofString::encodeEntity(const oofString& srcString)
{
	int index = 0;
	int destIndex = 0;
	char currentChar;
	const char * theString =  srcString;
	oofString res;
	unsigned int numberOfEntities = 0;
	unsigned int newBufferSize = 0;
	char * newBuffer;
	
	
	//count the number of entities
	while((currentChar = theString[index++])!='\0') {
		if( (unsigned char)currentChar>=128) {
			numberOfEntities += 6;
		}
		else {
			switch(currentChar) {
			case '<':
			case '>':	
				numberOfEntities += 3;
			break;
			
			case '"':
			case '\'':
				numberOfEntities += 5;
			break;
			
			case '&':
				numberOfEntities += 4;
				break;
				
			default:
				;
			}
		}
	
	}
	//get a new buffer
	newBufferSize = numberOfEntities + index ; 
	newBuffer = new char [newBufferSize];
	char tempStr[5];
	index = 0;
	//unsigned int theNumber;
	while((currentChar = theString[index++])!='\0'){
		if((unsigned char)currentChar>=128){
			//theNumber = (unsigned int)currentChar;
			//sprintf( tempStr, "%d", (unsigned char)currentChar );
			sprintf( tempStr, "%x", (unsigned char)currentChar );
			
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = '#';
			//newBuffer[destIndex++] = tempStr[0];
			//newBuffer[destIndex++] = tempStr[1];
			//newBuffer[destIndex++] = tempStr[2];
			newBuffer[destIndex++] = 'x';
			
			newBuffer[destIndex++] = tempStr[0];
			newBuffer[destIndex++] = tempStr[1];
			
			newBuffer[destIndex++] = ';';
			
		}
		else
		switch(currentChar){
		case '<':
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = 'l';
			newBuffer[destIndex++] = 't';
			newBuffer[destIndex++] = ';';
		break;
		
		case '>':
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = 'g';
			newBuffer[destIndex++] = 't';
			newBuffer[destIndex++] = ';';
		break;
		
		case '"':
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = 'q';
			newBuffer[destIndex++] = 'u';
			newBuffer[destIndex++] = 'o';
			newBuffer[destIndex++] = 't';
			newBuffer[destIndex++] = ';';
		break;
		
		case '&':
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = 'a';
			newBuffer[destIndex++] = 'm';
			newBuffer[destIndex++] = 'p';
			newBuffer[destIndex++] = ';';
		break;
		
		case '\'':
			newBuffer[destIndex++] = '&';
			newBuffer[destIndex++] = 'a';
			newBuffer[destIndex++] = 'p';
			newBuffer[destIndex++] = 'o';
			newBuffer[destIndex++] = 's';
			newBuffer[destIndex++] = ';';
		break;
		
		default:
			newBuffer[destIndex++] = currentChar;	
		}
	}
	newBuffer[destIndex] = '\0';
	res.adopt(newBuffer);
	return res;
}


oofString
oofString::utf8To8BitAscii(const char* srcString, unsigned long len)
{
	const char * theString;
	char  currentChar;
	unsigned long   srcIndex = 0, dstIndex = 0;
	
	theString = srcString;
	oofString res = srcString;
	
	while(srcIndex < len){
		currentChar = srcString[srcIndex++];
		if((unsigned char)currentChar< 0x80){
			res[dstIndex++] = currentChar;
		}
		else{
			res[dstIndex++] = (currentChar << 6) | ((theString[srcIndex++])&0x3f);		
		}
	}
	res.shorten(res.length()-dstIndex);
	return res;
}	


oofString
oofString::quoteString(const oofString& srcString, char quoteChar)
{
	return oofString(quoteChar, srcString, quoteChar);
}


oofString
oofString::legalName(const oofString& inString, char nonAlphaReplace, char leadingDigitReplace, char spaceReplace)
{	
	oofString legalName;
	
	const unsigned long stringLength = inString.length();
	if (stringLength == 0)
		return legalName; //early exit
		
	//replace non-alphanumeric characters
	for(unsigned long i = 0; i < stringLength; i++){
	
		if (inString[i] == ' ')
			legalName += spaceReplace;  
			
		else if ((inString[i] >= '0' && inString[i] <= '9') ||
			(inString[i] >= 'a' && inString[i] <= 'z') ||
			(inString[i] >= 'A' && inString[i] <= 'Z'))
			
			legalName += inString[i];
		else
			legalName += nonAlphaReplace;
	}
	
	//first character must not be a digit
	if (inString[0] >= '0' && inString[0] <= '9')
		legalName[0] = leadingDigitReplace;

	return legalName;
}


oofString
oofString::singular(const oofString& inString)
{
	oofString ret;
	const unsigned long inLen = inString.length();
	
// crude comparisons	
	if (inString.endsWith("ies")) {
		ret = inString.subString(0, inLen-2);
		ret[inLen-3] = 'y';  // 'ies' to 'y'
	}
	else if (inString.endsWith("ss")) {
		ret = inString;  // eg: Progress is returned unchanged
	}
	else if (inString.endsWith('s'))
		ret = inString.subString(0, inLen-1);
	else
		ret = inString;
	return ret;
}


bool 
oofString::startsWith(char inChar) const
{
	const bool ret = (firstChar() == inChar);
	return ret;
}


bool 
oofString::startsWith(const char* inStr) const
{
	if (!inStr)
		return false;  // trivial case for null pointer to protect strlen below
		
	const unsigned int inLen = strlen(inStr);
	if (inLen>mLen)
		return false;
	else {
		for (unsigned int i=0; i<inLen; i++) {
			if (mBody[i]!=inStr[i])
				return false;
		}
	}
	return true;  // if gets here all match
}


bool 
oofString::endsWith(char inChar) const
{
	const bool ret = (lastChar()== inChar);
	return ret;
}


bool 
oofString::endsWith(const char* inStr, int inLen) const
{
	if (!inStr || !mBody)
		return false;  // trivial case for null pointer to protect strlen below
		
	if (inLen<0)  // caller didn't calc for us
		inLen = strlen(inStr);
	if (inLen>mLen)
		return false;
	else {
		const unsigned int offset = mLen-inLen;
		for (unsigned int i=0; i<inLen; i++) {
			if (mBody[i+offset]!=inStr[i])
				return false;
		}
	}
	return true;  // if gets here all match
}


unsigned long 
oofString::replaceBetween(const char* leftStr, const char* rightStr, const char* insertStr,bool caseSensitive)
{
	unsigned long ret = 0;
	const long leftAt = findEnd(leftStr,caseSensitive);	
	if (leftAt!=kNotFound) {
		long rightAt = find(rightStr, leftAt  /* startFrom */,caseSensitive);	
		if (rightAt!=kNotFound) {
			ret = rightAt-leftAt;
			replaceAt(leftAt, ret, insertStr);
		}
	}
	return ret;
}



unsigned long 
oofString::stripChar(char toStrip, unsigned long startAt)
{
// scan left-to right. moving chars N to the left as we go
// leaves original allocation with null moved left
	unsigned long numStripped = 0;
	const unsigned long kLen = mLen;
	for (unsigned long i=startAt; i<kLen; i++) {
		if (mBody[i]==toStrip) {
			numStripped++;
		}
		else {
			if (numStripped) {
				const unsigned long moveTo = i-numStripped;
				mBody[moveTo] = mBody[i];
			}
		} // is a stripped char
	}  // loop all chars
	if (numStripped) {
		mLen -= numStripped;
		mBody[mLen] = '\0';
	}
	return numStripped;
}


void 
oofString::makeRandomChars(unsigned long numChars)
{
	if (numChars != mLen) {	// alloc once for efficiency sake
		delete[] mBody;
		mBody = new char[numChars+1];
		mLen = numChars;	
	}
	mBody[numChars]='\0';
	for (unsigned long i=0; i<numChars; ++i) {
		char c = rand()%0x5F + 0x20;	// printable range  0x20 -> 0x7F
		mBody[i]=c;
	}
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
}


bool 
oofString::lineEndingsMac()
{
	if (!mBody)
		return false;
	
	char* newBody = lineEndingsMac(mBody, mLen);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mBody = newBody;
	}
	return (newBody!=0);
}


char*   // 0 return means no change
oofString::lineEndingsMac(char*inChars, unsigned long& ioLen)
{
// WARNING - near identical copy of lineEndingsUnix, if you fix anything make sure
// you fix both

bool anyChange=false;

#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
// WARNING cannot assume same pattern will prevail throughout string
// application logic may have put just LF or CR in a DOS-style field
	unsigned long inLen = ioLen;  // we may shorten this if we remove LF's
	for (unsigned long i=0; i<inLen; i++) {
		if (inChars[i]==CR) {
			if (i==inLen-1)
				break;	// EXIT - WE'RE FINISHED - ends in CR (Mac string with linebreak at very end)
			else if (inChars[i+1]==LF) {	// is CRLF string at present
// move the rest of the string down one byte
				const long charsLeft = inLen - i;
				memmove(&inChars[i+1], &inChars[i+2], charsLeft);
				inLen--;
				inChars[inLen] = '\0';	// put terminating null in correct spot.
				anyChange=true;
			} // is CRLF so delete LF
		}  // is CR
		else if (inChars[i]==LF) {	// is Unix line ending - just LF
			inChars[i] = CR;  // easy replace on the spot
			anyChange=true;
		} // is LF
	} // conversion loop
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif

	char* ret = 0;
	if (anyChange) { // return copied short buffer so can delete original
		if (inLen == ioLen) {  // just in-place replacements
			ret = inChars;
		}
		else {
			ret = new char[inLen+1];
			memcpy(ret, inChars, inLen);
			ret[inLen]='\0';
			ioLen = inLen;  // update length to shortened length
		}
	}
	return ret;
}


bool 
oofString::lineEndingsUnix()
{
	if (!mBody)
		return false;
	
	char* newBody = lineEndingsUnix(mBody, mLen);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mBody = newBody;
	}
	return (newBody!=0);
}


char*    // 0 return means no change
oofString::lineEndingsUnix(char*inChars, unsigned long& ioLen)
{
// WARNING - near identical copy of lineEndingsMac, if you fix anything make sure
// you fix both

bool anyChange=false;

#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
// WARNING cannot assume same pattern will prevail throughout string
// application logic may have put just LF or CR in a DOS-style field
	unsigned long inLen = ioLen;  // we may shorten this if we remove LF's
	for (unsigned long i=0; i<inLen; i++) {
		if (inChars[i]==CR) {
			inChars[i] = LF;  // easy replace on the spot
			anyChange=true;

			if (i==inLen-1)
					break;	// EXIT - WE'RE FINISHED - ends in CR (Mac string with linebreak at very end)
				else if (inChars[i+1]==LF) {	// is CRLF string at present
	// move the rest of the string down one byte (we just put our LF on top of the CR, so our loop is identical to Mac)
					const long charsLeft = inLen - i;
					memmove(&inChars[i+1], &inChars[i+2], charsLeft);
					inLen--;
					inChars[inLen] = '\0';	// put terminating null in correct spot.
				} // is CRLF so delete LF
		}  // is CR
	} // conversion loop
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif

	char* ret = 0;
	if (anyChange) { // return copied short buffer so can delete original
		if (inLen == ioLen) {  // just in-place replacements
			ret = inChars;
		}
		else {
			ret = new char[inLen+1];
			memcpy(ret, inChars, inLen);
			ret[inLen]='\0';
			ioLen = inLen;  // update length to shortened length
		}
	}
	return ret;
}


bool 
oofString::lineEndingsDOS()
{
	if (!mBody)
		return false;
	
	char* newBody = lineEndingsDOS(mBody, mLen);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mBody = newBody;
	}
	return (newBody!=0);
}


bool
oofString::lineEndingsCurrentPlatform()
{
#if defined(_Macintosh)
	return lineEndingsMac();
#elif defined(_MSDOS)  // implies _Windows
	return lineEndingsDOS();
#elif defined(_Unix)
	return lineEndingsUnix();
#else
	ERROR "must have _Macintosh _Windows or _Unix defined"
#endif
}


char*   // 0 return means no change
oofString::lineEndingsDOS(char*inChars, unsigned long& ioLen)
{
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
// WARNING cannot assume same pattern will prevail throughout string
// application logic may have put just LF or CR in a DOS-style field
	unsigned long startAt = 0;
	unsigned long numInserts = 0;
	const unsigned long inLen = ioLen;
// determine starting point of conversion needed if any and number of inserts in same pass
// count the inserts to know how much we expand, as expanding by one char each time is 
// vastly more expensive than the repeated scans
	for (unsigned long i=0; i<inLen; i++) {
		if (inChars[i]==CR) {
			if (i==inLen-1) {
				if (startAt==0)
					startAt = i;  // single line ending - Mac style at end of string
				numInserts++;
			}
			else if (inChars[i+1]==LF) {	// is CRLF string at present
				i++;	// skip over LF that's part of DOS line ending we ignore
			}
			else {
				if (startAt==0)
					startAt = i;  // Mac-style embedded in string
				numInserts++;
			}
		}  // hit a CR
		else if (inChars[i]==LF) {	// is Unix string - just LF
			if (startAt==0)
				startAt = i;
			inChars[i]=CR;  // simplifies conversion later - only insert LF's after!!!
			numInserts++;
		}
	} // loop to determine conversion
// when we get here we know that we are converting *something* if startAt points to it
// we either hit a Mac or Unix line ending, even if combined with some DOS line endings
	if (numInserts==0)
		return 0;  // EXIT - NO LINE ENDINGS, OR DOS-STYLE ONLY
		
	unsigned long currentLen = inLen;	// will move along as we extend by insertion
	const unsigned long newLen = currentLen + numInserts;
	char* newBody = new char[newLen+1];	// allow for trailing null

// copy body and insert chars where necessary	
// we know that we either have CRLF pairs, or just CR by themselves as we converted any LF's above
	unsigned long copyFrom = 0;
	for (unsigned long copyTo=0; copyTo < newLen; copyFrom++, copyTo++) {
		const char ch = inChars[copyFrom];
		newBody[copyTo] = ch;
		if ( (ch==CR) && (inChars[copyFrom+1]!=LF) ) { 
			copyTo++;
			newBody[copyTo] = LF;
		} // found a CR and needed to insert following LF
	} // copy all chars

	newBody[newLen] = '\0';
	ioLen = newLen;  // RETURN new length
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	return newBody;
}


unsigned short 
oofString::asUnsignedShort() const
{
	unsigned short ret = 0;
	if (length()) {
		sscanf(mBody, "%hu", &ret);
	}
	return ret;
}


short 
oofString::asShort() const
{
	short ret = 0;
	if (length()) {
		sscanf(mBody, "%hd", &ret);
	}
	return ret;
}



long 
oofString::asLong() const
{
	long ret = 0;
	if (length()) {
		sscanf(mBody, "%ld", &ret);
	}
	return ret;

}



unsigned long 
oofString::asUnsignedLong() const
{
	unsigned long ret = 0;
	if (length()) {
		sscanf(mBody, "%lu", &ret);
	}
	return ret;

}



double 
oofString::asDouble() const
{
	double ret = 0;
	if (length()) {
		sscanf(mBody, "%le", &ret);
	}
	return ret;

}


char
oofString::firstChar() const
{
	if ((mLen==0) || !mBody)
		return '\0';
	else
		return mBody[0];
}


char
oofString::lastChar() const
{
	if ((mLen==0) || !mBody)
		return '\0';
	else
		return mBody[mLen-1];
}


void
oofString::adopt(char* str)
{		
	delete[] mBody;
	mBody = str;
	mLen = strlen(str);
}


void
oofString::adopt(oofString& rhs)
{		
	delete[] mBody;
	mLen = rhs.length();
	mBody = rhs.orphan(false);	// don't want it to generate an empty string if rhs empty
}


void
oofString::clear()
{
	delete[] mBody;
	mBody = 0;
	mLen = 0;
}


char*
oofString::orphan(bool neverReturnNil)
{		
	char* ret=mBody;
// valid for an empty oofString to have no body, this is one of our optimisations
	if (!ret) {  
		if (neverReturnNil)
			ret = new char[1];
			*ret = '\0';	// make an empty string for them to own
	}
	else {
		mBody = 0;
		mLen = 0;
	}
	assert(mLen==0 && mBody==0);
	return ret;
}


oofString 
oofString::left(unsigned long numChars) const
{
	if (numChars > mLen)
		numChars = mLen;
	return oofString(mBody, numChars);
}


oofString 
oofString::right(unsigned long numChars) const
{
	if (numChars > mLen)
		numChars = mLen;
	unsigned long offset = mLen - numChars;
	return oofString(mBody+offset, numChars);
}


void
oofString::shorten(unsigned short dropChars)
{
	if (dropChars>mLen)
		dropChars = mLen;
	mLen -= dropChars;
	mBody[mLen] = '\0';
}


#ifndef OOF_NO_STDLIB
	OOFILE_EXPORT ostream& 
	operator<<(ostream& os, const oofString& str)
	{
		os << str.chars();
		return os;
	}
#endif

oofString 
operator+(const oofString& lhs, const char* rhs)
{
	return oofString(lhs, rhs);
}


oofString 
operator+(const oofString& lhs, const oofString& rhs)
{
	return oofString(lhs, rhs);
}


bool 
oofString::isAlphaNumeric() const
{
	if (isEmpty())
		return true;
	
	const unsigned long theLen = mLen;
	for (unsigned long i=0; i<theLen; i++) {
		const char ch = mBody[i];
		if (!isalnum(ch))
			return false;
	}
	return true;
}


// -------------------------------------------------------
//              o o f C h e c k S t r i n g
// -------------------------------------------------------
oofCheckString::oofCheckString(unsigned long n, char sepChar)
{
// NOT YET IMPLEMENTED - estimate size based on size of number & prealloc
// later do more efficient implementation - I need this NOW!
	char buf[80];
	sprintf(buf, "%lu", n);
	unsigned int numChars = strlen(buf);
// the spaces at the end of these strings are replaced by the specified separator
// if it is different	
	for (unsigned int i=0; i<numChars; ++i) {
		switch(buf[i]) {
		case '0' :
			*this += "zero ";
			break;
		case '1' :
			*this += "one ";
			break;
		case '2' :
			*this += "two ";
			break;
		case '3' :
			*this += "three ";
			break;
		case '4' :
			*this += "four ";
			break;
		case '5' :
			*this += "five ";
			break;
		case '6' :
			*this += "six ";
			break;
		case '7' :
			*this += "seven ";
			break;
		case '8' :
			*this += "eight ";
			break;
		case '9' :
			*this += "nine ";
			break;
		default :
			*this += " ";
		}
		if (sepChar!=0x20)
			mBody[mLen-1] = sepChar;
	}
	shorten(1);	// drop the last separator
}


