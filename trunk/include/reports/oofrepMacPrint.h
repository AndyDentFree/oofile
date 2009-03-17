#ifndef H_REPMACPRINT
#define H_REPMACPRINT

// COPYRIGHT 1996 A.D. Software, All rights reserved

// GUI report-writer layer of OOFILE database


#ifndef H_OOFFILE
	#include "oofile.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPMAC
	#include "oofrepMac.h"
#endif

class dbRepPICTs;

/**
	Specify Macintosh GUI drawing environment printed to classic printer.
	\ingroup oofRenderMac
*/
class oofRepMacPrint : public oofRepMacEnv {
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
