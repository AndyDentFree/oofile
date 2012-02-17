// cross-platform part of Sample Reports
#include "stdafx.h"
#include "oof4.h"
#include "database.h"


const unsigned short kMaxLevel = 5;

//Students
void
dbStudents::AddRecord(const unsigned long studentID, const char*  fname, const char* lname, const char* studentNotes)
{
	newRecord();
	ID = studentID;
	FirstName = fname;
	LastName = lname;
	Note = studentNotes;
	saveRecord();
	
	
}


void
dbStudents::AddSampleData()
{
	AddRecord(10, "Peter", "Harrison","This is a student note");
		AddClassRecord(1, 30);
		AddClassRecord(2, 31);
	saveRecord();
	
	AddRecord(11, "Marion", "Carey", 
		"Marion has had an active social life this year in which she has excelled "\
		"at a range of sporting and community activities. Whilst many students would "\
		"have trouble balancing such demanding external commitments with their studies, "\
		"Marion appears to have used disciplined scheduling to maintain a high grade "\
		"point average.\n\n"\
		"We are very impressed with her maturity in balancing these activities and keen "\
		"to use her as an example of the way that good results do NOT mean turning into "\
		"a 'boring swot'. Marion is popular with other students and so is already leading "\
		"by example in both academic and social spheres.\n\n"\
		"Highlights of Marion's year include:\n"\
		"-  captain of the winning debate team against Littlewood Junior High\n"\
		"-  MVP in the netball B team\n"
		"-  Junior Citizen of the Year\n"
		"-  Secretary of the Young Achievers team which recorded the state's highest sales\n\n"\
		"Marion has received preliminary offers from two universities and is considering a "\
		"career in software user interface design. This demanding field mixes her technical "\
		"skills with rule-of-thumb psychology and artistic flair.\n\n"\
		"Whilst I am going on at length about Marion, this is because I need to take up "\
		"a substantial amount of space on one of the sample reports and it seems worth it "\
		"to have one of the notes blather on in a manner not unlike a teacher writing one "\
		"of those platitudinous citations or report comments.\n\n"\
		"I'm not saying that such paragons of virtue are absent from our schools.\n\n"\
		"Just that this sort of person is generally the kind you love to hate, although "\
		"the most annoying specimens are those who are actually such nice people as well "\
		"as being mega-achievers that you cannot help but like them, and loath yourself "\
		"even more for WANTING to dislike them.\n\n"\
		"Anyway, enough dribbling on at length here, this is at least better than randomly "\
		"entered characters which always make me wonder if the tester really entered that "\
		"rubbish or have we stumbled upon some area of memory that looks like an insane "\
		"cockroach had a drunken Fandango on the keyboard.\n\n"\
		"In possible honour to Archy (of Don Marquis' work \"archy and mehitabel\" which  "\
		"poems tells of a giant cockroach he discovered one night typing painfully "\
		"diving one key at a time on the keyboard, of course in lower-case alone) such "\
		"random rat-a-tat's of testers in a hurry lack any use of the shift key."\
		"And now some embedded rtf control chars to test rtf output \\, {, } ."
	);
		AddClassRecord(3, 30);
	saveRecord();

	AddRecord(12, "Jason", "Marson", "");
		AddClassRecord(4, 30);
	saveRecord();

	AddRecord(13, "Julia", "Henderson", "");
		AddClassRecord(5, 31);
	saveRecord();

	AddRecord(14, "Robert", "McKenzie", "");
		AddClassRecord(6, 31);
		AddClassRecord(7, 31);
	saveRecord();

	AddRecord(15, "Sara", "Watson", "");
		AddClassRecord(8, 32);
	saveRecord();

	AddRecord(16, "Mark", "Donaldson", "");
		AddClassRecord(9, 32);
	saveRecord();

	AddRecord(17, "Lara", "Fosters", "");
		AddClassRecord(10, 31);
	saveRecord();

	AddRecord(18, "Andrew", "Speares", 
		"Andrew is a boring prat.\n"\
		"A boring prat is he.\n"\
		"He sits at home and programs\n"\
		"with computers, three.\n\n"\
		"He never goes out with a girl\n"\
		"or even with his mates.\n"\
		"The Internet's his social whirl\n"\
		"and chat rooms where he dates."
	);
		AddClassRecord(11, 31);
	saveRecord();

	AddRecord(19, "Sharon", "Dobson", "");
		AddClassRecord(12, 31);
	saveRecord();

};

void
dbStudents::AddClassRecord(const unsigned long classID, const unsigned long teacherID)
{
	Class->newRecord();
	Class->ID = classID;
	Class->TeacherID = teacherID; 
	Class->saveRecord();
}

