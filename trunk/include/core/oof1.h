#ifndef H_OOF1
#define H_OOF1

// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database

// NOTE inline definitions included at end of this header file

// see misc.txt for code conventions
#ifndef H_OOF0
	#include "oof0.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif
#ifdef _Macintosh
	#include <Types.h>
	#include <Events.h>
	#include <Files.h>
#endif
#if OOF_USE_ANSI_STREAMS && !defined(H_OOFIOS)
	#include "oofios.h"
#endif
#ifndef H_OOFSTR
	#include "oofstr.h"
#endif


class dbQueryBinaryNofield;
class oofE_Base;

/**
	Base class used to refer to main OOFILE classes and provide base reflective API.
	Implements member mHidden for marking objects as hidden, eg: so dbField members
	may not appear in dbTable::extract().
	\ingroup oofDatabase
*/ 
class OOFILE_EXPORT OOF_PublicBase {
public:
	bool hidden() const { return mHidden; };
	void hide(bool hideIt=true) { mHidden = hideIt; };
#ifndef	OOF_NO_STDLIB
	virtual void describe(ostream&) const;
#endif
	virtual oofString name() const;
	virtual ~OOF_PublicBase() {};

protected:
	OOF_PublicBase() : mHidden(false) {};
	OOF_PublicBase(const OOF_PublicBase& rhs) : mHidden(rhs.mHidden) {};
	
	bool mHidden;
};

 /**
 	Base class typedef to make dictionaries easier to declare.
	\ingroup oofUtils
 */
typedef OOF_PublicBase* OOF_bitPointer;


/**
	Mixin for reference counted classes.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_mixRefCount {
public:
	void incRefs();
	void decRefs();
protected:
	OOF_mixRefCount(unsigned long initialRefs = 1);
	virtual ~OOF_mixRefCount();
// use default copy ctor
	unsigned int mReferences;
};


/**
	Representation of ref-counted dictionary.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_DictRep : public OOF_mixRefCount {
private:
	OOF_DictRep(unsigned int numSlots, unsigned int expandBySlots);
	OOF_DictRep(const OOF_DictRep*);
	~OOF_DictRep();
	void operator=(const OOF_DictRep&) {assert(0);};

// access protocol
	void Append(OOF_bitPointer);
	OOF_bitPointer Pop();
	
	OOF_bitPointer& operator[](unsigned int index); 
	OOF_bitPointer& operator[](const char *name);
	OOF_bitPointer& lookupByAttribute(const char* attributeValue, const char* attributeType=0);
	OOF_bitPointer& item(unsigned int index);
	OOF_bitPointer value(unsigned int index) const;
	void Reset();
	void ExpandToInclude(unsigned int indexToCover);

// data storage
	enum {kSlotSize=sizeof(OOF_bitPointer)};
	OOF_bitPointer *mBits;	// owned
	unsigned int mExpansionChunk, mNextFreeEntry, mNumSlots;

	friend class OOF_Dictionary;
	friend class dbView;
	friend class dbRelChain;
};


/**
	Provides searchable dictionary of common OOFILE classes like dbTable.
	a Handle or Envelope class for OOF_DictRep, independently acting as an iterator.
	contains list of OOF_PublicBase and allows lookup by name and 
	\todo lookup by other attributes
	\see OOF_Dictionary::lookupByAttribute 
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_Dictionary {  
// default constructor, or passing in zero items  skips allocating storage
// and storage will be allocated later as necessary.
public:
	enum {kDefExpansionChunk=10};
	
	OOF_Dictionary() ;
	OOF_Dictionary(const OOF_DictRep* rep);
	OOF_Dictionary(unsigned int numSlots);
	OOF_Dictionary(unsigned int numSlots, unsigned int expansionChunk, bool inOwnsContents=false);
	OOF_Dictionary(const OOF_Dictionary&);  
	OOF_Dictionary clone() const;
	virtual ~OOF_Dictionary();
	OOF_Dictionary& operator=(const OOF_Dictionary&);	

// access operators
	OOF_bitPointer& operator[](unsigned int); 
	OOF_bitPointer& operator[](const char*);  // may return OOF_Dictionary::missingItem() if fails to match name
	OOF_bitPointer& lookupByAttribute(const char* attributeValue, const char* attributeType=0);

	OOF_bitPointer& item(unsigned int index);
	OOF_bitPointer value(unsigned int index) const;
	OOF_bitPointer& operator()();
	
	void append(OOF_bitPointer);
	OOF_bitPointer pop();  // returns last item, maybe 0
	
	void reset();
	
	// iterator protocol
	unsigned int iter() const;
	void start();
	bool more() const;
	void next();
	unsigned int count() const;

	void startVisible(bool wantVisible=true);
	bool moreVisible(bool wantVisible=true);
	void nextVisible(bool wantVisible=true);
	unsigned int countVisible(bool wantVisible=true);

// other protocol
	bool allObjectsMatch(OOF_Dictionary&);
	void deleteAll();
#ifndef	OOF_NO_STDLIB
	virtual void describe(ostream&) const;
	virtual void describeVisible(ostream&, bool wantVisible) const;
#endif
	void ownsContents(bool ownsFlag=true);

protected:
	bool EntryMatchesVisibility(const OOF_PublicBase*, bool) const;

// data storage
	OOF_DictRep *mRep;	// owned
	unsigned int mInternalIter;
	bool mOwnsContents;
	static OOF_bitPointer sMissingItem;
	
public:
	static OOF_bitPointer& missingItem();
};


/**
	type we pass around pretending we have a real OID.
	When we move to supporting HUGE Faircom files with 8-byte pointers, 
	may become a real class
	\ingroup oofDatabase
*/
typedef unsigned long oidT;


/**
	type used to specify index options for dbField's.
	\ingroup oofDatabase
*/
enum OOF_IndexOptions { kNotIndexed=0, kIndexed=1, kIndexNoDups=2, kIndexCaseSensitive=4,
						kIndexCompressLeading=8, kIndexCompressPadding=16, kIndexNoNulls=32,
						kIndexCompress = kIndexCompressLeading + kIndexCompressPadding,
						kIndexNoDupsNoNulls = kIndexNoDups + kIndexNoNulls,
						kIndexCompressNoDups = kIndexCompress + kIndexNoDups,
						kIndexCompressNoNulls = kIndexCompress + kIndexNoNulls,
						kIndexCompressNoDupsNoNulls = kIndexCompress + kIndexNoDups + kIndexNoNulls,
						kIndexCompressLeadingNoDups = kIndexCompressLeading + kIndexNoDups,
						kIndexCompressLeadingNoDupsNoNulls = kIndexCompressLeadingNoDups + kIndexNoNulls,
						kIndexCompressPaddingNoDups = kIndexCompressPadding + kIndexNoDups,
						kIndexCompressPaddingNoDupsNoNulls = kIndexCompressPaddingNoDups + kIndexNoNulls
						}; 
/* NEW VERSION
enum OOF_IndexOptions { kNotIndexed=0, kIndexed=1, kIndexedNoDups=2, kIndexedCaseSensitive=4,
												kIndexedCompressLeading=8, kIndexedCompressPadding=16,
												kIndexedCompress = kIndexedCompressLeading + kIndexedCompressPadding,
												kIndexedCompressNoDups = kIndexedCompress + kIndexedNoDups,
												kIndexedCompressLeadingNoDups = kIndexedCompressLeading + kIndexedNoDups,
												kIndexedCompressPaddingNoDups = kIndexedCompressPadding + kIndexedNoDups
											};
											*/
// for backwards compatability	and to balance things out so we have both a verb and noun form
// (the original one is as follows except for kNotIndexed and kIndexed
/*enum OOF_IndexOptions2 { kNotIndex=0, kIndex=1, kIndexNoDups=2, kIndexCaseSensitive=4,
												kIndexCompressLeading=8, kIndexCompressPadding=16,
												kIndexCompress = kIndexCompressLeading + kIndexCompressPadding,
												kIndexCompressNoDups = kIndexCompress + kIndexNoDups,
												kIndexCompressLeadingNoDups = kIndexCompressLeading + kIndexNoDups,
												kIndexCompressPaddingNoDups = kIndexCompressPadding + kIndexNoDups
											};
											*/
