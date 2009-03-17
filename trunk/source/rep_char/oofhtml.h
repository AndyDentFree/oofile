#ifndef H_OOFHTML
#define H_OOFHTML
// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database

// NOTE inline definitions included at end of this header file

#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOFRW
	#include "oofrw.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif

/**
	Line of text to render in HTML.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepLineHTML : public OOF_PublicBase {
	public:
		dbRepLineHTML() :
			mLine(0) {};
		~dbRepLineHTML();
	private:
		dbRepLineHTML(const dbRepLine&) { assert(0); };
		void operator=(const dbRepLine&) { assert(0); };
	
	public:
		
		void prepare();
		void clear();
		void drawNCharsAt(unsigned int hPos, const char* theChars, unsigned int len);
		void draw(unsigned int indent, ostream& os);
		
		char *mLine;		      // owned		// for drawing onto
		unsigned int mWidth;	  // size of horizontal line - given to us by dbRepPage
		unsigned int* mFieldPos;  // starting position of each field	// owned
};



/**
	Simple HTML character report, one table only.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepHTML : public dbRep {
public:
// construction
	dbRepHTML(const dbRepSizer& sizer, const dbView& fields, const ReportStyles style=columnar) :
				dbRep(sizer, fields, style),
				mLine() {};
	
	dbRepHTML(const dbRepSizer& sizer, const dbRepColWidths colWidths, const dbView& fields, const ReportStyles style=columnar) :
				dbRep(sizer, colWidths, fields, style),
				mLine() {};
	// use default copy ctor

// presentation	
	void formatForCharStream();		// may be others later
	void draw(ostream&);
	
	bool drawWrappedCharsSingle(unsigned int hPos,unsigned int width,char **theString);
	void drawColumnar(ostream&);
	void drawPageWise(ostream&);
	void drawHeader(ostream&);

private:
	dbRepLineHTML mLine;
};


/**
	Variant on simple one-table HTML report using table for layout.
	\ingroup oofCharRW
*/
class OOFILE_EXPORT dbRepHTMLTable : public dbRep {
public:
// construction
	dbRepHTMLTable(const dbRepSizer& sizer, const dbView& fields, const ReportStyles style=columnar) :
				dbRep(sizer, fields, style),
				mCellWidths(0) {};
	
	dbRepHTMLTable(const dbRepSizer& sizer, const dbRepColWidths colWidths, const dbView& fields, const ReportStyles style=columnar) :
				dbRep(sizer, colWidths, fields, style),
				mCellWidths(0) {};
	// use default copy ctor

// presentation	
	void formatForCharStream();		// may be others later
	void draw(ostream&);
	
	void drawColumnar(ostream&);
	void drawPageWise(ostream&);
	void drawHeader(ostream&);

private:
	unsigned int* mCellWidths;
};


#endif

