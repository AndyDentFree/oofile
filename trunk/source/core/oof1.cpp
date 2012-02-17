// COPYRIGHT 1994 A.D. Software, All rights reserved

// public layer of OOFILE database

#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOF2
	#include "oof2.h"
#endif
#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h" 
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFRAM
	#include "oofram.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#ifndef _CTYPE_H
	#include <ctype.h>
#endif

// Mac toolbox includes if not using precompiled header
#ifdef _Macintosh
	#ifndef __DIALOGS__
		#include <Dialogs.h>
	#endif
	#ifndef __SOUND__
		#include <Sound.h>  // for SysBeep
	#endif
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE  
#endif

#ifdef _Macintosh

/**
	error reporting alert number.
	if you have not included oofmac.rsrc or have changed the number
	of the alert you will need to fix the following
	\ingroup oofMacros
*/
	#define kBigErrAlert  1001

	/**
		Word-wrapping message box.
		Uses traditional Mac alert defined by kBigErrAlert.
		\ingroup oofMacSpecific
	*/
	class macAlertMsg {
	public:
		macAlertMsg(const char*);
// use default copy ctor
	private:
		int FindWordBreakBefore(int bitLen, const char* msg, int &nextWordStart);
	};
#endif

// define static variables
dbConnect* dbConnect::sCurrentlyConstructing;
bool dbConnect::sTruncateTrailingSpaces = false;
oofString dbConnect::sSchemaFileExt ="osc";
#ifdef OOF_DEBUG_LOG_NAME
ofstream dbConnect::sDebugLog;	// open and close bracketing as we log
oofDirectory dbConnect::sDebugDir;	// so same log file used regardless of current dir
#endif	

dbTable* dbTable::sCurrentlyConstructing;
dbGUIFactory* dbGUIFactory::sGUIFactory;

dbTable::selSharingT OOF_dbTableStatics::sCloningSelSharing = dbTable::selCopyOnWrite;
bool OOF_dbTableStatics::sCloning = false;
bool OOF_dbTableStatics::sCloningInDifferentConnection = false;

// error reporting - Singleton
oofErrorReporter	oofErrorReporter::sDefaultReporter;	// concrete object itself
unsigned short		oofErrorReporter::sNumReporters=0;	// instance counter
oofErrorReporter*	oofErrorReporter::sUserSpecifiedReporter=0;	// optional override

bool 
skipTillDigit(istream& is, char fieldSep, char recSep)
{
	const char peekCh = is.peek();
	if ((peekCh==fieldSep) || (peekCh==recSep))
		return false;  // early exit - empty line

	while (is.good()) {
		char c = is.peek();
		if ((c>='0') && (c<='9')) {
			return true;
		}
		is >> c;	// skip the char
	}
	return false;
}


bool 
skipTillNumber(istream& is, char fieldSep, char recSep, bool canStartWithDecimal)
{
	const char peekCh = is.peek();
	if ((peekCh==fieldSep) || (peekCh==recSep))
		return false;  // early exit - empty line

	while (is.good()) {
		char c = is.peek();
		if ((c>='0') && (c<='9') || (c=='-') || (c=='-') || (canStartWithDecimal && (c=='.'))) {
			return true;
		}
		is >> c;
	}
	return false;
}


bool 
skipRestOfField(istream& is, char fieldSep, char recSep)
{
	while (is.good()) {
		char c = is.peek();
		if ((c==fieldSep) || (c==recSep)) {
			return true;  // OK - hit a separator
		}
		is >> c;	// skip the char
	}
	return false;
}


// -------------------------------------------------------
//              o o f E r r o r R e p o r t e r
// -------------------------------------------------------
oofErrorReporter::oofErrorReporter()
{
	sNumReporters++;	// static default will be number 1
}


oofErrorReporter::~oofErrorReporter()
{
	sNumReporters--;
	if (sNumReporters==0)	// deleting static default
		delete sUserSpecifiedReporter;	// don't leak at application level!
}


oofErrorReporter*  
oofErrorReporter::defaultReporter()
{
	oofErrorReporter* ret;
	if (sUserSpecifiedReporter)
		ret = sUserSpecifiedReporter;
	else
		ret = &sDefaultReporter;
	return ret;
}


void 
oofErrorReporter::setDefaultReporter(oofErrorReporter* adoptedReporter)
{
	delete sUserSpecifiedReporter;
	sUserSpecifiedReporter = adoptedReporter;
}


void 
oofErrorReporter::raise(ostream& os, bool terminateAfterMsg)
{
	oofString msg(os);
	msg.replaceChar('\n', '\r');

// now display the message
#ifdef _Windows
	::MessageBox(0, msg, "OOFILE Error", MB_ICONSTOP);
#else
	#ifdef _Macintosh
		macAlertMsg x(msg);
	#else
		cout << msg << endl;
		cout << "Press Return to continue";
		char c;
		cin.flags(cin.flags() & ~ios::skipws);
		cin >> c;
	#endif
#endif
	MaybeTerminate(terminateAfterMsg);
}


void 
oofErrorReporter::raise(const char* str, bool terminateAfterMsg)
{
	#ifdef _Windows
		::MessageBox(0, str, "OOFILE Error", MB_ICONSTOP);
	#else
		#ifdef _Macintosh
			oofString msg(str);
			msg.replaceChar('\n', '\r');
			macAlertMsg x(msg);
		#else		
			cout << str << endl;
			cout << "Press Return to continue";
			char c;
			cin.flags(cin.flags() & ~ios::skipws);
			cin >> c;
		#endif
	#endif
	MaybeTerminate(terminateAfterMsg);
}


void 
oofErrorReporter::MaybeTerminate(bool terminateAfterMsg)
{
	if (terminateAfterMsg)
		exit(0);
		//abort();
}


// -------------------------------------------------------
//           o o f C G I E r r o r R e p o r t e r
// -------------------------------------------------------
oofCGIErrorReporter::oofCGIErrorReporter(ostream& os) :
	mStream(os)
{
}


oofCGIErrorReporter::~oofCGIErrorReporter() 
{
}


void 
oofCGIErrorReporter::raise(const char* str, bool terminateAfterMsg)
{
	Header();
	mStream << str;
	Footer();
	MaybeTerminate(terminateAfterMsg);
}

void 
oofCGIErrorReporter::raise(ostream& os, bool terminateAfterMsg)
{
	Header();
	#if !OOF_USE_ANSI_STREAMS && defined(__MWERKS__)
		// old Metrowerks lacked an operator<<(streambuf*)
		mStream << *(os.rdbuf()) ;
	#else
		mStream << os.rdbuf() ;
	#endif
	Footer();
	MaybeTerminate(terminateAfterMsg);
}


void 
oofCGIErrorReporter::Header()
{
	mStream << "Content-TYPE: text/html\n\n";
	mStream << "<HTML><HEAD>\n";
	mStream << "</HEAD>\n<BODY>\n";

	mStream << "<PRE>\n";
}


void 
oofCGIErrorReporter::Footer()
{
	mStream << "\n</PRE>\n";
	mStream << "</BODY></HTML>\n";
}


// -------------------------------------------------------
//               O O F I L E _ P u b l i c B a s e
// -------------------------------------------------------

/**
	Provide human-readable information about object.
	Overriden to provide much more detail particularly for
	dbTable::describe() which provides full schema. 
*/
void 
OOF_PublicBase::describe(ostream& os) const
{
	os << name();
}


oofString
OOF_PublicBase::name() const
{
	return oofString("unnamed OOFILE object");
}


// -------------------------------------------------------
//                      d b C o n n e c t
// -------------------------------------------------------
dbConnect::dbConnect(bool isTemporary) :
					mTables(4, 4),  /* go up in chunks of 4 tables at a time */ 
					mOpen(false),
					mTempDatabase(isTemporary),
					mDefaultPrototypeParser(0),
					mDatabaseDir(0)
{
	sCurrentlyConstructing = this;
// FOLLOWING IS PORTABILITY CHECK
// we are considerably rooted if this fails - we make heavy use of 
// longs to store pointers to objects	
	assert(sizeof(unsigned long) == sizeof(void*));
}


dbConnect::~dbConnect() 
{
	if (mDefaultPrototypeParser)
		mDefaultPrototypeParser->decRefs();
	delete mDatabaseDir;

	if (mOpen) {	
// assume we own our tables (if they are deleted before us they call close()
// if you have dbTables as concrete members of a subclass of dbConnect they
// will have been deleted before reaching this base dtor, so this is safe
		const unsigned int numTables=mTables.count();
		mTables.start();
		for (unsigned int i=0; i<numTables; i++) {
			dbTable* theTable = (dbTable*) mTables.value(i);	// safe downcast
			theTable->mConnection = 0;  // prevent table trying to close us!
			delete theTable;
			mTables.next();
		}
	}
}


void 
dbConnect::attachTable(dbTable* theTable) 
{
	theTable->mTableNumber = mTables.count();
	mTables.append(theTable);
	theTable->mBackend = MakeTableBackend(theTable);
	theTable->mConnection = this;
}


oofString 
dbConnect::name() const
{
	return connectionName();
}


void
dbConnect::MakeAllTablesValid() const
{
// note using value like this, instead of mTables.start more next is const
// on mTables and hence const on us
	const unsigned short numTables = mTables.count();
	for (unsigned short i=0; i<numTables; i++) {
		dbTable* theTable = (dbTable*) mTables.value(i);		// safe downcast
		theTable->setTableValid(true);
	}
	// reset global to detect fields declared outside tables
	// got caught once due to a missing oofDate::operator=(char*) which invoked a dbDate(char*)
	// ctor to create temporary rhs for oofDate::operator=(dbDate&)
	dbTable::sCurrentlyConstructing = 0;
}


dbTable*
dbConnect::table(unsigned int tableNum) const
{
	return (dbTable*) mTables.value(tableNum);
}


dbTable*
dbConnect::table(const char* tableName)
{
	return (dbTable*) mTables[tableName];	
}


bool 
dbConnect::usesSeparateStorageFiles() const
{
	return false;  // assume monolithic file (or server connection)
}

