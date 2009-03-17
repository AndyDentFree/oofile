// ===========================================================================
//	oofRepPPWindow.cpp				
// COPYRIGHT 1998,2000,2001 A.D. Software, All rights reserved
// ===========================================================================
//

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPWIN
	#include "oofRepPPWindow.h"
#endif
#ifndef REPMACGUIPREVPP
	#include "oofRepPreviewPP.h"
#endif
#ifndef H_OOFREPHTML
	#include "oofrpHTM.h"
#endif
#ifndef H_OOFREPRTF
	#include "oofrpRTF.h"
#endif
#ifndef H_OOFREPCSV
	#include "oofrpCSV.h"
#endif
#ifndef H_OOFREPEDITX
	#include "oofrepeditx.h"
#endif
#ifndef H_OOFREPXML
	#include "oofrpXML.h"
#endif H_OOFREPXML
#ifndef OOFYNC_H
	#include "oofync.h"
#endif

// reloading reports is optional feature
#ifdef OOF_READ_REPORTS
	#ifndef H_OOFRIXML
		#include "oofriXML.h"
	#endif H_OOFREPXML
#endif

#ifndef H_OOFREPTSV
	#include "oofrpTSV.h"
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"		// for oofDirectory
#endif
#ifndef __STANDARDFILE__
	#include "StandardFile.h"	// for CustomPutFile()
#endif
#ifndef __DIALOGS__
	#include "Dialogs.h"		// for GetDialogItemText()
#endif
#ifndef _H_CHandlePicture
	#include "CHandlePicture.h"
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
#ifndef _H_LStream
	#include <LStream.h>
#endif
#ifndef _H_LStdControl
	#include <LStdControl.h>
#endif
#ifndef _H_LScroller
	#include <LScroller.h>
#endif
#ifndef _H_PP_Messages
	#include <PP_Messages.h>
#endif
#ifndef __SCRAP__
	#include <Scrap.h>
#endif
#ifndef _H_UReanimator
	#include <UReanimator.h> 
#endif

#if defined(PP_Target_Carbon) && PP_Target_Carbon==1
	#ifndef _H_UNavServicesDialogs
		#include <UNavServicesDialogs.h>
	#endif
#else
	#ifndef _H_UClassicDialogs
		#include <UClassicDialogs.h>
	#endif
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif


// Prototype

pascal short	RepSaveDlgHook(short item, DialogPtr theDialog, void *yourDataPtr);


// -------------------------------------------------------
//                    d b R e p P I C T s
// -------------------------------------------------------

void
dbRepPICTs::Add(PicHandle thePICT)
{
	newRecord();
	HLock((Handle)thePICT);
	PICT.setBytes((char *)*thePICT,GetHandleSize((Handle)thePICT));
	HUnlock((Handle)thePICT);
	saveRecord();
}


#define PPob_WIND_Rep		29010
#define PPob_PANE_PICT		29012
#define PPob_PANE_First		'frst'
#define PPob_PANE_Next		'next'
#define PPob_PANE_Prev		'prev'
#define PPob_PANE_Last		'last'
#define PPob_PANE_SaveTSV	29013
#define PPob_PANE_SaveCSV	29014
#define PPob_PANE_SaveHTML	29015
#define PPob_PANE_SaveRTF	29016
#define PPob_PANE_PrintBW	29017

// ---------------------------------------------------------------------------
//		¥ oofRepWindow
// ---------------------------------------------------------------------------

//	Register the functions to create our custom Pane classes
void
oofRepWindow::RegisterClass()
{
	URegistrar::RegisterClass(oofRepWindow::class_ID,  (ClassCreatorFunc)oofRepWindow::CreateFromStream);
	URegistrar::RegisterClass(CHandlePicture::class_ID,  (ClassCreatorFunc)CHandlePicture::CreatePictureStream);

// standard PowerPlant classes
#if (__MWERKS__ >= 0x2000)
// Assume later compiler implies PowerPlant 1.8 or above
	RegisterClass_(LScroller);
	RegisterClass_(LStdButton);
	RegisterClass_(LStdCheckBox);
#else
// old PowerPlant
	URegistrar::RegisterClass(LWindow::class_ID,  (ClassCreatorFunc)LWindow::CreateWindowStream);
	URegistrar::RegisterClass(LScroller::class_ID,  (ClassCreatorFunc)LScroller::CreateScrollerStream);
	URegistrar::RegisterClass(LStdButton::class_ID,  (ClassCreatorFunc)LStdButton::CreateStdButtonStream);
	URegistrar::RegisterClass(LStdCheckBox::class_ID,  (ClassCreatorFunc)LStdCheckBox::CreateStdCheckBoxStream);
#endif
}


//----------
//	The read-from-stream constructor just reads a window object.
oofRepWindow::oofRepWindow(LStream* inStream) :
	LWindow(inStream),
	mRepPtr(0),
	mPICTs(0),
	mEnv(0),
#ifdef OOF_READ_REPORTS
	mOpenedReportDB(0),	
