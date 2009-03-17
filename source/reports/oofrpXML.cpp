// COPYRIGHT 1998 A.D. Software, All rights reserved

// report-writer layer of OOFILE database
// renderer factories & renderers for XML output

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"  // knows a bit about fields
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef H_OOFRPXML
	#include "oofrpXML.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFRAM
	#include "oofram.h"
#endif
#ifndef H_OOFXML
	#include "oofxml.h"
#endif

#if OOF_USE_ANSI_STREAMS
	#include <iomanip>
#else
	#include <iomanip.h>
#endif

#include <stdio.h>  // for tmpnam

#ifdef __MWERKS__
	#if __option(mpwc_newline)
		 #pragma message("you are supposed to build this with 'Map newline to CR' unchecked!")
	#endif
#endif


// -------------------------------------------------------
//   O O F _ r e p X M L s t y l e V i s i t o r
// -------------------------------------------------------
/**
	Used by oofRepXMLEnv::StartReport to visit all stylable items.
	Creates strings in CSS format inside \<style\> tag.
	\see oofGraphSettings2CSS::emitCSSstyle
	\see oofRepTextStyle2CSS::emitCSSstyle
	\warning MUST be run before the layout visitor, to assign the ID's
	\ingroup oofRenderXML
*/
class OOF_repXMLstyleVisitor : public oofRepVisitor {
public:
	OOF_repXMLstyleVisitor(oofIDfactory& identities) : 
		mIdentities(identities),
		mIndent(mIdentities.getIndentString() ),
		mSetThisIDanyway(false),
		mAdornersEncountered(false)
	{};
	
	bool adornersEncountered() const;
	
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
	virtual void VisitViewBandBody(oofRepViewBand*);
	virtual void EnterLevel();
	virtual void LeaveLevel();

	oofIDfactory& mIdentities;
	const oofString mIndent;  // use const indent otherwise style strings zig-zag 
	bool mSetThisIDanyway, mAdornersEncountered;
};


inline bool
OOF_repXMLstyleVisitor::adornersEncountered() const
{
	return mAdornersEncountered;
}


void
OOF_repXMLstyleVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
// IMPORTANT - this sets the ID referred to later by layout visitors
// so must be called before we visit to construct layouts

// set the ID only if we write a style string
// but calculate it regardless
	const oofString newID = mIdentities.getIDstring();
	mIdentities.incrementID();
	
	const bool drawableHasAdorners = inDrawable->hasAdorners();
	if (drawableHasAdorners)
		mAdornersEncountered = true;

	// check if graph band and write settings string in styles
	// pretty gross hack!!
#ifndef OOF_RTTI_ENABLED
	error - RTTI required
#endif
	oofRepGraphBand* theGrapher = dynamic_cast<oofRepGraphBand*>(inDrawable);
	if (theGrapher) {
		inDrawable->ID(newID);
		oofRepStreamEnv::out() 
			<< mIndent << mIndent  // double-indent all style strings
			<< '#' << newID
			<< oofGraphSettings2CSS::emitCSSstyle(theGrapher->graph()->settings())
			<< '\n';
		return;  // ****** EARLY EXIT after graph special case
	}

	oofRepTextStyle* theStyle = inDrawable->localTextStyle();
	if (theStyle) {
		inDrawable->ID(newID);
// write ID and style
		oofRepStreamEnv::out() 
			<< mIndent << mIndent  // double-indent all style strings
			<< '#' << newID
			<< oofRepTextStyle2CSS::emitCSSstyle(*theStyle)
			<< '\n';
	}
	else if (mSetThisIDanyway || drawableHasAdorners) {
		mSetThisIDanyway = false;
		inDrawable->ID(newID);
	}
}


void 
OOF_repXMLstyleVisitor::VisitViewBandBody(oofRepViewBand* inBand)
{
	mSetThisIDanyway = true;
	oofRepVisitor::VisitViewBandBody(inBand);
}


void
OOF_repXMLstyleVisitor::EnterLevel()
{
	mIdentities.enterLevel();
}


void
OOF_repXMLstyleVisitor::LeaveLevel()
{
	mIdentities.leaveLevel();
}


// -------------------------------------------------------
//   O O F _ X M L s c h e m a V i s i t o r
// -------------------------------------------------------
/**
	Used by oofRepXMLEnv::StartReport to visit all report items that may be linked to database to add schema entries.
	\ingroup oofRenderXML
*/
class OOF_repXMLschemaVisitor : public oofRepSchemaVisitor {
public:
	OOF_repXMLschemaVisitor(oofIDfactory& identities) : 
		mIdentities(identities)
	{};
	
protected:
	virtual void VisitViewBand(oofRepViewBand*);
	virtual void VisitView(dbView*); 
	
	void UniquifyTables(dbView*);

// data storage	
	oofIDfactory& mIdentities;
	OOF_Dictionary mKnownTables;
	OOF_DummyString	mDummyNames;	// statically stored self-incrementing const char*'s
};


void
OOF_repXMLschemaVisitor::VisitView(dbView* inView)
{
// default for all other bands which are presumed to have non-nested schemae
	UniquifyTables(inView);
	oofXMLDBWriter schemaWriter(oofRepStreamEnv::out(), inView, mIdentities);		
	schemaWriter.startSchema();
	schemaWriter.endSchema();
}


void
OOF_repXMLschemaVisitor::VisitViewBand(oofRepViewBand* inBand)
{
	dbView* theView = inBand->view();
	UniquifyTables(theView);
	oofXMLDBWriter schemaWriter(oofRepStreamEnv::out(), theView, mIdentities);
// within the boundary of this band's view, may have nested data
// hence the split start/endSchema	

// any views attached to headers or footers are OUTSIDE the data of
// the main view so not directly related to it

// the following structural navigation is a modified copy of oofRepVisitor's method
// so needs updating if change report
	mRep->incrementViewLevel();	
	if (inBand->headers().count()>0)
		VisitViewHeaders(inBand->headers());
		
	schemaWriter.startSchema();

// data in breaks is related to a given record so nested inside the startSchema
	if (inBand->headerBreaks().count()>0)
			VisitViewHeaderBreaks(inBand->headerBreaks());
			
// don't visit Body as we're accomplishing that with the start/endSchema here
	
	if (inBand->footerBreaks().count()>0)
			VisitViewFooterBreaks(inBand->footerBreaks());
			
	schemaWriter.endSchema();

	if (inBand->footers().count()>0)
			VisitViewFooters(inBand->footers());
	mRep->decrementViewLevel();	
}





