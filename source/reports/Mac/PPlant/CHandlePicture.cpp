// ===========================================================================
//	CHandlePicture.cp				   					    Based on LPicture.
// ===========================================================================
//
//	Displays a 'PICT' Handle which is owned by us

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include "CHandlePicture.h"
#include "assert.h"

#include <LStream.h>
#include <UDrawingState.h>

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif


// ---------------------------------------------------------------------------
//		 CreatePictureStream [static]
// ---------------------------------------------------------------------------
//	Return a new Picture object initialized using data from a Stream

CHandlePicture*
CHandlePicture::CreatePictureStream(
	LStream	*inStream)
{
	return (new CHandlePicture(inStream));
}


// ---------------------------------------------------------------------------
//		 CHandlePicture
// ---------------------------------------------------------------------------
//	Default Constructor

CHandlePicture::CHandlePicture()
{
	mPICT = nil;
}


// ---------------------------------------------------------------------------
//		 CHandlePicture(const CHandlePicture&)
// ---------------------------------------------------------------------------
//	Copy Constructor

CHandlePicture::CHandlePicture(
	const CHandlePicture	&inOriginal)
		: LView(inOriginal)
{
	mPICT = (PicHandle) NewHandle(GetHandleSize((Handle)inOriginal.mPICT));
	assert(mPICT);
	HLock((Handle)inOriginal.mPICT);
	HLock((Handle)mPICT);
	BlockMove(*inOriginal.mPICT,*mPICT,GetHandleSize((Handle)inOriginal.mPICT));
	HUnlock((Handle)inOriginal.mPICT);
	HUnlock((Handle)mPICT);
	InitPicture();
}


// ---------------------------------------------------------------------------
//		 CHandlePicture(SPaneInfo&, SViewInfo&, PicHandle)
// ---------------------------------------------------------------------------
//	Construct Picture from input parameters

CHandlePicture::CHandlePicture(
	const SPaneInfo	&inPaneInfo,
	const SViewInfo	&inViewInfo,
	PicHandle		inPICT)
		: LView(inPaneInfo, inViewInfo)
{
	mPICT = (PicHandle) NewHandle(GetHandleSize((Handle)inPICT));
	assert(mPICT);
	HLock((Handle)inPICT);
	HLock((Handle)mPICT);
	BlockMove(*inPICT,*mPICT,GetHandleSize((Handle)inPICT));
	HUnlock((Handle)inPICT);
	HUnlock((Handle)mPICT);
	InitPicture();
}


// ---------------------------------------------------------------------------
//		 CHandlePicture(LStream*)
// ---------------------------------------------------------------------------
//	Construct Picture from the data in a Stream

CHandlePicture::CHandlePicture(
	LStream	*inStream)
		: LView(inStream)
{
	// We don't read from the PPob because it makes no sense to have a predeclared
	// PicHandle. - Maybe we could have a picture in the PPob, but this seems a bit
	// silly - just use a PICT resource and use LPicture.
	mPICT=nil;
}


// ---------------------------------------------------------------------------
//		 CHandlePicture(PicHandle)
// ---------------------------------------------------------------------------
//	Construct a Picture from a PICT Handle

CHandlePicture::CHandlePicture(
	PicHandle	inPICT)
{
	mPICT = (PicHandle) NewHandle(GetHandleSize((Handle)inPICT));
	assert(mPICT);
	HLock((Handle)inPICT);
	HLock((Handle)mPICT);
	BlockMove(*inPICT,*mPICT,GetHandleSize((Handle)inPICT));
	HUnlock((Handle)inPICT);
	HUnlock((Handle)mPICT);
	InitPicture();
}


// ---------------------------------------------------------------------------
//		 InitPicture
// ---------------------------------------------------------------------------
//	Private Initializer. Assumes mPICT is set.

void
CHandlePicture::InitPicture()
{
	// If the PICT exists , set the Image size to the size of the picture.
	if (mPICT != nil) {
		MoveTo(0,0);
		Rect	picFrame = (**mPICT).picFrame;
		ResizeImageTo(picFrame.right - picFrame.left,
					  picFrame.bottom - picFrame.top, false);
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//		 GetPictureH
// ---------------------------------------------------------------------------
//	Return the PICT Handle associated with a Picture

PicHandle
CHandlePicture::GetPictureH()
{
	return mPICT;
}


// ---------------------------------------------------------------------------
//		 SetPictureH
// ---------------------------------------------------------------------------
//	Set the PICT Handle associated with a Picture
//
//	This changes size of the Image to match the bounds of the PICT.

void
CHandlePicture::SetPictureH(
	PicHandle	inPICT)
{
	if(mPICT)
		DisposeHandle((Handle)mPICT);
	mPICT = (PicHandle) NewHandle(GetHandleSize((Handle)inPICT));
	assert(mPICT);
	HLock((Handle)inPICT);
	HLock((Handle)mPICT);
	BlockMove(*inPICT,*mPICT,GetHandleSize((Handle)inPICT));
	HUnlock((Handle)inPICT);
	HUnlock((Handle)mPICT);
	InitPicture();
}

// ---------------------------------------------------------------------------
//		 SetPicture
// ---------------------------------------------------------------------------
//	Set the PICT associated with a Picture by copying it from a pointer.
//
//	This changes size of the Image to match the bounds of the PICT.

void
CHandlePicture::SetPicture(
	void *inPictPtr,
	unsigned long size)
{
	if(mPICT)
		DisposeHandle((Handle)mPICT);
	mPICT = (PicHandle) NewHandle(size);
	assert(mPICT);
	HLock((Handle)mPICT);
	BlockMove(inPictPtr,*mPICT,size);
	HUnlock((Handle)mPICT);
	InitPicture();
}


// ---------------------------------------------------------------------------
//		 DrawSelf
// ---------------------------------------------------------------------------
//	Draw a Picture

void
CHandlePicture::DrawSelf()
{
		// If Picture exists, draw it. Otherwise, fill the
		// Frame with a light gray pattern and a one-pixel border.
		
	if (mPICT != nil) {

		SDimension32	imageSize;
		GetImageSize(imageSize);
		
		Rect	pictureBounds;
		pictureBounds.left   = 0;
		pictureBounds.top    = 0;
		pictureBounds.right  = imageSize.width;
		pictureBounds.bottom = imageSize.height;
		
		::DrawPicture(mPICT, &pictureBounds);
		
	} else {
		Rect	frame;
		CalcLocalFrameRect(frame);
		::PenNormal();
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		Pattern drawPat;
		::FillRect(&frame,::GetQDGlobalsGray(&drawPat));
#else
		::FillRect(&frame, &UQDGlobals::GetQDGlobals()->ltGray);
#endif
		::FrameRect(&frame);
	}
}
	