/**
	produce a file name for schema file.
	If a file name exists for the data base use it with the extension 
	sSchemaFileExt (default "osc") else use the connection name with 
	sSchemaFileExt appended.
*/
oofString
dbConnect::MakeSchemaFileName()
{
/*
commented out David stuff to stop warning messages
	unsigned long lowerPos, upperLim, index;
	
	oofString schemaFileName;
	if(mSchemaFileName.length() == 0)
		schemaFileName = mConnectionName;
	else
		schemaFileName = mSchemaFileName;
//	static char* relativePathMac(char*, const unsigned long& ioLen, bool appendSeparator= true);
	
	char * sName = oofPathString::relativePathMac((char*)schemaFileName, schemaFileName.length() ,false);
	if(sName)
		schemaFileName.adopt(sName);

	upperLim = schemaFileName.length();

	index = 1;
	while((schemaFileName[index++] != ':')&& (index <= upperLim))
		;
	

	schemaFileName = 	schemaFileName.subString(1, index-2);
		
	schemaFileName += ".";
	schemaFileName +=sSchemaFileExt;
	return schemaFileName;
*/
	return "";
}	


/**
	Write the oofschema out to the schema file.
	\note temporarily changes current directory whilst writing file.
*/
void	   
dbConnect::WriteOOFSchemaFile()
{
	stSaveDirectory willReturnToCurrentDir;
	gotoDatabaseDir();
	oofString schemaFileName = MakeSchemaFileName();
	ofstream schemaFile;
	schemaFile.open(schemaFileName.chars(), ios::out);	
	schemaFile << "oofschemaVer1.0";
	describe(schemaFile);

	schemaFile.close();
}

/**
	Produce  a string containing the oofschema with the length of the
	schema string aphended to the end.
	The length is used as an offset in dbConnect_rampBackend to find the schema beginning 
*/
oofString
dbConnect::MakeOOFSchemaString()
{
	//osstream theSchema;
	ostringstream theSchema;
	theSchema << "oofschemaVer1.0";
	describe(theSchema);
	//oofString = theSchema.str().c_str();// find out does c_string orhan buffer
	oofString ret = theSchema.str();// find out does  str()) orhan buffer
	ret.convertNumber(ret.length()+8, "%8lx",true);

	return ret;
}

void
dbConnect::writeOOFSchema()
{
	WriteOOFSchemaFile();
}


/**
	Reads & parses oofSchema from the file schemaFileName.
	Populates the dbConnect::sCurrentlyConstructing connection with
	extra tables. 
	\todo make a version that takes FSSpec or oofFileRef
*/
void 	
dbConnect::readOOFSchema(const oofString& /*schemaFileName*/)
{
	assert(sCurrentlyConstructing); //so cannot read after connected
	
/*
	oofSchemaParser* parser = new oofSchemaParser;

	parser->loadSchema(schemaFileName);
	parser->parseSchemaText();
*/	
}


void 
dbConnect::describe(ostream& os) const
{
	os << "Connection:\t" << mConnectionName << endl;
	mTables.describe(os);
}


void 
dbConnect::stats(ostream& os)
{
	const unsigned int numEntries = mTables.count();
	for (unsigned int i=0; i<numEntries; i++) {
		dbTable* theTable = (dbTable*) mTables.value(i);		// safe downcast
		theTable->stats(os);
	}
	os << endl;
}


void 
dbConnect::suspendSorting() const
{
	const unsigned int numEntries = mTables.count();
	for (unsigned int i=0; i<numEntries; i++) {
		dbTable* theTable = (dbTable*) mTables.value(i);		// safe downcast
		theTable->suspendSorting();
	}
}


void 
dbConnect::resumeSorting() const
{
	const unsigned int numEntries = mTables.count();
	for (unsigned int i=0; i<numEntries; i++) {
		dbTable* theTable = (dbTable*) mTables.value(i);		// safe downcast
		theTable->resumeSorting();
	}
}


void 
dbConnect::deleteAll()
{
   const unsigned int numTables = mTables.count();
	for (unsigned int i=0; i<numTables; i++)	{
			dbTable* theTable = (dbTable*) mTables[i];  // safe downcast
			theTable->deleteAll();
	}
}


void 
dbConnect::deleteStorage()
{
	close();
	const unsigned int numTables = mTables.count();
	for (unsigned int i=0; i<numTables; i++) {
		dbTable* theTable = (dbTable*) mTables[i];  // safe downcast
		theTable->deleteStorage();
	}
	if (usesSeparateStorageFiles()) {
		assert(mDatabaseDir);
		if (mDatabaseDir->createdDiskDir() && mDatabaseDir->isEmpty())
			mDatabaseDir->delDirectory(true);  // was ours and nobody else using it!
	}
}


void 
dbConnect::copyAllFrom(dbConnect* rhs)
{
	unsigned int numTables = mTables.count();
	const unsigned int rhsTables = rhs->mTables.count();
	if (numTables > rhsTables)
		numTables = rhsTables;	// copy smaller number of tables
	for (unsigned int i=0; i<numTables; i++)	{
		dbTable* lhsTable = (dbTable*) mTables[i];  // safe downcast
		dbTable* rhsTable = (dbTable*) rhs->mTables[i];  // safe downcast
		lhsTable->copyAllFrom(rhsTable);
	}
}

