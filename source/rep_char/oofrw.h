#ifndef H_OOFRW
#define H_OOFRW
// COPYRIGHT 1994 A.D. Software, All rights reserved

/**
	\file oofrw.h 
	Old simple character mode report-writer classes.
*/

/**
	\defgroup oofCharRW OOFILE Character-mode simple report writer.
	Simple reports that output just one table formatted as plain
	text with spaces and returns or simple HTML.
	Included in the Open Source OOFILE.
	\ingroup Presentation
*/



// NOTE inline definitions included at end of this header file

#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif


/**
	Set of column widths as characters.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepColWidths : public OOF_ExpandableLongArray {
public:
// construction
	dbRepColWidths (unsigned int defColWidth=20) :
							OOF_ExpandableLongArray(defColWidth) {};
	// use default copy ctor
	dbRepColWidths& operator<<(unsigned int);
};


/**
	Contains starting position of a new page within each dbRepLine.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepPageBreak : public OOF_ExpandableLongArray {
public:
// construction
	dbRepPageBreak (unsigned int breakPos=0xFFFF) :
							OOF_ExpandableLongArray(breakPos) {};
	private:
		dbRepPageBreak(const dbRepPageBreak&) { assert(0); };
		void operator=(const dbRepPageBreak&) { assert(0); };
};




/**
	Describe an over-run onto next page.
	\ingroup oofCharRW
*/
class dbRepOverRun : public OOF_PublicBase{
	public:
		dbRepOverRun() :
			OverRun(0),
			DeleteMe(0) {};
		~dbRepOverRun() {};
	private:
		dbRepOverRun(const dbRepOverRun&) { assert(0); };
		void operator=(const dbRepOverRun&) { assert(0); };

public:		
		char* OverRun;
		char* DeleteMe;
};




/**
	Line of text to draw in simple character report.
	\ingroup oofCharRW
*/
class dbRepLine : public OOF_PublicBase {
	public:
		dbRepLine() :
			mLine(0) {};
		~dbRepLine();
	private:
		dbRepLine(const dbRepLine&) { assert(0); };
		void operator=(const dbRepLine&) { assert(0); };
	
	public:
		
		void prepare();
		void clear();
		void drawNCharsAt(unsigned int hPos, const char* theChars, unsigned int len);
		void fillNCharsAt(unsigned int hPos, char theChar, unsigned int len);
		void drawToStream(unsigned int hPos, unsigned int len, ostream& os);
		
		char *mLine;		// owned									// for drawing onto
		static unsigned int mWidth;		// size of horizontal line - given to us by dbRepPage

};