void
OOF_repXMLschemaVisitor::UniquifyTables(dbView* inView)
{
	dbTable* viewTable = inView->table();
	if (viewTable) {
		const unsigned long numTablesSeen = mKnownTables.count();
		if (numTablesSeen>0) {  // don't waste time with lookup on first table
			oofString currentName = viewTable->tableName();
			dbTable* knownTable = (dbTable*) mKnownTables[currentName];  // safe downcast
			if (knownTable) {  
			// table may be encountered several times depending on how dbViews established
			// particularly in multiple report, so clone it!
				if (knownTable==viewTable) {
					inView->cloneTable();  // tell the view to clone its table, do fixups
					viewTable = inView->table();
					assert(knownTable!=viewTable);  // it better be different by now!
				}
			// make name unique by simply appending a rising count
				currentName.convertNumber(numTablesSeen, 0, true);  // append
				viewTable->setName(currentName);
			}
		}
		mKnownTables.append(viewTable);
	}
	else {  // set name of view so it can be retrieved later
		if (inView->name().isEmpty()) {  // may have name set by user
			inView->name( mDummyNames.nextDummy() );
		}
	}
	// now fix any empty field names!
	// note that if this causes a field to appear with a name above its column that was
	// not wanted, setHeading can be used on the view to set a null overriding heading
	const unsigned int numflds = inView->count();	
	for( unsigned int i=0; i < numflds; i++ ) {
		dbField* fld = inView->writeableField(i);
		if (fld->fieldName().isEmpty()) {
			fld->setName( mDummyNames.nextDummy() );
		}
	}
}


// -------------------------------------------------------
//   O O F _ X M L l a y o u t V i s i t o r
// -------------------------------------------------------
/**
	Used by oofRepXMLEnv::StartReport to visit all report items and describe them in \<layout\>.
	This is the most significant of the visitors - it writes the information crucial to recreating a report.
	\note we overload the term "layout" where this refers to the overall layout of a report 
	and has nothing to do with the use in oofRepLayoutBand or its associated render classes.
	In that latter case, "layout" refers to layout of fixed sized blocks within a band.
	\ingroup oofRenderXML
*/
class OOF_repXMLlayoutVisitor : public oofRepVisitor {
public:
	OOF_repXMLlayoutVisitor(oofIDfactory& identities) : 
		mIdentities(identities)
	{};
	
protected:
	virtual void VisitFirstPageHeaders(oofRepBandList&);
	virtual void VisitFirstPageFooters(oofRepBandList&);
	virtual void VisitPageHeaders(oofRepBandList&);
	virtual void VisitPageFooters(oofRepBandList&);
	virtual void VisitViewHeaders(oofRepBandList&);
	virtual void VisitViewFooters(oofRepBandList&);
	virtual void VisitViewHeaderBreaks(oofRepBreakList&);
	virtual void VisitViewFooterBreaks(oofRepBreakList&);

// core visit methods
	virtual void VisitBandList(oofRepBandList&);
	virtual void VisitBreakList(oofRepBreakList&);
	virtual void VisitViewBand(oofRepViewBand*);
	virtual void VisitLayoutBand(oofRepLayoutBand*);
	virtual void VisitDrawable(oofRepDrawable*); 

	void WriteColumns(oofRepViewBand*);
	
// data storage	
	oofIDfactory& mIdentities;
};


void
OOF_repXMLlayoutVisitor::VisitFirstPageHeaders(oofRepBandList& inBands)
{
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "<header where='first'>\n";
	VisitBandList(inBands);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</header>\n";
}


void
OOF_repXMLlayoutVisitor::VisitFirstPageFooters(oofRepBandList& inBands)
{
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "<footer where='first'>\n";
	VisitBandList(inBands);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</footer>\n";
}


void
OOF_repXMLlayoutVisitor::VisitPageHeaders(oofRepBandList& inBands)
{
	VisitViewHeaders(inBands);		// uses same tags for page headers as view headers
}


void
OOF_repXMLlayoutVisitor::VisitPageFooters(oofRepBandList& inBands)
{
	VisitViewFooters(inBands);		// uses same tags for page footers as view footers
}


void
OOF_repXMLlayoutVisitor::VisitViewHeaders(oofRepBandList& inBands)
{
	oofRepStreamEnv::out() << mIdentities.getIndentString();
	if (inBands.keepTogether())
		oofRepStreamEnv::out()	<< "<header>\n";	
	else  // default is ON, and simplifies XML
		oofRepStreamEnv::out()	<< "<header keepTogether='false'>\n";
	VisitBandList(inBands);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</header>\n";
}


void
OOF_repXMLlayoutVisitor::VisitViewFooters(oofRepBandList& inBands)
{
	oofRepStreamEnv::out() << mIdentities.getIndentString();
	if (inBands.keepTogether())
		oofRepStreamEnv::out()	<< "<footer>\n";	
	else  // default is ON, and simplifies XML
		oofRepStreamEnv::out()	<< "<footer keepTogether='false'>\n";
	VisitBandList(inBands);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</footer>\n";
}


void
OOF_repXMLlayoutVisitor::VisitViewHeaderBreaks(oofRepBreakList& inBreaks)
{
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "<headerBreaks>\n";
	VisitBreakList(inBreaks);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</headerBreaks>\n";
}


void
OOF_repXMLlayoutVisitor::VisitViewFooterBreaks(oofRepBreakList& inBreaks)
{
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "<footerBreaks>\n";
	VisitBreakList(inBreaks);
	oofRepStreamEnv::out() 
		<< mIdentities.getIndentString()
		<< "</footerBreaks>\n";
}


void
OOF_repXMLlayoutVisitor::VisitBandList(oofRepBandList& inBands)
{
	mIdentities.enterLevel();
	oofRepVisitor::VisitBandList(inBands);
	mIdentities.leaveLevel();
}


