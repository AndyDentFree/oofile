// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database - database views

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//                     d b V i e w
// -------------------------------------------------------

dbView::dbView() :
	mTable(0),
	mCloneSource(false),
	mSuppressHeadings(false),
	mSpecifiedFindFieldNum(USHRT_MAX)
{
}


dbView::dbView(dbTable*	tbl, bool cloneSource) :
	mCloneSource(cloneSource),
	mSuppressHeadings(false),
	mSpecifiedFindFieldNum(USHRT_MAX)
{
	if (tbl) { // is now legal to have nil pointer, could be collection of standalone oofChars etc.
	// so allow a nil dbTable* and will iterate ONCE
		if (tbl->isRelatedClone()) {
			cloneSource = false;	// don't allow cloning of these related tables
			mCloneSource = false;
		}
		if (cloneSource)
			mTable = tbl->cloneTableSharingSelection();
		else
			mTable = tbl;
	}
	else
		mCloneSource = false;	// can't clone if have no table
}


dbView::dbView(dbTable&	tbl, bool cloneSource) :
	mCloneSource(cloneSource),
	mSuppressHeadings(false),
	mSpecifiedFindFieldNum(USHRT_MAX)
{
	if (tbl.isRelatedClone()) {
		cloneSource = false;	// don't allow cloning of these related tables
		mCloneSource = false;
	}
	if (cloneSource)
		mTable = tbl.cloneTableSharingSelection();
	else
		mTable = &tbl;
}


dbView::dbView(dbRelRefBase* tblRef, bool cloneSource) :	
	mCloneSource(cloneSource),
	mSuppressHeadings(false),
	mSpecifiedFindFieldNum(USHRT_MAX)
{
	if (tblRef) {
		assert(!cloneSource);  // don't support cloning related tables
		mTable = tblRef->relatedTable();
	}
	else
		mTable = 0;
}


dbView::dbView(dbRelRefBase& tblRef, bool cloneSource) :
	mCloneSource(cloneSource),
	mSuppressHeadings(false),
	mSpecifiedFindFieldNum(USHRT_MAX)
{
	assert(!cloneSource);  // don't support cloning related tables
	mTable = tblRef.relatedTable();
}


dbView::dbView(const dbView& rhs, bool keepIndependentSelection) : 
				OOF_Dictionary(rhs),
				OOF_mixColSizable(rhs),
				mTable(rhs.mTable),
				mCloneSource(rhs.mCloneSource),
				mHeadings(0, rhs.mHeadings.count()),
				mOwnedFieldsInView(rhs.mOwnedFieldsInView),
				mSuppressHeadings(rhs.mSuppressHeadings),
				mSpecifiedFindFieldNum(rhs.mSpecifiedFindFieldNum)
{
	if (keepIndependentSelection) {  // explicit!
		mCloneSource = true;
		mTable = mTable->cloneTable();  // even if it's a related table 
	}
	else {
		if (mCloneSource) {  // rhs had cloned source so we must too!
			assert(mTable && !mTable->isRelatedClone());
			mTable = mTable->cloneTableSharingSelection();
		}
	}
	FinishCopy(rhs);
}


dbView::dbView(const dbView& rhs, dbTable* newTable) : 
				OOF_Dictionary(rhs),
				OOF_mixColSizable(rhs),
				mTable(newTable),
				mCloneSource(false),
				mHeadings(0, rhs.mHeadings.count()),
				mOwnedFieldsInView(rhs.mOwnedFieldsInView),
				mSuppressHeadings(rhs.mSuppressHeadings),
				mSpecifiedFindFieldNum(rhs.mSpecifiedFindFieldNum)
{
	assert(rhs.table()->tableNumber() == newTable->tableNumber()); 
	FinishCopy(rhs);  // still called to do rest of finishing
}


dbView*
dbView::clone(bool keepIndependentSelection) const
{
	return new dbView(*this, keepIndependentSelection);
}


void
dbView::cloneTable(bool shareSelection)
{
	if (mTable) {
		dbTable* newTable;
		if (shareSelection)
			newTable = mTable->cloneTableSharingSelection();
		else
			newTable = mTable->cloneTable();
		if (mCloneSource)
			delete mTable;
		mCloneSource = true;
		mTable = newTable;
		AdjustFieldsToNewTable();
	}
}


