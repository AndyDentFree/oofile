#ifndef H_OOF3
#define H_OOF3

// COPYRIGHT 1994 A.D. Software, All rights reserved

// OOFILE database non-numeric field-related classes
// see also oof4.h

#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifdef _Macintosh
#include <Types.h>
#endif

enum OOF_fieldTypes {charField, textField, dateField, shortField, uShortField, longField, uLongField, realField, blobField, compoundField, boolField, dateTimeField, timeField, fixedBinaryField, relationshipField};
OOFILE_EXPORT ostream& operator<<(ostream&, OOF_fieldTypes);

class dbChar;
class dbText;

/**
	Base class for a calculator attached to a dbField.
	The calculator will be used to supply the value for the field, either
	always or just a default to initialise an empty field.
	\par
	\see dbField::calculateWith for how you attach calculators.
	\par
	In general, calculated fields do not occupy storage space. 
	However, the author of a specific calculated field may do something 
	to cause another field to be allocated in the database.
	Also, indexed calculated fields will normally allocate a field to contain
	the indexed value. This makes them very fast to search.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbCalculator {
public:
	virtual dbCalculator* clone() const=0;
	virtual dbCalculator* cloneOwned(dbField* newOwner) const;  // has default implementation as not necessary to override
	virtual ~dbCalculator() {};
};



/**
	Base class for persistent fields in dbTable's.
	Subclasses prefixed by "oof", eg: oofDate are <em>not persistent</em>
	and allow you to use local variables that match the other OOFILE typing.
	eg: you may want a local variable included in a dbView fed to oofRep or
	linked to a form with dbEditHelper.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbField : public OOF_PublicBase {
public:
	enum FieldIsStandaloneT {eFieldIsStandalone};  // used as flag to constructors
	enum { kKeyLengthAutoCalculated = USHRT_MAX};

public:
	dbField(const OOF_IndexOptions); 
	dbField(const char* fieldName=0);
	dbField(const char* fieldName, const OOF_IndexOptions);
 	dbField(const dbField&); 
 	dbField(FieldIsStandaloneT); 
	virtual ~dbField();
	virtual dbField* clone() const=0;

	virtual void postCloneTableCleanup() {};
	
	virtual oofString name() const;

// additional setting operators
	void index(const OOF_IndexOptions=kIndexed);
	void indexPartialKeyLen(unsigned short);
	void setName(const char* name);
	
	bool hasDefaultCalculator() const;
	void useCalculatorOnlyForDefaults();
	bool storesCalculatedValues() const;
	void storeCalculatedValues();
	bool usingCalculator() const;

// reflective operators
	const char* tableName() const;
	bool fieldIsStandalone() const;
	virtual bool fieldIsSameTypeAs(const dbField*) const;
	virtual void describe(ostream&) const;
//	virtual unsigned short countValues() const;
	virtual OOF_fieldTypes fieldType() const=0;
	virtual OOF_fieldTypes nativeType() const;
	virtual unsigned long fieldStorageLen() const=0;
	virtual unsigned long fieldMinWrapWidth() const;
	virtual unsigned long fieldUsualWrapWidth() const;
	dbTable* fieldTable() const;
	unsigned long fieldDataLen() const;
	virtual bool isEmpty() const;
	bool isDirty() const;
	fieldNumT fieldNumber() const;
	virtual const oofString& fieldName() const;
	OOF_IndexOptions fieldIndexOptions() const;
	bool fieldIndexIgnoresNulls() const;
	bool fieldIndexIsCompressLeading() const;
	bool fieldIndexIsCompressPadding() const;
	virtual bool fieldIsIndexed() const;
	bool fieldIsPartiallyIndexed() const;
	virtual bool fieldIsKeywordIndexed() const;
	virtual bool fieldIsUniqueIndexed() const;
	unsigned short fieldNumIndexes() const;
	unsigned short fieldKeyLen() const;
	
	bool caseSensitive() const;
	virtual bool fieldIsBlob() const;
	virtual bool fieldIsBinary() const;
	virtual bool fieldIsVirtual() const;
	dbField* equivalentFieldFromTable(dbTable*) const;
	bool valueIsDuplicate() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;	// has default imp for non searchables
	bool fieldIsJoinKey() const;

// data access
// removed due to VC5 (possibly ANSI rules, or MS bug)	operator oofString() const;
	virtual void clear() {};  
	virtual void setString(const char*) {};  
	virtual void setNumber(long) {};  
	void operator=(const char*); 
	virtual oofString copyString() const;
	virtual void getChars(char* ioBuff, unsigned long& outLen) const;
	virtual const char* asChars() const;
	const void* binaryContents() const;
	const void* currentbinaryContents() const;
	virtual void extract(ostream&) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*) {};  // should override but only if makes sense to join on field type
	virtual void copyValueIfDifferent(const dbField*) {};
	virtual bool validateContents();
	virtual oofWordParser* words() const;
	void saveDefaultCalculatedValue();
	void saveStoredCalculatedValue();

	virtual void generateTestData(bool /*generateRandom*/, unsigned long /*seedOrCount*/) {};	
	
	
