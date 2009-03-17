// COPYRIGHT 1996 A.D. Software, All rights reserved
// file utilities for OOFILE database

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif

#include <ctype.h>
#ifndef _INC_TIME
	#include <time.h>
#endif

#ifdef _Unix
	#include <dirent.h>
	#include <sys/stat.h>
	#include <errno.h>
#elif defined(_Windows)
	#ifndef _INC_TYPES
		#ifdef __MWERKS__
			#include <types.h>
			#include <tchar.h>
		#else
		#include <sys/types.h>
	#endif
	#endif
	#ifndef _INC_STAT
		#ifdef __MWERKS__
			#include <stat.h> 
		#else
			#include <sys/stat.h>
		#endif
		#ifdef __BCPLUSPLUS__
	        #if __BCPLUSPLUS__ >= 0x0540
			    #define _stat stat
	        #endif
        #endif
	#endif
#elif defined(_Macintosh)
	#ifndef __DATETIMEUTILS__
		#include <DateTimeUtils.h>  // for SecondsToDate
	#endif
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef _Macintosh
//   STUFF FROM DTS SNIPPET - SendFinderOpen.c

/* This snippet shows you how send a Finder OpenSelection event. */
/* A Finder OpenSelection allows you to have the Finder launch an Application, or */
/* open a document, which will of course cause the owning Application to launch and */
/* open that document.  Kinda just like the user had double-clicked on the file */
/* from a Finder window. */
/* This is an alternate to using the LaunchWithDoc code (somewhere else on this CD ) */
/* which shows you how to launch an application with a document.  Look for that sample and */
/* contrast the two methods, and pick the best for your use. */
/* C.K. Haun */
/* Apple Developer Tech Support */

/* include the ususal suspects, all the .h's you need  */
#include <Types.h>
#include <memory.h>
#include <Packages.h>
#include <Errors.h>
#include <quickdraw.h>
#include <fonts.h>
#include <dialogs.h>
#include <windows.h>
#include <Menus.h>
#include <Devices.h>
#include <Events.h>
#include <diskinit.h>
#include <OSUtils.h>
#include <resources.h>
#include <toolutils.h>
#include <AppleEvents.h>
#include <EPPC.h>
#include <Gestalt.h>
#include <PPCToolbox.h> 
#include <Processes.h>
#include <Aliases.h>
#include <Files.h>

#define aeSelectionKeyword		 'fsel'
#define aeOpenSelection 'sope'
#define kFinderSig 'FNDR'
#define kSystemType 'MACS'
/* prototype for process finding routine */
OSErr FindAProcess(OSType typeToFind, OSType creatorToFind, ProcessSerialNumberPtr processSN);
OSErr OpenSelection(FSSpecPtr theDocToOpen);

// -------------------------------------------------------
//          M o r e F i l e s   U t i l i t i e s - prototypes
// -------------------------------------------------------
// see MoreFiles for full comments

#define hasOpenDeny(volParms)		(((volParms).vMAttrib & (1L << bHasOpenDeny)) != 0)

/*
**	Deny mode permissions for use with the HOpenAware, HOpenRFAware,
**	FSpOpenAware, and FSpOpenRFAware functions.
*/

enum
{
	dmNone			= 0x0000,
	dmNoneDenyRd	= 0x0010,
	dmNoneDenyWr	= 0x0020,
	dmNoneDenyRdWr	= 0x0030,
	dmRd			= 0x0001,	/* Single writer, multiple readers; the readers */
	dmRdDenyRd		= 0x0011,
	dmRdDenyWr		= 0x0021,	/* Browsing - equivalent to fsRdPerm */
	dmRdDenyRdWr	= 0x0031,
	dmWr			= 0x0002,
	dmWrDenyRd		= 0x0012,
	dmWrDenyWr		= 0x0022,
	dmWrDenyRdWr	= 0x0032,
	dmRdWr			= 0x0003,	/* Shared access - equivalent to fsRdWrShPerm */
	dmRdWrDenyRd	= 0x0013,
	dmRdWrDenyWr	= 0x0023,	/* Single writer, multiple readers; the writer */
	dmRdWrDenyRdWr	= 0x0033	/* Exclusive access - equivalent to fsRdWrPerm */
};


/**
	Mac union of file system parameter blocks.
	For those times where you need to use more than one kind of File Manager parameter
	block but don't feel like wasting stack space, here's a parameter block you can reuse.
	\ingroup oofMacSpecific
*/

union OOF_UniversalFMPB
{
	ParamBlockRec	PB;
	CInfoPBRec		ciPB;
	DTPBRec			dtPB;
	HParamBlockRec	hPB;
	CMovePBRec		cmPB;
	WDPBRec			wdPB;
	FCBPBRec		fcbPB;
	XVolumeParam	xPB;
};
typedef union OOF_UniversalFMPB OOF_UniversalFMPB;
typedef OOF_UniversalFMPB *UniversalFMPBPtr, **UniversalFMPBHandle;


OSErr	MF_CopyFileMgrAttributes(short srcVRefNum,
									  long srcDirID,
									  ConstStr255Param srcName,
									  short dstVRefNum,
									  long dstDirID,
									  ConstStr255Param dstName,
									  Boolean copyLockBit);

OSErr	MF_FSpCopyFileMgrAttributes(const FSSpec *srcSpec,
										 const FSSpec *dstSpec,
										 Boolean copyLockBit);

OSErr	MF_CheckForForks(short vRefNum,
							  long dirID,
							  ConstStr255Param name,
							  Boolean *hasDataFork,
							  Boolean *hasResourceFork);

OSErr	MF_CopyFork(short srcRefNum,
						 short dstRefNum,
						 void *copyBufferPtr,
						 long copyBufferSize);

OSErr	MF_HGetVolParms(ConstStr255Param volName,
							 short vRefNum,
							 GetVolParmsInfoBuffer *volParmsInfo,
							 long *infoSize);

OSErr	MF_HCreateMinimum(short vRefNum,
							   long dirID,
							   ConstStr255Param fileName);

OSErr	MF_HCopyFile(short srcVRefNum,
						  long srcDirID,
						  ConstStr255Param srcName,
						  short dstVRefNum,
						  long dstDirID,
						  ConstStr255Param dstPathname,
						  ConstStr255Param copyName);

OSErr	MF_HOpenAware(short vRefNum,
						   long dirID,
						   ConstStr255Param fileName,
						   short denyModes,
						   short *refNum);

OSErr	MF_HOpenRFAware(short vRefNum,
							 long dirID,
							 ConstStr255Param fileName,
							 short denyModes,
							 short *refNum);


OSErr	MF_CheckVolLock(ConstStr255Param pathname,
							 short vRefNum);

OSErr	MF_GetVolumeInfoNoName(ConstStr255Param pathname,
									short vRefNum,
									HParmBlkPtr pb);

#endif  // _Macintosh


// -------------------------------------------------------
//           1 6 b i t   W i n d o w s   U t i l s
// -------------------------------------------------------
#if defined (_Windows) && !defined (_WIN32) && !defined (_Win32)
// functions that exist in Win32, but need to be emulated for Win16
#include <dir.h>
#include <errno.h>
BOOL 
SetCurrentDirectory(const char *lpPathName)
{
	if(lpPathName[1] == ':') {
		::setdisk(lpPathName[1]);
    }
    return ::chdir(lpPathName) == 0;
}


DWORD 
GetCurrentDirectory(DWORD nBufferLength, char *lpBuffer)
{
    DWORD result = 0;
	if(lpBuffer != NULL) {
		char *res = getcwd(lpBuffer, (int)nBufferLength);
	    if(res == NULL && errno == ERANGE) {
        	res = getcwd(NULL, MAXPATH);
            if(res != NULL) {
            	result = strlen(res)+1;
                free(res);
            }
	    } else {
	    	result = strlen(lpBuffer);
	    }
    }
    return result;
}


BOOL 
CreateDirectory(const char *lpPathName, void *)  // _Windows 16
{
	return ::mkdir(lpPathName) == 0;
}

// NOT YET IMPLEMENTED - other emulated functions eg: RemoveDirectory, DeleteFile

#endif


// -------------------------------------------------------
//                o o f P a t h S t r i n g
// -------------------------------------------------------


bool
oofPathString::relativePathDOS(bool appendSeparator)
{
	if (!mBody)
		return false;
										        
	char* newBody = relativePathDOS(mBody, mLen,appendSeparator);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mLen = strlen(newBody);
		mBody = newBody;
		
	}
	return (newBody!=0);
}


char*  // 0 return means no change
oofPathString::relativePathDOS(char*inChars, const unsigned long& ioLen,bool appendSeparator)
{
	const pathTypeT thePath = pathType(inChars);
	
	if (thePath==eJustFile)
		return 0;
		
	char  separator;
	const char platformSep = '\\';
	unsigned long inLen = ioLen;
	unsigned long index;	
	
	assert(isRelativePath(thePath)); //dud path? eg absolute
	// TO DO - make this an exception?
	
	bool itsMacPath = ((thePath == eMacRelPath) || (thePath ==eMacParentRelPath));
	switch(thePath){
		case eMacRelPath:
		case eMacParentRelPath:
			separator = ':';
		break;
		case eUnixRelPath:
		case eUnixParentRelPath:
			separator = '/';
		break;
		case eDOSRelPath:
		case eDOSParentRelPath:
			separator = '\\';
		break;
	}
	char* ret = 0;

	if( platformSep == separator ){
		if(appendSeparator){
			if(inChars[ioLen-1] != separator){
				ret = new char[ioLen+2];
				memcpy( ret, inChars,ioLen+2  );

				ret[ioLen] = separator;
				ret[ioLen+1] = '\0';
				#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
					OOF_MEM_DEBUG_FORCE_POOL_CHECK
				#endif
	
			}
		
		}
		return ret; 
	
	}
	// now do the translate
	unsigned long newLen = ioLen;
	unsigned long index2;
	if( itsMacPath ) 
		newLen++;
	if( (inChars[ioLen-1]!= separator) &&  appendSeparator)
		newLen++ ;
		
			
 //allocate memory to returnValue of size newString length
	ret = new char[newLen+1];
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	
	if((thePath == eMacRelPath)||(thePath ==eUnixRelPath )||(thePath ==eDOSRelPath )){
		ret[0] = '.';
		ret[1] = platformSep;
		index2 = 2;
		if(itsMacPath)
			index = 1;
		else
			index = 2;
	}
	else{
		ret[0] = '.';
		ret[1] = '.';
		ret[2] = platformSep;
		index2 = 3;
		if(itsMacPath)
			index = 2;
		else
			index = 3;	
	}
	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif

	for(/*index set above*/;index < ioLen;index++){
		if( inChars[index] == separator )
				ret[index2++] =  platformSep;
		else
			ret[index2++] =  inChars[index];
	}
	if((ret[index2-1] != platformSep )&&  appendSeparator ){
		ret[index2++] = platformSep ;
	}
	ret[index2]='\0';
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	return ret;
}



// to mac
bool 
oofPathString::relativePathMac(bool appendSeparator)
{
	if (!mBody)
		return false;
	
	char* newBody = relativePathMac(mBody, mLen,appendSeparator);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mLen = strlen(newBody);
		mBody = newBody;
	}
	return (newBody!=0);
}


char*  // 0 return means no change
oofPathString::relativePathMac(char* inChars, const unsigned long& ioLen,bool appendSeparator)
{
	const pathTypeT thePath = pathType(inChars);
	if (thePath==eJustFile)
		return 0;
		
	assert(isRelativePath(thePath)); //dud path? eg absolute
	// TO DO - make this an exception?

	char  separator;
	const char platformSep = ':';
	unsigned long inLen = ioLen;
	unsigned long index;	
	bool itsMacPath = ((thePath == eMacRelPath) || (thePath ==eMacParentRelPath));
	
	switch(thePath){
		case eMacRelPath:
		case eMacParentRelPath:
			separator = ':';
		break;
		case eUnixRelPath:
		case eUnixParentRelPath:
			separator = '/';
		break;
		case eDOSRelPath:
		case eDOSParentRelPath:
			separator = '\\';
		break;
	}
	char* ret = 0;

	
	if( platformSep == separator ){
		if(appendSeparator){
			if(inChars[ioLen-1] != separator){
				ret = new char[ioLen+2];
				memcpy( ret, inChars,ioLen+2  );

				ret[ioLen] = separator;
				ret[ioLen+1] = '\0';
				#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
					OOF_MEM_DEBUG_FORCE_POOL_CHECK
				#endif
	
			}
		
		}
		return ret; 
	}
	// now do the translate
	unsigned long newLen = ioLen;
	unsigned long index2;
	if( (thePath != eMacRelPath) ||(thePath !=  eMacParentRelPath) ) 
		newLen--;
	if( (inChars[ioLen-1]!= separator) &&  appendSeparator)
		newLen++ ;
			
 //allocate memory to returnValue of size newString length
	ret = new char[newLen+1];
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	
	if((thePath == eMacRelPath)||(thePath ==eUnixRelPath )||(thePath ==eDOSRelPath )){
		if( (thePath ==eUnixRelPath) || (thePath ==eDOSRelPath )){
			ret[0] = ':';
			index = 2;
			index2 = 1;
		}
		else{
			index = 0;
			index2 = 0;
		}
	}
	else{
		ret[0] = ':';
		ret[1] = ':';
		index2 = 2;
		if(separator == ':')
			index = 2;
		else
			index = 3;	
	}
	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif

	for(/*index set above*/;index < ioLen;index++){
		 	if( inChars[index] == separator )
				 	ret[index2++] =  platformSep;
			else
		 		ret[index2++] =  inChars[index];
	}
	if((ret[index2-1] != platformSep )&&  appendSeparator ){
		ret[index2++] = platformSep ;
	}
	ret[index2]='\0';
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	return ret;
}



bool
oofPathString::relativePathUnix(bool appendSeparator)
{
	if (!mBody)
		return false;
	
	char* newBody = relativePathUnix(mBody, mLen,appendSeparator);
	if (newBody && newBody!=mBody) {
		delete[] mBody;
		mLen = strlen(newBody);
		mBody = newBody;
	}
	return (newBody!=0);
}

//#define kPathTerminorMac ':'
//#define kPathTerminorDOS '\\'
//#define kPathTerminorUnix '/'
	
char*  // 0 return means no change
oofPathString::relativePathUnix(char*inChars,const  unsigned long& ioLen,bool appendSeparator)
{
	const pathTypeT thePath = pathType(inChars);
	if (thePath==eJustFile)
		return 0;
		
	assert(isRelativePath(thePath)); //dud path? eg absolute
	// TO DO - make this an exception?

	char  separator;
	const char platformSep = '/';
	unsigned long inLen = ioLen;
	unsigned long index;	
	bool itsMacPath = ((thePath == eMacRelPath) || (thePath ==eMacParentRelPath));
	
	switch(thePath){
		case eMacRelPath:
		case eMacParentRelPath:
			separator = ':';
		break;
		case eUnixRelPath:
		case eUnixParentRelPath:
			separator = '/';
		break;
		case eDOSRelPath:
		case eDOSParentRelPath:
			separator = '\\';
		break;
	}
	char* ret = 0;

	
	if( platformSep == separator ){
		if(appendSeparator){
			if(inChars[ioLen-1] != separator){
				ret = new char[ioLen+2];
				memcpy( ret, inChars,ioLen+2  );

				ret[ioLen] = separator;
				ret[ioLen+1] = '\0';
				#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
					OOF_MEM_DEBUG_FORCE_POOL_CHECK
				#endif
	
			}
		
		}
		return ret; 
	
	}
	// now do the translate
	unsigned long newLen = ioLen;
	unsigned long index2;
	if( itsMacPath )
		newLen++;
	if( (inChars[ioLen-1]!= separator) &&  appendSeparator)
		newLen++ ;
		
			
 //allocate memory to returnValue of size newString length
	ret = new char[newLen+1];
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	
	if((thePath == eMacRelPath)||(thePath ==eUnixRelPath )||(thePath ==eDOSRelPath )){
		ret[0] = '.';
		ret[1] = platformSep;
		index2 = 2;
		if(itsMacPath)
			index = 1;
		else
			index = 2;
	}
	else{
		ret[0] = '.';
		ret[1] = '.';
		ret[2] = platformSep;
		index2 = 3;
		if(itsMacPath)
			index = 2;
		else
			index = 3;	
	}
	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif

	for(/*index set above*/;index < ioLen;index++){
		 	if( inChars[index] == separator )
				 	ret[index2++] =  platformSep;
			else
		 		ret[index2++] =  inChars[index];
	}
	if((ret[index2-1] != platformSep )&&  appendSeparator ){
		ret[index2++] = platformSep ;
	}
	ret[index2]='\0';
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif
	return ret;
}


bool 
oofPathString::isRelativePath(const char* inChars)
{
	const pathTypeT thePath = pathType(inChars);
	
	const bool ret = isRelativePath(thePath);
	return ret;
}


oofPathString::pathTypeT 
oofPathString::pathType(const char* inChars)
{
	if (!inChars)
		return eJustFile;

	const long inLen = ::strlen(inChars);
	if (inLen==0)
		return eJustFile;
		
	pathTypeT ret;
	const char a = inChars[0];
	const char b = inChars[1];
	const char c = (inLen>1) ? inChars[2] : '\0';	// make sure consistent value in 3rd char for 1 or 2 char names

	switch(a){
		case '/':
		case '~':
			ret = eUnixAbsPath;
			break;
						
		case '\\':
			ret = eDOSAbsPath;  // includes those starting with named server \\servername\something
			break;
			
		case ':' :
			if(b != ':')
				ret = eMacRelPath;
			else
				ret = eMacParentRelPath;
		break;

		case '.':
			switch(b){
				case '\\':
					ret = eDOSRelPath;
				break;
			
				case '/':
					ret = eUnixRelPath;
				break;
				
				case '.':
					switch(c){
						case '\\':
							ret = eDOSParentRelPath;
						break;
			
						case '/':
							ret = eUnixParentRelPath;
						break;
						
						default:
					 		ret = eUnixParentRelPath;  // or DOS can't tell!
						break;
					}
				break;
			
			
			default:
				if (b=='\0')  // single dot
					ret = eUnixParentRelPath;  // or DOS can't tell!
				else
					ret = eJustFile;  // starting with dot
				
			break;
			}//b
		break;
		
		default:  // starts with any other letters
			// try for DOS absolute - relaxed test assume starts any letter, rather than legal drive letter
			if ( (b==':') && (c=='\\') )   // eg: c:\something
				ret = eDOSAbsPath;
			else {  // Mac absolute or filename - does it contain any colons. We know starts with non-special char.
				if (::strchr(inChars, ':'))  // eg: My Disk:a folder:something
					ret = eMacAbsPath;
				else
					ret = eJustFile;  // which caller may assume to be any platform they like - it just lacks path!
			}
		break;

		}


	return ret;
}


ostream& 
operator<<(ostream& os, oofPathString::pathTypeT aPath)
{
	switch(aPath){
		case oofPathString::eMacAbsPath:
			os << "eMacAbsPath" ;
		break;
		
		case oofPathString::eMacRelPath:
			os << "eMacRelPath" ;
		break;
		
		case oofPathString::eMacParentRelPath:
			os << "eMacParentRelPath" ;
		break;
		
		case oofPathString::eDOSAbsPath:
			os << "eDOSAbsPath" ;
		break;
		
		case oofPathString::eDOSRelPath:
			os << "eDOSRelPath" ;
		break;
		
		case oofPathString::eUnixAbsPath:
			os << "eUnixAbsPath" ;
		break;
		
		case oofPathString::eUnixRelPath:
			os << "eUnixRelPath" ;
		break;
		
		case oofPathString::eDOSParentRelPath:
			os << "eDOSParentRelPath" ;
		break; 
		
		case oofPathString::eUnixParentRelPath:
			os << "eUnixParentRelPath" ;
		break;
		
		case oofPathString::eJustFile:
			os << "eJustFile" ;
		break;
	}
	return os;
}


