// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST44

// This sample tests the directory iteration in the utility class 
// oofDirectoryIterator and using an iterator oofDirectoryList

// it also shows how to define your own directory visitor

// Simple stream I/O is used to interact with the user.

#include "oofile.h"
#include "ooffiles.h"  // to get the oofDirectory and oofDirectoryIterator classes
#include "oofexcep.h"



class countDirs : public oofDirectoryVisitor {
public:
	countDirs();
	~countDirs();
	virtual void visit(const oofFileRef&, oofDirectory* caller=0);
	virtual bool enterDir(const oofFileRef&, oofDirectory* caller=0);
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
countDirs::visit(const oofFileRef&, oofDirectory*)
{
// always have to override, just to save people making mistakes
}


bool 
countDirs::enterDir(const oofFileRef&, oofDirectory*)
{
	mDirCount++;
	return true;
}


int main()
{
	cout << "OOFILE Validation Suite - Test 44\n"
		 << "Simple test to iterate the current directory"  << endl << endl;

	oofDirectory currentDir;
	cout << currentDir;  // default output via an oofDirectoryList
	
	cout << endl << " now starting a directory tree" << endl;
	oofDirectory dirAboveUs("::");
	//dirAboveUs.visitFiles(oofDirectoryList(cout, false, "\t"));  // just dir tree, with tabs
	oofDirectoryList  theList(cout, false, "\t");  // just dir tree, with tabs david debug 000515
	dirAboveUs.visitFiles(theList);  // just dir tree, with tabs
	
	
	cout << endl << "now starting our directory counter" << endl;
	try {
		// will get warning about non-const ref to temporary
		dirAboveUs.visitFiles(countDirs());
	}
	catch(oofE_AliasNotResolved&) {
		cout << "bad alias interrupted us" << endl;
	}
	
	
	cout << endl << "Test Completed" << endl;
	
	return EXIT_SUCCESS;
}       

