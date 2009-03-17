/**
@file doxyPowerPlant.h
Pure documentation header for Doxygen.

Define classes which are the PowerPlant GUI base classes which may show up as targets or parents of ours.

PowerPlant was the GUI included with the Macintosh Metrowerks compiler. It has since been released as <a href="http://sourceforge.net/projects/open-powerplant/">open source</a>.
*/

/**
	\class LAttachment
	PowerPlant mixin used to intercept events on controls.
	Because we can use these, the PowerPlant forms integration doesn't
	need to subclass the way the MFC version does and is much more
	flexible.
*/


/**
	\class LDialogBox
	PowerPlant base class for a dialog box (fairly recent, was LGADialog).
*/


/**
	\class LEditField
	PowerPlant base class for small text entry.
*/


/**
	\class LListener
	PowerPlant mixin class for listeners, like oofBroadcaster.
*/


/**
	\class LOutlineItem
	PowerPlant base class for managing an item in a hierarchical tree.
*/


/**
	\class LPane
	PowerPlant base class for drawable item.
*/


/**
	\class LPeriodical
	PowerPlant base class for time controller eg: for timeouts.
*/



/**
	\class LTableStorage
	PowerPlant base class for abstract storage of data on LTableView.
*/


/**
	\class LView
	PowerPlant base class for scrollable display.
*/


/**
	\class LWindow
	PowerPlant base class for scrollable, resizable window.
*/
