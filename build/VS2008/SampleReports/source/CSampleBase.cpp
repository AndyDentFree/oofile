// CSampleBase.cpp : implementation file
// cross-platform part of Sample Reports
//

#include "stdafx.h"
#include "CSampleBase.h"

#ifdef _Windows
	#include "resource.h"
	#include "oofRepMFC.h"
	int CSampleBase::mReportChoice;
#else
	#include "oofGrphs.h"
	#include "oofRepPPWindow.h"
#endif
#ifndef H_OOFRPXML
	#include "oofrpXML.h"
#endif
#ifndef H_OOFRIXML
	#include "oofriXML.h"
#endif

#ifndef H_OOFREPEDIT
	#include "oofrepedit.h"
#endif

// -------------------------------------------------------
//        C A l t e r n a t e R o w s S h a d e 
//-------------------------------------------------------
class CAlternateRowsShade : public oofShadingAdorner {
public:
	CAlternateRowsShade() :
		oofShadingAdorner(15 /* percent */)
	{};
// use default copy ctor, dtor	
	virtual oofAdorner* clone() const;

	virtual void drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, AdornerCellCode);
};


oofAdorner*
CAlternateRowsShade::clone() const
{
	return new CAlternateRowsShade(*this);
}


void 
CAlternateRowsShade::drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, AdornerCellCode)
{
	// NOT YET IMPLEMENTED - custom adorner test
}


// -------------------------------------------------------
//            C D r a w B i g C h a r s
// -------------------------------------------------------
class CDrawBigChars : public oofRepCustomViewDrawer {
// sample custom drawer
// draws the field contents one char per row, in the font size specified at construction
public:
	CDrawBigChars(const oofRepTextStyle&);

// mandatory overrides
	virtual void draw(int left, int top);
	virtual void calculateWrapping(const dbField*, unsigned long sizedWidth, unsigned long& wrappedHeight);
	virtual unsigned long calcNextDrawHeight(unsigned long heightAvailable, unsigned long& heightUsed);  // returns  heightLeftToDraw

#ifdef _Windows
	virtual oofDE* makeDrawDE(int left, int top); 
#endif

// serialization methods
	static void registerSerializable();  
	static oofSerializable* makeDrawer(const oofString&);
	virtual oofString serialize() const;  
	virtual oofString signature() const;

private:
	oofRepTextStyle mStyle;
	unsigned short mStyleHeight;
	oofString mFieldStr;
	unsigned short mDrawFromChar, mCharsToDraw;
	bool mDrawerJustReset;
};


CDrawBigChars::CDrawBigChars(const oofRepTextStyle& inStyle) :
	mStyle(inStyle)
{}


void 
CDrawBigChars::draw(int left, int top)
{
// assumption is that when we are called we have cached how much to draw
// either from calculateWrapping or being called back calcNextDrawHeight

// WARNING a draw routine on Windows will be called to refresh the page
// so must NOT update the state - do that in the calculateWrapping and
// calcNextDrawHeight routines

// WARNING2 under Windows, the replay of this draw may occur after
// the environment has been trashed (try saving to file from preview window)
// so the following is dangerous:
#ifdef _Macintosh
	oofRep::currentReport()->environment()->changeTextStyle(&mStyle);
#else
	oofWinTextStyleDE::safeChangeStyle(&mStyle);  // static "replay-safe" method
#endif

	for (short i=0; i<mCharsToDraw; i++) {
		const char theChar = mFieldStr[mDrawFromChar+i];
#ifdef _Macintosh
		top += mStyleHeight;
		::MoveTo(left, top);
		::DrawChar(theChar);
#elif defined _Windows
		HDC		hdc = oofCurrentDraw::getCurrentDC();
		::SetBkMode(hdc, TRANSPARENT);
		::TextOut(hdc, left, top, &theChar, 1);
		top += mStyleHeight;  // Win draws top down, Mac on baseline???
#endif
	}
}


void 
CDrawBigChars::calculateWrapping(const dbField* inField, unsigned long /*sizedWidth*/, unsigned long& wrappedHeight)
{
	// reset what we draw
	mFieldStr = inField->copyString();
	mDrawFromChar = 0;
	mCharsToDraw = mFieldStr.length();
	
	// ignore sizedWidth in this sample - we draw each char in the style given
	// but a custom drawer of formatted text might calc its own wrapping here

	// defer this to here, a rendering-time activity to make sure we have a drawing environment
	mStyleHeight  = oofRep::currentReport()->environment()->heightOfTextStyle(&mStyle);
	wrappedHeight = mStyleHeight * mCharsToDraw;
	mDrawerJustReset = true;
}


