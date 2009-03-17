#ifndef H_OOFFILES
#define H_OOFFILES

// COPYRIGHT 1996-1998 A.D. Software, All rights reserved

// see misc.txt for code conventions
#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif

#ifdef _Macintosh
	#include <Types.h>
	#include <Files.h>
#else	
	#ifdef _Windows
		#ifndef __SHELLAPI_H
			#include <shellapi.h>
		#endif
		#if  !OOF_USE_ANSI_STREAMS
		// need headers to define tm
		#endif
	#endif
#endif	

#ifndef _INC_TIME
	#include <time.h>
#endif	


/**
	\defgroup oofFiles OOFILE file and directory manipulation.
	Cross-platform classes for iterating directory trees,
	copying and managing files, reading and writing text files.
	Used for file archiving and web page index maintenance programs.
	Included in the Open Source OOFILE.
	\ingroup appAuthor
*/


/**
	Portable way to refer to files.
	Macintosh version allows specification via FSSpec structure and vol/dir ID plus filenames
	as well as the traditional file path string.
	\todo replace FSSpec with FSRef for later Carbon builds and Mach-O on Macintosh.
	\todo add 	oofBlob readBinaryFile() const; 
	\todo add 	void writeBinaryFile(oofBlob&) const;
	\ingroup oofFiles
*/
class OOFILE_EXPORT oofFileRef {
public:
	enum isDirT { eDir, eFile, eNotSet };

	oofFileRef(const oofString&, isDirT isDir=eNotSet);
	oofFileRef(const char* path=0, isDirT isDir=eNotSet) ;
	oofFileRef(const oofDirectory& theDir, const oofString& name, isDirT isDir=eNotSet);
#ifdef _Macintosh
	oofFileRef(const FSSpec&, isDirT isDir=eNotSet);
	oofFileRef(short vRefNum, long dirID, const oofString& name, isDirT isDir=eNotSet);
	operator FSSpec&() { return mSpec; };
	const FSSpec& spec() const { return mSpec; };
	const oofFileRef& operator=(const FSSpec&);
	bool scanVolsUntilFound();
#endif
	// use default copy ctor, op=, dtor
	const oofFileRef& operator=(const oofString&);
	void clear();  // useful to point the ref elsewhere!

	bool isValid() const;
		
// feature methods
	oofFileRef makeRefToContainedFile(const oofString& name) const;
	void extract(ostream&) const;
	FILE* fopen(const char* mode="rb") const;  // make it easy to use C io calls 

/// \name file manipulation that can throw exceptions, oofE_MacOSErr or oofE_General
//@{
	void gotoParentDirectory() const;  // make the dir containing file current dir
	oofString readTextFile() const;  // opens and closes file
	void writeTextFile(oofString& theStr) const;
	void deleteFile(bool ignoreMissingFiles=true) const;    // like a const pointer - the ref remains untouched but file zapped
	void renameFile(const oofString& newName);
	void copyToDirectory(const oofDirectory&, bool overwriteDest=false) const;
	void moveToDirectory(const oofDirectory&, bool overwriteDest=false);
//@}
	
/// \name reflective methods
//@{
	oofString path() const;  
	oofString filename() const;
	oofDirectory directory() const;
	bool isDirectory() const;
	tm created() const;
	tm lastModified() const;
	unsigned long fileSize(bool wantPhysical=true) const;
	bool dirIsParent(const oofDirectory&) const;
//@}
	
#ifdef _Macintosh
	OSErr readCInfo(CInfoPBRec& cibr) const; // utility useful to get file info, fnfErr if invalid
	void setFileType(OSType typeCode, OSType creatorCode=0);
	void flushVolume() const;
#endif		

private:

// data storage
#ifdef _Macintosh
	FSSpec	mSpec;
#else
	oofString	mPath;	///< path variable for other platforms, Mac creates path() on fly	
#endif
	isDirT  mIsDir;  ///< tri-state flag to optimise processing if we KNOW we are a directory or file
};
OOFILE_EXPORT ostream& operator<<(ostream&, const oofFileRef&);


/**
	Abstract base class used to visit directory tree.
	Descends tree depth first including traversing aliases on Mac.
	\ingroup oofFiles
*/
class OOFILE_EXPORT oofDirectoryVisitor {
public:
	oofDirectoryVisitor(bool ignoreBadAliases=false);