// -------------------------------------------------------
//                o o f F i l e L i s t
// -------------------------------------------------------

oofFileList::oofFileList()
{
	mRefs = new OOF_ExpandableLongArray;
}


oofFileList::oofFileList(const oofFileList& rhs) 
{
	mRefs = new OOF_ExpandableLongArray;
	const unsigned short numFiles = rhs.count();
	for (unsigned short i=0;i<numFiles; i++) {
		const oofFileRef* rhsFile = (oofFileRef*)(rhs.mRefs->value(i));
		append(*rhsFile);
	}
}


oofFileList::~oofFileList()
{
	const unsigned short numFiles = count();
	for (unsigned short i=0;i<numFiles; i++) {
		oofFileRef* theFile = (oofFileRef*)(mRefs->value(i));
		delete theFile;
	}
	delete mRefs;
}

	
void 
oofFileList::append(const oofFileRef& inRef)
{
	assert(mRefs);
	oofFileRef* adoptedRef = new oofFileRef(inRef);
	mRefs->append( (long)adoptedRef);
}


unsigned short
oofFileList::count() const
{
	return mRefs->count();
}

oofFileRef 
oofFileList::ref(unsigned short fileIndex) const
{
	assert(mRefs);
	if (fileIndex < mRefs->count()) {
		const oofFileRef* theRef = (const oofFileRef*)(mRefs->value(fileIndex));
		return oofFileRef(*theRef);
	}
	else
		return oofFileRef();
		
}
	

// -------------------------------------------------------
//                o o f D i r e c t o r y
// -------------------------------------------------------

oofDirectory::oofDirectory() :
#ifdef _Macintosh
	mWorkingDirID(0),
#endif
	mValidDir(true),	// always assume our current location is valid (true for all platforms???)
	mCreatedDiskDir(false)
{
	GetCurrentLocation();
}

oofDirectory::~oofDirectory()
{
	#ifdef _Macintosh
		#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0
			closeWDDirectory(); 
		#endif
	#endif
}

oofDirectory::oofDirectory(const char* path, char* outFileName, bool createSubdirsIfMissing,bool makeAbsolute) :
#ifdef _Macintosh
	mWorkingDirID(0),
#endif
	mPath(path),
	mValidDir(true),	// set false by failures parsing path or creating dirs
	mCreatedDiskDir(false)
{
	if (mPath.isEmpty()) {
		GetCurrentLocation();
		if (outFileName)
			outFileName[0] = '\0';
	}
	else 
		setDirFromPath(path, outFileName, createSubdirsIfMissing, makeAbsolute);
}


void 
oofDirectory::delDirectory(bool delDirAsWellAsContents, bool delTree, bool delSubdirs)
{
	int maxDepth = 0;  // don't iterate into subdirs
	if (delTree)
		maxDepth = 1024;  // pick a number to
		
	oofDelDirTreeVisitor tempVisitor(delSubdirs);
	visitFiles(tempVisitor , maxDepth );
	if (delDirAsWellAsContents) {
#ifdef _Macintosh
		#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0
			closeWDDirectory();  // cannot delete current dir as have WD pointing at it
		#endif
// NOT YET IMPLEMENTED - central registration of WD's so can delete more flexibly
#endif
		oofFileRef thisDir(*this, "", oofFileRef::eDir);
		thisDir.deleteFile();
		mValidDir = false;  // because it no longer exists!
	}
}
		
		

#ifdef _Macintosh
oofDirectory::oofDirectory(short vRefNum, long dirID, bool searchForDirAllVols) :
	mVRefNum(vRefNum),
	mWorkingDirID(0),
	mDirID(dirID),
	mValidDir(true),
	mCreatedDiskDir(false)
{
	assert(vRefNum!=0 && dirID!=0);  // only create default dirs with default ctor, to catch errors in user code
	createDirID(0, searchForDirAllVols);	// also checks caller isn't lying to us and that vol/dir id's OK
	if (mValidDir) {
		FSSpec newSpec;
		newSpec.vRefNum = mVRefNum;
		newSpec.parID = mDirID;
		newSpec.name[0]=0;	// blank out name as it was directory, not file
		mPath = FSSpecToPath(newSpec);
		if (mPath.lastChar()!=':')
			mPath += ':';
	}
}


oofDirectory::oofDirectory(short vRefNum, long dirID, const char* fallBackPath, bool searchForDirAllVols) :
	mVRefNum(vRefNum),
	mWorkingDirID(0),
	mDirID(dirID),
	mValidDir(true),
	mCreatedDiskDir(false)
{
	assert(vRefNum!=0 && dirID!=0);  // only create default dirs with default ctor, to catch errors in user code
	CreateWorkingDirID(0, searchForDirAllVols);	// also checks caller isn't lying to us and that vol/dir id's OK
	if (mValidDir) {
		FSSpec newSpec;
		newSpec.vRefNum = mVRefNum;
		newSpec.parID = mDirID;
		newSpec.name[0]=0;	// blank out name as it was directory, not file
		mPath = FSSpecToPath(newSpec);
	}
	else {	// vRefNum and dirID are wrong, maybe out of date
		setDirFromPath(fallBackPath);
	}
}

#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0
	void oofDirectory::closeWDDirectory()
	{
		// NOT YET IMPLEMENTED - keep a list of the WD's we create, and delete them
		// will need to be ref-counted as a copy of this dir may be in use after
		// deletion of the original
		if (mWorkingDirID!=0) {
		// temp hack, don't close if current volume
		// this doesn't cover us if someone else caches this WD and tries to visit again
			short curVolRef;
			OSErr getErr = ::GetVol(0,&curVolRef);
			
			
			if (curVolRef!=mWorkingDirID) {	
				OSErr closeErr = ::CloseWD( mWorkingDirID); 
				mWorkingDirID = 0;
			}
		}
	}

#endif //TARGET_API_MAC_CARBON


oofDirectory::oofDirectory(const FSSpec& inSpec) :  // incoming spec may be a file spec
	mVRefNum(inSpec.vRefNum),
	mWorkingDirID(0),
	mDirID(inSpec.parID),
	mValidDir(false),
	mCreatedDiskDir(false)
{
	assert(inSpec.vRefNum!=0 && inSpec.parID!=0);  // only create default dirs with default ctor, to catch errors in user code
	const unsigned short nameLen =  inSpec.name[0];
	FSSpec newSpec = inSpec;
	if (nameLen>0) {
		if ((newSpec.name[1]!=':') && (newSpec.name[nameLen]!=':')) {  // is not subfolder name so make it one
			newSpec.name[0]+=2;
			::BlockMoveData(&newSpec.name[1], &newSpec.name[2], nameLen);
			newSpec.name[1]=':'; // insert leading colon 
			newSpec.name[nameLen+2]=':'; // append colon 
		}
		createDirID(newSpec.name);	// name might be directory, not file, so try it
		if (mValidDir)	{// was a directory, or empty name, so change our mDirID
	#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0
			//Given the workingDirectoryID get the "Hard DirID and real volNumber dg0911
			OSErr err = ::GetWDInfo(mWorkingDirID, &mVRefNum, &mDirID, 0); // carbonised code will set these members
	#endif	
			newSpec.vRefNum = mVRefNum; 
			newSpec.parID = mDirID;
			newSpec.name[0]=0;	// blank out name as it was directory, not file
			mPath = FSSpecToPath(newSpec);
		}  // create WD worked
	}  // passed in name in spec
	if (!mValidDir) {  // not directory name, so dir is that containing file, or no name
		createDirID();	// try without filename
		newSpec.name[0] = 0;  // blank name
		mPath = FSSpecToPath(newSpec);
	}
	if (mPath.lastChar()!=':')
		mPath += ':';
}


/**
setDirFromPath
 core conversion routine for external call or constructing oofDirectory from incoming path

 a non-null outFileName indicates the caller expects the path to
 terminate in a filename

 incoming paths converted if possible
*/
void	
oofDirectory::setDirFromPath(const char* path, char* outFileName, bool createSubdirsIfMissing, bool /*makeAbsolute*/) // _Macintosh
{
	oofPathString pathStr(path);
	const oofPathString::pathTypeT pathType = pathStr.pathType();
	if (oofPathString::isRelativePath(pathType))
		pathStr.relativePathPlatform(outFileName==NULL);  // appends separator if not expecting filename on end
		
	const unsigned int pathLen = pathStr.length();
	path = pathStr.chars();  // will be same still if not needed to convert
		
	assert(pathLen<=255);	// current Mac limit
	unsigned int macPathLen = pathLen;
	unsigned char* macPath=0;	// will be Pascal string
	
	if (oofPathString::isMacPath(pathType)) {
		if(outFileName) {
			unsigned int fileStartsAt = pathLen;
			for(;(fileStartsAt>0) && (path[fileStartsAt-1]!=':'); fileStartsAt--)	// measure filename at end
				;	
			if (fileStartsAt<pathLen) 
				::strcpy(outFileName,&path[fileStartsAt]);	// got a filename
			else 
				outFileName[0]='\0';	// send back a null string indicating no file at end of path
			macPathLen = fileStartsAt;		
		}
		macPath = new unsigned char[macPathLen+1];
		macPath[0] = macPathLen;
		::memcpy(&macPath[1], path, macPathLen);	// finish Pascal string
	} else {
		// We don't have a path
		// if not asked to return filename assume a single name is a subdirectory of our current location	
		
		if(outFileName){
			::memcpy(outFileName, path, pathLen);	// return entire path as filename
			outFileName[pathLen]='\0';
			macPath = new unsigned char[1];	// empty Pascal string
			macPath[0] = 0;
		}
		else {
			if (pathLen) {	// copy entire path to macPath, with leading colon to pretent to be relative path
				int extraCharLead = 1;
				if (path[0]==':')
					extraCharLead = 0;
				macPath = new unsigned char[macPathLen+extraCharLead + 1];
				macPath[0] = macPathLen+extraCharLead;
				if(extraCharLead)
					macPath[1] = ':';
				::memcpy(&macPath[extraCharLead+1], path, macPathLen);	// finish Pascal string
			}
		}	// filename or subdir
	}	// at least one embedded colon
	
	if (createSubdirsIfMissing)
		CreateSubDirs(macPath);
	else {
	PathToDirID(macPath);
// AD 981218 removed - it is LEGAL to create an invalid dir, so user can test
//			if (!mValidDir) 
//				RAISE_EXCEPTION( oofE_General(oofString("'", path,"' is not a valid directory")) );			
	}
	delete [] macPath;
}
	

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	/**
		Sets the oofDirectory to point to the directory identified by the path
		alters the members:
		 mDirID 
		 possibly the mVRefNum if path is absolute
	*/
	void
	oofDirectory::PathToDirID(const unsigned char* pascalPath) 
	{
		GetCurrentLocation();  // changes mPath to current path
		if (pascalPath && pascalPath[0])  { // if any subpath,  otherwise we're here
			createDirID(pascalPath);	// for new path-based location
			if (mValidDir) {	// was a directory, so change our mDirID
				if (pascalPath[1]==':') // was relative path
					mPath += oofString(pascalPath);  // 
				else
					mPath = pascalPath;
			}
		}
	}

#else	
	/**
		Sets the oofDirectory to point to the directory identified by the path
		alters the members:
		 mDirID 
		 possibly the mVRefNum if path is absolute
		 Uses working directories, updating mWorkingDirID.
	*/
	void
	oofDirectory::PathToDirID(const unsigned char* pascalPath) 
	{
		GetCurrentLocation();  // changes mPath to current path
		if (pascalPath && pascalPath[0])  { // if any subpath,  otherwise we're here
			createDirID(pascalPath);	// for new path-based location
			if (mValidDir) {	// was a directory, so change our mDirID
				OSErr err = ::GetWDInfo(mWorkingDirID, &mVRefNum, &mDirID, 0);  // in case partial path moved us
				if (pascalPath[1]==':') // was relative path
					mPath += oofString(pascalPath);  // 
				else
					mPath = pascalPath;
			}
		}
		else {	
			if (mWorkingDirID==0)
				createDirID();	// probably got here from FSSpec
			else
				mValidDir = true;
		}
	}

#endif //TARGET_API_MAC_CARBON

/**
	A simple wrapper to call the approriate directory method depending on target enviroment.
*/
void
oofDirectory::createDirID(const unsigned char* pascalPartialPath, bool searchForDirAllVols)
{
	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
		CreateHardDirID( pascalPartialPath,searchForDirAllVols);
	#else
		CreateWorkingDirID( pascalPartialPath,searchForDirAllVols);
	#endif

}


/**
	Search from current location for the directory identified by the partial path.
	and set:
		 mDirID &
		 possibly the mVRefNum if the dir is found
	This will result in oofDirectory pointing to directory identified partial payh
	If the path has more than one leading ":" recurse up the directry tree and then start then search 
	else search down
	if directory not found and searchForDirAllVols is true search other volumes
	If not found mValidDir will be false
	Named CreateHardDirID to match the original CreateWorkingDirID. Does not create a directory
	just gets the hard dirID of the directory identified by the pascalPartialPath.
	\see createDirID
*/

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
	void
	oofDirectory::CreateHardDirID(const unsigned char* pascalPartialPath, bool searchForDirAllVols)
	{
		// unlike working dir stuff not really a create but more of a get
		//  
		mValidDir = false;  // default assumes creation fails
		Str255 theFolder;
		char theScrap[256];
		char dirName[32];
		//create a temp cstring from a pascal string. 
		if(pascalPartialPath){
			memcpy(theScrap,(const char *)(pascalPartialPath+1),pascalPartialPath[0]);
			theScrap[pascalPartialPath[0]] = '\0';
		}
		else
			theScrap[0] = '\0';
		//cpb.ioNamePtr=theScrap;
		
		DirInfo cpb;
	   int theCount = 0;
		char *delims = ":";
		char * token;
		cpb.ioNamePtr=0;
	
		//Analyse partial path to see if it is a relative path
		// IF path is relative from a parent dir eg. ":::tree:house:" THEN
		//		count number of directories we have to move up from where we currently are 
		//		and move up
		//	ELSE
		//		we make path down from current loc
		//	End_IF
		
		char * pos = theScrap;
		// count the :
		if(*pos++ == ':') {//is it relative
			if(*pos == ':'){//is it  a parent
				//go up, so count how far up
				theCount = 1;
				pos ++;
				while(*pos++ == ':') {
					//pos++; david debug 001126
					theCount++;
				}
				
			
				// ok using count crawl up the tree
				cpb.ioVRefNum = mVRefNum;
				
				cpb.ioDrDirID = mDirID ;
				cpb.ioFDirIndex=-1;
			
				cpb.ioNamePtr = theFolder;
				while(theCount-- != 0){
					PBGetCatInfoSync((CInfoPBRec *)&cpb); // must check error
					cpb.ioDrDirID = cpb.ioDrParID;
				}
			//ok we now have hard dirID of top most parent  
				
			
				mDirID = cpb.ioDrParID;
			}  // is parent
		} // is relative
		
		// parse partial path and get each directory name and  recurse down the directory
		// tree and until we get to the final directry and obtain it's dirID 
		token = strtok( pos, delims );
		OSErr	error = 0;
		while ( token != NULL && !error ) { // maybe should check if this is a file
			strcpy(dirName + 1,token);
			*dirName = strlen(token); //make it into a pascal string
			cpb.ioNamePtr = (unsigned char*)dirName;
			cpb.ioDrDirID = mDirID ;
			cpb.ioFDirIndex=-1;
			error = PBGetCatInfoSync((CInfoPBRec *)&cpb); // must check error
			mDirID = cpb.ioDrDirID;
			token = strtok( NULL, delims );
		}
			
			
		if(error == 0)
			mValidDir = true;
		else {
			//could not find the directory so look on other volumes
			if (searchForDirAllVols && mDirID!=0 && mDirID!=2) {  // search for it and not root or default dir
				for (short vIndex = 1; ; vIndex++)	{  // loop all volumes except the one we just tried
					HParamBlockRec	HPBlock;
					
					HPBlock.volumeParam.ioNamePtr=0;
					HPBlock.volumeParam.ioVRefNum=0;
					HPBlock.volumeParam.ioVolIndex=vIndex;

					const OSErr volErr = ::PBHGetVInfoSync(&HPBlock);
					if (volErr==nsvErr)
						break;	// end of volumes
						
					assert(volErr==noErr);
					const short vRef = HPBlock.volumeParam.ioVRefNum;
					if (vRef==mVRefNum)
						continue;  // skip the one we tried earlier
						
					// now we have a volume, see if this directory exists	
					CInfoPBRec	cipbr;  // parameter block, union of dir and file info
					cipbr.hFileInfo.ioVRefNum = vRef;
					cipbr.hFileInfo.ioDirID = mDirID;
					cipbr.hFileInfo.ioNamePtr = 0; 
					cipbr.hFileInfo.ioFDirIndex = -1;  // dir info, ignore ioNamePtr
					const OSErr dirInfoErr = ::PBGetCatInfoSync(&cipbr);

					if (dirInfoErr==noErr) {  // found our directory!
						mVRefNum = vRef;  // correct our current vol!!!
						// make sure we are pointing to it
						if (!HSetVol(pascalPartialPath,mVRefNum,mDirID)) {
							mValidDir = true;
						}
					}  // found dir
				}  // volume loop
			}  // optional try all vols
		}  // found match for specified vol/dir
	}
# else
	/**
	Search for the directory from current location for the directory identified by the partial path
	and set:
		 mWorkingDirID 
		mValidDir
		mVRefNum if the dir is found
	\todo	check if PBOpenWDSync can handle paths like :::sea:saw:
		
	*/	
	void
	oofDirectory::CreateWorkingDirID(const unsigned char* pascalPartialPath, bool searchForDirAllVols)
	{
		mValidDir = false;  // default assumes creation fails
		
		WDPBRec	wdpb;
		
		wdpb.ioNamePtr=const_cast<unsigned char*>(pascalPartialPath);  // Apple's non-const interfaces cause warning here but read-only param
		wdpb.ioWDDirID=mDirID;
		wdpb.ioVRefNum=mVRefNum;
		wdpb.ioWDProcID=0;
		
		const OSErr err = ::PBOpenWDSync( &wdpb );
		if (err==noErr) {
			mWorkingDirID = wdpb.ioVRefNum;
			mValidDir = true;
		}
		else {
			if (searchForDirAllVols && mDirID!=0 && mDirID!=2) {  // search for it and not root or default dir
				for (short vIndex = 1; ; vIndex++)	{  // loop all volumes except the one we just tried
					HParamBlockRec	HPBlock;
					
					HPBlock.volumeParam.ioNamePtr=0;
					HPBlock.volumeParam.ioVRefNum=0;
					HPBlock.volumeParam.ioVolIndex=vIndex;

					const OSErr volErr = ::PBHGetVInfoSync(&HPBlock);
					if (volErr==nsvErr)
						break;	// end of volumes
						
					assert(volErr==noErr);
					const short vRef = HPBlock.volumeParam.ioVRefNum;
					if (vRef==mVRefNum)
						continue;  // skip the one we tried earlier
						
					// now we have a volume, see if this directory exists	
					CInfoPBRec	cipbr;  // parameter block, union of dir and file info
					cipbr.hFileInfo.ioVRefNum = vRef;
					cipbr.hFileInfo.ioDirID = mDirID;
					cipbr.hFileInfo.ioNamePtr = 0; 
					cipbr.hFileInfo.ioFDirIndex = -1;  // dir info, ignore ioNamePtr
					const OSErr dirInfoErr = ::PBGetCatInfoSync(&cipbr);

					if (dirInfoErr==noErr) {  // found our directory!
						mVRefNum = vRef;  // correct our current vol!!!
						wdpb.ioNamePtr=0;
						wdpb.ioWDDirID=mDirID;  // just to be sure, some calls clear
						wdpb.ioVRefNum=vRef;
						wdpb.ioWDProcID=0;
						
						const OSErr err2 = ::PBOpenWDSync( &wdpb );
						if (err2==noErr) {
							mWorkingDirID = wdpb.ioVRefNum;
							mValidDir = true;
						}
					}  // found dir
				}  // volume loop
			}  // optional try all vols
		}  // found match for specified vol/dir
	}
