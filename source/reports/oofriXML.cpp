// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofriXML.cpp
// Input of reports from XML
// see .h file for more details on the XML parser xpatpp 

#ifdef OOF_READ_REPORTS

#ifndef _STDIO_H
	#include <stdio.h>
#endif
#ifndef _STRING_H
	#include <string.h>
#endif
#ifndef H_oofSting
	#include "oofSting.h"
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFRIXML
	#include "oofriXML.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOF4
	#include "oof4.h"  // for dbBool
#endif


/**
	\defgroup oofRepRead Report Reading From File
	Provide the ability to read a report from a single XML document.
	The XML must include at least a report layout description and 
	using XML Xtras can be run against an open database.
	
	The original feature in Open Source OOFILE expected the XML file
	to also contain the database schema and report data.
	
	\ingroup oofRW
*/

/// individual bitmask states as can overlap
enum xmlRepParseStateT {
	eIdle=0,
	eInFirstPageHeader=1,
	eInHeader=2,
	eInFirstPageFooter=4,
	eInFooter=8,
	eInLayoutBand=0x10,
	eInViewBand=0x20,
	eInGraphband=0x40,
	eDoneFooter=0x80,
	eInBlock=0x100,
	eInBody=0x200,
	eInHeaderBreak=0x400,
	eInFooterBreak=0x800,
	eInTable=0x1000,
	eInPageHeader=0x2000,
	eInPageFooter=0x4000,
	eInColumn=0x8000,
	eInWidth=0x10000,
	eInCustomCode=0x20000
};


// -------------------------------------------------------
//   settings and style managers
//   local class declarations
// -------------------------------------------------------

enum { kCompletePromiseMsg='Prom' };

/**
	Base object for class that broadcasts messages to OOF_mixPromise instances.
	There will typically be one subclass of oofRepPromiseCompleter and many instances
	of the promises, created as parsing occurs.
	\todo later factor oofRepPromiseCompleter into parent oofPromiseCompleter
	but haven't the time to consider design issues in making useful outside this context.
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofRepPromiseCompleter : public oofBroadcaster {
public:
	
	oofRepPromiseCompleter(const oofString& inCSS) :
		mCSSstring(inCSS)
	{};
	virtual ~oofRepPromiseCompleter() {};
	virtual void completePromises()=0;

	const char* findStringForID(const oofString& theID) const { return findStringForID(theID, mCSSstring); };
	static const char* findStringForID(const oofString& theID, const char* inString);
	
// data
	const oofString& mCSSstring;  // assumes someone else maintains, just using us
};

	
class oofPromisedGraphSettings;  // forward
	
/**
	Used by OOF_XMLrepParser::FinishReport to match graph settings with ID.
	OOF_XMLrepParser::startElem_layout creates instance.
	\see oofPromisedGraphSettings::receiveMsg 
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofGraphSettingsCompleter: public oofRepPromiseCompleter {
public:
	oofGraphSettingsCompleter(const oofString&);
	virtual ~oofGraphSettingsCompleter();
	
	virtual void completePromises();
	void completeSettings(const oofString& inID, oofGraph*) const;  // callback from listener

	static oofGraphSettingsCompleter* currentlyConstructing() { return sCurrentlyConstructing; };

private:
	static oofGraphSettingsCompleter* sCurrentlyConstructing;	
};


class oofPromisedRepTextStyle;// forward

/**
	Used by OOF_XMLrepParser::FinishReport to match text styles with ID.
	OOF_XMLrepParser::startElem_layout creates instance.
	\see oofPromisedRepTextStyle::receiveMsg 
	\ingroup oofRepRead
*/
class OOFREP_EXPORT oofRepTextStyleCompleter: public oofRepPromiseCompleter {
public:
	oofRepTextStyleCompleter(const oofString&);
	virtual ~oofRepTextStyleCompleter();
	
	virtual void completePromises();
	void completeStyle(oofPromisedRepTextStyle*) const;  // callback from listener
	
	static oofRepTextStyleCompleter* currentlyConstructing() { return sCurrentlyConstructing; };

private:
	static oofRepTextStyleCompleter* sCurrentlyConstructing;	
};


/**
	Mixin for objects created to fulfill a promise later when parsing complete.
	Used at the end of the report to link text styles read in the \<style\>
	tag with the report objects created from parsing the \<layout\> tag.
	\ingroup oofRepRead
*/
class OOF_mixPromise : public oofSingleListener {
protected:  // only use as mixin so protect ctor etc.
	OOF_mixPromise(oofRepPromiseCompleter* listensTo, const oofString& theID) :
		oofSingleListener(listensTo),
		mID(theID)
	{};
	// uses default copy ctor, dtor

public:
	const oofString& ID() const { return mID; };

protected:		
// data storage
	const oofString mID;
};


/**
	Instances created every time we parse a stylable element that has an ID.
	\see OOF_XMLrepLayoutParser::ParseAccumulatedCharData
	\see OOF_XMLrepLayoutParser::ParseAccumulatedCharData
	\see OOF_XMLrepLayoutParser::OpenTable
	Use OOF_mixPromise as parent as we don't allocate the settings so don't get to use our subclass
	\ingroup oofRepRead
*/
class oofPromisedRepTextStyle : public oofRepTextStyle, public OOF_mixPromise {
public:
	oofPromisedRepTextStyle(oofRepPromiseCompleter* c, const oofString& theID) :
		OOF_mixPromise(c, theID)
	{};
	oofPromisedRepTextStyle(const oofString& theID) :
		OOF_mixPromise(oofRepTextStyleCompleter::currentlyConstructing(), theID)
	{};
	// use default copy ctor, dtor

	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);
	virtual oofRepTextStyle* clone() const;
};


/**
	Instance created for each graph band read in layout description to get settings.
	Use OOF_mixPromise as parent as we don't allocate the settings so don't get to use our subclass
	this is therefore not a promise standing in for the graph settings so much as a helper
	to clean them up
	\ingroup oofRepRead
*/
class oofPromisedGraphSettings : public OOF_mixPromise {
public:
	oofPromisedGraphSettings(oofRepPromiseCompleter* c, const oofString& theID, oofGraph* G) :
		OOF_mixPromise(c, theID),
		mGraph(G)
	{};
	oofPromisedGraphSettings(const oofString& theID, oofGraph* G) :
		OOF_mixPromise(oofGraphSettingsCompleter::currentlyConstructing(), theID),
		mGraph(G)
	{};
	// use default copy ctor, dtor
	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);

private:
	oofGraph* mGraph;	
};


// -------------------------------------------------------
//  O O F _ r e p X M L s t y l e C l e a n u p V i s i t o r
// -------------------------------------------------------

/**
	simple visitor to cleanup a report definition after import from XML.
	\see OOF_XMLrepParser::FinishReport
	\ingroup oofRepRead
*/
class OOF_repXMLstyleCleanupVisitor : public oofRepVisitor {
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
};


/**
	Deletes any styles found which were created but not completed.
	\see oofRepBlock::textStyle
*/
void
OOF_repXMLstyleCleanupVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	assert(inDrawable);
	oofRepTextStyle* drawableStyle = inDrawable->textStyle();
	if (drawableStyle && !drawableStyle->invariant())
		inDrawable->textStyle(0);  // delete incomplete styles
}


// -------------------------------------------------------
//  O O F _ r e p X M L c l e a r I D V i s i t o r
// -------------------------------------------------------
/**
	simple visitor to remove ID's for anything without text style matched to it.
	If report is then resaved, no ID attributes will be written without matching styles.
	\see OOF_XMLrepParser::FinishReport
	\ingroup oofRepRead
*/
class OOF_repXMLclearIDVisitor : public oofRepVisitor {
protected:
	virtual void VisitDrawable(oofRepDrawable*); 
};


void
OOF_repXMLclearIDVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	inDrawable->clearID();
}


// -------------------------------------------------------
//  O O F _ r e p X M L a d o r n e r L i n k V i s i t o r
// -------------------------------------------------------

/**
	links up adorners held in the report's dictionary with their drawables.
	\see OOF_XMLrepParser::FinishReport
	\ingroup oofRepRead
*/
class OOF_repXMLadornerLinkVisitor : public oofRepVisitor {
public:
	OOF_repXMLadornerLinkVisitor(OOF_Dictionary& adornerDict) :
		mCallerDict(adornerDict)
	{};
	
protected:
	virtual void VisitDrawable(oofRepDrawable*); 

// data storage
	OOF_Dictionary& mCallerDict;
};


void
OOF_repXMLadornerLinkVisitor::VisitDrawable(oofRepDrawable* inDrawable)
{
	const oofString theID = inDrawable->ID();
	if (!theID.isEmpty()) {
		OOF_adornerHolder* existingHolder = (OOF_adornerHolder*)mCallerDict[theID];  // safe downcast
		if (existingHolder)
			inDrawable->adorners().adopt(existingHolder->adorners());
	}
}


// -------------------------------------------------------
//         O O F _ X M L r e p P a r s e r S t a t e
// -------------------------------------------------------
/**
	Maintain a state stack for OOF_XMLrepParser.
	\ingroup oofXML oofInternals
*/
class OOF_XMLrepParserState{
private: 
friend class OOF_XMLrepLayoutParser;

