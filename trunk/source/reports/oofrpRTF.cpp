// COPYRIGHT 1997 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories & renderers for RTF output

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFREPRTF
	#include "oofrpRTF.h"
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

//const int	kPageWidth = 8560;	// page width in twips (1 twip is 1/10th of a point)
//bool		onceOnly = true;	// true - hasn't been written yet.


// ---------------------------------------
//     o o f R e p R T F E n v
// ---------------------------------------
oofRepRTFEnv::oofRepRTFEnv()
{
}


oofRepRTFEnv::oofRepRTFEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}


oofRepRTFEnv::oofRepRTFEnv(const char* fileName) :
	oofRepStreamEnv(fileName)
{
}


void	
oofRepRTFEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepRTFEnv&.
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
oofRepRTFEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}

// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepRTFEnv::renderType() const
{
	return eRenderType;
}


void
oofRepRTFEnv::drawPageBreak()
{
	*mOut << "\\page"; 
}



void 
oofRepRTFEnv::changeTextStyle(oofRepTextStyle*)
{
}


unsigned short 
oofRepRTFEnv::heightOfTextStyle(oofRepTextStyle*)
{
	return 12;
}


unsigned long 
oofRepRTFEnv::calcPageHeight()
{
	// return the value in pixels (use twips() to change to RTF twips)
	return 840;	//   // in A4. for US paper use  792
}


unsigned long 
oofRepRTFEnv::calcPageWidth()
{
	// return the value in pixels (use twips() to change to RTF twips)
	return 590;  // in A4. for US paper use 576;
}


/** 
called before each section, regardless of settings
unlike drawSectionBreak which may not be called if no page breaks specified
*/
void
oofRepRTFEnv::startSection()
{

// New section properties
	if (mSectionNumber>0)
		*mOut << "}\\sect ";	
	else  // 1st section or sole section in report
		*mOut << "\\sectd ";// reset to default section properties
		
	*mOut << "{\\pgnstart" << oofRep::currentReport()->startingPageNumber() << "\\pgnrestart"<< "}" << endl;

// header and footer position
	const unsigned long spaceAboveHeader = oofRep::currentReport()->settings()->topMargin();
	const unsigned long spaceBelowFooter = oofRep::currentReport()->settings()->bottomMargin();
	*mOut << "\\headery" << twips(spaceAboveHeader)  << endl;
	*mOut << "\\footery" << twips(spaceBelowFooter)  << endl;

// handle title page	
	if ((oofRep::currentReport()->firstPageHeaders().count()>0)||(oofRep::currentReport()->firstPageFooters().count()>0))	  
		*mOut << "\\titlepg"   << endl;

// define page headers
	*mOut << "{\\header\n";
	oofRep::currentReport()->drawPageHeaders();
	*mOut << "}\n";

// define page footers
	*mOut << "{\\footer\n";
	oofRep::currentReport()->drawPageFooters();
	*mOut << "}\n";
	
	
	if (oofRep::currentReport()->firstPageHeaders().count()>0){  
		*mOut << "{\\headerf\n";
		oofRep::currentReport()->drawFirstPageHeaders();
		*mOut << "}\n";
	}	

	
	if (oofRep::currentReport()->firstPageFooters().count()>0){  
		*mOut << "{\\footerf\n";
		oofRep::currentReport()->drawFirstPageFooters();
		*mOut << "}\n";
	
	
	}
	
	// open header block
	*mOut << "{\n";
	

	mSectionNumber++;
}


void
oofRepRTFEnv::startEnvironment()
{
	// RTF Header
	// version 1
	// ansi character set
	// default font number 0
	*mOut << "{\\rtf1\\ansi\\deff0" << endl;
	
#ifdef OOF_DEBUG
// must put this one after the RTF header so Word etc. can tell we are an RTF doc
	oofRepStreamEnv::out() << "{\\comment oofRepRTFEnv::startEnvironment}\n";
#endif

	// Font table
	*mOut << "{\\fonttbl";
	
	// Loop for all fonts adding them to the document like this:
//		for( unsigned int foo=0; foo<numFonts; foo++ ) {
//			mOut << "{\\f" << foo << "\\fnil " << fontName << ";}";
//			// NOT YET IMPLEMENTED:
//			// mOut << "{\\f" << foo << "\\f" << fontFamily() << ' ' << fontName() << ";}";
//		}
	// However, at the moment we'll use just one font, although varying size and style on the fly
	*mOut << "{\\f21\\fswiss Helvetica;}";
	
	// Finish font table
	*mOut << "}" << endl;
	
	// Colour table
	// In the future it might be nice to loop for all colours used in the doc
	*mOut << "{\\colortbl"
		<<	"\\red0\\green0\\blue0;"
		<<	"\\red0\\green0\\blue255;"
		<<	"\\red0\\green255\\blue255;"
		<<	"\\red0\\green255\\blue0;"
		<<	"\\red255\\green0\\blue255;"
		<<	"\\red255\\green0\\blue0;"
		<<	"\\red255\\green255\\blue0;"
		<<	"\\red255\\green255\\blue255;}" << endl;
	
	// Style Sheet
	// In the future it might be nice to loop for all styles used in the doc
	*mOut << "{\\stylesheet{\\fs20 \\snext0Normal;}}" << endl;
	
	// Info
	*mOut << "{\\info{\\author OOFile Report Writer}}" << endl;
	
// define page width and height otherwise RTF will be Word default, NOT current printer
// default (ie: will behave as if 8x11 even if A4 selected, until choose Page Setup
	*mOut << "\\paperw" << twips(oofRep::currentReport()->pageWidth())  << endl;
	*mOut << "\\paperh" << twips(oofRep::currentReport()->pageHeight())  << endl;

// define page margins, otherwise end up with whatever defaults user has set	
	*mOut << "\\margl" << twips(oofRep::currentReport()->settings()->leftMargin())  << endl;
	*mOut << "\\margr" << twips(oofRep::currentReport()->settings()->rightMargin())  << endl;
// top and bottom must be defined in terms of our page margins PLUS pageheader or footer height
// using negative numbers makes them "Exactly" the height, instead of the "At Least" which
// won't give us a consistent margin above the headers
	const unsigned long spaceAboveHeader = oofRep::currentReport()->settings()->topMargin();
	const unsigned long headerHeightPixels = oofRep::currentReport()->pageHeaders().fixedHeight() + spaceAboveHeader;
	*mOut << "\\margt" << twips(headerHeightPixels)  << endl;
	
	const unsigned long spaceBelowFooter = oofRep::currentReport()->settings()->bottomMargin();
	const unsigned long footerHeightPixels = oofRep::currentReport()->pageFooters().fixedHeight() + spaceBelowFooter;
	*mOut << "\\margb" << twips(footerHeightPixels) << endl;

	mSectionNumber=0;
	if (!oofRep::currentReport()->isMultiReport())
		startSection();		
}