unsigned long 
CDrawBigChars::calcNextDrawHeight(unsigned long heightAvailable, unsigned long& heightUsed)  // returns  heightLeftToDraw
{
// in our trivial example we have it easy but a formatted text renderer would probably have variable 
// height rows and so might have to re-measure the portion that was left
	if (mDrawerJustReset)
		mDrawerJustReset = false;
	else 
		mDrawFromChar += mCharsToDraw;  // adjust our relative position we just drew
	
	unsigned long retHeightLeftToDraw = 0;

	const unsigned int strLen = mFieldStr.length();
	if (mDrawFromChar>=strLen) {
// this would happen if we go onto a new page because of another column next to us carrying
// on over the page - we finished drawing all our content but they are still going	
		heightUsed = 0;
		mCharsToDraw = 0;
	}
	else {
		const unsigned int charsLeft = mFieldStr.length() - mDrawFromChar;
		const unsigned long charsThatWillFit = heightAvailable / mStyleHeight;  // truncating integer divide
		if (charsThatWillFit < charsLeft) {
			const unsigned int charsLeftToDraw = charsLeft - charsThatWillFit;
			retHeightLeftToDraw = mStyleHeight *  charsLeftToDraw;
			mCharsToDraw = charsThatWillFit;
		}
		else
			mCharsToDraw = charsLeft;
		heightUsed = mStyleHeight * mCharsToDraw;
	}
	return retHeightLeftToDraw;
}

oofString
CDrawBigChars::signature() const
{
	return "CDrawBigChars";
}


void 
CDrawBigChars::registerSerializable()
{
	oofSerializableFactory::registerDefaultMake("CDrawBigChars", CDrawBigChars::makeDrawer);
}


oofString 
CDrawBigChars::serialize() const
{
	return oofRepTextStyle2CSS::emitCSSstyle(mStyle);
}


oofSerializable* 
CDrawBigChars::makeDrawer (const oofString& inData)
{
	// inData contains a report text style that we've serialised using our CSS converter
	oofCSS2RepTextStyle styleParser;
	oofRepTextStyle copiedStyle;
	styleParser.parseStyleText(inData, &copiedStyle);
	return new CDrawBigChars(copiedStyle);
}


#ifdef _Windows
class CDrawBigCharsDE : public oofDE {
public:
	CDrawBigCharsDE(const CDrawBigChars*, int left, int top);
	virtual ~CDrawBigCharsDE();

	virtual void draw() const;

private:
	CDrawBigChars* mDrawer;  // owned, copied with state at drawing time
	int mLeft, mTop;
};


oofDE* 
CDrawBigChars::makeDrawDE(int left, int top)
{
	oofDE* ret = new CDrawBigCharsDE(this, left, top); 
 	return ret;
}



// -------------------------------------------------------
//         CDrawBigCharsDE
// -------------------------------------------------------
CDrawBigCharsDE::CDrawBigCharsDE(const CDrawBigChars* inDrawer, int left, int top) :
	mDrawer(new CDrawBigChars(*inDrawer)),  // our very own copy
	mLeft(left),
	mTop(top)
{}


CDrawBigCharsDE::~CDrawBigCharsDE()
{
	delete mDrawer;
}


void 
CDrawBigCharsDE::draw() const
{
	mDrawer->draw(mLeft, mTop);  // our copy will have the correct state
	// remember that the replaying of pages that calls this oofDE can
	// occur in ANY order. The original oofCustomViewDrawer::draw() calls
	// as used on the Mac occur in sequence after resetting the drawer
	// with calculateWrapping.
}

#endif // _Windows