#endif
	mPICTView(0),
	mFirstButton(0),
	mPrevButton(0),
	mNextButton(0),
	mLastButton(0),
	mPrintBWCheck(0)
{
}

// UReanimator factory
oofRepWindow*
oofRepWindow::CreateFromStream(LStream	*inStream)
{
	return new oofRepWindow(inStream);
}


//	Destructor
oofRepWindow::~oofRepWindow()
{
	const bool printInMono = mPrintBWCheck->GetValue();
	mRepPtr->defaultSettings()->printGraphsInMono(printInMono);	
	
	delete mRepPtr;
	delete mPICTs;
	delete mEnv;
#ifdef OOF_READ_REPORTS
	delete mOpenedReportDB;
#endif
}

void
oofRepWindow::FinishCreateSelf()
{
	// Get the PICT pane
	mPICTView = (CHandlePicture *) FindPaneByID(PPob_PANE_PICT);
	assert(mPICTView);
	
	// add a key scroller to use page up/down, pointing at the view to scroll
	LKeyScrollAttachment* adoptedKeyScroller = new LKeyScrollAttachment(mPICTView);
	AddAttachment(adoptedKeyScroller);  // report preview window is commander running attachment
	
	// Link the buttons to our listener
	mFirstButton = (LStdButton*) FindPaneByID(PPob_PANE_First);
	mPrevButton = (LStdButton*) FindPaneByID(PPob_PANE_Prev);
	mNextButton = (LStdButton*) FindPaneByID(PPob_PANE_Next);
	mLastButton = (LStdButton*) FindPaneByID(PPob_PANE_Last);
	UReanimator::LinkListenerToControls(this, this, PPob_WIND_Rep);	// RidL has same number


	mPrintBWCheck = (LStdCheckBox*) FindPaneByID(PPob_PANE_PrintBW);
}


void
oofRepWindow::UpdateMovementButtons()
{
	bool unableToMoveRight = true;
	bool unableToMoveLeft = true;
	
	if (mPICTs->count()>1) {  // more than one page, so may enable some!
		unableToMoveLeft = mPICTs->atFirst();
		unableToMoveRight = mPICTs->atLast();
	} // 2 or more pages in report

	if (unableToMoveLeft) {
		mFirstButton->Disable();
		mPrevButton->Disable();
	}
	else {
		mFirstButton->Enable();
		mPrevButton->Enable();
	}
	
	if (unableToMoveRight) {
		mLastButton->Disable();
		mNextButton->Disable();
	}
	else {
		mLastButton->Enable();
		mNextButton->Enable();
	}
}


//	Common Factory
oofRepWindow*
oofRepWindow::CreateWindow(
	LCommander* inSuperCommander,
	oofRep* adoptedReport,
	THPrint	inTHPrint
)
{
	oofRepWindow* displayWindow = ((oofRepWindow *)LWindow::CreateWindow(PPob_WIND_Rep, inSuperCommander));
	
	displayWindow->CreatePreviewContent(adoptedReport, inTHPrint);
	
	displayWindow->Show();
	return displayWindow;
}


void
oofRepWindow::CreatePreviewContent(
	oofRep* adoptedReport,
	THPrint	inTHPrint
)
{
	oofRepWindow* displayWindow = this;  // to easily copy code from static factory method
	
	if (adoptedReport->isMultiReport()) {  // see if one section and mutate
		oofRepMulti* asMulti = (oofRepMulti*)adoptedReport;  // safe downcast
		if (asMulti->count()==1) {  // if multi report contains just one report, mutate into single report
			oofRepSection* onlySection = asMulti->section(0);
			oofRep* newSingle = new oofRep(*onlySection);  // copy section to plain base oofRep
			delete adoptedReport;
			adoptedReport = newSingle;
		}  // only one section in multi
	} // is multi
	
	displayWindow->mRepPtr = adoptedReport;
	if (adoptedReport->settings()->printGraphsInMono())
		displayWindow->mPrintBWCheck->SetValue(1);
	else
		displayWindow->mPrintBWCheck->SetValue(0);
	
	if (adoptedReport->settings()->modalPreviewWindow())
		displayWindow->SetAttribute(windLayer_Modal);
	else
		displayWindow->SetAttribute(windAttr_Regular);
	
	// Open the RAM backend connection
	dbConnect_ram	RepDB;
	displayWindow->mPICTs = new dbRepPICTs;
	RepDB.newConnection();
	
	displayWindow->FocusDraw();
	
	oofRepEditor* possibleEditor = adoptedReport->settings()->reportEditor();
	if (possibleEditor) {
		oofRepEditingEnv* theEditEnv = possibleEditor->makeEditingEnvironment();;
		displayWindow->mEnv = theEditEnv;
		theEditEnv->parentCommander(this);
		theEditEnv->startMonitoringWindowInteractions(mPICTView);
	}
	else
		displayWindow->mEnv = new oofRepMacGUIPreviewPP;
	displayWindow->mEnv->finishConstruction(displayWindow->mPICTs,inTHPrint);
	// use a member variable rather than just temp as we need it in DoPrint
	
	adoptedReport->draw(*displayWindow->mEnv);
	
	// We should now have some pages !
	displayWindow->mPICTs->start();
	if(displayWindow->mPICTs->count()) // Well, we might not - if it's a stupid report !!
		displayWindow->mPICTView->SetPicture(displayWindow->mPICTs->PICT.bodyAddress(),
											 displayWindow->mPICTs->PICT.length());
	displayWindow->UpdateMovementButtons();
}


