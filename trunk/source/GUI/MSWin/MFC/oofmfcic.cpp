// COPYRIGHT 1998 A.D. Software, All rights reserved

// MFC Integration layer of OOFILE database - item chache implementation
#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include "oofmfcic.h"
#include "oofmfcct.h"

dbItemCache::dbItemCache(fieldNumT numFields) :
	OOF_ExpandableLongArray(),
	mFirstCachedIndex(LONG_MAX),
	mNumFields(numFields)
{
}

dbItemCache::~dbItemCache()
{
	deleteItemCache();
}

void
dbItemCache::initCacheRow(unsigned long cacheIndex, bool clearExistingContents)
{
	char** contentsRow = (char**) value(cacheIndex);	// may be existing
	if (contentsRow == NULL) {
		contentsRow = new char*[mNumFields];
		for (unsigned short i=0; i<mNumFields; i++)
			contentsRow[i] = NULL;
		item(cacheIndex) = (unsigned long) contentsRow;
	}
	else {
		if (clearExistingContents) {
			for (unsigned short i=0; i<mNumFields; i++) {
				delete contentsRow[i];
				contentsRow[i] = NULL;
			}
		}
	}
}

char*
dbItemCache::getCachedContents(unsigned long cacheIndex, fieldNumT fieldNum) const
{
	char** contentsRow = (char**) value(cacheIndex-mFirstCachedIndex);
	char* ret = NULL;

	if (contentsRow != NULL)
		ret = contentsRow[fieldNum];

	// can't return NULL as it causes an exception to be thrown in list control if a key
	// is pressed
	return ret != NULL ? ret : "";
}

char*
dbItemCache::getString(unsigned long rowInd, fieldNumT fieldNum, const dbView* view,
					   unsigned long firstVisibleIndex, unsigned long numItemsVisible)
{
	synchronizeCachedContents(firstVisibleIndex, numItemsVisible, view);
	return getCachedContents(rowInd, fieldNum);
}

char*
dbItemCache::getString(unsigned long rowInd, fieldNumT fieldNum, const COOFListCtrl* listCtrl)
{
	return getString(rowInd, fieldNum, listCtrl->getBrowsingView(), listCtrl->GetTopIndex(),
				listCtrl->GetCountPerPage()+1);
}

char*
dbItemCache::getString(unsigned long rowInd, fieldNumT fieldNum, const COOFListBox* listBox)
{
	return getString(rowInd, fieldNum, listBox->getBrowsingView(), listBox->GetTopIndex(),
		listBox->GetCountPerPage()+1);
}

char*
dbItemCache::getStringFromView(const dbView& view, unsigned long row, fieldNumT fieldIndex) const
{
	// static buffer for item description
	static char buf[BufSize];
	// create a view with cloned table
	// (the state of the original table won't be affected)
	dbView clonedView(view.table()->cloneTableSharingSelection(), false);
	CopyFieldsFromView(&clonedView, view);
	// record to display
	VERIFY(clonedView.source()->gotoRecord(row));
	// field handle - safe downcast
	const dbField* theField = (const dbField*)clonedView.value(fieldIndex);
	// text description
	oofString itemText(theField->copyString());
	ASSERT(BufSize > itemText.length());
	strcpy(buf, itemText);

	// see synchronizeCacheContents() for details
	delete clonedView.table();

	return buf;
}

void
dbItemCache::setCachedContents(unsigned long cacheIndex, fieldNumT fieldNum, char* adoptedStr)
{
	char** contentsRow = (char**) value(cacheIndex);
	ASSERT(contentsRow);
	char* existingContent = contentsRow[fieldNum] ;
	delete[] existingContent;
	contentsRow[fieldNum] = adoptedStr;
}

void
dbItemCache::deleteItemCache()
{
	unsigned long numRows = count();
	for (unsigned long i=0; i<numRows; i++) {
		char** contentsRow = (char**) value(i);
		if (contentsRow != NULL) {
			for (unsigned short j=0; j<mNumFields; j++) {
				char* aString = contentsRow[j];
				delete[] aString;
			}
		}
		delete[] contentsRow;
		item(i) = NULL;
	}
}

void
dbItemCache::clearItemCache()
{
	unsigned long numRows = count();
	for (unsigned long i=0; i<numRows; i++)
		clearCacheRow(i);
}

void
dbItemCache::synchronizeCachedContents(unsigned long firstIndex, unsigned int numItemsVisible, const dbView* view)
{
	if (mFirstCachedIndex == firstIndex)
		return;		// trivial case

	mFirstCachedIndex = firstIndex;

	// create a view with cloned source table
	// should be dbView clonedView(view->table(), true), but if the view->table()
	// is a related clone, the dbView ctor won't clone it - it just a workaround
	// (the same workaround is in the getStringFromView() method
	dbView clonedView(view->table()->cloneTableSharingSelection(), false);
	dbTable* clonedTable = clonedView.table();
	const unsigned long numItems = clonedTable->count();

	CopyFieldsFromView(&clonedView, *view);

	for (unsigned long i=0; i<numItems-mFirstCachedIndex && i<numItemsVisible; i++) {
		// go to the record which is being cached
		VERIFY(clonedTable->gotoRecord(mFirstCachedIndex+i));
		// read all fields
		initCacheRow(i);
		const unsigned long numFields = clonedView.count();
		for (fieldNumT fieldInd=0; fieldInd<numFields; fieldInd++) {
			// field handle - safe downcast
			const dbField* theField = (const dbField*)clonedView.value(fieldInd);
			// make a copy of the field string
			oofString fieldStr(theField->copyString());
			char* itemStr = new char[fieldStr.length()+1];	// count the ending '\0' as well
			strcpy(itemStr, fieldStr);
			// put the string into the cache
			setCachedContents(i, fieldInd, itemStr);
		}
	}

	// this is just a workaround - the view's table must be explicitly deleted,
	// as it is a clone of the original table, but clonedView was created with
	// "cloneSource" set to false, so it thinks, that its table isn't cloned,
	// and it wouldn't release it
	delete clonedView.table();
}

void
dbItemCache::CopyFieldsFromView(dbView* view, const dbView& sourceView) const
{
	const unsigned long numFields = sourceView.count();
	for (unsigned int fi=0; fi < numFields; fi++) {
		const dbField* theField = (const dbField*)sourceView.value(fi);  // safe downcast
		*view << theField->equivalentFieldFromTable(view->table());
	}	
}

void
dbItemCache::setNumFields(fieldNumT numFields)
{
	deleteItemCache();
	mNumFields = numFields;
}

void
dbItemCache::setNumFields(const dbView& sourceView)
{
	setNumFields(sourceView.count());
}
