// COPYRIGHT 1994-1997 A.D. Software, All rights reserved

// backend parent layer of OOFILE database

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//             s t a t i c   v a r s
// -------------------------------------------------------
OOF_DummyStorage OOF_DummyString::sStrings;
OOF_bitPointer OOF_Dictionary::sMissingItem;

// -------------------------------------------------------
//              O O F _ D u m m y S t r i n g
// -------------------------------------------------------
OOF_DummyString::OOF_DummyString(const char* dummyPrefix) :
	mSuffix(0)
{
	if (dummyPrefix)
		sStrings.SetPrefix(dummyPrefix);
}


const OOF_String&
OOF_DummyString::nextDummy()
{
	const OOF_String& ret = sStrings.Dummy(mSuffix);
	mSuffix++;
	return ret;
}


void 
OOF_DummyString::releaseAllStorage()
{
	sStrings.ReleaseAllStorage();
}


// -------------------------------------------------------
//              O O F _ D u m m y S t o r a g e
// -------------------------------------------------------
OOF_DummyStorage::OOF_DummyStorage() :
	mPrefix("dummy")
{
}


OOF_DummyStorage::~OOF_DummyStorage()
{
	DeleteAllStrings();
}


void
OOF_DummyStorage::DeleteAllStrings()
{
	const unsigned long numStr = mStringCache.count();
	for (unsigned int i=0; i<numStr; i++) {
		OOF_String* theStr = (OOF_String*) mStringCache.value(i);
		delete theStr;	
		mStringCache[i] = 0;
	}
	mStringCache.deleteAllCells(false);  // just reset counter
}


/**
	Total cleanup so can avoid leak reports based on statics.
*/
void
OOF_DummyStorage::ReleaseAllStorage()
{
	DeleteAllStrings();
	mStringCache.deleteAllCells(true);
}


void
OOF_DummyStorage::SetPrefix(const OOF_String& newPrefix)
{
	if (mPrefix!=newPrefix) // regenerate all the strings!
		DeleteAllStrings();	// but only do half the job
	// we don't have to recreate them until someone needs them
}


const OOF_String&
OOF_DummyStorage::Dummy(unsigned long suffix)
{
// create a dummy entry but hopefully there's one in the stringCache
	OOF_String* ret = (OOF_String*) mStringCache[suffix];
	if (!ret) {
		char buf[80];
		sprintf(buf, "%lu", suffix);	// concatenate 
		ret = new OOF_String(mPrefix, buf);
		mStringCache[suffix] = (unsigned long)ret;
	}
	return *ret;
}



// -------------------------------------------------------
//                 O O F _ S h a r e d A r r a y
// -------------------------------------------------------
OOF_SharedArray::OOF_SharedArray(unsigned long defaultValue, unsigned long numSlots, unsigned int expandBySlots) :
		OOF_ExpandableLongArray(defaultValue, numSlots, expandBySlots)
	{}
	

// -------------------------------------------------------
//           O O F _ t a b l e  B a c k e n d 
// -------------------------------------------------------
OOF_tableBackend::OOF_tableBackend(dbTable* theTable):
									mFields(theTable->mFields),
									mTable(theTable),
									mDirty(false),
									mHasDefaultCalculators(false),
									mHasStoredCalculators(false)
{
}


OOF_tableBackend::OOF_tableBackend(const OOF_tableBackend& rhs, dbTable* newTable) :
	mFields(newTable->mFields),
	mTable(newTable),
	mDirty(false),
	mHasDefaultCalculators(rhs.mHasDefaultCalculators),
	mHasStoredCalculators(rhs.mHasStoredCalculators)
{}


void
OOF_tableBackend::deleteStorage()
{
// empty default method
}
	

// -------------------------------------------------------
//                 O O F _ D i c t r e p
// -------------------------------------------------------
void 
OOF_DictRep::Append(OOF_bitPointer aBit)  {
	(*this)[mNextFreeEntry] = aBit;
}


