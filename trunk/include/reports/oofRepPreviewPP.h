#ifndef REPMACGUIPREVPP
#define REPMACGUIPREVPP

#ifndef H_OOFFILE
	#include "oofile.h" // Hmm - I shouldn't need this !
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	#ifndef H_REPMACCARBPRINT
		#include "oofMacCarbonPrint.h"
	#endif
#else
	#ifndef H_REPMACPRINT
		#include "oofRepMacPrint.h"	// to get the parent class
	#endif
#endif

class dbRepPICTs;
class LCommander;

/**
	Specify Macintosh GUI drawing environment previewed in PowerPlant window.
	\ingroup oofRenderMac
*/
class oofRepMacGUIPreviewPP : public oofRepMacEnv {
public:
	oofRepMacGUIPreviewPP();
	virtual ~oofRepMacGUIPreviewPP();
	
	void finishConstruction(dbRepPICTs* PICTs, THPrint pageSetup=nil);
	virtual void changeTextStyle(oofRepTextStyle*);
	virtual unsigned short heightOfTextStyle(oofRepTextStyle*);
	virtual void 	drawPageBreak();
	//virtual void 	moveToVerticalPos(unsigned long); // see oofRepEnvironment comments
	virtual unsigned long calcPageHeight();
	virtual unsigned long calcPageWidth();
	virtual void startEnvironment();
	virtual void endEnvironment();

	Point		printableOffset();
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Handle getFlatSettings(); // caller owns the handle
	Handle getFlatFormat();   // caller owns handle
	Handle	getOldRecord();	// caller owns handle
#endif
	
	void parentCommander(LCommander*);
	LCommander* parentCommander() const;
	
	unsigned long previewPageOrdinal() const;  ///< returns ordinal number, NOT the printable page number which may reset
	
private:
	dbRepPICTs*		mPICTs;		///< Passed to us - owned by the window
	PicHandle		mCurPagePICT;	///< OWNED - Macintosh Handle
	Rect			mPaperRect;		///< Paper Size
	Point			mOffset;		///< Paper to printable area offset
	LCommander*		mCommander;	///< for subclasses that need to pass on to created windows
};



// -------------------------------------------------------
//     o o f R e p M a c G U I P r e v i e w P P
// -------------------------------------------------------

inline Point		
oofRepMacGUIPreviewPP::printableOffset() 	
{ 
	return mOffset; 
}


inline void		
oofRepMacGUIPreviewPP::parentCommander(LCommander* inCommander) 	
{ 
	mCommander = inCommander; 
}


inline LCommander* 		
oofRepMacGUIPreviewPP::parentCommander() const	
{ 
	return mCommander; 
}


#endif