void 
dbConnect::dumpData(ostream& os)
{
	os << endl;
#ifdef OOF_EXTRACT_HIDDEN
   const unsigned int numTables = mTables.count();
	for (unsigned int i=0; i<numTables; i++)	{
		dbTable* theTable = table(i) ;
#else
	for (mTables.startVisible(); mTables.moreVisible(); mTables.nextVisible()) {
		dbTable* theTable = table(mTables.iter()) ;
#endif	
		theTable->selectAll();
		os << "table:\t" << theTable->tableName() << endl; 
		theTable->extract(os);
		os << endl << endl << endl;  
	}
}


void 
dbConnect::generateTestData(unsigned long maxRecs, bool generateRandom, unsigned long seedOrCount)
{
   	const unsigned int numTables = mTables.count();
	for (unsigned int i=0; i<numTables; i++)
		table(i)->generateTestData(maxRecs, generateRandom, seedOrCount);
}


void
dbConnect::gotoDatabaseDir() const
{
	assert(mDatabaseDir);	// responsibility of our subclasses to have set this in their openConnection and newConnection
	mDatabaseDir->gotoDirectory();
}


bool 
dbConnect::fileExists(const char* fName)
{
	const bool ret = oofDirectory::fileExists(fName);
	return ret;
}


void 
dbConnect::raise(ostream& os, bool terminateAfterMsg)
{
	oofErrorReporter::defaultReporter()->raise(os, terminateAfterMsg);
}


void 
dbConnect::raise(const char* str, bool terminateAfterMsg)
{
	oofErrorReporter::defaultReporter()->raise(str, terminateAfterMsg);
}


#ifndef OOF_USING_EXCEPTIONS
// implement conditionally to avoid people calling directly when 
// they should be using the RAISE_EXCEPTION macro instead
void
dbConnect::raise(const oofE_Base& inException, bool terminateAfterMsg)
{
	stringstream os;
	inException.extract(os);
	raise(os, terminateAfterMsg);
}
#endif

void 
dbConnect::schemaFileExt(const oofString& newExt)
{
	sSchemaFileExt = newExt;
}

/**
	Manage state transitions for FinishOpenOrCreateConnection.
	\param inCreatedOrOpened is true if just created file, false if opened
*/
void
dbConnect::OpenOrCreateStateTransition(bool inOpenedOrCreated)
{	
	if((inOpenedOrCreated && (mFileState == eOpenedFile)) || (!inOpenedOrCreated && (mFileState == eCreatedFile)))
		return;
	else if(inOpenedOrCreated && ( mFileState == eStart ))
		mFileState = eOpenedFile;
	else if(!inOpenedOrCreated && ( mFileState == eStart ))
		 mFileState = eCreatedFile;
	else if  (inOpenedOrCreated && ( mFileState == eCreatedFile ))
		mFileState = eInvalidlyOpenInCreation;
	else if(!inOpenedOrCreated && ( mFileState == eOpenedFile ))
		 mFileState = eInvalidlyCreationInOpen;
	
}

void
dbConnect::InitOpenOrCreateState()
{
	mFileState = eStart;
}

oofWordParser*
dbConnect::getDefaultWordParser()
{
	if	(!mDefaultPrototypeParser) {
		mDefaultPrototypeParser = new oofWordParser;
	}
	return mDefaultPrototypeParser;
}


void 
dbConnect::setDefaultWordParser(oofWordParser* inParser)
{
	delete mDefaultPrototypeParser;
	mDefaultPrototypeParser = inParser;
}




#ifdef OOF_DEBUG_LOG_NAME
void
dbConnect::startDebugWrite()
{
	stSaveDirectory preserveUserDir;
	
	sDebugDir.gotoDirectory();	// wherever we were when app started
	sDebugLog.open(OOF_DEBUG_LOG_NAME, ios::app | ios::out);	// now will always open in same place!
	assert(sDebugLog.good());
}


void
dbConnect::endDebugWrite()
{
	sDebugLog.flush();
	sDebugLog.close();
}


void			
dbConnect::logComment(const char* str)
{
	startDebugWrite();
	debugLog() << str << endl << endl;
	endDebugWrite(); 
}


void			
dbConnect::logComment(ostream& os)
{
	startDebugWrite();
#if define(__MWERKS__) && !OOF_USE_ANSI_STREAMS
		// old Metrowerks lacked an operator<<(streambuf*)
	debugLog() << *(os.rdbuf()) << endl << endl;
#else
	debugLog() << os.rdbuf() << endl << endl;
#endif	
	endDebugWrite(); 
}


void
dbConnect::LogConnection(const char* comment, const char* connectionName) 
{
	startDebugWrite();
	if (connectionName)
		debugLog() << comment << "Connection: '" << connectionName << "'\t";
	else
		debugLog() << comment << "Connection: unnamed\t";
	debugLog() << hex << this << dec << endl << endl;
	endDebugWrite();
}


#endif

// -------------------------------------------------------
//               m a c A l e r t M s g
// -------------------------------------------------------
#ifdef _Macintosh
macAlertMsg::macAlertMsg(const char* constMsg)
{
	Str255 s0, s1, s2, s3;
	s0[0] = s1[0] = s2[0] = s3[0] = 0;
	int lenToNextStart, lenToWordBreak, bitLen;
	
	char* msg = (char*) constMsg;  // cast away const as some of the toolbox routines aren't const
	                               // but *are* safe, so we are preserving the intent
	long msgLen = strlen(msg);

	if (msgLen>255) {
		lenToWordBreak = FindWordBreakBefore(255, msg, lenToNextStart);
		msgLen -= (lenToNextStart+1);
	}
	else {
		lenToWordBreak = msgLen;
		msgLen = 0;
	}
		
	memcpy(&s0[1], msg, lenToWordBreak);
	s0[0] = lenToWordBreak;
	
	if (msgLen>0) {
		msg += lenToNextStart; 
		bitLen = (msgLen>255) ? 255 : msgLen+1;  // point to nil byte at end of string
		lenToWordBreak = FindWordBreakBefore(bitLen, msg, lenToNextStart);
		memcpy(&s1[1], msg, lenToWordBreak);
		s1[0] = lenToWordBreak;
		msgLen -= (lenToNextStart+1);
	}
	
	if (msgLen>0) {
		msg += lenToNextStart; 
		bitLen = (msgLen>255) ? 255 : msgLen+1;
		lenToWordBreak = FindWordBreakBefore(bitLen, msg, lenToNextStart);
		memcpy(&s2[1], msg, lenToWordBreak);
		s2[0] = lenToWordBreak;
		msgLen -= (lenToNextStart+1);
	}
	
	if (msgLen>0) {
		msg += lenToNextStart; 
		bitLen = (msgLen>255) ? 255 : msgLen+1;
		lenToWordBreak = FindWordBreakBefore(bitLen, msg, lenToNextStart);
		memcpy(&s3[1], msg, lenToWordBreak);
		s3[0] = (lenToNextStart+1);
	}
	
	ParamText(s0, s1, s2, s3);
	StopAlert(kBigErrAlert, nil);
}


int 
macAlertMsg::FindWordBreakBefore(int bitLen, const char* msg, int& lenToNextStart)
{
	const char* scanFrom = msg+bitLen;
	lenToNextStart = bitLen;   // as far as we know - we aren't allowed to skip right
	const char* const endWord = scanFrom;  // in case all one word
	if (isspace(*scanFrom)) {  // skip blanks we started on
		while ((scanFrom>msg) && isspace(*scanFrom))
			scanFrom--;
	}
	else {
		if (*scanFrom!='\0') { // not pointing at terminating null
			while ((scanFrom>msg) && !isspace(*scanFrom))
				scanFrom--;
			if ((scanFrom>msg)) {
				const char* splitWordStart = scanFrom+1;
				while (isspace(*scanFrom) && (scanFrom>msg))  // skip blanks we reached
					scanFrom--;
				if ((scanFrom>msg))
					lenToNextStart = splitWordStart-msg+2;
			}
		}
	}
	return scanFrom-msg+1;		
}
#endif


// -------------------------------------------------------
//             d b T a b l e A s s i g n e r
// -------------------------------------------------------

dbTableAssigner::dbTableAssigner(dbTable* inTable, const char* inFirstFieldValue) :
	mTable(inTable),
	mNextFieldNum(0)
{
	assert(!mTable->isNewRecord());
	mTable->newRecord();
	setNextFieldString(inFirstFieldValue);
}


/**
	finish the steps of assigning fields by saving record
*/
dbTableAssigner::~dbTableAssigner()
{
	assert(mTable->isNewRecord());
	mTable->saveRecord();
}


/**
	Set a field to a value and increment iterator across fields.
	\param inString can be null pointer, meaning just skip a field.
*/
void
dbTableAssigner::setNextFieldString(const char* inString)
{
	dbField* settingField = mTable->field(mNextFieldNum);
	assert(settingField);
	if (inString && settingField)
		settingField->setString(inString);
	mNextFieldNum++;
}


// -------------------------------------------------------
//                      d b T a b l e
// -------------------------------------------------------
dbTable::dbTable(const char* tableName) :
								mBackend(0),
								mTableName(tableName),
								mTableNumber(USHRT_MAX),
								mFieldCount(0),
								mSaveOption(requireExplicit),
								mAllowShortImports(false),
								mTableValid(eInvalid),
								mValidatingDepth(0), 
								mGUI(0),
								mRelToLHS(0),
								mDefaultPrototypeParser(0),
								mPrototype(this),
								mLockedCurrentRecord(false),
								mInTransaction(false),
								mTransactionDirty(false),
								mInSaveLoop(false)
#ifndef OOF_DISALLOW_CHANGE_SHIELDING
								,mChangeShielded(false)
#endif	
{
// if they don't supply a name at this point we are stuffed - the virtual function defaultName()
// can't be called from within this ctor! So just report the table as 'untitled'.

	const char* badTableName = tableName;
	if (badTableName==0 || strlen(badTableName)==0)
		badTableName = "'untitled'";
	if(dbConnect::currentlyConstructing()==0) {
		dbConnect::raise(stringstream() << flush << "dbTable: " << badTableName 
		<< " is defined AFTER the call to newConnection or openConnection, too late to be part of this database connection's schema", false);
		mConnection = 0;	// so this table can be deleted
	} 
	else {
		dbConnect::currentlyConstructing()->attachTable(this);
		dbTable::sCurrentlyConstructing = this;
	}
}



dbTable::~dbTable()  
{
	setSaveOption(requireExplicit);  // so backend can destroy cache prior close
	if (mConnection && prototype()==this)
		mConnection->close();
	delete mBackend;
	if (mDefaultPrototypeParser)
		mDefaultPrototypeParser->decRefs();
}


dbTable::dbTable(const dbTable& rhs) :
				mFields(rhs.mFieldCount),  // will be populated by dbField copy ctor attaching to table
				mTableName(rhs.mTableName),
				mTableNumber(rhs.mTableNumber),
				mFieldCount(0),
				mSaveOption(rhs.mSaveOption),
				mAllowShortImports(rhs.mAllowShortImports),
				mTableValid(eInvalid),
				mValidatingDepth(0), 
				mGUI(0),
				mRelToLHS(0),  // is set when the relationship is instantiated
				mDefaultPrototypeParser(rhs.mDefaultPrototypeParser),
				mConnection(rhs.mConnection),
				mPrototype(rhs.mPrototype),
				mLockedCurrentRecord(false),	// clone does NOT manage locking unless shared sel
				mInTransaction(false),
				mTransactionDirty(false),
				mInSaveLoop(false)
#ifndef OOF_DISALLOW_CHANGE_SHIELDING
				,mChangeShielded(rhs.mChangeShielded)
#endif	
{
	if (mDefaultPrototypeParser)
		mDefaultPrototypeParser->incRefs();

	if (OOF_dbTableStatics::sCloningInDifferentConnection) {
		dbConnect::currentlyConstructing()->attachTable(this);
		OOF_dbTableStatics::sCloningInDifferentConnection = false;
	}
	else {
		if (OOF_dbTableStatics::sCloningSelSharing==selShared) {
			mGUI = rhs.mGUI;
			mLockedCurrentRecord = rhs.mLockedCurrentRecord;
		}
		mBackend = rhs.mBackend->clone(OOF_dbTableStatics::sCloningSelSharing, this);	
	}
		
	sCurrentlyConstructing = this;    // so fields that are about to invoke dbField copy constructor point to ME
	if (OOF_dbTableStatics::sCloning)  {
		OOF_dbTableStatics::sCloning = false;
		mTableValid = eValid;
	}
	else // called from user copy ctor, not via clone()
		new dbCloneCleanup(this);  // self-destroying dependent to tell table to postCloneTableCleanup()
// last item, in case we need to test it above
	OOF_dbTableStatics::sCloningSelSharing = selCopyOnWrite;
}


dbTable::dbTable(const dbTable& rhs, OOF_tableBackend* inBackend) :
				mBackend(inBackend),
				mFields(rhs.mFieldCount),  // will be populated by dbField copy ctor attaching to table
				mTableName(rhs.mTableName),
				mTableNumber(rhs.mTableNumber),
				mFieldCount(0),
				mSaveOption(rhs.mSaveOption),
				mAllowShortImports(rhs.mAllowShortImports),
				mTableValid(eInvalid),
				mValidatingDepth(0), 
				mGUI(0),
				mRelToLHS(0),  // is set when the relationship is instantiated
				mDefaultPrototypeParser(rhs.mDefaultPrototypeParser),
				mConnection(rhs.mConnection),
				mPrototype(this),
				mLockedCurrentRecord(false),	// clone does NOT manage locking
				mInTransaction(false),
				mTransactionDirty(false),
				mInSaveLoop(false)
#ifndef OOF_DISALLOW_CHANGE_SHIELDING
				,mChangeShielded(rhs.mChangeShielded)
#endif	
{
	if (mDefaultPrototypeParser)
		mDefaultPrototypeParser->incRefs();

	if (rhs.mDefaultPrototypeParser) {
		mDefaultPrototypeParser->incRefs();
		mDefaultPrototypeParser = rhs.mDefaultPrototypeParser;
	}
	sCurrentlyConstructing = this;    // so fields that are about to invoke dbField copy constructor point to ME
}


void 
dbTable::stillCurrentlyConstructing()
{
	assert(sCurrentlyConstructing && (sCurrentlyConstructing!=this) );  // redundant, indicates someone losing track
	assert(mConnection && (mConnection==dbConnect::currentlyConstructing()) ); // IMPORTANT
	// it is only valid to call this
	// 1) when constructing our connection, before newConnection/openConnection
	// 2) when this table has been partly constructed, and we are continuing it
	//    eg: when reading in a schema, where we've recursed to construct
	//        nested tables
	sCurrentlyConstructing=this;
}


void 
dbTable::createTableInConnection(const dbConnect* theDB) 
{
	mBackend->createTableInConnection(theDB);
	setTableValid(true);
}

bool 
dbTable::openTableInConnection(const dbConnect* theDB,const bool createIfMissing) 
{
	bool ret;
	sCurrentlyConstructing = this;    // so fields created by backend, eg: if extra fields in opened file, point to ME
	ret = mBackend->openTableInConnection(theDB,createIfMissing);
	setTableValid(true);
	return ret;
}


oofString 
dbTable::name() const
{
	return tableName();
}


oofWordParser*
dbTable::getDefaultWordParser()
{
	if	(mDefaultPrototypeParser) 
		return mDefaultPrototypeParser;
	else
		return mConnection->getDefaultWordParser();
}


void 
dbTable::setDefaultWordParser(oofWordParser* inParser)
{
	delete mDefaultPrototypeParser;
	mDefaultPrototypeParser = inParser;
}


void 
dbTable::setSortOrder(const dbSorter& srt)
{
	mBackend->setSortOrder(new dbSorter(srt));
// don't log until finished setting new sorter as it's not complete to describe
 #ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
		dbConnect::debugLog() << "setSortOrder:\t";
		dbSorter* newSorter = sorter();	// created and fully initialised above
		newSorter->describe(dbConnect::debugLog());
		describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif
}
 
 
void 
dbTable::addIndices(const char* newIDXname) 
{
	mBackend->addIndices(newIDXname);
}


void 
dbTable::rebuild() 
{
	mBackend->rebuild();
}


void 
dbTable::close() 
{
	if (isOpen()) {
		#ifdef OOF_DEBUG_LOG_NAME
			StandardDebugLog("dbTable::close()");
		#endif
		mBackend->close();
		mTableValid = eClosed;
	}
	else {
		#ifdef OOF_DEBUG_LOG_NAME
			StandardDebugLog("dbTable::close() attempt to close already closed table");
		#endif
	}
}


void 
dbTable::deleteStorage() 
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::deleteStorage()");
#endif
	mBackend->deleteStorage();
}


