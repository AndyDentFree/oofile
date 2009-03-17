#ifndef H_OOFVIEW
#define H_OOFVIEW

// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database - database views

// NOTE inline definitions included at end of this header file

#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifndef H_OOFSIZE
	#include "oofsize.h"
#endif

/**
	Provide an iterable set of fields.
	A dbView can be created without a dbTable in which case it will iterate just once.
	This is very useful for creating a set of fields for an oofRepViewBand.
	\note you can attach standalone fields like oofChar to have a constant displayed
	alongside variable data. You can even then get exotic by having this standalone
	field calculated by a dbCalculator so you effectively have a function attached to 
	the list.
*/
class OOFILE_EXPORT dbView : public OOF_Dictionary, public OOF_mixColSizable {
public:
	dbView();
	dbView(dbTable*, bool cloneSource=true);
	dbView(dbTable&, bool cloneSource=true);
	dbView(dbRelRefBase*, bool cloneSource=false);	// most desirable for cloning to NOT default
	dbView(dbRelRefBase&, bool cloneSource=false);	// or even occur at all on related tables
	dbView(const dbView&, bool keepIndependentSelection=false);
	dbView(const dbView&, dbTable*);  // eg: copy a view to a recursively related table
	const dbView& operator=(const dbView&);
	~dbView();
	
	virtual dbView* clone(bool keepIndependentSelection=false) const;
	
	void cloneTable(bool shareSelection=false);  // tells view to clone its internal table, must fixup fields etc.

	void extract(ostream&);

// shortcut specialised append
// eg: dbView(People) << LastName << Othernames

	dbView& append(const dbField&);
	dbView& append(const dbField*);
	dbView& append(dbField*);  // adopts the field, cloning it if not standalone
	dbView& append(const char*);	// generates a standalone oofChar owned by the view
	dbView& operator<<(const dbField&);	
	dbView& operator<<(const dbField*);	
	dbView& operator<<(dbField*);	
	dbView& operator<<(const char*);	

	
	const dbField& field(unsigned int) const;
	dbField* writeableField(unsigned int) const;  // future may return 0 indicating not writeable
	
	dbTable* table() const;
	dbTable* source() const;  // old synonym for table()
	void name(const oofString&);
	oofString name() const;
	oofString getHeading(unsigned int, bool* tellIfOverride=0) const;
	oofString currentHeading() const;
	unsigned long recordNumber() const; // call to dbTable that copes if no table on view
	
// default find fields
	const dbField& defaultFindField() const;
	bool defaultFindFieldSpecified() const;
	fieldNumT defaultFindFieldNo() const;
	void defaultFindFieldNo(fieldNumT);
	
	
// NOT YET IMPLEMENTED	oofString getHeading(const dbField&) const;
	void setHeading(unsigned int, const char*);
	
	bool willSuppressHeadings() const;
	void suppressHeadings(bool willSuppress=true);

protected:
	virtual oofColSizer* MakeDefaultSizer(unsigned int numCols=0) const;
	void FinishCopy(const dbView& rhs);
	void AdjustFieldsToNewTable();
	void AppendOwnedField(dbField* adoptedField);
	
// data storage
private:
	dbTable*	mTable;
	bool		mCloneSource;
	OOF_ExpandableLongArray	mHeadings;	// set of oofStrings, default is all null
	OOF_ExpandableLongArray	mOwnedFieldsInView;	// set of oofChars or other standalone fields, for constant columns	
	bool	mSuppressHeadings;				//... stored as index number of fields across dbView, to ease cloning
	fieldNumT	mSpecifiedFindFieldNum;
	oofString mOverrideName;  // overrides tableName if we have a table
};
OOFILE_EXPORT ostream& operator<<(ostream& os, dbView& theView);
OOFILE_EXPORT dbView* operator<<(dbView*, dbField&);	






// -------------------------------------------------------
//                   d b V i e w
// -------------------------------------------------------
inline dbTable*
dbView::table() const
{
	return mTable;
}


inline void
dbView::name(const oofString& inName)
{
	mOverrideName = inName;
}


inline dbTable*
dbView::source() const
{
	return mTable;
}


inline dbView& 
dbView::operator<<(const dbField& rhs)
{
	append(rhs);
	return *this;
}


inline dbView& 
dbView::operator<<(dbField* rhs)
{
	append(rhs);
	return *this;
}


inline dbView& 
dbView::operator<<(const dbField* rhs)
{
	append(rhs);
	return *this;
}


inline dbView& 
dbView::operator<<(const char* rhs)
{
	append(rhs);
	return *this;
}


inline bool 
dbView::willSuppressHeadings() const
{
	return mSuppressHeadings;
}


inline void 
dbView::suppressHeadings(bool willSuppress)
{
	mSuppressHeadings = willSuppress;
}


inline bool 
dbView::defaultFindFieldSpecified() const
{
	return mSpecifiedFindFieldNum != USHRT_MAX;
}


inline fieldNumT 
dbView::defaultFindFieldNo() const
{
	return mSpecifiedFindFieldNum;
}


inline void 
dbView::defaultFindFieldNo(fieldNumT inNum)
{
	mSpecifiedFindFieldNum = inNum;
}


// helper function, not method
inline dbView* 
operator<<(dbView* lhs, dbField& rhs)
{
	lhs->append(rhs);
	return lhs;
}


inline const dbField& 
dbView::field(unsigned int fieldNoInView) const
{
	const dbField* theField = (const dbField*) value(fieldNoInView);  // safe downcast
	return *theField;
}


inline dbField*
dbView::writeableField(unsigned int fieldNoInView) const
{
// gives us scope later to have protected fields
	dbField* theField = (dbField*) value(fieldNoInView);  // safe downcast
	return theField;
}


inline unsigned long 
dbView::recordNumber() const
{
	if (mTable)
		return mTable->recordNumber();
	return 0;  // assumes no table means stuck on first row
}
#endif


