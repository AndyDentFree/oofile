// Copyright 1998 A.D. Software. All Rights Reserved

// included in ooftst46

// both Ref and Set needed as directory points to itself
#ifndef OOFTST46_H
#define OOFTST46_H

#include "oofile.h"
#include "ooffiles.h"  // to get the oofDirectory and oofDirectoryIterator classes


DECLARE_REF(dbDirTree)
DECLARE_SET(dbDirTree)
	
class dbDirTree : public dbTable {
OOFILE_METHODS(dbDirTree)
#ifdef _Macintosh
	enum {kNameLen=31};
#else
	enum {kNameLen=200};  // should be 511 but cut down to 200 for standard c-tree build test
#endif
	dbChar			Name;
	dbDateTime		LastModified;
	dbDirTreeRef	Parent;
	dbDirTreeSet	Children;
	dbBool			IsFolder;

	dbDirTree() : dbTable("DirTree"),
				Name(kNameLen, "Name", kIndexCompressPadding),
				IsFolder("Is Folder")
	// requires custom c-tree Build on PC to index large field, default is 200 chars
	// unless you define MAXLEN in ctoptn.h, otherwise you will get err 45
	{
		LastModified.formatMask("HH:MIN:SEC DD/Mon/YYYY");
	};
	
// my own data entry procedures
	void Add(const oofFileRef&);
	void AddRoot(const char*);
	
	virtual void deleteAll();  // override dbTable 
	
// tree navigation and output
	void SelectRoot();
	void Dump(ostream& os, long indentLevel=0);
};


class DirTreeBuilder : public oofDirectoryVisitor {
public:
	DirTreeBuilder(const oofDirectory&, dbDirTree& inTree);
	~DirTreeBuilder();
	virtual void visit(const oofFileRef&, oofDirectory*);
	virtual bool enterDir(const oofFileRef&, oofDirectory*);
	virtual void leaveDir(const oofFileRef&, oofDirectory*);

private:
	dbDirTree&	mDirTree;
	dbDirTree*	mCurrentDirTreeLevel;
};


#endif

