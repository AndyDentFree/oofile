// COPYRIGHT 1997 A.D. Software, All rights reserved

// adorner layer of OOFILE database
// renderer factories for CSV output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFADORNCSV
	#include "oofadCSV.h"
#endif

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------

void	
oofBorderAdorner::makeRender(const oofRepCSVEnv&) 
{
	delete mRender;
	mRender = new oofBorderAdorner_CSVRender(this);
}

// -------------------------------------------------------
//   o o f B o r d e r A d o r n e r _ C S V R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBorderAdorner_CSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofBorderAdorner_CSVRender((oofBorderAdorner*)newOwner);	// safe downcast
}

void
oofBorderAdorner_CSVRender::draw(oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofBorderAdorner_CSVRender::draw";
}

// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofShadingAdorner::makeRender(const oofRepCSVEnv&) 
{
	delete mRender;
	mRender = new oofShadingAdorner_CSVRender(this);
}

// -------------------------------------------------------
//   o o f S h a d i n g A d o r n e r _ C S V R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofShadingAdorner_CSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofShadingAdorner_CSVRender((oofShadingAdorner*)newOwner);	// safe downcast
}

void
oofShadingAdorner_CSVRender::draw( /*const*/ oofRepDrawable* /*drawable*/, unsigned long /*height*/, unsigned long /*width*/ )
{
	//cout << "oofShadingAdorner_CSVRender::draw";
}

// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------

void	
oofBandShadingAdorner::makeRender(const oofRepCSVEnv&) 
{
	delete mRender;
	mRender = new oofBandShadingAdorner_CSVRender(this);
}

// -------------------------------------------------------
//   o o f B a n d S h a d i n g A d o r n e r _ C S V R e n d e r 
// -------------------------------------------------------
oofAdornerRender*
oofBandShadingAdorner_CSVRender::clone(const oofAdorner* newOwner) const
{
	return new oofBandShadingAdorner_CSVRender((oofBandShadingAdorner*)newOwner);	// safe downcast
}

void 
oofBandShadingAdorner_CSVRender::draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
	//cout << "oofBandShadingAdorner_CSVRender::draw";
}