	OOF_XMLrepParserState();
	//setters
	void saveOldState(bool clearNew=true);
	void restoreOldState();
	void set(unsigned int inState);
	void clear(unsigned int inState);
	
	
	// getters
	
	unsigned int getRepBuildState();
	
private:
	unsigned int  mRepGenState;
	OOF_ExpandableLongArray mOldState;
};


inline void 
OOF_XMLrepParserState::set(unsigned int inState)
{
	mRepGenState = mRepGenState | inState;

}


inline void 
OOF_XMLrepParserState::clear(unsigned int inState)
{
	mRepGenState = mRepGenState &(~inState);

}


// getters
inline unsigned int  
OOF_XMLrepParserState::getRepBuildState()
{	
	return mRepGenState;
}


// -------------------------------------------------------
//            O O F _ X M L r e p P a r s e r
// -------------------------------------------------------
/**
	top level parser reading report from XML file.
	May read just report layout or also read style tags for 
	text styles, report and graph settings attached to layout.
	\param inDB is optional existing database connection that
	can be used with the layout. 
	schema tags within the XML file will be added to this
	DB. If no dbConnect is passed in, one is created
	\see 
*/
OOF_XMLrepParser::OOF_XMLrepParser(dbConnect* inDB) :
	mReport(0),
	mMultiRep(0),
	mDB(inDB),
	mTextStyleCompleter(0),
	mGraphSettingsCompleter(0),
	mAdornerDict(10, 2, true /* owns contents*/)
{
}


OOF_XMLrepParser::OOF_XMLrepParser(expatppNesting* parent, dbConnect* inDB) :
	expatppNesting(parent),
	mReport(0),
	mMultiRep(0),
	mDB(inDB),
	mTextStyleCompleter(0),
	mGraphSettingsCompleter(0)
{
}


OOF_XMLrepParser::~OOF_XMLrepParser()
{
	const XML_Error err = XML_GetErrorCode();
	if (err==0) {
	// DEBUGGING ONLY - extract the data we've parsed
#if 0
		{
			ofstream dumpFile("OOF_XMLrepParser.dump");
			mDB->describe(dumpFile);
			mDB->extract(dumpFile);
			dumpFile.close();
		}
#endif
		assert(mDepth==0);  // completed parsing
		assert(!mReport);  // otherwise not much point in using! - user failed to retrieve report
		assert(!mTextStyleCompleter);
		assert(!mTextStyleCompleter);
	}
	else {  // cleanup to avoid leaks just because bad file
		delete mReport;
		delete mTextStyleCompleter;
		delete mGraphSettingsCompleter;
	}
}


bool
OOF_XMLrepParser::parseFile(const oofFileRef& theFile)
{	
	bool ret = true;
	stSaveDirectory returnToCurrentDirWhenDone;
	const oofDirectory fileDir = theFile.directory();
	fileDir.gotoDirectory();  // ANSI file open gets name alone
	const oofString theFilename = theFile.filename();
	
//largely copied from the sample code
	FILE* xmlFile;
	char buf[BUFSIZ];
	int done;
	int depth = 0;

	xmlFile = fopen(theFilename, "r");
	if (xmlFile) {
		do {  // loop reading the file in a fixed size buffer
			size_t len = fread(buf, 1, sizeof(buf), xmlFile /*stdin*/);
			done = (len < sizeof(buf));
			if (!XML_Parse(buf, len, done)) {
	#ifdef OOF_DEBUG
	/*
	from original sample
				fprintf(stderr,
					"%s at line %d\n",
					XML_ErrorString(XML_GetErrorCode()),
					XML_GetCurrentLineNumber());
	*/
				dbConnect::raise( stringstream() << flush 
					<< "OOF_XMLrepParser::parseFile error '"
					<< XML_ErrorString(XML_GetErrorCode())
					<< "' at line " << XML_GetCurrentLineNumber(),
					false /* don't terminate */
				);
	#endif
				ret=false;
				break;
			}
		} while (!done);
		fclose(xmlFile);
	}
	return ret;
}


/**
	Relinquishes ownership so caller responsible for deleting.
	\return the generated report.
*/
oofRep* 
OOF_XMLrepParser::orphanReport()
{
 	oofRep* ret = mReport;
	mReport = 0;
	return ret;
}
	
	
/**
	startElement call back.
*/	
void 
OOF_XMLrepParser::startElement(const char* name, const char** atts)
{
	if(strcmp(name,"report")==0)
		startElem_report(atts);
		
	else if(strcmp(name,"section")==0)
		startElem_section(atts);
		
	else if(strcmp(name,"style")==0)
		startElem_style(atts);

	else if(strcmp(name,"adorners")==0)
		startElem_adorners(atts);

	else if(strcmp(name,"layout")==0)
		startElem_layout(atts);

	else if(strcmp(name,"schema")==0)
		startElem_schema(atts);

	else 
		startUnknownElem(name, atts);

}// end start element


void 
OOF_XMLrepParser::startElem_report(const char** atts)
{
		assert(!mReport);
		assert(mDepth==1);  // report is root element
		if (atts[0] && (strcmp(atts[0], "ID")==0)) {
			mID = atts[1];
			if (atts[2])  // GROSS ASSUMPTION - any attribute on <report> after ID means it's multi
				mMultiRep = new oofRepMulti;
			else
				mReport = new oofRep;
		}
		else {
			mID = "1";
			mReport = new oofRep;
		}
	}


void 
OOF_XMLrepParser::startElem_section(const char** atts)
{
		assert(mDepth==2);  // can only get section immediately within report
		assert(mMultiRep);
		assert(!mReport);
		mReport = new oofRepSection;
		assert(atts[0] && (strcmp(atts[0], "ID")==0));
		mID = atts[1];
	}


void 
OOF_XMLrepParser::startElem_style(const char** atts)
{
		new OOF_XMLrepStyleParser(this, mStyleString);  // self-deleting sub parser
	}


void 
OOF_XMLrepParser::startElem_adorners(const char** atts)
{
		new OOF_XMLrepAdornerParser(this, mAdornerDict);  // self-deleting sub parser
	}


void 
OOF_XMLrepParser::startElem_layout(const char** atts)
{
		assert(!mTextStyleCompleter);
		mTextStyleCompleter = new oofRepTextStyleCompleter(mStyleString);
		
		assert(!mGraphSettingsCompleter);
		mGraphSettingsCompleter = new oofGraphSettingsCompleter(mStyleString);
		new OOF_XMLrepLayoutParser(this, mDB);  // self-deleting sub parser
	}


void 
OOF_XMLrepParser::startElem_schema(const char** atts)
{
		new oofXMLschemaParser(this);  // self-deleting sub parser
	}


void 
OOF_XMLrepParser::startUnknownElem(const char* name, const char** atts)
{
		oofXMLdataParser* dataParser = new oofXMLdataParser(this, mDB);  // self-deleting sub parser
		mDepth--;  // this is not our tag - so we didn't really go down this far
		nestedStartElementCallback(dataParser, name, atts);  // give this tag to the sub-parser
		// NOTE the difference in this case from the others is that we don't have a wrapping tag
		// within which we invoke the sub-parser. If all the data was wrapped in a <data> tag
		// then this case would be as simple as the earlier ones
	}





/**
	top level parser endElement.
	<ul>
	<li><em>schema</em> creates mDB using oofRepXMLsettings::createTempConnectFiles
	with the newConnection in createTempConnectFiles, our schema is complete and
	the tables attached to it can be used in dbViews, have data added etc.
	until newConnection, table definitions aren't finalised as the user can change
	table details with calls after the dbTable ctor. The caller will usually be 
	responsible for deleting this created database, eg: a preview window will hang
	onto the database for redraw purposes!
	Because the <schema> tag creates the database, there is ONE such tag even
	in a multi report, preceding all the <section> tags.
	</ul>
	\warning must have created some instance of dbConnect before invoking to parse
	a report file containing a schema
*/
void
OOF_XMLrepParser::endElement(const char* name)
{
	if(strcmp(name,"report")==0)
		endElem_report();
		
	else if(strcmp(name,"section")==0)
		endElem_section();
		
	else if(strcmp(name,"style")==0)
		endElem_style();

	else if(strcmp(name,"adorners")==0)
		endElem_adorners();

	else if(strcmp(name,"layout")==0)
		endElem_layout();

	else if(strcmp(name,"schema")==0)
		endElem_schema();

	else 
		endUnknownElem(name);

}// end element


void 
OOF_XMLrepParser::endElem_report()
{
		assert(mDepth==1);  // report is root element
		if (mMultiRep) {
			assert(!mReport);  // section finished
			mReport = mMultiRep;  // finally we return the report regardless of single or multi
		}
		else
			FinishReport();  // multi rep reports call at end of <section>
		assert(mReport);
	}


void 
OOF_XMLrepParser::endElem_section()
{
		assert(mDepth==2);  // can only get section immediately within report
		assert(mMultiRep);
		assert(mReport);
		FinishReport();
		mMultiRep->appendReport(mReport);
		mReport=0;
	}


void 
OOF_XMLrepParser::endElem_style()
{
// unused default, override as needed
}


void 
OOF_XMLrepParser::endElem_adorners()
{
// unused default, override as needed
}


