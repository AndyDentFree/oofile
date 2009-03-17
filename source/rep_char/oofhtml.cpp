// COPYRIGHT 1994 A.D. Software, All rights reserved

// report-writer layer of OOFILE database

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFRW
	#include "oofrw.h" // We're based on the standard Report Writer
#endif
#ifndef H_OOFHTML
	#include "oofhtml.h"
#endif
#include <string.h>

#ifdef OOF_MEM_DEBUG
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// -------------------------------------------------------
//            d b R e p L i n e H T M L
// -------------------------------------------------------

// This Class is based heavily upon the standard dbRepLine, but
// is different enough in its storage to warrant a separate class

dbRepLineHTML::~dbRepLineHTML()
{
	delete[] mLine;
}


void 
dbRepLineHTML::prepare()
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
dbRepLineHTML::clear()
{
	if(!mLine)
		prepare();
	else {
		mLine[mWidth]='\0';
		memset(mLine, 0x20, mWidth);    // 0x20 is ASCII for space
	}
}


void dbRepLineHTML::drawNCharsAt(unsigned int hPos, const char* theChars, unsigned int len)
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
dbRepLineHTML::draw(unsigned int indent, ostream& os)
{
	if(mLine) {
		for(unsigned int l=0;l<indent;l++)
			os << ' ';
		unsigned int m=mWidth-1;
		for(;(mLine[m]==' ')&&(m>0);m--)
			;
		if(m>0) {
			mLine[m+1]='\0';
			os << mLine;
		}
	}
}


// -------------------------------------------------------
//                 d b R e p H T M L
// -------------------------------------------------------

void 
dbRepHTML::formatForCharStream()
{
	unsigned int numfields;
	
	switch(mReportStyle) {
		case columnar: numfields = mFields.count();
										break;
										
		case pageWise: numfields = 2;
										break;
		
		default: assert(false);
	}
	
	mLine.mFieldPos = new unsigned int[numfields];
	
	// In this version, we ignore, page width and height directives !

	mLine.mWidth = 0;
	for (unsigned int i=0; i<numfields; i++)
	{
		mLine.mFieldPos[i] = mLine.mWidth;
		mLine.mWidth+=mColWidths[i];
		mLine.mWidth+=mSizer.mColSepWidth;  // Add space between columns
	}
}


void 
dbRepHTML::drawHeader(ostream& os)
{
//	os << "Content-type: text/html" << endl << endl;	// This prints a MIME header, but I think it should
														// really be handled by the cgi-kit !
	
	os << "<title>" << mSizer.mReportTitle << "</title>" << endl;
	os << "<h2 align=center>" << mSizer.mReportTitle << "</h2>" << endl;
	os << "<pre>" << endl;
	
	switch(mReportStyle) {
		case columnar: {	unsigned int numFields = mFields.count();
		
								for(unsigned int i=0;i<numFields;i++)
										{
											dbField  *theField = (dbField  *) mFields[i];  // safe downcast (Or so the Guru tells me !)
											mLine.drawNCharsAt(mLine.mFieldPos[i],theField->fieldName(),mColWidths[i]);
										}
								mLine.draw(mSizer.mLeftMargin, os);
								mLine.clear();
								
								os << "<br><hr><br>" << endl;					
								break;
							}
										
		case pageWise: {
								os << "<br><hr><br>" << endl;
								break;
							}		  
		
		default:assert(false);
	}
}


