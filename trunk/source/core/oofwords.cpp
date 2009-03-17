/*	oofWords.cpp: oofWordParser class implementation
	Note that within valid words, punctuation is ignored
*/

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
#ifndef H_OOFRAM
	#include "oofram.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif

#include <ctype.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif



// -------------------------------------------------------
//                     o o f W o r d P a r s e r
// -------------------------------------------------------
oofWordParser::oofWordParser(const char * s, unsigned short minWordLen,
	const char ** stopWords, unsigned short numStopWords) :
	mpString(0),
	mpStringStart(0),
	mMinWordLength(minWordLen),
	mStopWords(stopWords),
	mNumStopWords(numStopWords),
	mGeneratedStrings(0),
	mGeneratedCount(0),
	mIterStrings(0)
{
	if (s)
		start(s);
} // oofWordParser


/**
	dtor.
	Delete the const char* strings owned in mGeneratedStrings, if we ever generated them!
	\todo fix mGeneratedWords management so deletes without complaint
*/
oofWordParser::~oofWordParser()
{
	mReferences = 0;	// prevent assertion in parent class if someone deletes us
						// like having a stack-based word parser
	if (mGeneratedStrings) {
		const unsigned long numWords = mGeneratedCount;
		for (unsigned int i=0; i<numWords; i++) {
			#ifdef _Windows
				delete[] (char*)(mGeneratedStrings[i]); // hack for Windows
			#else
				delete[] mGeneratedStrings[i]; 
			#endif
		}
		delete[] mGeneratedStrings;
	}
}


void
oofWordParser::start(const char* s)
// skips over non-alphanumerical characters
// and primes pointers to the first word
{
	if (mGeneratedStrings) {
		mIterStrings = 0;
		mpString = mGeneratedStrings[0];
		strncpy(mWord, mpString, maxWordLength);
	}
	else {
		// position start of string
		mpStringStart = mpString = s;
		bool firstWord = true;
		do {
			// skip over (invalid) word to space
			int i;
			if (!firstWord) {
				mpString += mWordLength;
				for (i = 0; mpString[i]&&!isspace(mpString[i]); ++i)
					;
				mpString += i;
			}
			// skip over spaces and non-printable characters
			for (i = 0; mpString[i]&&!isalnum(mpString[i]); ++i)
				;
			mpString += i;
			// write each valid character to mWord
			int p = 0;
			mWordLength = 0;
			for (i = 0; mpString[i]&&!isspace(mpString[i]); ++i)
				if (!ispunct(mpString[i]))
					mWord[p++] = mpString[i];
			mWord[p] = '\0';
			mWordLength = p;
			firstWord = false;
		} while ((mWordLength>0)&&!isValidWord(mWord));
	}
} // start


/**
Find the next word, updating the mWord pointer returned by word();
\todo refactor to isolate the common parsing code in start() & here
\note I don't know who wrote this - it wasn't Andy!
*/
void oofWordParser::next()
// primes the private string pointer to the start
// of the next valid word
{
	assert(mpStringStart);
	if (mGeneratedStrings) {
		mIterStrings++;
		mpString = mGeneratedStrings[mIterStrings];
		mWordLength = strlen(mpString);
		strncpy(mWord, mpString, maxWordLength);
	}
	else do {
		// skip over current word to space
		int i;
		mpString += mWordLength;
		for (i = 0; mpString[i]&&!isspace(mpString[i]); ++i)
			;
		mpString += i;
		// skip over spaces and non-printable characters
		for (i = 0; mpString[i]&&!isalnum(mpString[i]); ++i)
			;
		mpString += i;
		// write each valid character to mWord
		int p = 0;
		mWordLength = 0;
		for (i = 0; mpString[i]&&!isspace(mpString[i]); ++i)
			if (!ispunct(mpString[i]))
				mWord[p++] = mpString[i];
		mWord[p] = '\0';
		mWordLength = p;
	} while ((mWordLength>0)&&!isValidWord(mWord));
} // next


