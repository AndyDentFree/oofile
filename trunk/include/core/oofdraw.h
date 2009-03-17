//////////////////////////////////////////////////////////////////
// oofDraw.h - Definition for oofCurrentDraw
// and text sizer
// COPYRIGHT 1997 A.D. Software, All rights reserved

#ifndef H_OOFDRAW
#define H_OOFDRAW

#ifndef H_OOF0
#include "oof0.h"
#endif

#ifndef H_OOFSTR
	#include "oofstr.h"
#endif

#ifndef H_OOFRECT
	#include "oofrect.h"
#endif

#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif

#ifdef _Macintosh
	#include <QDOffscreen.h>  // define GWorlds
#endif

#ifdef _Windows
/**
	Manage the current Windows drawing context.
	Used by oofRepGraphBand_RTFRender::draw for Windows picture capture.
	\ingroup oofWinRender
*/
class OOFGRAPH_EXPORT oofCurrentDraw
{
public:
	// Constructor
	oofCurrentDraw();
	~oofCurrentDraw();
	
	// Attributes
	static void makeInfoDC();
	static void deleteInfoDC();

	static const HDC getCurrentDC() 
				{ return shdc; };
	static void	setCurrentDC(HDC hdc);
	static const HDC getInfoDC() 
				{ return sInfoDC; };
/*	static void	setInfoDC(HDC hdc)
				{ sInfoDC = hdc; };
*/
	static BOOL  moveToEx(HDC hdc, int X, int Y, LPPOINT lpPoint);
	static void resetCoordinates();
	static void incX(int dx) { sX += dx; }				
	static void incY(int dy) { sY += dy; }		
	static int x() { return sX; }
	static int y() { return sY; }
	
	// utilities
	static void makeDC72dpiLogical(HDC);
	
protected:
	static oofCurrentDraw	sCurrentDraw;
	static HDC	shdc;  // ref to current, maintained by others
	static HDC	sInfoDC;  // owned
	static int sX;
	static int sY;

};
#endif	// _Windows


/**
	Cross-platform sizer which wraps to multiple rows.
	\ingroup oofRenderMac oofRenderWin
*/
class OOFGRAPH_EXPORT oofTextSizer {
public:
					oofTextSizer();
	virtual			~oofTextSizer();

	virtual void	CalculateWrapping(
								const char		*inValue,	// C string
								unsigned long			inItemSize,
								unsigned long			inColWidth,
								bool				 		inTruncWords=true);

	virtual void 	describe(ostream&) const;

// get results
	const unsigned long* 	GetLineStarts() const;
	const unsigned long* 	GetLineLengths() const;
	unsigned short 			GetNumRows() const;

protected:
	void StoreLineStart(unsigned long inLineStart);
	void StoreLineLength(unsigned long inLineStart);
	void SkipWhiteForward(const char		*inValue,	// C string
								unsigned long&			ioWordStart,
								unsigned long&			ioCharsLeft) const;
	unsigned long TruncateTrailingWord(const char		*inValue,	// C string
								unsigned long			inEndWord,
								unsigned long&			outNextWordStart) const;
								
private:
	unsigned short* mMeasureArray;
	unsigned long* mLineStarts;
	unsigned long* mLineLengths;
	unsigned short mMeasureLen, mNumRows, mLineStartsCells;
};


/**
	Array of oofStrings paired with oofTextSizer.

	\note we don't expose the sizer so clients can only call
	our CalculateWrapping thus we can update mMaxNumRows
	otherwise they could bypass our routine and cause inconsistencies

	\ingroup oofRenderMac oofRenderWin
*/
class OOFGRAPH_EXPORT oofSizedTextArray {
private:
	oofSizedTextArray(const oofSizedTextArray&) {};	// not copyable

public:
	oofSizedTextArray(unsigned short numItems=0);
	~oofSizedTextArray();

	void initSizers(unsigned short numItems);

	oofString& text(unsigned short index) const;
	unsigned short maxRows() const;
	unsigned short count() const;

// publish a subset of the oofTextSizer interface,
// not sure what pattern this falls under (not really Facade)
	const unsigned long* 	GetLineStarts(unsigned short index) const;
	const unsigned long* 	GetLineLengths(unsigned short index) const;
	unsigned short 			GetNumRows(unsigned short index) const;
	virtual void		CalculateWrapping(unsigned short index,
								unsigned short			inColWidth,
								bool				 		inTruncWords=true);
	

private:
	oofTextSizer*	mSizers;	// owned
	oofString*	mStrings;		// owned
	unsigned short	mCount, mMaxNumRows;
};


#ifdef _Macintosh
/**
	Reference-counted wrapper for a GWorld.
	\see oofPict
	\ingroup oofInternals oofMacSpecific
*/
class OOFREP_EXPORT OOF_GWorldOwner {
public:
	OOF_GWorldOwner();
	~OOF_GWorldOwner();
	OOF_GWorldOwner(const OOF_GWorldOwner& rhs);	// copy ctor
	GWorldPtr operator->() { return mGWorld; };
	OOF_GWorldOwner& operator=(GWorldPtr adoptedGWorld);
	const GWorldPtr gWorld() { return mGWorld; };
	
private:
	GWorldPtr	mGWorld;	// owned, ref-counted
	unsigned long*	mRefs;	// owned
};
#endif // _Macintosh