OOFILE_EXPORT ostream& operator<<(ostream&, OOF_IndexOptions);


#if defined(_MSC_VER) && !defined(__MWERKS__)
	// VC5 considers field(0) ambiguous - sloppy evaluation of overloads!!!
	typedef  int tableNumT;  
	typedef  int fieldNumT;  
#else
	typedef  unsigned short tableNumT;  // never more than 64k classes in database!
	typedef  unsigned short fieldNumT;  // never more than 64k fields in a record!
#endif

class dbQueryClause;
class dbRelRefBase;
class dbField;
class OOF_tableBackend;
class dbContext;
class dbConnect;
class OOF_ExpandableLongArray;
class dbSorter;
class OOF_Selection;
class dbTable;
class dbGUI;
class oofWordParser;
class oofDirectory;


/**
	Abstract factory for dbGUI.
	\ingroup oofGUI
*/
class OOFILE_EXPORT dbGUIFactory {
public:
	dbGUIFactory();
	virtual dbGUI*  concreteMakeGUI(dbTable*)=0;	///< Abstract factory impl.
	static dbGUI*  makeGUI(dbTable*);		///< Abstract factory interface

protected:
	static dbGUIFactory* sGUIFactory;
	
};


/**
	Envelope class to contain an abstract selection apart from its dbTable.
	Useful for set manipulations as a lighter way to temporarily save and
	restore a set, or retain for other manipulation.
	\ingroup oofDatabase
*/
class  OOFILE_EXPORT dbSelection {  
// public interface to selections
public:
	dbSelection();
	dbSelection(const OOF_Selection*);
	dbSelection(const dbSelection&);
	~dbSelection();

	dbSelection& operator=(const dbSelection&);
	dbSelection& operator=(dbTable&);
	

// combinatorial operators
	void difference_with(const dbSelection&);
	void difference_with(dbTable&);
	void difference_with(dbTable*);
	void operator -=(const dbSelection&);
	void operator -=(dbTable&);
	void operator -=(dbTable*);
	void operator %=(const dbSelection&);
	void operator %=(dbTable&);
	void operator %=(dbTable*);
	
	void intersection_with(const dbSelection&);
	void intersection_with(dbTable&); 
	void intersection_with(dbTable*); 
	void operator &=(const dbSelection&);
	void operator &=(dbTable&);
	void operator &=(dbTable*);
	
	void union_with(const dbSelection&);
	void union_with(dbTable&);
	void union_with(dbTable*);
	void union_with_no_overlap(const dbSelection&);
	void union_with_no_overlap(dbTable&);
	void union_with_no_overlap(dbTable*);
	void operator +=(const dbSelection&);
	void operator +=(dbTable&);
	void operator +=(dbTable*);
	void operator |=(const dbSelection&);
	void operator |=(dbTable&);
	void operator |=(dbTable*);
	
	void union_with(oidT);
	void operator+=(oidT);
	void intersection_with(oidT);
	
	void invert();
	void operator~();
	void operator!();

// reflective operators  
	bool isEmpty() const;
	bool isAllRecs() const;
	bool contains(oidT) const;
	unsigned long count() const;
	const OOF_Selection* internalSelection() const;
	const dbTable* prototypicalTable() const;
	
private:
	void DeleteContents();

// data storage
	OOF_Selection*	mSelection;	// owned
	unsigned long*	mReferences;
};