bool 
dbRepHTML::drawWrappedCharsSingle(unsigned int hPos,unsigned int width,char **theString)
{
		unsigned long dataLen = strlen(*theString);
		bool finished = true;
		
		if(dataLen>0) {
		
			unsigned int i=0;
			
			char *simpleStr=*theString;
			
			for(;((simpleStr[i]!='\0')&&(simpleStr[i]!='\n')&&(i<width));i++)
				;
					// Hunt down and kill carriage returns
					// Perhaps we should convert tabs to spaces as well ?
					
			if (i==width) {					// we ran out of room - back up to the end of the last word.
				for(;((simpleStr[i]!=' ')&&(i>0));i--)
					;
				if(i==0)
					i=width;				// the word was too long for the field width - so we'll just chop it !
			}
			
			*theString+=i;
			dataLen-=i;
			if ((simpleStr[i]=='\n')||(simpleStr[i]==' '))	{
				*theString+=1;					// skip over the CR or the space
				dataLen--;
			}

			mLine.drawNCharsAt(hPos,simpleStr,i);				// draw the stuff !
		}

		if (dataLen>0)		// It won't fit !
			finished = false;		
		
		return(finished);
}


void 
dbRepHTML::drawColumnar(ostream& os)
{
	dbRepOverRun *recOverRun = new dbRepOverRun[mFields.count()];
	
	char *theString;
	char *deleteMe;
	int IsOverRun = false;
	int OverRunning = false;

	unsigned int numFields = mFields.count();
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {

		int Advance = true;

		for (unsigned int i=0; i<numFields; i++) {		// start field iteration (horizontal)
			
			if(IsOverRun)
			{
				if(!recOverRun[i].OverRun)
					continue;
				theString = recOverRun[i].OverRun;
				deleteMe = recOverRun[i].DeleteMe;
				recOverRun[i].OverRun=0;
			} else {
				dbField  *theField = (dbField  *) mFields[i]; 		 	// safe downcast
				oofString str = theField->copyString();
// keep original char* logic as it's rather complex to recode and this is an essentially obsolete
// class which won't be evolving				
				theString = str.orphan();// retrieve the field data as a char*
				deleteMe = theString;									// keep track of our original pointer
			}
			
			bool finished = drawWrappedCharsSingle(mLine.mFieldPos[i],mColWidths[i],&theString);
			// WARNING - drawWrappedCharsSingle WILL change the value of theString !
			
			if (!finished) {
				recOverRun[i].OverRun=theString;
				recOverRun[i].DeleteMe=deleteMe;
				Advance = false;
				OverRunning = true;
			} else {
				delete[] deleteMe;
				recOverRun[i].OverRun=0;
			}
			
		}
		IsOverRun = OverRunning;
		OverRunning = false;

		mLine.draw(mSizer.mLeftMargin, os);
		os << endl;
		mLine.clear();
		
		if (Advance) {
			for(unsigned int k=0;k<mSizer.mBlockVertSep;k++)
				os << "<br>" << endl;
			mFields.source()->next();
		}
	}
	os << "<hr>" << endl;
	os << "</pre>" << endl;
}

void 
dbRepHTML::drawPageWise(ostream& os)
{
// NOTE: mFields.table = the database/selection
// mFields = ordered list of fields to report

	char *theString;
	char *deleteMe;
	unsigned int fNum;

	dbRepOverRun *recOverRun = new dbRepOverRun[2];	
	int IsOverRun = false;
	int OverRunning = false;

	unsigned int numFields = mFields.count();
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {
	
		if (!IsOverRun)
			fNum=0;																											// start field iteration (horizontal)
		
		while (fNum<numFields) {
			dbField  *theField = (dbField  *) mFields[fNum]; 			// safe downcast
			
			for (unsigned int i=0; i<2; i++) {							// page left->right
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
					deleteMe = theString;													// keep track of our original pointer
				}
				
				bool finished = drawWrappedCharsSingle(mLine.mFieldPos[i],mColWidths[i],&theString);
				// WARNING - drawWrappedCharsSingle WILL change the value of theString !
				
				if (!finished) {
					recOverRun[i].OverRun=theString;
					recOverRun[i].DeleteMe=deleteMe;
					OverRunning = true;
				} else {
					delete[] deleteMe;
					recOverRun[i].OverRun=0;
				}
				
			}
			IsOverRun = OverRunning;
			OverRunning = false;

			mLine.draw(mSizer.mLeftMargin, os);
			os << endl;
			mLine.clear();
		
			if(!IsOverRun) {
 				fNum++;	   // Go on to the next field - We won't get here if we're OverRun
 				os << "<br>" << endl;
 			}
		}
		
		for(unsigned int k=0;k<(mSizer.mBlockVertSep);k++)
			os << "<br>" << endl;
		os << "<hr><br>" << endl;
		mFields.source()->next();
	}
	os << "</pre>" << endl;
}


