// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofxml.h

// support classes for XML output including
// Factory class for ID's corresponding to CSS and HTML syntax

#ifndef H_OOFXML
#define H_OOFXML

#ifndef H_OOFSTR
	#include "oofstr.h"
#endif
#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif

/**
	Generate nestable Dewey Decimal-style ID's to easily provide unique XML ID attributes.
	Optionally can replace the separator.
	Traacks the indent level of the current ID so making it easy to indent nested XML being
	written. Typically call enterLevel() and leaveLevel() as you descend the tree being
	written, and incrementID() each time a node is written.
*/
class OOFILE_EXPORT oofIDfactory{
public:
	oofIDfactory(char theSeparator  = '-',int indentDepth = 3);
	void reset();
	void enterLevel();
	void leaveLevel();
	void incrementID();
	oofString getIDstring();
	oofString getQuotedIDstring();
	
	oofString getIndentString();
	int currentLevel() const;
	
private:
	oofString mIDString;  // use member for ease of debugging, so can see last ID calculated
	int	mCurrentLevel;
	const int mIndentSize;
	const char mSeparator;
	OOF_ExpandableLongArray mLevels;
};


/**
	oofTagMaker generates xml tags
	general form foo(idString,tag,indentString)
	where result is 
	".......<tag ID="iDstring">"
	and "..... "is the indent string
	Line and space tags return the form
	"......<tag ID="IdString" width="aNumber" height="aNumber"/>" 
*/
class OOFILE_EXPORT oofTagMaker{
public:
	static oofString makeOpeningTag(const char* tag, const char* indentString=0, const char* ID=0, const char* otherAttributes=0, bool emptyTag=false);
	static oofString makeClosingTag(const char *tag, const char* indentString=0);
	static oofString makeSimpleBoundedElement(const char* body, const char* tag, const char* indentString=0, const char* ID=0, const char* otherAttributes=0, bool emptyTag=false);
	static oofString encodeName(const oofString&);
	static oofString decodeName(const oofString&);
	static const char* kEncodedNameOptPrefix;
	static const short kEncodedNameOptPrefixLen;
};


/**
	Generic class for writing XML.
	Provides simple methods to handle the translation and bracketing.
*/
class OOFILE_EXPORT oofXMLwriter {
	oofXMLwriter(const oofXMLwriter&) {};  // prevent copying
	void operator=(const oofXMLwriter&) {};  // prevent copying
	
public:
	oofXMLwriter();
	~oofXMLwriter();
	
// setters
	void startElement(const char*, const char* otherAttributes=0, bool withID=false);	
	void endElement();
	void addEmptyElement(const char*, const char* otherAttributes=0, bool withID=false);	
	void addSimpleElement(const char* inName, const char* inBody, const char* otherAttributes=0, bool withID=false);
	void addSimpleElement(const char* inName, int, const char* otherAttributes=0, bool withID=false);
	void addSimpleElement(const char* inName, double, const char* otherAttributes=0, bool withID=false);

// getters
	static const char*  xmlSimpleVersionString();
	const oofString& generatedXML() const;  // return the finished XML content
	bool topLevelClosed() const;
	
private:
	oofIDfactory	mIDlevels;	
	oofString mStr;
	OOF_ExpandableLongArray mElementNames;
};

// -------------------------------------------------------
//            o o f I D f a c t o r y
// -------------------------------------------------------
inline void
oofIDfactory::enterLevel()
{
	mCurrentLevel++;
}


inline void
oofIDfactory::incrementID()
{
	mLevels[mCurrentLevel]+=1;
}


inline oofString 
oofIDfactory::getIndentString()
{
	return oofString(' ', mCurrentLevel*mIndentSize);
}


inline int
oofIDfactory::currentLevel() const
{
	return mCurrentLevel;
}


// -------------------------------------------------------
//            o o f X M L w r i t e r
// -------------------------------------------------------
inline void 
oofXMLwriter::addSimpleElement(const char* inName, int inBody, const char* otherAttributes, bool withID)
{
	oofString theBody;
	theBody.convertNumber(inBody);
	addSimpleElement(inName, theBody, otherAttributes, withID);
}


inline void 
oofXMLwriter::addSimpleElement(const char* inName, double inBody, const char* otherAttributes, bool withID)
{
	oofString theBody;
	theBody.convertNumber(inBody);
	addSimpleElement(inName, theBody, otherAttributes, withID);
}


inline	const oofString& 
oofXMLwriter::generatedXML() const
{
	return mStr;
}

#endif