// -------------------------------------------------------
//           C S a m p l e B a s e 
// -------------------------------------------------------
CSampleBase::CSampleBase() :
	mNumReports(11)
{	
	InitialiseReports();
	
	//Menu Entries
	mMenuEntry = new oofString[mNumReports];
	mMenuEntry[0] = "Simple List";
	mMenuEntry[1] = "List with Column Alignment";
	mMenuEntry[2] = "Report with Graph";
	mMenuEntry[3] = "Report with Graph and Legend";
	mMenuEntry[4] = "Multi report";
	mMenuEntry[5] = "Picture Test";
	mMenuEntry[6] = "Student Notes";
	mMenuEntry[7] = "Just Graphs";
	mMenuEntry[8] = "Many Graphs";
	mMenuEntry[9] = "Notes with Custom Draw";
	mMenuEntry[10] = "Single Note";
	
	//Report Descriptions
	mDescription = new oofString[mNumReports];
	mDescription[0] = "Simple Report listing with two columns.";
	mDescription[1] = "Report with column alignment and total of items listed.";
	mDescription[2] = "Report with adorners for shading, table and graph.";
	mDescription[3] = "Report with adorners for shading, table and graph, including a legend.";
	mDescription[4] = "Multi report combining first three";
	mDescription[5] = "Simple Report with Picture blocks and Bands";
	mDescription[6] = "Simple List showing wrapping of text field over page boundary";
	mDescription[7] = "Range of graph types";
	mDescription[8] = "Multi report workout - 200 lots of Just Graphs";
	mDescription[9] = "Student Notes with 2nd col using custom draw object showing how we can integrate parallel drawing, eg: for formatted text rendering";
	mDescription[10] = "Single note field from a standalone oofText variable, showing just printing text without a database";
}

CSampleBase::~CSampleBase()
{
	delete[] mMenuEntry;
	delete[] mDescription;
	delete mData;  
}

oofString
CSampleBase::menuEntry(unsigned short menuIndex)
{
	return mMenuEntry[menuIndex];
}

oofString
CSampleBase::description(unsigned short descriptionIndex)
{
	return mDescription[descriptionIndex];
}

oofRep*
CSampleBase::makeReport(unsigned short reportNum)
{
	oofRep* ret;
	switch (reportNum) {
	case (0) :
		ret = DoStudentListReport();
		break;
		
	case (1) :
		ret = DoPerformanceOnCurricItems();
		break;
		
	case (2) :
		ret = DoStudentReport();
		break;
		
	case (3) :
		ret = DoStudentReportWithLegend();
		break;
		
	case (4) :
		ret = DoMultiReport();
		break;
		
	case (5) :
		ret = DoPictureTest();
		break;
		
	case (6) :
		ret = DoStudentNotes();
		break;
		
	case (7) :
		ret = DoJustGraphs();
		break;
		
	case (8) :
		ret = DoManyGraphs();
		break;
		
	case (9) :
		ret = DoNotesWithCustomDraw();
		break;
		
	case (10) :
		ret = DoSingleNote();
		break;
		
	default:
		assert(!"unknown report number");
	}
	return ret;
}



oofRep*
CSampleBase::DoStudentListReport()
{
//using multi report for single report
	oofRep* ret = new oofRepMulti;

	oofRep* theRep = new oofRep;
	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->selectAll();
	
	
	//Page Settings
	
	ret->getLocalSettings()->borderType(3);
	ret->getLocalSettings()->leftMargin(40);
	ret->getLocalSettings()->bottomMargin(100);
	ret->getLocalSettings()->rightMargin(92);
	ret->getLocalSettings()->bandIndenting(50);
	
	//First Page Header
	oofRepLayoutBand* firstPageHeader = new oofRepLayoutBand;
	firstPageHeader->textStyle("Helvetica", 14, bold);
	*firstPageHeader << oofRepTextBlock("School Title\n", new oofRepTextStyle("Helvetica", 16, bold)).align(oofRepBlock::alignCentre);
	*firstPageHeader << oofRepTextBlock("Student List\n").align(oofRepBlock::alignCentre);
#if defined _MSC_VER && _MSC_VER >= 1100
// caused internal compiler error in VC5
// all other examples will be coded in this manner, retained the old form to show the error
	*firstPageHeader << oofRepTextBlock(oofString("Date: ",dbDate::currentDate().copyString(),"\n")).align(oofRepBlock::alignRight);
#else
	*firstPageHeader << oofRepTextBlock(ostrstream() << "Date: " << dbDate::today << "\n").align(oofRepBlock::alignRight);
#endif	
	
	theRep->firstPageHeaders() << new oofRepSpaceBand(3)
									<< firstPageHeader << new oofRepSpaceBand(7)
									<< new oofRepLineBand(2) << new oofRepSpaceBand(5);
									
	
	//Report Body
	dbView StudentsView (mData->Students, false);
	StudentsView << mData->Students->LastName << mData->Students->FirstName << "Fixed Text";	
	oofColSizer* theSizer = StudentsView.colSizer();

// 990526 comment out the following test because it draws funny!
//	theSizer->setFixedWidth(2, 0);  // hide "Fixed Text" column by setting zero width
	// this would be used if you wanted to 'attach' data to a report so it gets exported, but never
	// have it visible to the user in printed or viewed reports
	
	oofRepViewBand* theBody = new oofRepViewBand(StudentsView, false);
	
	theRep->body(theBody);

	// test creating a multi-report which contains one copy of a single report

	// set local page footers to see that we can do this wth a multi report
	// leaving the global default as is	
	ret->pageFooters()  
		<< new oofRepSpaceBand(3) 
		<< new oofRepLineBand(2)
		<< (oofRepLayoutBand()
			<< "Student List Footer"
			<< oofRepPageNoBlock("Page").align(oofRepBlock::alignRight)
		)
		<< new oofRepSpaceBand(4);

	((oofRepMulti*)ret)->appendReport(theRep);
	return ret;
}