# endif
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0

/** 
	Set members to point to pascalPartialPath, create it and any parents.
	Find the directory identified by pascalPartialPath
	and make the oofDirectory point to the directory.
	If the directory does not exist create it and any other missing directories.
	Will modify members:
		mVRefNum
		mDirID
	will return mValidDir = true if sucseeful else false
	Algorithm is as follows.	
	IF we have a partial path 
		IF path is relative THEN
			GET curVol and SET mVRefNum 
			GET  cur dirID and SET temp variable   theDirID
		
		ELSE
			IF path is absolute AND vol name in path exists THEN
				SET mVRefNum = volnum of vol with name at beginning of pascalPartialPath
			ELSE
				SET mValidDir = false
			END_IF		
				make 
		END_IF
		IF volume exists THEN
			GET catinfo on named object
			IF named object exists	THEN	
				GET catinfo on named object
				IF named object is dir
					SET mDirID  to cpb.ioDrDirID
				ELSE
					SET mValidDir  false	it's a file or something
					return 
				END_IF
			ELSE	// must make a directory
				loop back through path name until an existing folder name is found or in current dir
				loop through path name from valid point creating missing nested directories 
				SET mDirID to hpb.ioDirID
		ELSE
				SET mValidDir = false
		END_IF
		IF mValidDir THEN
			convert current dir to path
		END_IF
	ELSE
		CALL	GETCurrentLocation
	ENDIF	
	
	\todo clean up implementation	
*/
void	
oofDirectory::CreateSubDirs(const unsigned char* pascalPartialPath)
{
// NOT YET IMPLEMENTED - clean this up!
// is a copy of Ken's original, which needs rewriting to use the pascal path
// and we already have the vol refnum etc. for a relative path!

// NOTE other changes internally to avoid just crashing in some circumstances
// and just set the mValidDir flag instead

	mValidDir = true;  // default OK
	
	// hack, hack, hack - convert pascal path back into C string
	// to save time rewriting the following
	if (pascalPartialPath && pascalPartialPath[0])  { // if any subpath,  otherwise we're here
		mCreatedDiskDir = true;
		
		long pathLen = pascalPartialPath[0];
		char* path = new char[pathLen+1];
		memcpy(path, &pascalPartialPath[1], pathLen);
		path[pathLen]='\0'; 
		OSErr iErr;
		
		long theVRefNum;
		long theDirID;
		unsigned int i=0;
		bool relative;
		
		// OK - Let's get started
		if(path[0]==':') { // We have a relative path
			// Extract the default directory information
			relative=true;
			iErr = HGetVol((unsigned char*) 0,&mVRefNum,&theDirID); //as we will be relative from current default
			assert (iErr != -50);// default vol no 
			
			// but what would happen if we realtived to another VOL CAN WE DO THAT ? 
			// WILL NOT 
		} 
		else {  // looks like absolute path
			// Extract the Volume Information to make sure the volume exists
			HVolumeParam vpb;
			Str255 theVol;
			
			for(;((path[i]!=':')&&(path[i]!='\0'));i++){
				theVol[i+1]=path[i];
			}
			theVol[0]=i+1;
			theVol[i+1]=':';
			vpb.ioNamePtr=theVol;
			vpb.ioVolIndex=-1;
			//getinfo about volume  specified by name and make mVRefNum point to thatvolume
			iErr = PBHGetVInfoSync((HParamBlockRec *)&vpb);
			mVRefNum=vpb.ioVRefNum;
			relative=false;
		}
		
		if(!iErr){
			// OK, we now have a valid VRefNum, see if the folder exists
			DirInfo cpb;
			Str255 theFolder;
			
			strcpy((char *)&theFolder[1],path);
			theFolder[0]=strlen(path);
			cpb.ioNamePtr=theFolder;
			if(relative) {
				cpb.ioDrDirID=theDirID;
				cpb.ioVRefNum=0;
			} else
				cpb.ioVRefNum=mVRefNum;
			cpb.ioFDirIndex=0;
			iErr = PBGetCatInfoSync((CInfoPBRec *)&cpb);
			if(!iErr) {
				if(cpb.ioFlAttrib&0x0010) {
				// The Directory Exists
				
				// OK - lets set the location  and fill in fields!
					mDirID = cpb.ioDrDirID;
				} 
				else {
					mValidDir = false;
					return;
				}
			} // existing directory
			else {
				// OK, we have a valid volume, but the folder seems bogus
				//     Let's create it !
				// First, we'll keep going back until we find a valid folder
				// Then we'll create the folders from there
				unsigned int OKToStart=false;
				
				while(!OKToStart) {
					for(i=theFolder[0]-1;(theFolder[i]!=':')&&i;i--)
						;
					theFolder[0]=i;  // Ken's cute hack of maintaining the entire path null-terminated 
									 // but playing with the Pascal string length byte to make it shorter
					cpb.ioNamePtr=theFolder;
					if(relative) {
						cpb.ioDrDirID=theDirID;
						cpb.ioVRefNum=0;
					} 
					else
						cpb.ioVRefNum=mVRefNum;
					cpb.ioFDirIndex=0;
					iErr = PBGetCatInfoSync((CInfoPBRec *)&cpb);
					if(!iErr) {
						if(cpb.ioFlAttrib&0x0010)
							OKToStart=true;
						else {
			 				mValidDir = false;
							return;
						}
					}
				}  // loop back trying shorter & shorter paths to valid folder
				// Found a valid folder, we'll build from here
				OKToStart=false;
				HFileParam hpb;
				while(!OKToStart){
					
					for(i=theFolder[0]+1;(theFolder[i]!=':')&&(theFolder[i]!='\0');i++)
						;
					if(theFolder[i]=='\0'){
						OKToStart=true;	// only get here if last dir in path did NOT end in colon
						i--;
					}
					else {
						if(theFolder[i+1]=='\0')	// last dir in path ends with a colon
							OKToStart=true;
					}
					theFolder[0]=i;
					hpb.ioNamePtr=theFolder;
					if(relative) {
						hpb.ioDirID=theDirID;
						hpb.ioVRefNum=0;
					} 
					else
						hpb.ioVRefNum=mVRefNum;
					hpb.ioFDirIndex=0;
					iErr = ::PBDirCreateSync((HParamBlockRec *)&hpb);
					if(iErr){
						oofString errMsg = "during folder creation" + oofString(path);  // convert Pascal path
						RAISE_EXCEPTION( oofE_MacOSErr(iErr, errMsg) );					
					}
				}
					mDirID = hpb.ioDirID;
			}
		} 
		else
			mValidDir = false;


		if (mValidDir) {
			// Convert current directory to path
			FSSpec fileSpec;
			OSErr makeSpecErr = ::FSMakeFSSpec(mVRefNum, mDirID, nil, &fileSpec);	// toolbox call
			assert(makeSpecErr==noErr);	
			mPath = FSSpecToPath(fileSpec);
			if (mPath.lastChar()!=':')
				mPath += ':';
		}  // catch-all fix the path when dir is valid
		delete [] path;
	}  // given partial path
	else {	
		GetCurrentLocation();
	}
}


#else
	
	
	/**
		a simple wrapper to allow us to call CreateSubdirsAndWorkingDirID
		Will modify members:
			mWorkingDirID
			mVRefNum
			
		Will set mValidDir = true if sucseeful else false
		mWorkingDirID, &mVRefNum, &mDirID
	*/
	void
	oofDirectory::CreateSubDirs(const unsigned char* pascalPartialPath)
	{
		CreateSubdirsAndWorkingDirID(pascalPartialPath);

	}
	
	
	
	/** 
		Find the directory identified by pascalPartialPath
		and make the oofDirectory point to the directory.
		If the directory does not exist create it and any other missing directories.
		Will modify members:
			mWorkingDirID
			mVRefNum
			
		Will set mValidDir = true if sucseeful else false
		mWorkingDirID, &mVRefNum, &mDirID
		
		Algorithm is as follows.	
		IF we have a partial path 
			IF path is relative THEN
				GET default working directory ID 
			ELSE
				IF path is absolute AND vol name in path exists THEN
					SET mVRefNum = volnum of vol with name at beginning of pascalPartialPath
				ELSE
					SET mValidDir = false
				END_IF		
					
			END_IF
			IF volume exists THEN
				GET catinfo on named object
				IF named object exists	THEN	
					GET catinfo on named object
					IF named object is dir
						create a working directory and SET mWorkingDirID
					ELSE
						SET mValidDir  false	it's a file or something
						return 
					END_IF
				ELSE	// must make a directory
					loop back through path name until an existing folder name is found or in current dir
					loop through path name from valid point creating missing nested directories 
					PBOpenWD 
					SET mWorkingDirID to  wpb.ioVRefNum (jus open wdirectory)
			ELSE
					SET mValidDir = false
			END_IF
			IF mValidDir THEN
				convert current dir to path
			END_IF
		ELSE
			CALL	GETCurrentLocation
		ENDIF	
		
		
	*/
	void
	oofDirectory::CreateSubdirsAndWorkingDirID(const unsigned char* pascalPartialPath)
	{
	// NOT YET IMPLEMENTED - clean this up!
	// is a copy of Ken's original, which needs rewriting to use the pascal path
	// and we already have the vol refnum etc. for a relative path!

	// NOTE other changes internally to avoid just crashing in some circumstances
	// and just set the mValidDir flag instead

		mValidDir = true;  // default OK
		
		// hack, hack, hack - convert pascal path back into C string
		// to save time rewriting the following
		if (pascalPartialPath && pascalPartialPath[0])  { // if any subpath,  otherwise we're here
			mCreatedDiskDir = true;
			
			long pathLen = pascalPartialPath[0];
			char* path = new char[pathLen+1];
			memcpy(path, &pascalPartialPath[1], pathLen);
			path[pathLen]='\0'; 
			OSErr iErr;
			
			long theVRefNum;
			unsigned int i=0;
			bool relative;
			
			// OK - Let's get started
			if(path[0]==':') { // We have a relative path
				// Extract the default directory information
				HFileInfo rcpb;
		
				rcpb.ioNamePtr=0;
				rcpb.ioVRefNum=0;
				rcpb.ioDirID=0;
				rcpb.ioFDirIndex=-1;
				iErr = PBGetCatInfoSync((CInfoPBRec *)&rcpb);
				theVRefNum=rcpb.ioDirID;
				relative=true;
			} 
			else {  // looks like absolute path
				// Extract the Volume Information to make sure the volume exists
				VolumeParam vpb;
				Str255 theVol;
				
				for(;((path[i]!=':')&&(path[i]!='\0'));i++){
					theVol[i+1]=path[i];
				}
				theVol[0]=i+1;
				theVol[i+1]=':';
				vpb.ioNamePtr=theVol;
				vpb.ioVolIndex=-1;
				iErr = PBGetVInfoSync((ParamBlockRec *)&vpb);
				theVRefNum=vpb.ioVRefNum;
				relative=false;
			}
			
			if(!iErr){
				// OK, we now have a valid VRefNum, see if the folder exists
				DirInfo cpb;
				Str255 theFolder;
				
				strcpy((char *)&theFolder[1],path);
				theFolder[0]=strlen(path);
				cpb.ioNamePtr=theFolder;
				if(relative) {
					cpb.ioDrDirID=theVRefNum;
					cpb.ioVRefNum=0;
				} else
					cpb.ioVRefNum=theVRefNum;
				cpb.ioFDirIndex=0;
				iErr = PBGetCatInfoSync((CInfoPBRec *)&cpb);
				if(!iErr) {
				  if(cpb.ioFlAttrib&0x0010) {
					// The Directory Exists
					
					// OK - lets set the location !
					WDPBRec wpb;
					
					wpb.ioNamePtr=0;
					wpb.ioVRefNum=cpb.ioVRefNum;
					wpb.ioWDDirID=cpb.ioDrDirID;
					iErr = PBOpenWDSync(&wpb);
		// FINALLY GOT WD, SO CAN FILL IN OUR FIELDS
					assert(iErr==noErr);
					mWorkingDirID = wpb.ioVRefNum;
					iErr = ::GetWDInfo(mWorkingDirID, &mVRefNum, &mDirID, 0);
				  } 
				  else {
		 		 	mValidDir = false;
					return;
				  }
				} // existing directory
				else {
					// OK, we have a valid volume, but the folder seems bogus
					//     Let's create it !
					// First, we'll keep going back until we find a valid folder
					// Then we'll create the folders from there
					unsigned int OKToStart=false;
					
					while(!OKToStart) {
						for(i=theFolder[0]-1;(theFolder[i]!=':')&&i;i--)
							;
						theFolder[0]=i;  // Ken's cute hack of maintaining the entire path null-terminated 
										 // but playing with the Pascal string length byte to make it shorter
						cpb.ioNamePtr=theFolder;
						if(relative) {
							cpb.ioDrDirID=theVRefNum;
							cpb.ioVRefNum=0;
						} else
							cpb.ioVRefNum=theVRefNum;
						cpb.ioFDirIndex=0;
						iErr = PBGetCatInfoSync((CInfoPBRec *)&cpb);
						if(!iErr)
						  if(cpb.ioFlAttrib&0x0010)
						  	OKToStart=true;
						  else {
				 		 	mValidDir = false;
							return;
						  }
					}  // loop back trying shorter & shorter paths to valid folder
					// Found a valid folder, we'll build from here
					OKToStart=false;
					HFileParam hpb;
					while(!OKToStart){
						
						for(i=theFolder[0]+1;(theFolder[i]!=':')&&(theFolder[i]!='\0');i++)
							;
						if(theFolder[i]=='\0'){
							OKToStart=true;	// only get here if last dir in path did NOT end in colon
							i--;
						}
						else
							if(theFolder[i+1]=='\0')	// last dir in path ends with a colon
								OKToStart=true;
						theFolder[0]=i;
						hpb.ioNamePtr=theFolder;
						if(relative) {
							hpb.ioDirID=theVRefNum;
							hpb.ioVRefNum=0;
						} else
							hpb.ioVRefNum=theVRefNum;
						hpb.ioFDirIndex=0;
						iErr = ::PBDirCreateSync((HParamBlockRec *)&hpb);
						if(iErr){
							oofString errMsg = "during folder creation" + oofString(path);  // convert Pascal path
							RAISE_EXCEPTION( oofE_MacOSErr(iErr, errMsg) );					
						}
					}
					// Now The Directory Exists
					
					// OK - lets set the location !
					WDPBRec wpb;
					
					if(relative){
						wpb.ioNamePtr=theFolder;
						wpb.ioWDDirID=theVRefNum;
						wpb.ioVRefNum=0;
					} 
					else {
						wpb.ioNamePtr=theFolder;
						wpb.ioVRefNum=theVRefNum;
					}
					iErr = ::PBOpenWDSync(&wpb);

		// FINALLY GOT WD, SO CAN FILL IN OUR FIELDS
					mWorkingDirID = wpb.ioVRefNum;
					iErr = ::GetWDInfo(mWorkingDirID, &mVRefNum, &mDirID, 0);
				}
			} 
			else
				mValidDir = false;


			if (mValidDir) {
				// Convert current directory to path
				FSSpec fileSpec;
				OSErr makeSpecErr = ::FSMakeFSSpec(mVRefNum, mDirID, nil, &fileSpec);	// toolbox call
				assert(makeSpecErr==noErr);	
				mPath = FSSpecToPath(fileSpec);
				if (mPath.lastChar()!=':')
					mPath += ':';
			}  // catch-all fix the path when dir is valid
			delete [] path; 
		}  // given partial path
		else {	
			GetCurrentLocation();
			if (mWorkingDirID==0)
				createDirID();	// probably got here from FSSpec
			else
				mValidDir = false;
		
		}
	}
#endif //TARGET_API_MAC_CARBON


#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
	
	/**
		Set this oofDirectory to point to current default location.
		By getting Current default volume and directory, then setting members:
		 mVRefNum 
		 mDirID
		 mPath
	*/
	void
	oofDirectory::GetCurrentLocation()  // _Macintosh
	{
		OSErr getErr = HGetVol((unsigned char*) 0,&mVRefNum,&mDirID); //as we will be relative from current default
		// Convert current directory to path
		FSSpec fileSpec;
		OSErr makeSpecErr = ::FSMakeFSSpec(mVRefNum, mDirID, nil, &fileSpec);	// toolbox call
		assert(makeSpecErr==noErr);	
		mPath = FSSpecToPath(fileSpec);
		if (mPath.lastChar()!=':')
			mPath += ':';
	}

#else
	/**
		Set this oofDirectory to point to current default location.
			By getting Current default volume and directory, then setting members:
		 mVRefNum 
		 mDirID
		 mWorkingDirID
		 mPath
	*/
	
	
	void
	oofDirectory::GetCurrentLocation()  // _Macintosh
	{
		WDPBRec pb;
		pb.ioNamePtr = 0;
		::PBHGetVolSync(&pb);  // don't check error, ioResult field contains error value
		assert(pb.ioResult==noErr);	// asserts that current volume exists
		mDirID = pb.ioWDDirID;
		if (pb.ioVRefNum==pb.ioWDVRefNum) {  // not a Working Directory
			mVRefNum = pb.ioVRefNum;
			mWorkingDirID = 0;
		}
		else {	// translate Working Directory into real directory and volume
			mVRefNum = pb.ioWDVRefNum;
			mWorkingDirID = pb.ioVRefNum;
	// don't need call, info is available in the parameter block above
	//		error = ::GetWDInfo(pb.ioVRefNum, &mVRefNum, &mDirID, 0);
	
		}
		
		// Convert current directory to path
		FSSpec fileSpec;
		OSErr makeSpecErr = ::FSMakeFSSpec(mVRefNum, mDirID, nil, &fileSpec);	// toolbox call
		assert(makeSpecErr==noErr);	
		mPath = FSSpecToPath(fileSpec);
		if (mPath.lastChar()!=':')
			mPath += ':';
	}
#endif

/**
	Make the  directory identified by 	
	inPath
	the current default.	

*/

void 
oofDirectory::gotoDirectory(const oofString& inPath)  // _Macintosh
{
	// horrible quick implementation
	if (!inPath.isEmpty())  {
		oofDirectory tempDir(inPath);
		tempDir.gotoDirectory();  // will throw oofE_InvalidDir if invalid
	}
}




/**
	Make the  directory identified by	
	vRefNum & dirID
	the current default.
*/
void
oofDirectory::gotoDirectory(short vRefNum, long dirID) //  _Macintosh
{
	const OSErr err = ::HSetVol(NULL, vRefNum, dirID);
	if (err!=noErr) {
		oofDirectory tempDir(vRefNum, dirID);
		RAISE_EXCEPTION( oofE_InvalidDir(tempDir, "oofDirectory::gotoDirectory(vRefNum, dirID)") );
	}
}


/**
	Change current dir to this oofDirectory.
	directory identified by
		mWorkingDirID	(classic)
		or
		mVRefNum & mDirID	(classic or carbon)
*/
void 
oofDirectory::gotoDirectory() const  //  _Macintosh
{
//	assert(mValidDir);  nice idea but causes immediate OOFILE crash!
	OSErr setErr;
#if !defined(TARGET_API_MAC_CARBON) || TARGET_API_MAC_CARBON==0	
	if (mWorkingDirID) {	// have one if opened from path
		setErr = ::SetVol(NULL, mWorkingDirID);	
		if (setErr==nsvErr) {  // no such volume - most likely our WD is out of date
			setErr = ::HSetVol(NULL, mVRefNum, mDirID);
		}
	}
	else
#endif //TARGET_API_MAC_CARBON
		setErr = ::HSetVol(NULL, mVRefNum, mDirID);
			
	if (setErr!=noErr) 
		RAISE_EXCEPTION( oofE_InvalidDir(*this, "oofDirectory::gotoDirectory") );
}
	


