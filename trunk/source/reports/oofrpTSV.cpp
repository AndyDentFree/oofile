// COPYRIGHT 1998 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories & renderers for TSV output

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPTSV
	#include "oofrpTSV.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif

#if OOF_USE_ANSI_STREAMS
	#include <iomanip>
#else
	#include <iomanip.h>
#endif



// ---------------------------------------
//     o o f R e p T S V E n v
// ---------------------------------------
oofRepTSVEnv::oofRepTSVEnv()
{
}


oofRepTSVEnv::oofRepTSVEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}


oofRepTSVEnv::oofRepTSVEnv(const char* fileName) :
	oofRepStreamEnv(fileName)
{
}


void	
oofRepTSVEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepTSVEnv&.
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
oofRepTSVEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}

// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepTSVEnv::renderType() const
{
	return eRenderType;
}


void
oofRepTSVEnv::drawPageBreak()
{
}


void 
oofRepTSVEnv::changeTextStyle(oofRepTextStyle*)
{
}


unsigned short 
oofRepTSVEnv::heightOfTextStyle(oofRepTextStyle*)
{
	// Not relevant for TSV output... dummy value used instead
	return 12;
}


unsigned long 
oofRepTSVEnv::calcPageHeight()
{
	// Not relevant for TSV output... dummy value used instead
	return 840;
}


unsigned long 
oofRepTSVEnv::calcPageWidth()
{
	// Not relevant for TSV output... dummy value used instead
	return 590;
}

void
oofRepTSVEnv::startEnvironment()
{
}


void
oofRepTSVEnv::endEnvironment()
{
}


bool
oofRepTSVEnv::preprocessesPageHeadersAndFooters() const
{
	return false; 
}



// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_TSVRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_TSVRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_TSVRender(this);

}


void	
oofRepLineBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepLineBand_TSVRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_TSVRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_TSVRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_TSVRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepTSVEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_TSVRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _ T S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_TSVRender((oofRepViewBand*)newOwner);	// safe downcast
}

	

void 
oofRepViewBand_TSVRender::draw()	// draws just current record
{
	dbView* view = mOwner->view();
	unsigned int numflds = view->count();
	
	if (!mDrawnColHeadersThisPage) {	// only draw the headers once
		mOwner->drawColumnHeaders();
	}		
	for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
		oofString buf;
		oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(colNum);
		if (theDrawer) 
			buf = theDrawer->plainText(&view->field(colNum));
		else
			buf = view->field(colNum).copyString();
		
		// output a comma to seperate all elements in the table
		if( colNum > 0 ) oofRepStreamEnv::out() << '\t';
		
		oofRepStreamEnv::out() << buf;
	}
	oofRepStreamEnv::out() << endl;
}


void 
oofRepViewBand_TSVRender::drawColumnHeaders()
{
	dbView* view = mOwner->view();
	mDrawnColHeadersThisPage = true;
	if( oofRep::currentReport()->shouldDrawViewColumnHeaders() ) {
		unsigned int numflds = view->count();
		
		for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
			oofString buf = view->getHeading(colNum);
		
			// output a comma to seperate all elements in the table
			if( colNum > 0 ) oofRepStreamEnv::out() << '\t';
		
			oofRepStreamEnv::out() << buf;
		}
		oofRepStreamEnv::out() << endl;
	}
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _ T S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_TSVRender((oofRepLayoutBand*)newOwner);	// safe downcast
}

void 
oofRepLayoutBand_TSVRender::draw()
{
	oofRepBlockList& blocks = mOwner->blocks();
	blocks.start();
	
	while( blocks.more() ) {
	
		oofRepBlock *	theBlock = blocks();
		theBlock->draw();
		if( theBlock->moveDownAfter() ) {
			oofRepStreamEnv::out() << endl;
			
			blocks.next();
			
			if( blocks.more() ) {
				NULL;
			}
		}
		else {
			blocks.next();
			
			if( blocks.more() ) {
				oofRepStreamEnv::out() << '\t';
			}
		}
	} // while blocks
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _ T S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_TSVRender((oofRepLineBand*)newOwner);	// safe downcast
}

void 
oofRepLineBand_TSVRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _ T S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_TSVRender((oofRepPictBand*)newOwner);	// safe downcast
}

void 
oofRepPictBand_TSVRender::draw()
{
}

// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _ T S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_TSVRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_TSVRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ T S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_TSVRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_TSVRender::draw()
{
	dbView * graphView = mOwner->graph()->getView();
	dbTable * graphTable = graphView->table();

	unsigned int numflds = graphView->count();

	if( graphTable ) {
		
		oofRepStreamEnv::out() << mOwner->graph()->getTitle() << endl;
		
		if( numflds == 3 ) {
			// Output the different categories (from Column 2)
			
			// Reset the table to the beginning
			graphTable->start();
			
			// find out the first value from the first column
			oofString firstval = graphView->field(0).copyString();
			
			while( graphTable->more() ) {
			
				oofString buf = graphView->field(0).copyString();
				
				// if it's the same as the first one, output column 2 as the categories
				if( firstval == buf ) {
					oofString secondval = graphView->field(1).copyString();
					oofRepStreamEnv::out() << '\t' << secondval;
				}
				graphTable->next();
			};
			oofRepStreamEnv::out() << endl;
			
			// now output all the values
			
			// Reset the table to the beginning
			graphTable->start();
			
			// find out the previous value to keep track of current person
			oofString previousval, thisval;
			
			if( graphTable->more() ) {
				previousval = graphView->field(0).copyString();
				oofRepStreamEnv::out() << previousval;
			}
			
			while( graphTable->more() ) {
			
				thisval = graphView->field(0).copyString();
			
				// if it's the same record, output all in one line
				if( thisval == previousval ) {
					oofRepStreamEnv::out() << '\t' << graphView->field(2).copyString();
				} else {
					oofRepStreamEnv::out() << endl;
					oofRepStreamEnv::out() << thisval << '\t' << graphView->field(2).copyString();
				}
				
				// find out the first value from the first column
				previousval = graphView->field(0).copyString();
				
				graphTable->next();
			};
			oofRepStreamEnv::out() << endl;
		}
		else {
			for (graphTable->start(); graphTable->more(); graphTable->next()) {
				numflds = graphView->count();
				
				for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
					// output a comma to seperate all elements in the table
					if( colNum > 0 ) oofRepStreamEnv::out() << '\t';
					
					oofRepStreamEnv::out()<< graphView->field(colNum).copyString();
				}
				oofRepStreamEnv::out() << endl;
			}
		}
	} // if( graphTable )
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _ T S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_TSVRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_TSVRender::draw()
{
	oofRepStreamEnv::out() << mOwner->text();
}


// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _ T S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_TSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_TSVRender((oofRepPictBlock*)newOwner);	// safe downcast
}


void
oofRepPictBlock_TSVRender::draw()
{
}


