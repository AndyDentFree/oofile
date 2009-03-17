// COPYRIGHT 2001 A.D. Software, All rights reserved

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0

#ifndef H_REPMACPRINT
	#include "oofMacCarbonPrint.h"
#endif
#include <Fonts.h>

#ifdef OOF_DEBUG
	//#define KENDEBUG
#endif
void testPrinter(void);
OSStatus DoPageSetupDialog(PMPageFormat* pageFormat);
OSStatus DoPrintDialog(PMPageFormat* pageFormat, PMPrintSettings* printSettings);

	
// ---------------------------------------
//     o o f R e p M a c P r i n t
// ---------------------------------------

oofMacCarbonPrint::oofMacCarbonPrint(Handle flatFormat, Handle flatSettings):
mAmPrinting(false),
mHaveCarbonSettings(false),
mSettings(kPMNoPrintSettings),
mFormat(kPMNoPageFormat),
mPrintPort(kPMNoReference)
{
	
	if(flatFormat){
		::HLock(flatFormat);
		mFlatFormat = flatFormat;
		::HandToHand((Handle*)&mFlatFormat);
		::HUnlock(flatFormat);
	}
	else{
		mFlatFormat = 0;
	}
	if(flatSettings){
		mFlatSettings = flatSettings;
		::HLock(flatFormat);
		::HandToHand((Handle*)&mFlatSettings);
		::HUnlock(flatFormat);
	}
	else{
		mFlatSettings = 0;
	}
	mPrintRecordH = 0;

}




oofMacCarbonPrint::oofMacCarbonPrint(THPrint pageSetup):
mAmPrinting(false),
mHaveCarbonSettings(false),
mSettings(kPMNoPrintSettings),
mFormat(kPMNoPageFormat),
mPrintPort(kPMNoReference)
{
	
	
	if(pageSetup) {
		// Copy the one passed to us
		mPrintRecordH = pageSetup;
		::HandToHand((Handle*)&mPrintRecordH);	// weird toolbox call copies the param and overwrites the param to return!
	}
	else
		//mPrintRecordH = nil;
		mPrintRecordH = nil;
	mFlatFormat = nil;
	mFlatSettings = nil;
}



oofMacCarbonPrint::~oofMacCarbonPrint()
{
	
	if(mPrintRecordH)
		::DisposeHandle((Handle)mPrintRecordH);
	if(mFlatSettings)
		::DisposeHandle((Handle)mFlatSettings);
	if(mFlatFormat)
		::DisposeHandle((Handle)mFlatFormat);

}		


bool
oofMacCarbonPrint::isPrintingEnvironment() const
{
	return true;
}


void 
oofMacCarbonPrint::startEnvironment()
{
// NOTE calcPageHeight is called earlier
	MoveTo(0,0);
}



void
oofMacCarbonPrint::endEnvironment()
{
	::ClosePicture();
	
	OSStatus pageRectErr;
	Rect   pageRect;
	PMRect	pmPageRect;


	pageRectErr =  ::PMGetAdjustedPageRect (mFormat,&pmPageRect);
	//pageRectErr =  ::PMGetPhysicalPageSize (mFormat,&pmPageRect);
	//assert(( pmPageRect.left >= -32768)&&( pmPageRect.left <= 32767));
	//assert(( pmPageRect.top >= -32768)&&( pmPageRect.top <= 32767));
	//assert(( pmPageRect.bottom >= -32768)&&( pmPageRect.bottom <= 32767));
	//assert(( pmPageRect.right >= -32768)&&( pmPageRect.right <= 32767));

	pageRect.left = (short)pmPageRect.left;
	pageRect.top = (short)pmPageRect.top;
	pageRect.bottom = (short)pmPageRect.bottom;
	pageRect.right = (short)pmPageRect.right;
	

	::OffsetRect(&pageRect,-mOffset.h,-mOffset.v);
	::DrawPicture(mCurPagePICT,&pageRect);
	::KillPicture(mCurPagePICT);
	
	
	OSStatus 	pageErr = ::PMEndPage(mPrintPort);
  	OSStatus docErr = ::PMEndDocument (mPrintPort); 
	OSStatus  dsipErr = ::PMDisposePageFormat (mFormat);
	dsipErr = ::PMDisposePrintSettings (mSettings);

	OSStatus endErr = ::PMEnd();
	::SetPort(mSavedPort);
}


void
oofMacCarbonPrint::drawPageBreak()
{
	::ClosePicture();
	 Rect pageRect=(*mCurPagePICT)->picFrame;
	 
	 ::OffsetRect(&pageRect,-mOffset.h,-mOffset.v);
	 ::DrawPicture(mCurPagePICT,&pageRect);
	 ::KillPicture(mCurPagePICT);
	 
	 OSStatus endPageErr;
	 endPageErr = ::PMEndPage(mPrintPort);
  	 OSStatus  beginPageErr = ::PMBeginPage (mPrintPort,0); 
	 OSStatus fPortErr;
 	GrafPtr pageGrafPort;
	fPortErr = ::PMGetGrafPtr(mPrintPort, &pageGrafPort);
	::SetPort(pageGrafPort);
		
	
	// Make a fresh PICT
	ClipRect(&mPaperRect);		// Important: set clipRgn
	mCurPagePICT= ::OpenPicture(&mPaperRect);
#ifdef KENDEBUG
	Rect printableArea=tPr.prInfo.rPage;
	OSStatus pageRectErr =  ::PMGetUnadjustedPageRect (mFormat,&pageRect);
	::OffsetRect(&printableArea,mOffset.h,mOffset.v);
	assert((!fpageErr));	//TODO - Handle errors
	MoveTo(0,0);


#endif
}