/**
	Fill in an FSSpec from our current member variables.
	\todo investigate use of mac api call FSMakeFSSpec

*/
void 
oofDirectory::fillInFSSpec(const char* filename, FSSpec& ioSpec) const  // _Macintosh
{
	ioSpec.vRefNum = mVRefNum;
	ioSpec.parID = mDirID;
	if (filename) {
		unsigned short filenameLen = strlen(filename);
		assert(filenameLen<=63);	// hardcoded name array in FSSpec
		ioSpec.name[0] = filenameLen;
		memcpy(&ioSpec.name[1], filename, filenameLen);
	}
	else
		ioSpec.name[0] = 0;
}


bool
oofDirectory::openFile(const char* justFileName) const  // _Macintosh
{
	bool ret = false;
	FSSpec tempSpec;

	if (mValidDir) {
		fillInFSSpec(justFileName, tempSpec);
		OSErr err = ::OpenSelection(&tempSpec);
		ret = (err == noErr);
	}

	return ret;
}


OOF_String 
oofDirectory::FSSpecToPath(const FSSpec& inSpec)
{
// From the MoreFiles 1.4.2 file FullPath.c, FSpGetFullPath()

	OSErr		result;
	FSSpec		tempSpec;
	CInfoPBRec	pb;
	
	/* Make a copy of the input FSSpec that can be modified */
	BlockMoveData(&inSpec, &tempSpec, sizeof(FSSpec));
	
	if ( tempSpec.parID == fsRtParID )  
	{
		/* The object is a volume */
		
		/* Add a colon to make it a full pathname */
		++tempSpec.name[0];
		tempSpec.name[tempSpec.name[0]] = ':';
		
		/* We're done */
		return oofString((const char*)&tempSpec.name[1], tempSpec.name[0]);
	}

	/* The object isn't a volume */
	
	/* Put the object name in first */
	oofString ret((const char*)&tempSpec.name[1], tempSpec.name[0]);
	
	/* Get the ancestor directory names */
	pb.dirInfo.ioNamePtr = tempSpec.name;
	pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
	pb.dirInfo.ioDrParID = tempSpec.parID;
	do	/* loop until we have an error or find the root directory */
	{
		pb.dirInfo.ioFDirIndex = -1;
		pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
		result = PBGetCatInfoSync(&pb);
		if ( result == noErr )
		{
			/* Append colon to directory name */
			++tempSpec.name[0];
			tempSpec.name[tempSpec.name[0]] = ':';
			
			/* Add directory name to beginning of fullPath */
			ret.insertChars((const char*)&tempSpec.name[1], tempSpec.name[0], 0);
		}
	} while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
	if ( result != noErr )
		ret = "";
	return ret;
}


oofFileList*
oofDirectory::makeFileList()  const  // _Macintosh
{
	oofFileList* theList = new oofFileList;  // make on heap to avoid huge copy
	FSSpec theSpec;
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	
	cipbr.hFileInfo.ioVRefNum =  theSpec.vRefNum = mVRefNum;	
	theSpec.parID = mDirID;
	cipbr.hFileInfo.ioNamePtr =  theSpec.name;	// buffer to receive name
	cipbr.hFileInfo.ioCompletion = 0;
	for (short fileIndex=1;;fileIndex++) {
	// reset these params each iteration
		cipbr.hFileInfo.ioDirID = mDirID;
		cipbr.hFileInfo.ioFDirIndex = fileIndex;
		OSErr infoErr = ::PBGetCatInfoSync(&cipbr);
		if (infoErr!=noErr) {
			break;  // end of directory
		}
		theList->append(oofFileRef(theSpec, (cipbr.hFileInfo.ioFlAttrib & ioDirMask) ? oofFileRef::eDir : oofFileRef::eFile));
	}  // loop files
	return theList;
}


bool
oofDirectory::isEmpty()  const  // _Macintosh
{
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	
	cipbr.dirInfo.ioVRefNum =  mVRefNum;	
	cipbr.dirInfo.ioNamePtr =  0;  // so just uses ioDrDirID to lookup!
	cipbr.dirInfo.ioCompletion = 0;
	cipbr.dirInfo.ioDrDirID = mDirID;
	cipbr.dirInfo.ioFDirIndex = 0;
	OSErr infoErr = ::PBGetCatInfoSync(&cipbr);
	bool ret = true; // default empty same as error getting info!
	if (infoErr==noErr)
		ret = (cipbr.dirInfo.ioDrNmFls == 0);
	return ret;
}


/**
	Iterate through the directory tree applying Visitor.
	\todo fix Windows version for CodeWarrior - missing makepath.
*/
void 
oofDirectory::visitFiles(oofDirectoryVisitor& theVisitor, unsigned long remainingVisitDepth, bool firstVisit) // _Macintosh
{
	if (firstVisit && !theVisitor.enterRoot(this) )
		return;  // early exit - thrown out of this root for some reason!

	oofFileList* myFiles = makeFileList();
	const unsigned short numFiles = myFiles->count();
	for (unsigned short i=0;i<numFiles; i++) {
		oofRuntimeEnv::spendBackgroundTime();
		const oofFileRef theFile = myFiles->ref(i);
		FSSpec theSpec = theFile.spec();
//		cout << endl << "(Spec before " << oofFileRef(theSpec) << ")   ";
		Boolean isFolder, wasAliased;
		FSSpec prevSpec;
		::BlockMoveData(&theSpec, &prevSpec, sizeof(FSSpec));
		OSErr resolveErr = ::ResolveAliasFile(
			&theSpec, true /* don't traverse all chains */, 
			&isFolder, &wasAliased
		);
		if (resolveErr!=noErr) {
		// suspect the spec may have been trashed???
//			cout << "(Spec after " << oofFileRef(theSpec) << ") " << endl;
			if (theVisitor.handleLinkUnresolved(prevSpec))
				continue;
			else 
				RAISE_EXCEPTION( oofE_AliasNotResolved(prevSpec) );
		}  // failed to follow link
		
		if (isFolder) {
			const oofFileRef dirRef(theSpec, oofFileRef::eDir);
			const bool recurseDir = theVisitor.enterDir(dirRef, this);  
			if (recurseDir && remainingVisitDepth>1) {  // we are still recursing
				// if aliased, translate, if folder, need folder's dir (not parent)
				CInfoPBRec	cipbr;  // parameter block, union of dir and file info
				cipbr.hFileInfo.ioVRefNum = theSpec.vRefNum;
				cipbr.hFileInfo.ioDirID = theSpec.parID;
				cipbr.hFileInfo.ioNamePtr = theSpec.name;  // ESSENTIAL - if don't set this return buffer will overwrite low memory
				cipbr.hFileInfo.ioFDirIndex = 0;  // direct lookup
				OSErr dirInfoErr = ::PBGetCatInfoSync(&cipbr);
				if (wasAliased) {
				// no need to hit the disk for this lookup unless we are recursing into the dir
					if (dirInfoErr!=noErr)  {  // dir has vanished between resolving alias and now
						if (theVisitor.handleLinkUnresolved(theSpec))
							continue;
						else 
							RAISE_EXCEPTION( oofE_AliasNotResolved(theSpec));
					}  // failed to follow link
					const short savedVRefNum = mVRefNum;  // alias may be on another volume
					const long savedDirID = mDirID;  // RECURSION - save internal state on stack
					mVRefNum = theSpec.vRefNum;  // so other methods of this object work consistently
					// theSpec at this point has a parent ID of the directory *containing* the dir
					// we've jumped to, so we need to get info on the directory pointed to by the spec
					// to know the directory ID we are now iterating
					mDirID = cipbr.dirInfo.ioDrDirID;		
					visitFiles(theVisitor, remainingVisitDepth-1, false);  // recurse to this subdir
					mVRefNum = savedVRefNum;
					mDirID = savedDirID;
				}  // aliased folder recursion
				else {
					if (dirInfoErr==noErr)  {  // else dir has vanished while iterating
						const long savedDirID = mDirID;  // RECURSION - save internal state on stack
						mDirID = cipbr.dirInfo.ioDrDirID;	// ID of real directory we encountered
						visitFiles(theVisitor, remainingVisitDepth-1, false);  // recurse to this subdir
						mDirID = savedDirID;
					}
				}  // local folder recursion
			}  // recurse into folder
			theVisitor.leaveDir(dirRef, this);    // immediately leave it
		}  // is folder
		else { 
			const oofFileRef theFile(theSpec, oofFileRef::eFile);
			theVisitor.visit(theFile, this);  
		}
	}  // loop files
	
	delete myFiles;
}

bool 
oofDirectory::containsFile(const char* justFileName) const  // _Macintosh
{
	if (!justFileName || strlen(justFileName)==0)
		return false;	// **** EARLY EXIT - missing file name
		
	bool ret = mValidDir;
	if (ret) {	
		Str255 pascalPath;
		unsigned short pathLen = strlen(justFileName);
		assert(pathLen<256);
		pascalPath[0] = pathLen;
		memcpy(&pascalPath[1], justFileName, pathLen);
		CInfoPBRec pb;
		pb.dirInfo.ioNamePtr = pascalPath;	// entire or relative path
		pb.dirInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
		pb.dirInfo.ioVRefNum = 0;	// default vol
		pb.dirInfo.ioDrDirID = mDirID;// default dir
		OSErr err = PBGetCatInfoSync(&pb);
		ret = (err==noErr);
	}
	return ret;
}


/**
	Confirm a file exists.
	\warning very old code relying on parsing Pascal paths
	\todo rewrite to use relative path conversion
	\todo handle larger paths, maybe using oofFileRef
*/
bool
oofDirectory::fileExists(const char* fullPath)  // _Macintosh
{
	if (!fullPath || strlen(fullPath)==0)
		return false;	// **** EARLY EXIT - missing file name
		
	bool ret = false;
	Str255 pascalPath;
	unsigned short pathLen = strlen(fullPath);
	assert(pathLen<256);
	pascalPath[0] = pathLen;
	memcpy(&pascalPath[1], fullPath, pathLen);
	CInfoPBRec pb;
	pb.dirInfo.ioNamePtr = pascalPath;	// entire or relative path
	pb.dirInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	pb.dirInfo.ioVRefNum = 0;	// default vol
	pb.dirInfo.ioDrDirID = 0;// default dir
	OSErr err = PBGetCatInfoSync(&pb);
	ret = (err==noErr);
	return ret;
}


oofString 
oofDirectory::filename() const  // _Macintosh
{
	Str255 dirName;
	CInfoPBRec pb;
	pb.dirInfo.ioNamePtr = dirName;	// returns name
	pb.dirInfo.ioFDirIndex = -1;	/* use ioNamePtr returning name for dir in ioDrDirID */
	pb.dirInfo.ioVRefNum = mVRefNum;	
	pb.dirInfo.ioDrDirID = mDirID;// this dir
	OSErr err = PBGetCatInfoSync(&pb);
	
// this returning a temp object allows 'tail-end optimisation' to take place
// if compiler smart enough to avoid constructing extra temp copy	
	if (err==noErr)
		return oofString(dirName);
	else
		return oofString();	// empty name
}

#else
// common generic Windows and Unix methods



// static utility
bool
oofDirectory::fileExists(const char* fullPath)  // _Windows, _Unix
{
	if (!fullPath || strlen(fullPath)==0)
		return false;	// **** EARLY EXIT - missing file name
		
	bool ret = false;
// generic solution relies on cross-platform directory parsers
	FILE *fp;

	char* resultingName = new char[strlen(fullPath)+1];
	strcpy(resultingName, fullPath);

// cope with an embedded path in the file name	
	stSaveDirectory saveDir;
	oofDirectory fileDir(fullPath, resultingName);
	ret = fileDir.validDir();
	
	if (ret) {	// see if file exists in dir
		fileDir.gotoDirectory();
		
		fp = fopen(resultingName, "rb" );
		if (fp) {
			fclose( fp );
			ret = true;
		}
		else
			ret = false;
	}
	delete[] resultingName;
	return ret;
}


bool 
oofDirectory::containsFile(const char* justFileName) const  // _Windows, _Unix
{
	if (!justFileName || strlen(justFileName)==0)
		return false;	// **** EARLY EXIT - missing file name
		
	bool ret = mValidDir;
	if (ret) {
	// generic solution relies on cross-platform directory parsers
		FILE *fp;
	
	// cope with an embedded path in the file name	
		stSaveDirectory saveDir;
		gotoDirectory();
			
		fp = fopen(justFileName, "rb" );
		if (fp) {
			fclose( fp );
			ret = true;
		}
		else
			ret = false;
	}
	return ret;
}


