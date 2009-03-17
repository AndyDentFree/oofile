// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for HTML output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNHTM
	#include "oofadHTM.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------
void	
oofBorderAdorner::makeRender(const oofRepHTMLEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_HTMLRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ H T M L R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_HTMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_HTMLRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void 
oofBorderAdorner_HTMLRender::draw( oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_HTMLRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------
void	
oofShadingAdorner::makeRender(const oofRepHTMLEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_HTMLRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ H T M L R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_HTMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_HTMLRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void 
oofShadingAdorner_HTMLRender::draw( oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_HTMLRender::draw";
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------
void
oofBandShadingAdorner::makeRender(const oofRepHTMLEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_HTMLRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ H T M L R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_HTMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_HTMLRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_HTMLRender::draw( oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBandShadingAdorner_HTMLRender::draw";
}
