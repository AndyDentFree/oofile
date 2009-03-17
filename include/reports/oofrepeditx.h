// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditx.h
// report-writer layer of OOFILE database - private editing classes



#ifndef H_OOFREPEDITX
#define H_OOFREPEDITX

#ifndef H_OOFREPEDIT
	#include "oofrepedit.h"
#endif

#ifdef _Macintosh
	#ifndef REPMACGUIPREVPP
		#include "oofRepPreviewPP.h"
	#endif
#elif defined(_Windows)
	#ifndef H_OOFPRNWN
		#include "oofPrnWn.h"  // yeah I know these headers are badly named, low prio!
	#endif
#endif

class LPane;

/**
	Default redirector producing page map when render report to preview.
	Decorator pattern - forwards most calls directly to owned renderer
	but records draw position to build a page map so we can reconcile clicks
	on the preview window with the report objects.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepRedirectingRender : public oofRepRenderDecorator {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const=0;
	oofRepRedirectingRender(oofRepRender* adoptedRender=0);
	virtual ~oofRepRedirectingRender();
	
	virtual void draw()=0;
	oofRepEditor* editor() const;
	oofRepEditingEnv* editingEnv() const;
	bool editRedirectionActive() const;
	
protected:
	oofRepEditLink* mEditLink;  
};


/**
	Redirector for layout objects, recording block positions. 
	\note only one object active at a time (ignoring multi-threading possibility) so
	sets sCurrentLayoutRedirectingRender.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepLayoutRedirectingRender : public oofRepRedirectingRender {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	oofRepLayoutRedirectingRender(oofRepLayoutBand_Render* adoptedRender=0);
	virtual ~oofRepLayoutRedirectingRender();
	
	virtual void draw();
	
	void addAreaForCurrentBlock() const;
	
	static oofRepLayoutRedirectingRender* currentLayoutRedirectingRender();
	
private:
	oofRepLayoutBand_Render* LayoutRender() const;
	void SetupEditLink();
	
	// data storage
	oofRepLayoutEditLink* mEditLink;  // dec refs - partially owned
	static oofRepLayoutRedirectingRender* sCurrentLayoutRedirectingRender;	
};


/**
	Terminate previous areas so we know we have a dead area.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofNonEditableRedirectingRender : public oofRepRedirectingRender {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	oofNonEditableRedirectingRender(oofRepRender* adoptedRender=0);
	virtual void draw();
};


/**
	Record a block within an active oofRepLayoutRedirectingRender.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepBlockRedirectingRender : public oofRepRedirectingRender {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	oofRepBlockRedirectingRender(oofRepRender* adoptedRender=0);
	virtual void draw();
};



/**
	Redirector for view band so maps whilst we page break.
	Generates a grid of areas corresponding to the fields and records in the view
	and may extend over pages.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepViewRedirectingRender : public oofRepRedirectingRender, public OOF_mixViewExtraRender {
public:
	virtual oofRepRender* clone(const oofRepDrawable* newOwner) const;
	oofRepViewRedirectingRender(oofRepViewBand_Render* adoptedRender=0);
	virtual ~oofRepViewRedirectingRender();


	virtual OOF_mixViewExtraRender* finishClone(oofRepRender* newClone) const;

// overrides
	virtual void drawColumnHeaders();  ///< maybe do nothing but if drawn each page!
	virtual void startTableBody(bool headersHaveChanged);  ///< wrap entire process
	virtual void endTableBody();
	virtual void draw();  ///< per row
	
protected:
	void AddRowInteractionAreas();
	oofRepViewBand* ViewBand() const;
	oofRepEditLink* mColHeaderLink;   ///< owned, shared with OOF_InteractionPageAreas
};


/**
	Map blocks within layout band back to an index for editing.
	\ingroup oofrepEdit
*/
class OOF_RepBlockCellMap {
public:
	short mCellIndex;
	oofRepBlock* mBlock;
};


/**
	Typed array of OOF_RepBlockCellMap with searches.
	\ingroup oofrepEdit
*/
class OOF_RepBlockCellRow : public oofArray {
public:
	OOF_RepBlockCellRow();
	virtual ~OOF_RepBlockCellRow();

	OOF_RepBlockCellMap& map(unsigned long) const;
	void addBlockCells(oofRepBlock*);
	bool findBlock(oofRepBlock* inBlock, short& outCol) const;
		
protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	void AddMap(short inIndex, oofRepBlock*);
	
	OOF_RepBlockCellMap* mMaps;  ///< typed reference to mBits
};


/**
	Typed array of OOF_RepBlockCellRow with searches.
	\ingroup oofrepEdit
*/
class OOF_RepBlockCellRows : public oofArray {
public:
	OOF_RepBlockCellRows();
	virtual ~OOF_RepBlockCellRows();