void 
oofDirectory::visitFiles(oofDirectoryVisitor& theVisitor, unsigned long remainingVisitDepth, bool firstVisit) // _Windows, _Unix
{
	if (firstVisit && !theVisitor.enterRoot(this) )
		return;  // early exit - thrown out of this root for some reason!

	const char dirTermChar = directoryTerminator();  // cache this environment-dependent variable
	
	oofFileList* myFiles = makeFileList();
	const unsigned short numFiles = myFiles->count();
	for (unsigned short i=0;i<numFiles; i++) {
		oofRuntimeEnv::spendBackgroundTime();
		const oofFileRef theFile = myFiles->ref(i);

// NOT YET IMPLEMENTED - traverse Win95 desktop links as equivalent of Mac aliases
// and not sure we would want to ever implement that feature - it's not a Win32 API feature!

		if (theFile.isDirectory()) {
			const bool recurseDir = theVisitor.enterDir(theFile, this);  
			if (recurseDir && remainingVisitDepth>1) {  // we are still recursing
				const oofString savedPath = mPath;  // RECURSION - save internal state on stack
				mPath = theFile.path();
				mPath += dirTermChar;
					visitFiles(theVisitor, remainingVisitDepth-1, false);  // recurse to this subdir
				mPath = savedPath;
			}  // recurse into folder
			theVisitor.leaveDir(theFile, this);    // immediately leave it
		}  // is folder
		else 
			theVisitor.visit(theFile, this);  
	}  // loop files
	
	delete myFiles;
}


	#ifdef _Windows
		oofString 
		oofDirectory::filename() const  // _Windows
		{
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			::_splitpath(mPath, drive, dir, fname, ext);
			return oofString(fname, ext);
		}
		
		
		void	
		oofDirectory::setDirFromPath(const char* path, char* outFileName, bool createSubdirsIfMissing, bool /*makeAbsolute*/) // _Windows
		{
			assert(path != NULL);
	
			oofPathString pathStr(path);
			if (pathStr.isRelativePath())
				pathStr.relativePathPlatform(outFileName==NULL);  // appends separator if not expecting filename on end
				
			const unsigned int pathLen = pathStr.length();
			path = pathStr.chars();  // will be same still if not needed to convert

			char workDir[_MAX_PATH];
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			oofString fixedPath(path);
			if (!outFileName && (fixedPath.lastChar() != '\\'))
				fixedPath+= '\\'; // append terminating \ unless we expect to parse a filename

			path = fixedPath;  // change the pointer so it uses our copy which MAY have been terminated
			
			// cope with relative paths in path
			mValidDir = false;
			if (::_fullpath(workDir, path, _MAX_PATH) == NULL)
				return;  //**** early failure exit - illegal path
	
			::_splitpath(workDir, drive, dir, fname, ext);

			// outFileName will contain only the file name without the path
			if (outFileName != NULL) {
				if (drive[0]=='\0' && dir[0]=='0')
					::strcpy(outFileName, path);  // just return original filename if no Drive or Directory
				else {
					// Elemir originally used makepath again here but it does an overwrite, trying to 
					// write _MAX_PATH chars over the outFileName, which might only be the length of the
					// original path
					::strcpy(outFileName, fname);
					::strcat(outFileName, ext);
				}
			}
		#ifdef __MWERKS__
			assert(!"this doesn't work - Andy to fix");
		#else
			::_makepath(workDir, drive, dir, NULL, NULL);
		#endif
			if (::strlen(workDir) == 0 || ::SetCurrentDirectory(workDir)) {
				// the workDir might contain relative path, so read it again
				mValidDir = ::GetCurrentDirectory(_MAX_PATH, workDir) <= _MAX_PATH;
				mPath = workDir;
			}
			else if (createSubdirsIfMissing) {
				if (::strlen(drive) > 0 && !::SetCurrentDirectory(drive))
					return;		// invalid drive
	
				mValidDir = oofDirectory::CreateSubdirectories(dir);
				mCreatedDiskDir = mValidDir;

				// the workDir might contain relative path, so read it again
				mValidDir = mValidDir && ::GetCurrentDirectory(_MAX_PATH, workDir) <= _MAX_PATH;
				// new directory is stored
				mPath = workDir;
			}  // create missing
			if (mPath.lastChar() != '\\')  // being super-careful, but testing shows
				mPath += '\\'; // DOS doesn't terminate paths!
		}
			
			
		bool
		oofDirectory::CreateSubdirectories(char* dir)  // _Windows
		{
			assert(dir != NULL);
		
			if (strlen(dir) == 0)
				return true;
		
			char* tmpDir = strchr(dir, '\\');
		
			// the last char of dir isn't '\'
			if (tmpDir == NULL)
				tmpDir = dir + strlen(dir) - 1;
		
			char prevChar = *++tmpDir;
		
			*tmpDir = '\0';
		
			if (!::SetCurrentDirectory(dir))
				if (!::CreateDirectory(dir, NULL) || !::SetCurrentDirectory(dir))
					return false;
		
			*tmpDir = prevChar;
		
			return CreateSubdirectories(tmpDir);
		}
		
		void
		oofDirectory::GetCurrentLocation()  // _Windows
		{
			char buffer[_MAX_PATH];
		
			if (::GetCurrentDirectory(_MAX_PATH, buffer) > 0) {
				mPath = buffer;
				if (mPath.lastChar() != '\\')  // being super-careful, but testing shows
					mPath += '\\'; // DOS doesn't terminate paths!
			}
			else
				mValidDir = false;
		}
		
		
		void 
		oofDirectory::gotoDirectory(const oofString& inPath)  // _Windows
		{
			if (!inPath.isEmpty()) {
				if (!::SetCurrentDirectory(inPath)) {
					const oofDirectory tempDir(inPath);
					RAISE_EXCEPTION( oofE_InvalidDir(tempDir, "oofDirectory::gotoDirectory") );
				}
			}
		}
		
		
		void 
		oofDirectory::gotoDirectory() const  // _Windows
		{
			if (!mPath.isEmpty()) {
				if (!::SetCurrentDirectory(mPath))
					RAISE_EXCEPTION( oofE_InvalidDir(*this, "oofDirectory::gotoDirectory") );
			}
		}
		
		
		#if(WINVER >= 0x0400)
		void
		oofDirectory::FillInExecInfo(const char* filename, SHELLEXECUTEINFO& sei, HWND hwnd) const
		{
			sei.cbSize = sizeof(sei);
			sei.fMask  = 0;
			sei.hwnd   = hwnd;
			sei.lpVerb = "open";
			sei.lpFile = filename;
			sei.lpParameters = NULL;
			sei.lpDirectory  = NULL;
			sei.nShow  = SW_SHOWNORMAL;
			sei.hInstApp = NULL;
		}
		#endif  /* (WINVER >= 0x0400) */
		
		bool
		oofDirectory::openFile(const char* justFileName, HWND hwnd) const  // _Windows
		{
			bool ret = false;
		#if  WINVER >= 0x0400
		// NOT YET IMPLEMENTED for Win3.11 without win32s
		    if (oofRuntimeEnv::runtimeEnv() == oofRuntimeEnv::Win32s){
		        HINSTANCE hShell = ::ShellExecute(hwnd, NULL, justFileName, NULL, NULL, SW_NORMAL);
		        ret = hShell > (HINSTANCE)32; //ShellExecute returns the instance handle or an error message <= 32
		    }
		    else{
			    SHELLEXECUTEINFO sei;
		
			    if (mValidDir) {
				    FillInExecInfo(justFileName, sei, hwnd);
				    ret = ::ShellExecuteEx(&sei) == TRUE;
			    }
		    }
		#endif /* WINVER >= 0x0400 */
		
			return ret;
		}
		
		
		bool
		oofDirectory::convertMac2WinPath(const char* macPath, char* winPath)
		{
			char vol[MAX_MAC_VOL];
			char dir[MAX_MAC_DIR];
			char file[MAX_MAC_FNAME];
			char ext[MAX_MAC_EXT];
		
			if (!SplitMacPath(macPath, vol, dir, file, ext))
				return false;
		
			char winDrive[_MAX_DRIVE];
			char winDir[_MAX_DIR];
		
			if (TransformMacDir(dir, winDir))
			{
				char fname[_MAX_FNAME+_MAX_EXT];
		
				strcpy(fname, file);
				strcat(fname, ext);
		
				if (FindWinDrive(winDrive, vol, winDir, fname))
				{
					strcpy(winPath, winDrive);
					strcat(winPath, winDir);
					strcat(winPath, fname);
					return true;
				}
			}
		
			return false;
		}
		
		
		bool
		oofDirectory::SplitMacPath(const char* macPath, char* vol, char* dir, char* file, char* ext)
		{
			if (macPath == NULL)
				return false;
		
			int ind;
		
			// volume
			//
			for(ind = 0; macPath[ind] != '\0' && macPath[ind] != ':'; ind++)
				;
		
			if (macPath[ind] != '\0') {
				if (ind >= MAX_MAC_VOL)
					return false;
			}
			else
				ind = 0;	// no volume found
		
			if (vol != NULL) {
				strncpy(vol, macPath, ind);
				vol[ind] = '\0';
			}
		
			// dir
			//
			int colonInd;
			for (colonInd = strlen(macPath); colonInd >= 0 && macPath[colonInd] != ':'; colonInd--)
				;
		
			if (colonInd > 0) { 
				int dirLen = colonInd - ind + 1;	// count the ending ':'
				if (dirLen >= MAX_MAC_DIR)
					return false;
		
				if (dir != NULL) {
					strncpy(dir, &macPath[ind], dirLen);
					dir[dirLen] = '\0';
				}
		
				ind = colonInd+1;
			}
			else if (dir != NULL)
				dir[0] = '\0';
		
			// file name
			//
			int nameInd;
			for(nameInd = ind; macPath[nameInd] != '\0' && macPath[nameInd] != '.'; nameInd++)
				;
		
			int nameLen = nameInd - ind;
			if (nameLen >= MAX_MAC_FNAME)
				return false;
		
			if (file != NULL) {
				strncpy(file, &macPath[ind], nameLen);
				file[nameLen] = '\0';
			}
		
			ind = nameInd;
		
			if (ext != NULL) {
				if (strlen(&macPath[ind]) >= MAX_MAC_EXT)
					return false;
		
				strcpy(ext, &macPath[ind]);
			}
		
			return true;
		}
		
		bool
		oofDirectory::TransformMacDir(const char* macDir, char* winDir)
		{
			int i=0;
			int j=0;
		
			while (macDir[i] != '\0')
			{
				if (macDir[i] == ':') {
					if (macDir[i+1] == ':') {
						if (j == 0 || winDir[j] == '\\') {
							memcpy(&winDir[j], "..\\", 3);
							j += 2;
						}
						else {
							memcpy(&winDir[j], "\\..\\", 4);
							j += 3;
						}
		
						i += 1;
					}
					else
						winDir[j] = '\\';
				}
				else
					winDir[j] = macDir[i];
		
				i++; j++;
		
				if (j >= _MAX_DIR)
					return false;
			}
		
			winDir[j] = '\0';
		
			return true;
		}
		
		bool
		oofDirectory::FindWinDrive(char* winDrive, const char* macVol, const char* winDir, const char* fname)
		{
			assert(winDrive != NULL);
			assert(macVol != NULL);
			assert(winDir != NULL);
		
			stSaveDirectory curDir;
		
			char testDrive[_MAX_DRIVE] = {" :"};
		
			// if we can switch to the drive, it exists (skip drives A and B)
			for (int drive = 3; drive <= 26; drive++) {
				testDrive[0] = drive + 'A' - 1;
				if (::SetCurrentDirectory(testDrive)) {
					// save current directory
					stSaveDirectory curDir;
					// check whether winDir exists on this drive
					if (::SetCurrentDirectory(winDir)) {
						if (fname == NULL || fileExists(fname)) {
							strcpy(winDrive, testDrive);
							return true;
						}
					} // change dir
				} // change drive
			} // enum available drives
		
			return false;
		}
						
		
		oofFileList*
		oofDirectory::makeFileList()  const  // _Windows
		{
			assert(mValidDir);
			oofFileList* theList = new oofFileList;  // make on heap to avoid huge copy

			const oofString searchPath(mPath,"*.*");  // wildcard for all files - NOT YET IMPLEMENTED - pass in wildcards!
			WIN32_FIND_DATA findData;
			HANDLE hFindFile = ::FindFirstFile(searchPath, &findData);
			if (hFindFile!=INVALID_HANDLE_VALUE) {
				do{
					const char* fn = findData.cFileName;
					if (
						(fn[0]=='.') &&
						(
							(fn[1]=='\0') ||    // single period
							((fn[1]=='.') && (fn[2]=='\0'))   // two periods
						)
					)
						continue;  // loop for "." or ".." but cope if leading ".Blah" or "..Blah"
					
					const bool fileIsDir = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
					const oofString filePath(mPath, fn);
					theList->append(
						oofFileRef(filePath, fileIsDir ? oofFileRef::eDir : oofFileRef::eFile)
					);
				}
				while (::FindNextFile(hFindFile, &findData) );  // loop files
				::FindClose(hFindFile);
			}  // failure to find first file
			return theList;
		}
		
		
		bool
		oofDirectory::isEmpty()  const  // _Windows
		{
			// an empty DOS dir still contains "." and ".."

			assert(mValidDir);

			// copy of loop logic from makeFileList, but don't add to list!
			// looks a bit weird by itself - would normally invert the sense of the comparisons
			// but I wanted to keep the code IDENTICAL with makeFileList apart from the actual
			// adding of files to the list
			const oofString searchPath(mPath,"*.*");  // wildcard for all files - NOT YET IMPLEMENTED - pass in wildcards!
			WIN32_FIND_DATA findData;
			bool ret = true;
			HANDLE hFindFile = ::FindFirstFile(searchPath, &findData);
			if (hFindFile!=INVALID_HANDLE_VALUE) {
				do{
					const char* fn = findData.cFileName;
					if (
						(fn[0]=='.') &&
						(
							(fn[1]=='\0') ||    // single period
							((fn[1]=='.') && (fn[2]=='\0'))   // two periods
						)
					)
						continue;  // loop for "." or ".." but cope if leading ".Blah" or "..Blah"
					
					ret = false;  // found something else
					break; // IMPORTANT to break loop and call FindClose, rather than just return false!
				}
				while (::FindNextFile(hFindFile, &findData) );  // loop files
				::FindClose(hFindFile);
			}  // failure to find first file
			return ret;
		}



	#else // assume some Unix variant		
		void 
		oofDirectory::gotoDirectory(const oofString& inPath)  // _Unix
		{
			if (!inPath.isEmpty()) {
				const int err = ::chdir(inPath);
				if (err!=0) {
					const oofDirectory tempDir(inPath);
					RAISE_EXCEPTION( oofE_InvalidDir(tempDir, "oofDirectory::gotoDirectory") );
				}
			}
		}


		void 
		oofDirectory::gotoDirectory() const  // _Unix
		{
			if (!mPath.isEmpty()) {
				const int err = ::chdir(mPath);
				if (err!=0)
					RAISE_EXCEPTION( oofE_InvalidDir(*this, "oofDirectory::gotoDirectory") );
			}
		}
		
		
		oofString 
		oofDirectory::filename() const  // _Unix
		{
			const long slashAt = mPath.findLast('/');
			if (slashAt==oofString::kNotFound)
				return mPath;
			else
				return mPath.subString(slashAt+1);
		}
		
		
		void	
		oofDirectory::setDirFromPath(const char* path, char* outFileName, bool createSubdirsIfMissing, bool makeAbsolute) // _Unix
		{
			oofPathString pathStr(path);
			bool isRelative = pathStr.isRelativePath();  // note that "~/something" is not relative as '~' indicates absolute root
			if (isRelative)
				pathStr.relativePathPlatform(outFileName==NULL);  // appends separator if not expecting filename on end
				
			const unsigned int pathLen = pathStr.length();
			path = pathStr.chars();  // will be same still if not needed to convert

			if (outFileName != NULL) {
				bool anyDirSepsMet = false;
				unsigned int lastChar = strlen(path)-1;
				outFileName[0]='\0';	// default to empty filename, string is dir path only
				if (path[lastChar]!='/') { // have at least 1 char filename
					for (unsigned int ip = lastChar-1; ip>0; ip--) {
						if (path[ip]=='/') {
			// we have a leaf!
							strcpy(outFileName, &path[ip+1]);
							mPath.setChars(path, ip);	// copy rest into path	
							anyDirSepsMet = true;
							break;		
						} // found sep
					} // loop looking for sep
				}  // doesn't end in sep
				if (!anyDirSepsMet) {
					strcpy(outFileName, path); // copy entire path to filename
					mPath.clear();
				}
			} // user wants filename back
			else
				mPath = path;	// just copy the path
			
			// at this point mPath has been assigned a path after appropriate conversion and removing filename 
			if (mPath.isEmpty()) {
				if (makeAbsolute) {
					GetCurrentLocation();
					isRelative = false;
				}
				else
					mPath = "./";
			}
			else {  // have a path which may still be relative
				if (mPath.lastChar()!='/')
					mPath += '/';  // OOFILE keeps dirctory paths terminated!
	
				DIR* dp = ::opendir(mPath);
				if (dp)
					::closedir(dp);
				else {  // path not there
					if (createSubdirsIfMissing) {
						const int makeErr = ::mkdir(mPath, 0777);  // all access, modified by umask
						mValidDir = (makeErr==0);
						mCreatedDiskDir = true;
					}
					else {  // dir not there and asked not to create
						mValidDir = false;
					}
				} // not there!	
				if  (isRelative && makeAbsolute) {
					const oofString saveRel = mPath;
					GetCurrentLocation();
					mPath += saveRel;  // it's OK for Unix to have a path that starts out absolute and navigates relatively
				}
			} // had path
		}
		
		
		void
		oofDirectory::GetCurrentLocation()  // _Unix
		{
		// loop trying ever increasing buffer sizes until manage to read
		// entire path (see "Advanced Programming in the Unix Environment" p113)
			for (size_t pathAlloc=1025; ;pathAlloc+=1024) {
				char* pathBuf = new char[pathAlloc];
				if (::getcwd(pathBuf, pathAlloc)) {  // returns pathBuf if fits
					mPath.adopt(pathBuf);					
					break;  // got it!
				}
				delete[] pathBuf;
			} // loop until path big enough!
			if (mPath.lastChar()!='/')
				mPath += '/';  // OOFILE keeps dirctory paths terminated!
		}
		
		
		oofFileList*
		oofDirectory::makeFileList()  const  // _Unix
		{
			assert(mValidDir);
			oofFileList* theList = new oofFileList;  // make on heap to avoid huge copy

			DIR* dp = ::opendir(mPath);
			if (dp) {
				for (;;) {
					struct dirent* dirp = ::readdir(dp);
					if (!dirp)
						break;  // end of files
						
					const char* fn = dirp->d_name;
					if (
						(fn[0]=='.') &&
						(
							(fn[1]=='\0') ||    // single period
							((fn[1]=='.') && (fn[2]=='\0'))   // two periods
						)
					)
						continue;  // loop for "." or ".." but cope if leading ".Blah" or "..Blah"
					
					const oofString filePath(mPath, fn);
					theList->append( oofFileRef(filePath) );
				}
				::closedir(dp);
			}
			return theList;
		}
		
		
/**
	Check if Unix directory is empty by scanning for contents.
	Copy of loop logic from makeFileList, but don't add to list.
	looks a bit weird by itself - would normally invert the sense of the comparisons
	but I wanted to keep the code IDENTICAL with makeFileList apart from the actual
	adding of files to the list.
	\note an empty Unix dir still contains "." and ".."
*/
		bool
		oofDirectory::isEmpty()  const  // _Unix
		{

			assert(mValidDir);

			bool ret = true;
			DIR* dp = ::opendir(mPath);
			if (dp) {
				for (;;) {
					struct dirent* dirp = ::readdir(dp);
					if (!dirp)
						break;  // end of files
						
					const char* fn = dirp->d_name;
					if (
						(fn[0]=='.') &&
						(
							(fn[1]=='\0') ||    // single period
							((fn[1]=='.') && (fn[2]=='\0'))   // two periods
						)
					)
						continue;  // loop for "." or ".." but cope if leading ".Blah" or "..Blah"
					
					ret = false;  // found something!
					break;  // stop loop on first real file or dir 
				}
				::closedir(dp);
			}
			return ret;
		}



	#endif  // _Unix
#endif // _Macintosh vs _WIndows, _Unix


oofDirectory
oofDirectory::createDirectory(const char* path) 
{
	return oofDirectory(path, 0, true);
}


void 
oofDirectory::extract(ostream& os)
{
	oofDirectoryList tempVisitor(os);
	visitFiles(tempVisitor);
}


ostream& 
operator<<(ostream& os, oofDirectory& theDir)
{
	theDir.extract(os);
	return os;
}


// -------------------------------------------------------
//        o o f D i r e c t o r y V i s i t o r
// -------------------------------------------------------
oofDirectoryVisitor::oofDirectoryVisitor(bool ignoreBadAliases) :	
	mDirDepth(0),
	mIgnoreBadAliases(ignoreBadAliases)
{}


bool 
oofDirectoryVisitor::handleLinkUnresolved(const oofFileRef&)
{
	return mIgnoreBadAliases;		// default implementation
	// set or pass in true for mIgnoreBadAliases if don't want false here
}


/**
	default just updates depth.
	for consistency in depth, best if overriden versions also call this or update depth

	if wanted to treat just like any other dir, override with:
	\return enterDir(oofFileRef(*caller));
*/
bool
oofDirectoryVisitor::enterRoot(oofDirectory* /*caller*/)
{
	mDirDepth++;
	return true;
}


/**
	Called before iterating an oofDirectory's contents.
	The convention for subclasses is increment mDirDepth BEFORE any other processing in enterDir
	the reason being that we enter a directory as a result of iterating items in the preceding directory
	thus the very first directory being iterated is level 0 and we must increment BEFORE enterDir does anything
	to ensure that this dir is processed as level 1 or deeper.
*/
bool
oofDirectoryVisitor::enterDir(const oofFileRef&, oofDirectory* /*caller*/)
{
	mDirDepth++;
	return true;
}


void
oofDirectoryVisitor::leaveDir(const oofFileRef&, oofDirectory* /*caller*/)
{
	mDirDepth--;
}


// -------------------------------------------------------
//        o o f D i r e c t o r y L i s t
// -------------------------------------------------------
oofDirectoryList::oofDirectoryList(ostream& os, bool listFiles, const char* indentStr) : 
	mOS(os),
	mFilesVisited(0),
	mIndentString(indentStr),
	mListFiles(listFiles)
{}


void 
oofDirectoryList::WriteIndent() const
{
	assert(mOS.good());
	for (short i=0; i<mDirDepth; i++)
		mOS << mIndentString;
}


void 
oofDirectoryList::WriteIndentedName(const oofFileRef& fileRef) const
{
	WriteIndent();
	mOS << fileRef.filename();  // cross-platform string
}


bool
oofDirectoryList::enterDir(const oofFileRef& fileRef, oofDirectory* /*caller*/)
{
	mDirDepth++;
	WriteIndentedName(fileRef);
	mOS << ":" << endl;
	if (!mListFiles)
		mFilesVisited++;	// count directories instead
	return true;
}


void
oofDirectoryList::leaveDir(const oofFileRef&, oofDirectory* /*caller*/)
{
	mDirDepth--;
	if (mDirDepth<0)  // have just left top level
		if (mListFiles)
			mOS << endl << "Total files listed: " << mFilesVisited << endl;
		else  // we counted directories, not files
			mOS << endl << "Total directories listed: " << mFilesVisited << endl;
}


void 
oofDirectoryList::visit(const oofFileRef& fileRef, oofDirectory* /*caller*/)
{
	if (mListFiles) {
		WriteIndentedName(fileRef);
		mOS << endl;
		mFilesVisited++;
	}
}


bool 
oofDirectoryList::handleLinkUnresolved(const oofFileRef& fileRef)
{
	WriteIndentedName(fileRef);
	mOS << "   **** broken " << oofRuntimeEnv::termForLink() << " ****" << endl;
	return true;		// just report it
}


// -------------------------------------------------------
//        o o f D e l D i r T r e e V i s i t o r
// -------------------------------------------------------
oofDelDirTreeVisitor::oofDelDirTreeVisitor(bool delSubdirs) : 
	mDelSubdirs(delSubdirs)
{}


void
oofDelDirTreeVisitor::leaveDir(const oofFileRef&, oofDirectory* caller)
{
	if (mDelSubdirs) {
		oofFileRef thisDir(*caller, "", oofFileRef::eDir);
		thisDir.deleteFile();
	}
}


void 
oofDelDirTreeVisitor::visit(const oofFileRef& fileRef, oofDirectory* /*caller*/)
{
	fileRef.deleteFile();
}


// -------------------------------------------------------
//              s t S a v e D i r e c t o r y
// -------------------------------------------------------

/**
	creating instance automatically inits our dir to the current directory.
*/
stSaveDirectory::stSaveDirectory()
{
}


/**
	Return to the directory that was current when instance created.
	\note Default ctor for member mSavedDir automatically inits our dir to the current directory.
*/
stSaveDirectory::~stSaveDirectory()
{
	mSavedDir.gotoDirectory();
}





#ifdef _Macintosh
//   STUFF FROM DTS SNIPPET - SendFinderOpen.c


