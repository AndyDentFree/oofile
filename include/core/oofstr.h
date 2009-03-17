#ifndef H_OOFSTR
#define H_OOFSTR

// COPYRIGHT 1997 A.D. Software, All rights reserved

// OOFILE string classes

// NOTE inline definitions included at end of this header file

// see misc.txt for code conventions

#ifndef H_OOF0
	#include "oof0.h"
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#if !defined(OOF_NO_STDLIB) && !defined(H_OOFIOS)
	#include "oofios.h"
#endif
#ifdef _Macintosh
	#include <Types.h>
#endif

#ifndef OOF_NO_STDLIB
	#if OOF_HAS_ANSI_STRINGS
		#include <string>
		#include <sstream>
		#if defined(_MSC_VER) && !defined(__MWERKS__)
			#include <xstring>
		#endif
	#else
		class ostream;
		class istream;
		class ofstream;
	#endif
#endif

#ifdef _Windows
	#if _MSC_VER>=1010
		// want to suppress these warnings for ver 4.1 but not other compilers or warnings
		// eg: 0x800 for earlier version 1.5
		#pragma warning( disable : 4237 )  // bool, true & false
		#pragma warning( disable : 4800 )  // bool conversion to int
		#pragma warning( disable : 4355 )  // use of 'this' in intitalisation list
	#endif
#endif


/**
	Portable highly capable string class.
	Written because ANSI strings lack features on older compilers.
	Also Mac-aware if running on a Mac (not if using Quicktime for Windows).
	\todo add reference-counting subclass oofRefString
	\ingroup utils
*/
class OOFILE_EXPORT oofString {
public:
	enum {CR = 13, LF = 10, kNotFound=-1};

/** 
string comparison nearly directly compatible with PowerPlant's
except doesn't have inherent size limits from assumption of 
Str255 types (Pascal strings)

   left == right, returns 0
   left >  right, returns 1
   left <  right, returns -1
*/
	typedef	short	(*oofCompareFunc)(
							const void*		inLeft,
							const void*		inRight,
							long			inLeftLength,
							long			inRightLength);
							
// constructors
	oofString(char, int repeatCount);  // can't just have char param as VC stupid ambiguities cause problems with dbTable::field
	oofString(const char* str=0);  // default
	oofString(const oofString&);	// copy ctor
	oofString(const char* chars, unsigned long numChars);
	oofString(char* adoptedChars, unsigned long numChars, bool adopt);

	void loadResource(unsigned long strID);
	
#ifndef OOF_NO_STDLIB
// construct from streams
	oofString(ostream&);
#endif  // OOF_NO_STDLIB
#if OOF_USE_ANSI_STREAMS
	oofString(stringstream&);
#endif
	 
#if OOF_HAS_ANSI_STRINGS
	oofString(const string&);
#endif
	
#ifdef _Macintosh
	oofString(const Str255);
#endif
		
// variations for easy concatenation	
	oofString(const oofString&, const char*);
	oofString(const oofString&, const oofString&);
	oofString(const oofString&, const char, const char *);
	oofString(const oofString&, const char*, const oofString&);
	oofString(char, const oofString&, char);

	~oofString();

// setters
#if OOF_HAS_ANSI_STRINGS
	oofString& operator=(const string&);
#endif
	oofString& operator=(const oofString&);
	oofString& operator=(const char*);
	void insertChars(const char*, unsigned long, unsigned long beforeChar=ULONG_MAX);
	void setChars(const char*, unsigned long);
	void adopt(char*);
	void adopt(oofString&);
	void shorten(unsigned short);
	void padTo(unsigned short paddedLength, char padWith=' ', bool rightAlign = false);
	void operator+=(char);  
	void operator+=(const char*);  
	void operator+=(const oofString&);  
	void append(const char*, unsigned long appendLen);
	void clear();
	
#if defined(_MSC_VER) && !defined(__MWERKS__)
// extra operators because Visual C++ has always been stupid about
// claiming ambiguities when choosing between integer RHS conversions and creating temporaries
	char& operator[](int) const;
	char& operator[](short) const;
#endif
	char& operator[](unsigned long) const;
	void convertNumber(int, const char* printfMask=0, bool appendIt=false);
	void convertNumber(unsigned long, const char* printfMask=0, bool appendIt=false);
	void convertNumber(double, const char* printfMask=0, bool appendIt=false);

