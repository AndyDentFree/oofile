// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories for character stream output

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPTXT
	#include "oofrptxt.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif


//************ PROGRAMMER ALERT - NEEDS CONVERTING FROM MAC TO RTF ***********

// ---------------------------------------
//     o o f R e p T e x t E n v
// ---------------------------------------
oofRepTextEnv::oofRepTextEnv()
{
}


oofRepTextEnv::oofRepTextEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}


oofRepTextEnv::oofRepTextEnv(const char* fileName) :
	oofRepStreamEnv(fileName)
{
}


void	
oofRepTextEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepTextEnv&.
// The implication is that, if we add an environment, each drawable must have a makeRender
// added to match, or it will not be rendered (the base empty method will be invoked).

// eg: if we need to handle printing and previewing differently on the Mac, we would end up
// with a makeRender(const oofRepMacPrint&) and makeRender(const oofRepMacGUIPreviewPP&)
// for each oofRepDrawable

// The alternative would be to have each environment know each drawable. That is far
// less extensible - users can add their own drawables that we wouldn't know about,
// so the environment classes wouldn't be closed for extension.
	drawable->makeRender(*this);
}

void	
oofRepTextEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}


// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepTextEnv::renderType() const
{
	return eRenderType;
}

		
// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_TextRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_TextRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_TextRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepLineBand_TextRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_TextRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_TextRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_TextRender(this);
}

void	
oofRepPictBlock::makeRender(const oofRepTextEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_TextRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _T e x t R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_TextRender((oofRepViewBand*)newOwner);	// safe downcast
}


void 
oofRepViewBand_TextRender::draw()	// draws just current record
{
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _T e x t R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_TextRender((oofRepLayoutBand*)newOwner);	// safe downcast
}


void 
oofRepLayoutBand_TextRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _T e x t R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_TextRender((oofRepLineBand*)newOwner);	// safe downcast
}


void 
oofRepLineBand_TextRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _T e x t R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_TextRender((oofRepPictBand*)newOwner);	// safe downcast
}


void 
oofRepPictBand_TextRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _T e x t R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_TextRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_TextRender::draw()
{
	oofRep::currentReport()->currentVerticalPos() += mOwner->fixedHeight();
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _T e x t R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_TextRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_TextRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _T e x t R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_TextRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_TextRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _T e x t R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_TextRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_TextRender((oofRepPictBlock*)newOwner);	// safe downcast
}


void
oofRepPictBlock_TextRender::draw()
{
}

