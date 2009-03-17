// COPYRIGHT 1997 A.D. Software, All rights reserved

// array classes for OOFILE database

// NOTE inline definitions included at end of this header file

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//  O O F _ E x p a n d a b l e L o n g A r r a y
// -------------------------------------------------------
OOF_ExpandableLongArray::OOF_ExpandableLongArray(
	unsigned long defaultValue, 
	unsigned long numSlots, 
	unsigned int expandBySlots
) :
	mBits(0),
	mDefaultValue(defaultValue),
	mInternalIter(0),
	mNextFreeEntry(0),
	mNumSlots(0),
	mExpansionChunk(expandBySlots)
{
	if (mExpansionChunk<1)
		mExpansionChunk = 1;
	if (numSlots) {
		mNumSlots = ((numSlots-1) / expandBySlots + 1) * expandBySlots;
		mBits = new unsigned long[mNumSlots];
		assert(mBits);
		for (unsigned long i=0; i<mNumSlots; i++)
			mBits[i] = defaultValue;
	}
}


OOF_ExpandableLongArray::OOF_ExpandableLongArray(const OOF_ExpandableLongArray& rhs) :
	mBits(0),
	mDefaultValue(rhs.mDefaultValue),
	mNextFreeEntry(rhs.mNextFreeEntry),
	mNumSlots(rhs.mNumSlots),
	mInternalIter(0),
	mExpansionChunk(rhs.mExpansionChunk)
{
	CopyBits(rhs);
}


OOF_ExpandableLongArray& 
OOF_ExpandableLongArray::operator=(const OOF_ExpandableLongArray& rhs) 
{
	if ((this == &rhs) || (mBits == rhs.mBits))
		return *this;
	
	mDefaultValue = rhs.mDefaultValue;
	mNextFreeEntry = rhs.mNextFreeEntry;	
	mNumSlots = rhs.mNumSlots;
	mInternalIter = 0;
	mExpansionChunk = rhs.mExpansionChunk;
	CopyBits(rhs);
	return *this;
}


OOF_ExpandableLongArray::~OOF_ExpandableLongArray() 
{
	DeleteBits();
}


#ifndef OOF_NO_STDLIB
void 
OOF_ExpandableLongArray::describe(ostream& os) const
{
	os << this << ": " << endl;
	const unsigned long numBits = count();
	for(unsigned long i=0; i<numBits; i++)
	{
		os << value(i) << ", " ; 

	}
	os << endl;
}
#endif


void 
OOF_ExpandableLongArray::DeleteBits() 
{
	delete[] mBits;
	mBits = 0;	// in case we get called other than at destruction
	mNumSlots = 0;
}


void 
OOF_ExpandableLongArray::CopyBits(const OOF_ExpandableLongArray& rhs)
{
	if (mBits) 
		DeleteBits();
	const unsigned long newSlots = rhs.mNumSlots;
	if (newSlots==0)
		mBits = 0;
	else {
		mBits = new unsigned long[newSlots];
		mNumSlots = newSlots;
		assert(mBits);
		memcpy(mBits, rhs.mBits, newSlots*sizeof(unsigned long));
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	}
	assert(Invariant());
}


void 
OOF_ExpandableLongArray::append(unsigned long aBit)  {
	(*this)[mNextFreeEntry] = aBit;
}


void 
OOF_ExpandableLongArray::deleteItem(unsigned long index)  {
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	for (unsigned long i=index; i<mNextFreeEntry; i++)
		mBits[i] = mBits[i+1];
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	mNextFreeEntry--;
	mBits[mNextFreeEntry] = mDefaultValue;
}


void 
OOF_ExpandableLongArray::deleteFirstMatching(unsigned long schValue)  
{
	unsigned long foundIndex;
	if (find(schValue, foundIndex))
		deleteItem(foundIndex);
}


void 
OOF_ExpandableLongArray::deleteAllMatching(unsigned long schValue)  
{
	for (unsigned long i=0; i<mNextFreeEntry;) { 	// relies on mNextFreeEntry being decremented by deleteItem
		if (value(i) == schValue) 
			deleteItem(i);	// and don't advance
		else
			i++;
	}
// NOT YET IMPLEMENTED - make smarter for large collections, incorporate delete
}


bool 
OOF_ExpandableLongArray::contains(unsigned long schValue)  const
{
	const unsigned long numEntries = mNextFreeEntry;
	for (unsigned long i=0; i<numEntries; i++) {
		if (value(i)==schValue) {
			return true;
		}
	}
	return false;
}


bool 
OOF_ExpandableLongArray::find(unsigned long schValue, unsigned long& foundAt)  const
{
	const unsigned long numEntries = mNextFreeEntry;
	for (unsigned long i=0; i<numEntries; i++) {
		if (value(i)==schValue) {
			foundAt = i;
			return true;
		}
	}
	return false;
}


unsigned long 
OOF_ExpandableLongArray::countOccurrences(unsigned long schValue)  const
{
	unsigned long ret = 0;
	const unsigned long numEntries = mNextFreeEntry;
	for (unsigned long i=0; i<numEntries; i++) {
		if (value(i)==schValue) 
			ret++;
	}
	return ret;
}


