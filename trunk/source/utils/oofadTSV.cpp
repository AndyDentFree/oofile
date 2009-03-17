// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for TSV output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNTSV
	#include "oofadTSV.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepTSVEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_TSVRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_TSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_TSVRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void
oofBorderAdorner_TSVRender::draw(oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_TSVRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepTSVEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_TSVRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_TSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_TSVRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void
oofShadingAdorner_TSVRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_TSVRender::draw";
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepTSVEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_TSVRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_TSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_TSVRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_TSVRender::draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
	//cout << "oofBandShadingAdorner_TSVRender::draw";
}