void 
oofMacCarbonPrint::changeTextStyle(oofRepTextStyle* S)
{
	if (S) {
		/* short leading, unsigned char styleNum  */
		Str255 pFontName;
		//short fontNo;
		pFontName[0]=strlen(S->fontName());
		
		memcpy(&pFontName[1],S->fontName(),pFontName[0]);
	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		FMFontFamily theFontFamily;
		theFontFamily =::FMGetFontFamilyFromName (pFontName);
		::TextFont(theFontFamily);
	#else	
		short fontNo;
		GetFNum(pFontName,&fontNo);
		::TextFont(fontNo);
	#endif	
		
		::TextSize(S->fontSize());
		::TextFace(S->fontStyle());
		::RGBForeColor(S->color());
	}
}




unsigned short 
oofMacCarbonPrint::heightOfTextStyle(oofRepTextStyle* theStyle)
{
	changeTextStyle(theStyle);
	FontInfo fi;
	::GetFontInfo(&fi);	
	
	return (fi.ascent+fi.descent+fi.leading) + theStyle->leading();
}


/*void 	
oofMacCarbonPrint::moveToVerticalPos(unsigned long pos)
{
	MoveTo(0,pos);
}
*/ // See comments in oofRepEnvironment



unsigned long 
oofMacCarbonPrint::calcPageHeight()
{
	OSStatus err, beginErr;
	::GetPort(&mSavedPort); // save the current port
	beginErr = ::PMBegin();
	if (mPrintRecordH) {
		mAmPrinting = true;
		//ToDo add error handling	
		err =::PMConvertOldPrintRecord ((Handle)mPrintRecordH,&mSettings,&mFormat);
    	mAmPrinting = true;
	}
	else{
		//--->Boolean isValid;
		::PMNewPrintSettings (&mSettings);
		err = ::PMDefaultPrintSettings(mSettings);

		err = ::PMNewPageFormat (&mFormat);
		err = ::PMDefaultPageFormat(mFormat);
	
	}
	assert(err==0);//ToDo add error handling	

	OSStatus willPrint = DoPrintDialog();
	if(willPrint == noErr){
		OSStatus DocErr = ::PMBeginDocument(mSettings,mFormat,&mPrintPort);
		//=-=-=-=-=-
		//You must balance a call to PMBeginDocument with a call to PMEndDocument.
		//-=-=-==-
		
		// Open the first page
		OSStatus PageErr;
		PageErr =  ::PMBeginPage (mPrintPort,0);
	  	GrafPtr pageGrafPort;
		OSStatus fPortErr = ::PMGetGrafPtr(mPrintPort, &pageGrafPort);
	 	
		assert((!DocErr) &&(!PageErr));	//TODO - Handle errors
		PMRect papRect;
		OSStatus pageRectErr =  ::PMGetAdjustedPaperRect (mFormat,&papRect);
		// is this ok maybe asert that number is not to big 
		//assert((papRect.left >= -32768)&&(papRect.left <= 32767));
		//assert((papRect.top >= -32768)&&(papRect.top <= 32767));
		//assert((papRect.bottom >= -32768)&&(papRect.bottom <= 32767));
		//assert((papRect.right >= -32768)&&(papRect.right <= 32767));
		
		mPaperRect.left = (short)papRect.left;
		mPaperRect.top = (short)papRect.top;
		mPaperRect.bottom = (short)papRect.bottom;
		mPaperRect.right = (short)papRect.right;
		
		mOffset.h= -mPaperRect.left;
		mOffset.v= -mPaperRect.top;
		
		ClipRect(&mPaperRect);		// Important: set clipRgn
		mCurPagePICT= ::OpenPicture(&mPaperRect);
		#ifdef KENDEBUG
		Rect printableArea;
		OSStatus pageRectErr =  ::PMGetUnadjustedPageRect (mFormat,&printableArea);
		::OffsetRect(&printableArea,mOffset.h,mOffset.v);
	#endif
		return mPaperRect.bottom-mPaperRect.top;
	}
	else{
		if(willPrint == kPMCancel){
			::PMEnd();
			return 0;
		}	
		else{
			assert(0); //todo add exception
			return 0; // to make compiler happy
		}
	}	
 
 
 }	

/**/
unsigned long 
oofMacCarbonPrint::calcPageWidth()
{
	return mPaperRect.right-mPaperRect.left;
}

 
Handle 
oofMacCarbonPrint::getFlatSettings( ) // caller owns the handle
{
	Handle	settings = nil;
	
	if (mFlatSettings)
	settings = mFlatSettings;
	OSErr  memErr = ::HandToHand(&settings);
	return settings;

}
Handle 
oofMacCarbonPrint::getFlatFormat( )  // caller owns handle
{	

	Handle	format = nil;
	
	if (mFlatFormat)
	format = mFlatFormat;
	OSErr  memErr = ::HandToHand(&format);
	return format;
}

OSStatus 
oofMacCarbonPrint::DoPrintDialog()
{
    OSStatus    status;
    Boolean     accepted;
  if (mSettings == kPMNoPrintSettings)
   {
        status = ::PMNewPrintSettings(&mSettings);
		
        if ((status == noErr) && (mSettings != kPMNoPrintSettings))
            status = ::PMDefaultPrintSettings(mSettings);
    }
    else
        status = ::PMValidatePrintSettings(mSettings, kPMDontWantBoolean);
	
    //  Display the Print dialog.
    if (status == noErr)
    {
        status = ::PMPrintDialog(mSettings, mFormat, &accepted);
        if (!accepted)
            status = kPMCancel; // user clicked Cancel button
    }
		
    return status;
	
}   //  DoPrintDialog


#endif  // entire contents conditional on being in Carbon
