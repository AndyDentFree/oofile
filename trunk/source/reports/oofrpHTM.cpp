// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories for HTML output


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPHTML
	#include "oofrpHTM.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif

// ---------------------------------------
//     o o f R e p H T M L E n v
// ---------------------------------------
oofRepHTMLEnv::oofRepHTMLEnv()
{
}


oofRepHTMLEnv::oofRepHTMLEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}

oofRepHTMLEnv::oofRepHTMLEnv(const char* fileName) :
	oofRepStreamEnv(fileName)
{
}


void	
oofRepHTMLEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepHTMLEnv&.
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
oofRepHTMLEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}


// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepHTMLEnv::renderType() const
{
	return eRenderType;
}

void
oofRepHTMLEnv::drawPageBreak()
{
}


void 
oofRepHTMLEnv::changeTextStyle(oofRepTextStyle*)
{
}


unsigned short 
oofRepHTMLEnv::heightOfTextStyle(oofRepTextStyle*)
{
	return 12;
}


unsigned long 
oofRepHTMLEnv::calcPageHeight()
{
	return 400;
}


unsigned long 
oofRepHTMLEnv::calcPageWidth()
{
	return 200;
}

void
oofRepHTMLEnv::startEnvironment()
{
	// SGML document type
	*mOut << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 3.2//EN\">\n";
	
	// HTML Header
	*mOut << "<html>\n";
	
	// Open Header
	*mOut << "<head>\n";
	
	// Page Title
	*mOut << "<title>Report</title>\n";
	
	// Close Header
	*mOut << "</head>\n";
	
	// Open the body of the HTML document
	*mOut << "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n";
}


void
oofRepHTMLEnv::endEnvironment()
{
	// Close body section then end HTML document
	*mOut << endl << "</body>" << endl << "</html>" << endl;
}


// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_HTMLRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_HTMLRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_HTMLRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepLineBand_HTMLRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_HTMLRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_HTMLRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_HTMLRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepHTMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_HTMLRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _H T M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_HTMLRender((oofRepViewBand*)newOwner);	// safe downcast
}

void
oofRepViewBand_HTMLRender::startTableBody(bool /*headersHaveChanged*/)
{
	const unsigned short viewLevel = oofRep::currentReport()->nestedViewLevel()-1;
	const oofString indent('\t', viewLevel);
	if (viewLevel>0)  // wrap sub-table inside <TR>
		oofRepStreamEnv::out() << indent << "<tr><td>\n";
		
	if( mOwner->isBoxed() ) {
		oofRepStreamEnv::out() << indent << "<table width=\"100%\" border=1>\n";
	} else {
		oofRepStreamEnv::out() << indent << "<table width=\"100%\">\n";
	}
}

void
oofRepViewBand_HTMLRender::endTableBody()
{
	const unsigned short viewLevel = oofRep::currentReport()->nestedViewLevel()-1;
	const oofString indent('\t', viewLevel);
	oofRepStreamEnv::out() << indent << "</table>\n\n";
	if (viewLevel>0)  // wrap sub-table inside <TR>
		oofRepStreamEnv::out() << indent << "</td></tr>\n";
		
}


void 
oofRepViewBand_HTMLRender::CalcColWidths ()
{
	mOwner->view()->colSizer()->calculateWidths(100, oofRep::currentReport()->settings()->colSepWidth());  // output as percentages
}


