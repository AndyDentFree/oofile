// oofWordParser.h: oofWordParser class definition
// plus other string utilities

#ifndef H_OOFWORDS
#define H_OOFWORDS

#ifndef H_OOF2
	#include "oof2.h"
#endif

/**
	Base class for user-replaceable word parser.
	Used mainly to search tables and word-indexable fields. Default behaviour in
	this base class is used if no overriding default object is attached to
	a table or dbConnection.
	
	includes stop-word list and minimum length.
	Doesn't do any stemming or more complex parsing.
	
	A word is generated from the isalnum() chars between each
	isspace() separators. Any non alnum chars inside a word are skipped.
	
	mpString remains a pointer to the start of the current word.
	mWord is updated (in theory) with the filtered copy of the word.
	\bug SPOTTED 14 Nov 2000 - calling start() again after using parser
	doesn't update mWord
	
	To aid in search implementation, generateWordArray() creates a separate list
	of strings which is retained and can be accessed by generatedStrings()
	and generatedCount() 
	\see dbTable::searchSelContainsAnyOf for example of use.
	
	\note not to be regarded as good example of programming style until
	parser cleaned up and storage neater!
	
	\todo cleaner abstraction of word source iteration
	\todo work out why minWordLength returns a reference and add setter if necessary
	\ingroup oofUtils
*/
class OOFILE_EXPORT oofWordParser : public OOF_mixRefCount {
public:
	oofWordParser(const char* stringToParse=0, unsigned short minWordLen = 3,
		const char** stopWords = 0, unsigned short numStopWords = 0);
	virtual ~oofWordParser();
	
	virtual void start();
	virtual void start(const char* stringToParse);	///< call to start parsing different string
	virtual void next(void);
	virtual bool more() const;
	virtual const char* word() const;
	virtual const char* wordString() const;
	virtual unsigned short& minWordLength();
	virtual bool isValidWord(const char *) const;

	virtual void generateSearchArray(); 
	const char** generatedStrings() const;
	unsigned long generatedCount() const;
	
	void	extract(ostream&);

protected:
	enum {maxWordLength = 40};
	const char* mpString;  ///< pointer to start of words to process, may point to mGeneratedStrings entries
	const char* mpStringStart;  ///< pointer to original strings
	char mWord[maxWordLength];
	unsigned short mWordLength;
	unsigned short mMinWordLength;
	const char** mStopWords;  ///< passed in list of stop words to ignore
	unsigned short mNumStopWords;
	const char** mGeneratedStrings;  ///< owned strings generated for subsearch call
	unsigned long mGeneratedCount;
	unsigned long mIterStrings;
};


/**
	return array of pointers to individual words.
*/
inline const char** 
oofWordParser::generatedStrings() const
{
	return mGeneratedStrings;
}


inline unsigned long 
oofWordParser::generatedCount() const
{
	return mGeneratedCount;
}


OOFILE_EXPORT inline ostream& 
operator<<(ostream& os, oofWordParser& wp)
{
	wp.extract(os);
	return os;
}


/**
	Contain a list of words parsed from an input string or field being word-indexed.
	\see oofWordParser::generateSearchArray 
	\see OOF_ctreeKeywordIndex
	\ingroup oofUtils
*/
class	OOFILE_EXPORT OOF_WordList {
public:
	OOF_WordList();
	~OOF_WordList();
	bool contains(const char*) const;
	void append(const char*);
	const char** orphanStringArray(unsigned long& outCount);

private:
	OOF_ExpandableLongArray	mWords;	
};

#endif

