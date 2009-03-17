#include <LPeriodical.h>

class LControl;

/**
	Trigger a click on the button in given timeout.
	\see CControlEnablingEditField as user of this timeout.
*/
class CControlTimeout : public LPeriodical {
public:
	CControlTimeout(LControl* inButton, unsigned long inWaitTicks);
	virtual ~CControlTimeout() {};
	
	virtual	void	SpendTime(const EventRecord&);
	void Enable();
	void Disable();	

private:
	LControl*	 mButton;
	unsigned long	mWaitTicks, mTriggerAt;
};

