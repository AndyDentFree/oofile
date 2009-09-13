// Copyright 2000 A.D. Software. All Rights Reserved

// OOFTEST68


// This test checks out outdir functionality
// it is based on ooftest44 but adds functionality to test creation and moving between directories
// This sample tests the directory iteration in the utility class 
// oofDirectoryIterator and using an iterator oofDirectoryList

// it also shows how to define your own directory visitor

// Simple stream I/O is used to interact with the user.


#include "oofile.h"
#include "ooffiles.h"  // to get the oofDirectory and oofDirectoryIterator classes
#include "oofexcep.h"
#include "oofrecs.h"


class countDirs : public oofDirectoryVisitor {
public:
	countDirs();
	~countDirs();
	virtual void visit(const oofFileRef&, oofDirectory *);
	virtual bool enterDir(const oofFileRef&, oofDirectory *);
private:
	long mDirCount;
};


countDirs::countDirs() :
	mDirCount(0)
{}


countDirs::~countDirs()
{
	cout << "number of directories entered: " << mDirCount << endl;
}


void 
countDirs::visit(const oofFileRef&, oofDirectory *)
{
// always have to override, just to save people making mistakes
}


bool 
countDirs::enterDir(const oofFileRef&, oofDirectory *)
{
	mDirCount++;
	return true;
}


int main()
{
	cout << "OOFILE Validation Suite - Test 68\n"
		 << "Simple test to iterate the current directory"  << endl << endl;

cout << "Dump the current directory" << endl << endl;

	oofDirectory currentDir;
	cout << currentDir;  // default output via an oofDirectoryList
	
cout << " make a new directory and dump the current directory.Current directory should have the new nested Dirs added." << endl << endl;
	
	oofDirectory anotherDir(":fee:fi:fo:fum:","",true);// create if missing
	cout << currentDir;  // default output via an oofDirectoryList
	
	anotherDir.gotoDirectory();

cout << " goto the new directory and dump the current directory, should only be fum:." << endl << endl;
	//oofDirectory::gotoDirectory(":fee:fi:fo:fum:");
	cout << currentDir;  // default output via an oofDirectoryList
	

	cout << endl << " now starting a directory tree" << endl;
	oofDirectory dirAboveUs("::");
	//dirAboveUs.visitFiles(oofDirectoryList(cout, false, "\t"));  // just dir tree, with tabs
	oofDirectoryList  theList(cout, false, "\t");  // just dir tree, with tabs david debug 000515
	dirAboveUs.visitFiles(theList);  // just dir tree, with tabs
	
	
	cout << endl << "now starting our directory counter" << endl;
	try {
		dirAboveUs.visitFiles((oofDirectoryVisitor&)countDirs());
	}
	catch(oofE_AliasNotResolved&) {
		cout << "bad alias interrupted us" << endl;
	}

/*	
AD removed 2001-05-24 as these functions removed - they were somewhat redundant and only
implemented on Mac.
	cout << endl << "Test standalone OOF_SetPath function:" << endl;
	cout << endl << "Set path to \":humpty:Dumpty:sat:\" then dump dir above us"<< endl;
	cout << endl << "should just show sat:" << endl;
	OOF_SetPath(":humpty:Dumpty:sat:");
	
	oofDirectory thisNewNestedDir("::");
	
	cout << thisNewNestedDir;  // default output via an oofDirectoryList
	cout << endl << "Now Test standalone OOF_CheckPath function:" << endl;
	cout << "With call OOF_SetPath(\":::\"); should get: " << endl
   		 << "humpty:" << endl
     	 <<	 "  Dumpty:" << endl
     	 <<  "    sat:"  << endl << endl;
 	OOF_SetPath(":::");
	oofDirectory WhereAreWeNow("::");
	
	cout << WhereAreWeNow;  // default output via an oofDirectoryList
*/	
	
	
	cout << endl << "Next test oofFileRef::copyToDirectory :" << endl;
	
	
	
	oofFileRef aTestFile("On The Wall",oofFileRef::eFile) ;
	aTestFile.fopen("w");// should create it
	aTestFile.copyToDirectory(thisNewNestedDir, true);

	cout << endl << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

