// COPYRIGHT 1995 A.D. Software, All rights reserved

// PowerPlant Integration layer of OOFILE database

#pragma fullpath_prepdump on

#include "oofync.h"
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef _H_UModalDialogs
	#include <UModalDialogs.h>
#endif
#ifndef _H_UReanimator
	#include <UReanimator.h>
#endif
#ifndef _H_URegistrar
	#include <URegistrar.h>
#endif
#ifndef _H_LStream
	#include <LStream.h>
#endif
#ifndef _H_UAttachments
	#include <UAttachments.h>
#endif
#ifndef _H_PP_Messages
	#include <PP_Messages.h>
#endif
#if __PowerPlant__	< 0x02108000  // v2.1 prior to Pro 5.3
	#ifndef _H_LGADialog
		#include <LGADialog.h>
	#endif
#else
	#ifndef _H_LDialogBox
		#include <LDialogBox.h>
	#endif
#endif // old PP
#ifndef _H_PP_Types
	#include <PP_Types.h>
#endif
#include <LPushButton.h>
#include <LAMPushButtonImp.h>
#include <LGAPushButtonImp.h>
#include <LStaticText.h>
#include <LAMStaticTextImp.h>
#include <LGAStaticTextImp.h>
#include <LPicture.h>
#include <LAMControlImp.h>
#include <LGAPictureControlImp.h>

#ifndef __SOUND__
	#include <Sound.h>  // for SysBeep
#endif
#include <assert.h>

// define the window class used to display the messages


const CommandT	cmdBtn1Click	= 'btn1';
const CommandT	cmdBtn2Click	= 'btn2';
const CommandT	cmdBtn3Click	= 'btn3';

	#define PPob_YNCDlgID	29001
	#define RidL_YNCDlgID	29001


#if __PowerPlant__	< 0x02108000  
	#define PARENT_DIALOGBOXCLASS LGADialog
#else
	#define PARENT_DIALOGBOXCLASS LDialogBox
#endif // old PP


/**
	PowerPlant dialog class for equivalent to Windows MessageBox.
	Displays dialog with up to 3 buttons and icon.
*/
class	CYNCDlg :	public PARENT_DIALOGBOXCLASS {
public:	// these comprise the programming interface for using the dialog
	static	CYNCDlg*		CreateYNCDlg		(LCommander		*inSuperCommander);

public:	// these comprise the implementation
	static	CYNCDlg*	CreateYNCDlgStream	(LStream	*inStream);

						CYNCDlg();
						CYNCDlg		(LStream	*inStream);
	virtual				~CYNCDlg();

	//virtual void		ListenToMessage		(MessageT	inMessage,
	//											 void		*ioParam);

	virtual Boolean		ObeyCommand			(CommandT	inCommand,
											 void		*ioParam = nil);
	virtual void		FindCommandStatus	(CommandT	inCommand,
											 Boolean	&outEnabled,
											 Boolean	&outUsesMark,
											 UInt16		&outMark,
											 Str255		outName);
	virtual Boolean		FocusDraw(LPane* inSubPane=nil);

	// manual - moved this to public scope area
	static	void		RegisterClass();
	// end man
	
	// manual - setters for dialog members
	virtual void		SetMessageText(const char*);
	virtual void		HideButton(int);
	virtual void		SetButtonCaption(int, const char*);
	virtual void		SetTitle(const char*);
	virtual void		SetDefaultBtn(int);
	virtual void		SetCancelBtn(int);
	virtual void		SetIcon(ResIDT inIconRes);
	// end man
	
protected:
	virtual void		FinishCreateSelf();

protected:

protected:
	static Boolean		sIsRegistered;

	LPushButton*		mButton1;
	LPushButton*		mButton2;
	LPushButton*		mButton3;
	
	LStaticText*		mText;
	LPicture*	mIcon;
	
	};



Boolean		CYNCDlg::sIsRegistered = false;

//----------
#define	RegisterClasses(AbstractClass,AMImpClass,GAImpClass)	\
	RegisterClass_(AbstractClass);	\
	if (useAppearance) {	\
		RegisterClassID_(AMImpClass, AbstractClass::imp_class_ID);	\
	} else {	\
		RegisterClassID_(GAImpClass, AbstractClass::imp_class_ID);	\
	}