	virtual ~oofDirectoryVisitor() {};		// so subclasses get dtor call
	virtual void visit(const oofFileRef&, oofDirectory* caller=0)=0;
	virtual bool enterRoot(oofDirectory* caller=0);  ///< enterDir for starting directory
	virtual bool enterDir(const oofFileRef&, oofDirectory* caller=0);
	virtual void leaveDir(const oofFileRef&, oofDirectory* caller=0);
	virtual bool handleLinkUnresolved(const oofFileRef&);  ///< default implementation returns false

protected:
	short		mDirDepth;
	bool		mIgnoreBadAliases;
};


/**
	utility visitor used to output directory.
	\ingroup oofFiles
*/
class OOFILE_EXPORT oofDirectoryList : public oofDirectoryVisitor {
public:
	oofDirectoryList(ostream& os, bool listFiles=true, const char* indentString="  ");
	virtual void visit(const oofFileRef&, oofDirectory* caller);
	virtual bool enterDir(const oofFileRef&, oofDirectory* caller);
	virtual void leaveDir(const oofFileRef&, oofDirectory* caller);
	virtual bool handleLinkUnresolved(const oofFileRef&);  
	
protected:
	virtual void WriteIndentedName(const oofFileRef&) const;
	virtual void WriteIndent() const;
// data storage	
	ostream&	mOS;	
	long		mFilesVisited;
	oofString	mIndentString;
	bool		mListFiles;
};


/**
	utility visitor used to delete directory tree.
	\ingroup oofFiles
*/
class OOFILE_EXPORT oofDelDirTreeVisitor : public oofDirectoryVisitor {
public:
	oofDelDirTreeVisitor(bool delSubdirs);
	virtual void visit(const oofFileRef&, oofDirectory* caller);
	virtual void leaveDir(const oofFileRef&, oofDirectory* caller);  // del dir itself on exit
	
protected:
// data storage	
	bool mDelSubdirs;  ///< flag to indicate if leave empty dir structure behind
};


class OOF_ExpandableLongArray;  // forward

/**
	List of oofFileRef objects typically used in a directory listing.
	Can minimally iterate using count() and ref(i).
	\ingroup oofFiles
*/
class oofFileList
{
public:
	oofFileList();
	oofFileList(const oofFileList&);
	~oofFileList();
	
	void append(const oofFileRef&);
	oofFileRef ref(unsigned short fileIndex) const;
	unsigned short count() const;
	
private:
	OOF_ExpandableLongArray*	mRefs;	// owned
};


/**
	Provides cross-platform directory specification and iteration.
	\see oofDirectoryVisitor for iterator object, eg; used to list
	directories or read a tree into a database (ooftst46).
	\ingroup oofFiles
*/
class OOFILE_EXPORT oofDirectory {
public:
	oofDirectory();
#ifdef _Macintosh
	oofDirectory(short vRefNum, long dirID, bool searchForDirAllVols=false);
	oofDirectory(short vRefNum, long dirID, const char* fallBackPath, bool searchForDirAllVols=false);
	oofDirectory(const FSSpec&);
#endif
	oofDirectory(const char* path, char* outFileName=0, bool createSubdirsIfMissing=false, bool makeAbsolute=true);
	// a non-null outFileName indicates the caller expects the path to
	// terminate in a filename
// use default copy ctor
	virtual ~oofDirectory();
	
	void setDirFromPath(const char* path, char* outFileName=0, bool createSubdirsIfMissing=false, bool makeAbsolute=true);	
	void gotoDirectory() const;

/// \name reflective functions
//@{
	bool validDir() const;
	bool containsFile(const char* justFileName) const;
	const oofString& path() const;	
	oofString filename() const;
	bool isEmpty() const;
	bool createdDiskDir() const;  // ie: created it when this object was constructed
//@}

/// \name file lists and visiting
//@{
	oofFileList* makeFileList()  const; 
	virtual void visitFiles(oofDirectoryVisitor&, unsigned long remainingVisitDepth=1024, bool firstVisit=true /* private param, don't set */);  // visits tree within dir, not including dir!
	void extract(ostream&);  // directory listing
//@}
	
#ifdef _Macintosh
/// launch files as if double-click on desktop
	bool openFile(const char* justFileName) const;
#elif defined _Windows
	bool openFile(const char* justFileName, HWND hwnd=NULL) const;
#endif