/**
	open an XML file and parse it, replacing the current report.
	Uses OOF_XMLrepParser to parse the file.
	
	In Carbon builds has a copy of UNavServicesDialogs::AskOpenOneFile which has
	been enhanced to allow us to set a signature for opening files created by any app.
	
	\todo check if crash with UConditionalDialogs still occurs
*/
void
oofRepWindow::DoOpen()
{
#ifdef OOF_READ_REPORTS
// conditional compilation to avoid linking stuff we don't have
// such as expatpp XML parser

	oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	const OSType reportType = XMLsettings->saveReportDocMacFileType();
	
	FSSpec outFileSpec;
	#if defined(PP_Target_Carbon) && PP_Target_Carbon==1
//		const bool openedFile = UNavServicesDialogs::AskOpenOneFile(reportType,outFileSpec);

// more or less insert the original method
//bool UNavServicesDialogs::AskOpenOneFile(
	OSType inFileType = reportType;
//	FSSpec&					outFileSpec,
	NavDialogOptionFlags	inFlags =- kNavDefaultNavDlogOptions;
	bool askOK;
		{
			LFileTypeList	fileTypes(inFileType); // ctor sets the creator from process signature
			fileTypes.SetSignature(kNavGenericSignature);  // see Navigation.h - override process sig with '****'

			UNavServicesDialogs::LFileChooser	chooser;

			inFlags &= ~kNavAllowMultipleFiles;		// Can't select multiple files
			NavDialogOptions*	options = chooser.GetDialogOptions();
			options->dialogOptionFlags = inFlags;

			askOK = chooser.AskOpenFile(fileTypes);

			if (askOK) {
				chooser.GetFileSpec(1, outFileSpec);
			}

//			return askOK;
		}
		const bool openedFile = askOK;  // mimic return from method



	
	#else
// had crash on OS8.5.1 with SampleReports, so leave for now
// with just classic, rather than using UConditionalDialogs for
// runtime decision between NavServices and Classic
		const bool openedFile = UClassicDialogs::AskOpenOneFile(reportType,outFileSpec);
	#endif
	
	if (openedFile) {
		delete mOpenedReportDB;
		mOpenedReportDB = XMLsettings->makeTempConnection();
		// if you wanted it RAM based you could just use a dbConnect_ram
		

		OOF_XMLrepParser theParser;
		const oofFileRef theFile = outFileSpec; // Mac-specific file handling logic passing in outFileSpec
		if (theParser.parseFile( theFile) ) {
			delete mRepPtr;
			mRepPtr = theParser.orphanReport();
			ReflowReport();
		}
		else
			dbConnect::raise(oofString("Sorry, '", theFile.filename(), "' did not seem to be a report file"), false /* don't terminate*/);
	}  // if choose a file
#endif
}



/**
	Reflow report after page setup, opening or editing.
	Calculates page layout again from scratch.
	\todo show picture on blank reports.
*/void
oofRepWindow::ReflowReport()
{
	const unsigned long savePreviewPage = mEnv->previewPageOrdinal();
	
	// Chuck everything away and remake the report
	mPICTs->deleteAll();  // don't delete mPICTS as environment has copy
		
	// hang onto this as our DoPrint needs to call its methods
	mRepPtr->draw(*mEnv);	
	
	// We should now have some pages !
	const unsigned long numPreviewPages = mPICTs->count();
	
	if(numPreviewPages>0) {// Well, we might not - if it's a stupid report !!
		if (savePreviewPage > (numPreviewPages-1))
			mPICTs->last();
		else
			mPICTs->gotoRecord(savePreviewPage);
			
		mPICTView->SetPicture(mPICTs->PICT.bodyAddress(), mPICTs->PICT.length());
	}
	else {
// 	NOT YET IMPLEMENTED - need a picture for empty report
	}
	
	UpdateMovementButtons();
}


