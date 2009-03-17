#ifndef H_OOFADORN
#define H_OOFADORN
// ===========================================================================
//	oofadorn.h				©1996 A.D. Software   All rights reserved.
// ===========================================================================
//

#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFXMLDB
	#include "oofxmldb.h"  // need to know oofSerializable
#endif

class oofAdornerRender;
class oofRepEnvironment;
class oofRepMacEnv;
class oofRepWinEnv;
class oofRepTextEnv;
class oofRepRTFEnv;
class oofRepHTMLEnv;
class oofRepCSVEnv;
class oofRepTSVEnv;
class oofRepXMLEnv;
class oofRepDrawable;
class oofRepViewBand;

/**
	Base object to derive adorners from.

	\note  AD disclaimer 990528
	the entire adorner structure was added with far less design input
	than I would have like and written by a junior programmer with
	scant regard for our programming conventions (no I'm NOT going to
	explain the circumstances). Whilst I dislike changing API's and 
	breaking user code, it is likely that methods, enums etc. in here
	will be renamed in future and possibly the entire mechanism change.

	\ingroup oofRW
*/
class OOFREP_EXPORT oofAdorner : public oofSerializable {

public:
	enum AdornerContextCode {drawBefore, drawAfter, drawDuring};
	enum AdornerCellCode {startStop, startContinue, continueStop, continueContinue};

	oofAdorner();
	oofAdorner(const oofAdorner&);
	virtual ~oofAdorner();
	virtual oofAdorner* clone() const=0;
	
	AdornerContextCode drawWhen() { return mWhenCode; };
	virtual void draw( oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, AdornerCellCode overrunCode=startStop );
	
	// multi-method dispatch to factories for rendering report objects
	// ****** one for each environment *****
	// Note: don't use pure virtuals as some subclasses may not implement
	virtual void	makeRender(const oofRepMacEnv&);
	virtual void	makeRender(const oofRepWinEnv&);
	virtual void	makeRender(const oofRepTextEnv&);
	virtual void	makeRender(const oofRepRTFEnv&);
	virtual void	makeRender(const oofRepHTMLEnv&);
	virtual void	makeRender(const oofRepCSVEnv&);
	virtual void	makeRender(const oofRepTSVEnv&);
	virtual void	makeRender(const oofRepXMLEnv&);

protected:
	enum AdornerContextCode mWhenCode;
	oofAdornerRender*	mRender;	// owned
};


/**
	List of adorners attached to report element.
	Simple provide iterable, clonable list.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofAdornerList {
public:
	oofAdornerList();
	oofAdornerList(const oofAdornerList&);
	const oofAdornerList& operator=(const oofAdornerList&);
	void adopt(oofAdornerList&); 
	~oofAdornerList();
	oofAdornerList* clone();
	
	
	void start();
	bool more() const;
	void next();
	unsigned long count() const;
	oofAdorner* operator()();
	oofAdorner* current();
	oofAdorner* value(unsigned long) const;
	void append(oofAdorner* adoptedAdorner);
	oofAdornerList& operator<<(oofAdorner* adoptedAdorner);
	
private:
// data storage
	OOF_ExpandableLongArray	mAdorners;	
};


// ----------------------------------------------------------------

/**
	Mix in adorners with report objects (used mainly in oofrep.h).
	\ingroup oofRW
*/
class OOFREP_EXPORT OOF_mixAdornable {
public:
// uses default copy ctor
	oofAdornerList&	adorners();
	
	bool hasAdorners() const;
	
private:
	oofAdornerList	mAdorners;
};


// ----------------------------------------------------------------
// 

/**
	Abstract base rendering engine for the adorner model.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofAdornerRender {
public:
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const=0;
	virtual void draw( oofRepDrawable *drawable, unsigned long height, unsigned long width ) = 0;
	virtual void drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode=oofAdorner::startStop );
	virtual void finishConstruction() {};
};


// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	Base for adorners specifying borders for grids.
	In future would be used with other resizable bands.
	\see oofRepViewBand
	\ingroup oofRW
*/
class OOFREP_EXPORT oofBorderAdorner : public oofAdorner {
public:
	enum BorderCode {oneThin, twoThin, threeThin, oneThinOneThick, oneThickoneThin, oneThinOneThickOneThin};