void 
dbRepHTML::draw(ostream& os)
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

// -------------------------------------------------------
//            d b R e p H T M L T a b l e
// -------------------------------------------------------

void 
dbRepHTMLTable::formatForCharStream()
{
	unsigned int numfields;
	
	switch(mReportStyle) {
		case columnar: numfields = mFields.count();
										break;
										
		case pageWise: numfields = 2;
										break;
		
		default: assert(false);
	}
	
	mCellWidths = new unsigned int[numfields];
	
	// In this version, we ignore, page width and height directives !

// Just thinking about using percentages !

//	unsigned int PageWidth = mColWidths[0].;
//	for (unsigned int i=1; i<numfields; i++)
//	{
//		PageWidth+=mColWidths[i];
//	}

	for (unsigned int j=0; j<numfields; j++)
	{
		mCellWidths[j]=mColWidths[j]*5;   // Number of pixels wide - very nieve !
	}
}


void 
dbRepHTMLTable::drawHeader(ostream& os)
{
//	os << "Content-type: text/html" << endl << endl;
	
	os << "<title>" << mSizer.mReportTitle << "</title>" << endl;
	os << "<h2 align=center>" << mSizer.mReportTitle << "</h2>" << endl;
	
	switch(mReportStyle) {
		case columnar: {	unsigned int numFields = mFields.count();
		
								os << "<table border=" << mSizer.mColSepWidth << ">" << endl;
								os << "<tr>" << endl;
								for(unsigned int i=0;i<numFields;i++)
										{
											dbField  *theField = (dbField  *) mFields[i];  // safe downcast (Or so the Guru tells me !)
											os << "<th width=" << mCellWidths[i] << ">" << theField->fieldName() << "</th>";
										}
								os << "</tr>" << endl;					
								break;
							}
										
		case pageWise: break;		  
		
		default:assert(false);
	}
}


void 
dbRepHTMLTable::drawColumnar(ostream& os)
{
// NOTE: mFields.table = the database/selection
// mFields = ordered list of fields to report

	unsigned int numFields = mFields.count();
	
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {
	
		os << "<tr>";

		for (unsigned int i=0; i<numFields; i++) {		// start field iteration (horizontal)
			
			dbField  *theField = (dbField  *) mFields[i]; 		 	// safe downcast
			os << "<td>" << theField->copyString() << "</td>";	 	
		}
			
		os << "</tr>" << endl;
		
		mFields.source()->next();
	}
	os << "</table>" << endl;
}


void 
dbRepHTMLTable::drawPageWise(ostream& os)
{
// NOTE: mFields.table = the database/selection
// mFields = ordered list of fields to report

	unsigned int numFields = mFields.count();
	
	mFields.source()->start();		// start record iteration (vertical)
	while (mFields.source()->more()) {
	
		os << "<table border=" << mSizer.mColSepWidth << ">" << endl;
		
		for(unsigned int fNum=0;fNum<numFields;fNum++) {
		
			dbField  *theField = (dbField  *) mFields[fNum]; 		// safe downcast
			
			os << "<tr>";

			for (unsigned int i=0; i<2; i++) {						// page left->right
			
				if (fNum==0)
					os << "<td width=" << mCellWidths[i] << ">";
				else
					os << "<td>";
					
				if (i==0)
					os << "<b>" << theField->fieldName() << "</b>";	// retrieve the field name
				else
					os << theField->copyString();					// retrieve the field data

				os << "</td>";			
			}
			
			os << "</tr>" << endl;
		}
		
		os << "</table><p>" << endl;
		
		mFields.source()->next();
	}
}

	
void 
dbRepHTMLTable::draw(ostream& os)
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