void
oofRepWindow::DoPrint()
{
	//StGrafPortSaver shieldFromPrOpen;
	
	if (mPrintBWCheck->GetValue()) {
		mRepPtr->defaultSettings()->printGraphsInMono(true);	// change globally for this process (not persistent)
		if (!mRepPtr->settings()->printGraphsInMono())	
			mRepPtr->getLocalSettings()->printGraphsInMono(true);	// change local setting if the above is overriden

#ifdef OOF_PRECARBON_PP_PRINTING
		THPrint thePrintHandle = UPrintingMgr::GetDefaultPrintRecord();
#else
	LPrintSpec theSpec = UPrinting::GetAppPrintSpec();
	THPrint thePrintHandle =  (THPrint)theSpec.GetPrintRecord();
#endif

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
			
		oofMacCarbonPrint tempEnv(thePrintHandle);
		mRepPtr->draw(tempEnv); // will print in B&W
	
#else
		::PrOpen();
			assert(!::PrError());
			bool willPrint = ::PrJobDialog(thePrintHandle);
		::PrClose();	// report does its own opening and closing
		
		if(willPrint) {
			oofRepMacPrint tempEnv(thePrintHandle);
			mRepPtr->draw(tempEnv); // will print in B&W
		}
	#ifndef OOF_PRECARBON_PP_PRINTING
		::DisposeHandle ((Handle)thePrintHandle);
	#endif
#endif
	}
	else {
		PrintAsPreviewed();	// just print the colour picts
		mRepPtr->defaultSettings()->printGraphsInMono(false);	// a way to make the default Print buttons print colour
		if (mRepPtr->settings()->printGraphsInMono())	
			mRepPtr->getLocalSettings()->printGraphsInMono(false);	// change local setting if the above is overriden
	}
}


void
oofRepWindow::PrintAsPreviewed()
{
	//::PrOpen();
	//assert(!::PrError());	// KENTODO - Handle errors
	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		PMPageFormat pageForm;
		PMPrintSettings	pageSettings;
		PMPrintContext thisReportPrintout;
		OSStatus begErr = 	::PMBegin();
		assert(!begErr);// TODO - Handle errors
	#else
		::PrOpen();
		assert(!::PrError());
	#endif
	
	THPrint myTHPrint = mEnv->pageSetup();
	Point   myOffset  = mEnv->printableOffset();
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	OSStatus err =::PMConvertOldPrintRecord ((Handle)myTHPrint,&pageSettings,&pageForm);
	Boolean	willPrint;
	OSStatus diaErr = PMPrintDialog (pageSettings,pageForm, &willPrint);
#else
	
	////->>>
	::PrValidate(myTHPrint);
// don't seem needed	UDesktop::Deactivate();
	const bool willPrint = ::PrJobDialog(myTHPrint);
//	UDesktop::Activate();
#endif	
	if (willPrint) {
		Rect 		pageRect;
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		OSStatus DocErr;
		DocErr = ::PMBeginDocument(pageSettings, pageForm,&thisReportPrintout);
#else
		TPPrPort	myPrtPort;
		TPrStatus	myPrStatus;
		myPrtPort = PrOpenDoc(myTHPrint, 0,0); //let system allocate port and IO buf
#endif
		for (mPICTs->start(); mPICTs->more(); mPICTs->next()) {		// print all pages
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
			OSStatus fpageErr = ::PMBeginPage (thisReportPrintout,0);
#else			
			PrOpenPage(myPrtPort, 0);
			assert(!::PrError());	// KENTODO - Handle errors
#endif
			PicHandle thePicture=(PicHandle)::NewHandle(mPICTs->PICT.length());
			assert(thePicture);
			::HLock((Handle)thePicture);
			::BlockMove(mPICTs->PICT.bodyAddress(),*thePicture,mPICTs->PICT.length());
			pageRect=(*thePicture)->picFrame;
			::OffsetRect(&pageRect,-myOffset.h,-myOffset.v);
			::HUnlock((Handle)thePicture);
			::DrawPicture(thePicture,&pageRect);
			::DisposeHandle((Handle)thePicture);
		
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
			
			fpageErr = ::PMEndPage(thisReportPrintout);
		}
			OSStatus docErr = ::PMEndDocument (thisReportPrintout);
#else			
			assert(!::PrError());	// KENTODO - Handle errors
			::PrClosePage(myPrtPort);
		}
		::PrCloseDoc(myPrtPort);
		if ((*myTHPrint)->prJob.bJDocLoop==bSpoolLoop) {
			::PrPicFile(myTHPrint,0,0,0,&myPrStatus);
			assert(!::PrError());	// KENTODO - Handle errors
		}
#endif	
	}
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	::OSStatus endErr = ::PMEnd();
#else	
	::PrClose();
#endif
}

//	Respond to commands

Boolean
oofRepWindow::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean	cmdHandled = true;
	
	switch (inCommand) {
	
		// +++ Add cases here for the commands you handle
		//		Remember to add same cases to FindCommandStatus below
		//		to enable/disable the menu items for the commands
	
		case cmd_Open:  // only enabled if defined OOF_READ_REPORTS
			DoOpen();
			break;
	
		case cmd_SaveAs:
			DoSaveAs();  // we intercept, NOT relying on the Document-level handling
			break;
	
		case cmd_PageSetup:
			DoPageSetup();
			break;
			
		case cmd_Print:
			DoPrint();
			break;

		case cmd_Copy:
			CopyToScrap();
			break;

		case cmdRefresh:
			ReflowReport();
			break;

		default:
			cmdHandled = LWindow::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}