void 
OOF_XMLrepParser::endElem_layout()
{
// unused default, override as needed
}


void 
OOF_XMLrepParser::endElem_schema()
{
		assert(!mDB); // only one schema!
		mDB = dbConnect::currentlyConstructing();  
		assert(mDB);
		oofRepXMLsettings* XMLsettings = oofRepDefaultSettings::settings()->XMLsettings();
		XMLsettings->createTempConnectFiles(mDB);  // user overrideable function
	}


void 
OOF_XMLrepParser::endUnknownElem(const char* /*name*/)
{
// unused default, override as needed
}


/**
	cleanup and post-process report parsed from XML.
	Reconciles settings and styles read from <style> element with
	ID's of <layout> elements.
*/
void
OOF_XMLrepParser::FinishReport()
{
	assert(mReport);
	const char* styleString = oofRepPromiseCompleter::findStringForID(mID, mStyleString);
	if (styleString)
		oofCSS2ReportSettings::parseSettingsText(styleString, mReport->getLocalSettings());
	
	assert(mTextStyleCompleter);
	mTextStyleCompleter->completePromises();
	delete mTextStyleCompleter;
	mTextStyleCompleter=0;
	
	assert(mGraphSettingsCompleter);
	mGraphSettingsCompleter->completePromises();
	delete mGraphSettingsCompleter;
	mGraphSettingsCompleter=0;
	
	mStyleString="";  // clear for the next one	
	
	OOF_repXMLstyleCleanupVisitor styleCleaner;
	styleCleaner.visit(mReport);  // remove any styles that weren't init'd above
	// usually caused by having to have an ID on an object but no matching
	// style - at runtime they will use a cascaded style
	
	OOF_repXMLadornerLinkVisitor adornerLinker(mAdornerDict);
	adornerLinker.visit(mReport);  // link adorners
	mAdornerDict.deleteAll();
	
	OOF_repXMLclearIDVisitor clearIDsLast;  // just to be safe
	clearIDsLast.visit(mReport);  // remove any styles that weren't init'd above
}


// -------------------------------------------------------
//      O O F _ X M L r e p L a y o u t P a r s e r
// -------------------------------------------------------

/**
	parse a <layout> tag to create an oofRep.
	This is the only mandatory parser in reading a file.
*/
OOF_XMLrepLayoutParser::OOF_XMLrepLayoutParser(OOF_XMLrepParser* parent, dbConnect* inDB) :
	expatppNesting(parent),
	mDB(inDB),
	mReport(parent->buildingReport()),
	mCurrentBreakBand(0),
	mCurrentLayout(0)
{
	mBuildState = new OOF_XMLrepParserState;
}


OOF_XMLrepLayoutParser::~OOF_XMLrepLayoutParser()
{
	assert(
		(mParser && XML_GetErrorCode()!=0) ||  // !mParser if cleanly exit nested parser 
		(mDepth==0)
	);  // completed parsing or error exit
	delete mBuildState;
}
	
	
oofRepViewBand*
OOF_XMLrepLayoutParser::CurrentViewBand() const
{
	oofRepViewBand* ret;
	const int viewLevel = mTablesStack.count()-1;
	if (viewLevel>=0)
		ret = (oofRepViewBand*)(mTablesStack.value(viewLevel));
	else
		ret = 0;
	return ret;
}


/**
	Match a 'source' attribute to a related or top-level table.
	The context of nested <table> elements indicates how related tables
	should be searched. By default, if it can't find a related table
	it gives up and assumes unrelated data is used so looks for <em>any</em>
	table matching the name.
*/
dbTable* 
OOF_XMLrepLayoutParser::GetDatabaseInContext(const char** atts, unsigned short startAttribute, const char* attName)
{
	dbTable* ret = 0;

// find the 'source' attribute, allowing caller to save time by specifying startAttribute
	unsigned short sourceIndex;
	for (sourceIndex=startAttribute; atts[sourceIndex]; sourceIndex+=2) {
		if (strcmp(atts[sourceIndex], attName)==0) {
			break;
		}
	}
	const oofString sourceName = oofTagMaker::decodeName( atts[sourceIndex+1] );
	const unsigned int currentState = mBuildState->getRepBuildState();
	if(currentState&eInTable) {
		if (currentState & (eInHeaderBreak|eInFooterBreak)) {  // try to lookup related table
			oofRepViewBand* parentViewBand = CurrentViewBand();
			assert(parentViewBand);
			dbView* parentView = parentViewBand->view();
			assert(parentView);
			dbTable* parentTable = parentView->table();
			if (parentTable) {  // views can have null tables
				dbField* theField = parentTable->field(sourceName);  // rel field has same name as table
				if (theField) {  // found a match so looks like nesting !
					assert(theField->fieldType()==relationshipField);
					dbRelRefBase* rel2rhs = (dbRelRefBase*)theField;  // safe downcast
					ret = rel2rhs->relatedTable();  // RECURSE into nested table
				}
			}  // lookup nested rel from table
		}  // in break of table
	} // in table
	if (!ret)  // didn't find related table above
		ret = mDB->table(sourceName);  // lookup at top level of connection
	return ret;
}	

	
void 
OOF_XMLrepLayoutParser::OpenTable(const char* /*name*/, const char** atts)
{
	dbTable* theTable = GetDatabaseInContext(atts);
	mBuildState->saveOldState();  // eg: in a header or footer of table
	mBuildState->set(eInTable);
	assert(theTable);  // otherwise <layout> refers to different DB schema

	dbView* adoptedView = theTable->makeView();  // assumption is that table represents the view we saved
	// so creating a view from the table with all fields is simply restoring the original view
	// (but in the process additional fields in the table, outside the view) have been lost
	
	oofRepViewBand* theBand = new oofRepViewBand(adoptedView, false /* default not boxed*/); 
	if (atts[0] && (strcmp(atts[0], "ID")==0)) {
		mCurrentCSSID = atts[1];
		theBand->textStyle(new oofPromisedRepTextStyle(mCurrentCSSID));
		theBand->ID(mCurrentCSSID);
	}
	mTablesStack.append(theBand);
}	

	
/**
	startElement call back.
	\note elements checked in order decreasing frequency of use
*/
void 
OOF_XMLrepLayoutParser::startElement(const char* name, const char** atts)
{
	const unsigned long currentState = mBuildState->getRepBuildState();
	
	if(currentState & eInColumn) {  // optimise a bit by using state as quick check
		if(strcmp(name,"header")==0)
			mBuildState->set(eInHeader);
		else if(strcmp(name,"width")==0)
			mBuildState->set(eInWidth);
		else if(strcmp(name,"customDrawer")==0) {
			mBuildState->set(eInCustomCode);
			oofXMLserializableParser* theParser = new oofXMLserializableParser(this, &mCurrentSerializable);  // self-deleting sub parser
		}
		else
			assert(!"unrecognised state inside a column");
	}
	else if(strcmp(name,"inline")==0){
		BuildBlock(atts);
	}
	else if(strcmp(name,"header")==0){
		OpenHeaderOrFooter(atts, eInFirstPageHeader, eInHeader);
	}
	else if(strcmp(name,"footer")==0){
		OpenHeaderOrFooter(atts, eInFirstPageFooter, eInFooter);
	}
	else if(strcmp(name,"space")==0){
		BuildSpaceBand( atts);
	}
	else if(strcmp(name,"column")==0){
		OpenColumn( atts);
	}
	else if(strcmp(name,"box")==0){
		OpenLayout(atts);
	}
	else if(strcmp(name,"table")==0){
		OpenTable(name, atts);
	}
	else if(strcmp(name,"HR")==0){
		BuildLineBand(atts);
	}
	else if(strcmp(name,"headerBreaks")==0){
		assert ( ! (mBuildState->getRepBuildState() & (eInHeaderBreak|eInFooterBreak)) ) ;// guards against nested  breaks
		mBuildState->set(eInHeaderBreak);
	}
	else if(strcmp(name,"footerBreaks")==0){
		mBuildState->set(eInFooterBreak);
	}
	else if(strcmp(name,"break")==0){
		assert(!mCurrentBreakBand);  
		mCurrentBreakNewPageEveryTime = (atts[0]!=0);  // assume any break attribute = YES
		// see endElement for construction of break after we've picked up any tables
		// etc. within it
	}
	else if(strcmp(name,"graph")==0){
		BuildGraphBand(atts);
	}
// ignore BR

	mCharData.clear();
}// end start element



