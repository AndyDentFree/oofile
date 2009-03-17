#include "CControlTimeout.h"
#include <LStdControl.h>

CControlTimeout::CControlTimeout(LControl* inButton, unsigned long inWaitTicks) :
	mButton(inButton),
	mWaitTicks(inWaitTicks),
	mTriggerAt(0)	// not until told to!
{
}


void 
CControlTimeout::SpendTime(const EventRecord &inMacEvent)
{
	long ticksNow = ::TickCount();
	if (mTriggerAt && ticksNow >= mTriggerAt) {
		Disable();
		mButton->SimulateHotSpotClick(kControlButtonPart);
	}
}


void 
CControlTimeout::Enable()
{
	mTriggerAt = ::TickCount() + mWaitTicks;
	StartRepeating();
}


void 
CControlTimeout::Disable()
{
	mTriggerAt = 0;
	StopRepeating();
}