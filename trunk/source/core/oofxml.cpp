// COPYRIGHT 1999 A.D. Software, All rights reserved

// support classes for XML output including
// identifier generator class 

// Andy's disclaimer:
// this stuff was written primarily for a local project and under budget
// constraints that prevented efficiency or flexibility being considered
// in particular, I'd like more ease of efficiently creating tags!


#ifndef H_OOFXML
	#include "oofxml.h"
#endif 
#ifndef _CTYPE_H
	#include <ctype.h>
#endif

// -------------------------------------------------------
//            o o f I D f a c t o r y
// -------------------------------------------------------
oofIDfactory::oofIDfactory(char theSeparator,int indentDepth):
	mLevels(1,10,10),  // default value 1, don't think reports will go much beyond 10 levels
	mCurrentLevel(0),
	mIndentSize(indentDepth),
	mSeparator(theSeparator)
{
}


void
oofIDfactory::reset()
{
	mCurrentLevel = 0;
	mLevels.deleteAllCells();
}


void
oofIDfactory::leaveLevel()
{
	mCurrentLevel--;
	assert (mCurrentLevel >= 0);
}

	
oofString
oofIDfactory::getIDstring()
{
// could optimise by hanging onto the last string for each level above current
// and resetting when we leave a level, so aren't sprintf'ing so many times
	char scrapString[40]; 
	int index;
	mIDString = "";
	for(index=0;index<(mCurrentLevel+1);index++){
		sprintf(scrapString,"%d",mLevels[index]);
		mIDString += scrapString;
		if(index != mCurrentLevel){
			mIDString += mSeparator;
		}
	}
	return mIDString;
}


oofString
oofIDfactory::getQuotedIDstring()
{
// see getIDstring for optimisation comments
	char scrapString[40]; 
	mIDString = "'";
	for(int index=0;index<(mCurrentLevel+1);index++){
		sprintf(scrapString,"%d",mLevels[index]);
		mIDString += scrapString;
		if(index != mCurrentLevel){
			mIDString += mSeparator;
		}
	}
	mIDString += "'";
	return mIDString;
}


// -------------------------------------------------------
//            o o f T a g M a k e r
// -------------------------------------------------------
const char* oofTagMaker::kEncodedNameOptPrefix = "Z-_";
const short oofTagMaker::kEncodedNameOptPrefixLen = strlen(kEncodedNameOptPrefix);


oofString 
oofTagMaker::makeOpeningTag(const char* tag, const char* indentString, const char* idString, const char* otherAttributes, bool emptyTag)
{
	oofString result = indentString;
	result += '<';
	result += tag;
	if (idString && (strlen(idString)>0)) {
		result += " ID='";
		result += idString;
		result += '\'';
	}
	
	if (otherAttributes && (strlen(otherAttributes)>0)) {
		result += ' ';
		result += otherAttributes;
	}
	
	if (emptyTag)
		result += "/>\n";	
	else
		result += '>';	
	return result;
}


oofString 
oofTagMaker::makeClosingTag(const char *tag, const char* indentString)
{	
	oofString result = indentString;
	result += "</";
	result += tag;
	result += ">\n";
	return result;
}


oofString 
oofTagMaker::makeSimpleBoundedElement(const char* body, const char* tag, const char* indentString, const char* idString, const char* otherAttributes, bool /*emptyTag*/)
{
	oofString ret = oofTagMaker::makeOpeningTag(tag, indentString, idString, otherAttributes);
	ret += oofString::encodeEntity(body);
	ret += oofTagMaker::makeClosingTag(tag);
	return ret;
}


oofString 
oofTagMaker::encodeName(const oofString& inStr)
{
// encode all chars apart from alphanumeric
// spaces become underscore  (special case for common letter)
// hyphen becomes double-hyphen
// all others become hyphen-octal

// kEncodedNameOptPrefix = "Z-_" 
// recognisable string to drop on parsing (Z-_ in original would become Z---055)

// may not start with XML in any mixture of cases or leading non-letter, so add kEncodedNameOptPrefix
	const bool startsWithXML = 
		(inStr.length()>=3) &&
		(inStr[0]=='x' || inStr[0]=='X') &&
		(inStr[1]=='m' || inStr[1]=='M') &&
		(inStr[2]=='l' || inStr[2]=='L')
	;

	if (inStr.isAlphaNumeric()) {
		if (startsWithXML || isdigit(inStr[0]))
			return oofString(kEncodedNameOptPrefix, inStr);
		else
			return inStr;  //*** trivial exit - no conversion, including empty string
	}	
	oofString ret;
	const unsigned long inLen = inStr.length();
	const char* inChars = inStr;
	for (unsigned long i=0; i<inLen; i++) {
		const char ch = inChars[i];
		if (ch==' ') 
			ret +=  '_';
		else if (ch=='-') 
			ret +=  "--";
		else if (isalnum(ch))
			ret += ch;
		else {
			char octalNum[5];
			const unsigned short asShort = ch;
			sprintf(octalNum, "-%03ho", asShort);
			ret += octalNum;
		}
	}

	if (startsWithXML || !isalpha(ret[0]))
		return oofString(kEncodedNameOptPrefix, ret);
	else
		return ret;
}


