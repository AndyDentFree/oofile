#ifndef H_OOFREPWIN
#define H_OOFREPWIN
// ===========================================================================
//	oofRepPPWindow.h					
// ===========================================================================

#ifndef H_OOF4
	#include "oof4.h" // pick up fields, dbTable etc.
#endif
#ifndef H_OOFMACRO
	#include "oofmacro.h"
#endif
#include <PP_Macros.h>
#if !defined(__PowerPlant__) || __PowerPlant__	< 0x02118000
	#define OOF_PRECARBON_PP_PRINTING
#endif

#include <LWindow.h>
#include <LListener.h>
#ifdef OOF_PRECARBON_PP_PRINTING
	#ifndef _H_UPrintingMgr
		#include <UPrintingMgr.h>
	#endif
#else
	#ifndef _H_UPrinting
		#include <UPrinting.h>
	#endif
#endif
class 	LCommander;
class	oofRep;
class	LStream;
class 	CHandlePicture;
class	LStdButton;
class	LStdCheckBox;
class 	oofRepMacGUIPreviewPP;

/**
	dbTable used by oofRepMacGUIPreviewPP to store database of PICTs.
	Useful example of how to stash a Macintosh Pict data type (or any
	other binary handle) into a dbBLOB. 
	\ingroup oofRW
*/
class dbRepPICTs : public dbTable {
OOFILE_METHODS(dbRepPICTs)
	dbBLOB	PICT;
	
	dbRepPICTs ():
		PICT("Page Picture",kNotIndexed)
		{};
		
	void Add(PicHandle thePICT);
};


/**
	Preview window using the PowerPlant framework.
	\todo use Internet Config to read HTML browser and preferred TEXT editor for file types created by DoSaveAs.
	\ingroup oofRenderMac
*/
class	oofRepWindow : public LWindow,
					   public LListener {
public:
	enum { class_ID = 'RpWn', cmdRefresh = 'RepR' };

	
						oofRepWindow(LStream	*inStream);
	virtual				~oofRepWindow();
	static oofRepWindow* CreateFromStream(LStream*);
	
	virtual void		FinishCreateSelf();
	virtual Boolean		ObeyCommand(CommandT, void *ioParam = nil);
	virtual void		ListenToMessage(CommandT, void *ioParam = nil);
	virtual void		FindCommandStatus(CommandT,
							Boolean &outEnabled, Boolean &outUsesMark,
							UInt16 &outMark, Str255 outName);
protected:
	void				DoPageSetup();
	void				DoOpen();
	void				DoPrint();
	void				DoSaveAs();
	void				PrintAsPreviewed();
	void				CopyToScrap();
	void				UpdateMovementButtons();
	void				ReflowReport();

	dbRepPICTs			*PICTs() { return mPICTs; }

protected:
	virtual void CreatePreviewContent(oofRep* adoptedReport, THPrint inTHPrint=0);
	
	oofRep*					mRepPtr;	///> owned, report we're rendering adopted in CreatePreviewContent.
	dbRepPICTs*				mPICTs;	///> owned, database of PICTS for redrawing each page.
	oofRepMacGUIPreviewPP* 	mEnv;	///> owned, current rendering environment.

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	pascal Boolean navFilterProc(AEDesc* theItem, void* info, 
                            NavCallBackUserData callBackUD,
                            NavFilterModes filterMode);
	
	void 	setNavMenuItemSpec(NavMenuItemSpec& menSpecm, OSType menuCreat, OSType menuType, oofString name);
	void	doPopUpChoice(const FSSpec  dstFSSpec);

	
	NavMenuItemSpec mPopMenItem;
	OSType mMenType;
	OSType mMenCreat;
	ofstream* mOut;
#endif
#ifdef OOF_READ_REPORTS
	dbConnect*				mOpenedReportDB; ///> owned, temporary DB created when parse XML
#endif	
	// Powerplant objects
	CHandlePicture*	mPICTView;
	LStdButton*		mFirstButton;
	LStdButton*		mPrevButton;
	LStdButton*		mNextButton;
	LStdButton*		mLastButton;
	LStdCheckBox*	mPrintBWCheck;
	
public:
	static void RegisterClass();
	
	static oofRepWindow* CreateWindow(LCommander*, oofRep* adoptedReport, THPrint inTHPrint=0);
};

	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	pascal	void 	handleNavEvent(NavEventCallbackMessage callBackSelector, 
	                        NavCBRecPtr callBackParms, 
	                        NavCallBackUserData callBackUD);
	#endif
#endif
