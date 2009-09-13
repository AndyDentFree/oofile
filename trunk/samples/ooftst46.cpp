// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST46

// This sample tests the directory iteration in the utility class 
// oofDirectoryIterator and how to define a hierarchy in an OOFILE
// database. A custom directory visitor is used to populate a
// database of the directory contents.

// This sample includes calls to use profiling with the Metrowerks
// CodeWarrior compiler

// Simple stream I/O is used to interact with the user.

#include "oofexcep.h"
#include "ooftest.h"

#include "ooftst46.h"

#if __profile__ && defined(__MWERKS__)
	#include <profiler.h>
#endif

// -------------------------------------------------------
//           d b D i r T r e e 
// -------------------------------------------------------
void 
dbDirTree::Add(const oofFileRef& inFile)
{
	newRecord();
	Name = inFile.filename();
	IsFolder = inFile.isDirectory();
	LastModified = inFile.lastModified();
	saveRecord();
}


void 
dbDirTree::AddRoot(const char* inName)
{
	assert(countAll()==0);
	newRecord();
	Name = inName;
	IsFolder = true;
	LastModified = 0;  // don't know!
	saveRecord();
}


void 
dbDirTree::SelectRoot()
{
	search(Parent.nullRelationship());
}


void
dbDirTree::Dump(ostream& os, long indentLevel)
{
	if (indentLevel==0)
		SelectRoot();
	for (start(); more(); next()) {
		for (long i=0; i<indentLevel; i++)
			os << "  ";  // indent two spaces per level
		os << Name << "   " << LastModified << endl;
		Children->Dump(os, indentLevel+1);
	}
}


void
dbDirTree::deleteAll()
{
	Children.propagateRelatedDeletes(false);
	dbTable::deleteAll();  // now can just delete without cascading
	Children.propagateRelatedDeletes(true);
	
	/* alternative implementation
		SelectRoot();
		deleteRecord();  // take full advantage of our cascading deletes!
	*/
}


// -------------------------------------------------------
//             D i r T r e e B u i l d e r
// -------------------------------------------------------
DirTreeBuilder::DirTreeBuilder(const oofDirectory& rootDir, dbDirTree& inTree) : 
	mDirTree(inTree) 
{
	mDirTree.suspendSorting();
	mDirTree.AddRoot(rootDir.filename());  // the root is the dir we are visiting
	mCurrentDirTreeLevel = mDirTree.Children;  
	// see oofmacro.h, REL_REF_BODY, operator tb* if interested in how this works
	// this is a pointer to a dbTable object, NOT the records contained in the table
	// as it is a related table, records added will automatically be added as related records,  
	// and will point back to their parent record (pointer relationships store record offsets)
	// if we used unique ID's for the relationships, IDs would be propagated

	mCurrentDirTreeLevel->setSaveOption(dbTable::requireExplicit);  
	// so as we dirty each child record it doesn't propagate this state back to the parent
	// otherwise we'd have to save all the way back to the root each time
	// using dbTable::baseTableOfRelationChain() 
}


DirTreeBuilder::~DirTreeBuilder() 
{
	mDirTree.resumeSorting();
}


void 
DirTreeBuilder::visit(const oofFileRef& inFile, oofDirectory*)
{
// visit is called for all the non-directories 
	mCurrentDirTreeLevel->Add(inFile);
}


bool 
DirTreeBuilder::enterDir(const oofFileRef& inRef, oofDirectory*)
{
// enterDir is called instead of visit. We first add the just-entered directory 
// as if it were just another file in the parent directory
	mCurrentDirTreeLevel->Add(inRef);
	mCurrentDirTreeLevel = mCurrentDirTreeLevel->Children;
	mCurrentDirTreeLevel->setSaveOption(dbTable::requireExplicit);  
	return true;
}


void
DirTreeBuilder::leaveDir(const oofFileRef&, oofDirectory* /*caller*/)
{
	mCurrentDirTreeLevel = mCurrentDirTreeLevel->Parent;
	assert(mCurrentDirTreeLevel);
}


// -------------------------------------------------------
//            m a i n
// -------------------------------------------------------
int main()
{
	TEST_CONNECT    theDB;
	dbDirTree     DirTree;
	dbRelationship (DirTree.Children, DirTree.Parent);  
	// relate to self to define recursive tree
	// WARNING the order is important as it governs deletes
	// think of it as relating FROM the 'outward' link TO the 'inward' link
	// look at ooftst02 for contrast as it is between two different tables, not back to itself
	
	DirTree.setSortOrder(DirTree.Name);  
	// in this recursive relationship, will propagate to related tables so all in Name order unless changed explicitly

	cout << "OOFILE Validation Suite - Test 46\n"
		 << "Iterate the current directory and store tree in a directory database"  << endl << endl;

	#ifdef TESTING_DBASE
		#ifdef _Macintosh
			const char* kExistsName =  ":ooftst46:DirTree.dbf";
			const char* kDatabaseName = ":ooftst46:";
		#else
			const char* kExistsName =   "DirTree.dbf";
			const char* kDatabaseName = "";
		#endif	

	#else
		const char* kDatabaseName = "ooftst46.db";
		const char* kExistsName = kDatabaseName;
	#endif
	
	if (dbConnect::fileExists(kExistsName)) {
		theDB.openConnection(kDatabaseName);

		cout << endl << "Dump the tree last created" << endl;
		DirTree.Dump(cout);
		
		DirTree.deleteAll();
	}
	else {
		theDB.newConnection(kDatabaseName);
	}

	
	try {
		oofDirectory dirAboveUs( oofDirectory::parentDirPath() );

		cout << endl << "now starting our directory database building" << endl
			 << "for directory: " << dirAboveUs.path() << endl;

#if defined(__MWERKS__) && __profile__
	ProfilerInit(collectDetailed, bestTimeBase, 1000 /* functions */, 50 /*inStackDepth*/);
	ProfilerSetStatus(true);
#endif
		 DirTreeBuilder builder(dirAboveUs, DirTree);
		dirAboveUs.visitFiles(builder );  // custom visitor updates database

		//dirAboveUs.visitFiles( DirTreeBuilder(dirAboveUs, DirTree) );  // custom visitor updates database
		// use inline variant above if don't mind warnings about non-const ref to temporary
#if defined(__MWERKS__) && __profile__
	ProfilerDump("\pooftst46.prof");
	ProfilerTerm();
#endif

	}
	catch(oofE_AliasNotResolved&) {
		cout << "bad alias interrupted us" << endl;
	}
	
	cout << endl << "Dump the hierarchical tree we just created" << endl;
	DirTree.SelectRoot();
	DirTree.Dump(cout);  // Dump is recursive and can operate to dump any subtree, ie: start where you like
	cout << endl << "Test Completed" << endl;
	
#ifdef _Windows
	cout << endl << "CR to exit" << flush;
	cin.ignore(1);
#endif
	return EXIT_SUCCESS;
}       

