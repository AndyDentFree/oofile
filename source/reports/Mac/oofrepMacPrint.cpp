#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
#else

#ifndef H_REPMACPRINT
	#include "oofRepMacPrint.h"
#endif
#include <Fonts.h>

#ifndef __PRINTING__
	#include <Printing.h>
#endif

#ifdef OOF_DEBUG
	//#define KENDEBUG
#endif

// ---------------------------------------
//     o o f R e p M a c P r i n t
// ---------------------------------------


oofRepMacPrint::oofRepMacPrint(THPrint pageSetup)
{
	if(pageSetup) {
		// Copy the one passed to us
		mPrintRecordH = pageSetup;
		::HandToHand((Handle*)&mPrintRecordH);	// weird toolbox call copies the param and overwrites the param to return!
	}
	else
		mPrintRecordH = nil;
}


oofRepMacPrint::~oofRepMacPrint()
{
	::DisposeHandle((Handle)mPrintRecordH);
}


bool
oofRepMacPrint::isPrintingEnvironment() const
{
	return true;
}


void 
oofRepMacPrint::startEnvironment()
{
// NOTE calcPageHeight is called earlier
	MoveTo(0,0);
}


void 
oofRepMacPrint::endEnvironment()
{
	::ClosePicture();
	
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;
	Rect pageRect=tPr.prInfo.rPage;
	::HUnlock((Handle)mPrintRecordH);

	::OffsetRect(&pageRect,-mOffset.h,-mOffset.v);
	::DrawPicture(mCurPagePICT,&pageRect);
	::KillPicture(mCurPagePICT);

	::PrClosePage(mPrtPort);
	::PrCloseDoc(mPrtPort);

	if ((*mPrintRecordH)->prJob.bJDocLoop==bSpoolLoop) {
		::PrPicFile(mPrintRecordH,0,0,0,&mPrStatus);
		assert(!::PrError());	// KENTODO - Handle errors
	}
	
	::PrClose();
	::SetPort(mSavedPort);
}


void
oofRepMacPrint::drawPageBreak()
{
	::ClosePicture();
	
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;

	Rect pageRect=(*mCurPagePICT)->picFrame;
	::OffsetRect(&pageRect,-mOffset.h,-mOffset.v);
	::DrawPicture(mCurPagePICT,&pageRect);
	::KillPicture(mCurPagePICT);

	::PrClosePage(mPrtPort); 	// Close the current page
	::PrOpenPage(mPrtPort, 0); 	// and open a new one

	// Make a fresh PICT
	ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT=OpenPicture(&mPaperRect);
#ifdef KENDEBUG
	Rect printableArea=tPr.prInfo.rPage;
	::OffsetRect(&printableArea,mOffset.h,mOffset.v);
#endif

	::HUnlock((Handle)mPrintRecordH);
	
#ifdef KENDEBUG
	// Frame the printable area
	::FrameRect(&printableArea);
#endif
	
	assert(!::PrError());		// KENTODO - Handle errors
	MoveTo(0,0);
}


void 
oofRepMacPrint::changeTextStyle(oofRepTextStyle* S)
{
	if (S) {
		/* short leading, unsigned char styleNum  */
		Str255 pFontName;
		short fontNo;
		pFontName[0]=strlen(S->fontName());
		memcpy(&pFontName[1],S->fontName(),pFontName[0]);
		::GetFNum(pFontName,&fontNo);
		::TextFont(fontNo);
		::TextSize(S->fontSize());
		::TextFace(S->fontStyle());
	}
}


unsigned short 
oofRepMacPrint::heightOfTextStyle(oofRepTextStyle* theStyle)
{
	changeTextStyle(theStyle);
	FontInfo fi;
	::GetFontInfo(&fi);	
	
	return (fi.ascent+fi.descent+fi.leading) + theStyle->leading();
}


/*void 	
oofRepMacPrint::moveToVerticalPos(unsigned long pos)
{
	MoveTo(0,pos);
}
*/ // See comments in oofRepEnvironment

unsigned long 
oofRepMacPrint::calcPageHeight()
{
	::GetPort(&mSavedPort);
	if (!mPrintRecordH) {
		mPrintRecordH = (THPrint) NewHandle(sizeof(TPrint));
		assert(mPrintRecordH);
		// Get the default print record - for pagesizing, etc.
		::PrOpen();
		assert(PrError() == noErr);
		::PrintDefault(mPrintRecordH);
		// KENTODO - Do print dialog magic
		::PrClose();
	} 
	
	::PrOpen();
	assert(!::PrError());	// KENTODO - Handle errors
		
	mPrtPort = PrOpenDoc(mPrintRecordH, 0,0); //let system allocate port and IO buf
	
	// Open the first page
	PrOpenPage(mPrtPort, 0);
	assert(!::PrError());	// KENTODO - Handle errors
	
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;
	mOffset.h=-tPr.rPaper.left;
	mOffset.v=-tPr.rPaper.top;
	mPaperRect=tPr.rPaper;
	::OffsetRect(&mPaperRect,mOffset.h,mOffset.v);
	ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT=OpenPicture(&mPaperRect);
#ifdef KENDEBUG
	Rect printableArea=tPr.prInfo.rPage;
	::OffsetRect(&printableArea,mOffset.h,mOffset.v);
#endif
	::HUnlock((Handle)mPrintRecordH);

	return mPaperRect.bottom-mPaperRect.top;
}

unsigned long 
oofRepMacPrint::calcPageWidth()
{
	return mPaperRect.right-mPaperRect.left;
}

#endif  // wrap all if not TARGET_API_MAC_CARBON
