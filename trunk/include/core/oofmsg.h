#ifndef H_OOFMSG
#define H_OOFMSG
// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOF0
	#include "oof0.h"	
#endif

#if defined(_Windows)
	#ifndef MessageT
		#define MessageT DWORD
	#endif
// use #defines instead of typedefs so safe against multiple encounters
// when colliding with defs in OOFILE
	#ifndef Int32
		#define Int32		signed long
	#endif
	#ifndef Uint16
		#define Uint16		unsigned short
	#endif
#endif

/**
	\file oofmsg.h 
	messaging layer of OOFILE database.
	\ingroup oofUtils
*/

//---- following are internal database broadcasts using oofBroadcaster::broadcast
typedef unsigned long OOFmsgT;	// use long so can combine with PowerPlant and other 4char/long messages
const OOFmsgT OOFmsg_None				= 0;
const OOFmsgT OOFmsg_ChangeSelection	= 1;
const OOFmsgT OOFmsg_ChangeContext			= OOFmsg_ChangeSelection + 1;
const OOFmsgT OOFmsg_UnloadRecord			= OOFmsg_ChangeContext + 1;
const OOFmsgT OOFmsg_NewRecord				= OOFmsg_UnloadRecord + 1;
const OOFmsgT OOFmsg_DeleteRecord			= OOFmsg_NewRecord + 1;
const OOFmsgT OOFmsg_SaveRecord				= OOFmsg_DeleteRecord + 1;
const OOFmsgT OOFmsg_RevertRecord			= OOFmsg_SaveRecord + 1;
const OOFmsgT OOFmsg_BroadcasterClosing 	= OOFmsg_RevertRecord + 1;
const OOFmsgT OOFmsg_AttemptLeaveRecord		= OOFmsg_BroadcasterClosing  + 1;
const OOFmsgT OOFmsg_AttemptSaveRecord		= OOFmsg_AttemptLeaveRecord  + 1;
const OOFmsgT OOFmsg_FinishCloning			= OOFmsg_AttemptSaveRecord + 1;
// selection changes that broadcast, eg: to update MN relationship links
const OOFmsgT OOFmsg_AppendSelection		= OOFmsg_FinishCloning + 1;
const OOFmsgT OOFmsg_RemoveSelection		= OOFmsg_AppendSelection + 1;
const OOFmsgT OOFmsg_ClearSelection			= OOFmsg_RemoveSelection + 1;

// allow related tables and other dependants to react to suspension
const OOFmsgT OOFmsg_SuspendSorting		= OOFmsg_ClearSelection + 1;
const OOFmsgT OOFmsg_ResumeSorting		= OOFmsg_SuspendSorting + 1;

//--- end of internal database broadcasts


// starting points for our packages, and user packages
// broadcast through the OS message in Windows, so must avoid OS region
const OOFmsgT OOFmsg_OOFGUIMessageBase		= 1000;
const OOFmsgT OOFmsg_OOFGraphMessageBase	= 2000;
const OOFmsgT OOFmsg_OOFRepMessageBase		= 3000;

const OOFmsgT OOFmsg_UserMessageBase		= 9000;

const OOFmsgT OOFmsg_GetDisplayTableHandler		= OOFmsg_OOFGUIMessageBase + 1;


class oofReceiver;
class OOF_ExpandableLongArray;

/**
	Generic broadcaster for messaging.
	\see oofReceiver
	Has ability to iterate over listeners to get responses, eg: when seeking validation of database state.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofBroadcaster  {
public:
	oofBroadcaster();
	oofBroadcaster(const oofBroadcaster&);
	~oofBroadcaster();

// broadcasting	
	void broadcast(OOFmsgT msg, unsigned long senderDefined=0, const oofReceiver* skipping=0);
	oofReceiver* askAllStoppingAtFirst(bool stopOn, OOFmsgT msg, unsigned long senderDefined=0, const oofReceiver* skipping=0);
	bool askAllReturningAny(bool returnAny, OOFmsgT msg, unsigned long senderDefined=0, const oofReceiver* skipping=0);
	bool isBroadcasting() const;
	
// receiver maint
	void addReceiver(oofReceiver*);
	void removeReceiver(const oofReceiver*);
	
protected:
// data storage
	OOF_ExpandableLongArray* mReceivers;	///< owned
	unsigned long mNestedBroadcastingDepth; ///< aid to debugging and removeReceiver
	bool mLostReceiversWhileBroadcasting;  ///< simple flag that removeReceiver was called during nested broadcast
};


/**
	Generic receiver for messaging.
	\see oofBroadcaster
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofReceiver  {
public:
	oofReceiver() {};
	// use default copy ctor
	virtual ~oofReceiver() {};
	virtual void subscribeTo(oofBroadcaster*);
	virtual void unsubscribe(oofBroadcaster*);
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined)=0;
};


/**
	Special limited oofReceiver which will only ever listen to one oofBroadcaster at a time.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofSingleListener : public oofReceiver {
public:
	oofSingleListener(oofBroadcaster* listensTo=0);
	oofSingleListener(const oofSingleListener&);
	virtual ~oofSingleListener();

	virtual void subscribeTo(oofBroadcaster*);
	virtual void unsubscribe(oofBroadcaster* from=0);
	
	void suspendListening();
	void resumeListening();
	
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
protected:
// data storage
	oofBroadcaster* mListensTo;
	oofBroadcaster* mSuspendedListensTo;
	unsigned short mSuspendCount;
};


// -------------------------------------------------------
//                o o f B r o a d c a s t e r 
// -------------------------------------------------------

inline bool 
oofBroadcaster::isBroadcasting() const
{
	return (mNestedBroadcastingDepth>0);
}

#endif