void
oofRepRTFEnv::endEnvironment()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepRTFEnv::endEnvironment}\n";
#endif
	*mOut << "}\\sect}";
}


bool
oofRepRTFEnv::preprocessesPageHeadersAndFooters() const
{
	return true;
}




oofString
oofRepRTFEnv::encodeRTFString(const oofString& srcString)
{
	int index = 0;
	int destIndex = 0;
	char currentChar;
	const char * theString =  srcString;
	unsigned int numberOfEntities = 0;
	unsigned int newBufferSize = 0;
	char * newBuffer;
	while((currentChar = theString[index++])!='\0'){
		if((currentChar=='\\')||(currentChar=='{')||(currentChar=='}'))
			numberOfEntities++;
	}
	if(numberOfEntities){
		oofString res;
		newBufferSize = numberOfEntities + index + 1; 
		newBuffer = new char[newBufferSize];
		
		index = 0;
		while((currentChar = theString[index++])!='\0'){
			switch(currentChar){
			case '\\':
				newBuffer[destIndex++] = '\\';	
				newBuffer[destIndex++] = '\\';	
			
			break;
			case '{':
				newBuffer[destIndex++] = '\\';	
				newBuffer[destIndex++] = '{';	
			
			break;
			case '}':
				newBuffer[destIndex++] = '\\';	
				newBuffer[destIndex++] = '}';	
			break;
			default:
				newBuffer[destIndex++] = currentChar;	
			
			break;
			}
		}
		newBuffer[destIndex] = '\0';
		res.adopt(newBuffer);
		return res;
	}  // any entities
	else
		return srcString;
}


void		
oofRepRTFEnv::encodeToRTFStream(ostream& os, const char* theText)
{
	
	unsigned long index=0;
	char currentChar;
	while((currentChar = theText[index++])!= '\0'){
		switch(currentChar){
		case '{':
			os << '\\{';
			
		break;
		case '}':
			os << '\\}';
			break;
		case '\\':
			os << '\\\\';
		break;
		default:
			os << currentChar;
		}
	}
}



// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_RTFRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction();
}


void	
oofRepSpaceBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_RTFRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_RTFRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepLineBand_RTFRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_RTFRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_RTFRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_RTFRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_RTFRender(this);
}

void
oofRepPageNoBlock::makeRender(const oofRepRTFEnv& env) 
{
	delete mRender;
	mRender = new oofRepPageNoBlock_RTFRender(this);
}

// -------------------------------------------------------
//   o o f R e p V i e w B a n d _R T F R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepViewBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_RTFRender((oofRepViewBand*)newOwner);	// safe downcast
}

void 
oofRepViewBand_RTFRender::draw()	// draws just current record
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepViewBand_RTFRender::draw}\n";
#endif
	dbView* view = mOwner->view();
	bool isBoxed = mOwner->isBoxed();
	unsigned int numflds = view->count();
	
	// Microsoft Word 5.1a apparently demands repetitive rtf definitions or it crashes.
	// Thus where in some cases it would be briefer and more logical to allow
	// rows to inherit from previous rows, it's "safer" to be verbose.
	
	if (!mDrawnColHeadersThisPage) {
		mOwner->drawColumnHeaders();
	}		

	// Standard table opener (for each row, including the column headers)
	oofRepStreamEnv::out() << "\\trowd \\trgaph80\\trleft-80" << endl;
		
	view->start();

	// Make shading/border statements for each column
	unsigned long accumCellMargin = 0;
	for( unsigned int colNum=0; colNum<(numflds-1); colNum++ ) {
		if( isBoxed ) {
			oofRepStreamEnv::out() << "\\clbrdrt\\brdrs \\clbrdrl\\brdrs \\clbrdrb\\brdrs \\clbrdrr\\brdrs" << endl;
		}
		oofRepStreamEnv::out() << "\\clshdng0\\cellx";
		accumCellMargin += view->colSizer()->width(colNum) + oofRep::currentReport()->settings()->colSepWidth();
		oofRepStreamEnv::out() << twips( accumCellMargin ) << endl;
	}
	// last column stretches to the far edge of the page
	if( isBoxed ) {
		oofRepStreamEnv::out() << "\\clbrdrt\\brdrs \\clbrdrl\\brdrs \\clbrdrb\\brdrs \\clbrdrr\\brdrs" << endl;
	}
	oofRepStreamEnv::out() << "\\clshdng0\\cellx";
	oofRepStreamEnv::out() << twips(oofRep::currentReport()->bodyWidth()) << endl;
	
	oofRepStreamEnv::out() << "\\pard\\plain \\intbl" << endl;
	
	{	// ES - we need new scope for colNum, otherwise MSVC reports error - redefinition of colNum
	for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
	
		oofColSizer::colAlignT alignment = view->colSizer()->alignment(colNum);
		// Cope with natural alignment
		if(alignment==oofColSizer::alignNatural) 
			alignment = oofColSizer::naturalAlignmentForField(view->field(colNum).fieldType());

		switch(alignment){
		case (oofColSizer::alignLeft):{
				oofRepStreamEnv::out() << "\\ql";
			}
		 	break;
		 
		case (oofColSizer::alignRight):{
				oofRepStreamEnv::out() << "\\qr";
			}
		 	break;
		 
		case (oofColSizer::alignCentre):{
				oofRepStreamEnv::out() << "\\qc";
			}
			break;
			
		case (oofColSizer::alignJust):{
				oofRepStreamEnv::out() << "\\qj";
			}
			break;
		}

// WARNING \q directives ignored in cells if included inside the {}

		oofRepStreamEnv::out() << "{";
	
		// Output font size and style information
		int	theSize = mOwner->textStyle()->fontSize();
		int	theStyle = mOwner->textStyle()->fontStyle();

// NOT YET IMPLEMENTED - condense & extend ignored
		if( theStyle & shadow ) { oofRepStreamEnv::out() << "\\shad";}
		if( theStyle & outline ) { oofRepStreamEnv::out() << "\\outl";}
		if( theStyle & underline ) { oofRepStreamEnv::out() << "\\ul";}	
		if( theStyle & italic ) { oofRepStreamEnv::out() << "\\i";}
		if( theStyle & bold ) { oofRepStreamEnv::out() << "\\b"; }
		
		oofRepStreamEnv::out() << "\\fs" << 2*theSize;	// font size
		
		oofRepStreamEnv::out() << "\\f21 ";		// Helvetica ... NOT YET IMPLEMENTED - honor fontname

		// Scan the output for new lines whilst outputting
		const oofString buf = oofRepRTFEnv::encodeRTFString( mOwner->view()->field(colNum).copyString() );
		const char* runner = buf;
		
		while( *runner != '\0' ) {
			if( (*runner == 0x0D) /* CR - for PC and Unix */ ||
				(*runner == 0x0A) /* LF - for Mac */ )
				oofRepStreamEnv::out() << "\n\\line ";
			else
				oofRepStreamEnv::out() << *runner;
			runner++;
		}
		oofRepStreamEnv::out() << "}\\cell " << endl;
		}
	}
	
	// Finish off the table row
	oofRepStreamEnv::out() << "\\pard \\intbl \\row" << endl;
}


