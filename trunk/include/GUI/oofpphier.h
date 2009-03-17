// COPYRIGHT 1996 A.D. Software, All rights reserved

// Public interface of PowerPlant Integration layer of OOFILE database

#ifndef H_OOFPowerPlantHier
#define H_OOFPowerPlantHier

// PowerPlant Integration layer of OOFILE database - Hierarchical tables

// NOTE inline definitions included at end of this header file
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif
#ifndef H_OOFPowerPlant
	#include "oofpp.h"
#endif

// include PowerPlant definitions
#include <PP_Types.h>
#include <LOutlineItem.h>

class dbHierBrowseHelper;
class dbHierLevel;


/**
	Map LOutlineItem to specific record.
	mixin class to use in combo with LOutlineItem to provide OOFILE backing
	for the storage of outline items.
	\ingroup oofGUI oofMacSpecific
*/
class OOF_mixOutlineItem {

public:
	OOF_mixOutlineItem(oidT, dbHierBrowseHelper*, unsigned short indentLevel, bool isLeaf);

// our data access methods
	bool selectItemRecord();
	oidT oid() const;
	oidT findOIDforData(const void* inDataPtr,	UInt32 inDataSize) const;
	bool isLeaf() const;
	unsigned short indentLevel() const;

// lookups, virtual for future overriding if different views at same level
	virtual dbView* view() const;
	virtual dbHierLevel* level() const;
		
protected:
	void GetDrawContentsFromView(const STableCell&	 inCell, SOutlineDrawContents& ioDrawContents);

// data storage
protected:
	dbHierBrowseHelper*	mHelper;
private:
	oidT	mOID;	// our link to an object in some view
	unsigned short mIndentLevel;	// needs to know as helper returns different dbViews
	unsigned long mCacheIndex;
	bool mIsLeaf;
};


/**
	Link a dbView to one specific level in a tree control.
	\ingroup oofGUI oofMacSpecific
*/
class dbViewOutlineItem : public LOutlineItem, public OOF_mixOutlineItem {
public:
	dbViewOutlineItem(oidT, dbHierBrowseHelper*, unsigned short indentLevel, bool isLeaf);
	virtual ~dbViewOutlineItem();

	virtual Boolean		CanExpand() const;
	
	
protected:
	virtual void			ClickCell(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown);
	
	virtual void			ExpandSelf();
	virtual void			GetDrawContentsSelf(
									const STableCell&		inCell,
									SOutlineDrawContents&	ioDrawContents);
};



/**
	Manage a tree control linked to one or more dbView's.
	Lots of abstraction of how data is mapped to each item in tree and how independent it may be
	at each level.
	\ingroup oofGUI oofMacSpecific
*/
class dbHierBrowseHelper : public dbAbstractBrowseHelper {
public:
	dbHierBrowseHelper(LOutlineTable* inTable=0);
	virtual ~dbHierBrowseHelper();

// abstract overrides
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual dbView* view() const; // topmost view
	virtual unsigned long	highlightedRecNo() const;
	virtual dbSelection	highlightedSelection() const;
	virtual void highlightNothing() const;
	virtual void 	highlightRecNo(unsigned long, bool bSelect=TRUE) const;
	virtual void updateBrowserSelection() const;
	virtual void refreshBrowser();

	void browseViewWithTable(LOutlineTable* inTable);
	void resetTableAfterViewRestructure();
	void deleteAllLevels();	
	
	void appendLevel(dbHierLevel* adoptedLevel);
	dbHierBrowseHelper& operator<<(dbHierLevel*);

	dbHierLevel* level(unsigned short) const;
	dbHierLevel* lowestLevel() const;
	OOF_mixOutlineItem* highlightedItem() const;
	bool selectItemRecord();
	bool itemIsAtBottom(const OOF_mixOutlineItem* theItem) const;
	unsigned short deepestIndent() const;
	
	unsigned long countNestedItemsFromLevel(oidT, unsigned short) const;
	dbView* viewAtLevel(unsigned short) const;
	virtual void insertItemsBelow(LOutlineItem*, OOF_mixOutlineItem*);

	oidT	highlightedOID() const;

// content cache
	unsigned long nextCacheIndex();
	void initCacheRow(unsigned long cacheIndex, fieldNumT numFields);
	void setCachedContents(unsigned long cacheIndex, fieldNumT fieldNum, char* adoptedStr);
	const char* getCachedContents(unsigned long cacheIndex, fieldNumT fieldNum);
	void deleteContentsCache();
		
protected:
	void IncludeAllBelowItem(unsigned short inIndentLevel, oidT, dbSelection&) const;
	oidT FirstOIDBelowItem(unsigned short inIndentLevel, oidT inOID) const;
	
// data storage
	OOF_ExpandableLongArray	mHierLevels;
	LOutlineTable* mOutlineTable;
	unsigned long mNextCacheIndex;
	OOF_ExpandableLongArray mContentsCache;
};


/**
	Mixin for factories that make LOutlineItem's.
	Is named as a mixin because user subclasses may mixin but we commonly use as sole base class.
	\ingroup oofGUI oofMacSpecific
*/
class OOF_mixOutlineItemFactory {
public:
	virtual LOutlineItem* makeItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel);  // overrideable factory
};



