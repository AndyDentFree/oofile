#ifndef H_OOFSIZE
#define H_OOFSIZE
// ===========================================================================
//	oofsize.h				©1996 A.D. Software   All rights reserved.
// ===========================================================================
//

#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif

class dbView;

class oofColSizer;

/**
	Base class for something which can have column widths specified.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_mixColSizable {
public:
	OOF_mixColSizable(oofColSizer* adoptedSizer=0);
	OOF_mixColSizable(const OOF_mixColSizable&);
	virtual ~OOF_mixColSizable();
	
	void colSizer(oofColSizer* adoptedSizer);
	oofColSizer* colSizer();
	
	
protected:
	virtual oofColSizer* MakeDefaultSizer(unsigned int numCols=0) const;
	
private:
// data storage
	oofColSizer*	mSizer;		// owned
};



/**
	Specify or calculate a set of column widths & alignments.
	Fixed widths can be specified as points or percentages.
	Columns left unspecified have widths calculated varying on data type.
	It tries hard to create a reasonable looking set of widths, to be used in a report
	or GUI multi-column browser.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofColSizer {
public:
	oofColSizer(unsigned short numCols);
	enum colAlignT { alignLeft, alignCentre, alignRight, alignJust, alignNatural };
	enum {kNotSet=ULONG_MAX};

// use default copy ctor	
	virtual ~oofColSizer() {};
	
	virtual oofColSizer* clone(OOF_mixColSizable* newOwner) const;
	
	virtual void calculateWidths(unsigned long widthToDivide, unsigned short colSepWidth=0);
	unsigned long width(unsigned short colNum);
	
// getters
	unsigned long fixedWidth(unsigned short colNum) const;
	unsigned long fixedPercentage(unsigned short colNum) const;
	oofColSizer::colAlignT alignment(unsigned short colNum) const;

// setters
	void setFixedWidth(unsigned short colNum, unsigned long);
	void setFixedPercentage(unsigned short colNum, unsigned long);
	void setAlignment(unsigned short colNum, colAlignT);

	static oofColSizer::colAlignT naturalAlignmentForField(OOF_fieldTypes);
	
protected:
	virtual void CalculateDefaultWidths(unsigned long widthToDivide, unsigned int numberNotSet);
		
// data storage
	const unsigned short mNumCols;
	OOF_ExpandableLongArray	mColFixedWidths, mColPercentages, mColCalcWidths, mColAlignments;
	bool mHaveCalculatedWidth;
	};



/**
	Calculate a set of column widths from field types in a view.
	Tries to make an esthetic set based on data types (don't wrap dates or numbers)
	and the widths assigned to fixed text fields.
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofViewFieldDefaultsColSizer : public oofColSizer {
public:
	oofViewFieldDefaultsColSizer(const dbView*);
	oofViewFieldDefaultsColSizer(const oofViewFieldDefaultsColSizer&, const dbView*);
	virtual ~oofViewFieldDefaultsColSizer() {};

	virtual oofColSizer* clone(OOF_mixColSizable* newOwner) const;
		
protected:
	virtual void CalculateDefaultWidths(unsigned long widthToDivide, unsigned int numberNotSet);
		
private:
// data storage
	const dbView*	mView;
};


#endif