void
OOF_repXMLlayoutVisitor::VisitBreakList(oofRepBreakList& inBreaks)
{
	mIdentities.enterLevel();
	const unsigned long numBreaks = inBreaks.count();
	for (unsigned long i=0; i<numBreaks; i++) {
		oofRepBreak* theBreak = inBreaks.value(i);
		assert(theBreak);
		oofRepBand* theBand = theBreak->band();

		oofRepStreamEnv::out() << mIdentities.getIndentString();
		if (theBreak->newPage())
			oofRepStreamEnv::out() << "<break newPage='yes'>\n";
		else	
			oofRepStreamEnv::out() << "<break>\n";
		if (theBand) {
			mIdentities.enterLevel();
				VisitTypedBand(theBand);
			mIdentities.leaveLevel();
		}  // break without band

		oofRepStreamEnv::out() 
			<< mIdentities.getIndentString()
			<< "</break>\n";

	}  // loop breaks
	mIdentities.leaveLevel();
}


void
OOF_repXMLlayoutVisitor::VisitViewBand(oofRepViewBand* inBand)
{
	dbView* theView = inBand->view();
	const oofString attribString( "source=", oofString::quoteString(oofTagMaker::encodeName( theView->name() )) );
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeOpeningTag(
				"table", 
				mIdentities.getIndentString(), 
				inBand->ID(),
				attribString
			)
		<< '\n';

	mIdentities.enterLevel();
		WriteColumns(inBand);
		oofRepVisitor::VisitViewBand(inBand);
	mIdentities.leaveLevel();
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeClosingTag("table", mIdentities.getIndentString());
}


void
OOF_repXMLlayoutVisitor::WriteColumns(oofRepViewBand* inBand)
{
	dbView* theView = inBand->view();
	assert(theView);
	oofColSizer* theSizer = theView->colSizer();
	const oofString colIndent = mIdentities.getIndentString();
	
	const unsigned long numCols = theView->count();
	for (unsigned long col=0; col<numCols; col++) {
		oofString attribString;
		attribString.convertNumber(col, "number='%lu'"); // identify column by attribute

		oofRepCustomViewDrawer* customDrawer = inBand->customDrawer(col);
		bool writeColElement = (customDrawer!=0);
		
		bool overriddenHeading;
		const oofString colHeading = theView->getHeading(col, &overriddenHeading);
		writeColElement = writeColElement || overriddenHeading;  // may have returned empty heading - user suppressing it!
		
		const unsigned long colFixedWidth = theSizer->fixedWidth(col);
		const unsigned long colFixedPercentage = theSizer->fixedPercentage(col);
		const bool writeWidth = ((colFixedWidth!=oofColSizer::kNotSet) || (colFixedPercentage!=oofColSizer::kNotSet));
		
		writeColElement = writeColElement || writeWidth;
		
		const oofColSizer::colAlignT colAlignment = theSizer->alignment(col);
		
		if (colAlignment!=oofColSizer::alignNatural) {
			const oofColSizer::colAlignT colNaturalAlignment =	oofColSizer::naturalAlignmentForField(theView->field(col).fieldType());
			// check if they set it to natural for type without realising
			if (colAlignment!=colNaturalAlignment) {
			// use an attribute for alignment because it's not content
			// and to match the attribute we use elsewhere 
				writeColElement = true;
				switch(colAlignment){
					case oofColSizer::alignRight:
						attribString += " align='right'";
						break;
						
					case oofColSizer::alignCentre:
						attribString += " align='centre'";
						break;
						
					case oofColSizer::alignJust:
						attribString += " align='justify'";
						break;
						
					default: 
						attribString += " align='left'";
						break;
				}
			}  // alignment that's set doesn't match natural
		}  // un-natural alignment

		if (writeColElement) {
			oofRepStreamEnv::out() 
				<<  oofTagMaker::makeOpeningTag(
						"column", 
						colIndent, 
						0,  /* no ID as yet, will need if ever set text styles for column bodies */
						attribString
					)
				<< '\n';
			mIdentities.enterLevel(); {
				const oofString colContentIndent = mIdentities.getIndentString();
				if (!colHeading.isEmpty()) {
					oofRepStreamEnv::out() <<  oofTagMaker::makeSimpleBoundedElement(
						colHeading, 
						"header", 
						colContentIndent
					);/* no ID as yet, will need if ever set text styles for headings */
				} // write a heading
				
				if (writeWidth) {
					char widthStr[32];
					if (colFixedWidth!=oofColSizer::kNotSet)
						sprintf(widthStr, "%lu", colFixedWidth);
					else
						sprintf(widthStr, "%lu%%", colFixedPercentage); // include actual percent sign
					
					oofRepStreamEnv::out() <<  oofTagMaker::makeSimpleBoundedElement(
						widthStr, 
						"width", 
						colContentIndent
					);    // no ID as yet, will need if every set text styles per col 

				}  // write width
				
				if (customDrawer) {
					oofRepStreamEnv::out() <<  oofSerializable::makeSerializableElement(
						customDrawer, 
						"customDrawer", 
						&mIdentities
					);    
					
				}  // write customDrawer
					
			}
			mIdentities.leaveLevel();
			oofRepStreamEnv::out() 
				<<  oofTagMaker::makeClosingTag("column", colIndent);
		}  // some reason to write element
	} // loop cols
}

void 
OOF_repXMLlayoutVisitor::VisitLayoutBand(oofRepLayoutBand* inBand)
{
	const oofString layoutIndent = mIdentities.getIndentString();
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeOpeningTag(
				"box", 
				layoutIndent, 
				inBand->ID()
			)
		<< '\n';

	mIdentities.enterLevel();
		oofRepVisitor::VisitLayoutBand(inBand);
	mIdentities.leaveLevel();
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeClosingTag("box", layoutIndent);
}


void 
OOF_repXMLlayoutVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	inDrawable->describeLayout();
}