//----------
void	CYNCDlg::RegisterClass ()
{
	Boolean		useAppearance = UEnvironment::HasFeature (env_HasAppearance);

	URegistrar::RegisterClass ('oync', (ClassCreatorFunc)CYNCDlg::CreateYNCDlgStream);

	// register the pane classes we use
	RegisterClasses (LPushButton, LAMPushButtonImp, LGAPushButtonImp);
	RegisterClasses (LStaticText, LAMStaticTextImp, LGAStaticTextImp);
	RegisterClass_(LPicture);

	sIsRegistered = true;
}



//----------
CYNCDlg*
CYNCDlg::CreateYNCDlg		(LCommander		*inSuperCommander) {
	if (!sIsRegistered) {
		RegisterClass ();
	}
	CYNCDlg * dlog;
	dlog = ((CYNCDlg *)LWindow::CreateWindow(PPob_YNCDlgID, inSuperCommander));
	return dlog;
}


//----------
//	This is the function you register with URegistrar to create a
//	CYNCDlg from a resource
CYNCDlg*
CYNCDlg::CreateYNCDlgStream(
	LStream	*inStream)
{
	return (new CYNCDlg(inStream));
}


//----------
//	The default constructor does nothing.
CYNCDlg::CYNCDlg()
{
}


//----------
//	The read-from-stream constructor just reads a dialog object.
CYNCDlg::CYNCDlg(
	LStream	*inStream)
		: PARENT_DIALOGBOXCLASS(inStream)
{
}


//----------
CYNCDlg::~CYNCDlg()
{
}


//----------
//	This member function gets called once the containment hierarchy that contains
//	this pane has been built. It gives us a chance to get data members for
//	interesting subviews, and to do other operations now that our subviews exist.
void
CYNCDlg::FinishCreateSelf()
{
	PARENT_DIALOGBOXCLASS::FinishCreateSelf();

	mButton1 = (LPushButton*)FindPaneByID ('But1');
	mButton2 = (LPushButton*)FindPaneByID ('But2');
	mButton3 = (LPushButton*)FindPaneByID ('But3');
	mText = (LStaticText*)FindPaneByID('mtxt');
	mIcon = (LPicture*)FindPaneByID('icon');
	
	UReanimator::LinkListenerToControls(this, this, RidL_YNCDlgID);
		// the purpose is to "connect" self to whatever controls
		// that we want to "listen" to

// any additional initialization for your dialog:

}

/*
//----------
void
CYNCDlg::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	switch (inMessage) {
	case msg_OK:
			GetSuperCommander()->ProcessCommand(cmdOKdialog_YNCDlg, this);
		break;

	case msg_Cancel:
			DoClose();
		break;

	default:
		  ; // do something
		break;
	}
}
*/
//----------
Boolean
CYNCDlg::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {

	// +++ Add cases here for the commands you handle
	//		Remember to add same cases to FindCommandStatus below
	//		to enable/disable the commands


	default:
				cmdHandled = LWindow::ObeyCommand(inCommand, ioParam);
		break;
	}

	return cmdHandled;
}


//----------
void
CYNCDlg::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	outUsesMark = false;

	switch (inCommand) {

	// +++ Add cases here for the commands you handle

	default:
			PARENT_DIALOGBOXCLASS::FindCommandStatus(inCommand, outEnabled,
											outUsesMark, outMark, outName);
		break;
	}
}


/**
	Standard AppMaker-generated FocusDraw to set background color.
	Presumably overcomes long-time PowerPlant bug or weakness?
*/
Boolean
CYNCDlg::FocusDraw(LPane*)
{
	Boolean		focused = LView::FocusDraw();

	if (focused) {
#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0
		AuxWinHandle	awHndl;
		CTabHandle		awCTable;
		ColorSpec		contentSpec;

		GetAuxWin(GetMacPort(), &awHndl);
		awCTable = (**awHndl).awCTable;
		contentSpec = (**awCTable).ctTable [wContentColor];		// should search vs. index
		::RGBBackColor(&contentSpec.rgb);
#endif		
	}

	return focused;
}


void
CYNCDlg::SetMessageText(const char* inText)
{
	mText->SetDescriptor(LStr255(inText));
}

void
CYNCDlg::HideButton(int n)
{
	switch (n) {
	case 1:
		mButton1->Hide();
		break;
	case 2:
		mButton2->Hide();
		break;
	case 3:
		mButton3->Hide();
		break;
	}
}


