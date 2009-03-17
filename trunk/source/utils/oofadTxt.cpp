// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for Text output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNTXT
	#include "oofadTxt.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------
void	
oofBorderAdorner::makeRender(const oofRepTextEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_TextRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ T e x t R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_TextRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_TextRender((oofBorderAdorner*)newOwner);	// safe downcast
}


void 
oofBorderAdorner_TextRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_TextRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------
void	
oofShadingAdorner::makeRender(const oofRepTextEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_TextRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ T e x t R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_TextRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_TextRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void 
oofShadingAdorner_TextRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_TextRender::draw";
}


// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------
void
oofBandShadingAdorner::makeRender(const oofRepTextEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_TextRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ T e x t R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_TextRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_TextRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_TextRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBandShadingAdorner_TextRender::draw";
}