// -------------------------------------------------------
//   O O F _ r e p X M L a d o r n e r V i s i t o r
// -------------------------------------------------------
/**
	Used by oofRepXMLEnv::StartReport to visit all adornable items to serialise adorner descriptions.
	Users may provide their own adorners and have them serialised here and instantiated
	when the report is parsed. Each serialised adorner is responsible for encoding its
	own description as XML-compatible text.
	The same mechanism is used for our standard adorners such as oofBorderAdorner.
	\see oofSerializable
	\ingroup oofRenderXML
*/
class OOF_repXMLadornerVisitor : public oofRepVisitor {
// MUST be run before the layout visitor, to assign the ID's
public:
	OOF_repXMLadornerVisitor(oofIDfactory& identities) : 
		mIdentities(identities)
	{};
	
	
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
	virtual void VisitAdorner(oofAdorner*);

	oofIDfactory& mIdentities;
	oofString mParentIDattrib;
};


void 
OOF_repXMLadornerVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	const oofString drawID = inDrawable->ID();
	if (!drawID.isEmpty()) {	
		mParentIDattrib = oofString(" parentID = ", oofString::quoteString(drawID));
		VisitAdornerList(inDrawable->adorners());
	}
}


void
OOF_repXMLadornerVisitor::VisitAdorner(oofAdorner* inAdorner)
{
	oofRepStreamEnv::out() <<  oofSerializable::makeSerializableElement(
		inAdorner, 
		"adorner", 
		&mIdentities,
		0,  // no ID for adorner itself
		mParentIDattrib
	);    
}


// -------------------------------------------------------
//   o o f R e p X M L s e t t i n g s
// -------------------------------------------------------
oofRepXMLsettings* 
oofRepXMLsettings::clone()
{
	return new oofRepXMLsettings(*this);
}
	

// if subclassed to create different env which is subclass of oofRepXMLEnv,
// the following two constructors are where your class name would appear
oofRepXMLEnv* 
oofRepXMLsettings::makeXMLenv(ostream& os)
{
	return new oofRepXMLEnv(os);  
}
	

oofRepXMLEnv* 
oofRepXMLsettings::makeXMLenv(const char* filename)
{
	return new oofRepXMLEnv(filename);
}


dbConnect* 
oofRepXMLsettings::makeTempConnection()
{
	return new dbConnect_ram;
}


void 
oofRepXMLsettings::createTempConnectFiles(dbConnect* inConnect)
{
// default location is current dir
	char name[L_tmpnam];
	tmpnam( name );
	inConnect->newConnection(name);
}


oofString 
oofRepXMLsettings::saveReportDocDescription()
{
	return oofString("Report Document");
}


oofString 
oofRepXMLsettings::saveReportDocFileExt()
{
	return oofString("xml");
}



unsigned long 
oofRepXMLsettings::saveReportDocMacFileType()
{
	return 'TEXT';
}





// -------------------------------------------------------
//    o o f G r a p h S e t t i n g s 2 C S S 
// -------------------------------------------------------
oofString
oofGraphSettings2CSS::emitCSSstyle(const oofGraphSettings* aGraphSetting)
{
	char scrapString[256];

	oofString styleRun ="{";
	if(aGraphSetting->hasLegend())
		styleRun += " hasLegend: true; ";
	else
		styleRun += " hasLegend: false; ";
	
	if(aGraphSetting->has3DEffect())
		styleRun += " has3DEffect: true; ";
	else
		styleRun += " has3DEffect: false; ";
	
	if(aGraphSetting->hasTitle())
		styleRun += "hasTitle: true; ";
	else
		styleRun += "hasTitle: false; ";
	
	if(aGraphSetting->hasValuesOverColumns())
		styleRun += "hasValuesOverColumns: true; ";
	else
		styleRun += "hasValuesOverColumns: false; ";
	
	if(aGraphSetting->hasValuesNextToBars())
		styleRun += "hasValuesNextToBars: true; ";
	else
		styleRun += "hasValuesNextToBars: false; ";
	
	if(aGraphSetting->hasXLabel())
		styleRun += "hasXLabel: true; ";
	else
		styleRun += "hasXLabel: false; ";
	
	if(aGraphSetting->hasYLabel())
		styleRun += "hasYLabel: true; ";
	else
		styleRun += "hasYLabel: false; ";
	
	if(aGraphSetting->hasXAxisTicks())
		styleRun += "hasXAxisTicks: true; ";
	else
		styleRun += "hasXAxisTicks: false; ";
	
	if(aGraphSetting->hasYAxisTicks())
		styleRun += "hasYAxisTicks: true; ";
	else
		styleRun += "hasYAxisTicks: false; ";
	
	if(aGraphSetting->hasXTickLabels())
		styleRun += "hasXTickLabels: true; ";
	else
		styleRun += "hasXTickLabels: false; ";
	
	if(aGraphSetting->hasYTickLabels())
		styleRun += "hasYTickLabels: true; ";
	else
		styleRun += "hasYTickLabels: false; ";

	styleRun += "titlePos: ";
	styleRun += MakePosStateString(aGraphSetting->titlePos());
	
	styleRun += "XLabelPos: ";
	styleRun += MakePosStateString(aGraphSetting->XLabelPos());
	
	styleRun += "YLabelPos: ";
	styleRun += MakePosStateString(aGraphSetting->YLabelPos());

	sprintf(scrapString,"the3DEffectMultiplier: %1.3f ; ",aGraphSetting->the3DEffectMultiplier());
	styleRun += scrapString;

	sprintf(scrapString,"legendOffset: %d; ",aGraphSetting->legendOffset());
	styleRun += scrapString;

	sprintf(scrapString,"maxYLabelLen: %d ; ",aGraphSetting->maxYLabelLen());
	styleRun += scrapString;

	sprintf(scrapString,"maxYTickLabelLen: %d ; ",aGraphSetting->maxYTickLabelLen());
	styleRun += scrapString;

	sprintf(scrapString,"maxLegendLen: %d ; ",aGraphSetting->maxLegendLen());
	styleRun += scrapString;

	sprintf(scrapString,"minorTickSize: %d ; ",aGraphSetting->minorTickSize());
	styleRun += scrapString;

	sprintf(scrapString,"majorTickSize: %d ; ",aGraphSetting->majorTickSize());
	styleRun += scrapString;

	sprintf(scrapString,"YAxisHeight: %d ; ",aGraphSetting->YAxisHeight());
	styleRun += scrapString;

	styleRun += "intTickLabelMask: '";
	styleRun += aGraphSetting->intTickLabelMask();
	styleRun += "' ;";

	styleRun += "realTickLabelMask: '";
	styleRun += aGraphSetting->realTickLabelMask();
	styleRun += "'  }";
	
	return styleRun;
}