/**
	Save & restore current database selection on the stack.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT stSaveSelection {
public:
	stSaveSelection(dbTable*);
	stSaveSelection(dbTable&);
	~stSaveSelection();
	
protected:
	dbTable*	mTable;
	dbSelection	mSavedSelection;
};


/**
	Save and restore state of ignoring duplicates.
	Mainly used when importing data.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT stIgnoreDuplicateRecords {
public:
	stIgnoreDuplicateRecords(dbTable*);
	stIgnoreDuplicateRecords(dbTable&);
	~stIgnoreDuplicateRecords();
	
protected:
	dbTable*	mTable;
};


/**
	Helper class for dbTable to easily assign field values;
	Provides idiom of 
	People << "Andy" << "Dent" << "37";
	instead of
	People.newRecord();
	People.FirstName = "Andy";
	People.LastName = "Dent";
	People.Age = 37;
	People.saveRecord();
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTableAssigner {
public:
	dbTableAssigner(dbTable*, const char* inFirstFieldValue);
	~dbTableAssigner();
	
	dbTableAssigner& operator<<(const char*);
	void setNextFieldString(const char*);
	
protected:
	dbTable*	mTable;
	fieldNumT	mNextFieldNum;
};


/**
	Base class for persistent tables.
	Must be used in association with a dbConnect subclass to provide persistence.
	Contains one or more dbField members either by declaration or attachField().
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTable : public OOF_PublicBase, public oofBroadcaster {
public:
	dbTable(const char *name=0);  
	dbTable(const dbTable&);
	dbTable(const dbTable&, OOF_tableBackend*);
	virtual ~dbTable();
	void stillCurrentlyConstructing();
	dbTable* prototype() const;

	enum {kFieldSep = '\t'};
	enum {kRecSep = '\n'};
	enum selSharingT {selNotShared, selShared, selCopyOnWrite,selNotSharedCacheCopied};
	enum saveOptionsT{requireExplicit, autoOnContextChange, requireExplicitAndBuffer, bufferForever};
	enum tableValidT {eInvalid=0, eValid, eClosed};  // use invalid=0 so can cope if coerce false
	
	virtual const char* defaultName() const { return ""; };  // gets overridden by the methods generated by ABSTRACT_BASE_METHODS in oofmacro.h
	virtual oofString name() const;
	void setName(const char*);
	virtual dbTable* cloneTableSharingSelection() const;
	virtual dbTable* cloneTableWithoutSelection() const;
	virtual dbTable* cloneTbleWithCachNoSelection() const; 


	virtual dbTable* cloneTable(selSharingT selSharing=selCopyOnWrite)=0;  
	virtual dbTable* cloneTableConst(selSharingT selSharing) const=0;  
	// pure virtual cloneTable is implemented by macro OOFILE_METHODS, in oofmacro.h, for each user subclass
	// abstract to avoid bugs from people missing their OOFILE_METHODS in table declarations

	void postCloneTableCleanup();
//	operator dbTable*();

	virtual dbView* makeView();  // can't be const because the view retains a pointer to us

	bool ensureRecordLoaded(bool autoCreateRelated=false, bool failureAborts=true);
	void ensureSelectionLoaded();
	void relateAllDeletables();
	
	// hiding oid's which may be record offsets or generated unique numbers
	oidT currentOID();
	oidT oidOfRecord(unsigned long);
	
// other protocol
	bool loadRelatedContextJoiningFromTo(const dbField*, const dbField*);

	
	oofWordParser* getDefaultWordParser();
	void setDefaultWordParser(oofWordParser* inParser) ;

	void setSaveOption(const saveOptionsT);
	void setFileExtensions(const char* dataExt, const char* indexExt);
	dbField* field(fieldNumT) const;
	dbField* field(const oofString& );
	
	bool pointsToBackend(const OOF_tableBackend*) const;
	virtual bool canSaveRecord();  // expect user override in many cases

	void attachfield(dbField*);
	void buildSchema();
	void addIndices(const char* newIDXname);
	void suppressIndices();
	void rebuild();
	void close();
	void deleteStorage();
	void createTableInConnection(const dbConnect*);
	bool openTableInConnection(const dbConnect*, const bool createIfMissing = false);
	unsigned int numIndexes() const;
	unsigned int numFiles() const;
	unsigned long sequenceNumber() const;
	void setTableValid(bool isValid=true);
	void pack();
		
// recordwise access
	void start();
	void next();
	void first();
	void prev();
 	void last();

	bool more() const;
	bool atFirst() const;
	bool atLast() const;
	bool atRelative(unsigned long) const;

	unsigned long count();
	unsigned long countAll() const;

/// \name sorting	
//@{
	void setSortOrder(const dbField&, bool shouldReverse=false);
	void setSortOrder(const dbSorter&);
	void setReverseSortOrder(const dbField&);
	dbSorter* sorter() const;
	void sortNow() const;
	void unSorted();
	void suspendSorting();
	void resumeSorting();
//@}

	void unloadRecord();
	bool reloadRecord();
	bool isRecordLoaded() const;
	bool isEmpty();
	bool isNewRecord() const;
	bool isTableValid() const;
	tableValidT validStatus() const;
	int validatingDepth() const;

	bool isDirty() const;
	void markDirty();
	
	void ignoreDuplicateRecords();
	void noticeDuplicateRecords();

    unsigned long recordNumber() const;
    unsigned long recordNumberOfOID(oidT inOID);
	unsigned long recordNumber(const dbQueryClause* qClause, bool* outFoundIt=0) const;
	unsigned long recordNumber(const dbQueryClause& qClause, bool* outFoundIt=0) const;
	bool gotoRecord(unsigned long);

/// \name keyword searches	
//@{
	dbQueryBinaryNofield	hasWord(const char* str) const;
	dbQueryBinaryNofield	hasAnyWordsDelimited(const char*, char delimiter);
	dbQueryBinaryNofield	hasAllWordsDelimited(const char*, char delimiter);
	dbQueryBinaryNofield	hasAnyWordsOf(const char **schStrs, unsigned long count);
	dbQueryBinaryNofield	hasAllWordsOf(const char **schStrs, unsigned long count);
	dbQueryBinaryNofield	hasAnyWordsOf(oofWordParser*);
	dbQueryBinaryNofield	hasAllWordsOf(oofWordParser*);

	dbQueryBinaryNofield	hasWordStartsWith(const char* str) const;
	dbQueryBinaryNofield	hasAnyWordsStartsWithDelimited(const char*, char delimiter);
	dbQueryBinaryNofield	hasAllWordsStartsWithDelimited(const char*, char delimiter);
	dbQueryBinaryNofield	hasAnyWordsStartsWithOf(const char **schStrs, unsigned long count);
	dbQueryBinaryNofield	hasAllWordsStartsWithOf(const char **schStrs, unsigned long count);
	dbQueryBinaryNofield	hasAnyWordsStartsWithOf(oofWordParser*);
	dbQueryBinaryNofield	hasAllWordsStartsWithOf(oofWordParser*);
//@}

/// \name public search functions	
//@{
/// \todo	bool searchDefaultIndex(const char*);
	bool search(const dbQueryClause& query);
	bool searchSelection(const dbQueryClause& query);
	bool search(const dbQueryClause* query);
	bool searchSelection(const dbQueryClause* query);
	bool searchSelContainsAnyDelimited(const dbField& schField, const char *schStr, char delim);
	bool searchSelContainsAnyOf(const dbField& schField, const char **schStrs, unsigned long count);
	bool searchSelContainsAnyOf(const dbField& schField, oofWordParser*);
	bool searchSelContainsAllDelimited(const dbField& schField, const char *schStr, char delim);
	bool searchSelContainsAllOf(const dbField& schField, const char **schStrs, unsigned long count);
	bool searchSelContainsAllOf(const dbField& schField, oofWordParser*);
//@}

	
/// \name set operations
//@{
	void difference_with(dbTable&);
	void difference_with(dbTable*);
	void difference_with(const dbSelection&);
	void operator -=(dbTable&);
	void operator -=(dbTable*);
	void operator -=(const dbSelection&);
	void operator %=(dbTable&);
	void operator %=(dbTable*);
	void operator %=(const dbSelection&);
	
	void intersection_with(dbTable&); 
	void intersection_with(dbTable*); 
	void intersection_with(const dbSelection&); 
	void operator &=(dbTable&);
	void operator &=(dbTable*);
	void operator &=(const dbSelection&);
	
	void union_with(dbTable&);
	void union_with(dbTable*);
	void union_with(const dbSelection&);
	void union_with_no_overlap(dbTable&);
	void union_with_no_overlap(dbTable*);
	void union_with_no_overlap(const dbSelection&);
	void operator +=(dbTable&);
	void operator +=(dbTable*);
	void operator +=(const dbSelection&);
	void operator |=(dbTable&);
	void operator |=(dbTable*);
	void operator |=(const dbSelection&);
	
	void invert();
	void operator~();
	void operator!();
	
	bool operator+=(oidT);
	bool addOIDtoSelection(oidT);
	bool contains(oidT);
	void dropRecordFromSelection(unsigned long);
//@}
	
/// \name getting & saving selections as lightweight objects
//@{
	dbSelection currentSelection();
	dbSelection makeEmptySelection() const;
	void setSelection(const dbSelection&);
	void setSelection(const OOF_Selection*);
	const dbSelection& operator=(const dbSelection& rhs);
//@}

// selection changes that broadcast, eg: to update MN relationship links
/// \name getting & saving selections as lightweight objects
//@{
	void appendSelection(const dbSelection&);
	void removeSelection(const dbSelection&);
	void clearSelection();
//@}
	
/// \name selection of specific contexts
//@{
	void selectAll();
	void selectNone();
	bool selectJustOID(oidT);  
	void selectJustCurrent();
	bool relateSelectionFrom(dbTable*);
	bool selectAllRelated(bool broadcastChange=true);
//@}	

/// \name data access
//@{
	virtual void newRecord();
	virtual void cloneRecord();
	virtual void postCloneRecordCleanup();
	virtual void saveRecord();
	virtual void revertRecord();
	virtual void deleteRecord();
	void deleteSelection();
	void deleteAll();
	void copyAllFrom(dbTable*);
	dbTableAssigner operator<<(const char*);
	
	void copyRecFrom(const dbTable* rhs, fieldNumT numFields=0);
	dbTable& operator=(const dbTable&);
	
	virtual void extract(ostream&);
	virtual unsigned long insert(istream&);
	virtual void import(const char* ansiFileName);
	bool allowsShortImports() const;
	void setShortImports(bool);
//@}

/// \name reflective operators
//@{
	const oofString& tableName() const;
	virtual oofString tableSingularName() const;  // eg: returns Student if you have name Students
	tableNumT tableNumber() const;
	bool tableCachesDirtyRecords() const;
	saveOptionsT tableSaveOption() const;
	virtual void describe(ostream&) const;
	virtual void describeState(ostream&) const;
	virtual void stats(ostream&);
	bool isCopyCompatibleWith(const dbTable*) const;
	fieldNumT numFields() const;
	bool allSelected() const;
	bool isOpen() const;
	bool inDeclarativePhase() const;
//@}

// \name info about relationships
//@{
	bool isRelatedClone() const;
	dbRelRefBase* controllingRelationship() const;
	dbTable* lhsTableInRelationshipSpec() const;
	dbTable* baseTableOfRelationChain();
//@}

	dbGUI* getGUI();

	virtual void generateTestData(unsigned long numRecords, bool generateRandom=true, unsigned long seedOrCount=0);
	
// \name locking
//@{
	void lockRecord();	// no return type - throws exception
	void unlockRecord();
	bool lockedCurrentRecord() const;
//@}


// \name transactions
//@{
	void beginTransaction(short tMode =0);
	void abortTransaction(short rMode =0);
	void commitTransaction(short rMode =0);
	bool transactionDirty();
//@}

#ifndef OOF_DISALLOW_CHANGE_SHIELDING
// change shielding, so field changes don't mark the record as dirty (default is false)
	bool changeShielded() const;
	void changeShielded(bool setShielding=true);
#endif	
		
 
void describeInvariant(ostream& os) const;	//To pass in an output stream as a parameter 	

protected:
#ifdef OOF_DEBUG_LOG_NAME
	void StandardDebugLog(const char*) const;
	void StandardQueryDebugLog(const char*, const dbQueryClause&) const;
#endif
	
private:
	void CompleteField(dbField*);
	void ContextChange();
	void MakeTableValid();
	void LoadRelatedContext();
	void SaveLoop();

// data storage
protected:
	OOF_tableBackend *mBackend;  	// owned  (the bit that does all the work!!)
	OOF_Dictionary	mFields;
	oofString mTableName;
	tableNumT	mTableNumber;
	fieldNumT mFieldCount;
	saveOptionsT mSaveOption;
	bool mAllowShortImports;
	tableValidT mTableValid;
	int mValidatingDepth;
	dbGUI*	mGUI;		// owned
	
private:
	dbRelRefBase* mRelToLHS;  // if we are a clone managing a related selection, this is how we get back
	oofWordParser*	mDefaultPrototypeParser;
	dbConnect* mConnection;
	dbTable*	mPrototype;
	bool mLockedCurrentRecord;
	bool mInTransaction;
	bool mTransactionDirty;
	
	bool mInSaveLoop;
#ifndef OOF_DISALLOW_CHANGE_SHIELDING
	bool	mChangeShielded;
#endif	
	
	static dbGUIFactory* sGUIFactory;
	
protected:
	static dbTable *sCurrentlyConstructing;
	
	friend class dbField;
	friend class dbConnect;
	friend class dbConnect_ram;
	friend class dbConnect_ramp;
	friend class dbRelHalf;
	friend class dbRelRefBase;
	friend class OOF_tableBackend;
	friend void dbSelection::difference_with(dbTable&);
	friend class oofE_Table;
};  // dbTable
OOFILE_EXPORT ostream& operator<<(ostream& os, dbTable& tbl);
OOFILE_EXPORT ostream& operator<<(ostream& os, dbTable* tbl);
OOFILE_EXPORT istream& operator>>(istream& is, dbTable* tbl);
OOFILE_EXPORT istream& operator>>(istream& is, dbTable& tbl);


/**
	use for dynamic creation of fields attached to table.
	eg: in oofXMLdb parsing a schema from XML
	\todo add OOFILE_METHODS and then can instantiate directly instead of trivial subclasses.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbTableOwningFields : public dbTable {
private:
	void operator=(const dbTableOwningFields&) { ; };  // can't assign
	
public:
	dbTableOwningFields(const char* name=0);  
	dbTableOwningFields(const dbTable&, OOF_tableBackend*);
	dbTableOwningFields(const dbTableOwningFields&);
	// uses default dtor	
	void adoptField(dbField* adoptedField);

protected:
	OOF_Dictionary mOwnedFields;
};


/**
	Separate out statics from dbTable.
	Use separate public class as some old compilers don't support nested decls.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT OOF_dbTableStatics {
public:  // for convenience but really only update by dbTable or subclasses
	static bool sCloning;
	static bool sCloningInDifferentConnection;
	static dbTable::selSharingT sCloningSelSharing;
};


/**
	Base class used mainly when exceptions not available.
	\see dbConnect::raise() for calling.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofErrorReporter {
public:
	oofErrorReporter();
	virtual ~oofErrorReporter();
	
// default implementations
	virtual void raise(ostream&, bool terminateAfterMsg=true);
	virtual void raise(const char*, bool terminateAfterMsg=true);

	static oofErrorReporter*  defaultReporter();
	static void setDefaultReporter(oofErrorReporter*);

protected:
	virtual void MaybeTerminate(bool terminateAfterMsg);
	
private:
	static oofErrorReporter	sDefaultReporter;	// concrete default
	static unsigned short 	sNumReporters;
	static oofErrorReporter*	sUserSpecifiedReporter;
};


/**
	Reporter that formats output for HTML responses.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofCGIErrorReporter : public oofErrorReporter {
public:
	oofCGIErrorReporter(ostream&);
	virtual ~oofCGIErrorReporter();
	
	virtual void raise(ostream&, bool terminateAfterMsg=true);
	virtual void raise(const char*, bool terminateAfterMsg=true);

protected:
	virtual void Header();
	virtual void Footer();
	
private:
	ostream&	mStream;
};


/**
	Abstraction of common runtime environment stuff you'd call.
	Mainly used to identify current runtime environment.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofRuntimeEnv {
public:
	enum oofRuntimeEnvT {Macintosh, OS2, Unix, WinNT, Win95, Win98, Win32s, Win3, Win_UNKNOWN};
// NOT YET IMPLEMENTED - tests for OS2
	
	static oofRuntimeEnvT runtimeEnv();
	static bool isMacintosh();
	static bool isUnix();
	static bool isWindows();
	static const char* termForLink();
	static void spendBackgroundTime();
	static void beep();
};


/**
	Abstract interface for database.
	Subclassed to indicate type of database backend to use.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbConnect : public OOF_PublicBase {
public:
	dbConnect(bool isTemporary=false);
	virtual ~dbConnect();
	
private:
	dbConnect(const dbConnect&) { assert(0); };  // NEVER CALLED - MAKE IMPOSSIBLE TO COPY
	void operator=(const dbConnect&) { assert(0); };  // MAKE IMPOSSIBLE TO COPY

public:	
	virtual oofString name() const;
	static dbConnect* currentlyConstructing();
	
	virtual void newConnection(const char* connectionName)=0;
	virtual void openConnection(const char* connectionName)=0;
	
#ifdef _Macintosh
	virtual void newConnection(const FSSpec&)=0;
	virtual void openConnection(const FSSpec&)=0;
#endif
	virtual void close()=0;
	virtual void deleteStorage();
	virtual bool usesSeparateStorageFiles() const;
	
	void gotoDatabaseDir() const;
	oofDirectory* databaseDirectory() const;
	const oofString& connectionName() const;
	void attachTable(dbTable *);
	dbTable *table(unsigned int) const;
	dbTable *table(const char*);
	unsigned int numTables() const;
	bool isOpen() const;
	bool inDeclarativePhase() const;

// locking
	virtual void exitLocking()=0;
	virtual void enterWriteLocking()=0;
	virtual void enterReadLocking()=0;

// utilities
	static bool fileExists(const char * fName);  // legacy call - use oofDirectory::fileExists
	static bool underConstruction();
	virtual void describe(ostream&) const;
	virtual void writeOOFSchema();
	virtual void readOOFSchema(const oofString& schemaFileName);
	virtual void stats(ostream&);
	void dumpData(ostream&);
	void extract(ostream& os) {dumpData(os);};	// dumpData is old form, extract is more consistent
	void copyAllFrom(dbConnect*);
	void deleteAll();
	void generateTestData(unsigned long maxRecs=10000, bool generateRandom=true, unsigned long seedOrCount=0);
	static void raise(ostream&, bool terminateAfterMsg=true);
	static void raise(const char*, bool terminateAfterMsg=true);
	void schemaFileExt(const oofString&);
#ifndef OOF_USING_EXCEPTIONS
	static void raise(const oofE_Base&, bool terminateAfterMsg=true);
#endif
	
// batch settings
	void suspendSorting() const;
	void resumeSorting() const;	

// settings
	static void truncateTrailingSpaces(bool willTruncate=true);
	static bool willTruncateTrailingSpaces();
#ifdef OOF_DEBUG_LOG_NAME
	static ostream&		debugLog();
	static void			startDebugWrite();
	static void			logComment(const char*);
	static void			logComment(ostream&);
	static void			endDebugWrite();
#endif	
	
	oofWordParser* getDefaultWordParser();
	void setDefaultWordParser(oofWordParser* inParser) ;
	
protected:
	void MakeAllTablesValid() const;
	void InitOpenOrCreateState();
	void OpenOrCreateStateTransition(bool inOpenedOrCreated);
	enum connectionFileStateT {// has both transition and state
		eIdle=0,
		eStart=1,
		eOpenedFile=2,
		eCreatedFile=4,
		eInvalidlyOpenInCreation = 8,
		eInvalidlyCreationInOpen = 16
		};
#ifdef OOF_DEBUG_LOG_NAME
	void LogConnection(const char* comment, const char* connectionName);
#endif
	virtual void WriteOOFSchemaFile();
	virtual oofString MakeSchemaFileName();
	oofString	MakeOOFSchemaString();
private:
	
	virtual OOF_tableBackend *MakeTableBackend(dbTable*)=0;

// data storage
protected:
	OOF_Dictionary	mTables;
	oofString mConnectionName;
	oofString mSchemaFileName;
	bool mOpen, mTempDatabase, mInDeclarativePhase;	
	oofWordParser*	mDefaultPrototypeParser;	// owned, shared
	oofDirectory* mDatabaseDir;		// owned
	static dbConnect *sCurrentlyConstructing;
	static bool sTruncateTrailingSpaces;
	connectionFileStateT mFileState;
	static oofString sSchemaFileExt;
#ifdef OOF_DEBUG_LOG_NAME
	static ofstream	sDebugLog;
	static oofDirectory	sDebugDir;
#endif	
//	friend dbTable::dbTable(const char*);
//	friend dbTable::dbTable(const dbTable& rhs);
//	friend class dbRelationship;
};


class dbBLOB;


/**
	Abstract interface to database backend.
	\ingroup oofDatabase	
*/
class OOFILE_EXPORT OOF_tableBackend {
// uses lots of public methods but is only ever accessed by a dbTable or dbField subclass
// so no problems
protected:
	OOF_tableBackend(dbTable*);
	OOF_tableBackend(const OOF_tableBackend&, dbTable*);
	virtual ~OOF_tableBackend() {};
	