short
dbStudents::LevelAchievedForItem(const unsigned long CurricItemID)
{
	search(Progress->CurricID == CurricItemID);
	short ret = Progress->LevelAchieved;
	return ret;
}

short
dbStudents::LevelAchievedForItemInRange(const unsigned long CurricItemID, const oofDate& startDate, const oofDate& endDate)
{
	search(Progress->CurricID == CurricItemID && Progress->LastRecorded >= startDate && Progress->LastRecorded <= endDate);
	short ret = Progress->LevelAchieved;
	return ret;
}

	
//Teachers

void
dbTeachers::AddRecord(const unsigned long teacherID, const char* fname, const char* lname)
{
	newRecord();
	ID = teacherID;
	FirstName = fname;
	LastName = lname;
	saveRecord();
}


void
dbTeachers::AddSampleData()
{
	AddRecord(30, "Karen", "Stone");
	AddRecord(31, "David", "Thornton");
	AddRecord(32, "Susan", "Rye");
}

//Curriculum Items
void
dbCurricItems::AddRecord(const unsigned long itemID, const char* itemName, const char* itemNotes, const short level)
{
	assert(level >= 1 && level <= kMaxLevel);
	newRecord();
	ID = itemID;
	Name = itemName;
	Note = itemNotes;
	LevelRequired = level;
	saveRecord();
	
}

void
dbCurricItems::AddClassFocusRecord(const unsigned long classID)
{
	ClassFocus->newRecord();
	ClassFocus->ClassID = classID;
	ClassFocus->saveRecord();
}

void
dbCurricItems::AddSampleData()
{
	AddRecord(50, "Statistics - Interpretation",
	 	"Construct, use and interpret tabular and graphical representations (including simple frequency tables", 1);
	 	AddClassFocusRecord(1);
	 	AddClassFocusRecord(3);
	 	AddClassFocusRecord(4);
	saveRecord();

	AddRecord(51, "Statistics - Calculations", "Determine mean, mode, median and range for suitable statistical data", 1);
		AddClassFocusRecord(1);
	 	AddClassFocusRecord(3);
	 	AddClassFocusRecord(4);
	saveRecord();

	AddRecord(52, "Graphing", "Plot the points and read co-ordinates in a cartesian co-ordinate system", 1);
		AddClassFocusRecord(5);
	 	AddClassFocusRecord(6);
	 	AddClassFocusRecord(7);
	saveRecord();

	AddRecord(53, "Analytical Geometry", "Solve problems using analytical geometry techniques", 1);
		AddClassFocusRecord(5);
	 	AddClassFocusRecord(6);
	 	AddClassFocusRecord(7);
	saveRecord();

	AddRecord(54, "Number", "Solve practical problems involving real numbers", 1);
	
	AddRecord(55, "Communicate clearly", "Communicate clearly to a range of audiences", 1);
	AddRecord(56, "Note taking", "Develop note taking skills", 1);
	AddRecord(57, "Listening", "Telephone listening skills", 1);
	AddRecord(58, "Oral Communication", "Preparing and delivering a short talk", 1);
	AddRecord(59, "Media", "Analysing the uses of various media", 1);
	
	AddRecord(60, "Magnets", "Exploring the use of magnets in industry", 1);
	AddRecord(61, "Electricity", "", 1);
	AddRecord(62, "Engines", "", 1);
	AddRecord(63, "Planets", "", 1);
	AddRecord(64, "Solar System", "", 1);
	
	AddRecord(65, "Poetry", "", 1);
		AddClassFocusRecord(10);
	 	AddClassFocusRecord(11);
	 	AddClassFocusRecord(12);
	saveRecord();

	AddRecord(66, "Writing Essays", "", 1);
		AddClassFocusRecord(10);
	 	AddClassFocusRecord(11);
	 	AddClassFocusRecord(12);
	saveRecord();

	AddRecord(67, "Drama", "", 1);
	AddRecord(68, "Persuasive Writing", "", 1);
		AddClassFocusRecord(8);
	 	AddClassFocusRecord(9);
	saveRecord();

	AddRecord(69, "Metaphors and Similes", "", 1);
		AddClassFocusRecord(8);
	 	AddClassFocusRecord(9);
	saveRecord();
}

void
dbProgress::newRecord()
{
	dbTable::newRecord();
	sequenceNumber();
}