oofString
oofGraphSettings2CSS::MakePosStateString(oofGeo::PositionState thePositionState)
{
// see oofCSS2GraphSettings::ParsePosString in oofriXML.cpp

	oofString thePositionStr;

	switch(thePositionState){
	case oofGeo::posNone:
		thePositionStr +="posNone ;";
	break;
	case oofGeo::posTop:
		thePositionStr +="posTop ;";
	break;
	case oofGeo::posLeft:
		thePositionStr +=	"posLeft ;";
	break;
	case oofGeo::posRight:
		thePositionStr +=	"posRight ;";
	break;
	case oofGeo::posBottom:
		thePositionStr +=	"posBottom ;";
	break;
	case oofGeo::posBelow:
		thePositionStr +=	"posBelow ;";
	break;
	case oofGeo::posToLeft:
		thePositionStr +=	"posToLeft ;";
	break;
	case oofGeo::posCentre:
		thePositionStr +=	"posCentre ;";
	break;
	}
	return thePositionStr;
}	


// -------------------------------------------------------
//    o o f R e p T e x t S t y l e 2 C S S 
// -------------------------------------------------------
oofString
oofRepTextStyle2CSS::emitCSSstyle(const oofRepTextStyle& aStyleObject)
{
	unsigned short fontSize;
	short leading;
	unsigned char style;
	float lineHeight;		

	const char * tempString;
	bool FontNameHasASpace = false;
	oofString theStyleRun;
	
	char scrapString[256];
	
	// start building the style
	theStyleRun += "{";
	
	//theStyleRun += "{ " to the styleString
	//set FontFamily to fontName()
	// now add font-family
	theStyleRun += "font-family: ";
	//check for spaces
	tempString = aStyleObject.fontName();
	if(strstr(tempString," ")){
		FontNameHasASpace = true;
		theStyleRun += "'"; 
	}
		
	theStyleRun += aStyleObject.fontName();
	theStyleRun += "; " ;
	if(FontNameHasASpace)
		theStyleRun += "'" ;
	
	// fontsize
	fontSize = aStyleObject.fontSize();
	leading =  aStyleObject.leading(); 
	lineHeight = (fontSize+leading);
	lineHeight = lineHeight/fontSize;
	sprintf(scrapString,"line-height: %1.3f ; ",lineHeight);
	//convertlineheight to a str and stick into Astr
	//theStyleRun += "line-height:  " ast "; "
	theStyleRun +=  scrapString;
	sprintf(scrapString,"font-size:  %d ; ",fontSize);
	theStyleRun +=  scrapString;
	
	//convertFontSize to a str and stick into Astr
	//theStyleRun += :"font-size: "aStr" "pt ;"
	style = aStyleObject.fontStyle();
	makeStyleStrings(theStyleRun, aStyleObject.fontStyle());
	//
	theStyleRun +=  "color: rgb";
	ostrstream colorString;
	aStyleObject.color().extract(colorString);
	oofString colorWeights =  colorString;
	theStyleRun += colorWeights;
	theStyleRun += "; ";
	theStyleRun += makeAlignmentString( aStyleObject.alignment());
	
	theStyleRun += "}";	
	return theStyleRun;
}

	
void
oofRepTextStyle2CSS::makeStyleStrings(oofString& theStyleRun, unsigned char textStyleNumber)
{
	//function makeStyleStrings
	//Passed a styleNumber unsigned char
	//Returns an oofstr
	//theStyleRun += "{ " to the styleString
	if( textStyleNumber&bold ){
		theStyleRun += "font-weight: bold; ";
	}
	if( textStyleNumber& italic ){
		theStyleRun += "font-style: italic ;";
	}	
	if( textStyleNumber& underline ){
		theStyleRun += "text-decoration: underline; " ;
	}
	if( textStyleNumber& shadow ){
		theStyleRun += "text-shadow: 0.2em 0.2em";
	}
	//if( stylenumber&  outline ){
	//	theStyleRun += to"text-shadow: 0.2em 0.2em :" ;
	//}
	if( textStyleNumber & condense ){
		theStyleRun += "font-stretch: condensed; ";
	}
	if( textStyleNumber& extend ){
		theStyleRun += "font-stretch: expanded; ";
	
	}		
}	

oofString 
oofRepTextStyle2CSS::makeAlignmentString(oofColSizer::colAlignT inAlign)
{
	oofString alignString ="text-align:  " ;
	switch(inAlign){
	case oofColSizer::alignLeft:
		alignString +="left"	;
	break;
	case oofColSizer::alignCentre:
		alignString +="center";
	break;
	case oofColSizer::alignRight:
		alignString +=	"right";
	break;
	case oofColSizer::alignJust:
		alignString +=	"justify";
	break;
	}
	return alignString;
}	