protected:
	void CalculateWith(dbCalculator* adoptedCalculator, bool useOnlyForDefaults);
	virtual void CopyCalculatedValueToBuffer();

// data storage
	OOF_tableBackend *mBackend;
	dbTable*	mTable;
	fieldNumT mFieldNumber;
	dbCalculator* mCalculator;
	
private:
	OOF_IndexOptions mIndexOptions;
	unsigned short mPartialKeyLength;
	oofString mFieldName;
	bool mCalculateOnlyDefaults;
	bool mStoreCalculatedValues;

	friend	class dbTable;
};
typedef dbField *dbFieldPtr;
istream& operator>>(istream&, dbField*);
istream& operator>>(istream&, dbField&);


class dbQueryBinary;
class dbQueryTrinary;
class dbQueryBinaryCombo;
class oofWordParser;


/**
	Mixin class for fields that are keyword-indexed.
	\see OOF_ctreeKeywordIndex for implementation with the c-tree Plus
	backend in OOFILE Professional.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT OOF_mixKeywordableField {
public:
	OOF_mixKeywordableField(dbField*);
	OOF_mixKeywordableField(const OOF_mixKeywordableField& rhs, dbField* rhsAsField);
	~OOF_mixKeywordableField();
	
	void indexWords(oofWordParser*	adoptedParser = 0);
	oofWordParser*	wordParser() const;

	// searches
	dbQueryBinary	hasWord(const char* str) const;
	dbQueryBinary	hasAnyWordsDelimited(const char*, char delimiter);
	dbQueryBinary	hasAllWordsDelimited(const char*, char delimiter);
	dbQueryBinary	hasAnyWordsOf(const char **schStrs, unsigned long count);
	dbQueryBinary	hasAllWordsOf(const char **schStrs, unsigned long count);
	dbQueryBinary	hasAnyWordsOf(oofWordParser*);
	dbQueryBinary	hasAllWordsOf(oofWordParser*);

	dbQueryBinary	hasWordStartsWith(const char* str) const;
	dbQueryBinary	hasAnyWordsStartsWithDelimited(const char*, char delimiter);
	dbQueryBinary	hasAllWordsStartsWithDelimited(const char*, char delimiter);
	dbQueryBinary	hasAnyWordsStartsWithOf(const char **schStrs, unsigned long count);
	dbQueryBinary	hasAllWordsStartsWithOf(const char **schStrs, unsigned long count);
	dbQueryBinary	hasAnyWordsStartsWithOf(oofWordParser*);
	dbQueryBinary	hasAllWordsStartsWithOf(oofWordParser*);

protected:
// data storage
	oofWordParser*	mParser;	// owned
	dbField*		mThisField;
};


/**
	Calculator attached to a dbChar.
*/
class OOFILE_EXPORT dbCharCalculator : public dbCalculator {
public:
	virtual ~dbCharCalculator() {};
	virtual const char*	calc(const dbChar*)=0;
};