void 
oofRepViewBand_RTFRender::drawColumnHeaders()
{
	mDrawnColHeadersThisPage = true;
	if( oofRep::currentReport()->shouldDrawViewColumnHeaders() ) {
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepViewBand_RTFRender::drawColumnHeaders}\n";
#endif
	
		dbView* view = mOwner->view();
		bool isBoxed = mOwner->isBoxed();
		unsigned int numflds = view->count();

		// Standard table opener (for each row)
		oofRepStreamEnv::out() << "\\trowd \\trgaph80\\trleft-80" << endl;

		view->start();

		// Make shading/border statements for each column
		unsigned long accumCellMargin = 0;
		for( unsigned int colNum=0; colNum<(numflds-1); colNum++ ) {
			if( isBoxed ) {
				oofRepStreamEnv::out() << "\\clbrdrt\\brdrs \\clbrdrl\\brdrs \\clbrdrb\\brdrs \\clbrdrr\\brdrs" << endl;
			}
			oofRepStreamEnv::out() << "\\clshdng0\\cellx";
			accumCellMargin += view->colSizer()->width(colNum) + oofRep::currentReport()->settings()->colSepWidth();
			oofRepStreamEnv::out() << twips( accumCellMargin ) << endl;
		}
		// last column stretches to the far edge of the page
		if( isBoxed ) {
			oofRepStreamEnv::out() << "\\clbrdrt\\brdrs \\clbrdrl\\brdrs \\clbrdrb\\brdrs \\clbrdrr\\brdrs" << endl;
		}
		oofRepStreamEnv::out() << "\\clshdng0\\cellx";
		oofRepStreamEnv::out() << twips(oofRep::currentReport()->bodyWidth()) << endl;
			
		oofRepStreamEnv::out() << "\\pard\\plain \\intbl " << endl;
				
		{	// ES - we need new scope for colNum, otherwise MSVC reports error - redefinition of colNum
		for( unsigned int colNum=0; colNum<numflds; colNum++ ) {
		
			oofColSizer::colAlignT alignment = view->colSizer()->alignment(colNum);
			// Cope with natural alignment
			if(alignment==oofColSizer::alignNatural) 
				alignment = oofColSizer::naturalAlignmentForField(view->field(colNum).fieldType());

			switch(alignment){
			case (oofColSizer::alignLeft):{
					oofRepStreamEnv::out() << "\\ql";
				}
			 	break;
			 
			case (oofColSizer::alignRight):{
					oofRepStreamEnv::out() << "\\qr";
				}
			 	break;
			 
			case (oofColSizer::alignCentre):{
					oofRepStreamEnv::out() << "\\qc";
				}
				break;
				
			case (oofColSizer::alignJust):{
					oofRepStreamEnv::out() << "\\qj";
				}
				break;
			}
	
// WARNING \q directives ignored in cells if included inside the {}

			oofRepStreamEnv::out() << "{";
		
			// Output font size and style information
			int	theSize = mOwner->textStyle()->fontSize();
			//int	theStyle = mOwner->textStyle()->fontStyle();
			
			// THIS IS A HACK, FIX THE MAC RENDERER THEN DUPLICATE EFFECT HERE
			int	theStyle = 1;

// NOT YET IMPLEMENTED - condense & extend ignored
			if( theStyle & shadow ) { oofRepStreamEnv::out() << "\\shad";}
			if( theStyle & outline ) { oofRepStreamEnv::out() << "\\outl";}
			if( theStyle & underline ) { oofRepStreamEnv::out() << "\\ul";}	
			if( theStyle & italic ) { oofRepStreamEnv::out() << "\\i";}
			if( theStyle & bold ) { oofRepStreamEnv::out() << "\\b"; }
			
			oofRepStreamEnv::out() << "\\fs" << 2*theSize;	// font size
			
			oofRepStreamEnv::out() << "\\f21 ";		// Helvetica

			// Scan the output for new lines
			OOF_String buf = mOwner->view()->getHeading(colNum);
			const char* runner = buf;
			while( *runner != '\0' ) {
				if( (*runner == 0x0D) /* CR - for PC and Unix */ ||
					(*runner == 0x0A) /* LF - for Mac */ )
					oofRepStreamEnv::out() << "\n\\par \\intbl ";
				else
					oofRepStreamEnv::out() << *runner;
				runner++;
			}
			oofRepStreamEnv::out() << "}\\cell " << endl;
		}
		}
		
		// Finish off the table row
		oofRepStreamEnv::out() << "\\pard \\intbl \\row" << endl;
	}
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _R T F R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_RTFRender((oofRepLayoutBand*)newOwner);	// safe downcast
}