void
CYNCDlg::SetButtonCaption(int n, const char* inCap)
{
	switch (n) {
	case 1:
		mButton1->SetDescriptor(LStr255(inCap));
		break;
	case 2:
		mButton2->SetDescriptor(LStr255(inCap));
		break;
	case 3:
		mButton3->SetDescriptor(LStr255(inCap));
		break;
	}
}

void
CYNCDlg::SetTitle(const char* inTitle)
{
	SetDescriptor(LStr255(inTitle));
}


void
CYNCDlg::SetDefaultBtn(int n)
{
	switch (n) {
	case 1:
		SetDefaultButton(mButton1->GetPaneID());
		break;
	case 2:
		SetDefaultButton(mButton2->GetPaneID());
		break;
	case 3:
		SetDefaultButton(mButton3->GetPaneID());
		break;
	}
}


void
CYNCDlg::SetCancelBtn(int n)
{
	switch (n) {
	case 1:
		SetCancelButton(mButton1->GetPaneID());
		break;
	case 2:
		SetCancelButton(mButton2->GetPaneID());
		break;
	case 3:
		SetCancelButton(mButton3->GetPaneID());
		break;
	}
}


void
CYNCDlg::SetIcon(ResIDT inIconRes)
{
	if (inIconRes!=0) {
		mIcon->SetPictureID(inIconRes);	
		mText->ResizeFrameBy(-40, 0, false);		// 32 + 8
	}
}