    void delDirectory(bool delDirAsWellAsContents=true, bool delTree=true, bool delSubdirs=true);  // del contents and optionally dir itself
	
#ifdef _Macintosh
	void fillInFSSpec(const char* nameCanBeNull , FSSpec&) const;  // oofDirectory plus optional filename -> FSSpec
#elif defined _Windows
	static bool convertMac2WinPath(const char* macPath, char* winPath);
#endif
	
	
protected:
	void GetCurrentLocation();

#ifdef _Macintosh
	void PathToDirID(const unsigned char* pascalPath);
	void createDirID(const unsigned char* pascalPartialPath=0, bool searchForDirAllVols=false);
	void CreateHardDirID(const unsigned char* pascalPartialPath, bool searchForDirAllVols);
	void CreateWorkingDirID(const unsigned char* pascalPartialPath=0, bool searchForDirAllVols=false);
	void CreateSubDirs(const unsigned char* pascalPartialPath);
	void CreateSubdirsAndWorkingDirID(const unsigned char* pascalPartialPath);

// data storage
	short mVRefNum;  ///< volume number of a mounted volume, not meaningful over reboots
	short mWorkingDirID; ///< old Mac Classic Working Directory number, combines mVRefNum & mDirID
	long mDirID;  ///< directory ID on specified volume, preserved unless dir recreated.
#endif

#ifdef _Windows
	#if(WINVER >= 0x0400)
		void FillInExecInfo(const char* filename, SHELLEXECUTEINFO& sec, HWND hwnd) const;
	#endif /* WINVER >= 0x0400 */
	static bool CreateSubdirectories(char* dir);
	static bool TransformMacDir(const char* macDir, char* winDir);
	static bool FindWinDrive(char* winDrive, const char* macVol, const char* winDir, const char* fname);
	static bool SplitMacPath(const char* macPath, char* vol, char* dir, char* file, char* ext);

	enum {
		MAX_MAC_VOL		= 50,
		MAX_MAC_DIR		= 256,
		MAX_MAC_FNAME	= 256,
		MAX_MAC_EXT		= 10
	};
#endif

	oofString	mPath;
	bool	mValidDir;
	bool mCreatedDiskDir;
	
public:
	static oofDirectory createDirectory(const char* path);
	static bool fileExists(const char* fullPath);
	static void gotoDirectory(const oofString&);
#ifdef _Macintosh
	static oofString FSSpecToPath(const FSSpec&);
	static void gotoDirectory(short vRefNum, long dirID);
 	void closeWDDirectory();
#endif

// utilities for creating and parsing paths
	static const char* parentDirPath();
	static const char* relativeRootPath();
	static char directoryTerminator();
};

OOFILE_EXPORT ostream& operator<<(ostream& os, oofDirectory&);


/**
	Save the current directory and restore when we leave scope.
	Very useful when you need to change current directory if ANSI file I/O
	being used that only opens files in the current dir, not handling paths.
	\ingroup oofFiles
*/
class OOFILE_EXPORT stSaveDirectory {
public:
	stSaveDirectory();
	~stSaveDirectory();

private:
	oofDirectory	mSavedDir;
};

 
// define these exceptions here to avoid oofexcep.h having to know about ooffile.h
#ifdef _Macintosh
/**
	Exception for file operation on the Mac, able to include a file system error number.
	\ingroup oofExceptions oofFiles
*/
	class OOFILE_EXPORT oofE_File : public oofE_MacOSErr {  // _Macintosh
	public:
		oofE_File(const oofFileRef& f, const OSErr e, const char* extraMsg=0);
		const oofFileRef& file() const;
	
		virtual void extract(ostream& os) const;
		
	private:
		oofFileRef mFile;
	};

#else
/**
	Exception for file operation on other than Mac, where we only know file details.
	\ingroup oofExceptions oofFiles
*/
	class OOFILE_EXPORT oofE_File : public oofE_Base {  // _Windows or _Unix
	public:
		oofE_File(const oofFileRef& f, const char* extraMsg=0);
		const oofFileRef& file() const;
	
		virtual void extract(ostream& os) const;
		
	private:
		oofFileRef mFile;
		oofString mErrorString;
	};