	OOF_RepBlockCellRow& row(unsigned long) const;
	OOF_RepBlockCellRow* addRow();
	
protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	
	OOF_RepBlockCellRow* mRows;  // typed reference to mBits
};


/**
	oofRepEditLink to oofRepLayoutBand - multiple blocks in a single row.
	\ingroup oofrepEdit
*/
class OOFREP_EXPORT oofRepLayoutEditLink : public oofRepEditLink {
public:
	oofRepLayoutEditLink(oofRepLayoutBand*);
	void initMapping();

// cell content access methods
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=-1, short col=-1);
	virtual void 	setCellText(const oofString&, long row=-1, short col=-1);

// reflective methods to query link
	virtual bool allowsCellTextStyles() const;
	
	void getFirstCoordsForBlock(oofRepBlock*, short& col, short& row) const;

// movement testing
	virtual bool getRightOfCurrent(short& outCol, long& outRow) const;
	virtual bool getUpFromCurrent(short& outCol, long& outRow) const;
	virtual bool getDownFromCurrent(short& outCol, long& outRow) const;

protected:
	oofRepLayoutBand* Drawable() const;
	oofRepBlock* MapCoordsToBlock(long row, short col, short& outCellInBlock) const;	
	
	OOF_RepBlockCellRows mRows;
};


/**
	Edit link to oofRepViewBand that may extend over multiple pages.
	\ingroup oofRepEdit
*/
class OOFREP_EXPORT oofRepViewEditLink : public oofRepEditLink {
private:
	oofRepViewEditLink(const oofRepViewEditLink& rhs) : oofRepEditLink(rhs.mDrawable) {};  // can't copy
	
public:
	oofRepViewEditLink(oofRepViewBand*, bool editingSubSelection);
	virtual ~oofRepViewEditLink();

// cell content access methods
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=-1, short col=-1);
	virtual void 	setCellText(const oofString&, long row=-1, short col=-1);

// init and cleanup code when edit dialog launches and if it changes links
	virtual void enterLink(dbGUIAdapter*);

// reflective methods to query link
	virtual bool allowsCellTextStyles() const;

	virtual bool hasCustomViewDrawers() const;
	virtual oofRepCustomViewDrawer* customViewDrawer(long col) const;  // returns NULL if has none

// movement testing
	virtual bool getRightOfCurrent(short& outCol, long& outRow) const;
	virtual bool getUpFromCurrent(short& outCol, long& outRow) const;
	virtual bool getDownFromCurrent(short& outCol, long& outRow) const;

protected:
	oofRepViewBand* Drawable() const;
	dbView* View() const;
	
// data storage
	dbSelection* mSelection;  // owned but maybe NULL as may not need to reset selection
	unsigned long mRecordNumber;
};


/**
	Edit link to column headers of oofRepViewBand grid.
	These edited contents may appear mirrored on subsequent pages - you can't edit the headers
	on one page independently of the next page(s) the grid flows over.
	\ingroup oofRepEdit
*/
class OOFREP_EXPORT oofRepColHeaderEditLink : public oofRepEditLink {
public:
	oofRepColHeaderEditLink(oofRepViewBand*);

// cell content access methods
	virtual oofString getCellText(bool* outIsEditable=0, oofString* outLocationName=0, long row=-1, short col=-1);
	virtual void 	setCellText(const oofString&, long row=-1, short col=-1);

// movement testing
	virtual bool getRightOfCurrent(short& outCol, long& outRow) const;

protected:
	dbView* View() const;
	oofRepViewBand* Drawable() const;
};



/**
	Usual interactor triggering editing for a single click on the preview window.
	\ingroup oofRepEdit
*/
class oofRepInteractorSimpleClickStrategy : public oofRepInteractorStrategy {
public:
	virtual void singleClick(short inX, short inY, OOF_InteractionArea*, oofRepEditingEnv*);
};


/**
	Describe single rectangular area within preview.
	\see oofRepEditingEnv  OOF_InteractionPageAreas
	\ingroup oofRepEdit
*/
class OOF_InteractionArea {
public:
	OOF_InteractionArea();
	
	bool matchesPageLocation(short inX, short inY) const;
	void singleClick(short inX, short inY, oofRepEditingEnv*);
	
	short X, Y, mWidth, mHeight;
	long mRow;
	short mCol;
	oofRepEditLink* mEditLink;  // just a ref - may be many maps to same editor!
	oofRepInteractorStrategy*  mInteractionStrategy;
};


