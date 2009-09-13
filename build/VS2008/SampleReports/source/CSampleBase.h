// CSampleBase.h : header file
// cross-platform part of Sample Reports

#ifndef H_SAMPLE_BASE
#define H_SAMPLE_BASE

#include "database.h"
#ifndef _Windows
	#include "oofile.h"
	#include "oofrep.h"
#endif

class CSampleBase
{
public:
	CSampleBase();
	~CSampleBase();

	oofString menuEntry(unsigned short);
	oofString description(unsigned short);
	oofRep* makeReport(unsigned short);
	#ifdef _Windows
		static int mReportChoice;
		void showReport(oofRep*);
		void printReport(oofRep*);
	#endif
	
	unsigned short numberOfReps() const;

private:
	unsigned short mNumReports;
	ReportData* mData;
	oofString* mMenuEntry;
	oofString* mDescription;

//Reports
	oofRep* DoStudentListReport();
	oofRep* DoStudentReport();
	oofRep* DoPerformanceOnCurricItems();
	oofRep* DoStudentReportWithLegend();
	oofRep* DoMultiReport();
	oofRep* DoPictureTest();
	oofRep* DoStudentNotes();
	oofRep* DoJustGraphs();
	oofRep* DoManyGraphs();
	oofRep* DoNotesWithCustomDraw();
	oofRep* DoSingleNote();
	void InitialiseReports();
};


inline unsigned short
CSampleBase::numberOfReps() const
{
	return mNumReports;
}

#endif //H_SAMPLE_BASE