//Performance on Curriculum Items
oofRep*
CSampleBase::DoPerformanceOnCurricItems()
{
// for each student, nested view band with their progress in a sub-table
// without breaking to new page 
	oofRep* ret = new oofRep;
	
	//Use the first student
	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->start();

	//Report Body
	dbView StudentsView (mData->Students, false);
	StudentsView << mData->Students->LastName << mData->Students->Note;	
	oofRepViewBand* theBody = new oofRepViewBand(StudentsView, false);
//	theBody->textStyle("Helvetica", 10, bold);
	
	// nested table
	dbView ProgressView (mData->Students->Progress, false);
	ProgressView  << mData->Students->Progress->CurricItems->Name 
				  << mData->Students->Progress->LastRecorded 
				  << mData->Students->Progress->LevelAchieved;
				  
	ProgressView.colSizer()->setAlignment(0, oofColSizer::alignLeft);	// force numeric ID to align left					 				
	ProgressView.setHeading(0,"Curriculum ID");	// change numeric title			 				
	ProgressView.setHeading(1,"Subject");				 				
	oofRepViewBand* subBody = new oofRepViewBand(ProgressView, true /* draw boxed*/);
	subBody->footers() 
		<< new oofRepLineBand(8) 
		<< new oofRepSpaceBand(40);  // nice big gap
	
	theBody->footerBreaks() << new oofRepBreakEveryRecord(subBody);


	ret->body(theBody);
	return ret;
}


oofRep*
CSampleBase::DoStudentReport()
{
// for first student in database, report graph of their progress

	 oofRep* ret = new oofRep;

	//Page Settings
	
	ret->getLocalSettings()->borderType(3);
	ret->getLocalSettings()->leftMargin(40);
	ret->getLocalSettings()->bottomMargin(100);
	ret->getLocalSettings()->rightMargin(92);
	ret->getLocalSettings()->bandIndenting(50);
	
	//Use the first student
	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->start();

	//Page Header
	oofRepLayoutBand* pageHeader = new oofRepLayoutBand;
	pageHeader->textStyle("Helvetica", 14, bold);
	*pageHeader << oofRepTextBlock("School Title\n", new oofRepTextStyle("Helvetica", 16, bold)).align(oofRepBlock::alignCentre);
	*pageHeader << oofRepTextBlock("Student Report\n", new oofRepTextStyle("Helvetica", 16, bold)).align(oofRepBlock::alignCentre);
	*pageHeader << mData->Students->FirstName << " " << mData->Students->LastName;
	*pageHeader << oofRepTextBlock(oofString("Date: ",dbDate::currentDate().copyString(),"\n")).align(oofRepBlock::alignRight);
	*pageHeader << oofRepTextBlock("Another line of text to show effect of the newline in the date above", new oofRepTextStyle("Helvetica", 12));
		
	ret->pageHeaders() << new oofRepSpaceBand(3)
									<< pageHeader << new oofRepSpaceBand(7)
									<< new oofRepLineBand(2) << new oofRepSpaceBand(12);
	
	//Shaded band								
/* OLD
	oofRepLayoutBand* subHeading = new oofRepLayoutBand;
	*subHeading << oofRepTextBlock("Subject Summary\n", new oofRepTextStyle("Helvetica", 12, bold)).align(oofRepBlock::alignCentre);
	subHeading->adorners() << new oofShadingAdorner(10);
*/
// slightly more awkward syntax but users have used it - centre the block rather than the text style on the block
// this would be used more commonly where we wanted to set a text style on the band to apply across a range of blocks
// but one row contains a centred block
	oofRepLayoutBand* subHeading = new oofRepLayoutBand;
	subHeading->textStyle("Helvetica", 12, bold);

	oofRepTextBlock* theTitleTextBlock = new oofRepTextBlock("Subject Summary\n");
	theTitleTextBlock->align(oofRepTextBlock::alignCentre);
	*subHeading << theTitleTextBlock;

	subHeading->adorners() << new oofShadingAdorner(10);
	
	mData->CurriculumItems->setSortOrder(mData->CurriculumItems->Name);
	dbView CurricSummaryView(mData->CurriculumItems, false);
	CurricSummaryView << mData->CurriculumItems->Name << mData->CurriculumItems->Note;
	oofRepViewBand* CurriculumSummary = new oofRepViewBand(CurricSummaryView, true); 
									
	

	//Graph
	mData->Progress->selectAll();
	mData->Progress->search(mData->Progress->StudentID == mData->Students->ID);
	
	templateProgressData* progressData = mData->allocTempProgressData();
	mData->populateTempProgressData();

	dbView ProgressView(progressData, false);
	ProgressView << progressData->item << progressData->legend << progressData->levelAchieved;
	oofBarGraph* theGraph = new oofBarGraph(&ProgressView);
	theGraph->getLocalSettings()->hasLegend(false);
	oofRepGraphBand* theGraphBand = new oofRepGraphBand(theGraph, 400 , 400, 400, "Progress Graph");

	
	CurriculumSummary->headers() << subHeading << new oofRepSpaceBand(12);
	CurriculumSummary->footers() << theGraphBand;
	
	ret->body(CurriculumSummary);
	return ret;
}


