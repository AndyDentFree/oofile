// ===========================================================================
//	oofGrWin.cp				
// ===========================================================================
//


#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFGRWIN
	#include "oofGrPP.h"
#endif

/**
	\def OOF_PRECARBON_PP_PRINTING
	Indicates using recent PowerPlant with UPrinting instead of UPrintingMgr.
	\ingroup oofMacros
*/

#include <PP_Macros.h>
#if !defined(__PowerPlant__) || __PowerPlant__	< 0x02118000
	#define OOF_PRECARBON_PP_PRINTING
#endif

#ifndef H_OOFPLOTPANE
	#include "CPlotPane.h"
#endif
#ifndef _H_LApplication
	#include <LApplication.h>
#endif
#ifndef _H_LGrowZone
	#include <LGrowZone.h>
#endif
#ifndef _H_UDrawingState
	#include <UDrawingState.h>
#endif
#ifndef _H_UMemoryMgr
	#include <UMemoryMgr.h>
#endif
#ifndef _H_URegistrar
	#include <URegistrar.h>
#endif
#ifndef _H_UDesktop
	#include <UDesktop.h>
#endif
#ifdef OOF_PRECARBON_PP_PRINTING
	#ifndef _H_UPrintingMgr
		#include <UPrintingMgr.h>
	#endif
#else
	#ifndef _H_UPrinting
		#include <UPrinting.h>	
	#endif
#endif
#ifndef _H_LStream
	#include <LStream.h>
#endif
#ifndef _H_LPrintout
	#include <LPrintout.h>
#endif
#ifndef _H_LPlaceHolder
	#include <LPlaceHolder.h>
#endif
#ifndef __SCRAP__
	#include <Scrap.h>
#endif
#ifndef _H_PP_Messages
	#include <PP_Messages.h>
#endif

#define PPob_WIND_Graph		29003
#define PPob_PANE_Plot		301

// ---------------------------------------------------------------------------
//		 oofGraphWindow
// ---------------------------------------------------------------------------

//	Register the functions to create our custom Pane classes
void
oofGraphWindow::RegisterClass()
{
	URegistrar::RegisterClass(oofGraphWindow::class_ID,  (ClassCreatorFunc)oofGraphWindow::CreateFromStream);
	URegistrar::RegisterClass(CPlotPane::class_ID,(ClassCreatorFunc)CPlotPane::CreatePlotPaneStream);

// standard PowerPlant classes
#if (__MWERKS__ >= 0x2000)
// Assume later compiler implies PowerPlant 1.8 or above
	RegisterClass_(LWindow);
	RegisterClass_(LPrintout);
	RegisterClass_(LPlaceHolder);
#else
// Older PowerPlant
	URegistrar::RegisterClass(LWindow::class_ID,  (ClassCreatorFunc)LWindow::CreateWindowStream);
	URegistrar::RegisterClass(LPrintout::class_ID, 	(ClassCreatorFunc)LPrintout::CreatePrintoutStream);
	URegistrar::RegisterClass(LPlaceHolder::class_ID, (ClassCreatorFunc)LPlaceHolder::CreatePlaceHolderStream);
#endif
}


//----------
//	The read-from-stream constructor just reads a window object.
oofGraphWindow::oofGraphWindow(
	LStream	*inStream)
		: LWindow(inStream)
{
}

// UReanimator factory
oofGraphWindow*
oofGraphWindow::CreateFromStream(LStream	*inStream)
{
	return new oofGraphWindow(inStream);
}


//	Destructor
oofGraphWindow::~oofGraphWindow()
{
	delete mGraphPtr;
}


//	Common Factory
oofGraphWindow*
oofGraphWindow::CreateWindow(
	LCommander* inSuperCommander, 
	oofGraph* inGraphPtr,
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	oofGraphWindow* displayWindow = ((oofGraphWindow *)LWindow::CreateWindow(PPob_WIND_Graph, inSuperCommander));
	LStr255 windowName(theTitle);
	displayWindow->SetDescriptor(windowName);
	displayWindow->mGraphPtr = inGraphPtr;

// MUST FocusDraw AND SetPortFont() to ensure an environment in which we can measure text
	displayWindow->FocusDraw();	
	//let's find our pane..
	CPlotPane *PlotView = (CPlotPane *) displayWindow->FindPaneByID(PPob_PANE_Plot);
	PlotView->SetPortFont();
		
	
	Rect theFrame;
	PlotView->CalcLocalFrameRect( theFrame );
	inGraphPtr->setPaneRect( theFrame );
	inGraphPtr->finishConstruction(theView, theTitle, AxisLength);
	oofDrawStyleSetDefaultIteration tempIter;
	inGraphPtr->setDrawStyles( tempIter  /*oofDrawStyleSetDefaultIteration()*/);
	
	PlotView->SetGraph(inGraphPtr);

	displayWindow->Show();
	return displayWindow;
}


//-- Printing Stuff--

#define	PPob_Prto_PictText	29004		//	Printout for printing the scrolling view
#define	PPob_place_PictText	911		//	Placeholder for the main body

void
oofGraphWindow::DoPrint()
{
	UDesktop::Deactivate();
#ifdef OOF_PRECARBON_PP_PRINTING
		Boolean wantToPrint = UPrintingMgr::AskPrintJob(UPrintingMgr::GetDefaultPrintRecord());
#else
	Boolean wantToPrint = UPrinting::AskPrintJob(UPrinting::GetAppPrintSpec());
#endif

	UDesktop::Activate();
	
	if (!wantToPrint)
		return;		// USER CANCELLED PRINT
		
	// set graph to mono
	mGraphPtr->setStyleToMono();

	//	Read the Printout resource and find the place
	//	holders for the scrolling view.

	LPrintout	*thePrintout = LPrintout::CreatePrintout(PPob_Prto_PictText);
	LPlaceHolder *thePlace	= (LPlaceHolder*) thePrintout->FindPaneByID(PPob_place_PictText);

	//	Now find the view in this window 
	//	and install it into its place holder.

	LView *theView	= (LView *) FindPaneByID(PPob_PANE_Plot);
	thePlace->InstallOccupant(theView, kAlignAbsoluteCenter);

		//	This is the function that starts the printing.

	thePrintout->DoPrintJob();

		//	The Printout's destructor takes care of setting the
		//	views back to where they came from.

	delete thePrintout;

	mGraphPtr->setStyleToColor();
}
//	Respond to commands

