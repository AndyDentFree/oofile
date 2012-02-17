// Database schema for sample test reports
// cross-platform part of Sample Reports

#ifndef REPORT_DATA_H
#define REPORT_DATA_H
#include "oofile.h"
#include "assert.h"



DECLARE_REF(dbStudents)
DECLARE_REF(dbTeachers)
DECLARE_REF(dbClass)
DECLARE_REF(dbCurricItems)

DECLARE_SET(dbClass)
DECLARE_SET(dbClassFocus)
DECLARE_SET(dbProgress)

//Calculator
class hasAchievedCalc : public dbBoolCalculator{
public:
	virtual dbCalculator* clone() const{return new hasAchievedCalc(*this);};
	virtual bool	calc(const dbBool*);
};


//Tables

//Student table
class dbStudents : public dbTable {
OOFILE_METHODS(dbStudents)
		dbUlong	ID;
		dbChar	FirstName;
		dbChar	LastName;
		dbText	Note;
		
		//relationships
		dbClassSet Class;
		dbProgressSet Progress;
	
		dbStudents():
			ID("ID"),
			FirstName(15, "First Name"),
			LastName(25, "Last Name"),
			Note("Note")
		{
			Class.joinField(ID);
			Progress.joinField(ID);
		};
		
	void AddRecord(const unsigned long studentID, const char*  fname, const char* lname, const char* studentNotes);
	void AddClassRecord(const unsigned long classID, const unsigned long teacherID);
	void AddSampleData();
	short LevelAchievedForItem(const unsigned long CurricItemID);
	short LevelAchievedForItemInRange(const unsigned long CurricItemID, const oofDate& startDate, const oofDate& endDate);
};

class dbTeachers : public dbTable {
OOFILE_METHODS(dbTeachers)
		dbUlong	ID;
		dbChar	FirstName;
		dbChar	LastName;
		
		//relationships
		dbClassSet Class;
	
		dbTeachers():
			ID("ID"),
			FirstName(15, "First Name"),
			LastName(25, "Last Name")
		{
			Class.joinField(ID);
		};
		
	void AddRecord(const unsigned long teacherID, const char* fname, const char* lname);
	void AddSampleData();
};


class dbCurricItems : public dbTable {
OOFILE_METHODS(dbCurricItems)
	dbUlong	ID;
	dbChar	Name;
	dbText	Note;
	dbShort	LevelRequired;
		
	//relationships
	dbClassFocusSet	ClassFocus;
	dbProgressSet	Progress;
		
	dbCurricItems():
		ID("ID"),
		Name(40, "Name"),
		Note("Note"),
		LevelRequired("Level")
	{
		ClassFocus.joinField(ID);
		Progress.joinField(ID);
	};
		
	void AddRecord(const unsigned long itemID, const char* itemName, const char* itemNotes, const short level);
	void AddClassFocusRecord(const unsigned long classID);
	void AddSampleData();
};

class dbClass : public dbTable {
OOFILE_METHODS(dbClass)
	dbUlong	ID;
	dbUlong	StudentID;
	dbUlong	TeacherID;
		
	//relationships
	dbStudentsRef Students;
	dbTeachersRef Teachers;
	dbClassFocusSet ClassFocus;
		
	dbClass():
		ID("ID"),
		StudentID("StudentID"),
		TeacherID("TeacherID")
	{
		Students.joinField(StudentID);
		Teachers.joinField(TeacherID);
		ClassFocus.joinField(ID);
	};
};


class dbClassFocus : public dbTable {
OOFILE_METHODS(dbClassFocus)
		dbUlong	ClassID;
		dbUlong	CurricID;
		
		//relationships
		dbClassRef			Classes;
		dbCurricItemsRef	CurricItems;
		
		dbClassFocus():

			ClassID("ClassID"),
			CurricID("CurricID")
		{
			Classes.joinField(ClassID);
			CurricItems.joinField(CurricID);
		};
};


class dbProgress : public dbTable {
OOFILE_METHODS(dbProgress)
		dbUlong StudentID;
		dbUlong	CurricID;
		dbShort	LevelAchieved;
		dbDate	LastRecorded;
		dbText	Note;
		dbBool 	HasAchieved; //calculated field
		
		//relationships
		dbStudentsRef		Students;
		dbCurricItemsRef	CurricItems;	
		
		dbProgress():
			StudentID("StudentID"),
			CurricID("CurricID"),
			LevelAchieved("Achieved"),
			LastRecorded("Recorded"),
			Note("Note"),
			HasAchieved("HasAchieved")
		{
			Students.joinField(StudentID);
			CurricItems.joinField(CurricID);
			HasAchieved.calculateWith(new hasAchievedCalc);
		};
		
		
	void newRecord();
	void AddRecord(const unsigned long studentID, const unsigned long curricID, const short level,const char* note);
	void AddSampleData(dbStudents& Students, dbCurricItems& CurricItems);
};

//Table for GraphBand


class templateProgressData : public dbTable {
OOFILE_METHODS(templateProgressData)
	dbChar	item;
	dbChar	legend;
	dbLong	levelAchieved;
	
	templateProgressData():
		item (40, "Curriculum Items"),

		legend(20, "Students"),
		levelAchieved("Student Progress")
	{};
};


class ReportData
{	
public:
	ReportData();
	~ReportData();
	templateProgressData* allocTempProgressData();
	double populateTempProgressData(const char* inLegend=0);
	
	dbConnect_ram theDB;
	
	// tables all owned, will be deleted by theDB dtor
	dbStudents*		Students;
	dbTeachers*		Teachers;
	dbCurricItems*	CurriculumItems;
	dbProgress*		Progress;
	dbClass*		Classes;
	dbClassFocus*	ClassFocus;

private:	
	// pointer to table used to hang onto local temporary table
	templateProgressData* mTempProgressData;
};
#endif //REPORT_DATA_H