void 
oofRepLayoutBand_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepLayoutBand_RTFRender::draw}\n";
#endif
	oofRepBlockList& blocks = mOwner->blocks();
//	oofRep::currentReport()->changeTextStyle(blocks()->textStyle());
	
	//oofRepStreamEnv::out() << "\\pard \\tqr\\tx" << kPageWidth;
	oofRepStreamEnv::out() << "\\pard \\tqr\\tx" << twips(oofRep::currentReport()->bodyWidth()) << ' ';
	
	blocks.start();
	
	while( blocks.more() ) {
	
		oofRepBlock *	theBlock = blocks();
		
		// Output font size and style information
		int	theSize = blocks()->textStyle()->fontSize();
		int	theStyle = blocks()->textStyle()->fontStyle();
		
		oofRepStreamEnv::out() << "{";
		
// NOT YET IMPLEMENTED - condense & extend ignored
		if( theStyle & shadow ) { oofRepStreamEnv::out() << "\\shad";}
		if( theStyle & outline ) { oofRepStreamEnv::out() << "\\outl";}
		if( theStyle & underline ) { oofRepStreamEnv::out() << "\\ul";}	
		if( theStyle & italic ) { oofRepStreamEnv::out() << "\\i";}
		if( theStyle & bold ) { oofRepStreamEnv::out() << "\\b"; }
		
		oofRepStreamEnv::out() << "\\fs" << 2*theSize;	// font size
		
		oofRepStreamEnv::out() << "\\f21 ";		// Helvetica
		
		if( theBlock->align()==oofRepBlock::alignRight ) {	
			oofRepStreamEnv::out() << "\\tab ";		// right adjusted tab?
		}
		theBlock->draw();
		
		oofRepStreamEnv::out() << "}";
		
		if( theBlock->moveDownAfter() ) {
			oofRepStreamEnv::out() << "\\par " << endl;
			
			blocks.next();
			
			if( blocks.more() ) {
				NULL;
			}
		} 
		else {
			blocks.next();
		}
	} // while blocks
	oofRepStreamEnv::out() << "\\par " << endl;
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _R T F R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLineBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_RTFRender((oofRepLineBand*)newOwner);	// safe downcast
}

void 
oofRepLineBand_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepLineBand_RTFRender::draw}\n";
#endif
#ifdef _Macintosh
	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	Rect theLine={0,0,fixedHeight,oofRep::currentReport()->bodyWidth()};

	RgnHandle	saveClipRgn = NewRgn();		// restores changes to current grafport
	
	GetClip( saveClipRgn );					// save current grafport
	ClipRect(&theLine);
	
	PicHandle hPic = OpenPicture(&theLine);	// start drawing the line

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Pattern drawPat;
	if(1 )  // nasty hack to draw gray lines instead of black - need user settings!
	{
		FillRect(&theLine,::GetQDGlobalsGray(&drawPat));
		PenPat(::GetQDGlobalsGray(&drawPat));
	} else {
		::FillRect(&theLine,::GetQDGlobalsBlack(&drawPat));
		::PenPat(::GetQDGlobalsBlack(&drawPat));
	}
#else
	if(1 )  // nasty hack to draw gray lines instead of black - need user settings!
	{
		FillRect(&theLine,&qd.gray);
		PenPat(&qd.gray);
	} else {
		FillRect(&theLine,&qd.black);
		PenPat(&qd.black);
	}
#endif	
	FrameRect(&theLine);

	ClosePicture();							// finish drawing the line
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		PenPat(::GetQDGlobalsBlack(&drawPat));
#else
		PenPat(&qd.black);
#endif
	SetClip( saveClipRgn );					// restore previous value
	DisposeRgn( saveClipRgn );				// not needed any more

	// Output the RTF macpict header
	oofRepStreamEnv::out() 	
	<< "\\pard {\\pict\\macpict"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< "\\pichgoal" << twips(theLine.bottom)
	
	<< endl;

	HLock( (Handle)hPic );				// don't go away now...
	
	// PICT format reference: Inside Macintosh: Imaging With QuickDraw A-24
	// read in bits from the handle
	unsigned char	opcode;
	unsigned char*	bitsPointer = (unsigned char *)(*hPic);
	
	// In case the pict's size is greater than 32K it is necessary to
	// use this method instead of grabbing the first 2 bytes.
	int				picSize = GetHandleSize( (Handle)hPic );
	
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ >= 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
	int		wrap=0; 				// wrap every 60 words
	for( int word=0; word < picSize; word++ ) {
		opcode = *bitsPointer;
		bitsPointer++;
		
		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)opcode;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap=0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	// Should be 0xFF for End-Of-Pic opcode
	assert( opcode == 0xFF );
	
	HUnlock( (Handle)hPic );
	KillPicture( hPic );

	oofRepStreamEnv::out() << "} \\pard \\par" << endl;

