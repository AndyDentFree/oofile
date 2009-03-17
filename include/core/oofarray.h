#ifndef H_OOFARRAY
#define H_OOFARRAY

// COPYRIGHT 1997 A.D. Software, All rights reserved

// array classes for OOFILE database

// NOTE inline definitions included at end of this header file

#ifndef H_OOF0
	#include "oof0.h"
#endif
#if !defined(OOF_NO_STDLIB) && !defined(H_OOFIOS)
	#include "oofios.h"  // just to provide ostream for OOF_ExpandableLongArray::describe(ostream&)
#endif

/**
	Array of longs which expands automatically as you write to cells.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_ExpandableLongArray {		// this would be a good candidate to template-ize
public:
	OOF_ExpandableLongArray(unsigned long defaultValue=0, unsigned long numSlots=0, unsigned int expandBySlots=4);
	OOF_ExpandableLongArray(const OOF_ExpandableLongArray& rhs);
	~OOF_ExpandableLongArray();

	OOF_ExpandableLongArray& operator=(const OOF_ExpandableLongArray&);
#ifndef OOF_NO_STDLIB
	void describe(ostream&) const;
#endif

// access protocol
	void append(unsigned long);
	void deleteItem(unsigned long index);
	void deleteFirstMatching(unsigned long value);
	void deleteAllMatching(unsigned long value);
	void deleteAllCells(bool alwaysDeleteStorage=false);
	bool contains(unsigned long value) const;
	bool find(unsigned long value, unsigned long& foundAt) const;
	unsigned long& operator[](unsigned long index);
	unsigned long value(unsigned long index) const;
	unsigned long& item(unsigned long index);


	// iterator protocol
	void start();
	bool more() const;
	void next();
	unsigned long count() const;
	unsigned long index() const;
	unsigned long countOccurrences(unsigned long) const;
	unsigned long& operator()();

protected:
	bool Invariant() const;
	void ExpandToInclude(unsigned long indexToCover);
	void DeleteBits();
	void CopyBits(const OOF_ExpandableLongArray& rhs);  // used in copy constructor & operator=

// data storage
	unsigned long* mBits;	// owned
	unsigned long mDefaultValue;
	unsigned long mNextFreeEntry, mNumSlots, mInternalIter;
	unsigned int mExpansionChunk;

};
typedef OOF_ExpandableLongArray OOF_ULArray;


/**
	base class for user arrays.
	used instead of templatizing arrays.
	Works very much like OOF_ExpandableLongArray.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofArray {		
public:
	oofArray(unsigned long slotSize, unsigned long numSlots=0, unsigned int expandBySlots=4);
	oofArray(const oofArray& rhs);
	virtual ~oofArray();
	//  to invoke your DeleteBits must override dtor and
	// set mBits to 0 to avoid double-delete

	oofArray& operator=(const oofArray&);
	
	// iterator protocol
	void start();
	bool more() const;
	void next();
	unsigned long count() const;

protected:
	bool Invariant() const;
	unsigned long AllocSlot();
	void ExpandToInclude(unsigned long indexToCover);

// user overrides 
	virtual void* AllocBits(unsigned long) const=0;
	virtual void DeleteBits(bool becauseExpanding)=0;
	virtual void* CloneBits() const;  // has default implementation based on bit-level copy

// data storage
	void** mUserStorage;  // optional so user can have typed pointer
	void* mBits;	// owned
	unsigned long mNextFreeEntry, mNumSlots, mInternalIter, mSlotSize;
	unsigned int mExpansionChunk;
	
};

// -------------------------------------------------------
//  O O F _ E x p a n d a b l e L o n g A r r a y
// -------------------------------------------------------

/**
	Guarantee members managing storage are consistent.
	Note: we assume a user may cause mInternalIter to go out of bounds.
*/
inline bool 
OOF_ExpandableLongArray::Invariant() const
{
	return (mBits==0 || (mNumSlots>0 && mNextFreeEntry <= mNumSlots));
}


inline unsigned long 
OOF_ExpandableLongArray::count() const
{
	return mNextFreeEntry;
}


inline unsigned long 
OOF_ExpandableLongArray::index() const
{
	return mInternalIter;
}


inline void 
OOF_ExpandableLongArray::start()
{
	mInternalIter = 0;
}


inline bool 
OOF_ExpandableLongArray::more() const
{
	return mInternalIter < mNextFreeEntry;
}


inline void 
OOF_ExpandableLongArray::next()
{
	mInternalIter++;
}


inline unsigned long& 
OOF_ExpandableLongArray::operator()()
{ 
	return operator[](mInternalIter); 
}


inline unsigned long& 
OOF_ExpandableLongArray::item(unsigned long index)
{ 
	return operator[](index); 
}



// -------------------------------------------------------
//      o o f A r r a y
// -------------------------------------------------------
/**
	Guarantee members managing storage are consistent.
	Note: we assume a user may cause mInternalIter to go out of bounds.
*/
inline bool 
oofArray::Invariant() const
{
	return (mBits==0 || (mNumSlots>0 && mNextFreeEntry <= mNumSlots));
}


inline unsigned long 
oofArray::count() const
{
	return mNextFreeEntry;
}


inline void 
oofArray::start()
{
	mInternalIter = 0;
}


inline bool 
oofArray::more() const
{
	return mInternalIter < mNextFreeEntry;
}


inline void 
oofArray::next()
{
	mInternalIter++;
}


#endif