// -------------------------------------------------------
//    o o f R e p o r t S e t t i n g s 2 C S S 
// -------------------------------------------------------
// the caller after invoking this function must immmedialy
// apphend defaultTextID to the current CSS style run string/stream
// call then emit a css style on defaultTextStyle() 
oofString
oofReportSettings2CSS::emitCSSstyle(const oofRepSettings* aReportSetting, const oofString& defaultTextID)
{
	// apphend rp to strings to make unique from any possible standrad css identifier strings
	char scrapString[256];
	//oofString theDefaultTextStyle;
	oofString styleRun ="{";
	//oofRepTextStyle2CSS styleParser;
	sprintf(scrapString,"rpLineThickness: %d; ",aReportSetting->lineThickness());
	styleRun += scrapString;

	sprintf(scrapString,"rpPageHeight: %d ; ",aReportSetting->pageHeight());
	styleRun += scrapString;

	sprintf(scrapString,"rpPageWidth: %d ; ",aReportSetting->pageWidth());
	styleRun += scrapString;

	sprintf(scrapString,"rpBorderType: %d ; ",aReportSetting->borderType());
	styleRun += scrapString;

	sprintf(scrapString,"rpBandIndenting : %d ; ",aReportSetting->bandIndenting());
	styleRun += scrapString;

	sprintf(scrapString,"rpLeftMargin: %d ; ",aReportSetting->leftMargin());
	styleRun += scrapString;

	sprintf(scrapString,"rpRightMargin: %d ; ",aReportSetting->rightMargin());
	styleRun += scrapString;

	sprintf(scrapString,"rpTopMargin: %d ; ",aReportSetting->topMargin());
	styleRun += scrapString;
	
	sprintf(scrapString,"rpBottomMargin: %d ; ",aReportSetting->bottomMargin());
	styleRun += scrapString;
	
	sprintf(scrapString,"rpColSepWidth: %d ; ",aReportSetting->colSepWidth());
	styleRun += scrapString;
	
	sprintf(scrapString,"rpBlockVertSep: %d ; ",aReportSetting->blockVertSep());
	styleRun += scrapString;
	
	styleRun += "rpDefaultTextStyle: ";
	styleRun += defaultTextID;
	styleRun += "; ";

// defaultPageHeaders skipped - written in <layout>
// defaultPageFooters skipped
	
	sprintf(scrapString,"rpDrawHeadingsDownToLevel: %d ; ",aReportSetting->drawHeadingsDownToLevel());
	styleRun += scrapString;

	if(aReportSetting->printGraphsInMono())
		styleRun += "rpPrintGraphsInMono: true ; ";
	else
		styleRun += "rpPrintGraphsInMono: false ; ";
	
	if(aReportSetting->modalPreviewWindow())
		styleRun += "rpModalPreviewWindow: true ; ";
	else
		styleRun += "rpModalPreviewWindow: false ; ";
	
	sprintf(scrapString,"rpGraphBandMinSizePercent: %d ; ",aReportSetting->graphBandMinSizePercent());
	styleRun += scrapString;

// XMLsettings skipped - application code sets function pointer
	styleRun += " }";
	return styleRun;
}

	

// ---------------------------------------
//     o o f R e p X M L E n v
// ---------------------------------------
oofRepXMLEnv::oofRepXMLEnv(ostream& os) :
	oofRepStreamEnv(os)
{
}


oofRepXMLEnv::oofRepXMLEnv(const char* fileName) :
	oofRepStreamEnv(fileName) 
{}


void	
oofRepXMLEnv::makeRenderDispatch(oofRepDrawable* drawable) const
{
// now the magic bit.
// by dereferencing this, we make an overriding function match against the virtual
// method that takes an oofRepXMLEnv&.
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
oofRepXMLEnv::makeRenderDispatch(oofAdorner* drawable) const
{
	drawable->makeRender(*this);
}

// a little bit of RTTI which can be used to record what kind of render last
// used, and could be used in switches if the above virtual multiple dispatch
// technique wasn't being used instead
oofRepRenderT 
oofRepXMLEnv::renderType() const
{
	return eRenderType;
}


void
oofRepXMLEnv::drawPageBreak()
{
}


void 
oofRepXMLEnv::changeTextStyle(oofRepTextStyle*)
{
}


unsigned short 
oofRepXMLEnv::heightOfTextStyle(oofRepTextStyle*)
{
// Not relevant for XML output... dummy value used instead
	return 12;
}


unsigned long 
oofRepXMLEnv::calcPageHeight()
{
	// Not relevant for XML output... dummy value used instead
	return 840;
}


unsigned long 
oofRepXMLEnv::calcPageWidth()
{
	// Not relevant for XML output... dummy value used instead
	return 590;
}


void
oofRepXMLEnv::startEnvironment()
{
	mIdentities.reset();  // reset each time in case we rerun, so same ID's allocated
	
	oofRep* mainReport = oofRep::currentReport();  
	oofRepMulti* multiRep = 0;
	if (mainReport->isMultiReport())
		multiRep = (oofRepMulti*) mainReport;  // safe downcast
		
// output XML header 
	*mOut << oofXMLwriter::xmlSimpleVersionString();

// output start of report
	if (multiRep) {
		char scrapStr[40];
		const int numSections = multiRep->count();
		sprintf(scrapStr," numSections='%d'",numSections);
		*mOut << "<report ID='1'" << scrapStr << ">\n";
	}
	else
		*mOut << "<report ID='1'>\n";
		
	mIdentities.enterLevel();  // within root element <report>
		
// output unified schema for all report and sections
	{
		const oofString schemaIndent = mIdentities.getIndentString();
		*mOut << oofXMLDBWriter::schemaHeader(schemaIndent);
		mIdentities.enterLevel();  // want schema elements indented inside <schema>
			OOF_repXMLschemaVisitor schemaWriter(mIdentities);
			schemaWriter.visit(mainReport);
		mIdentities.leaveLevel();
		*mOut << oofTagMaker::makeClosingTag("schema", schemaIndent);
	}

// output layout and style for single reports
	if (multiRep)
		mSectionNumber=0;
	else
		StartReport("1");
}


void
oofRepXMLEnv::startSection()
{
// called before each section, regardless of settings
// unlike drawSectionBreak which may not be called if no page breaks specified
	if (mSectionNumber>0)
		CloseSection();
	
	const oofString reportID = mIdentities.getIDstring();
	*mOut << oofTagMaker::makeOpeningTag("section",mIdentities.getIndentString(), reportID) << '\n';
	mIdentities.incrementID();
	mIdentities.enterLevel();
	StartReport(reportID);
	mSectionNumber++;
}


void
oofRepXMLEnv::CloseSection()
{
	mIdentities.leaveLevel();
	*mOut << oofTagMaker::makeClosingTag("section",mIdentities.getIndentString());
}


void
oofRepXMLEnv::StartReport(const oofString reportID)
{
	oofRep* rep = oofRep::currentReport();
	
	const oofString styleIndent = mIdentities.getIndentString();
	const oofString reportDefaultTextStyleID = mIdentities.getIDstring();
	mIdentities.incrementID();  // settings include a text style written separately with its own ID
	
	*mOut << oofTagMaker::makeOpeningTag("style", styleIndent) << '\n';
	OOF_repXMLstyleVisitor styleWriter(mIdentities);
	mIdentities.enterLevel();
		oofRepStreamEnv::out() 
		<< styleIndent << styleIndent  // double-indent all style strings
		<< '#' << reportID
		<< oofReportSettings2CSS::emitCSSstyle(rep->settings(), reportDefaultTextStyleID)
		<< '\n'
		
		<< styleIndent << styleIndent 
		<< '#' << reportDefaultTextStyleID
		<< oofRepTextStyle2CSS::emitCSSstyle(*rep->settings()->defaultTextStyle())
		<< '\n';
		styleWriter.visit(rep);
	mIdentities.leaveLevel();
	*mOut << oofTagMaker::makeClosingTag("style", styleIndent);
	
	if (styleWriter.adornersEncountered()) {
		*mOut << oofTagMaker::makeOpeningTag("adorners", styleIndent) << '\n';
		OOF_repXMLadornerVisitor adornerWriter(mIdentities);
		mIdentities.enterLevel();
			adornerWriter.visit(rep);
		mIdentities.leaveLevel();
		*mOut << oofTagMaker::makeClosingTag("adorners", styleIndent);
	}
	
	*mOut << oofTagMaker::makeOpeningTag("layout", styleIndent) << '\n';
	OOF_repXMLlayoutVisitor layoutWriter(mIdentities);
	mIdentities.enterLevel();
		layoutWriter.visit(rep);
	mIdentities.leaveLevel();
	*mOut << oofTagMaker::makeClosingTag("layout", styleIndent);
}


void
oofRepXMLEnv::endEnvironment()
{
	oofRep* mainReport = oofRep::currentReport();  
	if (mainReport->isMultiReport()) 
		CloseSection();
// output closing report tag
	*mOut << "</report>\n";
}


bool
oofRepXMLEnv::preprocessesPageHeadersAndFooters() const
{
	return true;
}


// -------------------------------------------------------
//            o o f R e p T a g M a k e r
// -------------------------------------------------------
oofString 
oofRepTagMaker::makeLineTag(unsigned long height, const char* indentString, const char* /*idString*/)
{
	char scrapString[32];
	oofString result = indentString;
	result += "<HR";
	sprintf(scrapString," height='%d'/>\n", height);
	result += scrapString;
	return result;
}


oofString 
oofRepTagMaker::makeSpaceTag(unsigned long height, const char* indentString, const char* /*idString*/)
{
	char scrapString[32];
	oofString result = indentString;
	result += "<space";
	sprintf(scrapString," height='%d'/>\n", height);
	result +=scrapString;
	return result;
}	

		
oofString
oofRepTagMaker::makeBreakTag(const char* indentString)
{
	return oofString(indentString, "<BR/>\n");
}




// -------------------------------------------------------
//           r e n d e r   f a c t o r i e s
// -------------------------------------------------------
void	
oofRepViewBand::makeRender(const oofRepXMLEnv& inEnv) 
{
	delete mRender;	// assume if we're called and have existing it's wrong type
	oofRepViewBand_Render* theRender = new oofRepViewBand_XMLRender(this);
	mRender = theRender;
	mTableExtraRender = theRender;
	mRender->finishConstruction(); 
}


void	
oofRepSpaceBand::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepSpaceBand_XMLRender(this);
}