/**
	Calculator to strip <em>all</em> embedded characters.
	Defaults to stripping spaces.
	Use by attaching to a dbChar.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbCharStripperCalculator : public dbCharCalculator {
public:
	dbCharStripperCalculator(dbChar& stripsField, char toStrip = ' ');
		
	virtual const char*	calc(const dbChar*);
	virtual dbCalculator* clone() const;
	virtual dbCalculator* cloneOwned(dbField*) const;
	
private:
	dbChar*	mStripsField;
	char mStripChar;
	oofString mStrippedStr;
};


/**
	Persistent field used to store a fixed-length string.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbChar : public dbField, public OOF_mixKeywordableField  {
public:
// constructors
	dbChar(unsigned long fieldWidth=80, const char* fieldName=0);
	dbChar(unsigned long fieldWidth, const OOF_IndexOptions indexOptions);
	dbChar(unsigned long fieldWidth, const char *fieldName, const OOF_IndexOptions indexOptions);
 	dbChar(dbField::FieldIsStandaloneT k, unsigned long fieldWidth=80); 
	dbChar(const dbChar&);
	virtual ~dbChar();
	virtual dbField* clone() const;

	dbChar& calculateWith( dbCharCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbCharCalculator* calculator() const;

// reflective operators
	virtual bool fieldIsSameTypeAs(const dbField*) const;
	virtual void describe(ostream&) const;
	virtual OOF_fieldTypes fieldType() const;
	virtual bool fieldIsKeywordIndexed() const;
	virtual bool fieldIsBinary() const;
	virtual unsigned long fieldUsualWrapWidth() const;
	virtual unsigned long fieldStorageLen() const;
	virtual bool isEmpty() const;
	unsigned short length() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;

// search factories
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
	dbQueryBinary	startsWith(const char*) const;
//	dbQueryBinary	contains(const char*) const;
	dbQueryTrinary	between(const char*, const char*) const;
	dbQueryTrinary	outside(const char*, const char*) const;

// data access

// getters
	const char* value() const;
	virtual const char* asChars() const;
	virtual oofString copyString() const;
	virtual void getChars(char* ioBuff, unsigned long& ioLen) const;
	operator const char*() const;
	virtual void extract(ostream& os) const;
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);
	virtual oofWordParser* words() const;
#ifdef _Macintosh
	void getStr255(Str255 s) const;
#endif

// setters
#ifdef _Macintosh
	void setStr255(const Str255 s);
#endif
	virtual void clear();  
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	void setString(const oofString&);  
	dbChar& operator=(const char*);  
	dbChar& operator=(char);  
	void operator+=(char);  
	void operator+=(const char*);  
	void setChars(const char* str , unsigned long charLen);
	dbChar& operator=(const dbChar&);
	virtual bool insert(istream&, char fieldSep, char recSep);
	char& operator[](unsigned short);
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);

private:
	void CompleteInit();
	
protected:
	virtual void CopyCalculatedValueToBuffer();
// data storage
 unsigned long mMaxLength;
 
 friend class OOF_dbaseBackend;	// can fiddle our length, didn't want to grant public interface for that!
};


class dbBLOB;
/**
	Temporary cache field used to store dbBlob contents.
	Used partly so dbBlob const getters can delay loading without
	having to modify the dbBlob object itself.
	\ingroup oofDatabase
*/
class dbBLOBstorage {
private:  // owned totally by a dbBLOB, only exposed here to allow inline definitions
// constructors
	dbBLOBstorage(dbBLOB* owner);	
	dbBLOBstorage(const dbBLOBstorage&) { assert(0); };
	virtual ~dbBLOBstorage();

// reflective operators
	bool isDirty() const;

// data access
	char* bodyAddress() const;
#ifdef OOF_DebugDumpBlobState	
	void blobState(ostream& os) const;
#endif	
	void reset();
	char* allocRoomFor(unsigned long);
	char* orphanBody();
	void adoptBody(char* body, unsigned long bodyLen);
	void markDirty();
	void setChars(const char* str , unsigned long charLen);
	void setBytes(const char* buf , unsigned long bufLen);
	void convertLineEndings();

protected:
// data storage
	char*	mBody;	// owned
	unsigned long mBodyLength;  // allow for part-using the block
	bool mDirty,mLoaded;
	dbBLOB* mOwner;
	
friend class dbBLOB;
friend class dbText;
};


