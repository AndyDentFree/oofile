// COPYRIGHT 1999 A.D. Software, All rights reserved

// Platform-independent GUI Integration layer of OOFILE database

#include "oofpch_m.h"  // for precompilation of MFC GUI files

#ifndef OOFYNC_H
	#include "oofync.h"
#endif


// -------------------------------------------------------
//                   o o f Y N C
// -------------------------------------------------------
// initialise statics
const char* oofYNC::sDefaultTitle = 0;

oofYNC::oofYNC() :
	mType(MB_OK),
	// mMsg, an oofString, defaults to empty
	mTitle(0),
	mDefaultButton(0),
	mIcon(MB_ICONEXCLAMATION),
	mButton1(0),
	mButton2(0),
	mButton3(0),
	mDisplayed(false)
{}


oofYNC::oofYNC(const char* msg, const char* b1, const char* b2, const char* b3) :
	mType(MB_USERBUTTONS),
	mMsg(msg),
	mTitle(0),
	mDefaultButton(0),
	mIcon(MB_ICONEXCLAMATION),
	mButton1(b1),
	mButton2(b2),
	mButton3(b3),
	mDisplayed(false)
{}


oofYNC::oofYNC(const char* msg, msgTypeT type) :
	mMsg(msg),
	mType(type),
	mTitle(0),
	mDefaultButton(0),
	mIcon(MB_ICONEXCLAMATION),
	mButton1(0),
	mButton2(0),
	mButton3(0),
	mDisplayed(false)
{
}


#ifndef OOF_NO_STDLIB
oofYNC::oofYNC(ostream& os, msgTypeT type) :
	mMsg(os),
	mType(type),
	mTitle(0),
	mDefaultButton(0),
	mIcon(MB_ICONEXCLAMATION),
	mButton1(0),
	mButton2(0),
	mButton3(0),
	mDisplayed(false)
{}
#endif


oofYNC::~oofYNC()
{
// the following check allows for single line declaration and use of an anonymous oofYNC
// eg:
// 	oofYNC("Spellcheck complete!", "OK");

// however the user can make additional settings, and invoke msgBox manually, eg: to change
// icon types
	if (!mDisplayed)
		msgBox();
}