void
oofRepWindow::ListenToMessage(
	CommandT	inCommand,
	void		* /* ioParam */)
{
	bool moved = false;
	switch(inCommand) {

// literals from oofpp.h copied here as may sell reports separately
	case 'dFst': 
		mPICTs->first();
		moved = true;
		break;
					 
	case 'dPrv': 
		mPICTs->prev();
		moved = true;
		break;
					 
	case 'dNxt': 
		mPICTs->next();
		moved = true;
		break;
		
	case 'dLst': 
		mPICTs->last();
		moved = true;
		break;
		
	// Print button
	case 'prnT':
		DoPrint();
		break;
	
	//  Save as...
	case 'savE':
		DoSaveAs();
		break;

	//  Close
	case 'cloS':
		AttemptClose();
		break;
						 
	default:	 
		break;
	}
	if (moved) {	// adjust buttons
		mPICTView->SetPicture(mPICTs->PICT.bodyAddress(), mPICTs->PICT.length());
		UpdateMovementButtons();		
	} // movement button pressed
}
//	Pass back status of a (menu) command


void
oofRepWindow::FindCommandStatus(
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
		case cmd_SaveAs:
			outEnabled = true;
			break;

#ifdef OOF_READ_REPORTS
		case cmd_Open:
			outEnabled = true;
			break;
#endif
		default:
			LWindow::FindCommandStatus(inCommand, outEnabled, outUsesMark,
								outMark, outName);
			break;
	}
}


void
oofRepWindow::DoPageSetup()
{
	
	THPrint myTHPrint = mEnv->pageSetup();//
	const size_t myPrintRecSize = ::GetHandleSize((Handle)myTHPrint);
	
	const size_t printRecSize = sizeof(TPrint);
	assert(printRecSize==myPrintRecSize);  // because we're copying in BlockMove below
	
	THPrint myPrintRecordH = (THPrint) NewHandle(printRecSize);
	assert(myPrintRecordH);
	

	::HLock((Handle)myPrintRecordH);
	::HLock((Handle)myTHPrint);
	::BlockMove(*myTHPrint, *myPrintRecordH, printRecSize);
	::HUnlock((Handle)myPrintRecordH);
	::HUnlock((Handle)myTHPrint);
	
#ifdef OOF_PRECARBON_PP_PRINTING
	if(UPrintingMgr::AskPageSetup(myPrintRecordH)){
		mEnv->pageSetup(myPrintRecordH);
		ReflowReport();
	}
#else
	LPrintSpec myPrintSpec;
	myPrintSpec.SetPrintRecord((char**)myPrintRecordH);
	StPrintSession			Session(myPrintSpec);
	if(UPrinting::AskPageSetup(myPrintSpec)) {
		myPrintRecordH = (THPrint)myPrintSpec.GetPrintRecord();
		mEnv->pageSetup(myPrintRecordH);
		ReflowReport();
	}
#endif
	::DisposeHandle((Handle)myPrintRecordH);
}

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
/**
	Display save file dialog under carbon.

	We have three sets of flags here. The file creator is set
	depending on the default application we want to open files with, 
	eg: MSIE for HTML files, BBEdit for plain text and XML.

	The menuType is 'XML ' for XML files (ie: report documents) but
	the file type created for them is specified in our settings
	XMLsettings->saveReportDocMacFileType().
	
	Similarly for 'HTML' as a menuType, to drive using the HTML
	rending engine, creating a file of type 'TEXT' creator 'MSIE'.
	
	\todo Cleanup this code and explain how it works - AD 2001-12-20
*/
void
oofRepWindow::DoSaveAs()
{
  	
  	OSErr               anErr = noErr;
    NavReplyRecord      reply;
    NavDialogOptions    dialogOptions;
    
   OSType              fileTypeToSave = 'TEXT';  
   OSType              creatorType = 'R*ch';
 
  	mPopMenItem.menuType = 'XML ';
	mPopMenItem.menuCreator = creatorType;

	NavEventUPP         eventProc = ::NewNavEventUPP(handleNavEvent);
	
	anErr = NavGetDefaultDialogOptions(&dialogOptions);
	if (anErr == noErr){
		dialogOptions.dialogOptionFlags ^= kNavAllowStationery;  /* No stationery files */
		 dialogOptions.dialogOptionFlags ^=  kNavAllFilesInPopup ;/* bo idea*/
		 dialogOptions.dialogOptionFlags |= kNavAllowPreviews;	/*No Previews */
		oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	
		const oofString saveName("untitled.", XMLsettings->saveReportDocFileExt());
		saveName.getStr255(dialogOptions.savedFileName);
		dialogOptions.popupExtension = (NavMenuItemSpec**)::NewHandle(sizeof(NavMenuItemSpec)*5) ;
		
		::HLock((char**)dialogOptions.popupExtension);
		NavMenuItemSpec*  myMenu = *dialogOptions.popupExtension;
		
		// 2nd param is selector for doPopUpChoice, must be unique
		setNavMenuItemSpec(myMenu[0], 'R*ch', 'XML ', "Report Document (XML)");
		setNavMenuItemSpec(myMenu[1], 'MSWD', 'RTF ', "Rich Text Format (RTF)");
		setNavMenuItemSpec(myMenu[2], 'MSIE', 'HTML', "Web Page (HTML)");
		setNavMenuItemSpec(myMenu[3], 'R*ch', 'CSV ', "Comma Separated (CSV)");	
		setNavMenuItemSpec(myMenu[4], 'R*ch', 'TSV ', "Tab Separated (TSV)");/**/
		::HUnlock((char**)dialogOptions.popupExtension);

   		anErr = NavPutFile( (AEDesc *)NULL, &reply, &dialogOptions, eventProc, fileTypeToSave, kNavGenericSignature,&mPopMenItem );
		  	if (anErr == noErr && reply.validRecord){
				AEKeyword   theKeyword;
				DescType    actualType;
				Size        actualSize;
		        FSSpec      dstFSSpec;
						
				anErr = AEGetNthPtr(&(reply.selection), 1, typeFSS,
	                                &theKeyword, &actualType,
	                                &dstFSSpec, sizeof(dstFSSpec),
	                                &actualSize );
				if (anErr == noErr){
					stSaveDirectory saveDir;
					oofDirectory reportDir(dstFSSpec);
					reportDir.gotoDirectory();
					// convert Mac name from Pascal string to C string			
					char fileName[63];
					size_t nameLen = dstFSSpec.name[0];
					memcpy(fileName, &dstFSSpec.name[1], nameLen);
					fileName[nameLen] = '\0';
					mOut = new ofstream(fileName);
					assert(mOut->good());
					doPopUpChoice(dstFSSpec);
				}
				(void) NavDisposeReply(&reply);
			}
   
		}


	DisposeNavEventUPP (eventProc);



	NavEventUPP NewNavEventUPP (
    NavEventProcPtr userRoutine
);
}//dosaveas

