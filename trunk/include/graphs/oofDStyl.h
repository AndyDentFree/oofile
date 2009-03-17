// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFDSTYL
#define	H_OOFDSTYL
//=================================================================================
//	oofDStyl.h
//=================================================================================

#ifndef H_OOF0
	#include "oof0.h"
#endif
#ifndef H_OOF2
	#include "oof2.h" // for OOF_ExpandableLongArray
#endif
#ifndef H_OOFbool
	#include "oofbool.h"
#endif
#ifndef H_OOFCOLOR
	#include "oofcolor.h"
#endif
#ifndef H_OOFRECT
	#include "oofrect.h"
#endif

class oofSymbol;

/**
	Specify drawing styles used in graphs.
	\see oofColorable 
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofDrawStyle {
public:
		//typedef enum { eSquare,eCircle,eCross,eEcs } symbolType;
		oofDrawStyle(const oofColor& fg = oofColor::Black,
					const oofPattern& colPat = oofPattern::gray,
					const oofPattern& monoPat = oofPattern::gray,
					const oofColor& bg = oofColor::White,
					oofSymbol *sym = 0, bool adoptsSymbol=false);
					//oofDrawStyle::symbolType sym = oofDrawStyle::eSquare);
		oofDrawStyle(const oofDrawStyle&);
		oofDrawStyle(const oofDrawStyle*);
		~oofDrawStyle();
		const oofDrawStyle&	operator=(oofDrawStyle&);
		void	setPattern(const oofPattern&);
		
		void	setJustColorPattern(const oofPattern&);
		void	setJustMonoPattern(const oofPattern&);
		const oofColor&		foreColor() const;
		const oofColor&		backColor() const;
		const oofPattern&	colorPattern() const;
		const oofPattern&	monoPattern() const;
		const oofPattern&	currentPattern() const;
		oofSymbol*	symbol() const { return mSymbol; };
		void	useStyleColors() const;
		static void	setStyleToMono();
		static void	setStyleToColor();
		static bool	isMono();
		
private:
	oofColor	mForeColor,
				mBackColor;
	oofPattern	*mColorPattern;	///< owned
	oofPattern	*mMonoPattern;	///< owned
	static bool	sIsMono;		///< static flag indicating all DrawStyles are to be
								///< drawn with the mono pattern, rather than color
	//symbolType	mSymbol;		///< symbol when drawing a point
	oofSymbol	*mSymbol;		///< owned, symbol when drawing a point

#ifdef _Windows
	oofPen		mPen;			///< Windows pen for outline operations
#endif
};



/**
	Array of owned oofDrawStyle's adopting them for correct lifetime management.
	Provides both append() and operator<<() interfaces for choice of program style.
	oofDrawStyle& parameters are copied.
	Can adopt an entire oofDrawStyleArray to allow efficiently passing objects.
	\todo confirm can change append(oofDrawStyle&) to append(const oofDrawStyle&)
	\see oofDrawStyleSetSupplied
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofDrawStyleArray {
public:
	oofDrawStyleArray();
	~oofDrawStyleArray();
	void			append(oofDrawStyle&);
	oofDrawStyleArray&	operator<<(oofDrawStyle*);
	oofDrawStyleArray&	operator<<(oofDrawStyle&);
	const oofDrawStyle&	item(unsigned long index) const;
	void	adopt(oofDrawStyleArray*);
	unsigned long		count() const;
	void				deleteAll();
private:
	OOF_ExpandableLongArray	mDrawStyles;	///< pointers to draw styles - owned
};



/**
	Virtual base for generator class returning sequence of oofDrawStyle.
	Doesn't assume algorithm or any particular means used to generate 
	sequence - subclasses are totally independent just producing linear
	sequence as iterated but don't know in advance how many numbers.
	\warning must be completely deterministic for consistent graphs
	\note assumes results used once and copied for redraw purposes
	\todo introduce "restart" paradigm for redraws
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofDrawStyleSet {
public:
	oofDrawStyleSet() : mNumDrawStylesProduced(0) {};
	virtual ~oofDrawStyleSet() {};
	
	virtual oofDrawStyle	getNextDrawStyle();
	
// data storage
protected:
	unsigned long mNumDrawStylesProduced;
};



/**
	Common set generator generates well-distributed colours.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofDrawStyleSetDefaultIteration : public oofDrawStyleSet{
public:
	oofDrawStyleSetDefaultIteration() {};
	virtual ~oofDrawStyleSetDefaultIteration() {};
	virtual oofDrawStyle	getNextDrawStyle();
	static oofColor CalculateColor(unsigned long index);
};



/**
	Returns sequence of independently defined oofDrawStyle's.
	Takes oofDrawStyleArray to specify the oofDrawStyles.
	This is how to simply customise graph colours and symbols. 
	\see oofGraph::setDrawStyles
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofDrawStyleSetSupplied : public oofDrawStyleSet {
public:
	oofDrawStyleSetSupplied(oofDrawStyleArray*);
	virtual	~oofDrawStyleSetSupplied();
	virtual	oofDrawStyle	getNextDrawStyle();
protected:
	oofDrawStyleArray	mDrawStyles;	// pointers to draw styles - owned
};



/**
	Abstract base to draw a symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofSymbol {
public:
		oofSymbol() {};	// default ctor
		~oofSymbol() {};
	virtual	void	draw(short,short);
	virtual oofRect	BoundingRect()=0;
	virtual oofSymbol*	clone() const=0;
protected:
	virtual void	DrawSelf(short,short)=0;
	unsigned short	getSymbolSizeX() { return 7; }; ///< temporary method for experimenting with symbol sizes
	unsigned short	getSymbolSizeY() { return 7; };
};



/**
	Calculate bounding rect for a simple symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofSimpleSymbol : public oofSymbol {
public:
		oofSimpleSymbol() {};	// default ctor
		~oofSimpleSymbol() {};
	virtual oofRect	BoundingRect();
private:
	virtual void	DrawSelf(short,short)=0;
};



/**
	Draw a "X" symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofEcsSymbol : public oofSimpleSymbol {
public:
		oofEcsSymbol() {};
		~oofEcsSymbol() {};
	virtual oofSymbol*	clone() const;
private:
	virtual void 	DrawSelf(short,short);
};



/**
	Draw a cross symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofCrossSymbol : public oofSimpleSymbol {
public:
		oofCrossSymbol() {};
		~oofCrossSymbol() {};
	virtual oofSymbol*	clone() const;
private:
	virtual void 	DrawSelf(short,short);
};



/**
	Draw a circular symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofCircleSymbol : public oofSimpleSymbol {
public:
		oofCircleSymbol() {};
		~oofCircleSymbol() {};
	virtual oofSymbol*	clone() const;
private:
	virtual void 	DrawSelf(short,short);
};



/**
	Draw a hollow square symbol on a line graph.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofSquareSymbol : public oofSimpleSymbol {
public:
		oofSquareSymbol() {};
		~oofSquareSymbol() {};
	virtual oofSymbol*	clone() const;
private:
	virtual void 	DrawSelf(short,short);
};
#endif