#endif


/**
	Exception for file not found error, thrown only if you needed the file.
	It is legal to construct an oofFileRef for a missing file - you may create it!
	\ingroup oofExceptions oofFiles
*/
class OOFILE_EXPORT oofE_FileNotFound : public oofE_File {
public:
	oofE_FileNotFound(const oofFileRef&, const char* extraMsg=0);
};


/**
	Exception for directory not valid error, thrown only if you needed the dir.
	It is legal to construct an oofDirectory for a missing dir - you may create it!
	\ingroup oofExceptions oofFiles
*/
class OOFILE_EXPORT oofE_InvalidDir : public oofE_Base {
public:
	oofE_InvalidDir(const oofDirectory& d, const char* extraMsg=0) : 
		mDir(d),
		mErrorString(extraMsg)
	{};
	const oofDirectory& dir() const;

	virtual void extract(ostream& os) const;
	
private:
	oofDirectory mDir;
	oofString mErrorString;
};


/**
	Exception for failure to resolve an alias (Mac) or link (Windows/Unix).
	\ingroup oofExceptions oofFiles
*/
class OOFILE_EXPORT oofE_AliasNotResolved : public oofE_Base {
public:
	oofE_AliasNotResolved(const oofFileRef& inRef) : mRef(inRef) {};
#ifdef _Macintosh
	const FSSpec& spec() const;
#endif
	const oofFileRef& fileRef() const;
	
	virtual void extract(ostream& os) const;
	
private:
	oofFileRef mRef;
};


/**
	Subclass of oofString to isolate path manipulation functions.
	For converting between different platforms especially for
	opening databases and creating oofFileRef's with path strings
	from another platform.
	
	Attempts to classify paths as absolute, relative or parent-relative
	as shown in the pathTypeT enum below but can be confused by mixing
	characters. eg: starting a Mac directory name with a \ or / will be
	confused with DOS or Unix conventions.
	
	NOT YET IMPLEMENTED - possibly an application can guarantee the context
	of an original path so the path conversion knows to ignore characters 
	that suggest a different origin.
	
	\warning if you use the static converters such as relativePathDOS be aware
	they may allocate a new buffer which they return for you to dispose. 
	It is usually safer for you to create an oofPathString object and use it
	to manage the conversion. 
	
	\ingroup oofFiles
*/	
class OOFILE_EXPORT oofPathString : public oofString {
public:
// limited subset of ctors
	oofPathString(const char* str=0) : oofString(str){};  // default
	oofPathString(const char* chars, unsigned long numChars) : 
		oofString(chars, numChars){};
	oofPathString(char* adoptedChars, unsigned long numChars, bool adopt) : 
		oofString(adoptedChars, numChars, adopt){};
	
	enum pathTypeT {
		eMacRelPath, eUnixRelPath, eDOSRelPath, 
		eMacAbsPath, eUnixAbsPath, eDOSAbsPath, 
		eMacParentRelPath, eUnixParentRelPath, eDOSParentRelPath, 
		eJustFile 
	};
	
	pathTypeT pathType() const;

	bool isRelativePath() const;
	static bool isRelativePath(const char*);
	static bool isRelativePath(pathTypeT);
	
	static bool isMacPath(pathTypeT);
	static bool isDOSPath(pathTypeT);
	static bool isUnixPath(pathTypeT);
	
	
/// \name in-place content converters
//@{
	bool relativePathPlatform(bool appendSeparator = true);
	bool relativePathMac(bool appendSeparator = true);
	bool relativePathUnix(bool appendSeparator = true);
	bool relativePathDOS(bool appendSeparator = true);
//@}

/** \name content converters for external calls.
	\param inChars pointer to buffer which may be updated in place
	\return 0==no change, new buffer or inChars if modify but length unchanged	
*/
//@{	
	static char* relativePathDOS(char*, const unsigned long& ioLen, bool appendSeparator= true);
	static char* relativePathMac(char*, const unsigned long& ioLen, bool appendSeparator= true);
	static char*  relativePathUnix(char*, const unsigned long& ioLen, bool appendSeparator= true);
//@}

/// content test for external calls, taking path and returning type
	static pathTypeT pathType(const char*);
};
OOFILE_EXPORT ostream& operator<<(ostream&, oofPathString::pathTypeT);