/**
	Report page dimensions & page title.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepSizer : public OOF_PublicBase {
public:
// construction
	dbRepSizer (const OOF_String& reportTitle, 
							unsigned int pageHeight=70, 
							unsigned int pageWidth=80, 
							unsigned int leftMargin=0, 
							unsigned int rightMargin=0, 
							unsigned int topMargin=1, 
							unsigned int bottomMargin=1, 
							unsigned int colSepWidth=2, 
							unsigned int blockVertSep=1
							) :
							mReportTitle(reportTitle),
							mPageHeight(pageHeight),
							mPageWidth(pageWidth),
							mLeftMargin(leftMargin),
							mRightMargin(rightMargin),
							mTopMargin(topMargin),
							mBottomMargin(bottomMargin),
							mColSepWidth(colSepWidth),
							mBlockVertSep(blockVertSep) {};
	
	dbRepSizer (const char * reportTitle="", 
							unsigned int pageHeight=70, 
							unsigned int pageWidth=80, 
							unsigned int leftMargin=0, 
							unsigned int rightMargin=0, 
							unsigned int topMargin=1, 
							unsigned int bottomMargin=1, 
							unsigned int colSepWidth=2, 
							unsigned int blockVertSep=1
							) :
							mReportTitle(reportTitle),
							mPageHeight(pageHeight),
							mPageWidth(pageWidth),
							mLeftMargin(leftMargin),
							mRightMargin(rightMargin),
							mTopMargin(topMargin),
							mBottomMargin(bottomMargin),
							mColSepWidth(colSepWidth),
							mBlockVertSep(blockVertSep)  {};
	// use default copy ctor

// setters for use in simulating keyword arguments
// eg dbRep( dbRepSizer("Andy's Report").pageHeight(80).DefColWidth(12) );
	
	dbRepSizer& title(const OOF_String&);
	dbRepSizer& title(const char*);
	dbRepSizer& pageHeight(unsigned int);
	dbRepSizer& pageWidth(unsigned int);
	dbRepSizer& leftMargin(unsigned int);
	dbRepSizer& rightMargin(unsigned int); 
	dbRepSizer& topMargin(unsigned int); 
	dbRepSizer& bottomMargin(unsigned int); 
	dbRepSizer& colSepWidth(unsigned int);
	dbRepSizer& blockVertSep(unsigned int);

// presentation	
	
	OOF_String mReportTitle;
	unsigned int mPageHeight, mPageWidth, mLeftMargin, mRightMargin, mTopMargin, mBottomMargin;
	unsigned int mColSepWidth;		// whitespace between columns
	unsigned int mBlockVertSep;		// whitespace between records on columnar and blocks on pagewise

};




/**
	Page, containing lines..
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepPage : public OOF_PublicBase {
	public:
		dbRepPage();
		~dbRepPage();
	private:
		dbRepPage(const dbRepPage&) { assert(0); };
		void operator=(const dbRepPage&) { assert(0); };

	public:		
		void draw(dbRepSizer Sizer, ostream& os);
		void clearLines(unsigned int start, unsigned int end);
		void endPage(ostream& os);
		
		 dbRepLine* mPageMap;				// array of horizontal bands  	// owned
		 unsigned int	mWidth;				// width of valid lines
		 unsigned int mNumLines;			// number of prinatble lines
		 unsigned int mBodyStart;			// position of data area under header
		 									//   to save redrawing the header
		 unsigned int mNumPages;			// number of physical pages
		 unsigned int* mFieldPos;			// starting position of each field	// owned
		 									//   within each dbRepLine
		 dbRepPageBreak mPageBreak;			// starting position of each new page
		 									//   within each dbRepLine
		 unsigned int* mPageStart;			// starting Row on page below Header	// owned

};


/**
	early report writer providing character-based output.
	Only reports a single dbTable - no nested data or graphs.
	Has interesting pageWise format which puts field names
	on left of wrapped field data.
	Still useful for quick dump of a table, or writing simple HTML.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRep : public OOF_PublicBase {
public:
	enum ReportStyles {pageWise, columnar};
// construction
	dbRep(const dbRepSizer& sizer, const dbView& fields, const ReportStyles style):
				mSizer(sizer),
				mFields(fields),
				mReportStyle(style),
				mBuilt(false) {};
	
	dbRep(const dbRepSizer& sizer, const dbRepColWidths colWidths, const dbView& fields, const ReportStyles style):
				mSizer(sizer),
				mFields(fields),
				mReportStyle(style),
				mColWidths(colWidths),
				mBuilt(false) {};
				
	// use default copy ctor
	virtual ~dbRep() {};
	
	void setStyle(const ReportStyles);

// presentation	
	virtual void formatForCharStream() = 0;
	virtual void draw(ostream&) = 0;
	
	void extract(ostream&);
	char *copyStr(const char *theString);
	
// accessors
	const OOF_String&	reportTitle();

protected:	
	dbRepSizer		mSizer;
	dbView 			mFields;
	ReportStyles 	mReportStyle;
	dbRepColWidths	mColWidths;
	bool			mBuilt;
};


/**
	Write simple text report wrapped with spaces and newlines.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepChar : public dbRep{
public:
// construction
	dbRepChar(const dbRepSizer& sizer, const dbView& fields, const ReportStyles style=columnar):
				dbRep(sizer, fields, style),
				mPage() {};
	
	dbRepChar(const dbRepSizer& sizer, const dbRepColWidths colWidths, const dbView& fields, const ReportStyles style=columnar):
				dbRep(sizer, colWidths, fields, style),
				mPage() {};
	
	// use default copy ctor
	virtual ~dbRepChar() {};
	
// presentation	
	virtual void formatForCharStream();		// may be others later
	virtual void draw(ostream&);

	unsigned int drawWrappedChars(unsigned int line,unsigned int hPos,unsigned int width,char **theString);
	void drawColumnar(ostream&);
	void drawPageWise(ostream&);
	void drawHeader(ostream&);
	
private:
	dbRepPage mPage;
};



// -------------------------------------------------------
//                     d b R e p
// -------------------------------------------------------

inline const OOF_String&	
dbRep::reportTitle()
{
	return mSizer.mReportTitle;
}


// -------------------------------------------------------
//               d b R e p S i z e r
// -------------------------------------------------------

inline dbRepSizer& 
dbRepSizer::title(const OOF_String& reportTitle)
{
	mReportTitle = reportTitle;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::title(const char* reportTitle)
{
	mReportTitle = reportTitle;
	return *this;
}


inline dbRepSizer&
dbRepSizer::pageHeight(unsigned int pageHeight)
{
	mPageHeight = pageHeight;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::pageWidth(unsigned int pageWidth)
{
	mPageWidth = pageWidth;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::leftMargin(unsigned int leftMargin)
{
	mLeftMargin = leftMargin;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::rightMargin(unsigned int rightMargin)
{
	mRightMargin = rightMargin;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::topMargin(unsigned int topMargin)
{
	mTopMargin = topMargin;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::bottomMargin(unsigned int bottomMargin)
{
	mBottomMargin = bottomMargin;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::colSepWidth(unsigned int colSepWidth)
{
	mColSepWidth = colSepWidth;
	return *this;
}


inline dbRepSizer& 
dbRepSizer::blockVertSep(unsigned int blockVertSep)
{
	mBlockVertSep = blockVertSep;
	return *this;
}





// -------------------------------------------------------
//              d b R e p C o l W i d t h s 
// -------------------------------------------------------


//	friend 	
/*inline dbRepColWidths& 
operator<<(dbRepColWidths& lhs, unsigned int rhs)
{
	lhs.append(rhs);
	return lhs;
}
*/


inline dbRepColWidths& 
dbRepColWidths::operator<<(unsigned int rhs)
{
	append(rhs);
	return *this;
}


#endif