/**
	OpenSelection takes a FSSpec pointer, and creates a Finder Open Selection.
	AppleEvent for the document described by the FSSpec.  This can be an
	application or document. 
*/
OSErr 
OpenSelection(FSSpecPtr theDocToOpen)
{
	/* temp variables I'll be using */
	AppleEvent aeEvent;                                         /* the event to create */
	AEDesc myAddressDesc, aeDirDesc, listElem;                  /* some descriptors I'll need */
	FSSpec dirSpec;                                             /* FSSpec for the 'parent' directory of the file I'm opening */
	AEDesc fileList;                                            /* my list */
	OSErr myErr;                                                /* guess */
	ProcessSerialNumber process;                                /* This will hold the process serial number of the Finder */
	AliasHandle DirAlias, FileAlias;                            /* some aliases */
	
	/* go find the Finder's process information, please */
	if (FindAProcess(kFinderSig, kSystemType, &process))
		return(procNotFound);     /* if I can't find the Finder, quit.  Always check this, someone else */
	/* could have shut down the finder */
	/* Create an address descriptor so the AppleEvent manager knows where to send this event */
	if ((myErr = AECreateDesc(typeProcessSerialNumber, (Ptr)&process, sizeof(process), &myAddressDesc))!=0)
		return(myErr);
	
	/* Create the empty FinderEvent */
	/* it's a Finder 'FNDR', Open Selection 'sope' event */
	if ((myErr = AECreateAppleEvent(kFinderSig, aeOpenSelection, &myAddressDesc, kAutoGenerateReturnID, kAnyTransactionID, &aeEvent))!=0)
		return(myErr);
	
	/* make a FSSpec for the parent folder (see the OpenSeletion description in the AE Registry ) */
	/* using the information in the document FSSpec */
	FSMakeFSSpec(theDocToOpen->vRefNum, theDocToOpen->parID, nil, &dirSpec);
	NewAlias(nil, &dirSpec, &DirAlias);
	
	/* Create alias for file */
	NewAlias(nil, theDocToOpen, &FileAlias);
	/* Create the file  list */
	if ((myErr = AECreateList(nil, 0, false, &fileList))!=0)
		return(myErr);
	
	/*  create the folder  descriptor */
	HLock((Handle)DirAlias);
	AECreateDesc(typeAlias, (Ptr)*DirAlias, GetHandleSize((Handle)DirAlias), &aeDirDesc);
	HUnlock((Handle)DirAlias);
	DisposeHandle((Handle)DirAlias);
	/* put the Directory Desc in the event as the direct object */
	if ((myErr = AEPutParamDesc(&aeEvent, keyDirectObject, &aeDirDesc)) == noErr)
	{
	    /* done with the desc, kill it */
	    AEDisposeDesc(&aeDirDesc)
	        ;
	    /*  create the file descriptor and add to aliasList */
	    HLock((Handle)FileAlias);
	    AECreateDesc(typeAlias, (Ptr)*FileAlias, GetHandleSize((Handle)FileAlias), &listElem);
	    HLock((Handle)FileAlias);
	    DisposeHandle((Handle)FileAlias);
	    myErr = AEPutDesc(&fileList, 0, &listElem);
	}
	
	if (myErr)
		return(myErr);
	AEDisposeDesc(&listElem);
	/* Add the file alias list to the event */
	if ((myErr = AEPutParamDesc(&aeEvent, aeSelectionKeyword, &fileList))!=0)
		return(myErr);
	myErr = AEDisposeDesc(&fileList);
	
	/* And now send the event! */
	myErr = AESend(&aeEvent, nil, kAENoReply + kAEAlwaysInteract + kAECanSwitchLayer, kAENormalPriority, kAEDefaultTimeout, nil, nil);
	
	/* and kill the memory used */
	AEDisposeDesc(&aeEvent);
	return myErr;
}


/* This runs through the process list looking for the indicated application */
OSErr FindAProcess(OSType typeToFind, OSType creatorToFind, ProcessSerialNumberPtr processSN)
{
    ProcessInfoRec tempInfo;
    FSSpec procSpec;
    Str31 processName;
    OSErr myErr = noErr;
    /* nul out the PSN so we're starting at the beginning of the list */
    processSN->lowLongOfPSN = kNoProcess;
    processSN->highLongOfPSN = kNoProcess;
    /* initialize the process information record */
    tempInfo.processInfoLength = sizeof(ProcessInfoRec);
    tempInfo.processName = (unsigned char*)&processName;
    tempInfo.processAppSpec = &procSpec;
    /* loop through all the processes until we */
    /* 1) find the process we want */
    /* 2) error out because of some reason (usually, no more processes */
    do {
        myErr = GetNextProcess(processSN);
        if (myErr == noErr)
            GetProcessInformation(processSN, &tempInfo);
    }
    while ((tempInfo.processSignature != creatorToFind || tempInfo.processType != typeToFind) ||
                   myErr != noErr);
    return(myErr);
}



#endif




// -------------------------------------------------------
//                o o f F i l e R e f
// -------------------------------------------------------

#ifdef _Macintosh
/**
default ctor allows you to create oofFileRef then assign
*/
oofFileRef::oofFileRef(const char* path, isDirT isDir) : // _Macintosh
	mIsDir(isDir)
{
	if (!path || path[0]=='\0') { // empty or null path
		mSpec.vRefNum = 0;
		mSpec.parID = 0;
		mSpec.name[0] = 0;
		// initialise to an invalid state
	}
	else {
		char nameBuf[63];
		oofDirectory tempDir(path, nameBuf);
		tempDir.fillInFSSpec(nameBuf, mSpec);
	}
}


oofFileRef::oofFileRef(const oofString& path, isDirT isDir) : // _Macintosh
	mIsDir(isDir)
{
	char nameBuf[63];
	oofDirectory tempDir(path, nameBuf);
	tempDir.fillInFSSpec(nameBuf, mSpec);
}


oofFileRef::oofFileRef(const FSSpec& sp, isDirT isDir) :  // _Macintosh
	mSpec(sp),
	mIsDir(isDir)
{}


oofFileRef::oofFileRef(const oofDirectory& theDir, const oofString& name, isDirT isDir) : // _Macintosh
	mIsDir(isDir)
{
	theDir.fillInFSSpec(name, mSpec);
}


oofFileRef::oofFileRef(short vRefNum, long dirID, const oofString& name, isDirT isDir) : // _Macintosh
	mIsDir(isDir)
{
	mSpec.vRefNum = vRefNum;
	mSpec.parID = dirID;
	name.getStr255(mSpec.name);
}


const oofFileRef& 
oofFileRef::operator=(const FSSpec& inSpec) // _Macintosh
{
	mIsDir = eNotSet;
	mSpec = inSpec;
	return *this;
}


const oofFileRef& 
oofFileRef::operator=(const oofString& inPath) // _Macintosh
{
	mIsDir = eNotSet;
	char nameBuf[63];
	oofDirectory tempDir(inPath, nameBuf);
	tempDir.fillInFSSpec(nameBuf, mSpec);
	return *this;
}


void
oofFileRef::clear() // _Macintosh
{
	mIsDir = eNotSet;
	mSpec.vRefNum = 0;
	mSpec.parID = 0;
	mSpec.name[0] = 0;  // clear Pascal string
}

#else

oofFileRef::oofFileRef(const char* path, isDirT isDir) : // _Windows or _Unix
	mPath(path),
	mIsDir(isDir)
{}


oofFileRef::oofFileRef(const oofString& path, isDirT isDir) : //  _Windows or _Unix
	mPath(path),
	mIsDir(isDir)
{}


oofFileRef::oofFileRef(const oofDirectory& theDir, const oofString& name, isDirT isDir) : // _Windows or _Unix
	mPath(theDir.path(), name),
	mIsDir(isDir)
{
	assert(theDir.path().lastChar()==oofDirectory::directoryTerminator());
}



const oofFileRef& 
oofFileRef::operator=(const oofString& inPath)//  _Windows or _Unix
{
	mIsDir = eNotSet;
	mPath = inPath;
	return *this;
}


void
oofFileRef::clear() //  _Windows or _Unix
{
	mPath.clear();
}
#endif	


FILE* 
oofFileRef::fopen(const char* mode) const
{
	gotoParentDirectory();
	FILE* const ret = ::fopen(filename(), mode);
	return ret;
}


#ifdef _Macintosh
bool 
oofFileRef::scanVolsUntilFound() //  _Macintosh
{
 	if ((mSpec.vRefNum==0) && (mSpec.parID==0))
		return false;

	const short currVolToSkip = mSpec.vRefNum;
	bool ret = isValid();   // will only loop if not currently valid
 	
	for (short vIndex = 1; !ret; vIndex++)	{  // loop all volumes except the current one
		HParamBlockRec	HPBlock;
		
		HPBlock.volumeParam.ioNamePtr=0;
		HPBlock.volumeParam.ioVRefNum=0;
		HPBlock.volumeParam.ioVolIndex=vIndex;

		const OSErr volErr = ::PBHGetVInfoSync(&HPBlock);
		if (volErr==nsvErr) 
			break;	// not found by end of volumes
			
		assert(volErr==noErr);
		const short vRef = HPBlock.volumeParam.ioVRefNum;
		if (vRef!=currVolToSkip) {
			mSpec.vRefNum = vRef;
			ret = isValid();  // see if we exist
		}
	}  // volume loop
	return ret;
}


OSErr 
oofFileRef::readCInfo(CInfoPBRec& cipbr) const // _Macintosh
// utility used several times
{
	if ((mSpec.vRefNum==0) && (mSpec.parID==0))
		return fnfErr;
		
// now test if it actually exists
	// we have a combination of vol, dir ID and possibly partial path name 
	Str255 fileName;  // copy our directory's filename for the lookup, as it gets overwritten
	// (bloody horrible Mac OS interfaces which rewrite parameters!!@#$!
	::BlockMoveData(mSpec.name, fileName, mSpec.name[0]+1);
		
	cipbr.hFileInfo.ioVRefNum =  mSpec.vRefNum;	
	cipbr.hFileInfo.ioNamePtr =  fileName;	// buffer to specify/receive name
	cipbr.hFileInfo.ioCompletion = 0;
	cipbr.hFileInfo.ioDirID = mSpec.parID;
	cipbr.hFileInfo.ioFDirIndex = 0;  // direct lookup by ID + name
	const OSErr ret = ::PBGetCatInfoSync(&cipbr);
// NOT YET IMPLEMENTED - decide if this should cope with aliases	
	return ret;		
}


void 
oofFileRef::setFileType(OSType typeCode, OSType creatorCode)
{
	if(typeCode==0 && creatorCode==0)  // quick sanity check - silly call but legal 
		return;
		
	assert(isValid());
	
	FInfo theInfo;
	::FSpGetFInfo(&mSpec, &theInfo);
	
	if(typeCode!=0)
		theInfo.fdType = typeCode;
		
	if(creatorCode==0) {  // use current process (application)
		ProcessSerialNumber serialNum;
		OSErr err = ::GetCurrentProcess(&serialNum);
		if (err==noErr) {
			ProcessInfoRec	info;
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName=nil;
			info.processAppSpec=nil;
			
			err = ::GetProcessInformation(&serialNum, &info);
			if (err==noErr)
				theInfo.fdCreator = info.processSignature;
		}

		if (err!=noErr)
			RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::setFileType") );
	}
	else
		theInfo.fdCreator = creatorCode;
		
	OSErr setErr = ::FSpSetFInfo(&mSpec, &theInfo);
	if (setErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, setErr, "oofFileRef::setFileType") );
}



bool 
oofFileRef::isValid() const // _Macintosh
{
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	OSErr fileInfoErr = readCInfo(cipbr);
	const bool ret = (fileInfoErr==noErr);
	return ret;		
}
	
	
void
oofFileRef::extract(ostream& os) const // _Macintosh
{
	oofString tempName;
	tempName.setStr255(mSpec.name);
	os << mSpec.vRefNum << " " << mSpec.parID << " " << tempName;
}
	
	
void
oofFileRef::deleteFile(bool ignoreMissingFiles) const // _Macintosh
{
	OSErr delErr = ::FSpDelete(&mSpec);
	if (delErr!=noErr) { 
		if (delErr==fnfErr) {
			 if (!ignoreMissingFiles)
				RAISE_EXCEPTION( oofE_FileNotFound(*this, "oofFileRef::deleteFile") );
		}
		else
			RAISE_EXCEPTION( oofE_File(*this, delErr, "oofFileRef::deleteFile") );
	}
}


void 
oofFileRef::renameFile(const oofString& newName)   // _Macintosh
{
	Str255 newPascalName;
	newName.getStr255(newPascalName);
	const OSErr err = ::FSpRename(&mSpec, newPascalName);
	if (err!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::renameFile") );
}



bool 
oofFileRef::dirIsParent(const oofDirectory& inDir) const // _Macintosh
{
// was going to call this isInDirectory but that implies existence!
	FSSpec destSpec;
	inDir.fillInFSSpec(0, destSpec);
	const bool ret = ((destSpec.vRefNum==mSpec.vRefNum) && (destSpec.parID==mSpec.parID));
	return ret;
}



void 
oofFileRef::copyToDirectory(const oofDirectory& inDir, bool overwriteDest) const // _Macintosh
{
// portions of the following based on the Apple DTS sample FileCopy in MoreFiles
	enum
	{
		kBigCopyBuffSize  = 0x00080000,  // 512k
		kMinCopyBuffSize  = 0x00000200
	};


/*	The deny-mode privileges to use when opening the source and destination files. */

enum
{
	srcCopyMode = dmRdDenyWr,
	dstCopyMode = dmWrDenyRdWr
};


// definitions from MoreFilesExtras.h ----
	#define hasCopyFile(volParms)		(((volParms).vMAttrib & (1L << bHasCopyFile)) != 0)
//-----

	if (dirIsParent(inDir))
		return;  
		
	if (!inDir.validDir())
		RAISE_EXCEPTION( oofE_InvalidDir(inDir, "oofFileRef::copyToDirectory") );	
		
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	cipbr.dirInfo.ioACUser = 0;	/* ioACUser used to be filler2, clear it before calling GetCatInfo */
	const OSErr fileInfoErr = readCInfo(cipbr);
	if (fileInfoErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, fileInfoErr, "oofFileRef::copyToDirectory invalid source file") );	

	if (cipbr.hFileInfo.ioFlAttrib & ioDirMask)   // shortcut isDirectory() 
		RAISE_EXCEPTION( oofE_File(*this, fileInfoErr, "oofFileRef::copyToDirectory can't copy directory") );	
		
			
	FSSpec destSpec;
	inDir.fillInFSSpec(filename(), destSpec);
	if (overwriteDest) { 
		oofFileRef destFile(destSpec);
		if (destFile.isValid())
			destFile.deleteFile();
	}

	/* See if PBHCopyFile can be used.  Using PBHCopyFile saves time by letting the file server
	** copy the file if the source and destination locations are on the same file server. */
	GetVolParmsInfoBuffer infoBuffer;
	long tempLong = sizeof(infoBuffer);
	OSErr err = MF_HGetVolParms(mSpec.name, mSpec.vRefNum, &infoBuffer, &tempLong);
	if ( (err != noErr) && (err != paramErr) )
		RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory HGetVolParms") );	

	if ( (err != paramErr) && hasCopyFile(infoBuffer) )	{	/* The source volume supports PBHCopyFile. */
		long srcServerAdr = infoBuffer.vMServerAdr;

		/* Now, see if the destination volume is on the same file server. */
		tempLong = sizeof(infoBuffer);
		err = MF_HGetVolParms(NULL, destSpec.vRefNum, &infoBuffer, &tempLong);
		if ( (err != noErr) && (err != paramErr) )
			RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory HGetVolParms") );	
			
		if ( (err != paramErr) && (srcServerAdr == infoBuffer.vMServerAdr) )	{
			/* Source and Dest are on same server and PBHCopyFile is supported. Copy with CopyFile. */
			err = MF_HCopyFile(mSpec.vRefNum, mSpec.parID, destSpec.name, destSpec.vRefNum, destSpec.parID, 0, 0);
			if ( err != noErr )
				RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory HCopyFile") );	
						
			/* AppleShare's CopyFile clears the isAlias bit, so I still need to attempt to copy
			   the File's attributes to attempt to get things right. */
			MF_FSpCopyFileMgrAttributes(&mSpec, &destSpec, true /* copy lock bit*/);
		} // can copy with HCopyFile
	}  // HCopyFile is available
	
	else {
		Ptr copyBufferPtr = 0;
		bool dstCreated = false;
		short	srcRefNum = 0,			/* 0 when source data and resource fork are closed  */
				dstDataRefNum = 0,		/* 0 when destination data fork is closed */
				dstRsrcRefNum = 0;		/* 0 when destination resource fork is closed */

		try { // copy it ourselves
			long copyBufferSize = kBigCopyBuffSize;
		
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0
			copyBufferPtr = ::NewPtr(copyBufferSize);  
			if (!copyBufferPtr) {
					copyBufferSize = kMinCopyBuffSize;
					copyBufferPtr = ::NewPtr(copyBufferSize);
					if (!copyBufferPtr)
						RAISE_EXCEPTION( oofE_File(*this, memFullErr, "oofFileRef::copyToDirectory out of mem") );	
			}  // alloc small
			 
#else	
			if (!copyBufferPtr) {
				copyBufferPtr = ::NewPtrSys(copyBufferSize); // prefer alloc in sys heap
				if (!copyBufferPtr) {
					copyBufferSize = kMinCopyBuffSize;
					copyBufferPtr = ::NewPtr(copyBufferSize);
					if (!copyBufferPtr)
						RAISE_EXCEPTION( oofE_File(*this, memFullErr, "oofFileRef::copyToDirectory out of mem") );	
				}  // alloc small
			}  // fail alloc big in system heap
#endif		
			/* Open the source data fork. */
			err = MF_HOpenAware(mSpec.vRefNum, mSpec.parID, mSpec.name, srcCopyMode, &srcRefNum);
			if ( err != noErr )
				RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory opening source") );	

			/* Create the destination file. */
			err = MF_HCreateMinimum(destSpec.vRefNum, destSpec.parID, destSpec.name);
			if ( err != noErr )
				RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory creating dest file") );	
			dstCreated = true;	/* After creating the destination file, any
								** error conditions should delete the destination file */
								
			/** An AppleShare dropbox folder is a folder for which the user has the Make Changes
			** privilege (write access), but not See Files (read access) and See Folders (search access).
			** Copying a file into an AppleShare dropbox presents some special problems. Here are the
			** rules we have to follow to copy a file into a dropbox:
			** \li File attributes can be changed only when both forks of a file are empty.
			** \li DeskTop Manager comments can be added to a file only when both forks of a file 
			**   are empty.
			** \li A fork can be opened for write access only when both forks of a file are empty.
			** So, with those rules to live with, we'll do those operations now while both forks
			** are empty. */
		
			const bool isDropBox = ((cipbr.dirInfo.ioACUser & 0x07) == 0x03);
			if (isDropBox) {
				/** We only set the file attributes now if the file is being copied into a
				** drop box. In all other cases, it is better to set the attributes last
				** so that if FileCopy is modified to give up time to other processes
				** periodicly, the Finder won't try to read any bundle information (because
				** the bundle-bit will still be clear) from a partially copied file. If the
				** copy is into a drop box, we have to set the attributes now, but since the
				** destination forks are opened with write/deny-read/deny-write permissions,
				** any Finder that might see the file in the drop box won't be able to open
				** its resource fork until the resource fork is closed.
				**
				** Note: if you do modify FileCopy to give up time to other processes, don't
				** give up time between the time the destination file is created (above) and
				** the time both forks are opened (below). That way, you stand the best chance
				** of making sure the Finder doesn't read a partially copied resource fork.
				*/
				/* Copy attributes but don't lock the destination. */
				err = MF_FSpCopyFileMgrAttributes(&mSpec, &destSpec, false);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory setting file attributes") );	
			}  // dropbox
	
			/* Attempt to copy the comments while both forks are empty.
			** Ignore the result because we really don't care if it worked or not. */
// NOT YET IMPLEMENTED - copying all the MoreFiles stuff to copy comments!
//			DTCopyComment(mSpec.vRefNum, mSpec.parID, mSpec.name, destSpec.vRefNum, destSpec.parID, destSpec.name);

			/* See which forks we need to copy. By doing this, we won't create a data or resource fork
			** for the destination unless it's really needed (some foreign file systems such as
			** the ProDOS File System and Macintosh PC Exchange have to create additional disk
			** structures to support resource forks). */
			Boolean	hasDataFork;
			Boolean	hasResourceFork;
			err = MF_CheckForForks(mSpec.vRefNum, mSpec.parID, mSpec.name, &hasDataFork, &hasResourceFork);
			if ( err != noErr )
				RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory checking forks") );	

	
			if ( hasDataFork )	{
				/* Open the destination data fork. */
				err = MF_HOpenAware(destSpec.vRefNum, destSpec.parID, destSpec.name, dstCopyMode, &dstDataRefNum);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory opening dest data") );	
			}
		
			if ( hasResourceFork )
			{
				/* Open the destination resource fork. */
				err = MF_HOpenRFAware(destSpec.vRefNum, destSpec.parID, destSpec.name, dstCopyMode, &dstRsrcRefNum);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory opening dest resource") );	
			}
		
			if ( hasDataFork )	{
				/* Copy the data fork. */
				err = MF_CopyFork(srcRefNum, dstDataRefNum, copyBufferPtr, copyBufferSize);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory copying data") );	
			
				/* Close both data forks and clear reference numbers. */
				::FSClose(srcRefNum);
				::FSClose(dstDataRefNum);
				srcRefNum = dstDataRefNum = 0;
			}
			else	{
				/* Close the source data fork since it was opened earlier */
				::FSClose(srcRefNum);
				srcRefNum = 0;
			}
		
			if ( hasResourceFork )	{
				/* Open the source resource fork. */
				err = MF_HOpenRFAware(mSpec.vRefNum, mSpec.parID, mSpec.name, srcCopyMode, &srcRefNum);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory opening source resource") );	
			
				/* Copy the resource fork. */
				err = MF_CopyFork(srcRefNum, dstRsrcRefNum, copyBufferPtr, copyBufferSize);
				if ( err != noErr )
					RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::copyToDirectory copying resource") );	
			
				/* Close both resource forks and clear reference numbers. */
				::FSClose(srcRefNum);
				::FSClose(dstRsrcRefNum);
				
				srcRefNum = dstRsrcRefNum = 0;
			}
			if (copyBufferPtr)
				::DisposePtr(copyBufferPtr);  // cleanup is VITAL particularly if use sys heap

			/* Attempt to copy attributes again to set mod date.  Copy lock condition this time
			** since we're done with the copy operation.  This operation will fail if we're copying
			** into an AppleShare dropbox, so we don't check for error conditions. */
			MF_FSpCopyFileMgrAttributes(&mSpec, &destSpec, true);
			flushVolume();
		}
		catch(...) {
			if ( srcRefNum != 0 )
				::FSClose(srcRefNum);		/* Close the source file */
			if ( dstDataRefNum != 0 )
				::FSClose(dstDataRefNum);	/* Close the destination file data fork */
			if ( dstRsrcRefNum != 0 )
				::FSClose(dstRsrcRefNum);	/* Close the destination file resource fork */
			if ( dstCreated )
				::FSpDelete(&destSpec);	/* Delete dest file.  This may fail if the file 
														   is in a "drop folder" */
			if (copyBufferPtr)
				::DisposePtr(copyBufferPtr);  // cleanup is VITAL particularly if use sys heap
			
			flushVolume();
			throw;  // throw exception onwards - app may be able to continue
		}
	} // copied it ourself
}