void 
dbTable::suppressIndices() 
{
	mBackend->suppressIndices();
}


void 
dbTable::buildSchema() 
{
	if (mTableName.isEmpty()) {
		mTableName = defaultName();
	}
	mBackend->buildSchema();
}


// ensure a record loaded, most commonly by traversing a relationship, or maybe reloading
// the current record if someone's forgotten a start()
// can automatically create related records, depending on the caller's settings
bool
dbTable::ensureRecordLoaded(bool autoCreateRelated, bool failureAborts)
{
	bool ret = true;
	if (!isTableValid())
		MakeTableValid();
	if (!isRecordLoaded()) {
		if (mRelToLHS)  { // this is a clone managing a related selection
			if (!mRelToLHS->relateFromRecord()) {
				if (autoCreateRelated)
					newRecord();
				else {
					ret = false;
					if (failureAborts) {
						oofE_TableError excp(this, stringstream() << flush 
							<< "dbTable::ensureRecordLoaded failed to find related record in table "
							<< tableName()
							<< " - perhaps you have forgotten a newRecord()?");
						RAISE_EXCEPTION(excp);
					}
				}
			} // failed relationship traversal
		} // related record
		else {
			if (isEmpty()) {
				if (failureAborts) {
					oofE_TableError excp(this, stringstream() << flush 
						<< "dbTable::ensureRecordLoaded called on empty selection in table "
						<< tableName());
					RAISE_EXCEPTION(excp);
				}
			}
			else
				ret = reloadRecord();
		}	// try reloading - not related record
	} // record not loaded
	return ret;
}


void
dbTable::MakeTableValid()
{
	mValidatingDepth++;
	if (mRelToLHS) { // this is a clone managing a related selection
		mTableValid = eValid;	// WARNING in a few circumstances we have to set this 
						// before relateFromRecord called, to ensure our RHS value of the join doesn't trigger an endless recursion
		mRelToLHS->relateFromRecord();
		assert(isTableValid());  // should be side-effect of relateFromRecord
	}	
	else { //  invalid because not completed cleaning up after a clone
		broadcast(OOFmsg_FinishCloning);	// give attached cleanups a chance
		if (!isTableValid()) {	// if still invalid, it's a problem		
			oofE_TableError excp(this, stringstream() << flush 
				<< "dbTable::MakeTableValid - a non-related table is invalid: "
				<< tableName()
				<< " - possibly you have not performed a newConnection or openConnection?");
			RAISE_EXCEPTION(excp);
		}
	}
	mValidatingDepth--;
} 


void
dbTable::LoadRelatedContext()
{
	assert(mRelToLHS);   // this is a clone managing a related selection
	mRelToLHS->relateFromRecord();
}


void 
dbTable::appendSelection(const dbSelection& inSel)
{
// NOT YET IMPLEMENTED - any test to make sure inSel on same table
	if (allSelected())
		return;  // don't do anything - we can't be appended to!
	
	if (inSel.isAllRecs()) {
		selectAll();
		broadcast(OOFmsg_AppendSelection, (unsigned long)&inSel);
	}
	else {
	// it may seem tempting to replace this with a simple call to
	// mBackend->union_with(inSel) but note that we are broadcasting
	// the trimmed down selection appended, not the input selection!
		dbSelection ours = currentSelection();
		dbSelection addSel = inSel;
		addSel.difference_with(ours);	// remove overlap between incoming and us
		ours.union_with_no_overlap(addSel);	// an optimal append now we've done the difference!
		setSelection(ours);
		broadcast(OOFmsg_AppendSelection, (unsigned long)&addSel);
	}
}


void 
dbTable::removeSelection(const dbSelection& inSel)
{
// NOT YET IMPLEMENTED - any test to make sure inSel on same table
	if (inSel.isAllRecs()) {
		selectNone();
		broadcast(OOFmsg_RemoveSelection, (unsigned long)&inSel);
	}
	else {
	/*	if (allSelected()) {	// don't need to calculate overlap - actual set removed
			difference_with(inSel);
			broadcast(OOFmsg_RemoveSelection, (unsigned long)&inSel);
		} 
		else */{	// neither side in allRecs state - need to calculate overlapping set removed
			dbSelection ours = currentSelection();
			dbSelection removeSel = inSel;
			removeSel.intersection_with(ours);	// only want overlap between incoming and us
			ours.difference_with(removeSel);	// NOT YET IMPLEMENTED - an optimal difference now we've done the intersection!
			setSelection(ours);
			broadcast(OOFmsg_RemoveSelection, (unsigned long)&removeSel);
		}
	}
}


void 
dbTable::clearSelection()
{
	selectNone();
	broadcast(OOFmsg_ClearSelection);
}


void
dbTable::markDirty()
{
#ifndef OOF_DISALLOW_CHANGE_SHIELDING
	if (mChangeShielded)
		return;	// don't mark
// NOTE this implies a related table could be change shielded independently of its parent		
#endif

	if (mRelToLHS && tableCachesDirtyRecords()) {
		dbTable* leftTable = lhsTableInRelationshipSpec();
		assert(leftTable);	// from the mRelToLHS check above
		leftTable->markDirty();	// PROPAGATE DIRTINESS BACK TO PARENT RECORD
	}
	mBackend->markDirty();
}


ostream& 
operator<<(ostream& os, dbTable& tbl)
{
	tbl.extract(os);
	return os;
}


ostream& 
operator<<(ostream& os, dbTable* tbl)
{
	tbl->extract(os);
	return os;
}


istream& 
operator>>(istream& is, dbTable* tbl)
{
	tbl->insert(is);
	return is;
}


istream& 
operator>>(istream& is, dbTable& tbl)
{
	tbl.insert(is);
	return is;
}


bool 
dbTable::pointsToBackend(const OOF_tableBackend* rhsBackend) const
{
	const bool ret = (mBackend==rhsBackend);
	return ret;
} 


bool 
dbTable::canSaveRecord()
{
	return true;
}


dbTable* 
dbTable::cloneTableWithoutSelection() const
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::cloneTableWithoutSelection()");
#endif
	return cloneTableConst(selNotShared);
}


dbTable* 
dbTable::cloneTableSharingSelection() const
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::cloneTableSharingSelection()");
#endif
	return cloneTableConst(selShared);
}


dbTable* 
dbTable::cloneTbleWithCachNoSelection() const
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::cloneTbleWithCachNoSelection()");
#endif
	return cloneTableConst(selNotSharedCacheCopied);
}



void 
dbTable::postCloneTableCleanup()
{
	const fieldNumT numFields = mFieldCount;
	for (fieldNumT i=0; i<numFields; i++) 	{
		dbField* theField = field(i);
		theField->postCloneTableCleanup();
	}
	OOF_dbTableStatics::sCloning = false;
	setTableValid(true);
	sCurrentlyConstructing = 0;
}


void 
dbTable::attachfield(dbField* theField)  {
	mFields.append(theField);
	CompleteField(theField);
}


void 
dbTable::extract(ostream& os)
{
	const unsigned long numRecs = count();
	if (numRecs==0)
		return;
		
	start();		// start record iteration (vertical)
	for (unsigned long row=0; row<numRecs; row++) {
#ifdef OOF_EXTRACT_OIDS
		os << currentOID() << "\t";
// make the above conditional, and by default OFF, because the oids change
// with repeated runs of samples (that usually deleteAll) and this complicates
// our regression testing, making it harder to compare successive runs		
#endif
#ifdef OOF_EXTRACT_HIDDEN
		const fieldNumT numFields = mFieldCount;
		for (fieldNumT i=0; i<numFields; i++)
			os << field(i) << "\t";
#else
		for (mFields.startVisible(); mFields.moreVisible(); mFields.nextVisible())
			os << field(mFields.iter()) << "\t";
#endif
		os << endl;
		if (row<numRecs-1)
			next();
	}
}


dbView* 
dbTable::makeView()
{
	dbView* ret = new dbView(this);
	const fieldNumT numFields = mFieldCount;
#ifdef OOF_EXTRACT_HIDDEN
	for (fieldNumT i=0; i<numFields; i++) {
		const dbField* theField = field(i);
#else
	for (mFields.startVisible(); mFields.moreVisible(); mFields.nextVisible()) {
		const dbField* theField = field(mFields.iter());
#endif
		if (theField->fieldType()!=relationshipField)
			*ret << theField;
	}
	return ret;
}



bool 
dbTable:: allowsShortImports() const
{
	return mAllowShortImports;
}


void 
dbTable:: setShortImports(bool allowShorties)
{
	mAllowShortImports = allowShorties;
}


void
dbTable::import(const char* ansiFileName)
{
	ifstream imp(ansiFileName);
	if (imp.good()) {
		deleteAll();
		insert(imp);
		imp.close();
	}
}

 
unsigned long 
dbTable::insert(istream& is)
{
// later will have a dbView::insert() to let us just import into specified fields

// WARNING fails to cope with the the situation where
// 1) our table has only relationship fields at the end, AND
// 2) there are more fields in the import file than we are importing

	suspendSorting();  // otherwise will resort after every record read!
	selectAll();  // prevents us incurring the overhead of expanding a selection
		
	unsigned long numAdded = 0;
	is.flags(is.flags() & ~ios::skipws);  // we look at the white space!
	const unsigned int lastExpected = (mFieldCount-1);

	while (is.good()) {
		bool gotCompleteRecord = false;
		newRecord();		// start record iteration (vertical) only if something on line
		const fieldNumT numFields = mFieldCount;
		for (fieldNumT i=0; i<numFields; i++) {
			dbField* theField = field(i);

			if (theField->fieldIsVirtual())
				continue;
			
			if (!theField->insert(is, kFieldSep, kRecSep))  
				break;  // field failed for some reason, probably partial record
				
			if (is.good()) { // should be at field or rec separator, NOT end of stream
				char c;
				is >> c;  // consume record or field separator
				if (c==kFieldSep) { // most likely is FieldSep
					if (i==lastExpected) {	// FieldSep after last field, which some people do!
						do {				// or maybe more fields than our table imports
							is >> c;  // just skip chars until end of record
							if (c==kRecSep) {
								gotCompleteRecord = true;
								break;	// end of record, after 
							}
						}
						while (is.good());				
					}
				}
				else {
					gotCompleteRecord = true;
					assert(c==kRecSep);  // otherwise should be end of record
					if (i!=lastExpected && !mAllowShortImports) {
					// might be just relationship fields left						
						for (fieldNumT j=i+1; j < mFieldCount; j++) {
							dbField* maybeVirtField = field(j); 
							if (!maybeVirtField->fieldIsVirtual()) {
								gotCompleteRecord = false;
								break;
							}
						}
#ifdef OOF_DEBUG
						if (!gotCompleteRecord)						
							dbConnect::raise(stringstream() << flush << "dbTable::insert() in table: " 
								<< tableName() << " has hit a short record of " 
								<< i+1 << " out of " << lastExpected+1 
								<< " expected fields" << " on import record " << numAdded
							);   
#endif
					}
					break;  // end of record, short or complete
				}
			}  // stream is good
			else {
				if (is.eof())
					break;  // end of file exit for g++ streams lib peculiarity
					
#ifdef OOF_DEBUG
				dbConnect::raise(stringstream() << flush << "dbTable::insert() in table: " << tableName() 
				                              << " has a problem with import record " << numAdded
												);   
#endif
				break;  // error break - problem with stream
			}
		}  // loop through fields
		
		if (gotCompleteRecord) { 		
			numAdded++;
			saveRecord();
		}
		else {
			break;  // ABNORMAL EXIT FROM LOOP
		}
	} // while getting recs
	unloadRecord();	// unload last saved, or aborted record
	resumeSorting();
	broadcast(OOFmsg_ChangeSelection);
	return numAdded;
}


dbTable& 
dbTable::operator=(const dbTable& rhs)
{
// caller sets context - new record or existing
// leaves record dirty, ready to save or abandon
	copyRecFrom(&rhs);
	return *this;
}


void
dbTable::cloneRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::cloneRecord()");
#endif
	dbTable* toClone = cloneTable();
	toClone->selectJustCurrent();
	newRecord();
	copyRecFrom(toClone);
	delete toClone;
	postCloneRecordCleanup();	// virtual function that mainly clears relationships
// leaves with cloned but unsaved record
}


