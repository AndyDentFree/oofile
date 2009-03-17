// COPYRIGHT 1998 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories & renderers for CSV output

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPCSV
	#include "oofrpCSV.h"
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
//     o o f R e p C S V E n v
// ---------------------------------------
oofRepCSVEnv::oofRepCSVEnv()
{
}


oofRepCSVEnv::oofRepCSVEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}


oofRepCSVEnv::oofRepCSVEnv(const char* fileName) :
	oofRepStreamEnv(fileName)
{
}


void	
oofRepCSVEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepCSVEnv&.
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
oofRepCSVEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}

// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepCSVEnv::renderType() const
{
	return eRenderType;
}


void
oofRepCSVEnv::drawPageBreak()
{
}


void 
oofRepCSVEnv::changeTextStyle(oofRepTextStyle*)
{
}


unsigned short 
oofRepCSVEnv::heightOfTextStyle(oofRepTextStyle*)
{
	// Not relevant for CSV output... dummy value used instead
	return 12;
}


unsigned long 
oofRepCSVEnv::calcPageHeight()
{
	// Not relevant for CSV output... dummy value used instead
	return 840;
}


unsigned long 
oofRepCSVEnv::calcPageWidth()
{
	// Not relevant for CSV output... dummy value used instead
	return 590;
}

void
oofRepCSVEnv::startEnvironment()
{	
}


void
oofRepCSVEnv::endEnvironment()
{
}


bool
oofRepCSVEnv::preprocessesPageHeadersAndFooters() const
{
	return false; // david debug 991027
	
}


// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_CSVRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_CSVRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepGraphBand_CSVRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepLineBand_CSVRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepPictBand_CSVRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_CSVRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepTextBlock_CSVRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepCSVEnv& /*env*/) 
{
	delete mRender;
	mRender = new oofRepPictBlock_CSVRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _ C S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_CSVRender((oofRepViewBand*)newOwner);	// safe downcast
}

void 
oofRepViewBand_CSVRender::draw()	// draws just current record
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
		if( colNum > 0 ) oofRepStreamEnv::out() << ',';
		
		oofRepStreamEnv::out() << '"' << buf << '"';
	}
	oofRepStreamEnv::out() << endl;
}


void 
oofRepViewBand_CSVRender::drawColumnHeaders()
{
	dbView* view = mOwner->view();
	mDrawnColHeadersThisPage = true;
	if( oofRep::currentReport()->shouldDrawViewColumnHeaders() ) {
		unsigned int numflds = view->count();
		
		for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
			oofString buf = view->getHeading(colNum);
		
			// output a comma to seperate all elements in the table
			if( colNum > 0 ) oofRepStreamEnv::out() << ',';
		
			oofRepStreamEnv::out() << '"' << buf << '"';
		}
		oofRepStreamEnv::out() << endl;
	}
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _ C S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_CSVRender((oofRepLayoutBand*)newOwner);	// safe downcast
}

void 
oofRepLayoutBand_CSVRender::draw()
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
				oofRepStreamEnv::out() << ',';
			}
		}
	} // while blocks
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _ C S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_CSVRender((oofRepLineBand*)newOwner);	// safe downcast
}

void 
oofRepLineBand_CSVRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _ C S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_CSVRender((oofRepPictBand*)newOwner);	// safe downcast
}

void 
oofRepPictBand_CSVRender::draw()
{
}

// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _ C S V R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_CSVRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_CSVRender::draw()
{
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ C S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_CSVRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_CSVRender::draw()
{
	dbView * graphView = mOwner->graph()->getView();
	dbTable * graphTable = graphView->table();

	unsigned int numflds = graphView->count();

	if( graphTable ) {
		
		oofRepStreamEnv::out() << '"' << mOwner->graph()->getTitle() << '"' << endl;
		
		if( numflds == 3 ) {
			// Output the different categories (from Column 2)
			oofRepStreamEnv::out() << "\"\"";
			
			// Reset the table to the beginning
			graphTable->start();
			
			// find out the first value from the first column
			oofString firstval = graphView->field(0).copyString();
			
			while( graphTable->more() ) {
			
				oofString buf = graphView->field(0).copyString();
				
				// if it's the same as the first one, output column 2 as the categories
				if( firstval == buf ) {
					oofString secondval = graphView->field(1).copyString();
					oofRepStreamEnv::out() << ",\"" << secondval << '"';
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
				oofRepStreamEnv::out() << '"' << previousval << '"';
			}
			
			while( graphTable->more() ) {
			
				thisval = graphView->field(0).copyString();
			
				// if it's the same record, output all in one line
				if( thisval == previousval ) {
					oofRepStreamEnv::out() << ",\"" << graphView->field(2).copyString() << '"';
				} else {
					oofRepStreamEnv::out() << endl;
					oofRepStreamEnv::out() << '"' << thisval << "\",\"" << graphView->field(2).copyString() << '"';
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
					if( colNum > 0 ) oofRepStreamEnv::out() << ',';
					
					oofRepStreamEnv::out() << '"' << graphView->field(colNum).copyString() << '"';
				}
				oofRepStreamEnv::out() << endl;
			}
		}
	} // if( graphTable )
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _ C S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_CSVRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_CSVRender::draw()
{
	oofRepStreamEnv::out() << '"' << mOwner->text() << '"';
}


// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _ C S V R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_CSVRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_CSVRender((oofRepPictBlock*)newOwner);	// safe downcast
}


void
oofRepPictBlock_CSVRender::draw()
{
}