 public:
	virtual void addIndices(const char* newIDXname)=0;
	virtual void suppressIndices()=0;
	virtual void rebuild()=0;
	virtual void close()=0;
	virtual void deleteStorage();
	virtual void buildSchema(bool rebuilding=false)=0;
	virtual void createTableInConnection(const dbConnect*)=0;
	virtual bool openTableInConnection(const dbConnect*, const bool createIfMissing = false)=0;
	
	virtual void setFileExtensions(const char* dataExt, const char* indexExt)=0;
	virtual OOF_tableBackend* clone(dbTable::selSharingT, dbTable*) const=0;
	virtual void describeState(ostream&) const=0;
	virtual void describeInvariant(ostream&) const=0;	
	

/// \name OID access & generation	
//@{
	virtual oidT currentOID()=0;
	virtual oidT oidOfRecord(unsigned long)=0;
//@}

// recordwise access
	virtual void start()=0;
	virtual void next()=0;
	virtual void prev()=0;
	virtual bool more() const=0;
	virtual bool atFirst() const=0;
	virtual bool atLast() const=0;
	virtual bool atRelative(unsigned long) const=0;
	virtual unsigned long count() const=0;
	virtual unsigned long countAll() const=0;

/// \name sorting	
//@{
   virtual void setSortOrder(dbSorter* adoptSorter)=0;
   virtual dbSorter* sorter() const=0;
   virtual void sortSelectionNow(fieldNumT)=0;
   virtual void sortSelectionInverseNow(fieldNumT)=0;
   virtual void sortSelectionNow(const dbSorter*)=0;
//@}

// data access
	virtual void newRecord()=0;
	virtual void deleteRecord()=0;
	virtual void saveRecord()=0;
	virtual void revertRecord()=0;
	virtual void unloadRecord()=0;
	virtual void unloadCache()=0;
	virtual bool reloadRecord()=0;
	virtual bool isRecordLoaded() const=0;
	virtual void *getFieldWriteDest(const dbField*)=0;
	virtual void *getFieldReadFrom(fieldNumT)=0;
	virtual void setBlobLength(fieldNumT, unsigned long len)=0;
	virtual void loadBlob(const dbBLOB*)=0;
	bool isDirty() const;
	virtual bool isNewRecord() const=0;
	void markDirty();
	void fieldHasDefaultCalculator(dbField*);
	void fieldHasStoredCalculator(dbField*);
	virtual void pack()=0;
	

// reflective operators
	virtual unsigned int numIndexes() const=0;
	virtual unsigned int numFiles() const=0;
	virtual unsigned long fieldDataLen(const dbField*) const=0;
	virtual unsigned long blobPointerReferenceSize() const=0;
	virtual unsigned long sequenceNumber() const=0;
	virtual bool allSelected() const=0;
	virtual bool fieldIsDirty(fieldNumT) const=0;
	dbTable* table() const;
	dbField* field(fieldNumT) const;
	virtual void ignoreDuplicateRecords()=0;
	virtual void noticeDuplicateRecords()=0;

// public search functions
    virtual unsigned long recordNumber() const=0;
    virtual unsigned long recordNumberOfOID(oidT inOID)=0;	
	virtual bool gotoRecord(unsigned long relativeNum)=0;
	virtual void selectAll()=0;
	virtual void selectNone()=0;
	virtual bool search(const dbQueryClause*)=0;
	virtual unsigned long recordNumberMatching(const dbQueryClause* qClause, bool* outFoundIt=0) const=0;
	virtual bool checkDuplicate(const dbQueryClause*)=0;
	virtual bool searchSelection(const dbQueryClause*)=0;
	virtual bool searchEqual(const dbField*, const char*, bool matchEntireKey=true)=0;
	virtual bool searchEqual(const dbField*, const void*)=0;
	virtual bool searchSelContainsAnyDelimited(const dbField* schField, const char *schStr, char delim)=0;
	virtual bool searchSelContainsAnyOf(const dbField* schField, const char **schStrs, unsigned long count)=0;
	