#else
void
oofRepWindow::DoSaveAs()
{
	// Uses resource template in file: RepWindow.PPob
	#define kSaveAsTemplate 601
	
	// Open a file to write the output to...
	
	ofstream*			lOut;		// output stream to save data into
	StandardFileReply 	lReply;		// find out if the user canceled the save or not
	Point				lPoint;		// upper-left corner of box
	
	lPoint.h = -1;		// Centre the dialog
	lPoint.v = -1;

	UDesktop::Deactivate(); 	// PowerPlant 'out' to let us call standard file
	
	int		choice = 1;		// passed into the putfile dialog to find out which format is desired
							// Initialised to 1 as the popup starts with the top most value selected
							
	oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	
	const oofString saveName("untitled.", XMLsettings->saveReportDocFileExt());
	Str255 saveNameP;
	saveName.getStr255(saveNameP);
	CustomPutFile(	"\pSave as...",
					saveNameP, 
					&lReply, 
					kSaveAsTemplate,			// Dialog resource ID
					lPoint,			// Where to place the dialog on the screen
					NewDlgHookYDProc( RepSaveDlgHook ), 
					NULL,			// Modal Filter Hook
					NULL,			// Activation Order List Hook
					NULL,			// Activate Hook
					&choice);		// Pointer to my data

	UDesktop::Activate(); 	// PowerPlant 'in' to let us call PP again

	if (lReply.sfGood) {  // user clicked Save button

		oofDirectory reportDir(lReply.sfFile);
		reportDir.gotoDirectory();

// convert Mac name from Pascal string to C string			
		oofString fileName = lReply.sfFile.name;
		lOut = new ofstream(fileName);
		assert( lOut->good() );
		
		// The order in the file format popup list is: XML, RTF, HTML, CSV, TSV
		// NOT YET IMPLEMENTED - check the string that the user entered so we match Windows behaviour
		// allowing user to type 'blah.rtf' without having to choose from the popup
		
		switch( choice ) {
			case 1: {
					oofRepXMLEnv* XMLenv = XMLsettings->makeXMLenv(*lOut);
					mRepPtr->draw(*XMLenv);
					delete XMLenv;  
				}
				break;
			case 2: {
					oofRepRTFEnv tempEnv(*lOut);
					mRepPtr->draw(tempEnv);
				}
				break;
			case 3: {
					oofRepHTMLEnv tempEnv(*lOut);
					mRepPtr->draw(tempEnv);
				}
				break;
			case 4:
				{
					oofRepCSVEnv tempEnv(*lOut);
					mRepPtr->draw(tempEnv);
				}
				break;
			case 5: {
					oofRepTSVEnv tempEnv(*lOut);
					mRepPtr->draw(tempEnv);
				}
				break;
		}
		lOut->close();
		delete lOut;

		oofFileRef repFile(lReply.sfFile);
		switch( choice ) {
		case 1: 
			repFile.setFileType(XMLsettings->saveReportDocMacFileType());
			break;
		case 2: 
			repFile.setFileType('RTF ', 'MSWD');
			break;
		case 3: 
			repFile.setFileType('TEXT', 'MSIE');  // HTML - Internet Explorer
			break;
		case 4: 
		case 5: 
			repFile.setFileType('TEXT', 'R*ch');  // TEXT - BBEdit, OS suggests TeachText if not around
			break;
		}
	}
}

