// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepedit.h
// report-writer layer of OOFILE database - editing classes
// see also oofrepeditx.h

#ifndef H_OOFREPEDIT
#define H_OOFREPEDIT

#ifndef H_OOFREPEDITOR
	#include "oofrepeditor.h"
#endif
#ifndef H_OOFGUI
	#include "oofgui.h"  // for dbFieldLink, dbHelper
#endif


/**
	\defgroup oofRepEdit Report Editing support
	Report preview windows allow editing with a user-overrideable
	editor, usually a dialog that can step through cells of the report.
	Included in the Open Source OOFILE.
	\ingroup oofRW
*/


/**
	Abstract parent defining how report preview reacts to interaction.
	Strategy Pattern.
	First version just provides singleClick behaviour.
	\ingroup oofRepEdit
*/
class oofRepInteractorStrategy { 
public:
	virtual ~oofRepInteractorStrategy() {};
	virtual void singleClick(short inX, short inY, OOF_InteractionArea*, oofRepEditingEnv*)=0;
};


/**
	oofRepEditLink similarly to dbFieldLink, links a display to an OOFILE item.
	However much richer interaction is supported as the linked item (an oofRepDrawable) may have
	a) multiple text components, and
	b) multiple styles
	
	View bands may have custom draw objects for a specific column which may not be instantiated
	in the program that's read in an XML-saved report. A pointer so an interested edit interface can query the type.
	
	Future ideas - would be nice to abstract content even further and also add abstract property iteration
	for invisible properties such as column widths, sizes, view column headings enabling.
	
	A link to a drawable with multiple dbViews may present those views as adjacent and so
	edited as contiguous cells either horizontally or vertically.
	
	Remember all rows and cols are zero-based as normal.
	
	outIsEditable is used to provide additional user feedback (eg: a locked symbol) that the cell is
	locked. However the adapter is used to disable and enable the cell without depending on user code.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepEditLink : public dbFieldLink, public OOF_mixRefCount {
public:
	oofRepEditLink(oofRepDrawable*);
	virtual ~oofRepEditLink();
	
/// \name cell content access methods
//@{
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=-1, short col=-1)=0;
	virtual void 	setCellText(const oofString&, long row=-1, short col=-1)=0;
//@}

/// \name init and cleanup code when edit dialog launches and if it changes links

	virtual void enterLink(dbGUIAdapter*);
	virtual bool leaveLink();

/// \name style access
//@{
	virtual oofRepTextStyle* textStyle() const;  // should never be NULL - cascades from outer container
	virtual oofRepTextStyle* localTextStyle() const;  // may be NULL
	virtual bool allowsCellTextStyles() const;
//@}

/// \name reflective methods to query link
//@{
	virtual bool hasCustomViewDrawers() const;
	virtual oofRepCustomViewDrawer* customViewDrawer(long col) const;  // returns NULL if has none
//@}

	void getCurrentCoords(short& outCol, long& outRow) const;
	void setCurrentCoords(short inCol, long inRow);

/// \name movement testing
//@{
	virtual bool atFirst() const;
	virtual bool getLastCoords(short& outCol, long& outRow) const;
	virtual bool getLeftOfCurrent(short& outCol, long& outRow) const;
	virtual bool getRightOfCurrent(short& outCol, long& outRow) const;
	virtual bool getUpFromCurrent(short& outCol, long& outRow) const;
	virtual bool getDownFromCurrent(short& outCol, long& outRow) const;
//@}

// GUI interfaces
	virtual void tellGUIcareAboutLocking() const;
	
protected:
	oofRepDrawable* mDrawable;
	long mCurrentRow, mLastRow;
	short mCurrentCol, mLastCol;  // use short as it's big enough and will cause compiler warnings if confuse row/col
};


/**
	Support typical actions of report editing dialogs.
	The idea of the mixin base is to define a common set of controls
	and their behaviour so an application can subclass with just a few
	overrides, supporting with whatever dialog class and layout they 
	fancy. This is necessary because application code may have dialogs based
	on LGADialog or other base classes.
	
	Any buttons not initialised will not be implemented so the entire 
	movement approach can be ignored if desired.
	
	Most of the behaviour is cross platform in OOF_mixRepAbstractCellEditor
	thanks to the cross-platform GUI interface.
	\ingroup oofRepEdit
*/
class OOF_mixRepAbstractCellEditor : public dbHelper {
protected:  // only used by subclasses
	OOF_mixRepAbstractCellEditor();
	virtual ~OOF_mixRepAbstractCellEditor();
	virtual void FinishCreationByLinkingDataModel(oofRepEditingEnv*, oofRepEditLink*);  // final after ctor and FinishCreateSelf
	
	enum movementT {eGoFirst, eGoLast, eGoLeft, eGoRight, eGoUp, eGoDown};

// optional behavioural overrides
	virtual void UpdateMovementButtons();  ///< override to control disabling
	virtual void UnableToMoveWarning(movementT, bool movingByCell);  ///< default beep

// optional actions to override
	virtual void GoToAnotherCell(movementT);
	virtual void GoToAnotherItem(movementT);


// optional behavioural overrides but Adapter should take care of things
	virtual void LoadCellEditField();
	virtual bool AttemptSaveCellEditField();
	

// Template Method - calls several of the above
	void LoadAndUpdate();
	void EnterLink(oofRepEditLink*);
	
// dbHelper overrides
	virtual void maybeDirty(dbFieldLink*);
	
	
// data storage
	bool mAnyContentDirtied;
	oofRepEditingEnv* mEnv;
	oofRepEditLink* mCurrentLink;
	dbGUIAdapter* mAdapter;  // owned
};


// -------------------------------------------------------
//     o o f R e p E d i t L i n k
// -------------------------------------------------------
inline void 
oofRepEditLink::getCurrentCoords(short& outCol, long& outRow) const
{
	outCol = mCurrentCol;
	outRow = mCurrentRow;
}


inline void 
oofRepEditLink::setCurrentCoords(short inCol, long inRow)
{
	mCurrentCol = inCol;
	mCurrentRow = inRow;
}


#endif  // H_OOFREPEDIT