oofString 
oofTagMaker::decodeName(const oofString& inStr)
{
// see comments at top of decodeName
	if (inStr.startsWith(kEncodedNameOptPrefix))
		return decodeName(inStr.subString(kEncodedNameOptPrefixLen) );  // recursively process after prefix
		// have to strip the prefix before processing further as it would be mangled by logic below
		
	if (inStr.isAlphaNumeric())	
		return inStr;  //*** trivial exit - no conversion, including empty string
		
	oofString ret;
	const unsigned long inLen = inStr.length();
	const char* inChars = inStr;
	for (unsigned long i=0; i<inLen; i++) {
		const char ch = inChars[i];
		if (ch=='_') 
			ret +=  ' ';
		else if (ch=='-') {
			i++;  // skip to next char
			assert(i<inLen);  // can't end with single hyphen
			if (inChars[i]=='-') {
				ret += '-';  // hyphen escape char mapped as double hyphen
			}
			else {
				assert(i<(inLen-2));  // must have 3 char octal code after hyphen if not double hyphen
				char octalNum[4];
				octalNum[0] = inChars[i];
				i++;
				octalNum[1] = inChars[i];
				i++;
				octalNum[2] = inChars[i];
				octalNum[3]='\0';
				unsigned short toShort;
				sscanf(octalNum, "%3ho", &toShort);
				ret += (char) toShort;
			}
		}
		else {
			assert (isalnum(ch));
			ret += ch;
		}
	}
	return ret;
}


// -------------------------------------------------------
//            o o f X M L w r i t e r
// -------------------------------------------------------

oofXMLwriter::oofXMLwriter() :
	mStr(xmlSimpleVersionString())
{
}


oofXMLwriter::~oofXMLwriter()
{
	const int numStrings = mElementNames.count();
	for (int i=0; i<numStrings; i++) {
		oofString* theString = (oofString*) mElementNames.value(i); // safe downcast
		delete theString;
	}
}


const char*
oofXMLwriter::xmlSimpleVersionString() 
{
	return "<?xml version='1.0' standalone='yes'?>\n";
}


/**
avoid creating temp string if not necessary by using ternary operator in calls
*/
void
oofXMLwriter::startElement(const char* inName, const char* otherAttributes, bool withID)
{
	const int theLevel = mIDlevels.currentLevel();
	assert(theLevel>=0);
	oofString* elementName = (oofString*) mElementNames.value(theLevel); // safe downcast
	if (!elementName) {
		elementName = new oofString(inName);
		mElementNames[theLevel] = (long)elementName;
	}
	else
		*elementName = inName;
		
	mStr += oofTagMaker::makeOpeningTag(
		inName, 
		theLevel>0 ? mIDlevels.getIndentString().chars() : NULL,
		withID ? mIDlevels.getIDstring().chars() : NULL,  
		otherAttributes
	);
	mStr += '\n';
	mIDlevels.enterLevel();
}


void
oofXMLwriter::endElement()
{
	mIDlevels.leaveLevel();
	const int theLevel = mIDlevels.currentLevel();
	assert(theLevel>=0);
	oofString* elementName = (oofString*) mElementNames.value(theLevel); // safe downcast
	assert(elementName);

	mStr += oofTagMaker::makeClosingTag(
		elementName->chars(), 
		mIDlevels.getIndentString()
	);
	elementName->clear();  // wipe so know we left element
	// NOT YET IMPLEMENTED - something a bit more efficient for
	// massive yo-yoing as the above clear() deletes string body
}


void
oofXMLwriter::addEmptyElement(const char* inName, const char* otherAttributes, bool withID)
{
	const int theLevel = mIDlevels.currentLevel();
	mStr += oofTagMaker::makeOpeningTag(
		inName, 
		theLevel>0 ? mIDlevels.getIndentString().chars() : NULL,
		withID ? mIDlevels.getIDstring().chars() : NULL,  
		otherAttributes,
		true // is empty
	);
}


void
oofXMLwriter::addSimpleElement(const char* inName, const char* inBody, const char* otherAttributes, bool withID)
{
	const int theLevel = mIDlevels.currentLevel();
	assert(theLevel>=0);
	mStr += oofTagMaker::makeSimpleBoundedElement(
		inBody,
		inName,
		theLevel>0 ? mIDlevels.getIndentString().chars() : NULL,
		withID ? mIDlevels.getIDstring().chars() : NULL,  
		otherAttributes
	);
}


bool
oofXMLwriter::topLevelClosed() const
{
	const int theLevel = mIDlevels.currentLevel();
	if (theLevel!=0)
		return false;
		
	const oofString* theString = (oofString*) mElementNames.value(0); // safe downcast
	const bool hasOpenElement = (theString && !theString->isEmpty());
	return !hasOpenElement;
}