	oofBorderAdorner( BorderCode borderType , oofAdorner::AdornerContextCode whenCode = oofAdorner::drawBefore );
	oofBorderAdorner(const oofBorderAdorner& rhs);
// use default copy ctor	
	virtual ~oofBorderAdorner() {};
	virtual oofAdorner* clone() const;
	
	BorderCode borderType() const { return mBorderType; };
	void borderType(BorderCode inType) { mBorderType=inType; };
	
	// Access functions for border edges
	void leftStyle( oofBorderAdorner::BorderCode borderType ) { mLeftStyle = borderType; };
	oofBorderAdorner::BorderCode leftStyle() { return mLeftStyle; };
	
	void rightStyle( oofBorderAdorner::BorderCode borderType ) { mRightStyle = borderType; };
	oofBorderAdorner::BorderCode rightStyle() { return mRightStyle; };
	
	void topStyle( oofBorderAdorner::BorderCode borderType ) { mTopStyle = borderType; };
	oofBorderAdorner::BorderCode topStyle() { return mTopStyle; };
	
	void bottomStyle( oofBorderAdorner::BorderCode borderType ) { mBottomStyle = borderType; };
	oofBorderAdorner::BorderCode bottomStyle() { return mBottomStyle; };
	
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv&);
	virtual void	makeRender(const oofRepWinEnv&);
	virtual void	makeRender(const oofRepTextEnv&);
	virtual void	makeRender(const oofRepRTFEnv&);
	virtual void	makeRender(const oofRepHTMLEnv&);
	virtual void	makeRender(const oofRepCSVEnv&);
	virtual void	makeRender(const oofRepTSVEnv&);
	virtual void	makeRender(const oofRepXMLEnv&);

// serialization methods
	static void registerSerializable();  
	static oofSerializable* makeAdorner (const oofString&);
	virtual oofString serialize() const ;  
	virtual oofString signature() const;

private:
// data storage
	BorderCode mBorderType;
	
	BorderCode mLeftStyle;
	BorderCode mRightStyle;
	BorderCode mTopStyle;
	BorderCode mBottomStyle;
};

/**
	Abstract base for rendering oofBorderAdorner on specific oofRepEnvironment.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofBorderAdorner_Render : public oofAdornerRender {
protected:
	oofBorderAdorner_Render(oofBorderAdorner* b) : mOwner(b) {};

// data storage
	oofBorderAdorner* mOwner;
friend class oofBorderAdorner;	// owner and creator class
};


////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	Base for adorners specifying background shading for grids.
	In future would be used with other resizable bands.
	\see oofRepViewBand
	\ingroup oofRW
*/
class OOFREP_EXPORT oofShadingAdorner : public oofAdorner {
public:
	oofShadingAdorner( unsigned short percentage, oofAdorner::AdornerContextCode whenCode = oofAdorner::drawBefore );
	oofShadingAdorner(const oofShadingAdorner& rhs);
// use default copy ctor	
	virtual ~oofShadingAdorner() {};
	virtual oofAdorner* clone() const;
	
	//unsigned short percentage( void ) const { return mPercentage; };	// 0 - white  , 100 - black
	unsigned short red( void ) const { return mRed; };
	unsigned short green( void ) const { return mGreen; };
	unsigned short blue( void ) const { return mBlue; };
	
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv&);
	virtual void	makeRender(const oofRepWinEnv&);
	virtual void	makeRender(const oofRepTextEnv&);
	virtual void	makeRender(const oofRepRTFEnv&);
	virtual void	makeRender(const oofRepHTMLEnv&);
	virtual void	makeRender(const oofRepCSVEnv&);
	virtual void	makeRender(const oofRepTSVEnv&);
	virtual void	makeRender(const oofRepXMLEnv&); //dg 22/2/99

// serialization methods
	static void registerSerializable();  
	static oofSerializable* makeAdorner (const oofString&);
	virtual oofString serialize() const;  
	virtual oofString signature() const;

protected:
	unsigned short		mRed, mGreen, mBlue;  
	// more junior programmer crap - why didn't he use oofColor?
};
 