oofRep*
CSampleBase::DoStudentReportWithLegend()
{
	oofRep*	ret = new oofRep;
	//Page Settings
	ret->getLocalSettings()->borderType(3);
	ret->getLocalSettings()->leftMargin(40);
	ret->getLocalSettings()->bottomMargin(100);
	ret->getLocalSettings()->rightMargin(92);
	ret->getLocalSettings()->bandIndenting(50);

	//Use the first student
	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->start();

	//Page Header
	oofRepLayoutBand* pageHeader = new oofRepLayoutBand;
	pageHeader->textStyle("Helvetica", 14, bold);
	*pageHeader << oofRepTextBlock("School Title\n", new oofRepTextStyle("Helvetica", 16, bold)).align(oofRepBlock::alignCentre);
	*pageHeader << oofRepTextBlock("Student Report\n", new oofRepTextStyle("Helvetica", 16, bold)).align(oofRepBlock::alignCentre);
	*pageHeader << oofRepTextBlock(oofString("Date: ",dbDate::currentDate().copyString(),"\n")).align(oofRepBlock::alignRight);

	ret->pageHeaders() << new oofRepSpaceBand(3)
									<< pageHeader << new oofRepSpaceBand(7)
									<< new oofRepLineBand(2) << new oofRepSpaceBand(12);

	//Shaded band								
	oofRepLayoutBand* subHeading = new oofRepLayoutBand;
	*subHeading << oofRepTextBlock("Subject Summary\n", new oofRepTextStyle("Helvetica", 12, bold)).align(oofRepBlock::alignCentre);
	subHeading->adorners() << new oofShadingAdorner(10);

	mData->CurriculumItems->setSortOrder(mData->CurriculumItems->Name);
	dbView CurricSummaryView(mData->CurriculumItems, false);
	CurricSummaryView << mData->CurriculumItems->Name << mData->CurriculumItems->Note;
	oofRepViewBand* CurriculumSummary = new oofRepViewBand(CurricSummaryView, true); 
	
	templateProgressData* progressData = mData->allocTempProgressData();
	//Graph

	mData->Progress->selectAll();
	for (unsigned short i=1; i<4; i++){
		mData->Progress->search(mData->Progress->StudentID == mData->Students->ID);
		oofString studentName = mData->Students->FirstName.value();
		studentName += " ";
		studentName += mData->Students->LastName;

		mData->populateTempProgressData(studentName);
		mData->Students->next();
	}

		
	progressData->setSortOrder(dbSorter() << progressData->item << progressData->legend);
	dbView ProgressView(progressData, false);
	ProgressView << progressData->item << progressData->legend << progressData->levelAchieved;
	oofBarGraph* theGraph = new oofBarGraph(&ProgressView);
// deliberately oversized to show what happens
	oofRepGraphBand* theGraphBand = new oofRepGraphBand(theGraph, 800 , 800, 600, "Progress Graph");

	CurriculumSummary->headers() << subHeading << new oofRepSpaceBand(12);
	CurriculumSummary->footers() << theGraphBand;

	ret->body(CurriculumSummary);
	return ret;
}