/**
	Effectively delete cells by resetting mNextFreeEntry cursor.
	\param alwaysDeleteStorage means unconditionally cleanup storage so can force total cleanup.
	\todo delete mBits if past threshold
*/
void 
OOF_ExpandableLongArray::deleteAllCells(bool alwaysDeleteStorage)  {
	mNextFreeEntry=0;
	if (alwaysDeleteStorage) 
		DeleteBits() ;
	assert(Invariant());
}


unsigned long& 
OOF_ExpandableLongArray::operator[](unsigned long index)
{
	assert(mNextFreeEntry <= mNumSlots);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	if (index >= mNumSlots)
		ExpandToInclude(index);
	if (index >= mNextFreeEntry)
		mNextFreeEntry = index+1;
	return mBits[index];
}


unsigned long 
OOF_ExpandableLongArray::value(unsigned long index) const
{
	unsigned long ret;
	if (index >= mNextFreeEntry)
		ret = mDefaultValue;
	else
		ret = mBits[index];
	return ret;
}


/**
	\todo platform-specific optimal copy for expansion
*/
void 
OOF_ExpandableLongArray::ExpandToInclude(unsigned long indexToCover)
{
// expand the storage to include the specified index
// (ie: expand to the next chunk multiple >= indexToCover
	const unsigned long newSize = (indexToCover / mExpansionChunk + 1) * mExpansionChunk;
	unsigned long* newBits = new unsigned long[newSize];
// NOT YET IMPLEMENTED - need error handling here

	assert(newBits);
	if (mBits) {
		const unsigned long numEntries = mNextFreeEntry;
		for (unsigned long i=0; i<numEntries; i++)
			newBits[i] = mBits[i];
		delete[] mBits;
	}
	mBits = newBits;
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	for (unsigned long i=mNextFreeEntry; i<newSize; i++)
		mBits[i] = mDefaultValue;
	mNumSlots = newSize;
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


// -------------------------------------------------------
//       o o f A r r a y
// -------------------------------------------------------
oofArray::oofArray(
	unsigned long slotSize,
	unsigned long /*numSlots*/, 
	unsigned int expandBySlots
) :
	mUserStorage(0),  // user may init in subclass's ctor
	mBits(0),
	mNextFreeEntry(0),
	mNumSlots(0),
	mSlotSize(slotSize),
	mExpansionChunk(expandBySlots)
{
	if (mExpansionChunk<1)
		mExpansionChunk = 1;
// unlike OOF_ExpandableLongArray we use virtual allocators
// therefore cannot allocate space in this base ctor
	}


oofArray::oofArray(const oofArray& rhs) :
	mUserStorage(0),  // user may init in subclass's ctor
	mNextFreeEntry(rhs.mNextFreeEntry),
	mNumSlots(rhs.mNumSlots),
	mInternalIter(0),
	mExpansionChunk(rhs.mExpansionChunk)
{
	mBits = rhs.CloneBits();  // rhs is actual subclass so can invoke virtual	
}


oofArray::~oofArray()
{
	delete[] mBits;  // user must set to zero if have DeleteBits
	// and use their own dtor!!!
}


oofArray& 
oofArray::operator=(const oofArray& rhs) 
{
	if ((this == &rhs) || (mBits == rhs.mBits))
		return *this;
	
	mNextFreeEntry = rhs.mNextFreeEntry;	
	mNumSlots = rhs.mNumSlots;
	mInternalIter = 0;
	mExpansionChunk = rhs.mExpansionChunk;

	DeleteBits(true /* copying */);
	mBits = rhs.CloneBits();  // rhs is actual subclass so can invoke virtual	
	if (mUserStorage)
		*mUserStorage = mBits;
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	assert(Invariant());
	return *this;
}


void*
oofArray::CloneBits() const
{
	// default implementation based on bit-level copy of contents
	// you may have to implement your own copy loop
	// using actual user array slot types
	void* ret = AllocBits(mNumSlots);
	const unsigned long numBytes = mNumSlots*mSlotSize;
	memcpy(ret, mBits, numBytes);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	return ret;
}


unsigned long 
oofArray::AllocSlot()
{
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	const unsigned long ret = mNextFreeEntry;
	if (ret >= mNumSlots || !mBits)  // on init, next is 0 but have no storage
		ExpandToInclude(ret);
	mNextFreeEntry++;
	return ret;
}


void 
oofArray::ExpandToInclude(unsigned long indexToCover)
{
// expand the storage to include the specified index
// (ie: expand to the next chunk multiple >= indexToCover
	const unsigned long newSize = (indexToCover / mExpansionChunk + 1) * mExpansionChunk;
	void* newBits = AllocBits(newSize);
// NOT YET IMPLEMENTED - need error handling here

	assert(newBits);
	if (mBits) {
		const unsigned long bytesToCopy = mNextFreeEntry*mSlotSize;
		memcpy(newBits, mBits, bytesToCopy);
		DeleteBits(true /* copying */);
	}
	mBits = newBits;
	if (mUserStorage)
		*mUserStorage = newBits;  // update their pointer
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	mNumSlots = newSize;
	assert(Invariant());
}

