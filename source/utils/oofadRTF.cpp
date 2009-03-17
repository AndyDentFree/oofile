// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for RTF output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNRTF
	#include "oofadRTF.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepRTFEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_RTFRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_RTFRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_RTFRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void
oofBorderAdorner_RTFRender::draw(oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_RTFRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepRTFEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_RTFRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_RTFRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_RTFRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void
oofShadingAdorner_RTFRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_RTFRender::draw";
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepRTFEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_RTFRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_RTFRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_RTFRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_RTFRender::draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
	//cout << "oofBandShadingAdorner_RTFRender::draw";
}