void 
oofFileRef::moveToDirectory(const oofDirectory& inDir, bool overwriteDest) // _Macintosh
{
	if (dirIsParent(inDir))
		return;  // we're already there!
		
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	const OSErr fileInfoErr = readCInfo(cipbr);
	if (fileInfoErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, fileInfoErr, "oofFileRef::moveToDirectory invalid source file") );	

	if (!inDir.validDir())
		RAISE_EXCEPTION( oofE_InvalidDir(inDir, "oofFileRef::moveToDirectory") );	
		
	FSSpec destSpec;
	inDir.fillInFSSpec(filename(), destSpec);

/*	if (mSpec.vRefNum!=destSpec.vRefNum)  ALWAYS FOR NOW */{  // different volume: copy & delete
		copyToDirectory(inDir, overwriteDest);  // may throw oofE_File
		deleteFile(); 
	}
#if workOutBugInCatMoveWillEnableButSkipForNow
	else {
		OSErr err = ::FSpCatMove(&mSpec, &destSpec);  // ask the OS to move it!
		
		if (err==dupFNErr && overwriteDest) { // try to delete existing file & try again
			oofFileRef destFile(destSpec);
			destFile.deleteFile();  // will throw oofE_FileErr if fails
			err = ::FSpCatMove(&mSpec, &destSpec);
		}
		if (err!=noErr)
			RAISE_EXCEPTION( oofE_File(*this, err, "oofFileRef::moveToDirectory") );	
	}
#endif // temp disabled
}


bool 
oofFileRef::isDirectory() const // _Macintosh
{
	bool ret = false;
	
	if (mIsDir==eDir)
		ret = true;
	else if (mIsDir==eNotSet) {
		CInfoPBRec	cipbr;  // parameter block, union of dir and file info
		OSErr fileInfoErr = readCInfo(cipbr);
		ret = ((fileInfoErr==noErr) && (cipbr.hFileInfo.ioFlAttrib & ioDirMask)); 
	}
	return ret;
}

	
oofFileRef
oofFileRef::makeRefToContainedFile(const oofString& inFile) const // _Macintosh
{
	// we have a combination of vol, dir ID and name pointing at a directory
	// which we need to convert to a dir ID (presumably on the same volume)
	// or if this is not a directory, return an invalid ref
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	Str255 fileName;  // copy our directory's filename for the lookup, as it gets overwritten
	// (bloody horrible Mac OS interfaces which rewrite parameters!!@#$!
	::BlockMoveData(mSpec.name, fileName, mSpec.name[0]+1);
		
	cipbr.hFileInfo.ioVRefNum =  mSpec.vRefNum;	
	cipbr.hFileInfo.ioNamePtr =  fileName;	// buffer to specify/receive name
	cipbr.hFileInfo.ioCompletion = 0;
	cipbr.hFileInfo.ioDirID = mSpec.parID;
	cipbr.hFileInfo.ioFDirIndex = 0;  // direct lookup by ID + name
	OSErr actualDirErr = ::PBGetCatInfoSync(&cipbr);
// NOT YET IMPLEMENTED - decide if this should cope with aliases	
	if ((actualDirErr==noErr) && (cipbr.hFileInfo.ioFlAttrib & ioDirMask))  {  // our current ref is a directory
		inFile.getStr255(fileName);  // can therefore copy name and our current dir ID 
		return oofFileRef(mSpec.vRefNum, cipbr.hFileInfo.ioDirID, fileName);
	}
	else
		return oofFileRef();  // invalid default ref
}
	

/**
	Read contents of file in one hit as string.
	Appends a terminating null to the string that is read so it is a legal oofString.
	Use fopen if you want to read binary data.
	\warning if you attempt to read binary data which contains a null you will
	trigger an assert in oofString because strlen() won't match the length read.
	\todo return oofRefString when we add that class, to save copying buffers
	\todo implement Windows version
*/	
oofString
oofFileRef::readTextFile() const // _Macintosh
{
	short refNum;
	const OSErr openErr = ::FSpOpenDF(&mSpec, fsRdWrShPerm, &refNum);
	if (openErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, openErr, "oofFileRef::readTextFile opening file") );
	
	long fileLen=0;
	char* fileBuf=0;
	const OSErr sizeErr = ::GetEOF(refNum, &fileLen);
	if (sizeErr==noErr) {
		fileBuf = new char[fileLen+1];
		const OSErr readErr = ::FSRead(refNum, &fileLen, fileBuf);
		if (readErr==noErr)
			fileBuf[fileLen] = '\0';  // terminate for use as string
		else
			RAISE_EXCEPTION( oofE_File(*this, readErr, "oofFileRef::readTextFile reading file") );
	}
	else
		RAISE_EXCEPTION( oofE_File(*this, sizeErr, "oofFileRef::readTextFile getting file size") );

	::FSClose(refNum);	
	return oofString(fileBuf, fileLen, true /* adopt buffer */);
}


void
oofFileRef::writeTextFile(oofString& theStr) const // _Macintosh
{
	long writeLen = theStr.length();
	if (writeLen==0)
		return ;
		
	short refNum;
	OSErr openErr = ::FSpOpenDF(&mSpec, fsWrPerm, &refNum);
	
	if (openErr==fnfErr) {
		OSErr createErr = ::HCreate(mSpec.vRefNum, mSpec.parID, mSpec.name, 'R*ch', 'TEXT');
		if (createErr==noErr)
			openErr = ::FSpOpenDF(&mSpec, fsWrPerm, &refNum);
	}
	if (openErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, openErr, "oofFileRef::writeTextFile opening file") );
		
	const OSErr posErr = ::SetFPos(refNum, fsFromStart, 0);
	if (posErr==noErr) {
		const OSErr writeErr = ::FSWrite(refNum, &writeLen, (const char*)theStr);
		if (writeErr!=noErr)
			RAISE_EXCEPTION( oofE_File(*this, writeErr, "oofFileRef::writeTextFile writing") );
		const OSErr resizeErr = ::SetEOF(refNum, writeLen);
		if (resizeErr!=noErr)
			RAISE_EXCEPTION( oofE_File(*this, resizeErr, "oofFileRef::writeTextFile setting file size") );
	}
	else
		RAISE_EXCEPTION( oofE_File(*this, posErr, "oofFileRef::writeTextFile setting file to start") );
	
	::FSClose(refNum);	
	flushVolume();
}
	


void
oofFileRef::flushVolume() const // _Macintosh
{
	const OSErr flushErr = ::FlushVol(NULL, mSpec.vRefNum);
	if (flushErr!=noErr)
		RAISE_EXCEPTION( oofE_File(*this, flushErr, "oofFileRef::flushVolume") );
}
	

oofString
oofFileRef::path() const // _Macintosh
{
	oofDirectory tempDir(mSpec.vRefNum, mSpec.parID);
	oofString ret(tempDir.path(), oofString(mSpec.name) /* convert pstring */);
	return ret;
}


oofString 
oofFileRef::filename() const // _Macintosh
{
	return oofString( spec().name );  // converts pascal string
}


/**
	Get file last modification date/time.
*/
tm
oofFileRef::lastModified() const // _Macintosh
{
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	OSErr fileInfoErr = readCInfo(cipbr);
	if (fileInfoErr==noErr) {
		const long lastModInSecs = cipbr.hFileInfo.ioFlMdDat;
		DateTimeRec dtr;
		::SecondsToDate(lastModInSecs, &dtr);
		tm ret;
		ret.tm_sec = dtr.second;
		ret.tm_min = dtr.minute;
		ret.tm_hour = dtr.hour;
		ret.tm_mday = dtr.day;
		ret.tm_mon = dtr.month - 1;  // months since Jan
		ret.tm_year = dtr.year - 1900;
		ret.tm_wday = dtr.dayOfWeek;
		ret.tm_yday = 0;  // not used
		ret.tm_isdst = 0;  // not Daylight Saving
		return ret;
	}
	else {
		const tm nullTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		return nullTime;
	}
}


/**
	Get file creation date/time.
	\todo move common time translation code in lastModified out to utility method.
*/
tm
oofFileRef::created() const // _Macintosh
{
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	OSErr fileInfoErr = readCInfo(cipbr);
	if (fileInfoErr==noErr) {
		const long lastModInSecs = cipbr.hFileInfo.ioFlCrDat;
		DateTimeRec dtr;
		::SecondsToDate(lastModInSecs, &dtr);
		tm ret;
		ret.tm_sec = dtr.second;
		ret.tm_min = dtr.minute;
		ret.tm_hour = dtr.hour;
		ret.tm_mday = dtr.day;
		ret.tm_mon = dtr.month - 1;  // months since Jan
		ret.tm_year = dtr.year - 1900;
		ret.tm_wday = dtr.dayOfWeek;
		ret.tm_yday = 0;  // not used
		ret.tm_isdst = 0;  // not Daylight Saving
		return ret;
	}
	else {
		const tm nullTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		return nullTime;
	}
}

unsigned long
oofFileRef::fileSize(bool wantPhysical) const // _Macintosh
{
	CInfoPBRec	cipbr;  // parameter block, union of dir and file info
	OSErr fileInfoErr = readCInfo(cipbr);
	unsigned long ret = 0;
	if (fileInfoErr==noErr) {
		if (wantPhysical)
			ret = cipbr.hFileInfo.ioFlRPyLen + cipbr.hFileInfo.ioFlPyLen;
		else
			ret = cipbr.hFileInfo.ioFlRLgLen + cipbr.hFileInfo.ioFlLgLen;
	}
	return ret;
}

#else
	
// common logic for other platforms
void
oofFileRef::extract(ostream& os) const  // anything but _Macintosh
{
	os << mPath;
}


oofDirectory 
oofFileRef::directory() const  // anything but _Macintosh
{
	char ignoredFileName[255];
	return oofDirectory(mPath, ignoredFileName);  // will drop filename in parsing
}


oofFileRef
oofFileRef::makeRefToContainedFile(const oofString& fileName) const   // anything but _Macintosh
{
	assert(isDirectory());
	return oofFileRef(mPath+fileName);  // assumes path ends in delimiter
}
		
		
void
oofFileRef::gotoParentDirectory() const   // anything but _Macintosh
{
	if (mPath.isEmpty())
		return;  //*** early exit with trivial empty case - NO PARENT!

	const long dividerAt = mPath.findLast( oofDirectory::directoryTerminator() );
	// following throw oofE_InvalidDir if fail
	if (dividerAt!=oofString::kNotFound) 
		oofDirectory::gotoDirectory( mPath.subString(0, dividerAt+1  /* length */) );
	else
		oofDirectory::gotoDirectory( mPath );
}


