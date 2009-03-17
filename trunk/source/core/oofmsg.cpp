// COPYRIGHT 1996 A.D. Software, All rights reserved

// messaging layer of OOFILE database

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
//                o o f B r o a d c a s t e r 
// -------------------------------------------------------
oofBroadcaster::oofBroadcaster() :
	mReceivers(0),
	mNestedBroadcastingDepth(0),
	mLostReceiversWhileBroadcasting(false)
{}


oofBroadcaster::oofBroadcaster(const oofBroadcaster&) :
	mReceivers(0),
	mNestedBroadcastingDepth(0),
	mLostReceiversWhileBroadcasting(false)
{}


oofBroadcaster::~oofBroadcaster()
{
	if (mReceivers) {
		broadcast(OOFmsg_BroadcasterClosing);
		delete mReceivers;
	}
}


/**
	Broadcast a message to all anonymous subscribers.
	\param msg constant value indicating the action or event that has occurred.
	\param senderDefined additional param for caller to pass through
	\param skipping a receiver to be skipped when broadcasting, to avoid circular messages
	\todo use void* for senderDefined so can cope with values >32bits eg oidT in HUGE databases or 64bit pointers.
*/
void 
oofBroadcaster::broadcast(OOFmsgT msg, unsigned long senderDefined, const oofReceiver* skipping)
{
	if (!mReceivers) 
		return;
		
	mNestedBroadcastingDepth++;	// in case of receivers removing themselves - see removeReceiver
	const unsigned short numReceivers = mReceivers->count();
	if (skipping) {
		for (unsigned short i=0; i<numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr!=skipping) // implies check rcvr!=NULL
				rcvr->receiveMsg(msg, senderDefined);
		}
	}
	else { // copy of loop logic to avoid repeated if (skipping) inside loop
		for (unsigned short i=0; i<numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr)
				rcvr->receiveMsg(msg, senderDefined);
		}
	}

	mNestedBroadcastingDepth--;	// but may be in nested broadcasts so
	// don't compact receivers until it hits 0
	if (mNestedBroadcastingDepth==0 && mLostReceiversWhileBroadcasting) {
		mLostReceiversWhileBroadcasting = false;
		mReceivers->deleteAllMatching(0);
	}
}


oofReceiver* 
oofBroadcaster::askAllStoppingAtFirst(bool stopOn, OOFmsgT msg, unsigned long senderDefined, const oofReceiver* skipping)
{
	if (!mReceivers) 
		return 0;
		
	const unsigned short numReceivers = mReceivers->count();
	if (skipping) {
		for (unsigned short i = 0; i < numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr!=skipping) {  // implies check rcvr!=NULL
				const bool reaction = rcvr->receiveMsg(msg, senderDefined);
				if (reaction == stopOn)
					return rcvr;
			}
		}
	}
	else {// copy of loop logic to avoid repeated if (skipping) inside loop
		for (unsigned short i = 0; i < numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr) {// nested broadcast may have removed
				const bool reaction = rcvr->receiveMsg(msg, senderDefined);
				if (reaction == stopOn)
					return rcvr;
			}
		}
	}
	return 0;
}


bool 
oofBroadcaster::askAllReturningAny(bool returnAny, OOFmsgT msg, unsigned long senderDefined, const oofReceiver* skipping)
{
	if (!mReceivers) 
		return false;
		
	bool foundAny = false;
	const unsigned short numReceivers = mReceivers->count();
	if (skipping) {
		for (unsigned short i=0; i<numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr!=skipping) {// implies check rcvr!=NULL
				const bool ret = rcvr->receiveMsg(msg, senderDefined);
				if (ret==returnAny)
					foundAny = true;
			}
		}
	}
	else {// copy of loop logic to avoid repeated if (skipping) inside loop
		for (unsigned short i=0; i<numReceivers; i++) {
			oofReceiver* rcvr = (oofReceiver*) (mReceivers->value(i));  // safe downcast
			if (rcvr) {// nested broadcast may have removed
				const bool ret = rcvr->receiveMsg(msg, senderDefined);
				if (ret==returnAny)
					foundAny = true;
			}
		}
	}
	return foundAny;
}

