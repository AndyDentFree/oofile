// COPYRIGHT 1994 A.D. Software, All rights reserved

// report-writer layer of OOFILE database

// NOTE inline definitions included at end of this header file

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFRW
	#include "oofrw.h"
#endif
#include <string.h>

#ifdef OOF_MEM_DEBUG
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

unsigned int dbRepLine::mWidth;

// -------------------------------------------------------
//             d b R e p O v e r R u n
// -------------------------------------------------------

// -------------------------------------------------------
//                d b R e p L i n e
// -------------------------------------------------------

dbRepLine::~dbRepLine()
{
	delete[] mLine;
}


void
dbRepLine::prepare()
// Generate a new Line and fill it with whitespace
//
{
	if(!mLine){
		mLine = new char[mWidth+1];		// We'll null-terminate just to be safe
		assert(mLine);
		mLine[mWidth]='\0';
		memset(mLine, 0x20, mWidth);    // 0x20 is ASCII for space
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
	}
}


void
dbRepLine::clear()
{
	delete[] mLine;
	mLine = 0;
}

void dbRepLine::drawNCharsAt(unsigned int hPos, const char* theChars, unsigned
int len)
{
	prepare();				// Make sure we have some space to draw on
	unsigned long memLen=strlen(theChars);
	if(memLen>len)
		memLen=len;
	memcpy(&mLine[hPos],theChars,memLen);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


void
dbRepLine::fillNCharsAt(unsigned int hPos, char theChar, unsigned int len)
{
	prepare();				// Make sure we have some space to draw on
	memset(&mLine[hPos],theChar,len);
#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
	OOF_MEM_DEBUG_FORCE_POOL_CHECK
#endif
}


void
dbRepLine::drawToStream(unsigned int hPos, unsigned int len, ostream& os)
{
	if(mLine)
		os.write(&mLine[hPos],len);
}

// -------------------------------------------------------
//               d b R e p S i z e r
// -------------------------------------------------------


// -------------------------------------------------------
//                d b R e p P a g e
// -------------------------------------------------------

dbRepPage::dbRepPage() :
	mPageMap(0),
	mFieldPos(0),
	mPageStart(0)
{
// Do nothing - We are built by the appropriate FormatFor... routine.
//
//   At present, only CharStream is supported
};


dbRepPage::~dbRepPage()
{
	delete[] mPageMap;
}


void
dbRepPage::draw(dbRepSizer Sizer, ostream& os)
{
	for(unsigned int pageNum=0;pageNum<mNumPages;pageNum++)
	{
		for(unsigned int i=0;i<Sizer.mTopMargin;i++)  		// Top Margin
			os << '\n';
		
		unsigned int thispageWidth;	// Work out how much we have to draw
		if (pageNum+1==mNumPages) {
			thispageWidth=mWidth-mPageBreak[pageNum];
		} else {
			thispageWidth=mPageBreak[pageNum+1]-mPageBreak[pageNum];
		}
		for(unsigned int lineNum=0;lineNum<mNumLines;lineNum++)
		{
			for(unsigned int l=0;l<Sizer.mLeftMargin;l++)
				os << ' ';
			mPageMap[lineNum].drawToStream(mPageBreak[pageNum],thispageWidth,os);	// draw the stuff
			os << '\n';
		}
		for(unsigned int k=0;k<Sizer.mBottomMargin;k++)		// Bottom Margin
			os << '\n';
		
		endPage(os);
	}
}


void
dbRepPage::clearLines(unsigned int start, unsigned int end)
{
	assert(end>=start);
	assert(start<mNumLines);
	
	for(unsigned int i=start;i<=end;i++)
		mPageMap[i].clear();
}


void
dbRepPage::endPage(ostream& /*os*/)
{
//	os << "--" << '\n';
}

// -------------------------------------------------------
//                      d b R e p
// -------------------------------------------------------

void
dbRep::extract(ostream& os)
{
  unsigned int rNum=1;

	unsigned int numFields = mFields.count();
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {
		os << '\n' << "Record# " << rNum++ << '\n';
		for (unsigned int i=0; i<numFields; i++) {		// start field iteration (horizontal)
			dbField  *theField = (dbField  *) mFields[i]; 		  // safe downcast
			oofString theString = theField->copyString();
			os << theField->fieldName() << " : " << theString << '\n';
		}
	
		mFields.source()->next();	
	}
}

char *dbRep::copyStr(const char *theString)
{
	unsigned long dataLen = strlen(theString);

	char *retStr = new char[dataLen+1];
	
	strcpy(retStr,theString);
	
	return(retStr);
}


void
dbRep::setStyle(const ReportStyles style)
{
	mReportStyle = style;
}

// -------------------------------------------------------
//                  d b R e p C h a r
// -------------------------------------------------------


void
dbRepChar::formatForCharStream()
// Builds the contents of our dbRepPage for Character Stream.
//
{
	unsigned int numfields;
	
	switch(mReportStyle) {
		case columnar: numfields = mFields.count();
										break;
										
		case pageWise: numfields = 2;
										break;
		
		default: assert(false);
	}
	
	mPage.mFieldPos = new unsigned int[numfields];

	unsigned int Printable = mSizer.mPageWidth - (mSizer.mLeftMargin +
mSizer.mRightMargin);		// Actual Printable Width
	
	mPage.mWidth = 0;
	int FirstOnPage = true;
	unsigned int CurrpageWidth = 0;
	mPage.mNumPages = 1;
	mPage.mPageBreak.append(0);
	for (unsigned int i=0; i<numfields; i++)
	{
		mPage.mFieldPos[i] = mPage.mWidth;
		if (CurrpageWidth + mColWidths[i] <= Printable) {	// Can we fit the next field in this page ?
			mPage.mWidth+=mColWidths[i];
			CurrpageWidth+=mColWidths[i];
			FirstOnPage = false;
			if (CurrpageWidth + mSizer.mColSepWidth <= Printable) {		// Add space between columns
				mPage.mWidth+=mSizer.mColSepWidth;
				CurrpageWidth+=mSizer.mColSepWidth;
			} else {
				if(i!=numfields-1) {																// If there are any more fields...
					mPage.mNumPages++;																// Time for a new page
					if(mReportStyle==pageWise)
						assert(false);
					mPage.mPageBreak.append(mPage.mWidth);
					FirstOnPage = true;
					CurrpageWidth = 0;
				}
			}
		} else {
			if (FirstOnPage) {
				mColWidths[i] = Printable;		// We're the only field on this page - We'll have to squeeze in
				CurrpageWidth+=Printable;
				mPage.mWidth+=Printable;
				FirstOnPage = false;
			} else {
				i--;																							// Go back and fetch this one again on a new page
				mPage.mNumPages++;																// Time for a new page
				mPage.mPageBreak.append(mPage.mWidth);
				FirstOnPage = true;
				CurrpageWidth = 0;
			}
		}
	}

	mPage.mNumLines = mSizer.mPageHeight - (mSizer.mTopMargin + mSizer.mBottomMargin);	// Actual Printable Height
	assert(mPage.mNumLines>0);
	mPage.mPageMap = new dbRepLine[mPage.mNumLines];
	
	dbRepLine::mWidth = mPage.mWidth;
	mBuilt = true;
}


void
dbRepChar::drawHeader(ostream&)
{
	switch(mReportStyle) {
		case columnar: {
			unsigned int numFields = mFields.count();
			for(unsigned int i=0;i<numFields;i++)
			{
				dbField  *theField = (dbField  *) mFields[i];  // safe downcast (Or so the Guru tells me !)
				mPage.mPageMap[0].drawNCharsAt(mPage.mFieldPos[i],theField->fieldName(),mColWidths[i]);
				mPage.mPageMap[1].fillNCharsAt(mPage.mFieldPos[i],'-',mColWidths[i]);
			}
			mPage.mBodyStart = 3;
			break;
		}
			
		case pageWise: mPage.mBodyStart = 0;			// PageWise has no header.]
		  break;
		
		default:assert(false);
	}
}


unsigned int
dbRepChar::drawWrappedChars(unsigned int line,unsigned int hPos,unsigned
int width,char **theString)
{
		unsigned long dataLen = strlen(*theString);
		unsigned int numLines = 0;
		
		while((dataLen>0)&&(line+numLines<mPage.mNumLines)) {
		
			unsigned int i=0;
			
			char *simpleStr=*theString;
			
			for(;((simpleStr[i]!='\0')&&(simpleStr[i]!='\n')&&(i<width));i++)
				;
					// Hunt down and kill carriage returns
					// Perhaps we should convert tabs to spaces as well ?
					
			if (i==width) {	// we ran out of room - back up to the end of the last word.
				for(;((simpleStr[i]!=' ')&&(i>0));i--)
					;
				if(i==0)
					i=width;	// the word was too long for the field width - so we'll just chop it !
			}
			
			*theString+=i;
			dataLen-=i;
			if ((simpleStr[i]=='\n')||(simpleStr[i]==' '))	{
				*theString+=1;					// skip over the CR or the space
				dataLen--;
			}

			mPage.mPageMap[line+numLines].drawNCharsAt(hPos,simpleStr,i);				// draw the stuff !
			
			numLines++;
		}

		if (dataLen>0)		// It won't fit !
			numLines=0xff;		
		
		return(numLines);
}


void
dbRepChar::drawColumnar(ostream& os)
{
// NOTE: mFields.table = the database/selection
// mFields = ordered list of fields to report

int FirstOnPage = true;
	
  dbRepOverRun *recOverRun = new dbRepOverRun[mFields.count()];
	
	unsigned int currentLine = mPage.mBodyStart;
	char *theString;
	char *deleteMe;
	int IsOverRun = false;
	int OverRunning = false;

	unsigned int numFields = mFields.count();
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {

		int Advance = true;
		unsigned int maxLines = 0;

		for (unsigned int i=0; i<numFields; i++) {		// start field iteration (horizontal)
			dbField  *theField = (dbField  *) mFields[i]; 		  // safe downcast
			
			if(IsOverRun)
			{
				if(!recOverRun[i].OverRun)
					continue;
				theString = recOverRun[i].OverRun;
				deleteMe = recOverRun[i].DeleteMe;
				recOverRun[i].OverRun=0;
			} else {
				oofString str = theField->copyString();
// keep original char* logic as it's rather complex to recode and this is an essentially obsolete
// class which won't be evolving				
				theString = str.orphan();	// retrieve the field data as a char*
				deleteMe = theString;			// keep track of our original pointer
			}
			
			unsigned int linesUsed = drawWrappedChars(currentLine,mPage.mFieldPos[i],mColWidths[i],&theString);
			// WARNING - drawWrappedChars WILL change the value of theString !
			
			if (linesUsed==0xff)
				if (FirstOnPage) {
					recOverRun[i].OverRun=theString;
					recOverRun[i].DeleteMe=deleteMe;
					Advance = false;
					OverRunning = true;
					linesUsed = mPage.mNumLines-currentLine;
				} else {
					mPage.clearLines(currentLine,mPage.mNumLines-1);
					currentLine = mPage.mNumLines;
					delete[] deleteMe;
					Advance = false;
					break;
				}
			
			if (linesUsed>maxLines)
				maxLines=linesUsed;
			
			if((!OverRunning)||(OverRunning&&(recOverRun[i].DeleteMe!=deleteMe)))
				delete[] deleteMe;		// throw it away now that we're done
		}
		IsOverRun = OverRunning;
		OverRunning = false;

		currentLine+=(mSizer.mBlockVertSep + maxLines);
		
		FirstOnPage = false;
		
		if(currentLine>=mPage.mNumLines) {	// time to start a new page
			mPage.draw(mSizer,os);
			mPage.clearLines(mPage.mBodyStart,mPage.mNumLines-1);
			currentLine = mPage.mBodyStart;
			FirstOnPage = true;
		}
		if (Advance)
			mFields.source()->next();
	}
	
	if(currentLine != mPage.mBodyStart)
		mPage.draw(mSizer, os);
}


void
dbRepChar::drawPageWise(ostream& os)
{
// NOTE: mFields.table = the database/selection
// mFields = ordered list of fields to report

	int FirstOnPage = true;
	
	unsigned int currentLine = mPage.mBodyStart;
	char *theString;
	char *deleteMe;
	unsigned int fNum;

	dbRepOverRun *recOverRun = new dbRepOverRun[2];	
	int IsOverRun = false;
	int OverRunning = false;

	unsigned int numFields = mFields.count();
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {
	
		unsigned int recordStart = currentLine;
		int Advance = true;
		if (!IsOverRun)
			fNum=0;																											// start field iteration (horizontal)
		
		while (fNum<numFields) {
			dbField  *theField = (dbField  *) mFields[fNum]; 	 // safe downcast
			
			unsigned int maxLines = 0;

			for (unsigned int i=0; i<2; i++) {	// page left->right
				if(IsOverRun)
				{
					if(!recOverRun[i].OverRun)
						continue;
					theString = recOverRun[i].OverRun;
					deleteMe = recOverRun[i].DeleteMe;
					recOverRun->OverRun=0;
				} else {
					if (i==0)
						theString = copyStr(theField->fieldName());	// retrieve the field name as a separate char*
					else {
						oofString str = theField->copyString();
// keep original char* logic as it's rather complex to recode and this is an essentially obsolete
// class which won't be evolving				
						theString = str.orphan();// retrieve the field data as a char*
					}
					deleteMe = theString;	// keep track of our original pointer
				}
				
				unsigned int linesUsed = drawWrappedChars(currentLine,mPage.mFieldPos[i],mColWidths[i],&theString);
				// WARNING - drawWrappedChars WILL change the value of theString !
				
				if (linesUsed==0xff)
					if (FirstOnPage) {
						recOverRun[i].OverRun=theString;
						recOverRun[i].DeleteMe=deleteMe;
						Advance = false;
						OverRunning = true;
						currentLine = mPage.mNumLines;
					} else {
						mPage.clearLines(recordStart,mPage.mNumLines-1);
						currentLine = mPage.mNumLines;
						delete[] deleteMe;
						Advance = false;
						break;
					}
				
				if (linesUsed>maxLines)
					maxLines=linesUsed;
				
				if((!OverRunning)||(OverRunning&&(recOverRun[i].DeleteMe!=deleteMe)))
					delete[] deleteMe;																	// throw it away now that we're done

			}
			IsOverRun = OverRunning;
			OverRunning = false;

			if(IsOverRun)
				break;
				
			currentLine+=(mSizer.mBlockVertSep + maxLines);
			
			fNum++;		// Go on to the next field - We won't get here if we're OverRun
		}
		
		FirstOnPage = false;
		
		if(currentLine>=mPage.mNumLines) {	// time to start a new page
			mPage.draw(mSizer,os);
			mPage.clearLines(mPage.mBodyStart,mPage.mNumLines-1);
			currentLine = mPage.mBodyStart;
			FirstOnPage = true;
		}
		if (Advance)
			mFields.source()->next();
	}
	
	if(currentLine != mPage.mBodyStart)
		mPage.draw(mSizer, os);
}


void
dbRepChar::draw(ostream& os)
{
	if(!mBuilt)
		formatForCharStream();
		
	drawHeader(os);

	switch(mReportStyle)  {
		case columnar: drawColumnar(os);
										break;
		
		case pageWise: drawPageWise(os);
										break;
		
		default:assert(false);
						 break;
	}
}