/**
	char Data call back.
	Ignores data between elements.
	Called multiple times for each line inside an element.
*/
void 
OOF_XMLrepLayoutParser::charData(const char* s, int len)
{
	const unsigned long currentState = mBuildState->getRepBuildState();	
	if(currentState & currentState & (eInLayoutBand | eInBlock | eInColumn)) {
		const bool wantEmpty = currentState & (eInColumn & eInHeader);
		if (wantEmpty || !emptyCharData(s, len)){	
			oofString decodedString = oofString::utf8To8BitAscii(s,len);// convert from utf8 format
			mCharData.append(decodedString, decodedString.length());  // don't need overhead of decoding - expat has done that for us
		}
	}
}	

	
void 
OOF_XMLrepLayoutParser::ParseAccumulatedCharData()
{
	if (mCharData.isEmpty())
		return;
		
	const unsigned long currentState = mBuildState->getRepBuildState();	
	
	if(currentState & eInColumn) {  
		oofRepViewBand* theBand = CurrentViewBand();
		assert(theBand);
		dbView* theView = theBand->view();
		if (currentState & eInHeader) {
			theView->setHeading(mCurrentColNumber, mCharData);
			// WARNING doesn't cope with linebreaks in headings
			// would need to append to string before emptyCharData check above
			// see oofXMLdataParser for examples
		}
		else if (currentState & eInWidth) {
			oofColSizer* theSizer = theView->colSizer();
			if (mCharData.endsWith('%')) {
				mCharData.shorten(1);  // drop the '%'
				const unsigned long width = mCharData.asUnsignedLong();
				theSizer->setFixedPercentage(mCurrentColNumber, width);
			}
			else {
				const unsigned long width = mCharData.asUnsignedLong();
				theSizer->setFixedWidth(mCurrentColNumber, width);
			}
		}
//		else if (currentState & eInCustomCode) 
// implemented with a sub-parser so should never get here except with empty content!
//		else
//			assert(!"unrecognised state inside a column");
// can have column which specifies only in attributes so no nested tags, eg:
//
//         <column number='0' align='centre'>
//         </column>
	}
	else if (currentState & (eInLayoutBand | eInBlock)) {
		assert(mCurrentLayout);
		oofRepTextStyle* textStyle;
		if (mCurrentCSSID.isEmpty())
			textStyle = 0;
		else
			textStyle = new oofPromisedRepTextStyle(mCurrentCSSID);
			
		const long pageSymbolAt = mCharData.find("^P");
		if (pageSymbolAt==oofString::kNotFound)
			mLastBlock = new oofRepTextBlock(mCharData, textStyle);
		else {
			const oofString leadingText = mCharData.subString(0, pageSymbolAt /* count */);
			const oofString trailingText = mCharData.subString(pageSymbolAt+2  /* to end */);
			mLastBlock = new oofRepPageNoBlock(leadingText, trailingText, textStyle);			
		}
		
		mLastBlock->align(mCurrentBlockAlign);
		if (mCurrentBlockFixedWidth>0)
			mLastBlock->fixedWidth(mCurrentBlockFixedWidth);
			
		if (textStyle)
			mLastBlock->ID(mCurrentCSSID);
		*mCurrentLayout << mLastBlock; 
	}
	
	mCharData.clear();
}//end chardata	


void
OOF_XMLrepLayoutParser::endElement(const char* name)
{
	ParseAccumulatedCharData();
	
	const unsigned long currentState = mBuildState->getRepBuildState();
// order in decreasing frequency of use

	if(currentState & eInColumn) {  // optimise a bit by using state as quick check
		if(strcmp(name,"column")==0)
			mBuildState->restoreOldState();
		else if(strcmp(name,"header")==0)
			mBuildState->clear(eInHeader);
		else if(strcmp(name,"width")==0)
			mBuildState->clear(eInWidth);
		else if(strcmp(name,"customDrawer")==0) {
			mBuildState->clear(eInCustomCode);
			if (mCurrentSerializable) {
				oofRepViewBand* theBand = CurrentViewBand();
				oofRepCustomViewDrawer* theDrawer = dynamic_cast<oofRepCustomViewDrawer*>(mCurrentSerializable);
				assert(theDrawer);
				theBand->customDrawer(mCurrentColNumber, theDrawer); 
			}
		}
		else
			assert(!"unrecognised state inside a column");
	}
	else if(strcmp(name,"inline")==0){
		mBuildState->clear(eInBlock);
	}
	else if(strcmp(name,"BR")==0){
		if (mLastBlock)
			mLastBlock->moveDownAfter(true);
	}
	else if(strcmp(name,"header")==0){
		if(currentState & eInFirstPageHeader)
			mBuildState->clear(eInFirstPageHeader);
		else 
			mBuildState->clear(eInHeader);
	}
	else if(strcmp(name,"footer")==0){
		if(currentState & eInFirstPageFooter)
			mBuildState->clear(eInFirstPageFooter);
		else 
			mBuildState->clear(eInFooter);
	}
	else if(strcmp(name,"table")==0){
		CloseTable();
	}
	else if(strcmp(name,"box")==0){
		mBuildState->clear(eInLayoutBand);
		CloseLayout();
	}
	else if(strcmp(name,"headerBreaks")==0){
		mBuildState->clear(eInHeaderBreak);
	}
	else if(strcmp(name,"footerBreaks")==0){
		mBuildState->clear(eInFooterBreak);
	}
	else if(strcmp(name,"break")==0){
		BuildBreak();
	}
	
// ignore HR, graph
}// end element


void
OOF_XMLrepLayoutParser::BuildBreak()
{
	assert(mCurrentBreakBand);
	const int currentState = mBuildState->getRepBuildState();
	oofRepBreak* newBreak = new oofRepBreakEveryRecord(mCurrentBreakBand, mCurrentBreakNewPageEveryTime);
// NOT YET IMPLEMENTED - choice of break types

	oofRepViewBand* theViewBand = CurrentViewBand();
	assert(theViewBand);  // can only have a break occur within a view band
	
	if (currentState&eInHeaderBreak) {
		theViewBand->headerBreaks() << newBreak;
	}
	else {
		assert(currentState&eInFooterBreak);
		theViewBand->footerBreaks() << newBreak;
	}

	mCurrentBreakBand=0;
	mCurrentBreakNewPageEveryTime=false;
}


void
OOF_XMLrepLayoutParser::OpenColumn(const char** atts)
{
	// we ALWAYS have a column number attribute, but bow to the precedent in
	// other elements that an ID (if present) is the first attribute
	mBuildState->saveOldState(); 
	mBuildState->set(eInColumn);

	int colNumberAtt = 0;	
	if (atts[0]) {
		if(strcmp(atts[0], "ID")==0) {
			mCurrentCSSID = atts[1];
			colNumberAtt = 2;
		}
	}
	else
		mCurrentCSSID = "";

	assert(atts[0] && (strcmp(atts[colNumberAtt], "number")==0));
	mCurrentColNumber = atoi(atts[colNumberAtt+1]);
	
	oofColSizer::colAlignT colAlign;
	const int alignAt = colNumberAtt+2;
	if(atts[alignAt] && (strcmp(atts[alignAt], "align")==0)) {
		const int alignValueAtt = alignAt+1;
		assert(atts[alignValueAtt]);  // 2nd half of pair
		if(strcmp(atts[alignValueAtt], "centre")==0)
			colAlign = oofColSizer::alignCentre;
		else if(strcmp(atts[alignValueAtt], "right")==0)
			colAlign = oofColSizer::alignRight;
		else if(strcmp(atts[alignValueAtt], "left")==0)
			colAlign = oofColSizer::alignLeft;
		else
			colAlign = oofColSizer::alignJust;

		oofRepViewBand* theBand = CurrentViewBand();
		assert(theBand);
		dbView* theView = theBand->view();
		theView->colSizer()->setAlignment(mCurrentColNumber, colAlign);
	}  // has alignment	
	mCurrentSerializable = 0;
}


void
OOF_XMLrepLayoutParser::BuildBlock(const char** atts)
{
	int likelyAlignAtt = 0;
	int likelyWidthAtt = 0;
	if (atts[0] && (strcmp(atts[0], "ID")==0)) {
			mCurrentCSSID = atts[1];
			likelyWidthAtt = likelyAlignAtt = 2;  // assumes ID always precedes other attributes
	}
	else
		mCurrentCSSID = "";
	
	
	if(atts[likelyAlignAtt] && (strcmp(atts[likelyAlignAtt], "align")==0)) {
		const int alignValueAtt = likelyAlignAtt+1;
		if(strcmp(atts[alignValueAtt], "centre")==0)
			mCurrentBlockAlign = oofRepBlock::alignCentre;
		else if(strcmp(atts[alignValueAtt], "right")==0)
			mCurrentBlockAlign = oofRepBlock::alignRight;
		else
			mCurrentBlockAlign = oofRepBlock::alignLeft;
			
		likelyWidthAtt +=2;  // is after Align
	}
	else
		mCurrentBlockAlign = oofRepBlock::alignLeft;  // default if not specified

	if(atts[likelyWidthAtt] && (strcmp(atts[likelyWidthAtt], "fixedWidth")==0)) {
		const int widthValueAtt = likelyWidthAtt+1;
		assert(atts[widthValueAtt]);
		sscanf(atts[widthValueAtt], "%d", &mCurrentBlockFixedWidth);
	}
	else
		mCurrentBlockFixedWidth = 0;

	mBuildState->set(eInBlock);
}


/**
	Append band to single or list location depending on state.
	\see OOF_XMLrepLayoutParser::CloseTable for similar logic for view bands
*/
void
OOF_XMLrepLayoutParser::AppendBandDependingOnState(oofRepBand* orphanedBand)
{
	const int currentState = mBuildState->getRepBuildState();

	if (currentState & (eInFooterBreak|eInHeaderBreak))  // regardless of other states
		mCurrentBreakBand = orphanedBand; 
	else {
		oofRepBandList* theBands = CurrentBandListDependingOnState();
		if (theBands)
			theBands->append(orphanedBand);
		else {
			assert(!"OOF_XMLrepLayoutParser::AppendBandDependingOnState called with unknown state");
		}
	}
}