#elif defined _Windows
/*	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	oofRect theLine(0,0,oofRep::currentReport()->bodyWidth(),fixedHeight);
	// initial color;
	COLORREF color = RGB(0,0,0);
	// in-memory bitmap will be created
	HBITMAP hBmpLine = ::CreateBitmap(
		oofRep::currentReport()->bodyWidth(),	// bitmap width, in pixels 
		fixedHeight,							// bitmap height, in pixels 
		1,										// number of color planes used by device 
		1,										// number of bits required to identify a color  
		&color					 				// pointer to array containing color data 
		);

	assert(hBmpLine != NULL);

	// Output the RTF bitmap header
	oofRepStreamEnv::out() 	
	<< "\\pard {\\pict\\wbitmap0"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< endl;

	BITMAP bm;
	int nRetVal = ::GetObject(hBmpLine, sizeof(bm), &bm);
	assert(nRetVal != 0);

	int bmpSize = bm.bmWidthBytes * bm.bmHeight * bm.bmPlanes;

	BYTE* buf = new BYTE[bmpSize];
	DWORD dwRetVal = ::GetBitmapBits(hBmpLine, bmpSize, buf);
	assert(dwRetVal != 0);

	BYTE data;
	
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
	int wrap = 0;	// wrap every 60 words
	for( int i = 0; i < bmpSize; i++ ) {
		data = buf[i];

		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap = 0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	oofRepStreamEnv::out() << "} \\pard \\par" << endl;

	delete buf;

	BOOL bRetVal = ::DeleteObject(hBmpLine);
	assert(bRetVal);
*/
	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	oofRect theLine(0,0,twips(oofRep::currentReport()->bodyWidth()),twips(fixedHeight));

//	HDC hRefDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
//	HDC hMetaFile = ::CreateEnhMetaFile(hRefDC, NULL, theLine, NULL);
	HDC hMetaFile = ::CreateMetaFile(NULL);

	::MoveToEx(hMetaFile, 0, 0, NULL);
	::FillRect(hMetaFile, theLine, (HBRUSH)GetStockObject(GRAY_BRUSH));
//	HENHMETAFILE hEnhMeta = ::CloseEnhMetaFile(hMetaFile);
	HMETAFILE hMeta = ::CloseMetaFile(hMetaFile);

//	UINT cbBuffer = ::GetWinMetaFileBits(hEnhMeta, 0, NULL, MM_TEXT, hRefDC);
	UINT cbBuffer = ::GetMetaFileBitsEx(hMeta, 0, NULL);
	BYTE* buf = new BYTE[cbBuffer];
	assert(buf != NULL);

//	cbBuffer = ::GetWinMetaFileBits(hEnhMeta, cbBuffer, buf, MM_TEXT, hRefDC);
	cbBuffer = ::GetMetaFileBitsEx(hMeta, cbBuffer, buf);

//
	// Output the RTF metafile header
	oofRepStreamEnv::out() 	
	<< "\\pard {\\pict\\wmetafile1"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< "\\picwgoal" << theLine.right
	<< "\\pichgoal" << theLine.bottom
	<< endl;

	BYTE data;
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);

	int wrap = 0;	// wrap every 60 words
	for( int i = 0; (unsigned int)i < cbBuffer; i++ ) {
		data = buf[i];

		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap = 0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	oofRepStreamEnv::out() << "} \\pard \\par" << endl;
//
	delete buf;
//	::DeleteEnhMetaFile(hEnhMeta);
//	::DeleteObject(hRefDC);
	::DeleteMetaFile(hMeta);
#endif
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _R T F R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_RTFRender((oofRepPictBand*)newOwner);	// safe downcast
}

/**
	Draw picture details to RTF document.
	\bug appears to be copy of oofRepLineBand_RTFRender
*/
void 
oofRepPictBand_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepPictBand_RTFRender::draw}\n";
#endif
#ifdef _Macintosh
	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	Rect theLine={0,0,fixedHeight,oofRep::currentReport()->bodyWidth()};

	RgnHandle	saveClipRgn = NewRgn();		// restores changes to current grafport
	
	GetClip( saveClipRgn );					// save current grafport
	ClipRect(&theLine);
	
	PicHandle hPic = OpenPicture(&theLine);	// start drawing the line
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	Pattern drawPat;
	if(1 )  // nasty hack to draw gray lines instead of black - need user settings!
	{
		FillRect(&theLine,::GetQDGlobalsGray(&drawPat));
		PenPat(::GetQDGlobalsGray(&drawPat));
	} else {
		::FillRect(&theLine,::GetQDGlobalsBlack(&drawPat));
		::PenPat(::GetQDGlobalsBlack(&drawPat));
	}
#else
	if( 1 )
	{
		FillRect(&theLine,&qd.gray);
		PenPat(&qd.gray);
	} else {
		FillRect(&theLine,&qd.black);
		PenPat(&qd.black);
	}
#endif	
	FrameRect(&theLine);

	ClosePicture();							// finish drawing the line
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	PenPat(::GetQDGlobalsBlack(&drawPat));
#else
	PenPat(&qd.black);
#endif
	SetClip( saveClipRgn );					// restore previous value
	DisposeRgn( saveClipRgn );				// not needed any more

	// Output the RTF macpict header
	oofRepStreamEnv::out() 	
	<< "\\pard \\sl-" << twips(theLine.bottom) << "{\\pict\\macpict"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< endl;

	HLock( (Handle)hPic );				// don't go away now...
	
	// PICT format reference: Inside Macintosh: Imaging With QuickDraw A-24
	// read in bits from the handle
	unsigned char	opcode;
	unsigned char*	bitsPointer = (unsigned char *)(*hPic);
	
	// In case the pict's size is greater than 32K it is necessary to
	// use this method instead of grabbing the first 2 bytes.
	int				picSize = GetHandleSize( (Handle)hPic );
	
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ >= 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
	int		wrap=0; 				// wrap every 60 words
	for( int word=0; word < picSize; word++ ) {
		opcode = *bitsPointer;
		bitsPointer++;
		
		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)opcode;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap=0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	// Should be 0xFF for End-Of-Pic opcode
	assert( opcode == 0xFF );
	
	HUnlock( (Handle)hPic );

	oofRepStreamEnv::out() << "}" << endl;
	
	//oofRepStreamEnv::out() << "\\pard \\par";	// ** this causes the pict to take an extra line