/**
	Create LOutlineItem's for each item displayed in tree control.
	Commonly used with totally unrelated dbViews at each level.
	takes field number used to decide if leaf
	used by default if pass bool field into dbHierLevel subclasses (making dbViewOutlineItem's)
	or subclass to override makeItem and make your own item types 
	\todo document in better detail!
	\ingroup oofGUI oofMacSpecific
*/
class dbOutlineItemFactory : public OOF_mixOutlineItemFactory {
public:
	dbOutlineItemFactory(fieldNumT leafFieldNum);
	dbOutlineItemFactory(const dbBool& leafField);
	virtual LOutlineItem* makeItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel);  // overrideable factory

protected:
	bool IsLeaf(dbHierBrowseHelper*, unsigned short indentLevel);
	
private:
	fieldNumT mLeafFieldNum;
	// cached state
	unsigned short mLastIndentLevel;
	dbBool* mLastLeafField;
};


/**
	Abstract for factory making recursive levels.
	\note typically used to automatically generate a tree displaying a database containing a filesystem map.
	\ingroup oofGUI oofMacSpecific
*/
class dbHierLevel : public OOF_mixOutlineItemFactory {
	dbHierLevel(const dbHierLevel&) { assert(0); };  // never copy
public:
	dbHierLevel(dbView* adoptedView, OOF_mixOutlineItemFactory* adoptedFactory=0);
	dbHierLevel(dbView* adoptedView, const dbBool& isLeafField);
	virtual ~dbHierLevel();	// will subclass to change traversal mechanism

	virtual unsigned long countNestedItemsFor(oidT)=0;
	virtual dbTable* selectNestedItemsFor(oidT)=0;
	
/// factory for recursive levels, may return 0 if unable to recurse
	virtual dbHierLevel* makeNestedLevel() const;
	
	dbView* view() const;

/// methods for making outline items, or changing the factory used to make them	
	LOutlineItem* makeNestedOutlineItem(oidT inOID, dbHierBrowseHelper* inHelper, unsigned short indentLevel);  // overrideable factory
	void setAlternateItemFactory(OOF_mixOutlineItemFactory* adoptedFactory);
	
protected:
	// data storage
	dbView*	mView;	// owned
	OOF_mixOutlineItemFactory* mItemFactory;  // owned but default is this
};


/**
	Factory for levels based on relationships where each level a dbView has a relationship to the next.
	\ingroup oofGUI oofMacSpecific
*/
class dbHierRelatedLevel : public dbHierLevel {
public:
	dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase*	inTraversalField=0, OOF_mixOutlineItemFactory* adoptedFactory=0);
	dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory=0);
	dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField, const dbBool& isLeafField);
	// use default copy ctor, dtor

	virtual unsigned long countNestedItemsFor(oidT);
	virtual dbTable* selectNestedItemsFor(oidT);
	
protected:
	// data storage
	dbRelRefBase*	mTraversalField;	// in the adoptedView's table
};


/**
	Factory for recursive levels based on relationship where dbTable points back to itself.
	\note the dbRelRefBase* version of ctor doesn't allow default 0 arg, as we must have it in a 
	recursive situation to know how to get to the next level
	\ingroup oofGUI oofMacSpecific
*/
class dbHierRecursiveRelatedLevel : public dbHierRelatedLevel {
public:
	dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase*	inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory=0);
	dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField, OOF_mixOutlineItemFactory* adoptedFactory=0);
	dbHierRecursiveRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField, const dbBool& isLeafField);
	// use default copy ctor, dtor

	virtual dbHierLevel* makeNestedLevel() const;
};


// -------------------------------------------------------
//            d b H i e r B r o w s e H e l p e r
// -------------------------------------------------------
inline dbHierBrowseHelper& 
dbHierBrowseHelper::operator<<(dbHierLevel* adoptedLevel)
{
	appendLevel(adoptedLevel);
	return *this;
}


inline dbHierLevel* 
dbHierBrowseHelper::level(unsigned short inLevel) const
{
	dbHierLevel*  ret = (dbHierLevel*) mHierLevels.value(inLevel);	// safe downcast
	return ret;
}


inline dbHierLevel* 
dbHierBrowseHelper::lowestLevel() const
{
	dbHierLevel*  ret = (dbHierLevel*) mHierLevels.value(mHierLevels.count()-1);	// safe downcast
	return ret;
}


inline bool
dbHierBrowseHelper::itemIsAtBottom(const OOF_mixOutlineItem* theItem) const
{
// this assumes a model where we have a fixed number of levels
// some dbHierLevel types may allow infinite nesting, so the following is
// only true in terms of the levels recursed to so far
	bool ret = theItem->indentLevel()==mHierLevels.count()-1;
	return ret;
}


inline unsigned short
dbHierBrowseHelper::deepestIndent() const
{
	return mHierLevels.count()-1;
}


// -------------------------------------------------------
//            d b H i e r L e v e l
// -------------------------------------------------------
inline dbView* 
dbHierLevel::view() const
{
	return mView;
}


// -------------------------------------------------------
//        O O F _ m i x O u t l i n e I t e m
// -------------------------------------------------------
inline bool
OOF_mixOutlineItem::isLeaf() const
{
	return mIsLeaf;
}


inline oidT
OOF_mixOutlineItem::oid() const
{
	return mOID;
}


inline unsigned short
OOF_mixOutlineItem::indentLevel() const
{
	return mIndentLevel;
}
#endif