/**
	Cross-platform picture representation can load from file or resource and draw.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofPict {
public:
	oofPict(const oofFileRef&);
	oofPict(const oofPict&);
#ifdef _Macintosh
	oofPict(unsigned long resourceID);  // still support load from resource
#endif
	~oofPict();
	oofPict* clone() const;
	
// getters
#ifdef _Macintosh
	GWorldPtr			gWorld();
#endif
	unsigned long height();
	unsigned long width();
	oofString describe() const;	
	
	
#ifdef _Macintosh
	void draw(int x, int y, int right, int bottom, bool onPrinter=false);  // may care about printing later
#elif defined _Windows
	void draw(int x, int y, int right, int bottom, bool onPrinter, HDC drawDC);
#endif
	
private:
	void Load();
	
// data storage
	unsigned long		mHeight;
	unsigned long		mWidth;
	oofFileRef		mFile;
	bool	mFromFile;
	bool mLoaded;  // ignored for now - make life easier by implementing all const methods so no deferred loading
#ifdef _Macintosh
	OOF_GWorldOwner		mGWorld;	// owned
	unsigned long mResID;
#endif

public:	
#ifdef _Macintosh
	static PicHandle  readPict(const oofFileRef&);
#endif
};


/**
	Mixin for classes keeping an oofPict member.
	\ingroup oofInternals
*/
class OOFREP_EXPORT OOF_mixPictOwner {
public:
	OOF_mixPictOwner(oofPict* adoptedPict);
	OOF_mixPictOwner(const OOF_mixPictOwner&);
	virtual ~OOF_mixPictOwner();

// getters
	oofPict* picture() const;
	unsigned long height();
	unsigned long width();

private:
	oofPict*	mPict;	// owned
};


#ifdef _Macintosh

/**
	Provide text measurement and maintain a current font size.
	
	\todo decide if needed only on Windows or still want Mac version
	\ingroup oofRenderMac
*/
class OOFREP_EXPORT oofTextDrawer
{
public:
	static bool  setFontSize(int textSize = -1);
	static int   calcCharHeight(int textSize = -1);
	static bool  invariant() ;

private:
	static int sLastCharHeight, sLastCharHeightRequest, sCurrentFontSize;
};

#elif defined _Windows

/**
	Provide text measurement and maintain a current font size.
	Was previously used to render text as well, superseded by oofWinTextDE::recordElement
	
	\see oofRepWinEnv::getTextExtent
	\todo rewrite with member vars and using Singleton pattern for future use in multiple threaded contexts
	\ingroup oofRenderWin
*/
class OOFREP_EXPORT oofTextDrawer
{
public:
	static BOOL  textOutCP(HDC, LPCTSTR, int cbString, int textSize);
	static BOOL  textOutCP(HDC, const oofString&, int textSize);
	static BOOL  textOut(HDC, int x, int y, LPCTSTR, int cbString);
	static BOOL  textOut(HDC, int x, int y, const oofString&);
	static SIZE  getTextExtent(const oofString&);
	static HFONT selectFont(HDC, HFONT);
	static int   getCurrentFontDesc(LOGFONT*);
	static HFONT getCurrentFont();
	static HFONT createFontIndirect(const LOGFONT*);
	static BOOL  setFontSize(int textSize = -1);
	static int   calcCharHeight(int textSize = -1);
	static void	 cleanupFont();
	static bool invariant( );
	
private:
	static HFONT sFont;
	static int sLastCharHeight;
	static int sLastCharHeightRequest;
	static int sCurrentFontSize;
// NOT YET IMPLEMENTED - rewrite with member vars and using Singleton pattern
// for future use in multiple threaded contexts
};


#endif  // _Windows


// -------------------------------------------------------
//                 o o f T e x t S i z e r
// -------------------------------------------------------
inline const unsigned long* 
oofTextSizer::GetLineStarts() const 
{ 
	return mLineStarts; 
}


inline const unsigned long* 
oofTextSizer::GetLineLengths() const 
{ return mLineLengths; }


inline unsigned short 
oofTextSizer::GetNumRows() const 
{ 
	return mNumRows; 
}


// -------------------------------------------------------
//             o o f S i z e d T e x t A r r a y
// -------------------------------------------------------
inline unsigned short 
oofSizedTextArray::count() const
{
	return mCount;
}


inline unsigned short 
oofSizedTextArray::maxRows() const
{
	return mMaxNumRows;
}


// -------------------------------------------------------
//             o o f P i c t
// -------------------------------------------------------
#ifdef _Macintosh
inline GWorldPtr
oofPict::gWorld()
{
	// reach into the OOF_GWorldOwner member
	// and grab pointer to the graphic
	return mGWorld.gWorld();
}
#endif // _Macintosh


// -------------------------------------------------------
//            O O F _  m i x P i c t O w n e r
// -------------------------------------------------------
inline unsigned long
OOF_mixPictOwner::height()
{
	return mPict->height();
}

inline unsigned long
OOF_mixPictOwner::width()
{
	return mPict->width();
}


inline	oofPict* 
OOF_mixPictOwner::picture() const
{
	return mPict;
}

#endif	// H_OOFDRAW