#elif defined _Windows
/*	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	oofRect theLine(0,0,oofRep::currentReport()->bodyWidth(),fixedHeight);
	// initial color;
	COLORREF color = RGB(0,0,0);
	// in-memory bitmap will be created
	HBITMAP hBmpLine = ::CreateBitmap(
		oofRep::currentReport()->bodyWidth(),	// bitmap width, in pixels 
		fixedHeight,							// bitmap height, in pixels 
		1,										// number of color planes used by device 
		1,										// number of bits required to identify a color  
		&color					 				// pointer to array containing color data 
		);

	assert(hBmpLine != NULL);

	// Output the RTF bitmap header
	oofRepStreamEnv::out() 	
	<< "\\pard {\\pict\\wbitmap0"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< endl;

	BITMAP bm;
	int nRetVal = ::GetObject(hBmpLine, sizeof(bm), &bm);
	assert(nRetVal != 0);

	int bmpSize = bm.bmWidthBytes * bm.bmHeight * bm.bmPlanes;

	BYTE* buf = new BYTE[bmpSize];
	DWORD dwRetVal = ::GetBitmapBits(hBmpLine, bmpSize, buf);
	assert(dwRetVal != 0);

	BYTE data;
	
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
	int wrap = 0;	// wrap every 60 words
	for( int i = 0; i < bmpSize; i++ ) {
		data = buf[i];

		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap = 0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	oofRepStreamEnv::out() << "} \\pard \\par" << endl;

	delete buf;

	BOOL bRetVal = ::DeleteObject(hBmpLine);
	assert(bRetVal);
*/
	unsigned long fixedHeight = mOwner->fixedHeight();
	
	// bodyWidth is in pixels
	oofRect theLine(0,0,twips(oofRep::currentReport()->bodyWidth()),twips(fixedHeight));

//	HDC hRefDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
//	HDC hMetaFile = ::CreateEnhMetaFile(hRefDC, NULL, theLine, NULL);
	HDC hMetaFile = ::CreateMetaFile(NULL);

	::MoveToEx(hMetaFile, 0, 0, NULL);
	::FillRect(hMetaFile, theLine, (HBRUSH)GetStockObject(GRAY_BRUSH));
//	HENHMETAFILE hEnhMeta = ::CloseEnhMetaFile(hMetaFile);
	HMETAFILE hMeta = ::CloseMetaFile(hMetaFile);

//	UINT cbBuffer = ::GetWinMetaFileBits(hEnhMeta, 0, NULL, MM_TEXT, hRefDC);
	UINT cbBuffer = ::GetMetaFileBitsEx(hMeta, 0, NULL);
	BYTE* buf = new BYTE[cbBuffer];
	assert(buf != NULL);

//	cbBuffer = ::GetWinMetaFileBits(hEnhMeta, cbBuffer, buf, MM_TEXT, hRefDC);
	cbBuffer = ::GetMetaFileBitsEx(hMeta, cbBuffer, buf);

//
	// Output the RTF metafile header
	oofRepStreamEnv::out() 	
	<< "\\pard {\\pict\\wmetafile1"
	<< "\\picw" << theLine.right
	<< "\\pich" << theLine.bottom
	<< endl;

	BYTE data;
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);

	int wrap = 0;	// wrap every 60 words
	for( unsigned int i = 0; i < cbBuffer; i++ ) {
		data = buf[i];

		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap = 0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	oofRepStreamEnv::out() << "} \\pard \\par" << endl;
//
	delete buf;
//	::DeleteEnhMetaFile(hEnhMeta);
//	::DeleteObject(hRefDC);
	::DeleteMetaFile(hMeta);
#endif
}

// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _R T F R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepSpaceBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_RTFRender((oofRepSpaceBand*)newOwner);	// safe downcast
}


void 
oofRepSpaceBand_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepSpaceBand_RTFRender::draw}\n";
#endif
	oofRepStreamEnv::out() << "\\pard \\sl-" << twips(mOwner->fixedHeight()) << "\n\\par \\pard" << endl;
// minus sign means exactly
}


// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _R T F R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepGraphBand_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_RTFRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepGraphBand_RTFRender::draw}\n";
#endif
#ifdef _Macintosh
	
	unsigned long graphHeight = mOwner->graphHeight();
	unsigned long graphWidth = mOwner->graphWidth();
	Rect theFrame;
	
	unsigned long width=oofRep::currentReport()->bodyWidth();
	if (graphWidth>width) {
// STUPID BUG		double shrinkBy = (graphWidth - width)/graphWidth;
		double shrinkBy = 1.0 - double(graphWidth - width)/graphWidth;
		graphWidth = width;
		graphHeight = graphHeight * shrinkBy;
	}
	oofRepStreamEnv::out() 	
		<< "\\pard \\qc {\\pict\\macpict"
		<< "\\picw" << graphWidth
		<< "\\pich" << graphHeight
		<< endl;
	
	oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
    SetRect(&theFrame,0,0,graphWidth,graphHeight);  // left, top, right, bottom - bloody SetRect!!!
    OffsetRect(&theFrame,(width-graphWidth)/2,0);


	OffsetRect(&theFrame,oofRep::currentReport()->settings()->leftMargin(),
						 oofRep::currentReport()->currentVerticalPos());
	
	RgnHandle	saveClipRgn = NewRgn();		// restores changes to current grafport
	
	GetClip( saveClipRgn );					// save current grafport
	mOwner->graph()->setPaneRect(theFrame);
	ClipRect(&theFrame);
	
	PicHandle hPic = OpenPicture(&theFrame);
	mOwner->graph()->draw();
	ClosePicture();
	
	SetClip( saveClipRgn );					// restore previous value
	DisposeRgn( saveClipRgn );				// not needed any more

	HLock( (Handle)hPic );				// don't go away now...
	
	// PICT format reference: Inside Macintosh: Imaging With QuickDraw A-24
	// read in bits from the handle
	unsigned char	opcode;
	unsigned char*	bitsPointer = (unsigned char *)(*hPic);
	
	// In case the pict's size is greater than 32K it is necessary to
	// use this method instead of grabbing the first 2 bytes.
	int				picSize = GetHandleSize( (Handle)hPic );
	
	// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ >= 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
	oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
	int		wrap=0; 				// wrap every 60 words
	for( int word=0; word < picSize; word++ ) {
		opcode = *bitsPointer;
		bitsPointer++;
		
		oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)opcode;

		wrap++;
		if( wrap >= 60 ) {
			oofRepStreamEnv::out() << endl;
			wrap=0;
		}
	}

	// Restore the stream to it's old characteristics
	oofRepStreamEnv::out().flags( oldFlags );

	// Should be 0xFF for End-Of-Pic opcode
	assert( opcode == 0xFF );
	
	HUnlock( (Handle)hPic );

	oofRepStreamEnv::out() << "}\\par " << endl;