void
dbView::AdjustFieldsToNewTable()
{
	const unsigned long numFields = count();
	OOF_DictRep* oldRep = mRep;
	if(oldRep->mReferences>1) {  //  called from a copy
	// so the rep has been used by us plus the copied rhs, at least
		--(oldRep->mReferences);  // decrement because we're not using that rep
		mRep = new OOF_DictRep(numFields, 1 /*expansion size*/);  // just alloc, don't copy
	}  // otherwise just update in place
		
	for (unsigned int fi=0; fi < numFields; fi++) {
		const dbField* theField = (const dbField*) oldRep->value(fi);  // safe downcast
		mRep->item(fi) = theField->equivalentFieldFromTable(mTable);	// correct field to point to clone
	}
}


void
dbView::FinishCopy(const dbView& rhs)
{
	if (mTable!=rhs.mTable) {
		assert(mTable);	// a nil table pointer forces mCloneSource==false
	// correct all the copied field pointers to point to our newly cloned table
		AdjustFieldsToNewTable();
	}
	const unsigned long numHeadings = rhs.mHeadings.count();
	for (unsigned int i=0; i<numHeadings; i++) {
		oofString* aHeading = (oofString*) rhs.mHeadings.value(i);	// safe downcast
		if (aHeading)
			setHeading(i, *aHeading);
	}
}


const dbView& 
dbView::operator=(const dbView& rhs)
{
	if (this == &rhs)
		return *this;
	
	mTable = rhs.mTable;
	return *this;
}


dbView::~dbView()
{
	if (mCloneSource)
		delete mTable;
// delete any headings allocated.
// Note that they may not have been allocated up to or past all the fields in the view		
	for (unsigned int i=0; i<mHeadings.count(); i++) {
		unsigned long maybeHead = mHeadings.value(i);
		if (maybeHead)
			delete (oofString*) maybeHead;
	}
// delete any owned fields
	if (mRep->mReferences<=1) {
		const unsigned int numOwned = mOwnedFieldsInView.count();
		for (unsigned int j=0; j<numOwned; j++) {
			const unsigned long fieldInView = mOwnedFieldsInView.value(j);
			OOF_bitPointer ownedField = value(fieldInView);
			delete ownedField;
		}
	}
}


oofString 
dbView::name() const
{
	if (mOverrideName.isEmpty()) {
		if (mTable)
			return mTable->tableName();
		else
			return oofString();
	}
	else
		return mOverrideName;
}


const dbField& 
dbView::defaultFindField() const
{
// may return a field not in the view, if sort order by a non-visible field
	fieldNumT theFieldNum = 0;  // all other cases default to first field in view
	if (mSpecifiedFindFieldNum != USHRT_MAX) {
		theFieldNum  = mSpecifiedFindFieldNum;  
	} 
	else {
		if (mTable) {
			const dbSorter* tableSorter = mTable->sorter();
			if (tableSorter) {
				const fieldNumT sortFieldNum = tableSorter->sortFieldNo();
				dbField* sortField = mTable->field(sortFieldNum);
				assert(sortField);
				return *sortField;  // RETURN SORT FIELD, NOT NECESSARILY IN VIEW
			}
		}
	}
	const dbField* ret = (const dbField*) value(theFieldNum);  // safe downcast
	assert(ret);
	return *ret;
}


oofString
dbView::getHeading(unsigned int fieldNoInView, bool* tellIfOverride) const
{
	const oofString* maybeHead = (const oofString*)mHeadings.value(fieldNoInView);
	oofString ret;
	if (tellIfOverride)
		*tellIfOverride = (maybeHead!=0);
	if (maybeHead) 
		ret = *maybeHead;	// safe downcast
	else 
		ret = field(fieldNoInView).fieldName();
	ret.replaceChar('_', ' ');
	return ret;
}


oofString
dbView::currentHeading() const
{
// NOTE repeat the entire getHeading to avoid extra return object copies
// that we'd get by calling getHeading from here
// unless the compiler's optimizer is *very* good about optimising temp objects
	const unsigned long maybeHead = mHeadings.value(mInternalIter);
	if (maybeHead)
		return *( (const oofString*)maybeHead);
	else
		return field(mInternalIter).fieldName();
}