// -------------------------------------------------------
//                o o f D i r e c t o r y
// -------------------------------------------------------

inline bool
oofDirectory::validDir() const
{
	return mValidDir;
}


inline bool
oofDirectory::createdDiskDir() const
{
	return mCreatedDiskDir;
}


inline const oofString&  
oofDirectory::path() const
{
	return mPath;
}


inline const char*
oofDirectory::parentDirPath()
{
#ifdef _Macintosh
	return "::";
#elif defined _Windows
	return "..\\";
#else   // _Unix
	return "../";
#endif
}


inline const char*
oofDirectory::relativeRootPath()
{
#ifdef _Macintosh
	return ":";
#elif defined _Windows
	return ".\\";
#else   // _Unix,
	return "./";
#endif
}


inline char 
oofDirectory::directoryTerminator()
{
#ifdef _Macintosh
	return ':';
#elif defined _Windows
	return '\\';
#else   // _Unix
	return '/';
#endif
}

 
// -------------------------------------------------------
//                o o f F i l e R e f
// -------------------------------------------------------
#ifdef _Macintosh
inline oofDirectory 
oofFileRef::directory() const
{
	return oofDirectory(mSpec.vRefNum, mSpec.parID);
}


inline void 
oofFileRef::gotoParentDirectory() const
{
// throws oofE_InvalidDir if fails
	oofDirectory::gotoDirectory(mSpec.vRefNum, mSpec.parID);
}


#else
inline oofString 
oofFileRef::path() const
{
	return mPath;
}

#endif


// -------------------------------------------------------
//    o o f E _ A l i a s N o t R e s o l v e d
// -------------------------------------------------------
#ifdef _Macintosh
inline const FSSpec&
oofE_AliasNotResolved::spec() const
{
	return mRef.spec();
}
#endif


inline const oofFileRef&
oofE_AliasNotResolved::fileRef() const
{
	return mRef;
}


// -------------------------------------------------------
//           o o f E _ I n v a l i d D i r
// -------------------------------------------------------
inline const oofDirectory&
oofE_InvalidDir::dir() const
{
	return mDir;
}


// -------------------------------------------------------
//           o o f E _ F i l e
// -------------------------------------------------------
inline const oofFileRef&
oofE_File::file() const
{
	return mFile;
}


// -------------------------------------------------------
//                o o f P a t h S t r i n g
// -------------------------------------------------------
inline bool
oofPathString::relativePathPlatform(bool appendSeparator)
{
#if defined(_Macintosh)
	return relativePathMac(appendSeparator);
#elif defined(_MSDOS)  // implies _Windows
	return relativePathDOS(appendSeparator);
#elif defined(_Unix)
	return relativePathUnix(appendSeparator);
#else
	ERROR "must have _Macintosh _Windows or _Unix defined"
#endif
}


inline bool 
oofPathString::isRelativePath(pathTypeT thePath)
{
	const bool ret =  (
		(thePath == eMacRelPath) || (thePath == eUnixRelPath) || 
		(thePath == eDOSRelPath) || (thePath == eMacParentRelPath ) ||
		(thePath == eDOSParentRelPath) || (thePath == eUnixParentRelPath)
	);
	return ret;
}


inline bool 
oofPathString::isMacPath(pathTypeT thePath)
{
	const bool ret =  (
		(thePath == eMacRelPath) || (thePath == eMacAbsPath) || 
		(thePath == eMacParentRelPath)
	);
	return ret;
}


inline bool 
oofPathString::isDOSPath(pathTypeT thePath)
{
	const bool ret =  (
		(thePath == eDOSRelPath) || (thePath == eDOSAbsPath) || 
		(thePath == eDOSParentRelPath)
	);
	return ret;
}


inline bool 
oofPathString::isUnixPath(pathTypeT thePath)
{
	const bool ret =  (
		(thePath == eUnixRelPath) || (thePath == eUnixAbsPath) || 
		(thePath == eUnixParentRelPath)
	);
	return ret;
}


inline bool 
oofPathString::isRelativePath() const
{
	return isRelativePath(mBody);
}


inline oofPathString::pathTypeT 
oofPathString::pathType() const
{
	return pathType(mBody);
}

#endif