#elif defined _Windows
/* old crap
	unsigned long width=oofRep::currentReport()->bodyWidth();
	if (graphWidth>width) {
		double shrinkBy = 1.0 - double(graphWidth - width)/graphWidth;
		graphWidth = width;
		graphHeight = graphHeight * shrinkBy;
	}
	
	oofRect theFrame;

    theFrame.set(0,0,graphWidth,graphHeight);
    theFrame.offset((width-graphHeight)/2, 0);
	theFrame.offset(oofRep::currentReport()->settings()->leftMargin(),
						 oofRep::currentReport()->currentVerticalPos());
*/
	const unsigned long fixedHeight = mOwner->fixedHeight();	
    const unsigned long bodyWidth = oofRep::currentReport()->bodyWidth();
	const unsigned long graphWidth = mOwner->graphWidth();
	const unsigned long graphHeight = mOwner->graphHeight();
	
    assert(mOwner->graphWidth()>0);
	unsigned long graphLeft, graphRight, graphBottom, graphTop;
	if (bodyWidth > graphWidth) {
		graphLeft = (bodyWidth-graphWidth)/2;
		graphRight = graphWidth+graphLeft;
	}
	else {
		graphLeft = 0;
		graphRight = bodyWidth;
	}
	if (fixedHeight > graphHeight) {
  		graphTop = (fixedHeight-graphHeight)/2;
		graphBottom = graphTop+graphHeight;
	}
	else {
		graphTop = 0;
		graphBottom = fixedHeight;
	}
	oofRect theFrame(graphLeft,graphTop,graphRight ,graphBottom);
//	const bool usingWin32s = oofRuntimeEnv::runtimeEnv()==oofRuntimeEnv::Win32s;
//	if (usingWin32s) 
	{
		// create the standard in-memory metafile
		HDC hMetaFile = ::CreateMetaFile(NULL);
		oofCurrentDraw::setCurrentDC(hMetaFile);
	
		::MoveToEx(hMetaFile, 0, 0, NULL);
	// redraw the graph to create this metafile
//		::SetMapMode(hMetaFile, MM_TWIPS);
	
		oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
		mOwner->graph()->setPaneRect(theFrame);
		mOwner->graph()->draw();	
		HMETAFILE hMeta = ::CloseMetaFile(hMetaFile);
		oofCurrentDraw::setCurrentDC(0);
	
		UINT cbBuffer = ::GetMetaFileBitsEx(hMeta, 0, NULL);
		BYTE* buf = new BYTE[cbBuffer];
		assert(buf != NULL);
		cbBuffer = ::GetMetaFileBitsEx(hMeta, cbBuffer, buf);
	
		const unsigned long outWidth = theFrame.right;
		const unsigned long outHeight = theFrame.bottom;
		const unsigned long outGoalWidth = theFrame.right;
		const unsigned long outGoalHeight = theFrame.bottom;
	
	// Output the RTF metafile header
		oofRepStreamEnv::out() 	
//		<< "\\pard {\\pict\\wmetafile8"  // MM_ANISOTROPIC
		<< "\\pard {\\pict\\wmetafile1"  // MM_TEXT
		<< "\\picw" << outWidth
		<< "\\pich" << outHeight
		<< "\\picwgoal" << twips(outGoalWidth)
		<< "\\pichgoal" << twips(outGoalHeight)
		<< endl;

		BYTE data;
		// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
		oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
		int wrap = 0;	// wrap every 60 words
		for( unsigned int i = 0; i < cbBuffer; i++ ) {
			data = buf[i];
	
			oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;
	
			wrap++;
			if( wrap >= 60 ) {
				oofRepStreamEnv::out() << endl;
				wrap = 0;
			}
		}
	
		// Restore the stream to it's old characteristics
		oofRepStreamEnv::out().flags( oldFlags );
	
		oofRepStreamEnv::out() << "} \\pard \\par" << endl;
	//
		delete buf;
		::DeleteMetaFile(hMeta);
	}  // win32s so use old metafile