void
dbTable::postCloneRecordCleanup()
{
// can be overriden if user doesn't want this behaviour
	const fieldNumT numFields = mFieldCount;
	for (fieldNumT i=0; i<numFields; i++) {
		dbField* theField = field(i);
		if (theField->fieldType()==relationshipField) {
			dbField* joinField = ((dbRelRefBase*) theField)->joinField();	// safe downcast
			if (joinField) 
				joinField->clear();	// NOT the relationship but the join key it uses, MAY CLEAR MULTIPLE TIMES!
		}	// field is relationship
	}	// loop all fields
}


void 
dbTable::saveRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::saveRecord()");
#endif
	do {
		const bool wasOld = !isNewRecord();
		const oidT originalOID=currentOID();
		
	    if (mRelToLHS)  // this is a clone on the rhs of a relationship
			mRelToLHS->updateRelValue();  // so save the relationship
// WARNING this seems like a mistake, that this is an operation that should be performed
// once only. Remember that this copying only occurs for the rhs of a relationship which
// would often be a selection of multiple records.
		mBackend->saveRecord();
		broadcast(OOFmsg_SaveRecord, isNewRecord());  // AFTER saving us, so our oid is updated for any dependents
	// or we may not be the record we were an iteration ago - our contents may have
	// been replaced from the cache (again, via a dependent)
	if( mInTransaction)
		mTransactionDirty = true; 
	
// how can we tell if the record has changed?
// 1) if it is new now		
// 2) if it was not new, and the oid changed
// 3) if it was new, we EXPECT the oid to change, therefore...
//    NOT YET IMPLEMENTED - dunno how to cope
// how do we tell the difference between a recently-new record that's just been dirtied
// and an existing dirty record?????

		if (isNewRecord() || (wasOld && originalOID!=currentOID())) { // not same record
			if (mInSaveLoop) 
				break;	// we're already in a loop so exit this level 
						// as soon as the current record changes
			else
				SaveLoop();	// this is the first record in a set of cached ones, start the loop!
		}
	}
	while (isDirty() || isNewRecord());  // one of our dependents may have dirtied us
}


void 
dbTable::SaveLoop()
{
	mInSaveLoop = true;
	do {
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::SaveLoop()");
#endif

		saveRecord();	// virtual invocation of possible user overriden version
	}
	while (isDirty() || isNewRecord());  // one of our dependents may have dirtied us
	// or we may not be the record we were an iteration ago - our contents may have
	// been replaced from the cache (again, via a dependent)
	mInSaveLoop = false;
}


void 
dbTable::deleteRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::deleteRecord()");
#endif
	if (isRecordLoaded()) {
		relateAllDeletables();
		broadcast(OOFmsg_DeleteRecord);	// fix those to the right of us
		if (mRelToLHS)	// update our lhs
			mRelToLHS->breakRelLink();
		mBackend->deleteRecord();
	}
}


void 
dbTable::deleteSelection()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::deleteSelection()");
#endif
	start();	
	const unsigned long numRecs = count();
	for (unsigned long i=0; i<numRecs; i++) {
		deleteRecord();
	}
	selectNone();
}


void 
dbTable::deleteAll()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::deleteAll()");
#endif
	selectAll();
	suspendSorting();
	const unsigned long numRecs = countAll();
	for (unsigned long i=0; i<numRecs; i++) {
		start();
		deleteRecord();
	}
	resumeSorting();
}


void 
dbTable::stats(ostream& os)
{
	if (mTableName.isEmpty())
		os << "Un-named table";
	else
		os << "table: " << mTableName;
	const unsigned long numRecs = countAll();
	if (numRecs)
		os << " num records:" << numRecs << endl;
	else
		os << " is empty" << endl;
}


void 
dbTable::describe(ostream& os) const
{
	if (mTableName.isEmpty())
		os << "Un-named table";
	else
		os << "table: " << mTableName;
	os << " contains fields:" << endl;
	mFields.describe(os);
}


void 
dbTable::describeState(ostream& os) const
{
	if (mTableName.isEmpty())
		os << "Un-named table";
	else
		os << "table: " << mTableName;
	os << '\t' << hex << (void*)this << dec;	// many table clones could have same name, so always dump our pointer
	if (mTableValid==eValid)
		os << "\t(valid)";
	else if (mTableValid==eInvalid)
		os << "\t(INvalid)";
	else 
		os << "\t(closed)";
	if (mRelToLHS) {
		dbTable* relatedLHS = lhsTableInRelationshipSpec();
		os << "\tRHS of rel:";
		while (relatedLHS) {
			os << '\t' << relatedLHS->tableName() << " (" << (void*) relatedLHS << ')';
			relatedLHS = relatedLHS->lhsTableInRelationshipSpec();	// next in the chain
		}
	}
	os << "\tConnection: " << hex << mConnection << dec << endl;
	mBackend->describeState(os);
	os << endl;
}

#ifdef OOF_DEBUG_LOG_NAME
void 
dbTable::StandardDebugLog(const char* comment) const
{
	dbConnect::startDebugWrite();
		dbConnect::debugLog() << comment << endl;
		describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
}


void 
dbTable::StandardQueryDebugLog(const char* comment, const dbQueryClause& query) const
{
	dbConnect::startDebugWrite();
		dbConnect::debugLog() << comment;
		query.describe(dbConnect::debugLog());
		describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
}
#endif


void
dbTable::copyAllFrom(dbTable* rhs)
{
	#ifdef OOF_DEBUG
		if (!isCopyCompatibleWith(rhs)) {
			dbConnect::raise(stringstream() << flush 
				<< "dbTable::copyAllFrom attempt to copy incompatible tables: "
				<< rhs->tableName() << " to " << tableName(),
			false);
			return;
		}
	#endif	
	
	suspendSorting();	
	rhs->start();
	for (unsigned long numRecs = rhs->count(); numRecs>0; numRecs--) {
		newRecord();
		copyRecFrom(rhs);
		saveRecord();
		rhs->next();
	}
	resumeSorting();
}



bool
dbTable::relateSelectionFrom(dbTable* lhs)
{
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() << "relateSelection(" << hex << (void*)lhs << dec << " " << lhs->tableName() << ")";
	describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif

	assert(mRelToLHS);		// NOT YET IMPLEMENTED - cope with relationships not yet established

	bool ret;
	mRelToLHS->selectAllRelated();
	dbTable* nextInLine = mRelToLHS->relatedTable();
	if (nextInLine->isEmpty()) {
		lhs->selectNone();	// cease relating, even if not finished chain
		ret = false;
	}
	else
		if (nextInLine == lhs)	// at end of chain and found something!
			ret = true;
		else
			ret = nextInLine->relateSelectionFrom(lhs);	// recurse along chain
	return ret;
}


bool 
dbTable::selectAllRelated(bool broadcastChange)
{
	assert(mRelToLHS);		// NOT YET IMPLEMENTED - cope with relationships not yet established
	return mRelToLHS->selectAllRelated(broadcastChange);
	// allows user to use either syntax:
	// Patients->Visits->selectAllRelated()   OR
	// Patients->Visits.selectAllRelated()  
}


void
dbTable::copyRecFrom(const dbTable* rhsTable, fieldNumT maxCopied)
{
	fieldNumT lhs = 0;
	fieldNumT rhs = 0;
	const fieldNumT lhsNumFields = mFieldCount;
	fieldNumT rhsNumFields = rhsTable->mFieldCount;
	if (maxCopied && maxCopied<rhsNumFields)  // default is to copy all available, but can pass in this upper limit
		rhsNumFields = maxCopied;
		
	for(;;) {
		dbField* lhsField=0;
		dbField* rhsField=0;
		while (lhs<lhsNumFields) { // loop to skip virtual fields, no matter how many
			lhsField = field(lhs);
			lhs++;
			if (!lhsField->fieldIsVirtual())
				break;
			lhsField=0;
		}
		while (rhs<rhsNumFields) { // loop to skip virtual fields, no matter how many
			rhsField = rhsTable->field(rhs);
			rhs++;
			if (!rhsField->fieldIsVirtual())
				break;
			rhsField=0;
		}
		if (lhsField && rhsField)
			lhsField->copyValueFrom(rhsField);
		else
			break;	//**** EXIT COPY LOOP **** no longer have two valid fields, assume one side exhausted.
	}
}


dbTable*
dbTable::lhsTableInRelationshipSpec() const
{
	if (mRelToLHS)	// we ARE the cloned table on the rhs of a relationship
		return mRelToLHS->relatedTable();
	else
		return 0;
}