bool oofWordParser::isValidWord(const char *s) const
// returns true if word is valid, otherwise false
{
	// check for a word of valid length
	if (mWordLength < mMinWordLength)
		return false;
	// check for a stop word
	bool stopWord = false;
	for (int i = 0; i < mNumStopWords; ++i)
		if (OOF_stricmp(s,mStopWords[i])==0) {
			stopWord = true;
			break;
		}
	// return false if a stop word
	if (stopWord)
		return false;
	return true;
} // isValidWord


/**
	sets the oofWordParser to the start of the string.
	\todo Fix existing bug where re-starts do NOT reset the content
	of mWord. Note that both start(const char*) and next() have a
	copy to mWord which further filters the letters!!!!
*/
void 
oofWordParser::start()
{
	assert(mpStringStart);
	if (mGeneratedStrings) {
		mIterStrings = 0;
		mpString = mGeneratedStrings[0];
		strncpy(mWord, mpString, maxWordLength);
	}
	else {
		if (mpStringStart != mpString) {
			start(mpStringStart);
		}
	}
}


/**
	return true if any words left.
	\todo make this more reliable, preparse the next word at next() and start()
*/
bool 
oofWordParser::more() const
{	
	assert(mpStringStart);
	return (mpString[0]!=0);
}


/**
	returns the current word.
*/
const char* 
oofWordParser::word() const
{
	assert(mpStringStart);
	return mWord;
}


/**
	returns the original string.
*/
const char* 
oofWordParser::wordString() const
{
	return mpStringStart;
}



/**
	 returns/sets the minimum word length cut-off.
*/
unsigned short& 
oofWordParser::minWordLength()
{
	return mMinWordLength;
}


void
oofWordParser::extract(ostream& os)
{
	for (start(); more(); next()) {
		os << word() << endl;
	}	
}


/**
	\todo make more efficient
*/
void 
oofWordParser::generateSearchArray()
{
	if (mGeneratedStrings)
		return;  // have parsed!
		
	OOF_WordList tempWords;
	for (start(); more(); next()) {
		tempWords.append(word());
	}	
	mGeneratedStrings = tempWords.orphanStringArray(mGeneratedCount);		
}


// -------------------------------------------------------
//           O O F _ U n i q u e W o r d L i s t
// -------------------------------------------------------
// NOT YET IMPLEMENTED - use trie or something else that's better at contains()

OOF_WordList::OOF_WordList()
{
}


OOF_WordList::~OOF_WordList()
{
	const unsigned long numWords = mWords.count();
	for (unsigned int i=0; i<numWords; i++) {
		char* aWord = (char*) mWords.value(i);
		delete[] aWord;
	}
}



const char**
OOF_WordList::orphanStringArray(unsigned long& outCount)
{
	const unsigned long numWords = mWords.count();
	const char** retWords = new const char*[numWords];
	for (unsigned int i=0; i<numWords; i++) {
		retWords[i] = (char*) mWords.value(i);
		mWords[i] = 0;
	}
	outCount = numWords;
	return retWords;
}


bool 
OOF_WordList::contains(const char* inWord) const
{
	const unsigned long numWords = mWords.count();
	for (unsigned int i=0; i<numWords; i++) {
		const char* aWord = (const char*) mWords.value(i);
		if (OOF_stricmp(aWord, inWord)==0)
			return true;
	}
	return false;
}


void 
OOF_WordList::append(const char* inWord)
{
	const unsigned short wordLen = strlen(inWord);
	char* newWord = new char[wordLen+1];
	memcpy(newWord, inWord, wordLen);
	newWord[wordLen] = '\0';
	mWords.append((unsigned long)newWord);
}


// -------------------------------------------------------
//         d b T a b l e
// -------------------------------------------------------
dbQueryBinaryNofield	
dbTable::hasWord(const char* str) const
{
	return dbQueryBinaryNofield(dbQueryClause::hasWord, new dbQueryLiteralStr(str));
}


