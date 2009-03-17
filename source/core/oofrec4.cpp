// OOFILE record-oriented abstract backend implementation
// 
// COPYRIGHT 1994 A.D. Software, All rights reserved


#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFRECS
	#include "oofrecs.h"
#endif
#ifndef OOF_EXCEP
	#include "oofexcep.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//   O O F _ r e c o r d  B u f f e r e d C o n t e x t
// -------------------------------------------------------
OOF_recordBufferedContext::OOF_recordBufferedContext() :
	mRecordState(eUnloaded),
	mCurrLoadedRecOffset(0),
	mRecBufLen(0),
	mBuffer(0),
	mBlobFieldBodies(0)
{}


OOF_recordBufferedContext::OOF_recordBufferedContext(const OOF_recordBufferedContext& rhs) : 
	mRecordState(rhs.mRecordState),
	mCurrLoadedRecOffset(rhs.mCurrLoadedRecOffset),
	mRecBufLen(rhs.mRecBufLen),
	mBlobFieldBodies(0),
	mDirtyMap(rhs.mDirtyMap)
{
	assert(mRecBufLen);
	AllocBuffer();
	memcpy(mBuffer, rhs.mBuffer, mRecBufLen);
}


OOF_recordBufferedContext::~OOF_recordBufferedContext()
{
	delete[] mBuffer;
	char* blobContent;
	
	if (mBlobFieldBodies) {
		const unsigned int numBlobs = mBlobFieldBodies->count();
		for (unsigned int i=0; i<numBlobs; i++) {
			blobContent = (char*)mBlobFieldBodies->item(i);
			delete[] blobContent;	
		}
			
		delete mBlobFieldBodies;
	}
}


void 
OOF_recordBufferedContext::copyFrom(const OOF_recordBufferedContext& rhs)
{
	assert(mRecBufLen == rhs.mRecBufLen);
	mRecordState = rhs.mRecordState;
	mCurrLoadedRecOffset = rhs.mCurrLoadedRecOffset;
	assert(mBuffer);
	assert(rhs.mBuffer);
	memcpy(mBuffer, rhs.mBuffer, mRecBufLen);
}



const OOF_recordBufferedContext& 
OOF_recordBufferedContext::operator=(const OOF_recordBufferedContext& rhs)
{
	copyFrom(rhs);
	return *this;
}


bool
OOF_recordBufferedContext::operator==(const OOF_recordBufferedContext& rhs) const
{
	return (mCurrLoadedRecOffset==(unsigned long)rhs);
}