/**
	Persistent field used to store an arbitrary binary object.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbBLOB : public dbField {
public:
// constructors
	dbBLOB(const OOF_IndexOptions indexOptions);
	dbBLOB(const char* fieldName=0) ;
	dbBLOB(const char* fieldName, const OOF_IndexOptions indexOptions);
 	dbBLOB(dbField::FieldIsStandaloneT k); 
	dbBLOB(const dbBLOB&);
	virtual ~dbBLOB();
	virtual dbField* clone() const;
// debug stuff
#ifdef OOF_DebugDumpBlobState
   void blobState(ostream& os);
#endif
// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
	virtual bool isEmpty() const;
	virtual unsigned long length() const;
	virtual bool fieldIsBlob() const;
	bool isDirty() const;
	bool loaded() const;

// data access - all read ops are const because handled by separate storage
	virtual void extract(ostream& os) const;
	char* bodyAddress() const;
	void reset() const;
	char* allocRoomFor(unsigned long) const;
	char* orphanBody() const;
	void adoptBody(void* body, unsigned long bodyLen) const;	// used by backends
	void adoptBody(void* body, unsigned long bodyLen, unsigned long usedLen) const;
	void append(const dbBLOB&);
	virtual void append(const void* appendBody, unsigned long bodyLen, unsigned long usedLen=ULONG_MAX);
	void setBytes(const void* buf , unsigned long bufLen);
	void markDirty() const;
	dbBLOB& operator=(const dbBLOB&);
	void operator+=(const dbBLOB&);
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);

protected:
	 void SetLength(unsigned long len) const;
	void LoadField() const;
	
// data storage
	dbBLOBstorage*	mStorage;	// owned
	bool mIsTextField;  // used by dbText & other possible descendants to change behaviour
	
friend void dbBLOBstorage::setChars(const char* str , unsigned long charLen);
friend void dbBLOBstorage::setBytes(const char* buf , unsigned long bufLen);
};


/**
	Calculator attached to a dbText field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTextCalculator : public dbCalculator {
public:
	virtual ~dbTextCalculator() {};
	virtual const char*	calc(const dbText*)=0;

protected:
	oofString mBuffer;
};


/**
	Persistent field used to store a variable length string.
	Usually lazily evaluated, so only loaded on access to field rather
	than with main record.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbText : public dbBLOB, public OOF_mixKeywordableField {
public:
// constructors
	dbText(const OOF_IndexOptions);
	dbText(const char* fieldName=0);
	dbText(const char* fieldName, const OOF_IndexOptions);
 	dbText(dbField::FieldIsStandaloneT); 
	dbText(const dbText&);
	virtual ~dbText() {};
	virtual dbField* clone() const;

	dbText& calculateWith( dbTextCalculator* adoptedCalculator, bool useOnlyForDefaults=false);
	dbTextCalculator* calculator() const;

// global settings
	static void convertsLineEndingsOnLoad(bool willConvert);
	static bool convertsLineEndingsOnLoad();

// reflective operators
	virtual OOF_fieldTypes fieldType() const;
	virtual OOF_fieldTypes nativeType() const;
	virtual unsigned long fieldUsualWrapWidth() const;
	virtual bool fieldIsKeywordIndexed() const;
	virtual bool fieldIsBinary() const;
	

// search factories
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
//	dbQueryBinary	contains(const char*) const;

// data access
	const char* value() const;
	virtual dbQueryClause* valueAsQueryLiteral() const;
	virtual unsigned long length() const;
	virtual const char* asChars() const;
	virtual oofString copyString() const;
	virtual void getChars(char* ioBuff, unsigned long& outLen) const;
	operator const char*() const;
	virtual void clear();  
	virtual void setString(const char*);  
	virtual void setNumber(long);  
	void setString(const oofString&);  
	dbText& operator=(char); 
	dbText& operator=(const char*); 
	dbText& operator=(const dbText&);
	void operator+=(char);  
	void operator+=(const char*);  
	virtual void append(const void* appendBody, unsigned long bodyLen=ULONG_MAX, unsigned long usedLen=ULONG_MAX);
	void setChars(const char* str , unsigned long charLen);
	void adoptString(oofString& adoptedString) const;
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);
	virtual void extract(ostream& os) const;
	virtual bool insert(istream&, char fieldSep, char recSep);
	virtual void copyValueFrom(const dbField*);
	virtual void copyValueIfDifferent(const dbField*);
	virtual oofWordParser* words() const;

		
protected:
	virtual void CopyCalculatedValueToBuffer();
	static bool sConvertsLineEndingsOnLoad;  // = false
};


/**
	Persistent field used to store a fixed-length binary object.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbFixedBinary : public dbField {
public:
// constructors
	dbFixedBinary(long fieldWidth=4, const char* fieldName=0);  
	dbFixedBinary(long fieldWidth, const OOF_IndexOptions indexOptions); 
	dbFixedBinary(long fieldWidth, const char* fieldName, const OOF_IndexOptions indexOptions);  
	// use default copy ctor
	dbFixedBinary& operator=(const dbFixedBinary& rhs);
 	dbFixedBinary(dbField::FieldIsStandaloneT k) : dbField(k) {}; 
	virtual ~dbFixedBinary() {};
	virtual dbField* clone() const;

// reflective operators
	virtual bool fieldIsSameTypeAs(const dbField*) const;
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
	
// data access
	dbFixedBinary&  operator=(const char* str);
	virtual void setString(const char*);  
	void setValue(const void* str);
	void setByte(char c, unsigned long offset);
	void clear();
	virtual void extract(ostream& os) const;
	virtual void generateTestData(bool generateRandom, unsigned long seedOrCount);
	virtual void describe(ostream& os) const;

protected:
	
// data storage
	unsigned long mBodyLength; 
	

};


/**
	Persistent field used to store a set of segments referring to other fields.
	Most often used to declare a compound index.
	Unless indexed, often will not actually allocate storage in the database.
	Read-only field.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbCompoundField : public dbField, public OOF_FieldSegOwner {
public:
// constructors
	dbCompoundField(const OOF_IndexOptions);
	dbCompoundField(const char* fieldName=0);
	dbCompoundField(const char* fieldName, const OOF_IndexOptions);
	dbCompoundField(const dbCompoundField&);
	virtual ~dbCompoundField() {};
	virtual dbField* clone() const;

	virtual void addSegment(dbField&);
		
// data output
	virtual void extract(ostream& os) const;

// reflective operators
	virtual bool fieldIsSameTypeAs(const dbField*) const;
	virtual OOF_fieldTypes fieldType() const;
	virtual unsigned long fieldStorageLen() const;
	virtual bool fieldIsVirtual() const;

	const dbField* field(unsigned int) const;

// search factories
	dbQueryBinary	operator==(const char*) const;
	dbQueryBinary	operator<(const char*) const;
	dbQueryBinary	operator<=(const char*) const;
	dbQueryBinary	operator>=(const char*) const;
	dbQueryBinary	operator>(const char*) const;
	dbQueryBinary	operator!=(const char*) const;
	dbQueryBinary	startsWith(const char* str) const;

// data storage
private:
	unsigned int mTotalLength;
};


/**
	Utility class establishing copy dependency between fields.
	Not used much at present. May be vulnerable to cloning of dbFields so the
	field you specified and think is being updated is not the one connected to a GUI.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbFieldCopier : public oofSingleListener {
public:
	dbFieldCopier(oofBroadcaster* listensTo, unsigned short copyOnMessage, dbField* copyFrom, dbField* copyTo);
	// use default copy ctor
	virtual ~dbFieldCopier() {};
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
protected:
// data storage
	dbField *mCopyFrom, *mCopyTo;
	OOFmsgT mCopyOnMessage;
};


/**
	Internal class used to specify segments for dbCompoundField and dbSorter.
	\ingroup oofDatabase
*/
class OOF_Segment : public OOF_PublicBase 
{
private:  // yep, wholly owned
	OOF_Segment(const dbField& fld);
	OOF_Segment(const dbField* fld);
// use default copy ctor
	~OOF_Segment() {};

public:
	void setReverse();
	bool isReversed() const;
	bool operator!=(const OOF_Segment* rhs) const;
	
	
private:	
// data storage
	fieldNumT mFieldNo;
	bool mReversed;
	
friend class OOF_FieldSegOwner;
};