Boolean
oofGraphWindow::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean	cmdHandled = true;
	
	switch (inCommand) {
	
		// +++ Add cases here for the commands you handle
		//		Remember to add same cases to FindCommandStatus below
		//		to enable/disable the menu items for the commands
	
		case cmd_PageSetup:
			DoPageSetup();
			break;
			
		case cmd_Print:
			DoPrint();
			break;

		case cmd_Copy:
			CopyToScrap();
			break;

		default:
			cmdHandled = LWindow::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}


//	Pass back status of a (menu) command

void
oofGraphWindow::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {

		case cmd_Print:
		case cmd_PageSetup:
		case cmd_Copy:
			outEnabled = true;
			break;

		default:
			LWindow::FindCommandStatus(inCommand, outEnabled, outUsesMark,
								outMark, outName);
			break;
	}
}



// copied from LDocApplication
void
oofGraphWindow::DoPageSetup()
{
	UDesktop::Deactivate();
#ifdef OOF_PRECARBON_PP_PRINTING
	UPrintingMgr::AskPageSetup(UPrintingMgr::GetDefaultPrintRecord());
#else
	const Boolean wantToPrint = UPrinting::AskPageSetup(UPrinting::GetAppPrintSpec());
#endif
	UDesktop::Activate();
}


/**
	Put graph in clipboard as PICT.
	\todo confirm this works under Carbon, check PutScrapFlavor flag. 
*/
void
oofGraphWindow::CopyToScrap()
{
	FocusDraw();
	
	// Get our Frame Rectangle
	CPlotPane *PlotView = (CPlotPane *) FindPaneByID(PPob_PANE_Plot);
	Rect theFrame;
	PlotView->CalcLocalFrameRect(theFrame);

	ClipRect(&theFrame);
	PicHandle hPic = ::OpenPicture(&theFrame);
	mGraphPtr->draw();
	::ClosePicture();
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 

	OSStatus lErr = ::ClearCurrentScrap ();
	assert(!lErr);
	ScrapRef scrap;
	lErr = ::GetCurrentScrap (&scrap);
	assert(!lErr);
	::HLock((Handle)hPic);
	OSStatus pSErr = ::PutScrapFlavor (scrap, kScrapFlavorTypePicture, kScrapFlavorMaskTranslated ,GetHandleSize((Handle)hPic),*hPic);
	
#else
	
	OSErr lErr=::ZeroScrap();
	
	assert(!lErr);
	
	::HLock((Handle)hPic);
	::PutScrap(GetHandleSize((Handle)hPic),'PICT',*hPic); // Copy the PICT to the scrap !
#endif	
	::HUnlock((Handle)hPic);
	
//	DisposHandle((Handle)hPic);
}


// ---------------------------------------------------------------------------
//		 BarGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateBarGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofBarGraph, theView, theTitle, AxisLength);
}

	
// ---------------------------------------------------------------------------
//		 StackedBarGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateStackedBarGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofStackedBarGraph, theView, theTitle, AxisLength);
}

// ---------------------------------------------------------------------------
//		 StackedBarGraphWindowWithOverlay
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateStackedBarGraphWindowWithOverlay(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long NumOverlaid,
	unsigned long AxisLength)
{
	oofStackedBarGraph* newGraph = new oofStackedBarGraph;
	
	newGraph->setOverlaid(NumOverlaid);
	
	return CreateWindow(inSuperCommander, newGraph, theView, theTitle, AxisLength);
}

// ---------------------------------------------------------------------------
//		 StackedBarGraphWindowWithOverlayView
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateStackedBarGraphWindowWithOverlayView(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	dbView *overlayView,
	unsigned long AxisLength)
{
	oofStackedBarGraph* newGraph = new oofStackedBarGraph;
	
	newGraph->setOverlayView(overlayView);
	
	return CreateWindow(inSuperCommander, newGraph, theView, theTitle, AxisLength);
}
	
// ---------------------------------------------------------------------------
//		 OrdinalStackedBarGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateOrdinalStackedBarGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofOrdinalStackedBarGraph, theView, theTitle, AxisLength);
}

// ---------------------------------------------------------------------------
//		 ColumnGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateColumnGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofColumnGraph, theView, theTitle, AxisLength);
}

	
// ---------------------------------------------------------------------------
//		 PointGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreatePointGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofPointGraph, theView, theTitle, AxisLength);
}

	
// ---------------------------------------------------------------------------
//		 LineGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateLineGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofLineGraph, theView, theTitle, AxisLength);
}

	
// ---------------------------------------------------------------------------
//		 XYGraphWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreateXYGraphWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle, 
	unsigned long AxisLength)
{
	return CreateWindow(inSuperCommander, new oofXYGraph, theView, theTitle, AxisLength);
}

// ---------------------------------------------------------------------------
//		 PieChartWindow
// ---------------------------------------------------------------------------
oofGraphWindow*
oofGraphWindow::CreatePieChartWindow(LCommander* inSuperCommander, 
	dbView *theView, 
	const char* theTitle)
{
	return CreateWindow(inSuperCommander, new oofPieGraph, theView, theTitle);
}

	
