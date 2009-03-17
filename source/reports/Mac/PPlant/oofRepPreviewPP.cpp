#ifndef H_OOFREPWIN
	#include "oofRepPPWindow.h"
#endif
#ifndef REPMACGUIPREVPP
	#include "oofRepPreviewPP.h"
#endif
#include <Fonts.h>


// -------------------------------------------------------
//     o o f R e p M a c G U I P r e v i e w P P
// -------------------------------------------------------


oofRepMacGUIPreviewPP::oofRepMacGUIPreviewPP() :
	mPICTs(0),
	mCommander(0)
	
{
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		
	mFlatFormat = 0;
	mFlatSettings = 0;
#endif
}

void
oofRepMacGUIPreviewPP::finishConstruction(dbRepPICTs* inPICTs, THPrint inPageSetup)
{
	assert(!mPICTs);
	mPICTs = inPICTs;
	pageSetup(inPageSetup);
}


oofRepMacGUIPreviewPP::~oofRepMacGUIPreviewPP()
{
	if (mPrintRecordH)
		::DisposeHandle((Handle)mPrintRecordH); // kept until dtor in case redrawn
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		
	if(mFlatFormat)
		::DisposeHandle((Handle)mFlatFormat);
	if(mFlatSettings)
		::DisposeHandle((Handle)mFlatSettings);
#endif
}


void 
oofRepMacGUIPreviewPP::startEnvironment()
{
// NOTE calcPageHeight is called earlier
	::MoveTo(0,0);
}


void
oofRepMacGUIPreviewPP::endEnvironment()
{
	::ClosePicture();
	// SHould we actually make sure that the page is ended by the report
	// and hence this is an empty picture ?
	assert(mPICTs);  // guard against partial init
	mPICTs->Add(mCurPagePICT);		// This makes a copy
	::KillPicture(mCurPagePICT);
	::SetPort(mSavedPort);
}


void
oofRepMacGUIPreviewPP::drawPageBreak()
{
	// NOTE: Due to the use of ClosePicture() - This routine MUST be called exactly
	//       once for each page generated !
	::ClosePicture();
	mPICTs->Add(mCurPagePICT);		// This makes a copy
	::KillPicture(mCurPagePICT);
	// Make a fresh PICT
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;
	::ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT=::OpenPicture(&mPaperRect);
	::HUnlock((Handle)mPrintRecordH);
	
	// Frame the printable area, useful in debugging and maybe later option
	// may need to be called like this in CalcPageHeight as well
/*	{
		Rect printableArea=tPr.prInfo.rPage;
		::OffsetRect(&printableArea,mOffset.h,mOffset.v);
		::FrameRect(&printableArea);
	}
*/
	
	::MoveTo(0,0);
}

/**/
void
oofRepMacGUIPreviewPP::changeTextStyle(oofRepTextStyle* S)
{
	if (S) {
		/* short leading, unsigned char styleNum  */
		Str255 pFontName;
		pFontName[0]=strlen(S->fontName());
		
		memcpy(&pFontName[1],S->fontName(),pFontName[0]);
	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		FMFontFamily theFontFamily;
		theFontFamily =::FMGetFontFamilyFromName (pFontName);
		::TextFont(theFontFamily);
	#else	
		short fontNo;
		::GetFNum(pFontName,&fontNo);
		::TextFont(fontNo);
	#endif	
		::TextSize(S->fontSize());
		::TextFace(S->fontStyle());
		::RGBForeColor(S->color());
	}
}


unsigned short 
oofRepMacGUIPreviewPP::heightOfTextStyle(oofRepTextStyle* theStyle)
{
	changeTextStyle(theStyle);
	FontInfo fi;
	::GetFontInfo(&fi);	
	
	return (fi.ascent+fi.descent+fi.leading) + theStyle->leading();
}


/*void 	
oofRepMacGUIPreviewPP::moveToVerticalPos(unsigned long pos)
{
	::MoveTo(0,pos);
}
*/ // See comments in oofRepEnvironment