// -------------------------------------------------------
//                   d b F i e l d
// -------------------------------------------------------
#if 0
inline dbField::operator dbField*() 
{ 
	return this; 
};
#endif

inline void 
dbField::operator=(const char* str)
{
	setString(str);  // this is a virtual call - a way to get a virtual operator= without the hassles of overriding
}


inline OOF_IndexOptions 
dbField::fieldIndexOptions() const
{
	return mIndexOptions;
}


inline void 
dbField::indexPartialKeyLen(unsigned short inLen)
{
	mPartialKeyLength = inLen;
}


inline bool 
dbField::fieldIsPartiallyIndexed() const
{
	return (mPartialKeyLength!=kKeyLengthAutoCalculated);
}


inline bool 
dbField::fieldIndexIgnoresNulls() const
{
	bool ret = ((mIndexOptions & kIndexNoNulls)!=0);
	return ret;
} 


inline bool 
dbField::fieldIndexIsCompressLeading() const
{
	bool ret = ((mIndexOptions & kIndexCompressLeading)!=0);
	return ret;
}


inline bool 
dbField::fieldIndexIsCompressPadding() const
{
	bool ret = ((mIndexOptions & kIndexCompressPadding)!=0);
	return ret;
}


inline unsigned short 
dbField::fieldNumIndexes() const
{
	if (mIndexOptions==kNotIndexed)
		return 0;
	else
		return 1;  // NOT YET IMPLEMENTED - may later have multiple indexes/field
}