	virtual bool searchSelContainsAllDelimited(const dbField* schField, const char *schStr, char delim)=0;
	virtual bool searchSelContainsAllOf(const dbField* schField, const char **schStrs, unsigned long count)=0;
	virtual bool loadRelatedContextJoiningFromTo(const dbField*, const dbField*)=0;

	virtual bool addOIDtoSelection(oidT)=0;
	virtual bool contains(oidT) const=0;
	virtual void dropRecordFromSelection(unsigned long)=0;

	virtual void invert()=0;  
	virtual void difference_with(const OOF_tableBackend*)=0;
	virtual void intersection_with(const OOF_tableBackend*)=0;  
	virtual void union_with(const OOF_tableBackend*)=0;  
	virtual void union_with_no_overlap(const OOF_tableBackend*)=0;  
	virtual void difference_with(const dbSelection&)=0;
	virtual void intersection_with(const dbSelection&)=0;  
	virtual void union_with(const dbSelection&)=0;  
	virtual void union_with_no_overlap(const dbSelection&)=0;  

// getting & saving selections as lightweight objects
 	virtual dbSelection currentSelection() const=0;
	virtual dbSelection makeEmptySelection() const=0;
	virtual void setSelection(const dbSelection&)=0;
	virtual void setSelection(const OOF_Selection*)=0;
	
// selection of specific contexts
	virtual bool selectJustOID(oidT)=0;
	virtual void selectJustCurrent()=0;

// locking
	virtual void lockRecord()=0;
	virtual void unlockRecord()=0;
		
 
 // transactions
	virtual void beginTransaction(short tMode = 0)=0;
	virtual void abortTransaction(short rMode = 0)=0;
	virtual void commitTransaction(short rMode = 0)=0;

 //debugging 
#ifdef	OOF_DebugDumpBlobState	
	virtual void describeBlobs()=0;	
#endif

 protected:
	virtual void CachingContextChange()=0;
	virtual void ContextChange()=0;
	bool tableCachesDirtyRecords() const;
	
 // data storage
	OOF_Dictionary	 mFields;		// copy of dbTable handle object, one representation
	dbTable* mTable;
	bool mDirty;
	bool mHasDefaultCalculators, mHasStoredCalculators;

	friend class dbTable;
};


