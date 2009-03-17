// COPYRIGHT 2000 A.D. Software, All rights reserved

// GUI Carbon report-writer layer of OOFILE database

// this version does not support sessions
#ifndef H_REPMACCARBPRINT
#define	H_REPMACCARBPRINT
#ifndef H_OOFFILE
	#include "oofile.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPMAC
	#include "oofrepMac.h"
#endif
#ifndef __PMAPPLICATION__
	#include <PMApplication.h>
#endif

class dbRepPICTs;

/**
	Specify Macintosh GUI drawing environment printed to Carbon printer.
	\ingroup oofRenderMac
*/
class oofMacCarbonPrint : public oofRepMacEnv {
public:
	
	oofMacCarbonPrint(THPrint pageSetup=nil);
	oofMacCarbonPrint(Handle flatFormat = nil,Handle settings = nil);
	~oofMacCarbonPrint();
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	//virtual void 	moveToVerticalPos(unsigned long); // see oofRepEnvironment comments
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	virtual bool isPrintingEnvironment() const;

	virtual void startEnvironment();
	virtual void endEnvironment();
	virtual Handle getFlatSettings(); // caller owns the handle
	virtual Handle getFlatFormat();   // caller owns handle
	OSStatus	DoPrintDialog();
private:
TPPrPort		mPrtPort;
	TPrStatus		mPrStatus;
	PicHandle		mCurPagePICT;
	Rect			mPaperRect;		// Paper Size
Point			mOffset;	
bool			mAmPrinting;
bool			mHaveCarbonSettings;
THPrint 		mClassicRecord;

//PMPrintSettings *mSettings; 
//PMPageFormat    *mFormat;
//PMPrintContext *mPrintContext;

//Handle			mFlatFormat;
//Handle			mFlatSettings;



PMPrintSettings mSettings; // why do i have to do this
PMPageFormat    mFormat;
PMPrintContext mPrintPort;






/*OSStatus PMBeginDocument (
    PMPrintSettings printSettings, 
    PMPageFormat pageFormat, 
    PMPrintContext *printContext
);*/


};



/*class oofRepMacPrint : public oofRepMacEnv {
public:
	oofRepMacPrint(THPrint pageSetup=nil);
	~oofRepMacPrint();
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	//virtual void 	moveToVerticalPos(unsigned long); // see oofRepEnvironment comments
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	virtual bool isPrintingEnvironment() const;

	virtual void startEnvironment();
	virtual void endEnvironment();
	
private:
	TPPrPort		mPrtPort;
	TPrStatus		mPrStatus;
	PicHandle		mCurPagePICT;
	Rect			mPaperRect;		// Paper Size
	Point			mOffset;		// Paper to printable area offset
};

#endif
*/

#endif