OOF_bitPointer 
OOF_DictRep::Pop()  
{
	OOF_bitPointer ret = 0;
	if (mNextFreeEntry>0) {
		mNextFreeEntry--;
		assert(mBits);
		ret = mBits[mNextFreeEntry];
		mBits[mNextFreeEntry] = 0;
	}
	return ret;
}


void 
OOF_DictRep::Reset()  
{
	mNextFreeEntry=0;
} 


OOF_DictRep::OOF_DictRep(unsigned int numSlots, unsigned int expandBySlots) : 
	mBits(0),
	mExpansionChunk(expandBySlots),
	mNextFreeEntry(0),
	mNumSlots(0)
{
	if (mExpansionChunk<1)
		mExpansionChunk = 1;
	if (numSlots) 
	{
		mNumSlots = ((numSlots-1) / expandBySlots + 1) * expandBySlots;
		mBits = new OOF_bitPointer[mNumSlots];
		assert(mBits);
	}
}


OOF_DictRep::OOF_DictRep(const OOF_DictRep* rhs) : 
	mBits(0),
	mExpansionChunk(rhs->mExpansionChunk),
	mNextFreeEntry(rhs->mNextFreeEntry),
	mNumSlots(rhs->mNumSlots)
{
	if (mNumSlots) 
	{
		mBits = new OOF_bitPointer[mNumSlots];
		assert(mBits);
		for (unsigned long i=0; i<mNumSlots; i++)
			mBits[i] = rhs->mBits[i];
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
}


OOF_DictRep::~OOF_DictRep() 
{
	if (mNumSlots)
		delete[] mBits;
}


OOF_bitPointer& 
OOF_DictRep::operator[](unsigned int index)
{
	assert(mNextFreeEntry <= mNumSlots);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
#ifdef OOF_DEBUG
	if (index > mNumSlots) {
		dbConnect::raise("OOF_DictRep::operator[] skipped a cell - suspect index", false /* don't exit*/);
	}	
#endif
	if (index >= mNumSlots)
		ExpandToInclude(index);
	if (index >= mNextFreeEntry)
		mNextFreeEntry = index+1;
	return mBits[index];
}


OOF_bitPointer& 
OOF_DictRep::operator[](const char* name)
{
	return lookupByAttribute(name);
}


OOF_bitPointer& 
OOF_DictRep::lookupByAttribute(const char* attributeValue, const char* /*attributeType*/)
{
// NOT YET IMPLEMENTED - attributeType currently ignored
// NOT YET IMPLEMENTED - future dictionaries would stored hashed names 
// for rapid lookup, as well as storing other attributes or at least being
// able to invoke a virtual hasAttribute(value, type) on the mBits

	const unsigned int numEntries = mNextFreeEntry;
	for (unsigned long i=0; i<numEntries; i++) {
		OOF_bitPointer& theBit = mBits[i];
		if (theBit->name()==attributeValue)
			return theBit;
	}
	return OOF_Dictionary::missingItem();
}


OOF_bitPointer 
OOF_DictRep::value(unsigned int index) const
{ 
	assert(index < mNextFreeEntry);
	assert(mNextFreeEntry <= mNumSlots);
	return mBits[index];
}


void 
OOF_DictRep::ExpandToInclude(unsigned int indexToCover) 
{
	unsigned int newSize = (indexToCover / mExpansionChunk + 1) * mExpansionChunk;
	OOF_bitPointer *oldBits = mBits;
	mBits = new OOF_bitPointer[newSize];
// NOT YET IMPLEMENTED - need error handling here

	assert(mBits);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	if (oldBits) {
		for (unsigned long i=0; i<mNextFreeEntry; i++)
			mBits[i] = oldBits[i];
		delete[] oldBits;
	}
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	for (unsigned long i=mNextFreeEntry; i<newSize; i++)
		mBits[i] = 0;
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	mNumSlots = newSize;
}



// -------------------------------------------------------
//               O O F _ D i c t i o n a r y
// -------------------------------------------------------
OOF_Dictionary::OOF_Dictionary() : 
	mOwnsContents(false)
{
	mRep = new OOF_DictRep(0, kDefExpansionChunk);
}


OOF_Dictionary::OOF_Dictionary(const OOF_DictRep* rep) : 
	mOwnsContents(false)
{
	mRep = new OOF_DictRep(rep);
}


OOF_Dictionary::OOF_Dictionary(unsigned int numSlots) : 
	mOwnsContents(false)
{ 
	mRep = new OOF_DictRep(numSlots, kDefExpansionChunk);
}


OOF_Dictionary::OOF_Dictionary(unsigned int numSlots, unsigned int expansionChunk, bool inOwnsContents) : 
	mOwnsContents(inOwnsContents)
{
// only this particular ctor allows you to pass in inOwnsContents because of ambiguity problems
// with others having the bool param confused with a different ctor's int params
	mRep = new OOF_DictRep(numSlots, expansionChunk);
}


OOF_Dictionary::OOF_Dictionary(const OOF_Dictionary& rhs) :
	mRep(rhs.mRep),
	mOwnsContents(rhs.mOwnsContents)
{
	mRep->mReferences++;
}



OOF_Dictionary::~OOF_Dictionary() 
{
	if (--mRep->mReferences<=0) {
		if (mOwnsContents)
			deleteAll();
		delete mRep;
	}
}


OOF_Dictionary& 
OOF_Dictionary::operator=(const OOF_Dictionary& rhs)
{
	if ((this == &rhs) || (mRep == rhs.mRep))
		return *this;
	
	if (--mRep->mReferences<=0)
		delete mRep;

	mRep = rhs.mRep;
	mRep->mReferences++;
	return *this;
}


OOF_Dictionary 
OOF_Dictionary::clone() const
{
	OOF_Dictionary ret(mRep);  
	return ret;
}


unsigned int 
OOF_Dictionary::countVisible(bool wantVisible) 
{
	unsigned int ret=0;
   unsigned int theCount = count();
	for (unsigned int i=0; i<theCount; i++) {
		OOF_PublicBase* entry = mRep->operator[](i);
		assert(entry);  // looping known entries!
		if (EntryMatchesVisibility(entry, wantVisible))
			ret++;
	}
	return ret;
}


bool 
OOF_Dictionary::moreVisible(bool wantVisible) 
{
	if (!more())
		return false;
	OOF_PublicBase* entry=0;
	while (more()) {
		OOF_PublicBase* entry = operator()();
		assert(entry);  // looping known entries!
		if (EntryMatchesVisibility(entry, wantVisible))
			return true;
		next();
	}
	return (EntryMatchesVisibility(entry, wantVisible));
}


void 
OOF_Dictionary::deleteAll()
{
	const unsigned int numEntries = count();
	for (unsigned int i=0; i<numEntries; i++) {
		OOF_bitPointer toDel = item(i);
		delete toDel;
		next();
	}
	mRep->mNextFreeEntry = 0;
}


void 
OOF_Dictionary::describe(ostream& os) const
{
   unsigned int numEntries = count();
	for (unsigned int i=0; i<numEntries; i++) {
		(value(i))->describe(os);
	}
	os << endl;
}


void 
OOF_Dictionary::describeVisible(ostream& os, bool wantVisible) const
{
	unsigned int numEntries = count();
	for (unsigned int i=0; i<numEntries; i++) {
		OOF_bitPointer entry = value(i);
		if (EntryMatchesVisibility(entry, wantVisible))
			entry->describe(os);
	}
	os << endl;
}


bool 
OOF_Dictionary::allObjectsMatch(OOF_Dictionary& rhs)
{
	if (mRep==rhs.mRep)
		return true;  // trivial case of comparing aliases
		
	if (count() != rhs.count())
		return false;
	
   unsigned int numEntries = count();
	for (unsigned int i=0; i<numEntries; i++)
		if (operator[](i) != rhs.operator[](i))
			return false;
			
	return true;
}


// -------------------------------------------------------
//          O O F _ I n d e x O p t i o n s
// -------------------------------------------------------
ostream& 
operator<<(ostream& os, OOF_IndexOptions opt)
{
	switch (opt) {
	case (kNotIndexed) :
		os << "not indexed";
		break;
		
	case (kIndexed) :
		os << "indexed, allowing dups & ignoring case";
		break;
		
	case (kIndexNoDups) :
		os << "indexed, no dups & ignoring case";
		break;
		
	case (kIndexCaseSensitive) :
		os << "indexed, allowing dups but case-sensitive";
		break;
		
	case (kIndexCompressPaddingNoDups) :
		os << "indexed, no dups & ignoring case, compressing padding";
		break;
		
	case (kIndexCompressLeadingNoDups) :
		os << "indexed, no dups & ignoring case, compressing front";
		break;
		
	case (kIndexCompressNoDups) :
		os << "indexed, no dups & ignoring case, compress front & padding";
		break;
		
	case (kIndexCompress) :
		os << "indexed, ignoring case, compress front & padding";
		break;
		
	case (kIndexCompressPadding) :
		os << "indexed, ignoring case, compressing padding";
		break;
		
	case (kIndexCompressLeading) :
		os << "indexed, ignoring case, compressing front";
		break;
		
	case (kIndexNoNulls) :
		os << "indexed, ignoring case, no nulls";
		break;
		
	case (kIndexNoDupsNoNulls) :
		os << "indexed, ignoring case, no dups no nulls";
		break;
		
	case (kIndexCompressNoNulls) :
		os << "indexed, ignoring case, no nulls, compressing front & padding";
		break;
		
	case (kIndexCompressNoDupsNoNulls) :
		os << "indexed, ignoring case, no nulls, no dups, compressing front & padding";
		break;
		
	case (kIndexCompressLeadingNoDupsNoNulls) :
		os << "indexed, ignoring case, no nulls, no dups, compressing front";
		break;
		
	case (kIndexCompressPaddingNoDupsNoNulls) :
		os << "indexed, ignoring case, no nulls, no dups, compressing padding";
		break;
		
	}
	return os;
}




// -------------------------------------------------------
//                    d b S e l e c t i o n
// -------------------------------------------------------
dbSelection::dbSelection() :
	mSelection(0),
	mReferences(0)
{
}


dbSelection::dbSelection(const OOF_Selection* sel)
{
	mSelection = sel->clone();
	mReferences = new unsigned long(1);
}


dbSelection::~dbSelection()
{
	DeleteContents();
}


void
dbSelection::DeleteContents()
{
	if (mReferences) {  
		assert(mSelection);
		if (*mReferences<=1) {
			delete mSelection;
			delete mReferences;
		}
		else
			--*mReferences;
	}	
}


dbSelection::dbSelection(const dbSelection& rhs) :
	mSelection(rhs.mSelection),
	mReferences(rhs.mReferences)
{
	if (mReferences)
		++*mReferences;
}


dbSelection&
dbSelection::operator=(const dbSelection& rhs)
{
	if (mReferences)  { // not just a blank sel that's never been inited
		assert(prototypicalTable() == rhs.prototypicalTable());  // can't assign from different source!
		DeleteContents();
	}
	mSelection = rhs.mSelection;
	mReferences = rhs.mReferences;
	++*mReferences;
	return *this;
}


dbSelection&
dbSelection::operator=(dbTable& rhs)
{
	*this = rhs.currentSelection();
	return *this;
}


void 
dbSelection::intersection_with(const dbSelection& rhs)
{
	if (mSelection)
		mSelection->intersection_with(rhs.mSelection);
}


void 
dbSelection::intersection_with(dbTable& rhs)
{
	if (mSelection)
		intersection_with(rhs.currentSelection());
}


void 
dbSelection::intersection_with(oidT offset)
{
	if (mSelection)
		mSelection->intersection_with(offset);
}


void 
dbSelection::invert()
{
	if (mSelection)
		mSelection->invert();
}



void 
dbSelection::union_with_no_overlap(dbTable& rhs)
{
	if (mSelection)
		union_with_no_overlap(rhs.currentSelection());
	else
		*this = rhs;
}


void 
dbSelection::union_with_no_overlap(const dbSelection& rhs)
{
	if (mSelection)
		mSelection->union_with_no_overlap(rhs.mSelection);
	else
		*this = rhs;
}


void 
dbSelection::union_with(dbTable& rhs)
{
	if (mSelection)
		union_with(rhs.currentSelection());
	else
		*this = rhs;
}


void 
dbSelection::union_with(const dbSelection& rhs)
{
	if (mSelection)
		mSelection->union_with(rhs.mSelection);
	else
		*this = rhs;
}


void 
dbSelection::union_with(oidT offset)
{
	assert (mSelection);  // we don't know how to construct a selection from just an oid!, we just own an abstract pointer
	mSelection->union_with(offset);
}


// WARNING as an inline these routines taking dbSelection& rhs had major problems
// with Visual C++ 4.1, jumping into assert.c whilst processing the arguments
void 
dbSelection::difference_with(const dbSelection& rhs)
{
	if (mSelection)
		mSelection->difference_with(rhs.mSelection);
}


void 
dbSelection::difference_with(dbTable& rhs)
{
	if (mSelection)
		mSelection->difference_with(rhs.currentSelection().mSelection);
}


bool 
dbSelection::isEmpty() const
{
	bool ret;
	if (mSelection)
		ret = mSelection->isEmpty();
	else
		ret = true;
	return ret;
}


bool 
dbSelection::isAllRecs() const
{
	bool ret;
	if (mSelection)
		ret = mSelection->isAllRecs();
	else
		ret = false;	// assume empty selection means no, although in strict terms it could be
	return ret;
}


bool 
dbSelection::contains(oidT theOID) const
{
	bool ret;
	if (mSelection)
		ret = mSelection->contains(theOID);
	else 
		ret = false;
	return ret;
}


unsigned long 
dbSelection::count() const
{
	unsigned long ret;
	if (mReferences) { // has been inited
		assert(mSelection);
		if (mSelection->isAllRecs() )
			ret = prototypicalTable()->countAll();
		else
			ret = mSelection->count();
	}
	else
		ret = 0;
		
	return ret;
}


// -------------------------------------------------------
//                s t S a v e S e l e c t i o n
// -------------------------------------------------------
stSaveSelection::stSaveSelection(dbTable* inTable) :
	mTable(inTable),
	mSavedSelection(mTable->currentSelection())
{
}

stSaveSelection::stSaveSelection(dbTable& inTable) :
	mTable(&inTable),
	mSavedSelection(mTable->currentSelection())
{
}


stSaveSelection::~stSaveSelection()
{
	mTable->setSelection(mSavedSelection);
}


// -------------------------------------------------------
//     s t I g n o r e D u p l i c a t e R e c o r d s
// -------------------------------------------------------
stIgnoreDuplicateRecords::stIgnoreDuplicateRecords(dbTable* inTable) :
	mTable(inTable)
{
	mTable->ignoreDuplicateRecords();
}

stIgnoreDuplicateRecords::stIgnoreDuplicateRecords(dbTable& inTable) :
	mTable(&inTable)
{
	mTable->ignoreDuplicateRecords();
}


stIgnoreDuplicateRecords::~stIgnoreDuplicateRecords()
{
	mTable->noticeDuplicateRecords();
}


// -------------------------------------------------------
//  s t S a v e S e l e c t i o n A n d S u p p r e s s S o r t
// -------------------------------------------------------
stSaveSelectionAndSuspendSort::stSaveSelectionAndSuspendSort(dbTable* inTable) :
	stSaveSelection(inTable)
{
	mTable->suspendSorting();
}


stSaveSelectionAndSuspendSort::~stSaveSelectionAndSuspendSort()
{
	mTable->resumeSorting();
}


// -------------------------------------------------------
//           s t S u p p r e s s S o r t
// -------------------------------------------------------
stSuspendSort::stSuspendSort(dbTable* inTable) :
	mTable(inTable)
{
	mTable->suspendSorting();
}


stSuspendSort::~stSuspendSort()
{
	mTable->resumeSorting();
}