class OOF_Segment;
/**
	Mixin for classes allowing you to declare segments based on fields.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT OOF_FieldSegOwner {
public:
// constructors
	OOF_FieldSegOwner();
	OOF_FieldSegOwner(const OOF_FieldSegOwner&);
	virtual ~OOF_FieldSegOwner() {};

	
	virtual unsigned short countSegments() const;
	virtual void addSegment(dbField&);
	OOF_FieldSegOwner& operator<<(dbField&);
	OOF_FieldSegOwner& operator>>(dbField&);
	void toggleSegmentReversedFlags();
	
// reflective operators
	OOF_Segment* segment(unsigned int) const;
	fieldNumT fieldNumberOfSegment(unsigned int) const;

// data storage
private:
	OOF_Dictionary	mSegments;
}; 


/**
	Specify sort order by one more fields.
	Sort object retained by dbTable and possibly others
	to provide specification again in future.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbSorter : public OOF_FieldSegOwner {
public:
	dbSorter();
	dbSorter(const dbSorter&);
	dbSorter(const OOF_FieldSegOwner&);
	virtual ~dbSorter() {};
	virtual void describe(ostream&) const;

	dbSorter& operator<<(dbField&);
	dbSorter& operator>>(dbField&);
	
	
	void setSortOnSaves(bool willSortOnSaves=true);

	void suspendSorting();
	void resumeSorting();
	bool sortingSuspended() const;

// must implement the following in concrete classes	
	virtual dbSorter* clone() const;
	virtual void sortNow(OOF_tableBackend*);
	virtual unsigned short sortFieldNo() const;
	virtual bool sortsBySingleField() const; 
	bool operator!=(const dbSorter* rhs) const;

// state changers
	void backendSavedRecord(OOF_tableBackend*);
	void backendChangedContext(OOF_tableBackend*);
	void selectionChanged();
	
// state getter
	virtual unsigned long state() const;
	
protected:
// data storage
	bool mSortOnSaves, mNeedsToSort, mSorting, mSuspended;
};


/**
	simple specification class used to declare sort orders.
	Also used in expressions defining compound keys.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT reversed {
public:
	reversed(dbField& fld) : mField(fld) {};
	reversed(dbField* fld) : mField(*fld) {};
// use default copy ctor

	dbField& Field() const { return mField; }

private:
	dbField& mField;
};

// try including these definitions here anyway as complaints from VC6 about above friends
OOFILE_EXPORT dbSorter& operator<<(dbSorter&, const reversed&);
OOFILE_EXPORT OOF_FieldSegOwner& operator<<(OOF_FieldSegOwner&, const reversed&);


/**
	dbSorter subclass used when sorting by a single field.
	Provides optimal despatching for this common case to
	OOF_simpleRecordBackend::sortSelectionNow(fieldNumT)
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbFieldSorter : public dbSorter {
public:
	dbFieldSorter(fieldNumT, bool shouldReverse=false);
private:
	dbFieldSorter(const dbFieldSorter&) { assert(0); };
public:
	virtual ~dbFieldSorter() {};
	
	virtual void describe(ostream&) const;

	virtual dbSorter* clone() const;
	virtual void sortNow(OOF_tableBackend*);
	virtual unsigned short sortFieldNo() const;
	bool sortsInReverse() const;
	virtual bool sortsBySingleField() const;
	void changeSortFieldTo(fieldNumT, bool shouldReverse=false);
	
protected:
// data storage
	fieldNumT mSortFieldNo;
	bool mReverseSort;
};


/**
	Stack class to temporarily suspend sorting.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT stSuspendSort {
public:
	stSuspendSort(dbTable*);
	~stSuspendSort();
	
private:
	dbTable*	mTable;
};


/**
	Stack class to combine stSuspendSort and stSaveSelection.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT stSaveSelectionAndSuspendSort : public stSaveSelection {
public:
	stSaveSelectionAndSuspendSort(dbTable*);
	~stSaveSelectionAndSuspendSort();
};


#ifdef OOF_USING_EXCEPTIONS
/**
	Stack class to temporarily lock a record.
	\ingroup oofDatabase
*/
	class stLockRecord {
	public:
		stLockRecord(dbTable* inTable);
		stLockRecord(dbTable& inTable);
		~stLockRecord();
		
	private:
		void InitLock();
		dbTable*	mTable;
	};
#endif


#ifndef OOF_DISALLOW_CHANGE_SHIELDING
/**
	Stack class to enable Change Shielding, so listeners not informed database changed.
	Very useful for background manipulation on GUI screens to avoid flicker.
	\ingroup oofDatabase
*/
	class stChangeShielded {
	public:
		stChangeShielded(dbTable* inTable);
		stChangeShielded(dbTable& inTable);
		~stChangeShielded();
		
	private:
		dbTable*	mTable;
	};
#endif	


/**
	Cleanup after cloning a dbTable to get separate iterator.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbCloneCleanup : public oofSingleListener {
public:
	dbCloneCleanup(dbTable* listensTo);
// use default copy ctor
	virtual ~dbCloneCleanup() {};
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);

// data storage
private:
	dbTable* mTable;
};


class OOF_RelMN;  // forward

/**
	listener for an MN link that adds and deletes records.
	Acts in response to OOF_AppendSelection, RemoveSelection, ClearSelection
	and is often used in GUI applications where the user can create M:N links
	between tables, eg: by drag and drop to indicate an association.
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbRelMaintainer : public oofSingleListener {
public:
	dbRelMaintainer(dbRelRefBase* relToRHS, OOF_RelMN* linkWeAlert);
// use default copy ctor
	virtual ~dbRelMaintainer();
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);

// data storage
private:
	OOF_RelMN* mLink;
};


// utility functions
bool skipTillDigit(istream&, char fieldSep=dbTable::kFieldSep, char recSep=dbTable::kRecSep);
bool skipRestOfField(istream&, char fieldSep=dbTable::kFieldSep, char recSep=dbTable::kRecSep);
bool skipTillNumber(istream& is, char fieldSep, char recSep, bool canStartWithDecimal=false);


// -------------------------------------------------------
//                 O O F _ D i c t r e p
// -------------------------------------------------------
inline 
OOF_bitPointer& OOF_DictRep::item(unsigned int index)
{ 
	return operator[](index); 
}


// -------------------------------------------------------
//               O O F _ D i c t i o n a r y
// -------------------------------------------------------

inline unsigned int 
OOF_Dictionary::iter() const
{
	return mInternalIter;
}


inline unsigned int 
OOF_Dictionary::count() const
{
	return mRep->mNextFreeEntry;
}


inline void 
OOF_Dictionary::start() 
{
	mInternalIter = 0;
}


inline bool 
OOF_Dictionary::more() const
{
	return mInternalIter < mRep->mNextFreeEntry;
}


inline void 
OOF_Dictionary::next() 
{
	mInternalIter++;
}


inline bool 
OOF_Dictionary::EntryMatchesVisibility(const OOF_PublicBase* entry, bool wantVisible) const 
{
		bool isVisible = !entry->hidden();
		return ((isVisible && wantVisible) || (!isVisible && !wantVisible));
}


inline void 
OOF_Dictionary::startVisible(bool wantVisible) 
{
	start();
	moreVisible(wantVisible);
}


inline void 
OOF_Dictionary::nextVisible(bool wantVisible)
{
	next();
	moreVisible(wantVisible);
}
 
 
inline void 
OOF_Dictionary::append(OOF_bitPointer aBit)  
{
	mRep->Append(aBit);
}
 
 
inline OOF_bitPointer 
OOF_Dictionary::pop()  
{
	return mRep->Pop();
}


inline void 
OOF_Dictionary::reset()  
{
	mRep->Reset();
}


inline OOF_bitPointer& 
OOF_Dictionary::operator()() // current value
{ 
	return mRep->operator[](mInternalIter); 
}

/*
inline OOF_bitPointer& 
OOF_Dictionary::operator[](int index)
{
	assert(index>=0);
	return mRep->item(index);
}
*/

inline OOF_bitPointer& 
OOF_Dictionary::item(unsigned int index)
{ 
	return mRep->item(index); 
}


inline OOF_bitPointer 
OOF_Dictionary::value(unsigned int index) const
{ 
	return mRep->value(index); 
}


inline OOF_bitPointer& 
OOF_Dictionary::operator[](const char *name)
{
	return mRep->operator[](name);
}


inline OOF_bitPointer& 
OOF_Dictionary::operator[](unsigned int index)
{
	return mRep->operator[](index);
}


inline OOF_bitPointer& 
OOF_Dictionary::lookupByAttribute(const char* attributeValue, const char* attributeType)
{
	return mRep->lookupByAttribute(attributeValue, attributeType);
}


inline OOF_bitPointer& 
OOF_Dictionary::missingItem()
{
	return sMissingItem;
}



// -------------------------------------------------------
//                      d b C o n n e c t
// -------------------------------------------------------
inline dbConnect* 
dbConnect::currentlyConstructing()
{
	return sCurrentlyConstructing;
}


inline const oofString& 
dbConnect::connectionName() const
{
	return mConnectionName;
}

inline unsigned int
dbConnect::numTables() const
{
	return mTables.count();
}


inline bool 
dbConnect::underConstruction()
{
	return sCurrentlyConstructing!=0;
}


inline void 
dbConnect::truncateTrailingSpaces(bool willTruncate)
{
	sTruncateTrailingSpaces = willTruncate;
}


