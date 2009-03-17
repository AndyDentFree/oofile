#ifndef OOFMFCIC_H
#define OOFMFCIC_H

#ifndef H_OOFVIEW
	#include "oofview.h"
#endif

// forward declarations
class COOFListCtrl;
class COOFListBox;

/**
	Cache rows in a list and transfer from dbView.
	\ingroup oofWinSpecific oofGUI
*/
class dbItemCache : public OOF_ExpandableLongArray
{
// construction & destruction
public:
	dbItemCache(fieldNumT numFields = 0);
	virtual ~dbItemCache();

public:
	void initCacheRow(unsigned long cacheIndex, bool clearExistingContents=false);
	void clearCacheRow(unsigned long cacheIndex);
	char* getString(unsigned long rowInd, fieldNumT fieldNum, const dbView* view,
		unsigned long firstVisibleIndex, unsigned long numItemsVisible);
	char* getString(unsigned long rowInd, fieldNumT fieldNum, const COOFListCtrl* listCtrl);
	char* getString(unsigned long rowInd, fieldNumT fieldNum, const COOFListBox* listBox);
	char* getStringFromView(const dbView& view, unsigned long row, fieldNumT fieldIndex) const;
	char* getCachedContents(unsigned long cacheIndex, fieldNumT fieldNum) const;
	void setCachedContents(unsigned long cacheIndex, fieldNumT fieldNum, char* adoptedStr);
	void clearItemCache();
	void deleteItemCache();
	void synchronizeCachedContents(unsigned long firstIndex, unsigned int numItemsVisible,
		const dbView* view);
	void setDirtyFlag();
	bool isDirty() const;
	void setNumFields(fieldNumT numFields);
	void setNumFields(const dbView* sourceView);
	void setNumFields(const dbView& sourceView);

protected:
	void CopyFieldsFromView(dbView* view, const dbView& sourceView) const;

protected:
	enum {BufSize = 100};

// attributes
protected:
	unsigned long mFirstCachedIndex;
	fieldNumT mNumFields;
};

// inline definitions
inline bool
dbItemCache::isDirty() const
{
	return mFirstCachedIndex == ULONG_MAX;
}

inline void
dbItemCache::clearCacheRow(unsigned long cacheIndex)
{
	initCacheRow(cacheIndex, true);
}

inline void
dbItemCache::setNumFields(const dbView* sourceView)
{
	setNumFields(*sourceView);
}

inline void
dbItemCache::setDirtyFlag()
{
	mFirstCachedIndex = ULONG_MAX;
}

#endif