	// strip & replace return number replaced
	unsigned long replaceChar(char fromChar, char toChar, unsigned long startAt=0);  // return num replaces
	void replaceAt(unsigned long startAt, unsigned long numChars, const char* newStr, unsigned long newLen=ULONG_MAX);
	unsigned long replaceFirstString(const char* replaceStr, const char* newStr,bool caseSensitive = false);  // return size replaced //david debug 991209
	unsigned long replaceString(const char* replaceStr, const char* newStr, unsigned long startAt=0,bool caseSensitive = false);  // return num replaces//david debug 991209
	unsigned long replaceBetween(const char* leftStr, const char* rightStr, const char* insertStr,bool  caseSensitive = false);//david debug 991209
	unsigned long replaceStringWholeWord(const char* replaceStr, const char* newStr, unsigned long startAt=0,bool caseSensitive = false);
	
	unsigned long stripChar(char, unsigned long startAt=0);  // return num stripped
	void makeRandomChars(unsigned long numChars);
	void encodeURL();
	
#ifdef _Macintosh
	void setStr255(const Str255);
	oofString& operator=(const Str255);
#endif
	
// getters	
	operator const char*() const; 
	const char* chars() const; 
	char* charsForOldInterfacesNotWriting() const;
	char* orphan(bool neverReturnNil=true);
	char firstChar() const;
	char lastChar() const;
	bool startsWith(char) const;
	bool startsWith(const char*) const;
	bool endsWith(char) const;
	bool endsWith(const char*, int inLen=-1) const;
	void getChars(char* ioBuff, unsigned long& ioLen) const;
#ifdef _Macintosh
	void getStr255(Str255 s) const;
#endif
#if OOF_HAS_ANSI_STRINGS
	operator string() const;
#endif

// getters for portions & find utilities
	oofString left(unsigned long numChars) const;
	oofString right(unsigned long numChars) const;
	oofString subString(unsigned long fromChars, unsigned long numChars=ULONG_MAX) const;
	oofString	subStringBetween(const char*, const char*) const;
	oofString	subStringBetween(char, char) const;
	oofString operator-(const char*) const;
	
	long find(const char*, unsigned long startFrom=0, bool caseSensitive=false, unsigned long inLen=ULONG_MAX) const;
	bool contains(const char*, bool caseSensitive=false) const;
	bool contains(char, bool caseSensitive=false) const;
	long find(char, unsigned long startFrom=0, bool caseSensitive=false) const;
	long findLast(char, bool caseSensitive=false) const;
	long findEnd(const char*, unsigned long startFrom=0, bool caseSensitive=false) const;
	
// getters as numbers
	unsigned short asUnsignedShort() const;
	short asShort() const;
	long asLong() const;
	unsigned long asUnsignedLong() const;
	double asDouble() const;
	
	
// reflective operators	
	bool isEmpty() const;
	unsigned long length() const;
	bool isAlphaNumeric() const;
	
// comparison ops	CASE-SENSITIVE COMPARISONS!!!!! unless overriden
#ifdef _Macintosh
	bool operator==(const Str255) const;	
#endif
	bool operator==(const oofString& rhs) const;	
	bool operator==(const char*) const;	
	bool operator!=(const char*) const;	
	bool operator!=(const oofString& rhs) const;
	
// in-place content converters
	bool lineEndingsCurrentPlatform(); 
	bool lineEndingsMac();
	bool lineEndingsUnix();
	bool lineEndingsDOS();
	


// content converters for external calls, may update inChars and 
// return 0==no change, new buffer or inChars if length unchanged	
	static char* lineEndingsMac(char*, unsigned long& ioLen);
	static char* lineEndingsUnix(char*, unsigned long& ioLen);
	static char* lineEndingsDOS(char*, unsigned long& ioLen);
	