void	
oofRepGraphBand::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepGraphBand_XMLRender(this);
}


void	
oofRepLineBand::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepLineBand_XMLRender(this);
}


void	
oofRepPictBand::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBand_XMLRender(this);
}


void	
oofRepLayoutBand::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepLayoutBand_XMLRender(this);
}


void	
oofRepTextBlock::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepTextBlock_XMLRender(this);
}


void	
oofRepPictBlock::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepPictBlock_XMLRender(this);
}


void	
oofRepPageNoBlock::makeRender(const oofRepXMLEnv& env) 
{
	delete mRender;
	mRender = new oofRepPageNoBlock_XMLRender(this);
}


// -------------------------------------------------------
//   o o f R e p V i e w B a n d _ X M L R e n d e r 
// -------------------------------------------------------
oofRepViewBand_XMLRender::oofRepViewBand_XMLRender(oofRepViewBand* b) : 
	oofRepViewBand_Render(b)
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory& identities = thisEnvir->getIdentities();
	mWriter = new oofXMLDBWriter(oofRepStreamEnv::out(), mOwner->view(), identities);		
}


oofRepViewBand_XMLRender::~oofRepViewBand_XMLRender()
{
	delete mWriter;
}


oofRepRender*
oofRepViewBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepViewBand_XMLRender((oofRepViewBand*)newOwner);	// safe downcast
}



void 
oofRepViewBand_XMLRender::startTableBody(bool /*headersHaveChanged*/)
{
	assert(mWriter);  // created in ctor
	mWriter->startDatabase();
}


void 
oofRepViewBand_XMLRender::endTableBody()
{
	oofRep* theRep = oofRep::currentReport();
	mWriter->endDatabase();
}


void 
oofRepViewBand_XMLRender::draw()	// draws data for just current record
{
	assert(mWriter);
	mWriter->writeRecord();
}




// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d _ X M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepLayoutBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLayoutBand_XMLRender((oofRepLayoutBand*)newOwner);	// safe downcast
}


// -------------------------------------------------------
//    o o f R e p L i n e B a n d _ X M L R e n d e r 
// -------------------------------------------------------
void
oofRepLineBand_XMLRender::describeLayout()
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory&         	identities = thisEnvir->getIdentities();
	const unsigned long fixedHeight = mOwner->fixedHeight();
	oofRepStreamEnv::out() << oofRepTagMaker::makeLineTag(
		fixedHeight, 
		identities.getIndentString(), 
		mOwner->ID()
	);
}


oofRepRender*
oofRepLineBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepLineBand_XMLRender((oofRepLineBand*)newOwner);	// safe downcast
}


// -------------------------------------------------------
//    o o f R e p P i c t B a n d _ X M L R e n d e r 
// -------------------------------------------------------
oofRepRender*
oofRepPictBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBand_XMLRender((oofRepPictBand*)newOwner);	// safe downcast
}


// -------------------------------------------------------
//    o o f R e p S p a c e B a n d _ X M L R e n d e r 
// -------------------------------------------------------
void
oofRepSpaceBand_XMLRender::describeLayout()
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory&         	identities = thisEnvir->getIdentities();
	const unsigned long fixedHeight = mOwner->fixedHeight();
	oofRepStreamEnv::out() << oofRepTagMaker::makeSpaceTag(
		fixedHeight, 
		identities.getIndentString(), 
		mOwner->ID()
	);
}