//=-=-=-=-=-
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
unsigned long 
oofRepMacGUIPreviewPP::calcPageHeight()
{

	::GetPort(&mSavedPort);
	if(!mPrintRecordH){
		PMPrintSettings settings = kPMNoPrintSettings; ; // why do i have to do this
		PMPageFormat    format = kPMNoPageFormat; ;
		//PMPrintContext 	printContext;
		//Boolean	accepted;
		
		OSStatus begErr = 	::PMBegin();
		OSStatus err = ::PMNewPrintSettings(&settings);
		OSStatus defErr = ::PMDefaultPrintSettings(settings);
		OSStatus seterr = ::PMSetLastPage (settings,kPMPrintAllPages,false);
		OSStatus newFErr = ::PMNewPageFormat (&format);
		OSStatus newPFrr = ::PMDefaultPageFormat(format);
		assert(!begErr&&begErr!=kPMGeneralError);// TODO - Handle errors
		OSStatus makeorecErr = ::PMMakeOldPrintRecord (settings,format,(char***)&mPrintRecordH);
		OSStatus  dsipErr = ::PMDisposePageFormat (format);
		dsipErr = ::PMDisposePrintSettings (settings);
		begErr = 	::PMEnd();
		//todo add exceptions
	}
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;
	mOffset.h=-tPr.rPaper.left;
	mOffset.v=-tPr.rPaper.top;
	mPaperRect=tPr.rPaper;
	::OffsetRect(&mPaperRect,mOffset.h,mOffset.v);
	::ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT=::OpenPicture(&mPaperRect);
	::HUnlock((Handle)mPrintRecordH);
	
	return mPaperRect.bottom-mPaperRect.top;
}

#else


unsigned long 
oofRepMacGUIPreviewPP::calcPageHeight()
{
	
	::GetPort(&mSavedPort);
#ifdef OOF_PRECARBON_PP_PRINTING
	if(!mPrintRecordH) {
		// Get the default print record - for pagesizing, etc.
		mPrintRecordH=UPrintingMgr::CreatePrintRecord();
		if (!mPrintRecordH) {
			dbConnect::raise("Unable to get printer details - please select a printer in the Chooser", false);
			mPaperRect = oofRect(0,0,0,0);
			return 0;  //**** early exit - failed to get height
		}
	}	
#else
	StPrintSession			Session(UPrinting::GetAppPrintSpec());
	LPrintSpec& theSpec = UPrinting::GetAppPrintSpec();
	if(!mPrintRecordH) {
		mPrintRecordH = (THPrint)theSpec.GetPrintRecord(); //we take ownership of the handle
		if (!mPrintRecordH) {
			dbConnect::raise("Unable to get printer details - please select a printer in the Chooser", false);
			mPaperRect = oofRect(0,0,0,0);
			return 0;  //**** early exit - failed to get height
		}
	}	
#endif// OOF_PRECARBON_PP_PRINTING
	// Make a fresh PICT
	::HLock((Handle)mPrintRecordH);
	TPrint tPr=**mPrintRecordH;
	mOffset.h=-tPr.rPaper.left;
	mOffset.v=-tPr.rPaper.top;
	mPaperRect=tPr.rPaper;
	::OffsetRect(&mPaperRect,mOffset.h,mOffset.v);
	::ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT=::OpenPicture(&mPaperRect);
	::HUnlock((Handle)mPrintRecordH);
	
	return mPaperRect.bottom-mPaperRect.top;
}

#endif
/**/


unsigned long 
oofRepMacGUIPreviewPP::calcPageWidth()
{
	return mPaperRect.right-mPaperRect.left;
}


unsigned long 
oofRepMacGUIPreviewPP::previewPageOrdinal() const
{
	if (!mPICTs)
		return 0;
	else
		return mPICTs->recordNumber();
}

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	
Handle 
oofRepMacGUIPreviewPP::getFlatSettings( ) // caller owns the handle
{
	Handle	settings = nil;
	

	if (mFlatSettings)
		settings = mFlatSettings;
	::HLock(mFlatFormat);
	OSErr  memErr = ::HandToHand(&settings);
	::HUnlock(mFlatFormat);/**/

	return settings;

}


Handle 
oofRepMacGUIPreviewPP::getFlatFormat( )  // caller owns handle
{	

	Handle	format = nil;
	
	if (mFlatFormat)
	format = mFlatFormat;
	::HLock(mFlatFormat);
	OSErr  memErr = ::HandToHand(&format);
	Size howbigS = ::GetHandleSize (format);
	Size howbigSMem = ::GetHandleSize (mFlatFormat);
	::HUnlock(mFlatFormat);/**/
	return format;
}


Handle 
oofRepMacGUIPreviewPP::getOldRecord( ) // caller owns the handle
{
	
	
	Handle	record = nil;
	record = (char**)mPrintRecordH;
	OSErr  memErr = ::HandToHand(&record);
	return record;

}
#endif  // Carbon