oofRepBandList*
OOF_XMLrepLayoutParser::CurrentBandListDependingOnState() const 
{
 	const int maskedState = mBuildState->getRepBuildState() &  
		(eInFirstPageHeader|eInFirstPageFooter
		|eInHeader|eInTable|eInFooter
		|eInFooterBreak|eInHeaderBreak
	);

	oofRepBandList* ret;
	switch(maskedState){	
		case eInFirstPageHeader:
			ret = &mReport->firstPageHeaders();
		break;
				
		case eInHeader:
			ret = &mReport->pageHeaders();
		break;		
		
		case (eInHeader | eInTable): {
			oofRepViewBand* theViewBand = CurrentViewBand();
			ret = &theViewBand->headers();
		}
		break;	
			
		case (eInFooter | eInTable): {
			oofRepViewBand*theViewBand = CurrentViewBand();
			ret = &theViewBand->footers();
		}
		break;	
			
		case eInFirstPageFooter:
			ret = &mReport->firstPageFooters();
		break;	
		
		case eInFooter:
			ret = &mReport->pageFooters();
		break;	

		default:
			ret = 0;
	}
	return ret;
}


void
OOF_XMLrepLayoutParser::BuildGraphBand(const char** atts)
{
// eg: <graph ID='1-1-1-1-1'  type="bar" source="templateProgressData" height='400' graphWidth='400' graphHeight='246'/>

// NASTY ASSUMPTIONS TO ATTRIBUTE ORDER, SO PROTECT THEM
	assert(atts[0] && atts[1] && (strcmp(atts[0], "ID")==0));
	assert(atts[2] && atts[3] && (strcmp(atts[2], "type")==0));
	assert(atts[4] && atts[5] && (strcmp(atts[4], "source")==0));  // also in GetDatabaseInContext below
	assert(atts[6] && atts[7] && (strcmp(atts[6], "height")==0));
	assert(atts[8] && atts[9] && (strcmp(atts[8], "graphWidth")==0));
	assert(atts[10] && atts[11] && (strcmp(atts[10], "graphHeight")==0));
	assert(atts[12] && atts[13] && (strcmp(atts[12], "title")==0));
	
	mCurrentCSSID = atts[1];
	const char* typeStr = atts[3];
	dbTable* theTable = GetDatabaseInContext(atts, 4);  // cope with nested data in breaks, etc.
	assert(theTable);  // otherwise <graph> refers to different DB schema
	dbView* adoptedView = theTable->makeView();  // assumption is that table represents the view we saved
	
	const int bandHeight = atoi(atts[7]);
	const int graphWidth = atoi(atts[9]);
	const int graphHeight = atoi(atts[11]);
	const oofString graphTitle = atts[13];

// optional overlay attributes
	int numOverlaid = 0;
	dbView* overlayView = 0;
	if (atts[14] && atts[15] && (strcmp(atts[14], "numOverlaid")==0)) 
		numOverlaid = atoi(atts[15]);
	else if (atts[14] && atts[15] && (strcmp(atts[14], "overlaySource")==0)) {
// has an overlay view with second database source
		dbTable* theTable = GetDatabaseInContext(atts, 14, "overlaySource");
		assert(theTable);  
		overlayView = theTable->makeView();
	}
	
	oofGraph* orphanedGraph=0;
	if (strcmp(typeStr, "bar")==0) {
		orphanedGraph = new oofBarGraph(adoptedView);
	}
	else if (strcmp(typeStr, "column")==0) {
		orphanedGraph = new oofColumnGraph(adoptedView);
	}
	else if (strcmp(typeStr, "point")==0) {
		orphanedGraph = new oofPointGraph(adoptedView);
	}
	else if (strcmp(typeStr, "line")==0) {
		orphanedGraph = new oofLineGraph(adoptedView);
	}
	else if (strcmp(typeStr, "XY")==0) {
		orphanedGraph = new oofXYGraph(adoptedView);
	}
	else if (strcmp(typeStr, "stacked bar")==0) {
		oofStackedBarGraph* theBarGraph = new oofStackedBarGraph(adoptedView);  
		if (numOverlaid)
			theBarGraph->setOverlaid(numOverlaid);
		else if (overlayView)
			theBarGraph->setOverlayView(overlayView);  // currently copied
		orphanedGraph = theBarGraph;	
	}
	else if (strcmp(typeStr, "ordinal stacked bar")==0) {
		oofOrdinalStackedBarGraph* theBarGraph = new oofOrdinalStackedBarGraph(adoptedView); 
		if (numOverlaid)
			theBarGraph->setOverlaid(numOverlaid);
		else if (overlayView)
			theBarGraph->setOverlayView(overlayView);  // currently copied
		orphanedGraph = theBarGraph;	
	}
	else if (strcmp(typeStr, "pie")==0) {
		orphanedGraph = new oofPieGraph(adoptedView);
	}
	else
		dbConnect::raise (
			oofString("Unknown graph type in OOF_XMLrepLayoutParser::BuildGraphBand: ", typeStr)
		);

	assert(orphanedGraph);
	orphanedGraph->setTitle(graphTitle);
	new oofPromisedGraphSettings(mCurrentCSSID, orphanedGraph);  // self-deleting listener
	oofRepBand* adoptedBand = new oofRepGraphBand(orphanedGraph, bandHeight, graphHeight, graphWidth);
	if (!mCurrentCSSID.isEmpty())
		adoptedBand->ID(mCurrentCSSID);
	AppendBandDependingOnState(adoptedBand); 
	
	delete overlayView;  // overlay views currently cloned, not adopted but this will change!
}


void
OOF_XMLrepLayoutParser::BuildSpaceBand(const char** atts)
{
	mCurrentCSSID = "";
	int heightAtt;
	if (atts[2]==0)
		heightAtt = 1;
	else
		heightAtt = 3;
	assert(atts[heightAtt-1] && (strcmp(atts[heightAtt-1],"height")==0));
	const int spaceHeight = atoi(atts[heightAtt]);

	oofRepBand* adoptedBand = new oofRepSpaceBand(spaceHeight);
	if (!mCurrentCSSID.isEmpty())
		adoptedBand->ID(mCurrentCSSID);
	AppendBandDependingOnState(adoptedBand); 
}


void
OOF_XMLrepLayoutParser::BuildLineBand(const char** atts)
{
	mCurrentCSSID = "";
	int heightAtt;
	if (atts[2]==0)
		heightAtt = 1;
	else
		heightAtt = 3;
	assert(atts[heightAtt-1] && (strcmp(atts[heightAtt-1],"height")==0));
	const int lineHeight = atoi(atts[heightAtt]);	

	oofRepBand* adoptedBand = new oofRepLineBand(lineHeight);
	if (!mCurrentCSSID.isEmpty())
		adoptedBand->ID(mCurrentCSSID);
	AppendBandDependingOnState(adoptedBand); 
}


void 
OOF_XMLrepLayoutParser::CloseTable()
{
	const int currentLevel = mTablesStack.count()-1;//child
	const int kTopLevel = 0;
	if(currentLevel > kTopLevel){
		mBuildState->restoreOldState();
		const int currentState = mBuildState->getRepBuildState();
		oofRepBand* orphanedBand = mTablesStack.value(currentLevel);
		mTablesStack.deleteItem(currentLevel);
		if(currentState & (eInFooterBreak|eInHeaderBreak)){
			mCurrentBreakBand = orphanedBand;
		}
		else {
			oofRepViewBand* parentBand = CurrentViewBand();
			if(currentState & eInHeader){
				parentBand->headers() << orphanedBand;
			}
			else if(currentState & eInFooter){
				parentBand->footers() << orphanedBand;
			}
			else
				assert(!"Unknown nested state in OOF_XMLrepLayoutParser::CloseTable");
		}
	}
	else{  // at top
		oofRepViewBand* topViewBand = CurrentViewBand();
		mReport->body(topViewBand);
		mTablesStack.deleteItem(kTopLevel);
		mBuildState->clear(eInTable);
	}
}//CloseTable


void
OOF_XMLrepLayoutParser::OpenLayout(const char** atts)
{ 
	mBuildState->set(eInLayoutBand);
	mCurrentLayout = new oofRepLayoutBand;
	if (atts[0]) {
		assert(atts[0] && (strcmp(atts[0], "ID")==0));
		mCurrentCSSID = atts[1];
		mCurrentLayout->textStyle(new oofPromisedRepTextStyle(mCurrentCSSID));
		mCurrentLayout->ID(mCurrentCSSID);
	}
}


void
OOF_XMLrepLayoutParser::CloseLayout()
{ 
	AppendBandDependingOnState(mCurrentLayout); 
	mCurrentLayout = NULL;
}


void
OOF_XMLrepLayoutParser::OpenHeaderOrFooter(const char** atts, int firstState, int otherState)
{
	int likelyKeepAt = 0;
	if(atts[0] && (strcmp(atts[1],"first")==0)) {
		mBuildState->set(firstState);
		likelyKeepAt = 2;
	}
	else 
		mBuildState->set(otherState);// if not in table it's in page	
		
	if(atts[likelyKeepAt] && (strcmp(atts[likelyKeepAt],"keepTogether")==0)) {
		const bool willKeep = dbBool::str2bool(atts[likelyKeepAt+1]);
		oofRepBandList* theBands = CurrentBandListDependingOnState();
		if (theBands)
			theBands->keepTogether(willKeep);  // only record if not default of together
	}
	
}


