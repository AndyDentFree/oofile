// COPYRIGHT 1996 A.D. Software, All rights reserved

// Public interface of MFC Integration layer of OOFILE database

#ifndef H_OOFMFCHR
#define H_OOFMFCHR

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

/**
\file oofmfchr.h
MFC Integration layer of OOFILE database - Hierarchical tables
*/

// NOTE inline definitions included at end of this header file
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFMSG
	#include "oofmsg.h"
#endif
#ifndef H_OOFMFC
	#include "oofmfc.h"
#endif


class dbHierBrowseHelper;
class dbHierLevel;

/**
	Link a dbView to one specific level in a tree control.
	\ingroup oofWinSpecific oofGUI
*/
class dbViewOutlineItem
{
public:
	dbViewOutlineItem(HTREEITEM treeItem, unsigned int indentLevel, oidT oid);
	~dbViewOutlineItem();

public:
	oidT			oid() const;
	unsigned int	indentLevel() const;
	HTREEITEM		handle() const;
	bool			isNullItem() const;

	static dbViewOutlineItem nullItem();

protected:
	HTREEITEM		mTreeItem;
	unsigned int	mIndentLevel;
	oidT			mOID;
};

// maintains the illusion of a single column outline table
// but relies on a helper to supply a (possibly) different
// dbView for each level, and to navigate from a record in 
// one level to a selection in the next

// for now don't put the abstract layers in place of the other broser
// but assume we're talking directly to a COOFTreeCtrl

/**
	Manage a tree control linked to one or more dbView's.
	Lots of abstraction of how data is mapped to each item in tree and how independent it may be
	at each level.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT dbHierBrowseHelper : public dbAbstractBrowseHelper
{
public:
	dbHierBrowseHelper(dbHierLevel* level);
	virtual ~dbHierBrowseHelper();

// abstract overrides
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual unsigned long highlightedRecNo() const;
	virtual void highlightRecNo(unsigned long inRecNo, bool bSelect=TRUE) const;
	virtual void highlightNothing() const;
	virtual dbSelection	highlightedSelection() const;
	virtual void updateBrowserSelection() const;
	virtual dbView* view() const;
	virtual void refreshBrowser();
	
	void browseViewWithTreeCtrl(COOFTreeCtrl* inTree);
	void browseViewWithTreeCtrl(COOFTreeCtrl& inTree)  { browseViewWithTreeCtrl(&inTree); }
	void resetTableAfterViewRestructure();
	void deleteAllLevels();
	void appendLevel(dbHierLevel* adoptedLevel);
	dbHierBrowseHelper& operator<<(dbHierLevel*);

	dbHierLevel* level(unsigned short) const;
	dbHierLevel* lowestLevel() const;
	dbViewOutlineItem highlightedItem() const;
	bool selectItemRecord();
// ES
//	bool itemIsAtBottom(const dbViewOutlineItem* theItem) const;
	bool itemIsAtBottom(const dbViewOutlineItem& item) const;

	unsigned long countNestedItemsFromLevel(oidT, unsigned short) const;
	dbView* viewAtLevel(unsigned short) const;
// ES
//	void insertItemsBelow(dbViewOutlineItem* inSuper=0);
	void insertItems();
	HTREEITEM loadRow(dbView* view, HTREEITEM itemParent = TVI_ROOT);

	oidT	highlightedOID() const;
	unsigned short highlightedIndentLevel() const;

// content cache
	unsigned long nextCacheIndex();
	void initCacheRow(unsigned long cacheIndex, fieldNumT numFields);
	void setCachedContents(unsigned long cacheIndex, fieldNumT fieldNum, char* adoptedStr);
	const char* getCachedContents(unsigned long cacheIndex, fieldNumT fieldNum);
	void deleteContentsCache();

	unsigned long getNumLevels() const;

	virtual void deleteSelection(const dbSelection& inSel);


protected:
	void IncludeAllBelowItem(unsigned short inIndentLevel, oidT, dbSelection&) const;
	oidT FirstOIDBelowItem(unsigned short inIndentLevel, oidT inOID) const;
	void insertSubItems(int indentLevel, oidT oid, HTREEITEM treeItem);

// data storage
	OOF_ExpandableLongArray	mHierLevels;
	COOFTreeCtrl* mDisplay;
	unsigned long mNextCacheIndex;
	OOF_ExpandableLongArray mContentsCache;
	bool mLoadedData;
};


/**
	Abstract for factory making recursive levels.
	\note typically used to automatically generate a tree displaying a database containing a filesystem map.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT dbHierLevel {
public:
	dbHierLevel(dbView* adoptedView);
	virtual ~dbHierLevel();	// will subclass to change traversal mechanism

	virtual unsigned long countNestedItemsFor(oidT)=0;
	virtual dbTable* selectNestedItemsFor(oidT)=0;
	
	dbView* view() const;
	
protected:
	// data storage
	dbView*	mView;	// owned
};


/**
	Factory for levels based on relationships where each level a dbView has a relationship to the next.
	\ingroup oofWinSpecific oofGUI
*/
class OOFGUI_EXPORT dbHierRelatedLevel : public dbHierLevel {
public:
	dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase*	inTraversalField=0);
	dbHierRelatedLevel(dbView* adoptedView, dbRelRefBase&	inTraversalField);
	virtual ~dbHierRelatedLevel();	

	virtual unsigned long countNestedItemsFor(oidT);
	virtual dbTable* selectNestedItemsFor(oidT);
	
protected:
	// data storage
	dbRelRefBase*	mTraversalField;	// in the adoptedView's table
};

// -------------------------------------------------------
//            d b V i e w O u t l i n e I t e m
// -------------------------------------------------------
inline oidT
dbViewOutlineItem::oid() const
{
	return mOID;
}

inline unsigned int
dbViewOutlineItem::indentLevel() const
{
	return mIndentLevel;
}

inline HTREEITEM
dbViewOutlineItem::handle() const
{
	return mTreeItem;
}

inline bool
dbViewOutlineItem::isNullItem() const
{
	return mTreeItem == NULL;
}

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

inline unsigned long
dbHierBrowseHelper::getNumLevels() const
{
	return mHierLevels.count();
}

inline unsigned short
dbHierBrowseHelper::highlightedIndentLevel() const
{
	return mDisplay->highlightedIndentLevel();
}

// ES
/*
inline bool
dbHierBrowseHelper::itemIsAtBottom(const dbViewOutlineItem* theItem) const
{
	bool ret = theItem->mIndentLevel==mHierLevels.count()-1;
	return ret;
}
*/

inline bool
dbHierBrowseHelper::itemIsAtBottom(const dbViewOutlineItem& item) const
{
	bool ret = item.indentLevel() == mHierLevels.count()-1;
	return ret;
}

// -------------------------------------------------------
//            d b H i e r L e v e l
// -------------------------------------------------------
inline dbView* 
dbHierLevel::view() const
{
	return mView;
}


#endif // !_Macintosh for doxygen
#endif