#endif
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
/**
	Checks file type is TEXT, unknown or user-defined report file type.
*/
pascal Boolean 
oofRepWindow::navFilterProc(AEDesc* theItem, void* info, 
                            NavCallBackUserData callBackUD,
                            NavFilterModes filterMode)
{
    OSErr theErr = noErr;
    Boolean willDisplay = true;
    NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*)info;
    
    if (theItem->descriptorType == typeFSS)
        if (!theInfo->isFolder) {
           OSType fileType = theInfo->fileAndFolder.fileInfo.finderInfo.fdType; 
       			 
            if ((fileType != 'TEXT') && (fileType != '????')) {
				oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
            	willDisplay = (fileType == XMLsettings->saveReportDocMacFileType());
			}
    	}
    return willDisplay;
}



void
oofRepWindow::setNavMenuItemSpec(NavMenuItemSpec& menSpecm, OSType menuCreat, OSType menuType, oofString name)
{
	
	menSpecm.version = kNavMenuItemSpecVersion;
	menSpecm.menuCreator = menuCreat;
	menSpecm.menuType =menuType ;
	name.getStr255(menSpecm.menuItemName);
};


void
oofRepWindow::doPopUpChoice(const FSSpec  dstFSSpec)
{	

	oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
	const oofString fileExt = XMLsettings->saveReportDocFileExt();
	
	switch(mPopMenItem.menuType ) {
	
				case 'XML ': {
						oofRepXMLEnv* XMLenv = XMLsettings->makeXMLenv(*mOut);
						mRepPtr->draw(*XMLenv);
						delete XMLenv;  
					}
					break;
				case 'RTF ': {
						oofRepRTFEnv tempEnv(*mOut);
						mRepPtr->draw(tempEnv);
					}
					break;
				case 'HTML': {
						oofRepHTMLEnv tempEnv(*mOut);
						mRepPtr->draw(tempEnv);
					}
					break;
				case 'CSV ':
					{
						oofRepCSVEnv tempEnv(*mOut);
						mRepPtr->draw(tempEnv);
					}
					break;
				case 'TSV ': {
						oofRepTSVEnv tempEnv(*mOut);
						mRepPtr->draw(tempEnv);
					}
					break;
			}
			mOut->close();
			delete mOut;

			oofFileRef repFile(dstFSSpec);
			switch( mPopMenItem.menuType  ) {
			case 0: 
				repFile.setFileType(XMLsettings->saveReportDocMacFileType());
				break;
			case 'RTF ': 
				repFile.setFileType('RTF ', 'MSWD');
				break;
			case 'TEXT': 
				repFile.setFileType('TEXT', 'MSIE');  // HTML - Internet Explorer
				break;
			case 'CSV ': 
			case 'TSV ': 
				repFile.setFileType('TEXT', 'R*ch');  // TEXT - BBEdit, OS suggests TeachText if not around
				break;
			}
}	
#endif
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 

void
oofRepWindow::CopyToScrap()
{
	OSStatus lErr = ::ClearCurrentScrap ();
	assert(!lErr);
	ScrapRef scrap;
	lErr = ::GetCurrentScrap (&scrap);
	assert(!lErr);
	::HLock((Handle)mPICTView->GetPictureH());
	OSStatus pSErr = ::PutScrapFlavor (scrap, kScrapFlavorTypePicture, kScrapFlavorMaskTranslated ,GetHandleSize((Handle)mPICTView->GetPictureH()),*(Handle)mPICTView->GetPictureH());

	::HUnlock((Handle)mPICTView->GetPictureH());
}

#else
void
oofRepWindow::CopyToScrap()
{
	OSErr lErr=ZeroScrap();
	
	assert(!lErr);
	
	HLock((Handle)mPICTView->GetPictureH());
	PutScrap(GetHandleSize((Handle)mPICTView->GetPictureH()),'PICT',*mPICTView->GetPictureH()); // Copy the PICT to the scrap !
	HUnlock((Handle)mPICTView->GetPictureH());
}

#endif


// For 'Save As' dialog template in file: RepWindow.PPob

#define		kSaveAsPopupID	14