#if 0
	else {
		HDC hRefDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
		HDC hMetaFile = ::CreateEnhMetaFile(hRefDC, NULL, theFrame, NULL);
		oofCurrentDraw::setCurrentDC(hMetaFile);
	
		::MoveToEx(hMetaFile, 0, 0, NULL);
	// redraw the graph to create this metafile
		oofRep::currentReport()->changeTextStyle(mOwner->textStyle());
		mOwner->graph()->setPaneRect(theFrame);
		mOwner->graph()->draw();	
	
		HENHMETAFILE hEnhMeta = ::CloseEnhMetaFile(hMetaFile);
		oofCurrentDraw::setCurrentDC(0);
	
		UINT cbBuffer = ::GetWinMetaFileBits(hEnhMeta, 0, NULL, MM_TEXT, hRefDC);
		BYTE* buf = new BYTE[cbBuffer];
		assert(buf != NULL);
	
		cbBuffer = ::GetWinMetaFileBits(hEnhMeta, cbBuffer, buf, MM_TEXT, hRefDC);

	// Output the RTF metafile header
		ENHMETAHEADER hMetaHeader;
		UINT nRetVal = ::GetEnhMetaFileHeader(hEnhMeta, sizeof(ENHMETAHEADER), &hMetaHeader);
		const unsigned long outWidth = hMetaHeader.rclBounds.right;
		const unsigned long outHeight = hMetaHeader.rclBounds.bottom;
		const unsigned long outGoalWidth = hMetaHeader.rclFrame.right;
		const unsigned long outGoalHeight = hMetaHeader.rclFrame.bottom;

		oofRepStreamEnv::out() 	
		<< "\\pard {\\pict\\wmetafile1"  // MM_TEXT
//		<< "\\pard {\\pict\\wmetafile3"  // MM_LOMETRIC
		<< "\\picw" << outWidth
		<< "\\pich" << outHeight
//		<< "\\picwgoal" << outGoalWidth
//		<< "\\pichgoal" << outGoalHeight
		<< endl;

		BYTE data;
		// Save the streams old characteristics for restoration later
#if defined __MWERKS__  & __MWERKS__ == 0x2301	
	std::ios_base::fmtflags oldFlags;
#else
	int oldFlags;
#endif
		oldFlags = oofRepStreamEnv::out().flags(ios::right|ios::hex|ios::uppercase);
	
		int wrap = 0;	// wrap every 60 words
		for( unsigned int i = 0; i < cbBuffer; i++ ) {
			data = buf[i];
	
			oofRepStreamEnv::out() << setw(2) << setfill('0') << (unsigned short)data;
	
			wrap++;
			if( wrap >= 60 ) {
				oofRepStreamEnv::out() << endl;
				wrap = 0;
			}
		}
	
		// Restore the stream to it's old characteristics
		oofRepStreamEnv::out().flags( oldFlags );
	
		oofRepStreamEnv::out() << "} \\pard \\par" << endl;
	//
		delete buf;
		::DeleteEnhMetaFile(hEnhMeta);
		::DeleteObject(hRefDC);
	}  // output enhanced
#endif

/* INFO FROM RTF STANDARD VERSION 1.3, found from
MSDN library Product Documentation, Applications, Word for Windows 6.0

Picture Size, Scaling, and Cropping

\ picwN	xExt field if the picture is a Windows metafile; picture width in pixels if the picture is a bitmap or from QuickDraw.
\ pichN	yExt field if the picture is a Windows metafile; picture height in pixels if the picture is a bitmap or from QuickDraw.
\ picwgoalN	Desired width of the picture in twips.
\ pichgoalN	Desired height of the picture in twips.
\ picscalexN	Horizontal scaling value; the N argument is a value representing a percentage (the default is 100).
\ picscaleyN	Vertical scaling value; the N argument is a value representing a percentage (the default is 100).
\ picscaled	Scales the picture to fit within the specified frame; used only with \ macpict pictures.
\ piccroptN	Top cropping value in twips; a positive value crops toward the center of the picture; a negative value crops away from the center, adding a space border around picture (the default is 0).
\ piccropbN	Bottom cropping value in twips; a positive value crops toward the center of the picture; a negative value crops away from the center, adding a space border around picture (the default is 0).
\ piccroplN	Left cropping value in twips; a positive value crops toward the center of the picture; a negative value crops away from the center, adding a space border around picture (the default is 0).
\ piccroprN	Right cropping value in twips; a positive value crops toward the center of the picture; a negative value crops away from the center, adding a space border around picture (the default is 0).


Trailing digit on wmetafile indicates mapping mode
 Type	N argument

MM_TEXT	1
MM_LOMETRIC	2
MM_HIMETRIC	3
MM_LOENGLISH	4
MM_HIENGLISH	5
MM_TWIPS	6
MM_ISOTROPIC	7
MM_ANISOTROPIC	8

After standard metafile editing in Word, something like
\wmetafile8\picw12581\pich9245\picwgoal7133\pichgoal5241
*/
#endif
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _R T F R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepTextBlock_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_RTFRender((oofRepTextBlock*)newOwner);	// safe downcast
}


void
oofRepTextBlock_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepTextBlock_RTFRender::draw}\n";
#endif
	//oofRepStreamEnv::out() << mOwner->text();
	oofRepRTFEnv::encodeToRTFStream(oofRepStreamEnv::out(), mOwner->text());
}


// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _R T F R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_RTFRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_RTFRender((oofRepPictBlock*)newOwner);	// safe downcast
}


void
oofRepPictBlock_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepPictBlock_RTFRender::draw}\n";
#endif
	oofRepStreamEnv::out() << "";		//***********
}

// -------------------------------------------------------
//  o o f R e p P a g e N o B l o c k _R T F R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPageNoBlock_RTFRender::clone(const oofRepDrawable* newOwner)const
{
	return new oofRepPageNoBlock_RTFRender((oofRepPageNoBlock*)newOwner);	// safe downcast
}


void
oofRepPageNoBlock_RTFRender::draw()
{
#ifdef OOF_DEBUG
	oofRepStreamEnv::out() << "{\\comment oofRepPageNoBlock_RTFRender::draw}\n";
#endif
	oofRepStreamEnv::out() << mOwner->leadingText() <<"\\chpgn " << mOwner->trailingText();
	
// DON'T WANT ACTUAL PAGE NUMBERS HARDCODED, AS WORD DOES ITS OWN BREAKS
//	char pn[10];
//	sprintf(pn,"%d",oofRep::currentReport()->pageNumber());
	
//	oofRepStreamEnv::out() << pn << mOwner->trailingText();
}

