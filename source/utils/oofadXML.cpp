// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for XML output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNXML
	#include "oofadXML.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepXMLEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_XMLRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_XMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_XMLRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void
oofBorderAdorner_XMLRender::draw(oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_XMLRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepXMLEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_XMLRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_XMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_XMLRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void
oofShadingAdorner_XMLRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_XMLRender::draw";
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepXMLEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_XMLRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ R T F R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_XMLRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_XMLRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_XMLRender::draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
	//cout << "oofBandShadingAdorner_XMLRender::draw";
}