bool 
oofFileRef::dirIsParent(const oofDirectory& inDir) const // anything but _Macintosh
{

	if (mPath.isEmpty())
		return false;  //*** early exit with trivial empty case - NO PARENT!

	bool ret;
	const oofString inPath = inDir.path();
	const long dividerAt = mPath.findLast( oofDirectory::directoryTerminator() );
	if (dividerAt!=oofString::kNotFound) 
		ret = (inPath == mPath.subString(0, dividerAt+1  /* length */) );
	else
		ret = (inPath == mPath);
	return ret;
}


	#if defined(_Windows)

	bool 
	oofFileRef::isValid() const  // _Windows
	{
		struct _stat theStat;
		const int err = _stat(mPath, &theStat);
		const bool ret = (err==0);
		return true;		
	}
	
	
	oofString 
	oofFileRef::filename() const // _Windows
	{
		if (mPath.isEmpty())
			return oofString();  //*** early exit with trivial empty case
	
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
	
		::_splitpath(mPath, drive, dir, fname, ext);
		oofString ret(fname, ext);
		return ret;
	}


	/**
		\todo implement Windows version
	*/
	oofString
	oofFileRef::readTextFile() const // _Windows
	{
	// NOT YET IMPLEMENTED
		return "";
	}
	
	
	void
	oofFileRef::writeTextFile(oofString& inStr) const // _Windows
	{
		HANDLE fh = ::CreateFile(mPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
		if (fh==INVALID_HANDLE_VALUE)
			RAISE_EXCEPTION( oofE_General("oofFileRef::writeTextFile failed to open file") );			

// bizarre but OK to write empty file
		const unsigned long inLen = inStr.length();
		if (inLen) {
			unsigned long lenWritten;
			const bool writeOK = ::WriteFile(fh, inStr.chars(), inLen, &lenWritten, NULL);
			if (!writeOK || lenWritten!=inLen )
				RAISE_EXCEPTION( oofE_General("oofFileRef::writeTextFile failed to write file") );			
		}
		::CloseHandle(fh);
	}
	
	
		
	void 
	oofFileRef::deleteFile(bool ignoreMissingFiles) const   // _Windows
	{
		bool deleteOK;
		if (isDirectory())
			deleteOK = ::RemoveDirectory(mPath);
		else
			deleteOK = ::DeleteFile(mPath);
		if (!deleteOK) {
			const DWORD theErr = ::GetLastError();
			if (theErr==ERROR_FILE_NOT_FOUND) {
				 if (!ignoreMissingFiles)
					RAISE_EXCEPTION( oofE_FileNotFound(*this, "oofFileRef::deleteFile") );
			}
			else
				RAISE_EXCEPTION( oofE_File(*this, "oofFileRef::deleteFile") );	
		}
	}
	
	
	void 
	oofFileRef::renameFile(const oofString& newName)   // _Windows
	{
		if (!::MoveFile(mPath, newName) )
			RAISE_EXCEPTION( oofE_File(*this, "oofFileRef::renameFile") );	
	}



	void 
	oofFileRef::copyToDirectory(const oofDirectory& destDir, bool overwriteDest) const  // _Windows
	{
		if (!destDir.validDir())
			RAISE_EXCEPTION( oofE_InvalidDir(destDir, "oofFileRef::moveToDirectory destination") );	
		
		const oofString newPath (destDir.path(), filename());
		if(!::CopyFile(mPath, newPath, !overwriteDest) )
			RAISE_EXCEPTION( oofE_File(*this, "oofFileRef::copyToDirectory") );	
	}


	void 
	oofFileRef::moveToDirectory(const oofDirectory& destDir, bool overwriteDest)   // _Windows
	{
		if (!destDir.validDir())
			RAISE_EXCEPTION( oofE_InvalidDir(destDir, "oofFileRef::moveToDirectory destination") );	
		
		const oofString newPath (destDir.path(), filename());
		#ifdef MOVEFILE_WRITE_THROUGH
			DWORD flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH ;
		#else
			DWORD flags = MOVEFILE_COPY_ALLOWED;  //compile VC 4
		#endif
		if (overwriteDest)
				flags |= MOVEFILE_REPLACE_EXISTING;
		if (!::MoveFileEx(mPath, newPath, flags) )
			RAISE_EXCEPTION( oofE_File(*this, "oofFileRef::moveToDirectory") );	
	}


	bool 
	oofFileRef::isDirectory() const // _Windows
	{
		bool ret = false;
		
		if (mIsDir==eDir)
			ret = true;
		else if (mIsDir==eNotSet) {
			DWORD fileAttributes = ::GetFileAttributes(mPath);
			ret = (fileAttributes!=0xFFFFFFFF) // failure of call
				  && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}
		return ret;
	}


	tm
	oofFileRef::lastModified() const // _Windows
	{
		struct _stat theStat;
		const int err = _stat(mPath, &theStat);
		if (err==0) {
			const tm* timeBits = localtime(&theStat.st_mtime);
			return *timeBits;  // don't feel happy passing around pointer!		
		}
		else {
			const struct tm nullTime = {0, 0, 0, 0, 0, 0, 0, 0, 0};
			return nullTime;
		}
	}


	tm
	oofFileRef::created() const // _Windows
	{
		struct _stat theStat;
		const int err = _stat(mPath, &theStat);
		if (err==0) {
			const tm* timeBits = localtime(&theStat.st_ctime);
			return *timeBits;  // don't feel happy passing around pointer!		
		}
		else {
			const struct tm nullTime = {0, 0, 0, 0, 0, 0, 0, 0, 0};
			return nullTime;
		}
	}


	unsigned long
	oofFileRef::fileSize(bool /*wantPhysical*/) const // _Windows
	{
		unsigned long ret = 0;
		// NOT YET IMPLEMENTED
		return ret;
	}

	
	
	#elif defined(_Unix)

	bool 
	oofFileRef::isValid() const  // _Unix
	{
		struct stat theStat;
		const int err = ::stat(mPath, &theStat);
		const bool ret = (err==0);
		return true;		
	}
	
	
	oofString 
	oofFileRef::filename() const // _Unix
	{
		if (mPath.isEmpty())
			return oofString();  //*** early exit with trivial empty case
	
		const long dividerAt = mPath.findLast('/');
		if (dividerAt!=oofString::kNotFound) {
			return mPath.subString(dividerAt+1);
		}
		else
			return mPath;
	}


	oofString
	oofFileRef::readTextFile() const  // _Unix
	{
	// NOT YET IMPLEMENTED
		return "";
	}
	
	
	void
	oofFileRef::writeTextFile(oofString& theStr) const // _Unix
	{
	// NOT YET IMPLEMENTED
	}
	
	
	void 
	oofFileRef::deleteFile(bool ignoreMissingFiles) const   // _Unix
	{
		bool ret;
		if (isDirectory())
			ret = ::rmdir(mPath);
		else
			ret = ::unlink(mPath);
		if (! ret)
		{
		#ifdef ENOENT  // this Unix may not have the constants in errno.h
			if (errno == ENOENT) {
				if (!ignoreMissingFiles)
					RAISE_EXCEPTION( oofE_FileNotFound(*this, "oofFileRef::deleteFile") );	
			}
			else  // fall through to RAISE below because it must be some other error
		#endif
				if (!ignoreMissingFiles)
					RAISE_EXCEPTION( oofE_File(*this, "oofFileRef::deleteFile") );	
		}
	}
	
	
	void 
	oofFileRef::renameFile(const oofString& newName)   // _Unix
	{
	// NOT YET IMPLEMENTED
	}



	void 
	oofFileRef::copyToDirectory(const oofDirectory& inDir, bool overwrite/*Dest*/) const  // _Unix
	{
		if (!inDir.validDir())
			RAISE_EXCEPTION( oofE_InvalidDir(inDir, "oofFileRef::moveToDirectory") );	
			
	// NOT YET IMPLEMENTED
	}



	void 
	oofFileRef::moveToDirectory(const oofDirectory& inDir, bool overwrite/*Dest*/)   // _Unix
	{
		if (!inDir.validDir())
			RAISE_EXCEPTION( oofE_InvalidDir(inDir, "oofFileRef::moveToDirectory") );	
		
	// NOT YET IMPLEMENTED
	}



	bool 
	oofFileRef::isDirectory() const // _Unix
	{
		bool ret = false;  // default including for invalid paths
		
		if (mIsDir==eDir)
			ret = true;
		else if (mIsDir==eNotSet) {
			struct stat theStat;
			const int err = ::stat(mPath, &theStat);
			if (err==0) 
				ret = S_ISDIR(theStat.st_mode);
		}
		return ret;
	}


	tm
	oofFileRef::lastModified() const // _Unix
	{
		struct stat theStat;
		const int err = ::stat(mPath, &theStat);
		if (err==0) {
			const tm* timeBits = ::localtime(&theStat.st_mtime);
			return *timeBits;  // don't feel happy passing around pointer!		
		}
		else {
			const tm nullTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			return nullTime;
		}
	}
	

	tm
	oofFileRef::created() const // _Unix
	{
		struct stat theStat;
		const int err = ::stat(mPath, &theStat);
		if (err==0) {
			const tm* timeBits = ::localtime(&theStat.st_ctime);
			return *timeBits;  // don't feel happy passing around pointer!		
		}
		else {
			const tm nullTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			return nullTime;
		}
	}
	

	unsigned long
	oofFileRef::fileSize(bool /*wantPhysical*/) const // _Unix
	{
		unsigned long ret = 0;
		// NOT YET IMPLEMENTED
		return ret;
	}


	#else
		error undefined platform
	#endif
#endif


OOFILE_EXPORT ostream& 
operator<<(ostream& os, const oofFileRef& theRef)
{
	theRef.extract(os);
	return os;
}


// -------------------------------------------------------
//    o o f E _ A l i a s N o t R e s o l v e d
// -------------------------------------------------------
void
oofE_AliasNotResolved::extract(ostream& os) const
{
	os	<< "Alias not resolved with file ref: " <<  mRef;
}


// -------------------------------------------------------
//           o o f E _ I n v a l i d D i r
// -------------------------------------------------------
void
oofE_InvalidDir::extract(ostream& os) const
{
	if (!mErrorString.isEmpty())
		os	<< mErrorString << endl;
	os	<< "Invalid directory: " <<  mDir.path();
}


// -------------------------------------------------------
//           o o f E _ F i l e
// -------------------------------------------------------
#ifdef _Macintosh
		oofE_File::oofE_File(const oofFileRef& f, const OSErr e, const char* extraMsg) : // _Macintosh
			oofE_MacOSErr(e, extraMsg),
			mFile(f)
		{};
#elif defined _Unix
	oofE_File::oofE_File(const oofFileRef& f, const char* extraMsg) : // _Unix
			mFile(f),
			mErrorString(extraMsg)
		{
			if (!extraMsg)
				mErrorString = ::strerror(errno);
		}
#else
	oofE_File::oofE_File(const oofFileRef& f, const char* extraMsg) : // _Windows
			mFile(f),
			mErrorString(extraMsg)
		{}
#endif 


#ifdef _Macintosh
void
oofE_File::extract(ostream& os) const  // _Macintosh
{
	os	<< "Mac OS Error number: " << mErrNum;
	if (!mErrorString.isEmpty())
		os	<< mErrorString << endl;
	os	<< "File Error: " <<  mFile;
}
#else
void
oofE_File::extract(ostream& os) const  // anything but _Macintosh
{
	if (!mErrorString.isEmpty())
		os	<< mErrorString << endl;
	os	<< "File Error: " <<  mFile;
}
#endif

// -------------------------------------------------------
//           o o f E _ F i l e N o t F o u n d
// -------------------------------------------------------
#ifdef _Macintosh
// put Mac version here so don't have to include <Errors.h> in header
// as we always pass fnfError to parent as the error code 
	oofE_FileNotFound::oofE_FileNotFound(const oofFileRef& f, const char* extraMsg) : 
		oofE_File(f, fnfErr, extraMsg)
	{}
#else
	oofE_FileNotFound::oofE_FileNotFound(const oofFileRef& f, const char* extraMsg) : 
		oofE_File(f, extraMsg)
	{}

#endif  // _Macintosh	
	

#ifdef _Macintosh
// -------------------------------------------------------
//          M o r e F i l e s   U t i l i t i e s
// -------------------------------------------------------

OSErr	
MF_CopyFileMgrAttributes(short srcVRefNum,
	long srcDirID,
	ConstStr255Param srcName,
	short dstVRefNum,
	long dstDirID,
	ConstStr255Param dstName,
	Boolean copyLockBit)
{
	OOF_UniversalFMPB pb;
	Str31 tempName;
	OSErr error;
	Boolean objectIsDirectory;

	pb.ciPB.hFileInfo.ioVRefNum = srcVRefNum;
	pb.ciPB.hFileInfo.ioDirID = srcDirID;

	/* Protection against File Sharing problem */
	if ( (srcName == NULL) || (srcName[0] == 0) )
	{
		tempName[0] = 0;
		pb.ciPB.hFileInfo.ioNamePtr = tempName;
		pb.ciPB.hFileInfo.ioFDirIndex = -1;	/* use ioDirID */
	}
	else
	{
		pb.ciPB.hFileInfo.ioNamePtr = (StringPtr)srcName;
		pb.ciPB.hFileInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	}
	error = ::PBGetCatInfoSync(&pb.ciPB);
	if ( error == noErr )
	{
		objectIsDirectory = ( (pb.ciPB.hFileInfo.ioFlAttrib & ioDirMask) != 0 );
		pb.ciPB.hFileInfo.ioVRefNum = dstVRefNum;
		pb.ciPB.hFileInfo.ioDirID = dstDirID;
		if ( (dstName != NULL) && (dstName[0] == 0) )
		{
			pb.ciPB.hFileInfo.ioNamePtr = NULL;
		}
		else
		{
			pb.ciPB.hFileInfo.ioNamePtr = (StringPtr)dstName;
		}
		/* don't copy the hasBeenInited bit */
		pb.ciPB.hFileInfo.ioFlFndrInfo.fdFlags = ( pb.ciPB.hFileInfo.ioFlFndrInfo.fdFlags & 0xfeff );
		error = ::PBSetCatInfoSync(&pb.ciPB);
		if ( (error == noErr) && (copyLockBit) && ((pb.ciPB.hFileInfo.ioFlAttrib & 0x01) != 0) )
		{
			pb.hPB.fileParam.ioFVersNum = 0;
			error = ::PBHSetFLockSync(&pb.hPB);
			if ( (error != noErr) && (objectIsDirectory) )
			{
				error = noErr; /* ignore lock errors if destination is directory */
			}
		}
	}
	return ( error );
}


OSErr	
MF_FSpCopyFileMgrAttributes(const FSSpec* srcSpec, const FSSpec* dstSpec, Boolean copyLockBit)
{
	const OSErr ret = MF_CopyFileMgrAttributes(
		srcSpec->vRefNum, srcSpec->parID, srcSpec->name,
		dstSpec->vRefNum, dstSpec->parID, dstSpec->name,
		copyLockBit
	);
	return ret;
}


OSErr	
MF_CheckForForks(short vRefNum,
							  long dirID,
							  ConstStr255Param name,
							  Boolean *hasDataFork,
							  Boolean *hasResourceFork)
{
	HParamBlockRec pb;
	OSErr error;
	
	pb.fileParam.ioNamePtr = (StringPtr)name;
	pb.fileParam.ioVRefNum = vRefNum;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioDirID = dirID;
	pb.fileParam.ioFDirIndex = 0;
	error = ::PBHGetFInfoSync(&pb);
	*hasDataFork = (pb.fileParam.ioFlLgLen != 0);
	*hasResourceFork = (pb.fileParam.ioFlRLgLen != 0);
	
	return ( error );
}

/*****************************************************************************/

OSErr	
MF_CopyFork(short srcRefNum,
	short dstRefNum,
	void *copyBufferPtr,
	long copyBufferSize)
{
	ParamBlockRec srcPB;
	ParamBlockRec dstPB;
	OSErr srcError;
	OSErr dstError;

	if ( (copyBufferPtr == NULL) || (copyBufferSize == 0) )
		return ( paramErr );
	
	srcPB.ioParam.ioRefNum = srcRefNum;
	dstPB.ioParam.ioRefNum = dstRefNum;

	/* preallocate the destination fork and */
	/* ensure the destination fork's EOF is correct after the copy */
	srcError = ::PBGetEOFSync(&srcPB);
	if ( srcError != noErr )
		return ( srcError );
	dstPB.ioParam.ioMisc = srcPB.ioParam.ioMisc;
	dstError = ::PBSetEOFSync(&dstPB);
	if ( dstError != noErr )
		return ( dstError );

	/* reset source fork's mark */
	srcPB.ioParam.ioPosMode = fsFromStart;
	srcPB.ioParam.ioPosOffset = 0;
	srcError = ::PBSetFPosSync(&srcPB);
	if ( srcError != noErr )
		return ( srcError );

	/* reset destination fork's mark */
	dstPB.ioParam.ioPosMode = fsFromStart;
	dstPB.ioParam.ioPosOffset = 0;
	dstError = ::PBSetFPosSync(&dstPB);
	if ( dstError != noErr )
		return ( dstError );

	/* set up fields that won't change in the loop */
	srcPB.ioParam.ioBuffer = (Ptr)copyBufferPtr;
	srcPB.ioParam.ioPosMode = fsAtMark + 0x0020;/* fsAtMark + noCacheBit */
	/* If copyBufferSize is greater than 512 bytes, make it a multiple of 512 bytes */
	/* This will make writes on local volumes faster */
	if ( (copyBufferSize >= 512) && ((copyBufferSize & 0x1ff) != 0) )
	{
		srcPB.ioParam.ioReqCount = copyBufferSize & 0xfffffe00;
	}
	else
	{
		srcPB.ioParam.ioReqCount = copyBufferSize;
	}
	dstPB.ioParam.ioBuffer = (Ptr)copyBufferPtr;
	dstPB.ioParam.ioPosMode = fsAtMark + 0x0020;/* fsAtMark + noCacheBit */

	while ( (srcError == noErr) && (dstError == noErr) )
	{
		srcError = ::PBReadSync(&srcPB);
		dstPB.ioParam.ioReqCount = srcPB.ioParam.ioActCount;
		dstError = ::PBWriteSync(&dstPB);
	}

	/* make sure there were no errors at the destination */
	if ( dstError != noErr )
		return ( dstError );

	/* make sure the only error at the source was eofErr */
	if ( srcError != eofErr )
		return ( srcError );

	return ( noErr );
}


OSErr	
MF_HGetVolParms(ConstStr255Param volName,
	short vRefNum,
	GetVolParmsInfoBuffer *volParmsInfo,
	long *infoSize)
{
	HParamBlockRec pb;
	OSErr error;

	pb.ioParam.ioNamePtr = (StringPtr)volName;
	pb.ioParam.ioVRefNum = vRefNum;
	pb.ioParam.ioBuffer = (Ptr)volParmsInfo;
	pb.ioParam.ioReqCount = *infoSize;
	error = ::PBHGetVolParmsSync(&pb);
	if ( error == noErr ) {
		*infoSize = pb.ioParam.ioActCount;
	}
	return ( error );
}


OSErr	
MF_HCreateMinimum(short vRefNum,  long dirID,  ConstStr255Param fileName)
{
	HParamBlockRec pb;

	pb.fileParam.ioNamePtr = (StringPtr)fileName;
	pb.fileParam.ioVRefNum = vRefNum;
	pb.ioParam.ioVersNum = 0;
	pb.fileParam.ioDirID = dirID;
	return ( ::PBHCreateSync(&pb) );
}


OSErr	
MF_HCopyFile(short srcVRefNum,
						  long srcDirID,
						  ConstStr255Param srcName,
						  short dstVRefNum,
						  long dstDirID,
						  ConstStr255Param dstPathname,
						  ConstStr255Param copyName)
{
	HParamBlockRec pb;

	pb.copyParam.ioVRefNum = srcVRefNum;
	pb.copyParam.ioDirID = srcDirID;
	pb.copyParam.ioNamePtr = (StringPtr)srcName;
	pb.copyParam.ioDstVRefNum = dstVRefNum;
	pb.copyParam.ioNewDirID = dstDirID;
	pb.copyParam.ioNewName = (StringPtr)dstPathname;
	pb.copyParam.ioCopyName = (StringPtr)copyName;
	return ( PBHCopyFileSync(&pb) );
}


OSErr	
MF_HOpenAware(short vRefNum,
						   long dirID,
						   ConstStr255Param fileName,
						   short denyModes,
						   short *refNum)
{
	HParamBlockRec pb;
	OSErr error;
	GetVolParmsInfoBuffer volParmsInfo;
	long infoSize = sizeof(GetVolParmsInfoBuffer);

	pb.ioParam.ioMisc = NULL;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioNamePtr = (StringPtr)fileName;
	pb.fileParam.ioVRefNum = vRefNum;
	pb.fileParam.ioDirID = dirID;

	/* get volume attributes */
	/* this preflighting is needed because Foreign File Access based file systems don't */
	/* return the correct error result to the OpenDeny call */
	error = MF_HGetVolParms(fileName, vRefNum, &volParmsInfo, &infoSize);
	if ( (error == noErr) && hasOpenDeny(volParmsInfo) )
	{
		/* if volume supports OpenDeny, use it and return */
			pb.accessParam.ioDenyModes = denyModes;
			error = PBHOpenDenySync(&pb);
			*refNum = pb.ioParam.ioRefNum;
	}
	else if ( (error == noErr) || (error == paramErr) )	/* paramErr is OK, it just means this volume doesn't support GetVolParms */
	{
		/* OpenDeny isn't supported, so try File Manager Open functions */
		
		/* If request includes write permission, then see if the volume is */
		/* locked by hardware or software. The HFS file system doesn't check */
		/* for this when a file is opened - you only find out later when you */
		/* try to write and the write fails with a wPrErr or a vLckdErr. */
		
		if ( (denyModes & dmWr) != 0 )
		{
			error = MF_CheckVolLock(fileName, vRefNum);
		}
		else
		{
			error = noErr;
		}
		
		if ( error == noErr )
		{
			/* Set File Manager permissions to closest thing possible */
			if ( (denyModes == dmWr) || (denyModes == dmRdWr) )
			{
				pb.ioParam.ioPermssn = fsRdWrShPerm;
			}
			else
			{
				pb.ioParam.ioPermssn = denyModes % 4;
			}

			error = PBHOpenDFSync(&pb);				/* Try OpenDF */
			if ( error == paramErr )
			{
				error = PBHOpenSync(&pb);			/* OpenDF not supported, so try Open */
			}
			*refNum = pb.ioParam.ioRefNum;
		}
	}
	
	return ( error );
}


OSErr	
MF_HOpenRFAware(short vRefNum,
							 long dirID,
							 ConstStr255Param fileName,
							 short denyModes,
							 short *refNum)
{
	HParamBlockRec pb;
	OSErr error;
	GetVolParmsInfoBuffer volParmsInfo;
	long infoSize = sizeof(GetVolParmsInfoBuffer);

	pb.ioParam.ioMisc = NULL;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioNamePtr = (StringPtr)fileName;
	pb.fileParam.ioVRefNum = vRefNum;
	pb.fileParam.ioDirID = dirID;

	/* get volume attributes */
	/* this preflighting is needed because Foreign File Access based file systems don't */
	/* return the correct error result to the OpenRFDeny call */
	error = MF_HGetVolParms(fileName, vRefNum, &volParmsInfo, &infoSize);
	if ( (error == noErr) && hasOpenDeny(volParmsInfo) )
	{
		/* if volume supports OpenRFDeny, use it and return */
		if ( hasOpenDeny(volParmsInfo) )
		{
			pb.accessParam.ioDenyModes = denyModes;
			error = PBHOpenRFDenySync(&pb);
			*refNum = pb.ioParam.ioRefNum;
		}
	}
	else if ( (error == noErr) || (error == paramErr) )	/* paramErr is OK, it just means this volume doesn't support GetVolParms */
	{
		/* OpenRFDeny isn't supported, so try File Manager OpenRF function */
		
		/* If request includes write permission, then see if the volume is */
		/* locked by hardware or software. The HFS file system doesn't check */
		/* for this when a file is opened - you only find out later when you */
		/* try to write and the write fails with a wPrErr or a vLckdErr. */
		
		if ( (denyModes & dmWr) != 0 )
		{
			error = MF_CheckVolLock(fileName, vRefNum);
		}
		else
		{
			error = noErr;
		}
		
		if ( error == noErr )
		{
			/* Set File Manager permissions to closest thing possible */
			if ( (denyModes == dmWr) || (denyModes == dmRdWr) )
			{
				pb.ioParam.ioPermssn = fsRdWrShPerm;
			}
			else
			{
				pb.ioParam.ioPermssn = denyModes % 4;
			}

			error = PBHOpenRFSync(&pb);
			*refNum = pb.ioParam.ioRefNum;
		}
	}

	return ( error );
}


OSErr	
MF_CheckVolLock(ConstStr255Param pathname,
							 short vRefNum)
{
	HParamBlockRec pb;
	OSErr error;

	error = MF_GetVolumeInfoNoName(pathname,vRefNum, &pb);
	if ( error == noErr )
	{
		if ( (pb.volumeParam.ioVAtrb & 0x0080) != 0 )
		{
			error = wPrErr;		/* volume locked by hardware */
		}
		else if ( (pb.volumeParam.ioVAtrb & 0x8000) != 0 )
		{
			error = vLckdErr;	/* volume locked by software */
		}
	}
	
	return ( error );
}


OSErr	
MF_GetVolumeInfoNoName(ConstStr255Param pathname,
									short vRefNum,
									HParmBlkPtr pb)
{
	Str255 tempPathname;
	OSErr error;
	
	/* Make sure pb parameter is not NULL */ 
	if ( pb != NULL )
	{
		pb->volumeParam.ioVRefNum = vRefNum;
		if ( pathname == NULL )
		{
			pb->volumeParam.ioNamePtr = NULL;
			pb->volumeParam.ioVolIndex = 0;		/* use ioVRefNum only */
		}
		else
		{
			BlockMoveData(pathname, tempPathname, pathname[0] + 1);	/* make a copy of the string and */
			pb->volumeParam.ioNamePtr = (StringPtr)tempPathname;	/* use the copy so original isn't trashed */
			pb->volumeParam.ioVolIndex = -1;	/* use ioNamePtr/ioVRefNum combination */
		}
		error = PBHGetVInfoSync(pb);
		pb->volumeParam.ioNamePtr = NULL;	/* ioNamePtr may point to local	tempPathname, so don't return it */
	}
	else
	{
		error = paramErr;
	}
	return ( error );
}


#endif  // _Macintosh