oofRepRender*
oofRepSpaceBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepSpaceBand_XMLRender((oofRepSpaceBand*)newOwner);	// safe downcast
}



// -------------------------------------------------------
//    o o f R e p G r a p h B a n d _ X M L R e n d e r
// -------------------------------------------------------
void
oofRepGraphBand_XMLRender::describeLayout()
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory&         	identities = thisEnvir->getIdentities();

	const oofString srcName = mOwner->view()->name();
	oofString overlaySrcName;
		
	const dbView* overlayView = mOwner->view(1);
	if (overlayView) {
		overlaySrcName = overlayView->name();
	}
	char scrapString[256];
	const unsigned long height = mOwner->fixedHeight();
	const unsigned long gHeight = mOwner->originalGraphHeight();
	const unsigned long gWidth = mOwner->originalGraphWidth();
	const unsigned long numOverlaid = mOwner->numOverlaid();
	const oofString graphTitle = oofString::encodeEntity( mOwner->graph()->getTitle() );
	sprintf(scrapString," height='%d' graphWidth='%d' graphHeight='%d'",height, gWidth, gHeight);

// we use a few temp oofStrings for faster construction of concatenated strings
	oofString attribString( " type=", oofString::quoteString(mOwner->graph()->getType()) );
	
	attribString += oofString( " source=", oofString::quoteString(oofTagMaker::encodeName(srcName)), scrapString );

	attribString += oofString( " title=", oofString::quoteString(graphTitle) );  

// optional attributes AT END!
	if (numOverlaid>0) {
		sprintf(scrapString," numOverlaid='%d'", numOverlaid);
		attribString += scrapString;
	}
	else if (!overlaySrcName.isEmpty()) {
		attribString += oofString( " overlaySource=", oofString::quoteString(oofTagMaker::encodeName(overlaySrcName)) );
	}
	
	oofRepStreamEnv::out() <<  oofTagMaker::makeOpeningTag(
		"graph", 
		identities.getIndentString(), 
		mOwner->ID(),
		attribString,
		true // empty tag
	);
}


oofRepRender*
oofRepGraphBand_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepGraphBand_XMLRender((oofRepGraphBand*)newOwner);	// safe downcast
}


void 
oofRepGraphBand_XMLRender::draw()
{
	oofRepXMLEnv* thisEnvir = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast	
	dbView* graphView = mOwner->view();
	assert(graphView);
	oofXMLDBWriter graphWriter(oofRepStreamEnv::out(), graphView, thisEnvir->getIdentities());
	graphWriter.writeDatabase();


	graphView = mOwner->view(1);  // 2nd view is overlayView if it exists
	if (graphView) {
		oofXMLDBWriter overlayWriter(oofRepStreamEnv::out(), graphView, thisEnvir->getIdentities());
		overlayWriter.writeDatabase();
	}
}


// -------------------------------------------------------
//    o o f R e p T e x t B l o c k _ X M L R e n d e r
// -------------------------------------------------------
void
oofRepTextBlock_XMLRender::describeLayout()
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory& identities = thisEnvir->getIdentities();
	
	oofRepBlock::blockAlignT blockAlign = mOwner->align();
	oofString attStr;
	if (blockAlign==oofRepBlock::alignRight)
		attStr = "align='right'";
	else if (blockAlign==oofRepBlock::alignCentre)
		attStr = "align='centre'";
		
	const unsigned long fixedBlockWidth = mOwner->fixedWidth();
	if (fixedBlockWidth>0) 
		attStr.convertNumber(fixedBlockWidth, " fixedWidth='%d'", true);  // append fixedWidth attribute
		
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeSimpleBoundedElement(
			mOwner->text(), 
			"inline", 
			identities.getIndentString(), 
			mOwner->ID(),
			attStr
		);

	if( mOwner->moveDownAfter() ) {
		oofRepStreamEnv::out() << oofRepTagMaker::makeBreakTag(identities.getIndentString());	
	}
}


oofRepRender*
oofRepTextBlock_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepTextBlock_XMLRender((oofRepTextBlock*)newOwner);	// safe downcast
}


// -------------------------------------------------------
//    o o f R e p P a g e N o B l o c k _ X M L R e n d e r
// -------------------------------------------------------
/**
	straight copy of oofRepTextBlock_XMLRender version but with different text in the middle.
*/
void
oofRepPageNoBlock_XMLRender::describeLayout()
{
	oofRepXMLEnv* thisEnvir  = (oofRepXMLEnv*)oofRep::currentReport()->environment();  // safe downcast
	oofIDfactory& identities = thisEnvir->getIdentities();
	
	oofRepBlock::blockAlignT blockAlign = mOwner->align();
	const char* attStr = 0;
	if (blockAlign==oofRepBlock::alignRight)
		attStr = "align='right'";
	else if (blockAlign==oofRepBlock::alignCentre)
		attStr = "align='centre'";

	oofRepBracketedTextBlock* realOwner = (oofRepBracketedTextBlock*) mOwner;  // safe downcast
	const oofString pageNoStr(realOwner->leadingText(), "^P", 	realOwner->trailingText());	
	oofRepStreamEnv::out() 
		<<  oofTagMaker::makeSimpleBoundedElement(
			pageNoStr, 
			"inline", 
			identities.getIndentString(), 
			mOwner->ID(),
			attStr
		);

	if( mOwner->moveDownAfter() ) {
		oofRepStreamEnv::out() << oofRepTagMaker::makeBreakTag(identities.getIndentString());	
	}
}


oofRepRender*
oofRepPageNoBlock_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPageNoBlock_XMLRender((oofRepTextBlock*)newOwner);	// safe downcast
}



// -------------------------------------------------------
//    o o f R e p P i c t B l o c k _ X M L R e n d e r
// -------------------------------------------------------
oofRepRender*
oofRepPictBlock_XMLRender::clone(const oofRepDrawable* newOwner) const
{
	return new oofRepPictBlock_XMLRender((oofRepPictBlock*)newOwner);	// safe downcast
}