// -------------------------------------------------------
//      O O F _ X M L r e p S t y l e P a r s e r
// -------------------------------------------------------

/**
	parse the <style> element to read mainly text styles.
	Also reads report and graph settings.
	Reconciles these ID's with the ID attribute of layout <elements>.
	\see oofRepTextStyleCompleter
*/
OOF_XMLrepStyleParser::OOF_XMLrepStyleParser(expatppNesting* parent, oofString& s) :
	expatppNesting(parent),
	mCallerString(s)
{
	mCallerString = "";  // in case we get no content
}


// char Data call back
void 
OOF_XMLrepStyleParser::charData(const XML_Char *s, int len)
{
	if (emptyCharData(s, len))
		return;
	mCallerString.append(s,len);  // multiple lines call charData multiple times
}//end chardata	


// -------------------------------------------------------
//         O O F _ X M L r e p P a r s e r S t a t e
// -------------------------------------------------------
OOF_XMLrepParserState::OOF_XMLrepParserState():
mRepGenState(0)
{
}

void 
OOF_XMLrepParserState::saveOldState(bool clearNew)
{
	mOldState.append(mRepGenState);
	if (clearNew)
		mRepGenState = 0;
}


void 
OOF_XMLrepParserState::restoreOldState()
{		
	const unsigned long numEntries = mOldState.count();
	assert(numEntries>0);
	const unsigned long currentPos = numEntries-1;
	mRepGenState = mOldState.value(currentPos);
	mOldState.deleteItem(currentPos);
}


// -------------------------------------------------------
//      o o f C S S 2 R e p T e x t S t y l e
// -------------------------------------------------------
void
oofCSS2RepTextStyle::parseStyleText(const oofString& srcString, oofRepTextStyle* inStyle)
{
//make a temp str and copy the block into it 
// this makes parseing easier -  we can use strTok and strstr with no danger of them marcher of therought the whole 
// src string or wothout clobering thesrcString
	assert(!srcString.isEmpty());
	assert(srcString[0]=='{');
	oofString finder = srcString.subStringBetween('{', '}');
	mParseBuffer = finder.orphan();  // need a char string to avoid complaints in C routines	

	const unsigned short fontSize = FindFontSize();
	inStyle->fontName( FindFontFamily() );
	inStyle->fontSize( fontSize );
	inStyle->leading( CalculateLeading(fontSize, FindLineHeight() ) );
	inStyle->fontStyle( MakeStyleNumber() );
	inStyle->color( FindFontColour() );
	inStyle->alignment( FindAlignment() );
	inStyle->finishConstruction();
	
	delete[] mParseBuffer;	
}


oofString
oofCSS2RepTextStyle::FindFontFamily()
{ 
	oofString fontFamilyName;
	const char * fontStr = "font-family: ";
	const int kNameLen = 13;  // sizeof("font-family: ")
	char *result;
	char theChar;
	int index=0;
	 //Must zero string in case it's already been used
	fontFamilyName = "";

	//Find start of string "font-family: "
	//Set result = start of above string
	result = strstr(mParseBuffer,fontStr);
	if (result) {
		result +=kNameLen;
		theChar = result[index++];
		while(isprint(theChar)&&(theChar!=';'))  // printingchars only 
		{
			if(theChar != '\''){ //ignore quotes around font family names with spaces
				fontFamilyName += theChar;
			}
			theChar = result[index++];
		
		}
		return fontFamilyName;
	}
	else
		return oofRep::defaultSettings()->defaultTextStyle()->fontName();			
}//findFontFamily


short 
oofCSS2RepTextStyle::FindFontSize()
{
	const char* sizeStr = "font-size:";// CHECK SPCE ON END OF STRING
	char *result;
	int index=0;
	short fontSize;
	 
		//Find start of string "font-size:r"
	//Set result = start of above string
	result = strstr(mParseBuffer,sizeStr);
	if (result) {
		result +=11;
		fontSize = atoi(result);
		return fontSize;
	}
	else
		return oofRep::defaultSettings()->defaultTextStyle()->fontSize();
}//fontsize


float 
oofCSS2RepTextStyle::FindLineHeight()
{
	const char* sizeStr = "line-height:";
	char *result;
	int index=0;
	float lineHeight=1.0;

	result = strstr(mParseBuffer,sizeStr);
	if(result){
		result += 13;
		lineHeight= atof(result);
	}
	return lineHeight;	
}//LineHeight


short 
oofCSS2RepTextStyle::CalculateLeading(short fontSize,float lineHeight) const
{
	return (lineHeight*fontSize)-fontSize;
		
}//calculateLeading


unsigned char
oofCSS2RepTextStyle::MakeStyleNumber(){
	unsigned char styleNumber = 0;
	
	if (strstr(mParseBuffer,"font-weight: bold")){
		styleNumber = styleNumber|bold;
	}
	if(strstr(mParseBuffer,"font-style: italic")) {
		styleNumber = styleNumber|italic;
	}	
	if (strstr(mParseBuffer,"text-decoration: underline")) {
		styleNumber = styleNumber|underline;
	}
	if (strstr(mParseBuffer,"text-shadow:")){	
		styleNumber = styleNumber|shadow;
	}
	//if (strstr(mScratch,"stylenumber&  outline")) {
	//	cat to"text-shadow: 0.2em 0.2em :theStyleString
	//}
	if (strstr(mParseBuffer,"font-stretch: condensed)stylenumber& condense")) {
		styleNumber = styleNumber|condense;
	}
	if (strstr(mParseBuffer,"font-stretch: expanded)stylenumber& extend")) {
		styleNumber = styleNumber| extend;
	}		
	return styleNumber;
}


oofColor
oofCSS2RepTextStyle::FindFontColour()
{ 
	unsigned int rgbArray[3];
	const char fontStr[] = "color: rgb(";
	const int kNameLen = sizeof(fontStr)-1;  // subtract 1 for null
	char *result;
	int index=0;
	
	//Find start of string "color: rgb("
	//Set result = start of above string
	result = strstr(mParseBuffer,fontStr);
	if (result) {
		//jump over the string
		result +=kNameLen;
		sscanf(result,"%d,%d,%d",rgbArray,rgbArray+1,rgbArray+2);
		return oofColor(rgbArray[0],rgbArray[1],rgbArray[2]);
	}
	else
		return oofRep::defaultSettings()->defaultTextStyle()->color();
}//findFontColour


oofColSizer::colAlignT 
oofCSS2RepTextStyle::FindAlignment()
{ 
	const char* fontStr = "text-align: ";  // WARNING highly vulnerable parsing
	const int kNameLen = 12;  // sizeof("text-align: ")
	const char* result;
	char theChar;
	unsigned long hashedIndex=0;
	oofColSizer::colAlignT ret;
	//Find start of string "text-align: "
	//Set result = start of above string
	result = strstr(mParseBuffer,fontStr);
	if (result) {
		//jump over the string
		result += kNameLen;
		while(!isalpha(theChar = *result)) {  // find the align string which follows text-align: 
			result++;
		}	
		while(isalpha(theChar = *result)) {
			hashedIndex += theChar;
			result++;
		}
		switch(hashedIndex){// as sums are unique for above strings
			case (int) 'l'+'e'+'f'+'t':  // 427
				ret = oofColSizer::alignLeft;
			break;
			case (int) 'c'+'e'+'n'+'t'+'r'+'e':  // 641
				ret = oofColSizer::alignCentre;
			break;
			case (int) 'r'+'i'+'g'+'h'+'t':  //542
				ret = oofColSizer::alignRight;
			break;
			case (int) 'j'+'u'+'s'+'t':  // 782
				ret = oofColSizer::alignJust;
			break;
		
			default:
				ret = oofColSizer::alignLeft;
			break;
		}
	}
	else
		ret = oofRep::defaultSettings()->defaultTextStyle()->alignment();
	return ret;	
}


// -------------------------------------------------------
//      o o f C S S 2 G r a p h S e t t i n g s
// -------------------------------------------------------