/**
	Base for adorners specifying background shading for simple bands.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofShadingAdorner_Render : public oofAdornerRender {
protected:
	oofShadingAdorner_Render(oofShadingAdorner* b) : mOwner(b) {};

// data storage
	oofShadingAdorner* mOwner;
friend class oofShadingAdorner;	// owner and creator class
};


////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	Base for adorners specifying background shading for simple bands.
	\see oofRepBand
	\ingroup oofRW
*/
class OOFREP_EXPORT oofBandShadingAdorner : public oofAdorner {
public:
	oofBandShadingAdorner( unsigned short percentage, oofAdorner::AdornerContextCode whenCode = oofAdorner::drawBefore );
	oofBandShadingAdorner(const oofBandShadingAdorner& rhs);
// use default copy ctor	
	virtual ~oofBandShadingAdorner() {};
	virtual oofAdorner* clone() const;
	
	unsigned short percentage() const { return mPercentage; };
	
// multi-method dispatch to factories for rendering report objects
	virtual void	makeRender(const oofRepMacEnv&);
	virtual void	makeRender(const oofRepWinEnv&);
	virtual void	makeRender(const oofRepTextEnv&);
	virtual void	makeRender(const oofRepRTFEnv&);
	virtual void	makeRender(const oofRepHTMLEnv&);
	virtual void	makeRender(const oofRepCSVEnv&);
	virtual void	makeRender(const oofRepTSVEnv&);
	virtual void	makeRender(const oofRepXMLEnv&);//dg 22/2/99

// serialization methods
	static void registerSerializable();  
	static oofSerializable* makeAdorner (const oofString&);
	virtual oofString serialize() const;  
	virtual oofString signature() const;

private:
	unsigned short		mPercentage;		// 0 - white  , 100 - black
};


/**
	Abstract base for rendering oofBandShadingAdorner on specific oofRepEnvironment.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofBandShadingAdorner_Render : public oofAdornerRender {
protected:
	oofBandShadingAdorner_Render(oofBandShadingAdorner* b) : mOwner(b) {};

// data storage
	oofBandShadingAdorner* mOwner;
friend class oofBandShadingAdorner;	// owner and creator class
};


/**
	Holding space for adorner when reading report.
	\see OOF_XMLrepAdornerParser::endElement 
	Will be reconciled and replaced by actual adorner in OOF_repXMLadornerLinkVisitor::VisitDrawable. 
	\ingroup oofRepRead
*/
class OOFREP_EXPORT OOF_adornerHolder : public OOF_PublicBase {
public:
	OOF_adornerHolder(const oofString& id, oofAdorner*  firstAdorner=0);
	
	virtual oofString name() const;
	oofAdornerList& adorners();

private:
	const oofString		mID;
	oofAdornerList	mAdorners;	
};


// -------------------------------------------------------
//            O O F _ a d o r n e r H o l d e r
// -------------------------------------------------------
inline oofAdornerList&
OOF_adornerHolder::adorners()
{
	return mAdorners;
}



// -------------------------------------------------------
//             O O F _ m i x A d o r n a b l e 
// -------------------------------------------------------
inline oofAdornerList&
OOF_mixAdornable::adorners()
{
	return mAdorners;
}


inline bool
OOF_mixAdornable::hasAdorners() const
{
	return (mAdorners.count()>0);
}


// -------------------------------------------------------
//             o o f A d o r n e r L i s t
// -------------------------------------------------------
inline void 
oofAdornerList::start()
{
	mAdorners.start();
}


inline bool 
oofAdornerList::more() const
{
	return mAdorners.more();
}
	
	
inline void 
oofAdornerList::next()
{
	mAdorners.next();
}


inline unsigned long 
oofAdornerList::count() const
{
	return mAdorners.count();
}


inline oofAdorner* 
oofAdornerList::operator()()
{
	oofAdorner* ret = (oofAdorner*) mAdorners();	// safe downcast
	return ret;
}


inline oofAdorner* 
oofAdornerList::current()
{
	oofAdorner* ret = (oofAdorner*) mAdorners();	// safe downcast
	return ret;
}


inline oofAdorner* 
oofAdornerList::value(unsigned long index) const
{
	oofAdorner* ret = (oofAdorner*) mAdorners.value(index);	// safe downcast
	return ret;
}


inline oofAdornerList& 
oofAdornerList::operator<<(oofAdorner* adoptedAdorner)
{
	append(adoptedAdorner);
	return *this;
}

#endif