void
OOF_recordBufferedContext::AllocBuffer()
{	
	mBuffer = new char[mRecBufLen];		
	assert(mBuffer);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


// -------------------------------------------------------
//            O O F _ r e c o r d C a c h e
// -------------------------------------------------------
OOF_recordCache::OOF_recordCache() : 
	mDeletedCells(0)
{}


OOF_recordCache::OOF_recordCache(OOF_recordBufferedContext* buf) : 
	mDeletedCells(0)
{
	append(buf);
}


OOF_recordCache::~OOF_recordCache()
{
	assert(mBuffers.count()==0);
	if (mBuffers.count()!=0)  {
		dbConnect::raise("OOF_recordCache::~OOF_recordCache  mBuffers.count()>0 ", false);
	}
}
  

void 
OOF_recordCache::append(OOF_recordBufferedContext* buf)
{
	mBuffers.append((long)buf);
}


void 
OOF_recordCache::setItem(unsigned long index, OOF_recordBufferedContext* buf)
{
	mBuffers[index] = (long)buf; 
}


void 
OOF_recordCache::reuseDeletedItem(unsigned long index, OOF_recordBufferedContext* buf)
{
	assert(mDeletedCells);
	assert(mBuffers.value(index)==0);
	mDeletedCells--;
	setItem(index, buf); 
}


void 
OOF_recordCache::deleteRecord(unsigned long recOffset)
{
	unsigned long numCached = mBuffers.count();
	for (unsigned long i=0; i<numCached; i++) {
		OOF_recordBufferedContext* ctx = (OOF_recordBufferedContext*) (mBuffers.item(i));	// safe downcast
		if (ctx && ctx->currentOffset()==recOffset) {	// allow for nil entries in cache - concession to ram backend
			delete ctx;
			mBuffers[i] = 0;	// leave a hole
			mDeletedCells++;
			return;      //**** SUCCESSFUL EXIT *****
		}
	}
}


unsigned long 
OOF_recordCache::firstFree() const
{ 
	if (mDeletedCells) {
		unsigned long foundIndex;
		if (mBuffers.find(0, foundIndex))
			return foundIndex;
		else
			return kNotFound; 
	}
	else
		return kNotFound; 
}


void 
OOF_recordCache::deleteAll()
{
	const unsigned int numCached = count();
	for (unsigned int i=0; i<numCached; i++) {
		OOF_recordBufferedContext* ctx = item(i);
		delete ctx;
	}
	mBuffers.deleteAllCells();  // above loop deletes pointers in the cells
}



OOF_recordBufferedContext* 
OOF_recordCache::item(unsigned long index)
{ 
	return (OOF_recordBufferedContext*) (mBuffers.value(index)); 
}


unsigned long 
OOF_recordCache::containsContext(const OOF_recordBufferedContext* inCtx) const
{
	unsigned long numCached = mBuffers.count();
	for (unsigned long i=0; i<numCached; i++) {
		const OOF_recordBufferedContext* ctx = (OOF_recordBufferedContext*) (mBuffers.value(i));	// safe downcast
		assert(ctx);
		if (*ctx==*inCtx)  {
			return i;      //**** SUCCESSFUL EXIT *****
		}
	}
	return kNotFound;
}


unsigned long 
OOF_recordCache::containsOffset(unsigned long recOffset) const
{
	unsigned long numCached = mBuffers.count();
	for (unsigned long i=0; i<numCached; i++) {
		const OOF_recordBufferedContext* ctx = (const OOF_recordBufferedContext*) (mBuffers.value(i));	// safe downcast
		assert(ctx);
		if (*ctx==recOffset) 
			return i;      //**** SUCCESSFUL EXIT *****
	}
	return kNotFound;
}


unsigned long
OOF_recordCache::countNew() const
{
	unsigned long numCached = mBuffers.count();
	unsigned long numNew = 0;
	for (unsigned long i=0; i<numCached; i++) {
		const OOF_recordBufferedContext* ctx = (const OOF_recordBufferedContext*) (mBuffers.value(i));	// safe downcast
		assert(ctx);
		if (ctx->isNew())
			numNew++;
	}
	return numNew;
} 


OOF_recordBufferedContext* 
OOF_recordCache::orphanBuffer(unsigned long cellNo)
{
	OOF_recordBufferedContext*  buf = (OOF_recordBufferedContext* ) mBuffers[cellNo];
	mBuffers.deleteItem(cellNo);
	return buf;
}




  
// -------------------------------------------------------
//      O O F _ r e c o r d S e l R e c s R e p
//                 see also oofrec2.cpp
// -------------------------------------------------------
OOF_recordSelRecsRep::OOF_recordSelRecsRep(unsigned long numSlots, unsigned int expandBySlots) :
		OOF_ExpandableLongArray(0, numSlots, expandBySlots)
{}

 
// -------------------------------------------------------
//       O O F _ r e c o r d S e l e c t i o n R e p
// -------------------------------------------------------
OOF_recordSelectionRep::OOF_recordSelectionRep(const EselectionState inState, unsigned long numSlots) :
	mState(inState),
	mSorter(0)
{
	if (numSlots) {
		mRecsRep = new OOF_recordSelRecsRep(numSlots, kDefExpansionChunk);
		assert(mRecsRep);
	}
	else
		mRecsRep = 0;
	assert(Invariant());
}


OOF_recordSelectionRep::OOF_recordSelectionRep(unsigned long numSlots, unsigned int expandBySlots) :
	mState(empty),
	mSorter(0)
{
	if (numSlots) {
		mRecsRep = new OOF_recordSelRecsRep(numSlots, expandBySlots);
		assert(mRecsRep);
	}
	else
		mRecsRep = 0;
	assert(Invariant());
}


OOF_recordSelectionRep::OOF_recordSelectionRep(const OOF_recordSelectionRep& rhs) :
	mCurrSingleRecOffset(rhs.mCurrSingleRecOffset),
	mState(rhs.mState),
	mRecsRep(rhs.mRecsRep),
	mSorter(0)
{
	if (mRecsRep)
		mRecsRep->mReferences++;
	assert(Invariant());
}


OOF_recordSelectionRep& 
OOF_recordSelectionRep::operator=(const OOF_recordSelectionRep& rhs)
{
	if (this == &rhs)
		return *this;
	
	if (mRecsRep != rhs.mRecsRep) {
		if (mRecsRep && (--mRecsRep->mReferences)<=0)
			delete mRecsRep;
		mRecsRep = rhs.mRecsRep;
		if (mRecsRep)
			mRecsRep->mReferences++;
	}
	mCurrSingleRecOffset = rhs.mCurrSingleRecOffset;
	mState = rhs.mState;
	
	if (rhs.mSorter) { // get sort order from rhs
		delete mSorter;	// don't care what type of sorter we have
		mSorter = rhs.mSorter->clone();
	} // rhs is sorted (or has sort order and need sorting!)
	else
		SelectionChanged();	// ***** IF WE ARE SORTED, WE STAY SORTED EVEN IF ASSIGNED AN UNSORTED SELECTION
	
	assert(Invariant());
	return *this;
}


OOF_recordSelectionRep::~OOF_recordSelectionRep() 
{
	if (mRecsRep && (--mRecsRep->mReferences)<=0)
		delete mRecsRep;
	delete mSorter;
	assert(Invariant());
}



void 
OOF_recordSelectionRep::CopyContents(const OOF_recordSelectionRep* rhs)
{
	DropSelection();
	mRecsRep = new OOF_recordSelRecsRep(*(rhs->mRecsRep));  
	if (mSorter) { // we're sorted
		if (rhs->mSorter) { // but they may not be
			if (*mSorter != rhs->mSorter)	// if rhs is sorted same as us, nothing to do
				mSorter->selectionChanged();
		}
		else
			mSorter->selectionChanged();
	} // we are sorted
}
 

void 
OOF_recordSelectionRep::DropSelection() 
{
	if (mState != empty) {
		mState = empty;
		if (mRecsRep) {
			mRecsRep->mReferences--;
			if (mRecsRep->mReferences <= 0)
				delete mRecsRep;
			mRecsRep = 0;
		}	// unlink & maybe delete shared rep
	}	// change state
} 


void
OOF_recordSelectionRep::AboutToDirtySelection()
{
	if (!mRecsRep || mRecsRep->mReferences<=1)
		return;  // don't care
		
// enforce the copy-on-write behaviour		
	OOF_recordSelRecsRep*	newRep = new OOF_recordSelRecsRep(*mRecsRep);	
	assert(newRep);
	mRecsRep->mReferences--;
	mRecsRep = newRep;
}


unsigned long* 
OOF_recordSelectionRep::AllocSelection(unsigned long numSlots) 
{
	if (numSlots) {
		if (mRecsRep && (--mRecsRep->mReferences)<=0)
			delete mRecsRep;
		mRecsRep = new OOF_recordSelRecsRep(numSlots);
		assert(mRecsRep);
	}
	mState=someRecs;
	return ExposeArray();
} 


void
OOF_recordSelectionRep::ExpandToInclude(unsigned long indexToCover) 
{ 
	if (mRecsRep)
		mRecsRep->ExpandToInclude(indexToCover);
	else
		AllocSelection(indexToCover+1);
}


unsigned long* 
OOF_recordSelectionRep::AllocConsumedSelection(unsigned long numSlots) 
{
	unsigned long* ret = AllocSelection(numSlots);
	MarkAsUsed(numSlots);
	return ret;
} 


void 
OOF_recordSelectionRep::SelectNone()
{
	DropSelection();
	mState = empty;
	assert(Invariant());
}


void 
OOF_recordSelectionRep::SelectOneRec(unsigned long rec)
{
	DropSelection();
	mState = oneRec;
	mCurrSingleRecOffset = rec;
	assert(Invariant());
}


void 
OOF_recordSelectionRep::SelectAll()
{
	DropSelection();
	mState = allRecs;
	SelectionChanged();
	assert(Invariant());
}

 
void 
OOF_recordSelectionRep::SelectSome()
{
	mState = someRecs;
	SelectionChanged();
	assert(Invariant());
} 


void 
OOF_recordSelectionRep::setSortOrder(dbSorter* adoptSorter)
{
	delete mSorter;
	mSorter = adoptSorter;
}


dbSorter* 
OOF_recordSelectionRep::sorter() const
{
	return mSorter;
}


unsigned long& 
OOF_recordSelectionRep::item(unsigned long index)
{
	if (mState == oneRec) {
		assert(index == 0);
		return mCurrSingleRecOffset;
	}
	assert(mState != empty);
	assert(mRecsRep);
	return mRecsRep->item(index);
}



unsigned long
OOF_recordSelectionRep::value(unsigned long index) const
{
	if (mState == oneRec) {
		assert(index == 0);
		return mCurrSingleRecOffset;
	}
	assert(mState != empty);
	assert(mRecsRep);
	return mRecsRep->value(index);
}


void 
OOF_recordSelectionRep::Append(unsigned long n)
{
	switch(mState)
	{
	case empty:	// doesn't imply the database is empty, thus can't transition to AllRecords
		mCurrSingleRecOffset=n;
		mState = oneRec;
		break;
	
	case oneRec:
		AllocSelection(2);
		mRecsRep->append(mCurrSingleRecOffset);
		mRecsRep->append(n);		
		mState = someRecs;
		break;
	
	case someRecs:
		mRecsRep->append(n);
		SelectionChanged();		
		break;
	
	case allRecs:
		break;
	}
}


void 
OOF_recordSelectionRep::DeleteItem(unsigned long index)
{
	assert(mRecsRep);
	assert(index < mRecsRep->count());
	mRecsRep->deleteItem(index);
}


void
OOF_recordSelectionRep::SetItemOffset(unsigned long index, unsigned long newOffset) 
{ 
	if (mState==oneRec)
		mCurrSingleRecOffset = newOffset;
	else {
		assert(mRecsRep);
		assert(mState==someRecs);
		mRecsRep->item(index) = newOffset;
	}
	assert(Invariant());
} 


unsigned long 
OOF_recordSelectionRep::Count() const
{
	assert(Invariant());
	unsigned long ret = 0;
	if (mState == oneRec)
		ret = 1;
	else {	
		if (mRecsRep)  // implies someRecs or maybe allRecs with special case
			ret = mRecsRep->count(); 
	}	
	assert(Invariant());
	return ret;
} 


bool 
OOF_recordSelectionRep::Invariant() const
{
// Jesse's check which we don't hold - it *is* valid for nill mRecsRep in an allRecs
//        if (mState == allRecs)
//              ASSERT(mRecsRep != nil);

// we will always call from within another check, so just set a return status here
	bool ret = true;
	if (mState != allRecs) {	// don't care about mRecsRep in allRecs
		if (mState == someRecs) {
			ret = (mRecsRep != 0);	// *must* have a selection
        }
		else if (mState == oneRec) {
        	if (mRecsRep)
				ret = (mRecsRep->count() == 1);
        } 
		else {   // (mState == empty) 
        	if (mRecsRep)
            	ret = (mRecsRep->count() == 0);
		}
	}
	return ret;
}



bool 
OOF_recordSelectionRep::IsMember(unsigned long item) const
{
	unsigned long ignorePos;
	return Find(item, ignorePos);
}


bool 
OOF_recordSelectionRep::Find(unsigned long item, unsigned long& foundAt) const
{
	bool ret = false;
	foundAt = 0;
	switch(mState)
	{
	case empty:
		break;
	
	case oneRec:
		assert(mCurrSingleRecOffset>0);
		ret=((unsigned long) mCurrSingleRecOffset==item);
		break;
	
	case someRecs: {
// NOTE don't use iterators - this does NOT affect state
	   	assert(mRecsRep);
		unsigned long numItems = mRecsRep->count();
		unsigned long i;
		for (i=0; (i<numItems) && !ret; i++)
			ret=(item==mRecsRep->value(i));
		foundAt = i-1;
		break;
	}
	
	case allRecs:
		ret = true;
		break;
	}
	return ret;
}


// -------------------------------------------------------
//               O O F _ r e c o r d S e l e c t i o n
//                 see also oofrec2.cpp
// -------------------------------------------------------
OOF_recordSelection::OOF_recordSelection(dbTable* prototypicalTable) :
#ifdef OOF_DEBUG
	oofSingleListener(prototypicalTable),
#endif
	mInternalIter(0),
	mNextFakeNewRecOffset(OOF_FIRST_SYNTHETIC_RECORD_OFFSET),
	mPrototypicalTable(prototypicalTable)
{
	mRep = new OOF_recordSelectionRep();
	assert(mRep);
}


OOF_recordSelection::OOF_recordSelection(const EselectionState selState, unsigned long numSlots, dbTable* prototypicalTable) :
#ifdef OOF_DEBUG
	oofSingleListener(prototypicalTable),
#endif
	mInternalIter(0),
	mNextFakeNewRecOffset(OOF_FIRST_SYNTHETIC_RECORD_OFFSET),
	mPrototypicalTable(prototypicalTable)
{
		mRep = new OOF_recordSelectionRep(selState, numSlots);
		assert(mRep);
}


OOF_recordSelection::OOF_recordSelection(unsigned long numSlots, dbTable* prototypicalTable) :
#ifdef OOF_DEBUG
	oofSingleListener(prototypicalTable),
#endif
	mInternalIter(0),
	mNextFakeNewRecOffset(OOF_FIRST_SYNTHETIC_RECORD_OFFSET),
	mPrototypicalTable(prototypicalTable)
{ 
	mRep = new OOF_recordSelectionRep(numSlots);
	assert(mRep);
}


OOF_recordSelection::OOF_recordSelection(unsigned long numSlots, unsigned int expansionChunk, dbTable* prototypicalTable) :
#ifdef OOF_DEBUG
	oofSingleListener(prototypicalTable),
#endif
	mInternalIter(0),
	mNextFakeNewRecOffset(OOF_FIRST_SYNTHETIC_RECORD_OFFSET),
	mPrototypicalTable(prototypicalTable)
{
	mRep = new OOF_recordSelectionRep(numSlots, expansionChunk);
	assert(mRep);
}


OOF_recordSelection::OOF_recordSelection(const OOF_recordSelection& rhs) :
#ifdef OOF_DEBUG
	oofSingleListener(rhs),
#endif
	mInternalIter(rhs.mInternalIter),
	mNextFakeNewRecOffset(rhs.mNextFakeNewRecOffset),
	mPrototypicalTable(rhs.mPrototypicalTable)
{
// default copy semantics do NOT share selection, only explicit clone via copyFrom
	mRep = new OOF_recordSelectionRep(*rhs.mRep);
	assert(mRep);
}


OOF_recordSelection& 
OOF_recordSelection::operator=(const OOF_recordSelection& rhs)
{
	if (this == &rhs)
		return *this;
	
	assert(mPrototypicalTable==rhs.mPrototypicalTable);
	mInternalIter=rhs.mInternalIter;
	mNextFakeNewRecOffset = rhs.mNextFakeNewRecOffset;

// default copy semantics do NOT share selection, only explicit clone via copyFrom
// so ignore the fact that the rhs may be shared.
// however, as OUR rep may be, we must update the shared copy, not simply
// replace it!

// see the discussion in copyFrom()
	if (!mRep)
		mRep = new OOF_recordSelectionRep(*rhs.mRep);
	else
		*mRep = *rhs.mRep;
	return *this;
}


void 
OOF_recordSelection::copyFrom(const OOF_recordSelection& rhs, dbTable::selSharingT selSharing)
{
// Shared selections share at the OOF_recordSelection level, sharing a ref-counted
// OOF_recordSelectionRep. These occur because we've explicitly cloned a dbTable to
// act as a different iterator over the same selection. Actions on one table affect
// the selection of the other. eg: dbViews by default will iterate independently
// of the original table but have a shared selection.

// We also have an optimisation for copyOnWrite which makes this a little confusing,
// because there are two levels of reference counting occuring.

// A OOF_recordSelectionRep will avoid copying the OOF_recordSelRecsRep until it needs
// to. This optimisation saves a lot of copying of a potentially large array - as we
// create new selections we are likely to change their context so will create a new
// set of records (eg: via a search on a table after cloning).

	mInternalIter=rhs.mInternalIter;
	mNextFakeNewRecOffset = rhs.mNextFakeNewRecOffset;

	if (mRep != rhs.mRep) {
		if (mRep && (--mRep->mReferences)<=0)
			delete mRep;
		if (selSharing==dbTable::selShared) {
			mRep = rhs.mRep;
			assert(mRep);
			mRep->mReferences++;
		}
		else {
			assert(selSharing==dbTable::selCopyOnWrite);	// should not get to this method if selNotCopied
			mRep = new OOF_recordSelectionRep(*rhs.mRep);
			assert(mRep);
		}
	}
}


OOF_recordSelection::~OOF_recordSelection() 
{
	if ((--mRep->mReferences)<=0)
		delete mRep;
}


OOF_Selection*
OOF_recordSelection::clone() const
{
	return new OOF_recordSelection(*this);
}


#ifdef OOF_DEBUG
	bool 
	OOF_recordSelection::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
	{
		if (msg==OOFmsg_BroadcasterClosing) { 
			mPrototypicalTable = 0;  // will cause asserts if try to use now
			mListensTo = 0;
			/*
			It's not an error for the selection to persist beyond the table, 
			provided it is not used. (We must allow for variable scope to 
			cause deletes out of order!)
			*/
		}
		return true;
	}
#endif


void
OOF_recordSelection::describeRecsRep(ostream& logStream) const
{
	if(mRep->mRecsRep)
		mRep->mRecsRep->describe(logStream);
	else 
		logStream << this << ":  NULL mRecsRep" << endl;
}

void 
OOF_recordSelection::setSortOrder(dbSorter* adoptSorter)
{
	mRep->setSortOrder(adoptSorter);
}


dbSorter* 
OOF_recordSelection::sorter() const
{
	return mRep->sorter();
}


void 
OOF_recordSelection::adopt(OOF_recordSelection& rhs)
{
	*this = rhs;
	rhs.DropSelection();
}
 

void 
OOF_recordSelection::DropSelection() 
{
	mInternalIter = 0;
	mNextFakeNewRecOffset = OOF_FIRST_SYNTHETIC_RECORD_OFFSET;	// reset to stop growing to realistic values
	mRep->DropSelection();
} 


void 
OOF_recordSelection::deleteCurrent()
{
	if (state()==oneRec) 
		selectNone();
	else 
		if (state()==someRecs) {
			mRep->DeleteItem(mInternalIter);	
			if (count() == 1)
				selectOneRec(value(0));
			else
				if (mInternalIter >= count() )  // was last record
					mInternalIter = 0; // so cycle back to beginning
		}
}


void 
OOF_recordSelection::deleteItem(unsigned long index)
{
	if (state()==oneRec) {
		if (index==0)
			selectNone();
		else {
			RAISE_EXCEPTION(oofE_Selection(stringstream() << flush 
				<< "OOF_recordSelection::deleteItem - attempt to delete "
				<< index << " in selection of 1 record")
			);
		}
	}
	else 
		if (state()==someRecs) {
			mRep->DeleteItem(index);	
			if (count() == 1)
				selectOneRec(value(0));
			else
				if (mInternalIter > count() )  // was last record
					mInternalIter--;
		}
}


/**
	State transition and iterator update when dbTable::newRecord() called.
	\return mNextFakeNewRecOffset 
	\param appendIt indicates we should append mNextFakeNewRecOffset and increment it.
	
	Only append new records to the selection if we expect to iterate the selection and keep
	new records around. This only applies if the save mode is requireExplicitAndBuffer
	otherwise it is impossible to move off the new record without either saving, unloading or
	an error. someBackend::SaveContext() will call OOF_recordSelection::append in those cases
	or OOF_recordSelection::updateCurrent if we've already appended a (synthetic) new record No.
	
	In the normal use of a database, newRecord will occur many times and the fake rec offset is
	reused over and over. Otherwise we have problems with them hitting a boundary, either colliding
	with real offsets (if started at 0) or end of numeric range. If we put them into the selection,
	how does it get updated with the real offset?
	
	\todo work out if it is safe to always append the fake offsets to selection and implications 
	such as having to increment mNextFakeNewRecOffset, complicating normal newRecord case etc.
*/
unsigned long 
OOF_recordSelection::appendNewRecord(bool appendIt)
{
	unsigned long ret = mNextFakeNewRecOffset;  // used to keep them unique in case put in cache
	assert(mNextFakeNewRecOffset < ULONG_MAX);
	if (appendIt) {
		mNextFakeNewRecOffset++;
		append(ret);
		assert(state()!=empty);  // state transition in append
	
		if (state()==allRecs)
			mInternalIter = ULONG_MAX;
		else 
			mInternalIter = count()-1;
	}
	else 
		mInternalIter = ULONG_MAX;	// if didn't append don't behave as if we did!
	return ret;
}


void
OOF_recordSelection::updateCurrent(unsigned long newOffset) 
{ 
	assert((state()==oneRec) || (mInternalIter <= count()));
	mRep->SetItemOffset(mInternalIter, newOffset);
} 


void 
OOF_recordSelection::describeState(ostream& os) const
{
	os 	<< "Iter: " << mInternalIter;
	if (mRep) { 
		os	<< "\tState: " << mRep->mState;
		if (mRep->mState==oneRec)
			os  << '\t' << mRep->mCurrSingleRecOffset;
		else
			if (mRep->mState==someRecs)
				os  << '\t' << mRep->Count();
	}
	os << endl;
}


const dbTable* 
OOF_recordSelection::prototypicalTable() const
{
	return mPrototypicalTable;
}