dbTable*
dbTable::baseTableOfRelationChain()
{
	if (mRelToLHS)	 // we ARE the cloned table on the rhs of a relationship
		return mRelToLHS->relatedTable()->baseTableOfRelationChain();
	else
		return this;
}


bool
dbTable::isCopyCompatibleWith(const dbTable* rhsTable) const
{
// defined as the first N non-virtual fields being the same
// (ie: insertion of relationships and calculated fields is ignored

	int lhs = 0;
	int rhs = 0;
	const int lhsNumFields = mFieldCount;
	const int rhsNumFields = rhsTable->mFieldCount;
		
	for(;;) {
		dbField* lhsField=0;
		dbField* rhsField=0;
		while (lhs<lhsNumFields) { // loop to skip virtual fields, no matter how many
			lhsField = field(lhs);
			if (!lhsField->fieldIsVirtual())
				break;
			lhs++;
			lhsField=0;
		}
		while (rhs<rhsNumFields) { // loop to skip virtual fields, no matter how many
			rhsField = rhsTable->field(rhs);
			if (!rhsField->fieldIsVirtual())
				break;
			rhs++;
			rhsField=0;
		}
		if (lhsField && rhsField) {
			if (!lhsField->fieldIsSameTypeAs(rhsField))
				return false;
			lhs++;  // move on to compare next pair of fields
			rhs++;
		}
		else
			break;	//**** EXIT COMPARE LOOP **** no longer have two valid fields, assume one side exhausted.
	}
 	return true;
}


void 
dbTable::generateTestData(unsigned long numRecords, bool generateRandom, unsigned long seedOrCount)
{
	 if (generateRandom)
	 	srand(seedOrCount);	// seed the generator
		
	 for (unsigned int r=0;r<numRecords; r++) {
	 	newRecord();
		for (fieldNumT i=0; i<mFieldCount; i++) {
				dbField* theField = field(i);
				if (theField->fieldType() != relationshipField)
					theField->generateTestData(generateRandom, seedOrCount); 
		}
		beginTransaction();
		saveRecord();
		commitTransaction();
		if (!generateRandom)
			++seedOrCount;	// inc. the count for each record
	}
}


void 
dbTable::start() 
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::start()");
#endif

	if (!isTableValid())
		MakeTableValid();
/* DISABLE UNTIL REFINE THE SHARED SELECTION MODEL BETTER
	if (atFirst())
		return;  // optimisation, going to current record
*/		
	ContextChange();
	mBackend->start();
}


bool 
dbTable::reloadRecord() 
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::reloadRecord()");
#endif

	if (!isTableValid())
		MakeTableValid();
	ContextChange();
	const bool ret = mBackend->reloadRecord();	// may reload or may start()
	return ret;
}


void 
dbTable::newRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::newRecord()");
#endif

	if (!isTableValid())
		MakeTableValid();
	ContextChange();
	mBackend->newRecord();
	mLockedCurrentRecord = true;	// EFFECTIVELY TRUE, ALTHOUGH NO ACTUAL FILE LOCK
//	broadcast(OOFmsg_NewRecord);  // can't think of reason why now, maybe to support GUI
}



bool
dbTable::isEmpty()
{
	if (isRecordLoaded())
		return false;
	return (count()==0);
}


void 
dbTable::ContextChange()
{
	switch (mSaveOption) {
	case autoOnContextChange :
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::ContextChange() - autoSave");
#endif
		if (isRecordLoaded()) {
            saveRecord();
        }
		break;
	
	case bufferForever :
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::ContextChange() - bufferForever");
#endif
		broadcast(OOFmsg_ChangeContext); 
		mBackend->CachingContextChange();
		break;
	
	case requireExplicitAndBuffer :
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::ContextChange() - requireExplicitAndBuffer");
#endif
		broadcast(OOFmsg_ChangeContext);  // in the autoOnContextChange we broadcast messages like msgSaveRecord
		mBackend->CachingContextChange();
		break;
		
	case requireExplicit :
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::ContextChange() - requireExplicit");
#endif
#ifdef OOF_DEBUG
		if (isDirty())
			dbConnect::raise(stringstream() << flush << "ContextChange called on dirty record without saving: " 
			<< "file name: " << tableName() 
			<< endl);
#endif
		broadcast(OOFmsg_ChangeContext);
		mBackend->ContextChange();
		unloadRecord();
		break;	
		
	default :
		assert(0);
	}	
}


bool 
dbTable::addOIDtoSelection(oidT inOID)
{
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() << "addOIDtoSelection(" << inOID << ")";
	describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif

	if (!isTableValid())
		MakeTableValid();
	const bool ret = mBackend->addOIDtoSelection(inOID);
	if (ret)
		broadcast(OOFmsg_ChangeSelection);
	return ret;
}


void 
dbTable::dropRecordFromSelection(unsigned long inRecNum)
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->dropRecordFromSelection(inRecNum);
	broadcast(OOFmsg_ChangeSelection);
}


bool 
dbTable::gotoRecord(unsigned long relativeNum)
{
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() << "gotoRecord(" << relativeNum << ")";
	describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif

	if (!isTableValid())
		MakeTableValid();
// 97/04/24 removed until invalidates current position 
//	if (mBackend->atRelative(relativeNum))
//		return true;  // optimisation, going to current record

	ContextChange();
	return mBackend->gotoRecord(relativeNum);
}


/**
	Start a transaction if not already in one.
	\todo Decide if this is robust as ignores nested requests!
*/
void
dbTable::beginTransaction(short tMode)
{
	if (!mInTransaction) {
		mBackend->beginTransaction(tMode);  
		mInTransaction = true;
		mTransactionDirty = false;
	}
}


void 
dbTable::commitTransaction(short rMode)
{
	if (mInTransaction){ //will catch end without begins
		mBackend->commitTransaction(rMode);
		mTransactionDirty = false;
		mInTransaction =false;
	}
}


/**
	Forward request to backend to abort transaction, if mInTransaction.
	\todo Investigate if should use transaction nesting count, not just mInTransaction.
*/
void 
dbTable::abortTransaction(short rMode)
{
	if (mInTransaction){ //will catch end without begins
		mBackend->abortTransaction(rMode);
		mTransactionDirty = false;
	}
	mInTransaction =false;
}



/**
	Change table's current loaded selection and current record to satisfy a join.
	How the join is performed depends on each backend.
	Bit of redirection here - as a destination table, almost certainly in a relation,
	the field to passed in is probably a "real" field pointing back to the original table
	not the cloned destination of a relation. To make sure no side effects can change
	the original table, retrieve our copy of the field via the ordinal field number.
*/
bool 
dbTable::loadRelatedContextJoiningFromTo(const dbField* f1, const dbField* f2)
{
	broadcast(OOFmsg_UnloadRecord);
	dbField* reallyTo = field( f2->fieldNumber() );  
	
	return mBackend->loadRelatedContextJoiningFromTo(f1, reallyTo);
}


void
dbTable::relateAllDeletables()
{
	const fieldNumT numFields = mFieldCount;
	for (fieldNumT i=0; i<numFields; i++) {
		dbField* fld = field(i);
		if (fld->fieldType()==relationshipField) {
			dbRelRefBase* rel = (dbRelRefBase*) fld;  // safe downcast
			if (rel->propagatesDeletes()) {
// just because the traversal field exists, doesn't mean we have a fully instantiated
// relationship - we need to refer to the rhs table and we shouldn't bother relating
// if there's no related records!
                dbTable* relProto = rel->relatedTable(); 
                assert(relProto);
                relProto->LoadRelatedContext();
            }
		}
	}
}


/*
bool 
dbTable::searchDefaultIndex(const char* defaultIndexSearchStr)
{
	dbQueryBinary Q(new dbQueryField(field(mSortByFieldNum)), dbQueryClause::equals, 
		new dbQueryLiteralStr(defaultIndexSearchStr));
	return search(Q);
}
*/


bool 
dbTable::search(const dbQueryClause* query)
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardQueryDebugLog("dbTable::search()", *query);
#endif

	return search(*query);
}


bool 
dbTable::searchSelection(const dbQueryClause* query)
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardQueryDebugLog("dbTable::searchSelection()", *query);
#endif

	return searchSelection(*query);
}


bool 
dbTable::search(const dbQueryClause& query)
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardQueryDebugLog("dbTable::search()", query);
#endif

	if (!isTableValid()) {
		if (mRelToLHS)
			mTableValid = eValid;	// don't want to make valid from lhs relationship
		else 
			MakeTableValid();
	}
	suspendSorting();  // don't want to trigger a sort if our search ends up iterating the collection
	ContextChange();
	const bool foundAny = mBackend->search(&query);
	resumeSorting();
	broadcast(OOFmsg_ChangeSelection);
	return foundAny;
}


bool 
dbTable::searchSelection(const dbQueryClause& query)
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardQueryDebugLog("dbTable::searchSelection()", query);
#endif

	if (!isTableValid())
		MakeTableValid();
	suspendSorting();  // don't want to trigger a sort if our search ends up iterating the collection
	ContextChange();
	const bool foundAny = mBackend->searchSelection(&query);
	resumeSorting();
	broadcast(OOFmsg_ChangeSelection);
	return foundAny;
}

bool 
dbTable::searchSelContainsAnyDelimited(const dbField& schField, const char* schStr, char delim)
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->searchSelContainsAnyDelimited(&schField, schStr, delim);
}


bool
dbTable::searchSelContainsAnyOf(const dbField& schField, const char** schStrs, unsigned long count)
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->searchSelContainsAnyOf(&schField, schStrs, count);
}


bool
dbTable::searchSelContainsAnyOf(const dbField& schField, oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return searchSelContainsAnyOf(schField, inParser->generatedStrings(), inParser->generatedCount());
}


bool 
dbTable::searchSelContainsAllDelimited(const dbField& schField, const char* schStr, char delim)
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->searchSelContainsAllDelimited(&schField, schStr, delim);
}


bool
dbTable::searchSelContainsAllOf(const dbField& schField, const char** schStrs, unsigned long count)
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->searchSelContainsAllOf(&schField, schStrs, count);
}


bool
dbTable::searchSelContainsAllOf(const dbField& schField, oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return searchSelContainsAllOf(schField, inParser->generatedStrings(), inParser->generatedCount());
}


void 
dbTable::unSorted()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::unSorted()");
#endif

	mBackend->setSortOrder(0);
}
	