oofRep*
CSampleBase::DoMultiReport()
{
	oofRepMulti* ret = new oofRepMulti;
	
	// set local page footers to see that we can do this wth a multi report
	// leaving the global default as is	
	ret->pageFooters()  
		<< new oofRepSpaceBand(3) 
		<< new oofRepLineBand(2)
		<< (oofRepLayoutBand()
			<< "Student List Footer"
			<< oofRepPageNoBlock("Page").align(oofRepBlock::alignRight)
		)
		<< new oofRepSpaceBand(4);

// just append the first three reports
	for (unsigned short i=0; i<3; i++)
		ret->appendReport(makeReport(i));
	return ret;
}


oofRep*
CSampleBase::DoPictureTest()
{
	oofRep* ret = new oofRep;

// list contents copied from StudentListReport
	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->selectAll();
	
	// Page Header containing picture block
	oofRepLayoutBand* firstPageHeader = new oofRepLayoutBand;
	firstPageHeader->textStyle("Helvetica", 14, bold);
#ifdef _Windows
	oofRepPictBlock squashedPicBlock(new oofPict("oofrep.bmp"));
#else
	oofRepPictBlock squashedPicBlock(new oofPict(128));  // show Mac loading from resource
#endif
	squashedPicBlock.fixedWidth(160);  // constrained to fit
	*firstPageHeader << squashedPicBlock;
	squashedPicBlock.align(oofRepBlock::alignRight);  // after copying for LHS of header
	oofDate headerDate  = dbDate::currentDate();
	headerDate.formatMask("ddth Mmm YYYY");
	*firstPageHeader << oofRepTextBlock(headerDate.copyString()).align(oofRepBlock::alignCentre);  
	*firstPageHeader << squashedPicBlock;
	
	
	ret->pageHeaders() << new oofRepSpaceBand(3)
									<< firstPageHeader << new oofRepSpaceBand(7)
									<< new oofRepLineBand(2) << new oofRepSpaceBand(5);
									
	
	//Report Body
	dbView StudentsView (mData->Students, false);
	StudentsView << mData->Students->LastName << mData->Students->FirstName;	
	oofRepViewBand* theBody = new oofRepViewBand(StudentsView, false);
#ifdef _Windows
	theBody->footers() << new oofRepPictBand(new oofPict("oofrep.bmp"));
#else
	theBody->footers() << new oofRepPictBand(new oofPict("::oofrep.pict"));  
	// don't want in the Output dir as too likely to delete by accident!, keep at same level as project for testing
	// on the Mac, the current dir while executing is that containing the exe, not the workspace like Windows
#endif	
	ret->body(theBody);

	return ret;
}


oofRep*
CSampleBase::DoStudentNotes()
{
	oofRep* ret = new oofRep;

	mData->Students->setSortOrder(mData->Students->LastName);
	mData->Students->search(mData->Students->Note>"");  // only people with notes
	
	dbView studentsView(mData->Students);
	studentsView << mData->Students->LastName << mData->Students->FirstName << mData->Students->Note;
	studentsView.colSizer()->setFixedPercentage(2, 50);  // Notes column
	ret->body(new oofRepViewBand(studentsView));
	ret->body()->footerBreaks() << new oofRepBreakEveryRecord( new oofRepSpaceBand(3) );
	return ret;
}


oofRep*
CSampleBase::DoNotesWithCustomDraw()
{
	oofRep* ret = DoStudentNotes();
	
	// all we are doing is attaching a piece of code to draw part of the report differently.
	ret->body()->customDrawer(
		1, 
		new CDrawBigChars(oofRepTextStyle("Times", 36, italic, -2 /* leading */, oofColor::Magenta)) // custom drawer for col 1
	);  
	return ret;
}