/**
	Areas in just one page.
	Assumes linearly ascending page coordinates
	used in searching findAreaBelowAndRightOf.
	\ingroup oofRepEdit
*/
class OOF_InteractionPageAreas : public oofArray {
public:
	OOF_InteractionPageAreas();
	virtual ~OOF_InteractionPageAreas();
	
// data adders
	void pageAdoptsEditLink(oofRepEditLink*);  // add edit link entry to page, which implies adoption of link if 1st time added to a page
	void addInteractionArea(short inX, short inY,  short inCol, long inRow, oofRepEditLink*, oofRepInteractorStrategy* inInteractor=0, short inWidth=0, short inHeight=0);
	void addInteractionArea(const OOF_InteractionArea&);
	void changeAreasToNewLink(long matchingRow, oofRepEditLink* matchingLink, oofRepEditLink* newLink);
	void startNonEditableArea(unsigned long newY);
	

// queries
	OOF_InteractionArea* findAreaBelowAndRightOf(short inX, short inY);
	OOF_InteractionArea* lastAreaAdded() const;  // may return NULL if none on page yet
	
protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	
	OOF_InteractionArea* mAreas;  // typed reference to mBits
	OOF_ExpandableLongArray mOwnedLinks;  // secondary array of owned objects
};


/**
	Contain all the page maps for a preview window.
	\ingroup oofRepEdit
*/
class OOF_InteractionPages : public oofArray {
public:
	OOF_InteractionPages();
	virtual ~OOF_InteractionPages();
	
	void addPage();  
	OOF_InteractionPageAreas& latestPage();    ///< return ref so less likely to hang onto expiring pointer
	OOF_InteractionPageAreas& page(unsigned long index);
	
protected:
	virtual void* AllocBits(unsigned long) const;
	virtual void DeleteBits(bool becauseExpanding);
	
	OOF_InteractionPageAreas* mPages;  ///< typed reference to mBits
};


// capable base class but user can replace by subclassing oofRepEditor