void 
oofBroadcaster::addReceiver(oofReceiver* r)
{
	if (!mReceivers) {
		 mReceivers = new OOF_ExpandableLongArray(1);
		 assert(mReceivers);
	}
	mReceivers->append((unsigned long) r);
}


void 
oofBroadcaster::removeReceiver(const oofReceiver* rcvrPtr)
{
	assert(mReceivers);	// very weird for someone to think they are subscribed in this case
	if (mNestedBroadcastingDepth) {
		mLostReceiversWhileBroadcasting = true;
		unsigned long foundIndex;
		if (mReceivers->find((unsigned long) rcvrPtr, foundIndex)) {
			mReceivers->item(foundIndex) = 0;
		}
		else
			dbConnect::raise("Attempt to remove non-existent receiver", false);
	}
	else
		mReceivers->deleteFirstMatching((unsigned long) rcvrPtr);
}


// -------------------------------------------------------
//                      d b R e c e i v e r
// -------------------------------------------------------
void
oofReceiver::subscribeTo(oofBroadcaster* broad)
{
	assert(broad);
	broad->addReceiver(this);
}


void
oofReceiver::unsubscribe(oofBroadcaster* broad)
{
	broad->removeReceiver(this);
}




// -------------------------------------------------------
//         d b S i n g l e L i s t e n e r
// -------------------------------------------------------
oofSingleListener::oofSingleListener(oofBroadcaster* listensTo) :
		mListensTo(listensTo),
		mSuspendedListensTo(0),
		mSuspendCount(0)
{
	if (listensTo)
		listensTo->addReceiver(this);
}


oofSingleListener::oofSingleListener(const oofSingleListener& rhs) :
		mListensTo(rhs.mListensTo),
		mSuspendedListensTo(rhs.mSuspendedListensTo),
		mSuspendCount(rhs.mSuspendCount)
{
	if (mListensTo)
		mListensTo->addReceiver(this);
}


/**
The default receiveMsg behaviour is to delete yourself when the
broadcaster closes. In many user subclasses oofSingleListener is a mixin
and you have a class owned elsewhere, not self-deleting.

However it is still important to set mListensTo=0 otherwise when you
finally get around to deleting your oofSingleListener, it will try to
update a deleted oofBroadcaster and cause a crash.
*/
bool 
oofSingleListener::receiveMsg(OOFmsgT msg, unsigned long /*senderDefined*/)
{
	if (msg==OOFmsg_BroadcasterClosing) { 
		mListensTo = 0;  // *** EITHER CLEAR mListensTo or call THIS BASE METHOD ***
		delete this;     
	}
	return true;
}


oofSingleListener::~oofSingleListener() 
{
	unsubscribe();
} 


void
oofSingleListener::subscribeTo(oofBroadcaster* broad)
{
	assert(broad);
	if (mListensTo)
		oofReceiver::unsubscribe(mListensTo);
	mListensTo = broad;
	broad->addReceiver(this);
}


void
oofSingleListener::unsubscribe(oofBroadcaster* broad)
{
	if (broad) {
		assert(broad==mListensTo);
		;
	}
	if (mListensTo)
		oofReceiver::unsubscribe(mListensTo);
	mListensTo = 0;
}


void
oofSingleListener::suspendListening()
{
	mSuspendCount++;
	if (mSuspendCount==1) {
		mSuspendedListensTo = mListensTo;
		unsubscribe();
	}
	else {
		assert(mSuspendedListensTo);
#ifdef OOF_DEBUG
// sanity check on nested suspends
		if (mSuspendCount > 1000){
			dbConnect::raise("You've nested more than 1000 suspend listenings");
		}
#endif
		;
	}
}


void
oofSingleListener::resumeListening()
{
	if (mSuspendedListensTo==0)
		return; // let them get away with being sloppy about extra resumes
	assert(mSuspendCount>0);
	mSuspendCount--;
	if (mSuspendCount==0) {
		subscribeTo(mSuspendedListensTo);
		mSuspendedListensTo = 0;
	}
}
  