// create local table and database
// MOVED out CSampleBase::DoJustGraphs
	class dbSummaryTable : public dbTable {
		OOFILE_METHODS(dbSummaryTable)
		dbLong average; // integer type same as Achieved  values we overlay
		dbSummaryTable() : 
			average("average")
		{};
	};

oofRep*
CSampleBase::DoJustGraphs()
{

	dbConnect_ram temp;
	dbSummaryTable summaryTable;
	temp.newConnection();  // allocates table in ram
	
	// shows workaround awkward restriction in report writer initial model
	// that you have to put a view band at the top, so if we want a number
	// of graphs we attach them all as footers to an undisplayed view band
	// we attach each graph one after the other and let them fit as them may
	oofRep* ret = new oofRep;

	dbView studentsView(mData->Students);
	studentsView << mData->Students->LastName;
	ret->body(new oofRepViewBand(studentsView));

	// now add the graphs which will show as headers

	//Graph data used repeatedly
	templateProgressData* progressData = mData->allocTempProgressData();
	//local var for reference only - mData tracks the allocation internally

	
	mData->Students->selectAll();
	for (unsigned short i=0; i<4; i++){  // just want four data series in sample graphs
		
// limit the amount of data being graphed
		mData->Progress->search(
			mData->Progress->StudentID == mData->Students->ID  &&
			mData->Progress->CurricID >= 65  
		);
		oofString studentName = mData->Students->FirstName.value();
		studentName += " ";
		studentName += mData->Students->LastName;

		summaryTable.newRecord();
		summaryTable.average = mData->populateTempProgressData(studentName);
		summaryTable.saveRecord();
		
		mData->Students->next();
	}

	progressData->setSortOrder(dbSorter() << progressData->item << progressData->legend);
	dbView ProgressView(progressData, false);
	ProgressView << progressData->item << progressData->legend << progressData->levelAchieved;

	ret->getLocalSettings()->defaultTextStyle(new oofRepTextStyle("Helvetica", 7));

	// only stuff around with local variables if need to manipulate settings
		oofGraph* theGraph = new oofColumnGraph(&ProgressView);
		theGraph->getLocalSettings()->has3DEffect(true);
	
		// large band height to centre graph within
		oofRepBand* bigBand = new oofRepGraphBand(theGraph, 700, 600, 800, "Progress Cols");
		bigBand->textStyle(new oofRepTextStyle("Times", 10));  // SIZE is ignored!!

	oofStackedBarGraph* stackedBars1 = new oofStackedBarGraph(&ProgressView);
	dbView overlayAverage (summaryTable);
	overlayAverage << summaryTable.average;
	stackedBars1->setOverlayView(overlayAverage);
	
	oofStackedBarGraph* stackedBars2 = new oofStackedBarGraph(&ProgressView);
	stackedBars2->setOverlaid(2);

	ret->body()->headers() 
		<< bigBand
		<< new oofRepGraphBand(new oofLineGraph(&ProgressView), 700 , 700, 800, "Progress Lines")
		<< new oofRepGraphBand(new oofPointGraph(&ProgressView), 400 , 300, 600, "Progress Points")
		<< new oofRepGraphBand(new oofBarGraph(&ProgressView), 400 , 400, 600, "Progress Bars")
		<< new oofRepGraphBand(new oofStackedBarGraph(&ProgressView), 400 , 400, 700, "Progress Stack")
		<< new oofRepGraphBand(stackedBars1, 400 , 400, 700, "Progress Stack Overlay Average")
		<< new oofRepGraphBand(stackedBars2, 400 , 400, 700, "Progress Stack Series Overlay")
	;
	ret->body()->headers().keepTogether(false);
	
	mData->Students->selectNone();  // empty selection so just do the report header
	return ret;
}