pascal short	RepSaveDlgHook(short item, DialogPtr theDialog, void *yourDataPtr)
{
	short		myType;
	Handle		popup_handle;
	Handle		filename_handle;
	Rect		myRect;
	short		file_format;
	short		dlgReturn;
	Str255		filename;

	switch( item ) {
		
		case kSaveAsPopupID:              // user selected File Type pop-up menu
         	GetDialogItem( theDialog, kSaveAsPopupID, &myType, &popup_handle, &myRect );
         	file_format = GetControlValue( ControlRef( popup_handle ) );
         	
         	if( file_format != *((int *)yourDataPtr) ) {
				oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
				const oofString fileExt = XMLsettings->saveReportDocFileExt();
				assert(fileExt.length()==3);  // I know this sucks but it protects the creaky logic below

				// change the filename to reflect the new format
				GetDialogItem( theDialog, sfItemFileNameTextEdit, &myType, &filename_handle, &myRect );
				GetDialogItemText( filename_handle,filename );
								
				// convert the file extension to represent the new format
			
				// check for 4 letter extensions (html)
				if( filename[0] >= 5 )		// only check when there's enough characters
					if( filename[ filename[0] - 4 ] == '.' )
						filename[0] -= 1;	// so as not to end up with "blah..rtf" from "blah.html"
						
				// check for 3 letter extensions (rtf,txt,csv, xml)
				if( filename[0] >= 4 && filename[0] < (255-4) )
					if( filename[ filename[0] - 3 ] != '.' )
						filename[0] += 4;	// if no extension, add some space for one
				
				// if there are not enough characters to make a file extension, let's add some
				if( filename[0] <= 4 )
					filename[0] += 4;
						
				switch( file_format ) {
// SMART-ASS JUNIOR PROGRAMMER LOGIC BELOW
// may be reasonably efficient but not intuitive, but keep to the pattern as I'm in a hurry
// and don't want to spend extra time walking through a rewritten version				
					case 1:					// XML or user-overriden format
						filename[ filename[0] ] = fileExt[2];
						filename[ filename[0]-1] = fileExt[1];
						filename[ filename[0]-2] = fileExt[0];
						filename[ filename[0]-3] = '.';
						SetDialogItemText( filename_handle, filename );
						break;
					case 2:					// RTF
						filename[ filename[0] ] = 'f';
						filename[ filename[0]-1] = 't';
						filename[ filename[0]-2] = 'r';
						filename[ filename[0]-3] = '.';
						SetDialogItemText( filename_handle, filename );
						break;
					case 3:					// HTML
						filename[ filename[0] ] = 'm';
						filename[ filename[0]-1] = 't';
						filename[ filename[0]-2] = 'h';
						filename[ filename[0]-3] = '.';
						SetDialogItemText( filename_handle, filename );
						break;
					case 4:					// CSV
						filename[ filename[0] ] = 'v';
						filename[ filename[0]-1] = 's';
						filename[ filename[0]-2] = 'c';
						filename[ filename[0]-3] = '.';
						SetDialogItemText( filename_handle, filename );
						break;
					case 5:					// TXT
						filename[ filename[0] ] = 't';
						filename[ filename[0]-1] = 'x';
						filename[ filename[0]-2] = 't';
						filename[ filename[0]-3] = '.';
						SetDialogItemText( filename_handle, filename );
						break;
				}
				
         		// remember file format user has chosen
         		*((int *)yourDataPtr) = file_format;
         		
         		dlgReturn = sfHookRebuildList;
         	}
        	break;
        
        default:
        	dlgReturn = item;	// by default, return the item passed in
        	break;
	}

	return( dlgReturn );
}

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
pascal void 
handleNavEvent(NavEventCallbackMessage callBackSelector, 
                        NavCBRecPtr callBackParms, 
                        NavCallBackUserData callBackUD)
{
   // WindowPtr window = 
     //               (WindowPtr)callBackParms->eventData.eventDataParms.event->message;
    switch (callBackSelector)
    {
        case kNavCBEvent:
            switch (((callBackParms->eventData)
                    .eventDataParms).event->what)
            {
                case updateEvt:
                    WindowPtr window = 
                    (WindowPtr)callBackParms->eventData.eventDataParms.event->message;
  
                    //MyHandleUpdateEvent(window, 
                      //  (EventRecord*)callBackParms->eventData.event);
                    //not much for now
                    break;
            }
            break;
   		 case kNavCBPopupMenuSelect:
   	 		NavMenuItemSpec* theMItem =  	(NavMenuItemSpec*)callBackUD;
   	 		NavMenuItemSpec* callBackItem = (NavMenuItemSpec*)callBackParms->eventData.eventDataParms.param;
   	 		
   	 		
   	 		
   	 		theMItem->menuType = callBackItem->menuType ; 
 			theMItem->menuCreator = callBackItem->menuCreator;
			switch(callBackItem->menuType){
				case 'XML ': {
					CFStringRef xmlStr = CFSTR("untitled.xml"); 
					OSErr serErr =  NavDialogSetSaveFileName(callBackParms->context,xmlStr);
				}
					break;
				case 'RTF ': {
					CFStringRef rtfStr = CFSTR("untitled.rtf"); 
					OSErr serErr =  NavDialogSetSaveFileName(callBackParms->context,rtfStr);
				}
						break;
				case 'TEXT': {
					CFStringRef txtStr = CFSTR("untitled.text"); 
					OSErr serErr =  NavDialogSetSaveFileName(callBackParms->context,txtStr);
				}
					break;
				case 'CSV ':
					{
						CFStringRef csvStr = CFSTR("untitled.csv"); 
						OSErr serErr =  NavDialogSetSaveFileName(callBackParms->context,csvStr);
				}
					break;
				case 'TSV ': {
					CFStringRef tsvStr = CFSTR("untitled.tsv"); 
					OSErr serErr =  NavDialogSetSaveFileName(callBackParms->context,tsvStr);
				}
					break;
			}
			break;
			
			}
		
 }
#endif
//}