void 
oofRepViewBand_HTMLRender::draw()	// draws just current record
{
	dbView* view = mOwner->view();
	const unsigned int numflds = view->count();
	const unsigned short viewLevel = oofRep::currentReport()->nestedViewLevel();  // rows indent inside table
	const oofString rowIndent('\t', viewLevel);
	const oofString cellIndent('\t', viewLevel+1);
	
	// Standard row opener
	oofRepStreamEnv::out() << rowIndent << "<tr>" << endl;

	// for the moment table text is set to Helvetica plain
	// use:
	// mOwner->textStyle()
	//oofRepStreamEnv::out() << "\\pard\\plain \\intbl \\f21" << endl;
	
	for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
		oofString buf;
		oofRepCustomViewDrawer* theDrawer = mOwner->customDrawer(colNum);
		if (theDrawer) 
			buf = theDrawer->plainText(&view->field(colNum));
		else
			buf = view->field(colNum).copyString();

		oofColSizer::colAlignT alignment = view->colSizer()->alignment(colNum);
		// Cope with natural alignment
		if(alignment==oofColSizer::alignNatural) 
			alignment = oofColSizer::naturalAlignmentForField(view->field(colNum).fieldType());
			
		switch(alignment){
		case (oofColSizer::alignRight):{
			oofRepStreamEnv::out() << cellIndent << "<td align=right";
			}
		 	break;
		 
		case (oofColSizer::alignCentre):{
			oofRepStreamEnv::out() << cellIndent << "<td align=center";
			}
			break;
			
		default :{
			oofRepStreamEnv::out() << cellIndent << "<td";
			}
		}	

		oofRepStreamEnv::out() << " width=\"" <<  view->colSizer()->width(colNum) << "%\">" << buf << "</td>" << endl;
	}
	
	// Finish off the table row
	oofRepStreamEnv::out() << rowIndent << "</tr>" << endl;
}



// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _H T M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_HTMLRender((oofRepLayoutBand*)newOwner);	// safe downcast
}


void 
oofRepLayoutBand_HTMLRender::draw()
{
	//oofRepStreamEnv::out() << "-oofRepLayoutBand_HTMLRender::draw()" << endl;
	
	oofRepBlockList& blocks = mOwner->blocks();
//	oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
	
	blocks.start();
	
	while( blocks.more() ) {
	
		oofRepBlock *	theBlock = blocks();
		
		if( theBlock->align()==oofRepBlock::alignRight ) {	
			oofRepStreamEnv::out() << "<div align=right>";		// right adjusted?
		}
		
		theBlock->draw();
		
		if( theBlock->align()==oofRepBlock::alignRight ) {	
			oofRepStreamEnv::out() << "</div>";		// close right adjusted?
		}
		
		if( theBlock->moveDownAfter() ) {
			oofRepStreamEnv::out() << "<br>" << endl;
			
			blocks.next();
			
			if( blocks.more() ) {
				NULL;
			}
		} else {
			blocks.next();
		}
	}
	
	//oofRepStreamEnv::out() << "<br clear=all>" << endl;

}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _H T M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_HTMLRender((oofRepLineBand*)newOwner);	// safe downcast
}

void 
oofRepLineBand_HTMLRender::draw()
{
	oofRepStreamEnv::out() << "<hr>" << endl;
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _H T M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_HTMLRender((oofRepPictBand*)newOwner);	// safe downcast
}

/**
	Output \<IMG\> tag.
	\todo generate images and output actual file names.
*/
void 
oofRepPictBand_HTMLRender::draw()
{
	oofRepStreamEnv::out() << "<p align=centre><img src=\"oofreppictband.gif\"></p>" << endl;
}


// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _H T M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_HTMLRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_HTMLRender::draw()
{
	oofRepStreamEnv::out() << "<br clear=all>" << endl;
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _H T M L R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_HTMLRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_HTMLRender::draw()
{
	//oofRepStreamEnv::out() << "-oofRepGraphBand_HTMLRender::draw()" << endl;
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _H T M L R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_HTMLRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_HTMLRender::draw()
{
	//oofRepStreamEnv::out() << "-oofRepTextBlock_HTMLRender::draw()" << endl;
	oofRepStreamEnv::out() << mOwner->text();
}


// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _H T M L R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_HTMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_HTMLRender((oofRepPictBlock*)newOwner);	// safe downcast
}


/**
	Output \<IMG\> tag.
	\todo generate images and output actual file names.
*/
void
oofRepPictBlock_HTMLRender::draw()
{
	//oofRepStreamEnv::out() << "-oofRepPictBlock_HTMLRender::draw()" << endl;
	oofRepStreamEnv::out() << "<img src=\"oofreppictblock.gif\">";
}