/**
	Parse text representation of settings to override only those which are parsed.
	Leaves oofGraphSettings as overlay falling through to defaults if there are not
	matching overrides in the text.
*/
void
oofCSS2GraphSettings::parseSettingsText(const oofString& srcString, oofGraphSettings* theSettings)
{
	char* res;
	//make a temp str and copy the block into it 
	// this makes parseing easier we can use strTok and strstr with no danger of them marcher of therought the whole 
	// src string or wothout clobbering the srcString
	assert(!srcString.isEmpty());
	assert(srcString[0]=='{');
	oofString finder = srcString.subStringBetween('{', '}');
	char* parseBuffer = finder.orphan();  // need a char string to avoid complaints in C routines	
	
	// not fast or smart might fix in future
	res = strstr(parseBuffer, "hasLegend:");
	if(res)
		theSettings->hasLegend(ParseBoolString(res));
	
	res = strstr(parseBuffer, "has3DEffect:");
	if(res)
		theSettings->has3DEffect(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasTitle:");
	if(res)
		theSettings->hasTitle(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasValuesOverColumns:");
	if(res)
		theSettings->hasValuesOverColumns(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasValuesNextToBars:");
	if(res)
		theSettings->hasValuesNextToBars(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasXLabel:");
	if(res)
		theSettings->hasXLabel(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasYLabel:");
	theSettings->hasYLabel(ParseBoolString(res));
	
	res = strstr(parseBuffer, "hasXAxisTicks:");
	if(res)
		theSettings->hasXAxisTicks(ParseBoolString(res));
	
		
	res = strstr(parseBuffer, "hasYAxisTicks:");
	if(res)
		theSettings->hasYAxisTicks(ParseBoolString(res));
	
		
	res = strstr(parseBuffer, "hasXTickLabels:");
	if(res)
		theSettings->hasXTickLabels(ParseBoolString(res));
	
		
	res = strstr(parseBuffer, "hasYTickLabels:");
	if(res)
		theSettings->hasYTickLabels(ParseBoolString(res));
	
	res = strstr(parseBuffer, "titlePos:");
	if(res)
		theSettings->titlePos(ParsePosString(res));
	
	res = strstr(parseBuffer, "XLabelPos:");
	if(res)
		theSettings->XLabelPos(ParsePosString(res));
	
	res = strstr(parseBuffer, "YLabelPos:");
	if(res)
		theSettings->YLabelPos(ParsePosString(res));
	
	res = strstr(parseBuffer, "the3DEffectMultiplier:");
	if(res){
		res += 22;
		theSettings->the3DEffectMultiplier(atof(res));
	}
	
	res = strstr(parseBuffer, "legendOffset:");
	if(res){
		res += 13;
		theSettings->legendOffset(atoi(res));
	}
	
	res = strstr(parseBuffer, "maxYLabelLen:");
	if(res){
		res += 13;
		theSettings->maxYLabelLen(atoi(res));
	}
	res = strstr(parseBuffer, "maxYTickLabelLen:");
	if(res){
		res += 17;
		theSettings->maxYTickLabelLen(atoi(res));
	}
	res = strstr(parseBuffer, "maxLegendLen:");
	if(res){
		res += 13;
		theSettings->maxLegendLen(atoi(res));
	}
	res = strstr(parseBuffer, "minorTickSize:");
	if(res){
		res += 14;
		theSettings->minorTickSize(atoi(res));
	}
	res = strstr(parseBuffer, "majorTickSize:");
	res += 14;
	theSettings->majorTickSize(atoi(res));
	
	res = strstr(parseBuffer, "YAxisHeight:");
	if(res){
		res += 12;
		theSettings->YAxisHeight(atoi(res));
	}
	res = strstr(parseBuffer, "intTickLabelMask:");
	if(res){
		res += 17;
		const oofString remainingStr = res;
		theSettings->intTickLabelMask(remainingStr.subStringBetween('\'', '\''));
	}
	res = strstr(parseBuffer, "realTickLabelMask:");
	if(res){
		res += 18;
		const oofString remainingStr = res;
		theSettings->realTickLabelMask(remainingStr.subStringBetween('\'', '\''));
	}
	delete[] parseBuffer;
}


bool
oofCSS2GraphSettings::ParseBoolString(const char* searchStr)
{
// takes string of form  "someTag: true ;... "
// where .. implies further settings before null
// and extracts the true or false setting

	char theChar;
	unsigned int sum=0;
	while(isalnum(theChar = *searchStr))  // skip "someTag"
		searchStr++;
	while(!isalpha(theChar = *searchStr))  // skip ": " to start of true/false
		searchStr++;

	const char truthValue = *searchStr;  // just check first char
	const bool ret = dbBool::char2bool(truthValue);  // cope with Y/N, 1/0, T/F etc.
	return ret;
}


oofGeo::PositionState	
oofCSS2GraphSettings::ParsePosString(const char* searchStr)
{
	char theChar;
	unsigned int sum=0;
	while(isalpha(*searchStr))
		searchStr++;
	while(!isalpha(theChar = *searchStr))
		searchStr++;
	while(isalpha(theChar = *searchStr)){
		sum += theChar;
		searchStr++;
	}
	oofGeo::PositionState ret; 
	switch(sum)
	{
	case (int) 'p'+'o'+'s'+'N'+'o'+'n'+'e':  // 738:
		ret = oofGeo::posNone;
	break;
	case (int) 'p'+'o'+'s'+'T'+'o'+'p':  // 645:
		ret = oofGeo::posTop;
	break;
	case (int) 'p'+'o'+'s'+'L'+'e'+'f'+'t':  // 733:
		ret = oofGeo::posLeft;
	break;
	case (int) 'p'+'o'+'s'+'R'+'i'+'g'+'h'+'t':  // 848:
		ret = oofGeo::posRight;
	break;
	case (int) 'p'+'o'+'s'+'B'+'o'+'t'+'t'+'o'+'m':  // 851:
		ret = oofGeo::posBottom;
	break;
	case (int) 'p'+'o'+'s'+'B'+'e'+'l'+'o'+'w':  // 843:
		ret = oofGeo::posBelow;
	break;
	case (int) 'p'+'o'+'s'+'T'+'o'+'L'+'e'+'f'+'t':  // 928:
		ret = oofGeo::posToLeft;
	break;
	case (int) 'p'+'o'+'s'+'C'+'e'+'n'+'t'+'r'+'e':  // 947:
		ret = oofGeo::posCentre;
	break;
	default:
		ret = oofGeo::posNone;
	}
	return ret;		
}







// -------------------------------------------------------
//     o o f C S S 2 R e p o r t S e t t i n g s
// -------------------------------------------------------

void
oofCSS2ReportSettings::parseSettingsText(const oofString& srcString, oofRepSettings* reportSettings)
{
	// If we have a collision then we must just resort to an if(strcmp.. chain in following switch 
	assert(!srcString.isEmpty());
	assert(srcString[0]=='{');
	oofString finder = srcString.subStringBetween('{', '}');
	char* parseBuffer = finder.orphan();  // need a char string to avoid complaints in C routines	
	
	char *delims = "{:;}\\ ";//
	char *token;
	unsigned int HashResult;
	int value;
	// new better alg
	token = strtok( parseBuffer, delims );
	while ( token != NULL ) {
		HashResult = Hash(token);
		switch(HashResult){  // ordered as per oofRepSettings method decl
			case (int) 'r'+'p'+'L'+'i'+'n'+'e'+'T'+'h'+'i'+'c'+'k'+'n'+'e'+'s'+'s':    // 1558
				token = strtok( NULL, delims );
				value = atoi(token);
				reportSettings->lineThickness(value);
			break;

			case (int) 'r'+'p'+'P'+'a'+'g'+'e'+'H'+'e'+'i'+'g'+'h'+'t':    // 1208
				token = strtok( NULL, delims );
				reportSettings->pageHeight(atoi(token));
			break;

			case (int) 'r'+'p'+'P'+'a'+'g'+'e'+'W'+'i'+'d'+'t'+'h':    // 1119
				token = strtok( NULL, delims );
				reportSettings->pageWidth(atoi(token));
			break;

			case (int) 'r'+'p'+'B'+'o'+'r'+'d'+'e'+'r'+'T'+'y'+'p'+'e':    // 1250
				token = strtok( NULL, delims );
				reportSettings->borderType(atoi(token));
			break;

			case (int) 'r'+'p'+'B'+'a'+'n'+'d'+'I'+'n'+'d'+'e'+'n'+'t'+'i'+'n'+'g':    // 1527
				token = strtok( NULL, delims );
				reportSettings->bandIndenting(atoi(token));
			break;
			
			case (int) 'r'+'p'+'L'+'e'+'f'+'t'+'M'+'a'+'r'+'g'+'i'+'n':    // 1227
			token = strtok( NULL, delims );
			reportSettings->leftMargin(atoi(token));
			break;

			case (int) 'r'+'p'+'R'+'i'+'g'+'h'+'t'+'M'+'a'+'r'+'g'+'i'+'n':    // 1342
				token = strtok( NULL, delims );
				reportSettings->rightMargin(atoi(token));
			break;

			case (int) 'r'+'p'+'T'+'o'+'p'+'M'+'a'+'r'+'g'+'i'+'n':    // 1139
				token = strtok( NULL, delims );
				reportSettings->topMargin(atoi(token));
			break;
			
			case (int) 'r'+'p'+'B'+'o'+'t'+'t'+'o'+'m'+'M'+'a'+'r'+'g'+'i'+'n':    // 1461
				token = strtok( NULL, delims );
				reportSettings->bottomMargin(atoi(token));
			break;
			
			case (int) 'r'+'p'+'C'+'o'+'l'+'S'+'e'+'p'+'W'+'i'+'d'+'t'+'h':    // 1320
				token = strtok( NULL, delims );
				reportSettings->colSepWidth(atoi(token));
			break;

			case (int) 'r'+'p'+'B'+'l'+'o'+'c'+'k'+'V'+'e'+'r'+'t'+'S'+'e'+'p':    // 1430
				token = strtok( NULL, delims );
				reportSettings->blockVertSep(atoi(token));
			break;
			
			case (int) 'r'+'p'+'D'+'e'+'f'+'a'+'u'+'l'+'t'+'T'+'e'+'x'+'t'+'S'+'t'+'y'+'l'+'e':    // 1885
				token = strtok( NULL, delims );
				reportSettings->defaultTextStyle(new oofPromisedRepTextStyle(token));
			break;

// defaultPageHeaders skipped - saved in <layout>
// defaultPageFooters skipped

			case (int) 'r'+'p'+'D'+'r'+'a'+'w'+'H'+'e'+'a'+'d'+'i'+'n'+'g'+'s'+'D'+'o'+'w'+'n'+'T'+'o'+'L'+'e'+'v'+'e'+'l':   
				token = strtok( NULL, delims );
				reportSettings->drawHeadingsDownToLevel(atoi(token));
			break;
			
			case (int) 'r'+'p'+'P'+'r'+'i'+'n'+'t'+'G'+'r'+'a'+'p'+'h'+'s'+'I'+'n'+'M'+'o'+'n'+'o':    // 1956
				token = strtok( NULL, delims );
				if(strcmp(token,"true")==0)
					reportSettings->printGraphsInMono(true);
				else 
					reportSettings->printGraphsInMono(false);
				break;

			case (int) 'r'+'p'+'M'+'o'+'d'+'a'+'l'+'P'+'r'+'e'+'v'+'i'+'e'+'w'+'W'+'i'+'n'+'d'+'o'+'w':    
				token = strtok( NULL, delims );
				if(strcmp(token,"true")==0)
					reportSettings->modalPreviewWindow(true);
				else 
					reportSettings->modalPreviewWindow(false);
			break;

			case (int) 'r'+'p'+'G'+'r'+'a'+'p'+'h'+'B'+'a'+'n'+'d'+'M'+'i'+'n'+'S'+'i'+'z'+'e'+'P'+'e'+'r'+'c'+'e'+'n'+'t':   
				token = strtok( NULL, delims );
				reportSettings->graphBandMinSizePercent(atoi(token));
			break;		
			
// XMLsettings skipped - application code sets function pointer
			}//switch
		token = strtok( NULL, delims );
		
	}//while
	delete[] parseBuffer;	
}//ParseSettingsText


unsigned long 
oofCSS2ReportSettings::Hash(const char* theStr)
{
	int i =0;
	unsigned long res= 0;
	while(theStr[i] != NULL) {
		res += theStr[i++];
	}
	return res;
}


//-----------------------------------------
// o o f R e p P r o m i s e C o m p l e t e r
//-----------------------------------------
const char* 
oofRepPromiseCompleter::findStringForID(const oofString& theID, const char* inString)
{
	const char* result;
	oofString searchPattern ="#"; // avoids  id's emmbedded in css like rules 
	
	if(*theID != '#')
		searchPattern += theID;
	else 
		searchPattern = theID;
	const unsigned long searchLen = searchPattern.length();
	
	for (;;) {  // loop until match only pattern - may match start of larger pattern
		result = strstr(inString,searchPattern);  // could be "1-1-2" looking for "1-1"
		if (result==0)
			return 0; //**** FAILURE EXIT
			
		result += searchLen;
		const char charAfterPattern = *result;
		if ( (charAfterPattern=='-') ||
			 (charAfterPattern=='_') || 
			 isalnum(charAfterPattern)
		) 
			inString = result;  // adjust starting pos, matched substring so keep cycling
		else
			break;  // not part of larger ID
	}
	// find the opening {
	while(*result!='{')
		result++;
	return result;	
	 
}



//--------------------------------------------------
//	o o f G r a p h S e t t i n g s C o m p l e t e r
//---------------------------------------------------

oofGraphSettingsCompleter* oofGraphSettingsCompleter::sCurrentlyConstructing;	

oofGraphSettingsCompleter::oofGraphSettingsCompleter(const oofString& inCSS) :
		oofRepPromiseCompleter(inCSS)
{
	sCurrentlyConstructing = this;
}


oofGraphSettingsCompleter::~oofGraphSettingsCompleter() 
{
	sCurrentlyConstructing = 0;
}


void
oofGraphSettingsCompleter::completePromises()
{
	broadcast(kCompletePromiseMsg, (unsigned long)this);
}


void
oofGraphSettingsCompleter::completeSettings(const oofString& inID, oofGraph* inGraph) const
{
	assert(inGraph);
	oofGraphSettings* inSettings = inGraph->getLocalSettings();
	const char* settingsStartAt = findStringForID(inID);
	if (settingsStartAt)
		oofCSS2GraphSettings::parseSettingsText(settingsStartAt, inSettings);
}


//----------------------------------------------------
// o o f R e p T e x t S t y l e C o m p l e t er
//----------------------------------------------------

oofRepTextStyleCompleter* oofRepTextStyleCompleter::sCurrentlyConstructing;	


oofRepTextStyleCompleter::oofRepTextStyleCompleter(const oofString& inCSS) :
		oofRepPromiseCompleter(inCSS)
{
	sCurrentlyConstructing = this;
}


oofRepTextStyleCompleter::~oofRepTextStyleCompleter() 
{
	sCurrentlyConstructing = 0;
}


void 
oofRepTextStyleCompleter::completeStyle(oofPromisedRepTextStyle* inStyle) const
{
	const char* styleStartsAt = findStringForID(inStyle->ID());
	if (styleStartsAt) {
		oofCSS2RepTextStyle parser;
		parser.parseStyleText(styleStartsAt, inStyle);
	}
	// else will be caught by OOF_repXMLstyleCleanupVisitor
}


void
oofRepTextStyleCompleter::completePromises()
{
	broadcast(kCompletePromiseMsg, (unsigned long)this);
}



//---------------------------------------------------
//    o o f P r o m i s e d R e p T e x t S t y l e
//---------------------------------------------------
oofRepTextStyle* 
oofPromisedRepTextStyle::clone() const
{
	return new oofPromisedRepTextStyle(*this);
}


/**
	Receive broadcast from oofGraphSettingsCompleter::completePromises to get settings.
	Calls back to oofGraphSettingsCompleter::completeSettings to copy settings,
	that override the defaults in our local settings.
	Note it uses mID to locate the correct settings out of possible multiples.
*/
bool 
oofPromisedRepTextStyle::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	if (msg==OOFmsg_BroadcasterClosing) { 
		mListensTo = 0;
		// override parent because don't delete self
	}
	else {
		assert(msg==kCompletePromiseMsg);
		assert(sizeof(unsigned long)==sizeof(oofRepTextStyleCompleter*));
		oofRepTextStyleCompleter* completer = (oofRepTextStyleCompleter*) senderDefined;  // safe downcast
		completer->completeStyle(this);
	}		
	return true;
}
	

//---------------------------------------------------
//    o o f P r o m i s e d G r a p h S e t t i n g s
//---------------------------------------------------
/**
	Receive broadcast from oofGraphSettingsCompleter::completePromises to get settings.
	Calls back to oofGraphSettingsCompleter::completeSettings to copy settings,
	that override the defaults in our local settings.
	Note it uses mID to locate the correct settings out of possible multiples.
*/
bool 
oofPromisedGraphSettings::receiveMsg(OOFmsgT msg, unsigned long senderDefined)
{
	if(msg==kCompletePromiseMsg) {
		assert(sizeof(unsigned long)==sizeof(oofGraphSettingsCompleter*));
		oofGraphSettingsCompleter* completer = (oofGraphSettingsCompleter*) senderDefined;  // safe downcast
		completer->completeSettings(mID, mGraph);
	}
	else
		oofSingleListener::receiveMsg(msg, senderDefined);	
	return true;
}


// -------------------------------------------------------
//      O O F _ X M L r e p A d o r n e r P a r s e r
// -------------------------------------------------------
OOF_XMLrepAdornerParser::OOF_XMLrepAdornerParser(expatppNesting* parent, OOF_Dictionary& callerDict) :
	expatppNesting(parent),
	mCallerDict(callerDict)
{
}
	
		
void 
OOF_XMLrepAdornerParser::startElement(const char* name, const char** atts)
{
	assert(strcmp(name,"adorner")==0);  // only tags we should see are adorners
	assert(atts[0] && atts[1] && (strcmp(atts[0], "parentID")==0));
	mCurrentID = atts[1];
	mCurrentSerializable = 0;
	oofXMLserializableParser* theParser = new oofXMLserializableParser(this, &mCurrentSerializable);  // self-deleting sub parser
}


void 
OOF_XMLrepAdornerParser::endElement(const char* name)
{
	// this is after return from sub-parser
	assert(strcmp(name,"adorner")==0);  // only tags we should see are adorners
	if (!mCurrentSerializable)
		return;  // user code had unknown serializable
		
	oofAdorner* adoptedAdorner = dynamic_cast<oofAdorner*>(mCurrentSerializable);
	assert(adoptedAdorner);
	OOF_adornerHolder* existingHolder = (OOF_adornerHolder*)mCallerDict[mCurrentID];  // safe downcast
	if (existingHolder)
		existingHolder->adorners() << adoptedAdorner;
	else {
		OOF_adornerHolder* adoptedHolder = new OOF_adornerHolder(mCurrentID, adoptedAdorner);
		mCallerDict.append(adoptedHolder);
	}
}


#endif   // entire file conditional on OOF_READ_REPORTS