//Progress
void
dbProgress::AddRecord(const unsigned long studentID, const unsigned long curricID, const short level,const char* note)
{	
	oofDate today = dbDate::currentDate();
	assert(level >= 1 && level <= kMaxLevel);
	newRecord();	
	StudentID = studentID;
	CurricID = curricID;
	LevelAchieved = level;
	LastRecorded = today;
	Note = note;
	saveRecord();
}
	
	
void
dbProgress::AddSampleData(dbStudents& Students, dbCurricItems& CurricItems)
{	
// record kNumScores scores for each student, scattered across curriculum

	const unsigned long numCurric = CurricItems.count();
	unsigned long kNumScores = 20;  // ENTER YOUR VALUE HERE - number of scores generated affects size of reports

	if (kNumScores>numCurric)
		kNumScores = numCurric;

	const int kAverage = kMaxLevel * 0.65;
	const int kMaxBias = kMaxLevel * 0.8;
	
	CurricItems.start();
	Students.start();
	int biasBy = -1;
	const unsigned long numStudents = Students.count();
	for (unsigned long iStudent=0; iStudent < numStudents; iStudent++) {
		CurricItems.start();
		for (short iScore = kNumScores; iScore > 0 ; iScore--){

// calculate some scattered but non-random scores
			int bias =  1.0 + 2.0 * (iStudent+1.0)/numStudents * (1.0 + (1.0+iScore)/kNumScores);
			if (bias > kMaxBias) 
				bias = 1;
			int level = kAverage + biasBy * bias;  
			if (level > kMaxLevel) 
				level = 1;
			biasBy = -biasBy;
				
			if (level > 0) 
				AddRecord(Students.ID, CurricItems.ID, level, "");
			CurricItems.next();
			if (!CurricItems.more())
				CurricItems.start();  // if fewer scores than curric items, move across ALL curric items
		}  // scoring loop 			// not just allocating a block at the beginning
		Students.next();
	}  // students loop
}

bool
hasAchievedCalc::calc(const dbBool* theField)
{
	dbProgress* theTable = (dbProgress*) (theField->fieldTable());
	short achieved = theTable->LevelAchieved;
	short required = theTable->CurricItems->LevelRequired;
	bool ret = (achieved <= required); //1 is higher achievement than kMaxLevel
	return ret;
}

ReportData::ReportData() :
	theDB(false),  // not temporary - owns its tables & will delete them
	mTempProgressData(0)
{	
/*
these don't need to be variables as declaring a dbRelationship is sufficient to specify
the relationship. The only time we need a relationship variable is for MN relationships
where we have additional relateMNto() calls, and that still doesn't need to be on the heap

*/

	Students = new dbStudents;
	Teachers = new dbTeachers;
	CurriculumItems = new dbCurricItems;
	Progress = new dbProgress;
	Classes = new dbClass;
	ClassFocus = new dbClassFocus;

	//Relationships
	dbRelationship(Classes->Students, Students->Class);
	dbRelationship(Teachers->Class, Classes->Teachers);
	dbRelationship(Classes->ClassFocus, ClassFocus->Classes);
	dbRelationship(CurriculumItems->ClassFocus, ClassFocus->CurricItems);
	dbRelationship(Students->Progress, Progress->Students);
	dbRelationship(CurriculumItems->Progress, Progress->CurricItems);
	

	
	//Ram database
	theDB.newConnection();
	//Populate tables
	Students->AddSampleData();
	Teachers->AddSampleData();
	CurriculumItems->AddSampleData();
	Progress->AddSampleData(*Students, *CurriculumItems);
}


ReportData::~ReportData()
{
	delete mTempProgressData;  // AD990509 I'm not sure about this, it's not part of the main DB
	// pointers such as Students deleted by the theDB concrete member dtor
	// unless it is a dbConnect_ram as used here
	
}


templateProgressData*
ReportData::allocTempProgressData()
{
// for current selection in Progress, create a temp table and return it
// keeping track in a member variable so we can cleanup later

	if (mTempProgressData)
		mTempProgressData->deleteAll();
	else {
		dbConnect_ram tempDB;
		mTempProgressData = new templateProgressData;
		tempDB.newConnection();
		mTempProgressData->setSortOrder(mTempProgressData->item);
	}
	return mTempProgressData;
}


double  // returns average
ReportData::populateTempProgressData(const char* inLegend)
{
// for current selection in Progress
// populate with related data

// separated from allocation as may populate multiple times
	double totalLevel = 0;
	
	assert (mTempProgressData);
	const unsigned long numProgress = Progress->count();
	Progress->start();
	for (unsigned long i=0; i<numProgress; i++){
		mTempProgressData->newRecord();
		mTempProgressData->item = Progress->CurricItems->Name;
		if (inLegend)
			mTempProgressData->legend = inLegend;
		const long thisAchieved = Progress->LevelAchieved;
		totalLevel += thisAchieved;
		mTempProgressData->levelAchieved = thisAchieved;
		mTempProgressData->saveRecord();
		Progress->next();
	}
	
	double retAverage = 0;
	if (numProgress>0) 
		retAverage = totalLevel / numProgress;
	return retAverage;
}