	static oofString encodeEntity(const oofString&);
	static oofString decodeEntity(const oofString&);
	static oofString utf8To8BitAscii(const oofString&);
	static oofString utf8To8BitAscii(const char*, unsigned long);

	static oofString quoteString(const oofString&, char quoteChar='\'');
	static oofString singular(const oofString&);
	static oofString legalName(const oofString&, char nonAlphaReplace='_', char leadingDigitReplace='n', char spaceReplace='_');
	
private:
	void FinishConstruction(const char *chars, unsigned long numChars);
	void AppendSome(const char*, unsigned long appendLen);
			
protected:
// data storage
	char *mBody;	// owned
	unsigned long mLen;
	static char sEmptyChar;
};
#ifndef OOF_NO_STDLIB
	OOFILE_EXPORT ostream& operator<<(ostream&, const oofString&);
#endif

OOFILE_EXPORT oofString operator+(const oofString&, const char*);
OOFILE_EXPORT oofString operator+(const oofString&, const oofString&);

typedef oofString OOF_String;
// we are frequently using this as a datatype in user code, but our general
// rule is that the OOF_ prefix refers to internal OOFILE data types only
// and oof prefix is for public classes (non-database)

typedef OOF_String OOF_MacString;	// used to be separate subclass



/**
	strings for named numbers eg check (cheque) printing,
	eg:
	"one two zero"
	\ingroup utils
*/
class OOFILE_EXPORT oofCheckString : public oofString {
public:
// constructors
	oofCheckString(unsigned long, char sepChar=0x20);
};
typedef oofCheckString oofChequeString; // for the Aussies & British
typedef oofCheckString OOF_CheckString; // for internal use


/**
	Compare strings ignoring case.
	\ingroup utils
*/
int OOF_stricmp (const char*, const char*);


/**
	Compare strings ignoring case, with length limit.
	\ingroup utils
*/
int OOF_strnicmp (const char*, const char*, const unsigned int);


// -------------------------------------------------------
//              o o f S t r i n g
// -------------------------------------------------------
#if defined(_MSC_VER) && !defined(__MWERKS__)
// extra operators because Visual C++ has always been stupid about
// claiming ambiguities when choosing between integer RHS conversions and creating temporaries
inline char& 
oofString::operator[](int index) const
{
	return operator[]((unsigned long)index);
}


inline char& 
oofString::operator[](short index) const
{
	return operator[]((unsigned long)index);
}
#endif


inline bool
oofString::contains(const char* schStr, bool caseSensitive) const
{
	const bool ret = (find(schStr, 0, caseSensitive) != kNotFound);
	return ret;
}


inline bool
oofString::contains(char schChar, bool caseSensitive) const
{
	const bool ret = (find(schChar, 0, caseSensitive) != kNotFound);
	return ret;
}


inline const char*
oofString::chars() const
{
	if (mBody)
		return mBody;
	else
		return "";
}


inline char*
oofString::charsForOldInterfacesNotWriting() const
{
	if (mBody)
		return mBody;
	else
		return "";
}


inline bool 
oofString::isEmpty() const
{
	return mBody==0 || mLen==0;
}


inline bool 
oofString::operator!=(const oofString& rhs) const
{
	return !(*this==rhs);
}


inline bool 
oofString::operator!=(const char* rhs) const
{
	return !(*this==rhs);
}


inline unsigned long 
oofString::length() const
{
	return mLen;
}


inline oofString
oofString::utf8To8BitAscii(const oofString& srcString)
{
		return utf8To8BitAscii(srcString.chars(), srcString.length());
}


#if defined(_Macintosh)
inline oofString& 
oofString::operator=(const Str255 s)
{
	setStr255(s);
	return *this;
}
#endif


#endif