inline bool 
dbConnect::willTruncateTrailingSpaces()
{
	return sTruncateTrailingSpaces;
}


#ifdef OOF_DEBUG_LOG_NAME
inline ostream&	
dbConnect::debugLog()
{
	return sDebugLog;
}
#endif


inline oofDirectory* 
dbConnect::databaseDirectory() const
{
	return mDatabaseDir;
}


inline bool 
dbConnect::isOpen() const
{
	return mOpen;
}


/**
	Connection has been declared but not opened or created.
	Only one connection at a time can be in this state.
	\todo cope if declare 2nd connections before opening either, which is legal.
*/
inline bool 
dbConnect::inDeclarativePhase() const
{
	return sCurrentlyConstructing == this;
}


// -------------------------------------------------------
//           O O F _ t a b l e  B a c k e n d 
// -------------------------------------------------------
inline void 
OOF_tableBackend::markDirty()
{
		mDirty=true;
}


inline bool 
OOF_tableBackend::isDirty() const
{
		return mDirty;
}


inline dbTable* 
OOF_tableBackend::table() const
{
		return mTable;
}


inline dbField*
OOF_tableBackend::field(fieldNumT fieldNum) const
{ 
	return (dbField *) mFields.value(fieldNum); // safe downcast
}


inline void
OOF_tableBackend::fieldHasDefaultCalculator(dbField*)
{
	mHasDefaultCalculators = true;
}


inline void
OOF_tableBackend::fieldHasStoredCalculator(dbField*)
{
	mHasStoredCalculators = true;
}


// -------------------------------------------------------
//             d b T a b l e A s s i g n e r
// -------------------------------------------------------
inline dbTableAssigner& 
dbTableAssigner::operator<<(const char* inString)
{
	setNextFieldString(inString);
	return *this;
}



// -------------------------------------------------------
//                      d b T a b l e
// -------------------------------------------------------
#if 0
inline dbTable::operator dbTable*() 
{ 
	return this; 
};
#endif

inline bool 
dbTable::isOpen() const
{
	return (mTableValid!=eClosed);
}


/**
	Has database been opened or are we still potentially declaring tables.
	Will be true before newConnection or openConnection has been called.
*/
inline bool 
dbTable::inDeclarativePhase() const
{
	return (!mConnection || mConnection->inDeclarativePhase());
}



inline bool 
dbTable::isTableValid() const
{
	return (mTableValid==eValid);
}


inline dbTable::tableValidT 
dbTable::validStatus() const
{
	return mTableValid;
}	


inline int 
dbTable::validatingDepth() const
{
	return mValidatingDepth;
}	


inline dbRelRefBase*
dbTable::controllingRelationship() const
{
	return mRelToLHS;
}


inline bool 
dbTable::isRecordLoaded() const
{
	return mBackend->isRecordLoaded();
}


inline void
dbTable::noticeDuplicateRecords()
{
	mBackend->noticeDuplicateRecords();
}


inline void
dbTable::ignoreDuplicateRecords()
{
	mBackend->ignoreDuplicateRecords();
}

inline unsigned long 
dbTable::sequenceNumber() const
{
	return mBackend->sequenceNumber();
}


inline unsigned int 
dbTable::numIndexes() const
{
	return mBackend->numIndexes();
}


inline unsigned int 
dbTable::numFiles() const
{
	return mBackend->numFiles();
}


inline fieldNumT 
dbTable::numFields() const
{
	return mFieldCount;
}


/**
	Force current table state to valid.
	\warning do not use unless you know what you are doing.
*/
inline void 
dbTable::setTableValid(bool validState)
{
#ifdef OOF_DEBUG_LOG_NAME
	if (validState)
		StandardDebugLog("dbTable::setTableValid(true)");
	else
		StandardDebugLog("dbTable::setTableValid(false)");
#endif

	if (validState)
		mTableValid = eValid;
	else
		mTableValid = eInvalid;
	// can't set closed state - side-effect of close()
}


/**
	When iterating a selection, are there any more records beyond current.
*/
inline bool 
dbTable::more() const
{
	return mBackend->more();
}


/**
	Does the current selection represent all records or just a subset.
*/
inline bool 
dbTable::allSelected() const
{
	return mBackend->allSelected();
}


inline bool 
dbTable::atFirst() const
{
	return mBackend->atFirst();
}


inline bool 
dbTable::atLast() const
{
	return mBackend->atLast();
}


inline bool 
dbTable::atRelative(unsigned long rel) const
{
	return mBackend->atRelative(rel);
}


/**
	Absolute record address of current record.
	Persistent value can be used between application runs.
*/
inline oidT
dbTable::currentOID()
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->currentOID();
}


/**
	Check if current selection contains an OID.
	Use with selectAll to see if OID is anywhere in database.
*/
inline bool
dbTable::contains(oidT inOID)
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->contains(inOID);
}


inline oidT
dbTable::oidOfRecord(unsigned long recno)
{
	return mBackend->oidOfRecord(recno);
}


/**
	Next record in the selection becomes current.
*/
inline void 
dbTable::next() 
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::next()");
#endif

	ContextChange();
	mBackend->next();
}


/**
	First record in the selection becomes current.
*/
inline void 
dbTable::first()
{
	ContextChange();
	mBackend->gotoRecord(0);
}


/**
	Previous record in the selection becomes current.
*/
inline void 
dbTable::prev()
{
	ContextChange();
	mBackend->prev();
}


/**
	Last record in the selection becomes current.
*/
inline void 
dbTable::last()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::last()");
#endif

	ContextChange();
	mBackend->gotoRecord(mBackend->count()-1);
}


inline void 
dbTable::ensureSelectionLoaded()
{
	if (!isTableValid())
		MakeTableValid();
}


/**
	Count records in current selection.
	Includes current record even if new (ie: it has not had a record pointer saved to disk).
*/
inline unsigned long 
dbTable::count()
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->count();
}


/**
	Count records in database regardless of current selection.
*/
inline unsigned long 
dbTable::countAll() const
{
	return mBackend->countAll();
}


/**
	Ordinal record number of current record in selection.
	zero-based, will change if selection sorted differently.
	currentOID() returns absolute record address.
*/
inline unsigned long 
dbTable::recordNumber() const
{
	if (isRecordLoaded())		
		return mBackend->recordNumber();
	else
		return 0;
}


/**
	Find ordinal position in ordered selection of record by OID.
	\param oidT identifier (commonly record address) of desired record
	\todo decide if should return standard enum value for not found.
*/
inline unsigned long 
dbTable::recordNumberOfOID(oidT inOID)
{
	return mBackend->recordNumberOfOID(inOID);
}


inline unsigned long 
dbTable::recordNumber(const dbQueryClause* qClause, bool* outFoundIt) const
{
	return mBackend->recordNumberMatching(qClause, outFoundIt);
}


inline unsigned long 
dbTable::recordNumber(const dbQueryClause& qClause, bool* outFoundIt) const
{
	return mBackend->recordNumberMatching(&qClause, outFoundIt);
}


inline void 
dbTable::difference_with(dbTable* rhs)
{
	difference_with(*rhs);
}


/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator-=(dbTable* rhs)
{
	difference_with(*rhs);
}


/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator-=(dbTable& rhs)
{
	difference_with(rhs);
}
	

/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator-=(const dbSelection& rhs)
{
	difference_with(rhs);
}
	

/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator%=(const dbSelection& rhs)
{
	difference_with(rhs);
}
	

/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator%=(dbTable& rhs)
{
	difference_with(rhs);
}
	

/**
	Operator performing difference_with set operation.
*/
inline void 
dbTable::operator%=(dbTable* rhs)
{
	difference_with(*rhs);
}


inline void 
dbTable:: intersection_with(dbTable* rhs)  
{
	intersection_with(*rhs);
}


/**
	Operator performing intersection_with set operation.
*/
inline void 
dbTable:: operator&=(dbTable& rhs)  
{
	intersection_with(rhs);
}


/**
	Operator performing intersection_with set operation.
*/
inline void 
dbTable:: operator&=(const dbSelection& rhs)  
{
	intersection_with(rhs);
}