#ifdef _Macintosh
	/**
		Subclass platform preview environment to intercept for editing hooks.
		\ingroup oofRepEdit oofMacSpecific
	*/
	class OOFREP_EXPORT oofRepEditingEnv : public oofRepMacGUIPreviewPP {
	public:
		typedef  oofRepMacGUIPreviewPP parentGUIenvT;
#elif defined(_Windows)
	/**
		Subclass platform preview environment to intercept for editing hooks.
		\ingroup oofRepEdit oofWinSpecific
	*/
	class OOFREP_EXPORT oofRepEditingEnv : public oofRepWinGUIPreviewMFC {
	public:
		typedef oofRepWinGUIPreviewMFC parentGUIenvT;
#endif
	oofRepEditingEnv(oofRepEditor*);
	virtual ~oofRepEditingEnv();
	
	virtual void startEnvironment();
	virtual void endEnvironment();

	parentGUIenvT* parentGUIenv() const;
	oofRepEditor* editor() const;
	
	virtual void makeRenderDispatch(oofRepDrawable*) const;
	virtual void makeRenderDispatch(oofAdorner*) const;

	virtual void drawPageBreak();
	virtual void splitBandOverPage();
	virtual void resumeBandOverPage();

//	setup interaction
	virtual void addInteractionArea(short inX, short inY, short inCol, long inRow, oofRepEditLink* inEditor, oofRepInteractorStrategy* inInteractor=0, short width=0, short height=0);
	virtual void addInteractionArea(const OOF_InteractionArea&);
	void pageAdoptsEditLink(oofRepEditLink*);  // add edit link entry to page, which implies adoption of link if 1st time added to a page
	void changeAreasToNewLink(long matchingRow, oofRepEditLink* matchingLink, oofRepEditLink* newLink);
	void startNonEditableArea();
	
#ifdef _Macintosh
	virtual void startMonitoringWindowInteractions(LPane*);
#elif defined(_Windows)
#endif


	// interaction in progress
	virtual void singleClick(short x, short y);
			
protected:
	oofRepEditor* mEditor;
	OOF_InteractionArea* mLastAreaOnPrevPage;
	OOF_InteractionPages* mInteractionPages;  // owned
};


// -------------------------------------------------------
//     o o f R e p E d i t i n g E n v i r o n m e n t
// -------------------------------------------------------
inline oofRepEditingEnv::parentGUIenvT* 
oofRepEditingEnv::parentGUIenv()  const
{ 
	return (parentGUIenvT*) this; 
}


inline oofRepEditor* 
oofRepEditingEnv::editor() const
{ 
	return mEditor;
}


inline void
oofRepEditingEnv::pageAdoptsEditLink(oofRepEditLink* inLink)
{
	mInteractionPages->latestPage().pageAdoptsEditLink(inLink);
}


// -------------------------------------------------------
//    o o f R e p R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
inline oofRepEditingEnv*
oofRepRedirectingRender::editingEnv() const 
{
	const oofRep* curRep = oofRep::currentReport();
	oofRepEditingEnv* ret = (oofRepEditingEnv*)curRep->environment();  // slightly unsafe downcast, lacks local protection
	return ret;
}


inline oofRepEditor*
oofRepRedirectingRender::editor() const 
{
	oofRepEditor* ret = editingEnv()->editor();
	return ret;
}


// -------------------------------------------------------
//       o o f R e p L a y o u t E d i t L i n k
// -------------------------------------------------------

inline oofRepLayoutBand* 
oofRepLayoutEditLink::Drawable() const
{
	return (oofRepLayoutBand*) mDrawable;  // safe downcast
}


// -------------------------------------------------------
//          o o f R e p V i e w E d i t L i n k
// -------------------------------------------------------

inline oofRepViewBand* 
oofRepViewEditLink::Drawable() const
{
	return (oofRepViewBand*) mDrawable;  // safe downcast
}


inline dbView* 
oofRepViewEditLink::View() const
{
	return Drawable()->view(0);
}



// -------------------------------------------------------
//          o o f R e p C o l H e a d e r E d i t L i n k
// -------------------------------------------------------

inline dbView* 
oofRepColHeaderEditLink::View() const
{
	return ((oofRepViewBand*) mDrawable)->view(0);
}


inline oofRepViewBand* 
oofRepColHeaderEditLink::Drawable() const
{
	return (oofRepViewBand*) mDrawable;  // safe downcast
}


// -------------------------------------------------------
//   o o f R e p V i e w R e d i r e c t i n g R e n d e r
// -------------------------------------------------------

inline oofRepViewBand* 
oofRepViewRedirectingRender::ViewBand() const
{
	assert(mWrappedRender);
	return ((oofRepViewBand_Render*)mWrappedRender)->viewBand();  // safe downcast
}


// -------------------------------------------------------
//   o o f R e p L a y o u t R e d i r e c t i n g R e n d e r
// -------------------------------------------------------
inline oofRepLayoutRedirectingRender* 
oofRepLayoutRedirectingRender::currentLayoutRedirectingRender()
{
	return sCurrentLayoutRedirectingRender;
}


inline oofRepLayoutBand_Render* 
oofRepLayoutRedirectingRender::LayoutRender() const
{
	assert(mWrappedRender);
	return (oofRepLayoutBand_Render*)mWrappedRender;  // safe downcast
}



// -------------------------------------------------------
//       O O F _ I n t e r a c t i o n A r e a 
// -------------------------------------------------------
inline 
OOF_InteractionArea::OOF_InteractionArea() :
		X(USHRT_MAX),
		Y(USHRT_MAX),
		mEditLink(0),
		mWidth(0),
		mHeight(0),
		mInteractionStrategy(0),
		mRow(-1),
		mCol(-1)
{}

	
/**
	Forward a click to our strategy.
*/
inline void 
OOF_InteractionArea::singleClick(short inX, short inY, oofRepEditingEnv* inEnv) 
{
	mInteractionStrategy->singleClick(inX, inY, this, inEnv);  // Flyweight pattern, Strategy requires us to supply context
}


/**
	Simple rectangular inclusion test.
*/
inline bool 
OOF_InteractionArea::matchesPageLocation(short inX, short inY) const
{ 
	return (
		(X<=inX && Y<=inY) &&
		(mHeight==0 || mHeight>=(inY-Y)) &&
		(mWidth==0  || mWidth >=(inX-X))
	);
}



// -------------------------------------------------------
//       O O F _ I n t e r a c t i o n P a g e s
// -------------------------------------------------------
inline void
OOF_InteractionPages::addPage()
{
	const unsigned long allocSlot = AllocSlot();
}
	
	
inline OOF_InteractionPageAreas&
OOF_InteractionPages::latestPage()
{
	const unsigned long numItems = count();
	assert(numItems>0);  // always have a page active!
	return mPages[numItems-1];
}
	
	
inline OOF_InteractionPageAreas&
OOF_InteractionPages::page(unsigned long index)
{
	assert(index<count());
	return mPages[index];
}
	
	
// -------------------------------------------------------
//       O O F _ R e p B l o c k C e l l R o w s
// -------------------------------------------------------
inline OOF_RepBlockCellRow&
OOF_RepBlockCellRows::row(unsigned long index) const
{
	assert(index<count());
	return mRows[index];
}
	
	
inline OOF_RepBlockCellRow*  // return pointer so can reassign
OOF_RepBlockCellRows::addRow()
{
	unsigned long index = AllocSlot();
	return &mRows[index];
}
	
	

	
// -------------------------------------------------------
//       O O F _ R e p B l o c k C e l l R o w
// -------------------------------------------------------
inline OOF_RepBlockCellMap&
OOF_RepBlockCellRow::map(unsigned long index) const
{
	assert(index<count());
	return mMaps[index];
}
#endif  // H_OOFREPEDITX