inline bool 
dbField::fieldIsStandalone() const
{
	return (mTable==0);
}


/* inline 
dbField::operator oofString() const
{
	return copyString();
}
*/

inline bool 
dbField::caseSensitive() const
{
	return ((mIndexOptions & kIndexCaseSensitive)!=0);  
}


inline fieldNumT 
dbField::fieldNumber() const
{
	return mFieldNumber;  
}


inline unsigned long 
dbField::fieldDataLen() const
{
	assert(!fieldIsStandalone());
	return mBackend->fieldDataLen(this);  	
}


inline const oofString&
dbField::fieldName() const
{
	return mFieldName;  
}


inline const void* 
dbField::currentbinaryContents() const
{
	assert(!fieldIsStandalone());
	return mBackend->getFieldReadFrom(mFieldNumber);
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbField& fld)
{
	fld.extract(os);
	return os;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbField* fld)
{
	fld->extract(os);
	return os;
}


inline bool 
dbField::hasDefaultCalculator() const
{
	return (mCalculateOnlyDefaults);
}


inline void 
dbField::useCalculatorOnlyForDefaults()
{
	mCalculateOnlyDefaults = true;
}


inline bool 
dbField::storesCalculatedValues() const
{
	return mStoreCalculatedValues;
}


inline void 
dbField::storeCalculatedValues()
{
	mStoreCalculatedValues = true;
}


// -------------------------------------------------------
//                      d b C h a r
// -------------------------------------------------------
// friend
OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbChar& fld)
{
	fld.extract(os);
	return os;
}


inline void 
dbChar::setString(const oofString& str)
{
	setChars(str, str.length());
}


inline const char* 
dbChar::value() const
{
	return (const char*)*this;
}


inline dbCharCalculator* 
dbChar::calculator() const
{
	return (dbCharCalculator*) mCalculator;	// safe downcast
}



// -------------------------------------------------------
//                    d b B L O B 
// -------------------------------------------------------
inline bool 
dbBLOB::isDirty() const
{
	return mStorage->isDirty();
}


inline void 
dbBLOB::markDirty() const
{
	mStorage->markDirty();
}


inline bool 
dbBLOB::loaded() const
{
	return mStorage->mLoaded;
}


inline void 
dbBLOB::reset() const
{
	mStorage->reset();
}


inline char* 
dbBLOB::allocRoomFor(unsigned long len) const
{
	SetLength(len);
	return mStorage->allocRoomFor(len);
}


inline char* 
dbBLOB::orphanBody() const
{
	return mStorage->orphanBody();
}


// -------------------------------------------------------
//                      d b T e x t
// -------------------------------------------------------
OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, const dbText& fld)
{
	fld.extract(os);
	return os;
}


inline void 
dbText::convertsLineEndingsOnLoad(bool willConvert)
{
	sConvertsLineEndingsOnLoad = willConvert;
}


inline bool 
dbText::convertsLineEndingsOnLoad()
{
	return sConvertsLineEndingsOnLoad;
}


inline void 
dbText::setString(const oofString& str)
{
	setChars(str, str.length());
}


inline const char* 
dbText::value() const
{
	return (const char*)*this;
}


inline dbTextCalculator* 
dbText::calculator() const
{
	return (dbTextCalculator*) mCalculator;	// safe downcast
}



// -------------------------------------------------------
//               O O F _ S e g m e n t  
// -------------------------------------------------------

inline void 
OOF_Segment::setReverse()
{
	mReversed = true;
}


inline bool 
OOF_Segment::isReversed() const
{
	return mReversed;
}
 
 
inline bool 
OOF_Segment::operator!=(const OOF_Segment* rhs) const
{
	bool ret = ((mFieldNo != rhs->mFieldNo) || (mReversed != rhs->mReversed));
	return ret;
}

 
#endif