/**
	Operator performing intersection_with set operation.
*/
inline void 
dbTable:: operator&=(dbTable* rhs)  
{
	intersection_with(*rhs);
}


/**
	Operator performing invert set operation.
*/
inline void 
dbTable::operator!()
{
	invert();
} 


/**
	Operator performing invert set operation.
*/
inline void 
dbTable::operator~()
{
	invert();
} 


inline void 
dbTable::union_with(dbTable* rhs)
{
	union_with(*rhs);
}


inline void 
dbTable::union_with_no_overlap(dbTable* rhs)
{
	union_with_no_overlap(*rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator+=(dbTable& rhs)
{
	union_with(rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator+=(const dbSelection& rhs)
{
	union_with(rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator+=(dbTable* rhs)
{
	union_with(*rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator|=(dbTable* rhs)
{
	union_with(*rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator|=(dbTable& rhs)
{
	union_with(rhs);
}


/**
	Operator performing union_with set operation.
*/
inline void 
dbTable::operator|=(const dbSelection& rhs)
{
	union_with(rhs);
}


/**
	Operator performing addOIDtoSelection set operation like union_with.
*/
inline bool
dbTable::operator+=(oidT rhs)
{
	return addOIDtoSelection(rhs);
}


/**
	Create dbTableAssigner as fast way to create and set entire record.
	Allows you to pass in all fields as text strings, eg:
	\code
	People << "Andy" << "Dent" << "37";
	\endcode
*/
inline dbTableAssigner
dbTable::operator<<(const char* inString)
{
	return dbTableAssigner(this, inString);
}


/**
	Change the current selection to all records.
	This state is explicitly tracked as it allows backends to maintain
	an optimal representation, eg: not needing to retain a list of record pointers.
*/
inline void 
dbTable::selectAll()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::selectAll()");
#endif

	mBackend->selectAll();
	broadcast(OOFmsg_ChangeSelection);
}


/**
	Change the current selection to no records.
	This empty set is what would be returned by an unsuccessful search().
	\see isEmpty()
*/
inline void 
dbTable::selectNone()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::selectNone()");
#endif

	mBackend->selectNone();
	unloadRecord();
	broadcast(OOFmsg_ChangeSelection);
}


inline bool 
dbTable::isDirty() const
{
	return mBackend->isDirty();
}


inline bool 
dbTable::isNewRecord() const
{
	return mBackend->isNewRecord();
}


inline bool
dbTable::isRelatedClone() const
{
	return (mRelToLHS!=0);
}


inline dbField*
dbTable::field(fieldNumT fieldNum) const
{ 
	return (dbField *) mFields.value(fieldNum); // safe downcast
}


inline dbField*
dbTable::field(const oofString& fieldName)
{ 
	return  (dbField *) mFields[fieldName]; 
}


inline const oofString&
dbTable::tableName() const
{ 
	return  mTableName; 
}


inline tableNumT
dbTable::tableNumber() const
{
	return mTableNumber;
}


inline bool
dbTable::tableCachesDirtyRecords() const
{
	bool ret = ( 
		(mSaveOption == dbTable::requireExplicitAndBuffer) || 
		(mSaveOption == dbTable::bufferForever)  )
	;
	return ret;
}


inline dbTable::saveOptionsT
dbTable::tableSaveOption() const
{
	return mSaveOption;
}


inline void 
dbTable::setName(const char* name)
{ 
	mTableName = name; 
}

 
inline void 
dbTable::setFileExtensions(const char* dataExt, const char* indexExt)
{
	mBackend->setFileExtensions(dataExt, indexExt);
}


inline dbSorter* 
dbTable::sorter() const
{
	return mBackend->sorter();
}
 


/**
	Change the current selection to just the current record.
	This makes a selection behave like a single object.
*/
inline void 
dbTable::selectJustCurrent()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::selectJustCurrent()");
#endif
	mBackend->selectJustCurrent();
	broadcast(OOFmsg_ChangeSelection);
}


inline void 
dbTable::lockRecord()
{
	if (!mLockedCurrentRecord)
		mBackend->lockRecord();  // may throw OOFE_LockedRecord if already locked
	mLockedCurrentRecord = true;
}


inline void 
dbTable::unlockRecord()
{
	if (mLockedCurrentRecord)
		mBackend->unlockRecord();
	mLockedCurrentRecord =false;
}


inline bool
dbTable::transactionDirty()
{
	return mTransactionDirty;
}


inline void 
dbTable::pack()
{
	mBackend->pack();
}


inline bool
dbTable::lockedCurrentRecord() const
{
	return mLockedCurrentRecord;
}


inline dbTable* 
dbTable::prototype() const
{
	return mPrototype;
}



#ifndef OOF_DISALLOW_CHANGE_SHIELDING
/**
	 change shielding, so field changes don't mark the record as dirty.
	 (default is false)
	 Used when we don't want operations to propagate, eg: if updating
	 a field and need to finish some local op before related data updates.
*/
inline bool
dbTable::changeShielded() const
{
	return mChangeShielded;
}


inline void 
dbTable::changeShielded(bool setShielding)
{
	mChangeShielded = setShielding;
}
#endif	


inline 
void dbTable::describeInvariant(ostream& os) const
{		
	mBackend->describeInvariant(os);
}

// -------------------------------------------------------
//           O O F _ t a b l e  B a c k e n d 
// -------------------------------------------------------
// just the methods that have to be after dbTable inlines above

inline bool
OOF_tableBackend::tableCachesDirtyRecords() const
{
	const bool ret = mTable->tableCachesDirtyRecords();
	return ret;
}



// -------------------------------------------------------
//                      d b S o r t e r 
// -------------------------------------------------------
inline bool 
dbSorter::sortingSuspended() const
{
   return mSuspended;
}


inline void 
dbSorter::setSortOnSaves(bool onSaves)
{
   mSortOnSaves = onSaves;
}


inline dbSorter& 
dbSorter::operator<<(dbField& segField)
{
	addSegment(segField);
	return *this;
}




// -------------------------------------------------------
//               O O F _ D i c t i o n a r y
// -------------------------------------------------------
inline void 
OOF_Dictionary::ownsContents(bool ownsFlag)
{
	mOwnsContents = ownsFlag;
}


// -------------------------------------------------------
//                      d b F i e l d S o r t e r 
// -------------------------------------------------------
inline bool 
dbFieldSorter::sortsInReverse() const
{
   return mReverseSort;
}


// -------------------------------------------------------
//           O O F _ F i e l d S e g O w n e r
// -------------------------------------------------------
inline unsigned short 
OOF_FieldSegOwner::countSegments() const
{	
	return mSegments.count();
}


inline OOF_FieldSegOwner& 
OOF_FieldSegOwner::operator<<(dbField& segField)
{
	addSegment(segField);
	return *this;
}


inline OOF_Segment* 
OOF_FieldSegOwner::segment(unsigned int i) const
{
	assert(i<mSegments.count());
	return (OOF_Segment*)(mSegments.value(i));  // safe downcast
}


 
// -------------------------------------------------------
//         o o f R u n t i m e E n v 
// -------------------------------------------------------

inline  bool 
oofRuntimeEnv::isMacintosh()
 {
 	#ifdef _Macintosh
		return true;
	#else
		return false;
	#endif
 }
 
 
inline  bool 
oofRuntimeEnv::isWindows()
 {
 	#ifdef _Windows
		return true;
	#else
		return false;
	#endif
 }
 
 
inline  bool 
oofRuntimeEnv::isUnix()
 {
 	#ifdef _Unix
		return true;
	#else
		return false;
	#endif
 }
 
 
inline const char* 
oofRuntimeEnv::termForLink()
{
#ifdef _Macintosh
	return "alias";
#elif defined _Windows
	return "shortcut";
#elif defined _Unix
	return "link";
#else
error platform unknown
#endif
}
 

inline void
oofRuntimeEnv::spendBackgroundTime()
{
#ifdef _Macintosh
	EventRecord ev;
	::WaitNextEvent(everyEvent, &ev, 0x0, NULL);
#elif defined _Windows
// NOT YET IMPLEMENTED
#elif defined _Unix
// don't need to spend background time??
#else
error platform unknown
#endif
}


#endif