void 
dbTable::setSortOrder(const dbField& theField, bool shouldReverse)
{
 #ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
		if (shouldReverse)
			dbConnect::debugLog() << "setSortOrder(reverse):\t";
		else
			dbConnect::debugLog() << "setSortOrder:\t";
		theField.describe(dbConnect::debugLog());
		describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif

   dbSorter* currentSorter = sorter();
    if (currentSorter && currentSorter->sortsBySingleField())
        ((dbFieldSorter*)currentSorter)->changeSortFieldTo(theField.fieldNumber(), shouldReverse);  // safe downcast
    else
        mBackend->setSortOrder(new dbFieldSorter(theField.fieldNumber(), shouldReverse));
}


void 
dbTable::setReverseSortOrder(const dbField& theField)
{
   setSortOrder(theField, true);
} 


void
dbTable::sortNow() const
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::suspendSorting()");
#endif

	dbSorter* currentSorter = sorter();
	if (currentSorter)
		currentSorter->sortNow(mBackend);
}


void 
dbTable::suspendSorting()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::suspendSorting()");
#endif

    dbSorter* currentSorter = sorter();
	if (currentSorter) 
		currentSorter->suspendSorting();
	
// our fields or other dependants may need to know, eg: if have related tables
	broadcast(OOFmsg_SuspendSorting);
} 


void 
dbTable::resumeSorting()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::resumeSorting()");
#endif

    dbSorter* currentSorter = sorter();
	if (currentSorter)
		currentSorter->resumeSorting();
	
// our fields or other dependants may need to know, eg: if have related tables
	broadcast(OOFmsg_ResumeSorting);
} 


void 
dbTable::revertRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::revertRecord()");
#endif

	broadcast(OOFmsg_ChangeContext);
	mBackend->revertRecord();
}


void 
dbTable::unloadRecord()
{
#ifdef OOF_DEBUG_LOG_NAME
	StandardDebugLog("dbTable::unloadRecord()");
#endif

	broadcast(OOFmsg_UnloadRecord);
	mBackend->unloadRecord();
}


void 
dbTable::CompleteField(dbField* theField)
{
	theField->mBackend = mBackend;
	theField->mFieldNumber = mFieldCount++;
	theField->mTable = this;
}


dbGUI*
dbTable::getGUI()
{
	if (!mGUI) {		
		if (mRelToLHS)
			mGUI = baseTableOfRelationChain()->getGUI();	// may already have it
		else 
			mGUI = dbGUIFactory::makeGUI(this);
		assert(mGUI);
	}
	return mGUI;
}



void 
dbTable::difference_with(dbTable& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	if (!rhs.isTableValid())
		rhs.MakeTableValid();
	assert(isCopyCompatibleWith(&rhs));
	mBackend->difference_with(rhs.mBackend);
	broadcast(OOFmsg_ChangeSelection);
}
	

void 
dbTable::difference_with(const dbSelection& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->difference_with(rhs);
	broadcast(OOFmsg_ChangeSelection);
}
	

void 
dbTable::invert()
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->invert();
	broadcast(OOFmsg_ChangeSelection);
} 


void 
dbTable::union_with(dbTable& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	if (!rhs.isTableValid())
		rhs.MakeTableValid();
	assert(isCopyCompatibleWith(&rhs));
	mBackend->union_with(rhs.mBackend);
	broadcast(OOFmsg_ChangeSelection);
}


void 
dbTable::union_with(const dbSelection& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->union_with(rhs);
	broadcast(OOFmsg_ChangeSelection);
}


void 
dbTable::union_with_no_overlap(dbTable& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	if (!rhs.isTableValid())
		rhs.MakeTableValid();
	assert(isCopyCompatibleWith(&rhs));
	mBackend->union_with_no_overlap(rhs.mBackend);
	broadcast(OOFmsg_ChangeSelection);
}


void 
dbTable::union_with_no_overlap(const dbSelection& rhs)
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->union_with_no_overlap(rhs);
	broadcast(OOFmsg_ChangeSelection);
}


void 
dbTable::intersection_with(dbTable& rhs)  
{
	if (!isTableValid())
		MakeTableValid();
	if (!rhs.isTableValid())
		rhs.MakeTableValid();
	assert(isCopyCompatibleWith(&rhs));
	mBackend->intersection_with(rhs.mBackend);
	broadcast(OOFmsg_ChangeSelection);
}


void 
dbTable::intersection_with(const dbSelection& rhs)  
{
	if (!isTableValid())
		MakeTableValid();
	mBackend->intersection_with(rhs);
	broadcast(OOFmsg_ChangeSelection);
}


dbSelection 
dbTable::currentSelection()
{
	if (!isTableValid())
		MakeTableValid();
	return mBackend->currentSelection();
}


dbSelection 
dbTable::makeEmptySelection() const
{
	return mBackend->makeEmptySelection();
}


oofString
dbTable::tableSingularName() const
{
	return oofString::singular(mTableName);
}


/**
	Try to change the current selection to just the matching record.
	This makes a selection behave like a single object.
	\param oid is a unique record address. Zero input is allowed but causes selectNone().
	If it fails, behaviour is like selectNone().
	\return true if there is a matching oid, false if no match or zero input
	\todo pass address of inOID when change broadcast to take void*
*/
bool 
dbTable::selectJustOID(oidT inOID)
{
#ifdef OOF_DEBUG_LOG_NAME
	dbConnect::startDebugWrite();
	dbConnect::debugLog() << "selectJustOID(" << inOID << ")";
	describeState(dbConnect::debugLog());
	dbConnect::endDebugWrite();
#endif
	if (!inOID) {
		selectNone();
		return false;
	}
	const bool ret = mBackend->selectJustOID(inOID);
	if (ret)
		broadcast(OOFmsg_ChangeSelection, inOID);
	return ret;
}


void 
dbTable::setSelection(const dbSelection& rhs)
{
	setSelection(rhs.internalSelection());
}


void 
dbTable::setSelection(const OOF_Selection* rhs)
{
	if (mSaveOption==bufferForever && isTableValid() && mBackend->isDirty())
		ContextChange();  // want to buffer in this very specific context
		
	mBackend->setSelection(rhs);
	broadcast(OOFmsg_ChangeSelection);
	start();
}


const dbSelection& 
dbTable::operator=(const dbSelection& rhs)
{
	setSelection(rhs);
	return rhs;
}


void 
dbTable::setSaveOption(const saveOptionsT inOpt)
{
	const saveOptionsT oldOpt = mSaveOption;
	if (oldOpt==inOpt)
		return;
		
	mSaveOption = inOpt;
	if (oldOpt==bufferForever)
		mBackend->unloadCache();  // AFTER changing mSaveOption so we will really unload!
}



// -------------------------------------------------------
//         d b T a b l e O w n i n g F i e l d s
// -------------------------------------------------------
dbTableOwningFields::dbTableOwningFields(const char* name) :
	dbTable(name),
	mOwnedFields(10, 2, true)  // so dtor will delete them
{	
}


dbTableOwningFields::dbTableOwningFields(const dbTable& rhs, OOF_tableBackend* newBackend) :
	dbTable(rhs, newBackend),
	mOwnedFields(10, 2, true)  // so dtor will delete them
{
}


dbTableOwningFields::dbTableOwningFields(const dbTableOwningFields& rhs) :
	dbTable(rhs),
	mOwnedFields(rhs.mOwnedFields.count())  // don't copy our dict from rhs, just init to the size required
{
// in theory we could share one set of dbFields
// however each dbField points directly to its owning table, hence we 
// actually need to copy the dbFields

// normally dbFields are concrete members of the dbTable and so are
// automatically copied, and the base dbField ctor attaches them to 
// the parent table

// because we added our fields with a runtime loop, we need a loop
// here to copy them - it's the same issue as any class with heap
// members rather than concrete members

	mOwnedFields.ownsContents();  // so dtor will delete them

	assert(dbTable::sCurrentlyConstructing==this);  // fields we clone will attach to us!
	// this static gets zeroed in dbTable::postCloneTableCleanup()
	// this is how we avoid passing dbTable pointers around all the time when creating fields
	// particularly in user constructors when it would clutter things up a LOT!
	
	// now clone all the rhs's fields
	const fieldNumT numFields = rhs.numFields();
	for (fieldNumT i=0; i<numFields; i++) {
		const dbField* theField = rhs.field(i);
		adoptField(theField->clone());
	}
}

// uses default dtor

/*
dbTable* 
dbTableOwningFields::cloneTable(selSharingT selSharing) const
{
// body copied from oofmacro.h
#ifdef OOF_DEBUG_LOG_NAME
	if (selSharing==selCopyOnWrite) 
		StandardDebugLog("dbTableOwningFields::cloneTableSharingSelection()"); 
#endif
	OOF_dbTableStatics::sCloningSelSharing = selSharing; 
	OOF_dbTableStatics::sCloning = true; 
	dbTableOwningFields* ret = new dbTableOwningFields(*this); 
	ret->postCloneTableCleanup(); 
	return ret; 
}
*/

void 
dbTableOwningFields::adoptField(dbField* adoptedField)
{
	mOwnedFields.append(adoptedField);
}


// -------------------------------------------------------
//           O O F _ F i e l d S e g O w n e r
// -------------------------------------------------------
OOF_FieldSegOwner::OOF_FieldSegOwner()
{
	mSegments.ownsContents();
}


OOF_FieldSegOwner::OOF_FieldSegOwner(const OOF_FieldSegOwner& rhs) :
	mSegments(rhs.mSegments)  // ref counted copy	
{
}


fieldNumT 
OOF_FieldSegOwner::fieldNumberOfSegment(unsigned int i) const
{
	return segment(i)->mFieldNo;
} 


void 
OOF_FieldSegOwner::addSegment(dbField& segField) {
	OOF_Segment* seg = new OOF_Segment(segField);
	mSegments.append(seg);
}


OOF_FieldSegOwner& 
OOF_FieldSegOwner::operator>>(dbField& segField)
{
	addSegment(segField);
	segment(countSegments()-1)->setReverse();
	return *this;
}


void
OOF_FieldSegOwner::toggleSegmentReversedFlags()
{
	const unsigned short numSegs = countSegments();
	for (unsigned short i=0; i<numSegs; i++) {
		OOF_Segment* theSeg = segment(i);
		theSeg->setReverse();
	}
}



// -------------------------------------------------------
//                      d b S o r t e r 
// -------------------------------------------------------
dbSorter::dbSorter() : 
    mSortOnSaves(true),
	mNeedsToSort(true),
	mSorting(false),
	mSuspended(false)
{}


dbSorter::dbSorter(const dbSorter& rhs) : 
	OOF_FieldSegOwner(rhs),
    mSortOnSaves(rhs.mSortOnSaves),
	mNeedsToSort(rhs.mNeedsToSort),
	mSorting(false),
	mSuspended(rhs.mSuspended)
{} 