oofRep*
CSampleBase::DoManyGraphs()
{
// largely a copy of DoJustGraphs but with loop appending graphs
	oofRep* ret = new oofRep;

	dbView studentsView(mData->Students);
	studentsView << mData->Students->LastName;
	ret->body(new oofRepViewBand(studentsView));

	// now add the graphs which will show as headers

	//Graph data used repeatedly
	templateProgressData* progressData = mData->allocTempProgressData();
	//local var for reference only - mData tracks the allocation internally

	mData->Students->selectAll();
	for (unsigned short i=1; i<4; i++){
		mData->Progress->search(
			mData->Progress->StudentID == mData->Students->ID &&
			mData->Progress->CurricID >= 65
		);
		oofString studentName = mData->Students->FirstName.value();
		studentName += " ";
		studentName += mData->Students->LastName;

		mData->populateTempProgressData(studentName);
		mData->Students->next();
	}

	progressData->setSortOrder(dbSorter() << progressData->item << progressData->legend);
	dbView ProgressView(progressData, false);
	ProgressView << progressData->item << progressData->legend << progressData->levelAchieved;

	int loopBars = 200;
    for (int j=0; j<loopBars; j++) {
		ret->body()->headers() 
			<< new oofRepGraphBand(new oofLineGraph(&ProgressView), 700 , 700, 800, "Progress Lines")
			<< new oofRepGraphBand(new oofPointGraph(&ProgressView), 400 , 300, 600, "Progress Points")
			<< new oofRepGraphBand(new oofBarGraph(&ProgressView), 400 , 400, 600, "Progress Bars")
			<< new oofRepGraphBand(new oofStackedBarGraph(&ProgressView), 400 , 400, 700, "Progress Stack")
		;
		ret->body()->headers().keepTogether(false);
	}
	mData->Students->selectNone();  // empty selection so just do the report header
	return ret;

/* the following is the intuitive way to do this test
Unfortunately, something that could not really be called a bug, is that
OOFRep tries to optimise cloning reports and shares a lot of data reference
counted. If you therefore clone the same report multiple times to run in
a multi-report, weird things happen and you certainly do NOT get N copies
of the original.

Multi-reports had of course not been conceived when the sharing model was
created - it was to optimise copying of reports as concrete objects.
	
	oofRepMulti* ret = new oofRepMulti;

	oofRep* graphsRep = makeReport(7);
	for (unsigned short i=0; i<19; i++)
//		ret->appendReport(graphsRep->clone());
		ret->appendReport(new oofRep(*graphsRep));  // have copy ctor but no clone
	ret->appendReport(graphsRep);  // consume the original

	return ret;
*/
}  // DoManyGraphs




oofRep*
CSampleBase::DoSingleNote()
{
// print a very trivial report with a bunch of text from a variable, NOT a database
	oofText theNote = "This is an example of how OOFILE allows you to use a standalone variable which is "\
	"still a descendant of dbField and so can be used in a dbView. The dbView is used without a database table "\
	"and so 'iterates' just once.\n\n";
	
	theNote += "The combination of a table-less view and a standalone field is an easy way to put a large chunk "\
	"of wrapped text into a report, without going through the minor complication of creating a temporary database\n\n";
	
	theNote += "The standalone field types in OOFILE are very useful as sources of data for the report writer, "\
	"and the GUI integration: connected to editing controls";
	
/*
	Alternative syntax which allows you to pass a single chunk of text in but lacks the above
	flexibility where we can keep appending to the oofText variable:
	
	dbView trivialView;
	trivialView << "Some standalone text";
*/
	oofRep* ret = new oofRep;
	ret->body(new oofRepViewBand(dbView() << theNote));
	return ret;
}



void
CSampleBase::InitialiseReports()
{	
	//Create data
	mData = new ReportData;

// setup page header and footer that will apply if not overriden
	oofRep::defaultSettings()->defaultPageHeaders()  
		<< (oofRepLayoutBand()
			<< oofRepTextBlock("OOFILE Sample Report", new oofRepTextStyle("Helvetica", 14, bold)).align(oofRepBlock::alignCentre)
		)
		<< new oofRepLineBand(2)
		<< new oofRepSpaceBand(10); 

	oofRep::defaultSettings()->defaultPageFooters()  
		<< new oofRepSpaceBand(10) 
		<< (oofRepLayoutBand()
			<< "OOFILE Sample Report Footer"
			<< oofRepPageNoBlock("Page ").align(oofRepBlock::alignRight)
		);

	// sometimes you want the legend to start at the end of the axis
	// but often it is better offset some pixels to the right.
	oofGraphDefaultSettings::settings()->legendOffset(20);

	oofRep::defaultSettings()->reportEditor(new oofRepEditor);  

// register custom code for reading in reports
	CDrawBigChars::registerSerializable();
}


#ifdef _Windows
void
CSampleBase::showReport(oofRep* theReport)
{
	oofAfxDoModalPreview(IDR_MAINFRAME, theReport, 0, FALSE);
}

void 
CSampleBase::printReport(oofRep* theReport)
{
	oofAfxPrintReport(theReport);
}
#endif