dbQueryBinaryNofield	
dbTable::hasWordStartsWith(const char* str) const
{
	return dbQueryBinaryNofield(dbQueryClause::hasWordStartsWith, new dbQueryLiteralStr(str));
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsDelimited(const char* str, char delimiter)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAnyWord, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinaryNofield	
dbTable::hasAllWordsDelimited(const char* str, char delimiter)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAllWords, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAnyWord, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinaryNofield	
dbTable::hasAllWordsOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAllWords, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAnyWordsOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinaryNofield	
dbTable::hasAllWordsOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAllWordsOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsStartsWithDelimited(const char* str, char delimiter)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAnyWordStartsWith, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinaryNofield	
dbTable::hasAllWordsStartsWithDelimited(const char* str, char delimiter)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAllWordsStartsWith, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsStartsWithOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAnyWordStartsWith, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinaryNofield	
dbTable::hasAllWordsStartsWithOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinaryNofield(dbQueryClause::hasAllWordsStartsWith, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinaryNofield	
dbTable::hasAllWordsStartsWithOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAllWordsStartsWithOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinaryNofield	
dbTable::hasAnyWordsStartsWithOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAnyWordsStartsWithOf(inParser->generatedStrings(), inParser->generatedCount());
}


// -------------------------------------------------------
//              O O F _ m i x K e y w o r d a b l e 
// -------------------------------------------------------
OOF_mixKeywordableField::OOF_mixKeywordableField(dbField* thisField) :
	mParser(0),
	mThisField(thisField)
{
}


OOF_mixKeywordableField::OOF_mixKeywordableField(const OOF_mixKeywordableField& rhs, dbField* rhsAsField) :
	mParser(rhs.mParser),
	mThisField(rhsAsField)
{
	if (mParser)
		mParser->incRefs();
}



OOF_mixKeywordableField::~OOF_mixKeywordableField()
{
	if (mParser)
		mParser->decRefs();
}


dbQueryBinary	
OOF_mixKeywordableField::hasWord(const char* str) const
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasWord, new dbQueryLiteralStr(str));
}


dbQueryBinary	
OOF_mixKeywordableField::hasWordStartsWith(const char* str) const
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasWordStartsWith, new dbQueryLiteralStr(str));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsDelimited(const char* str, char delimiter)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAnyWord, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsDelimited(const char* str, char delimiter)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAllWords, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAnyWord, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAllWords, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAnyWordsOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAllWordsOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsStartsWithDelimited(const char* str, char delimiter)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAnyWordStartsWith, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsStartsWithDelimited(const char* str, char delimiter)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAllWordsStartsWith, new dbQueryLiteralStrDelimited(str, delimiter));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsStartsWithOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAnyWordStartsWith, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsStartsWithOf(const char **schStrs, unsigned long count)
{
	return dbQueryBinary(new dbQueryField(mThisField), dbQueryClause::hasAllWordsStartsWith, new dbQueryLiteralStrArray(schStrs, count));
}


dbQueryBinary	
OOF_mixKeywordableField::hasAnyWordsStartsWithOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAnyWordsStartsWithOf(inParser->generatedStrings(), inParser->generatedCount());
}


dbQueryBinary	
OOF_mixKeywordableField::hasAllWordsStartsWithOf(oofWordParser* inParser)
{
	inParser->generateSearchArray();
	return hasAllWordsStartsWithOf(inParser->generatedStrings(), inParser->generatedCount());
}


void
OOF_mixKeywordableField::indexWords(oofWordParser*	adoptedParser)
{
#ifdef OOF_DEBUG
	if (mThisField->fieldIsStandalone())
		dbConnect::raise( "Attempt to specify that standalone field has keyword index", false);
	else
#endif	
	{
		if (adoptedParser)
			mParser = adoptedParser;
		else {
			mParser = mThisField->fieldTable()->getDefaultWordParser();
			mParser->incRefs();
		}
	}
}


oofWordParser*	
OOF_mixKeywordableField::wordParser() const
{
	if (mParser)
		return mParser;
	else
		return mThisField->fieldTable()->getDefaultWordParser();
}