dbSorter::dbSorter(const OOF_FieldSegOwner& rhs) : 
	OOF_FieldSegOwner(rhs),
    mSortOnSaves(true),
	mNeedsToSort(true),
	mSorting(false),
	mSuspended(false)
{} 


dbSorter* 
dbSorter::clone() const 
{ 
    dbSorter* ret = 0;
	if (!mSorting)
		ret =new dbSorter(*this);
    return ret;
}


dbSorter& 
dbSorter::operator>>(dbField& segField)
{
	addSegment(segField);
	segment(countSegments()-1)->setReverse();
	return *this;
} 


void 
dbSorter::describe(ostream& os) const
{
	const unsigned short numSegs = countSegments();
	for (unsigned short i=0; i<numSegs; i++) {
		OOF_Segment* theSeg = segment(i);
		os << '\t';
		if (theSeg->isReversed())
			os << "dbFieldSorter(reverse):\t";
		else
			os << "dbFieldSorter:\t";
		os << "Field Number: " << fieldNumberOfSegment(i) << endl;
	}
}


bool	
dbSorter::operator!=(const dbSorter* rhs) const
{
	bool isNotEqual = true;
	if (sortsBySingleField()) {
		if (rhs->sortsBySingleField())
			isNotEqual = sortFieldNo() != rhs->sortFieldNo();
	}
	else { // maybe same as us
		if (!rhs->sortsBySingleField() && mNeedsToSort == rhs->mNeedsToSort) {
// ignore mSorting and mSortOnSaves		
			const unsigned int numSegs = countSegments();
			isNotEqual = false;	
			if (numSegs==rhs->countSegments()) {
				for (unsigned int i=0; i<numSegs; i++) {
					if (*segment(i) != rhs->segment(i)) {
						isNotEqual = true;	
						break;
					} // segments differ
				}  // loop segments
			} // same num segs
		}  // same needs to sort
	} // assumed base dbSorter
	return isNotEqual;
}


void 
dbSorter::sortNow(OOF_tableBackend* sorts)
{ 
OOF_DEBUG_LOG(stringstream() << flush << "dbSorter::sortNow\t" 
	<< hex << (void*)this 
	<< "\tsorting: " << sorts->table()->tableName() << " (" << (void*)(sorts->table()) << ')'
);
   mNeedsToSort = false;
   mSorting = true;
   sorts->sortSelectionNow(this);  
// NOTE unlike dbFieldSorter::sortNow, there's no reverse case, reverse sorts are specified segment-by-segment
   mSorting = false;
}






void 
dbSorter::backendChangedContext(OOF_tableBackend* whoChanged) 
{
	if (!mSuspended && mNeedsToSort)
		sortNow(whoChanged);  
}


void 
dbSorter::selectionChanged()
{
	if (!mSorting) 
		mNeedsToSort = true;  // don't assume we should sort now, they may not need the selection  
}


void 
dbSorter::backendSavedRecord(OOF_tableBackend*)
{
	assert(!mSorting);	// seems impossible, but....
	if (!mSuspended && mSortOnSaves)
 		   mNeedsToSort = true; //sortNow(whoChanged);  
}


void 
dbSorter::suspendSorting()
{
	mSuspended = true;
} 


void 
dbSorter::resumeSorting()
{
	mSuspended = false;
} 


unsigned short
dbSorter::sortFieldNo() const
{
	fieldNumT ret = 0; // default to first field in table, if no sort order
	if(countSegments())
		ret = fieldNumberOfSegment(0);
	return ret;
}


bool
dbSorter::sortsBySingleField() const
{
      return false;
}
 
 
unsigned long 
dbSorter::state() const
{
	unsigned long ret = 0;
	(mSortOnSaves ? ret |= 'O\0\0\0' : ret |= 'o\0\0\0');
	(mNeedsToSort ? ret |= '\0N\0\0' : ret |= '\0n\0\0');
	(mSorting ? ret 	 |= '\0\0S\0' : ret |= '\0\0s\0');
	(mSuspended ? ret 	 |= '\0\0\0X' : ret |= '\0\0\0x');

	return ret;
}
 

// -------------------------------------------------------
//                      d b F i e l d S o r t e r 
// -------------------------------------------------------
dbFieldSorter::dbFieldSorter(fieldNumT sortField, bool shouldReverse) : 
    mSortFieldNo(sortField),
    mReverseSort(shouldReverse)
{}
 
 
dbSorter* 
dbFieldSorter::clone() const
{
	dbFieldSorter* ret = 0;
	if (!mSorting) {
		ret = new dbFieldSorter(mSortFieldNo, mReverseSort);
	    ret->mSortOnSaves = mSortOnSaves;
	}
    return ret;
}


void 
dbFieldSorter::describe(ostream& os) const
{
	if (mReverseSort)
		os << "dbFieldSorter(reverse):\t";
	else
		os << "dbFieldSorter:\t";
	os << "Field Number: " << mSortFieldNo;
}


unsigned short
dbFieldSorter::sortFieldNo() const
{
      return mSortFieldNo;
}


bool
dbFieldSorter::sortsBySingleField() const
{
      return true;
}
 
 
void
dbFieldSorter::sortNow(OOF_tableBackend* sorts)
{
 OOF_DEBUG_LOG(stringstream() << flush << "dbFieldSorter::sortNow\t" 
	<< hex << (void*)this 
	<< "\tsorting: " << sorts->table()->tableName() << " (" << (void*)(sorts->table()) << ')'
);

   mNeedsToSort = false;
   mSorting = true;
   if (mReverseSort)
      sorts->sortSelectionInverseNow(mSortFieldNo);
   else
      sorts->sortSelectionNow(mSortFieldNo);
   mSorting = false;
}


void
dbFieldSorter::changeSortFieldTo(fieldNumT  newNum, bool shouldReverse)
{
   if ((mSortFieldNo==newNum) && (mReverseSort==shouldReverse))
      return;  // trivial case
   
   mSortFieldNo = newNum;
   mNeedsToSort = true;
   mReverseSort = shouldReverse;
   mSuspended = false;
}




// -------------------------------------------------------
//                  r e v e r s e d
// -------------------------------------------------------
// functor used instead of dbSorter::operator>> for people
// who prefer
// dbSorter() << field1 << reversed(field2);
// to
// dbSorter() << field1 >> field2

// NOTE the following 2 functions are prototyped by their friend declarations in
// class reversed
dbSorter& 
operator<<(dbSorter& os, const reversed& r)
{
	os >> r.Field();
	return os;
}


OOF_FieldSegOwner& 
operator<<(OOF_FieldSegOwner& os, const reversed& r)
{
	os >> r.Field();
	return os;
}


// -------------------------------------------------------
//               d b G U I F a c t o r y
// -------------------------------------------------------
dbGUIFactory::dbGUIFactory()
{
	if (sGUIFactory==0)
		sGUIFactory = this;
	else {
		oofE_GUIError excp(sGUIFactory, "You have more than one OOFILE GUI framework linked");
		RAISE_EXCEPTION(excp);
	}
}


dbGUI*
dbGUIFactory::makeGUI(dbTable* inTable) 
{
	dbGUI* ret = 0;
	if (sGUIFactory==0) {
		oofE_GUIError excp(sGUIFactory, "You have no OOFILE GUI framework linked");
		RAISE_EXCEPTION(excp);
	}
	else {
		ret = sGUIFactory->concreteMakeGUI(inTable);
		assert(ret);
	}
	return ret;
}


// -------------------------------------------------------
//           O O F _ m i x R e f C o u n t 
// -------------------------------------------------------
OOF_mixRefCount::OOF_mixRefCount(unsigned long initialRefs) :
		mReferences(initialRefs) 
{}


OOF_mixRefCount::~OOF_mixRefCount()
{
	assert(mReferences==0);
}

void
OOF_mixRefCount::incRefs()
{
	++mReferences;
}


void
OOF_mixRefCount::decRefs()
{
	assert(mReferences>0);  // otherwise unbalanced inc/dec
	if (mReferences>0)
		--mReferences;
	if (mReferences==0)
		delete this;
//later maybe have mDestroysSelf flag
}


#ifdef OOF_USING_EXCEPTIONS
// -------------------------------------------------------
//         s t L o c k R e c o r d 
// -------------------------------------------------------
stLockRecord::stLockRecord(dbTable* inTable) :
	mTable(inTable)
{
	InitLock();
}


stLockRecord::stLockRecord(dbTable& inTable) :
	mTable(&inTable)
{
	InitLock();
}


stLockRecord::~stLockRecord()
{
	if (mTable)
		mTable->unlockRecord();
}


void
stLockRecord::InitLock()
{
	try {
		mTable->lockRecord();	// may throw oofE_RecordLocked
	}
	catch (oofE_RecordLocked&) {
		mTable = 0;	
		throw;
	}
}	

#endif
// OOF_USING_EXCEPTIONS


// -------------------------------------------------------
//              s t C h a n g e S h i e l d e d 
// -------------------------------------------------------
stChangeShielded::stChangeShielded(dbTable* inTable) :
	mTable(inTable)
{
	if (mTable)
		mTable->changeShielded();
}


stChangeShielded::stChangeShielded(dbTable& inTable) :
	mTable(&inTable)
{
	mTable->changeShielded();
}


stChangeShielded::~stChangeShielded()
{
	if (mTable)
		mTable->changeShielded(false);
}


// -------------------------------------------------------
//         o o f R u n t i m e E n v 
// -------------------------------------------------------
oofRuntimeEnv::oofRuntimeEnvT 
oofRuntimeEnv::runtimeEnv()
{
	#ifdef _Unix
		return Unix;
	#elif defined _Macintosh
		return Macintosh;
	#else
	// runtime check for Windows OS type - see p194 "Programming the Win32 API"
		OSVERSIONINFO osver;
		osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!::GetVersionEx(&osver))
			return Win3;	// shouldn't be possible anyway
			
		switch(osver.dwPlatformId) {
		case VER_PLATFORM_WIN32_NT:
			return WinNT;
		case VER_PLATFORM_WIN32s:
			return Win32s;
		case VER_PLATFORM_WIN32_WINDOWS:
			return Win95;
		default:
			return Win_UNKNOWN;
		};
	#endif
}


void
oofRuntimeEnv::beep()
{
#ifdef _Macintosh
		::SysBeep(30);
#elif defined _Windows
		::MessageBeep(MB_OK);
#elif defined _Unix
	cout << 0x7;
#else
error platform unknown
#endif

}