void
dbView::setHeading(unsigned int fieldNoInView, const char* inHead)
{
	const unsigned long maybeHead = mHeadings.value(fieldNoInView);
	oofString* theHead;
	if (maybeHead) {
		theHead = (oofString*) maybeHead;  // safe downcast - this class owns the list
		*theHead = inHead;  // change content of oofString object - it's already in the list
	}
	else {
		theHead = new oofString(inHead);
		assert(sizeof(unsigned long)==sizeof(oofString*));  // we make this sort of assumption a lot!
		mHeadings[fieldNoInView] = (unsigned long) theHead;  // adopt the string
	}		
}


/**
	Append field to view, cloning standalone fields or using append(dbField*).
*/
dbView& 
dbView::append(const dbField& rhs)
{
	if (rhs.fieldIsStandalone()) {
		dbField* newField = rhs.clone();	// copy refs as probably local temp variables
		AppendOwnedField(newField);	
	}
	else {
		append((dbField*) &rhs);	// const_cast, safe as we don't delete our fields
	}
	return *this;
}


/**
	Append field to view, using AppendOwnedField or cloning as appropriate.
	Fields are cloned when we have already cloned the main table (mCloneSource==true)
	so we can iterate it without disturbing the original.
	We map fields across to the equivalent field on our main table when we've cloned
	or if it's just a user specification error - they have specified the field from a clone
	of the wrong table. This is an easy mistake to make in application code - coping in here
	makes life a lot easier, and we take advantage of that in dbFieldPickLink::copyFieldToLink.
*/
dbView& 
dbView::append(dbField* rhs)
{
	if (rhs->fieldIsStandalone()) {
		AppendOwnedField(rhs);	// adopt pointers as expect inline 'new dbChar' expressions
	}
	else {
		bool useEquivField;
		if (mCloneSource)  // our table is guaranteed not to be same as theirs
			useEquivField = true;
		else {  // see if rhs is just on a clone of our table
			dbTable* rhsTable = rhs->fieldTable();
			useEquivField = (rhsTable!=mTable && rhsTable->tableNumber()==mTable->tableNumber());
		}
		if (useEquivField)
			OOF_Dictionary::append(rhs->equivalentFieldFromTable(mTable));
		else
			OOF_Dictionary::append(rhs);  // field could be related, could be totally unrelated, don't care
	}
	return *this;
}


/**
	Append field to view, using append(dbField*).
*/
dbView& 
dbView::append(const dbField* rhs)
{
	assert (!rhs->fieldIsStandalone());
	append((dbField*) rhs);	// const_cast, safe as we don't delete our fields
	return *this;
}


dbView& 
dbView::append(const char* rhs)
{
	oofChar* newField = new oofChar(rhs);
	AppendOwnedField(newField);
	return *this;
}



void
dbView::AppendOwnedField(dbField* adoptedField)
{
	const unsigned long ownedFieldIndex = count();
	mOwnedFieldsInView.append(ownedFieldIndex);
	if (adoptedField->fieldName().length()==0) {
		char buf[15];
		sprintf(buf, "field%lu", ownedFieldIndex);
		adoptedField->setName(buf);
	}
	OOF_Dictionary::append(adoptedField);
}


ostream& 
operator<<(ostream& os, dbView& theView)
{
	theView.extract(os);
	return os;
}


void 
dbView::extract(ostream& os)
{
	unsigned long numRecs = 1;	// default is to iterate once with nil pointer
	if (mTable) {
		mTable->start();	// this precedes count() as a bit of a hack from the past, to force table validity
		numRecs = mTable->count();
	}
	const unsigned int numFields = count();
	// print a row of headings
	bool printedAny = false;
	for (unsigned int col=0; col<numFields; col++) {		// print headings
		oofString head = getHeading(col);
		if (head.length())
			printedAny = true;
		os << head << '\t';
	}
	os << endl;	// which may be after empty line of tabs
	if (printedAny) {
		for (unsigned int col=0; col<numFields; col++) {		// print underlines for headings
			const unsigned long headLen = getHeading(col).length();
			os << oofString('-',headLen) << '\t';
		}
		os << endl;
	}
	for (unsigned long row=0; row<numRecs; row++) {	// iterate through records (vertical)
		for (unsigned int col=0; col<numFields; col++) {		// start field iteration (horizontal)
			field(col).extract(os);
			os << '\t';
		}
		os << endl;
		if (mTable && row<numRecs-1)
				mTable->next();
	}
}


oofColSizer*
dbView::MakeDefaultSizer(unsigned int /*numCols*/) const
{
	return new oofViewFieldDefaultsColSizer(this);
}