// -------------------------------------------------------
//                   o o f Y N C
// -------------------------------------------------------
retTypeT
oofYNC::msgBox(bool playBeep)
{
	mDisplayed = true;	// so dtor doesn't display
	
	assert(mMsg.length());	// user must supply a message at least
	
	retTypeT ret = IDOK;
	
	CYNCDlg::RegisterClass();
	
	// save default commander, since it gets set to nil somewhere
	LCommander* saveCommander = LCommander::GetDefaultCommander();
	
	if (playBeep)
		SysBeep(1);
		
	StDialogHandler theDialog(PPob_YNCDlgID, LCommander::GetTarget());

	// shortcut for accessing dialog members
	#define YNC_DIALOG ((CYNCDlg*) theDialog.GetDialog())

	// set dialog text, buttons captions, etc based on passed parameters
	
	// 0. Dialog title
	// try manually set title first
	if (mTitle)
		YNC_DIALOG->SetTitle(mTitle);
	else	// try default title
		if (sDefaultTitle)
			YNC_DIALOG->SetTitle(sDefaultTitle);
	
	// 1. Text
	YNC_DIALOG->SetMessageText(mMsg);
	
	// 2. Button visibility
	switch (mType) {
	case MB_OK:
		YNC_DIALOG->HideButton(3);
		YNC_DIALOG->HideButton(2);
		break;
		
	case MB_OKCANCEL:	// fall through
	case MB_YESNO:		// fall through
	case MB_RETRYCANCEL:
		YNC_DIALOG->HideButton(3);
		break;
		
	case MB_USERBUTTONS:
		if (!mButton2)	
			YNC_DIALOG->HideButton(2);
		if (!mButton3)	
			YNC_DIALOG->HideButton(3);
		break;
	}
	
	// 3. Button captions
	// This could be made more efficient using fall throughs.
	switch (mType) {
	case MB_OK:
		YNC_DIALOG->SetButtonCaption(1, "OK");
		break;
	case MB_OKCANCEL:
		YNC_DIALOG->SetButtonCaption(1, "OK");
		YNC_DIALOG->SetButtonCaption(2, "Cancel");
		break;
	case MB_ABORTRETRYIGNORE:
		YNC_DIALOG->SetButtonCaption(1, "Abort");
		YNC_DIALOG->SetButtonCaption(2, "Retry");
		YNC_DIALOG->SetButtonCaption(3, "Ignore");
		break;
	case MB_YESNOCANCEL:
		YNC_DIALOG->SetButtonCaption(1, "Yes");
		YNC_DIALOG->SetButtonCaption(2, "No");
		YNC_DIALOG->SetButtonCaption(3, "Cancel");
		break;
	case MB_YESNO:
		YNC_DIALOG->SetButtonCaption(1, "Yes");
		YNC_DIALOG->SetButtonCaption(2, "No");
		break;
	case MB_RETRYCANCEL:
		YNC_DIALOG->SetButtonCaption(1, "Retry");
		YNC_DIALOG->SetButtonCaption(2, "Cancel");
		break;
	case MB_USERBUTTONS:
		YNC_DIALOG->SetButtonCaption(1, mButton1);
		if (mButton2)	
			YNC_DIALOG->SetButtonCaption(2, mButton2);
		if (mButton3)	
			YNC_DIALOG->SetButtonCaption(3, mButton3);
		break;
	}
	
	// 4. Set default/cancel buttons
	if (mDefaultButton)
		YNC_DIALOG->SetDefaultBtn(mDefaultButton);
	
	// cancel button may be the same as the default button
	switch (mType) {
	case MB_OK:
		YNC_DIALOG->SetCancelBtn(1);	// OK is default and cancel
		break;
	case MB_OKCANCEL:
		YNC_DIALOG->SetCancelBtn(2);
		break;
	case MB_ABORTRETRYIGNORE:
		YNC_DIALOG->SetCancelBtn(1);	// ignore is default and cancel
		break; 
	case MB_YESNOCANCEL:
		YNC_DIALOG->SetCancelBtn(3);
		break;
	case MB_YESNO:
		YNC_DIALOG->SetCancelBtn(2);
		break;
	case MB_RETRYCANCEL:
		YNC_DIALOG->SetCancelBtn(2);
		break;
	case MB_USERBUTTONS:
		if (mButton3)	
			YNC_DIALOG->SetCancelBtn(3);
		else if (mButton2)	
			YNC_DIALOG->SetCancelBtn(2);
		break;
	}

	// 5. Set Icon
	ResIDT thePicIcon=0;
	// if user has not set icon use defaults
	if (!mIcon) {
		switch (mType) {
		case MB_OK:
		case MB_OKCANCEL:
			mIcon = MB_ICONINFORMATION;
			break;
		case MB_YESNO:
		case MB_RETRYCANCEL:
		case MB_YESNOCANCEL:
			mIcon = MB_ICONQUESTION;
			break;
		case MB_ABORTRETRYIGNORE:
			mIcon = MB_ICONSTOP;
			break;
		}
	}
	
	if (mIcon) {
		switch (mIcon) {
		case MB_ICONSTOP:
			thePicIcon = 29000;
			break;
		case MB_ICONINFORMATION:
			thePicIcon = 29001;
			break;
		case MB_ICONEXCLAMATION:
			thePicIcon = 29002;
			break;
		case MB_ICONQUESTION:	// this is a Windows icon - Mac GUI standards don't have a different		
			thePicIcon = 29003;	// icon for questions, so our default is MB_ICONEXCLAMATION
			break;
		}			
		YNC_DIALOG->SetIcon(thePicIcon);
	}

	// 6. Show dialog and wait for response
	YNC_DIALOG->Show();
	
	// call StDialogHandler::DoDialog to process events
	// keep looping until message received
	MessageT dlgVal;
	do
	{
		dlgVal = theDialog.DoDialog();
	}
	while (dlgVal != 'btn1' && dlgVal != 'btn2' && dlgVal != 'btn3');

	// 7. set return value based on dialog response
	switch (dlgVal) {
	case 'btn1':
		switch (mType) {
		case MB_OK:				// fall through
		case MB_OKCANCEL:
			ret = IDOK;
			break;
		case MB_ABORTRETRYIGNORE:
			ret = IDABORT;
			break;
		case MB_YESNOCANCEL:	// fall through
		case MB_YESNO:
			ret = IDYES;
			break;
		case MB_RETRYCANCEL:
			ret = IDRETRY;
			break;
		default:
			ret = IDYES;
		}
		break;
	case 'btn2':
		switch (mType) {
		case MB_OKCANCEL:		// fall through
		case MB_RETRYCANCEL:
			ret = IDCANCEL;
			break;
		case MB_ABORTRETRYIGNORE:
			ret = IDRETRY;
			break;
		case MB_YESNOCANCEL:
			ret = IDNO;
			break;
		case MB_YESNO:
			ret = IDNO;
			break;
		default:
			ret = IDNO;
		}
		break;
	case 'btn3':
		switch (mType) {
		case MB_ABORTRETRYIGNORE:
			ret = IDIGNORE;
			break;
		case MB_YESNOCANCEL:
			ret = IDCANCEL;
			break;
		default:
			ret = IDCANCEL;
		}
		break;
	}
	
	// restore previous default commander
	LCommander::SetDefaultCommander(saveCommander);
	
	return ret;
	#undef YNC_DIALOG
}



