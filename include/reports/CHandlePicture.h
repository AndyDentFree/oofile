// ===========================================================================
//	CHandlePicture.h				  					    Based on LPicture.
// ===========================================================================

#ifndef _H_CHandlePicture
#define _H_CHandlePicture
#pragma once

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
	#pragma import on
#endif

#include <LView.h>

/**
	View to display a PICT taking up the whole enclosing window.
	Used in the Mac report writer preview.
*/
class	CHandlePicture : public LView {

public:
	enum { class_ID = 'Pikt' };
	static CHandlePicture*	CreatePictureStream(LStream *inStream);
	
						CHandlePicture();
						CHandlePicture(const CHandlePicture &inOriginal);
						CHandlePicture(const SPaneInfo &inPaneInfo,
							const SViewInfo &inViewInfo, PicHandle inPict);
						CHandlePicture(LStream *inStream);
						CHandlePicture(PicHandle inPicture);
						
	void				SetPicture(void *inPictPtr, unsigned long size);
	
	PicHandle			GetPictureH();
	void				SetPictureH(PicHandle inPict);

protected:
	PicHandle			mPICT;  //Owned !

	virtual void		DrawSelf();

private:
	void				InitPicture();
};

#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)
	#pragma import reset
#endif

#endif
