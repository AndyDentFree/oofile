// Copyright 1998 A.D. Software. All Rights Reserved

// OOFTEST43

// Simple stream I/O is used to interact with the user.
// to demonstrate the cross-platform line ending conversion

// NOTE if you want this behaviour in your databases, you must
// call dbText::convertsLineEndingsOnLoad(true) once before loading any
// data, the default is not to convert

#include "ooftest.h"

int main()
{
	cout << "OOFILE Validation Suite - Test 43\n"
		 << "Shows String operations:\n"
		 << "1) Converts a number of embedded line endings\n"
		 << "to the current platform style\n\n"
		 << "2) Uses operators to manipulate strings\n"
		 << "=======================================\n";

	oofString theStr;
	theStr += "This is the first line, original ending just a Unix-style LF\n";
	theStr += "Second line, ending in DOS-style CRLF\r\n";
	theStr += "Third line, ending in Mac-style CR\r";

// NOTE on Mac with CodeWarrior the embedded \r characters prevent the string from
// displaying so don't panic when you fail to see it - you can use the debugger to see
// that the internal string buffer has been converted correctly
	cout << "Original string\n" << theStr << endl << endl;
	theStr.lineEndingsCurrentPlatform();
	cout << "The string now contains...\n" << theStr << endl << endl;

// now show some string manipulations
	theStr = "This is a phrase";
	cout << "Original phrase: '" << theStr << "'\n";
	cout << "Leftmost four chars: '" << theStr.left(4) << "'\n";
	cout << "Rightmost five chars: '" << theStr.right(5) << "'\n";
	
	theStr.replaceChar(' ', '*');
	cout << "Replaced spaces with asterisks: '" << theStr << "'\n";
	
	theStr.stripChar('*');
	cout << "Stripped those asterisks: '" << theStr << "'\n\n";
	
	oofCheckString checkString(123);
	cout << "A check (cheque for the English) string of 123: '" << checkString << "'\n";


// string replacements as used in constructing dynamic web pages
	oofString webString = "<!-- IndexMain = Luxury Home -->\n<!-- IndexSub = $625, 000 -->";
	
	cout << "initial web string" << endl << webString << endl;
	webString.replaceBetween("<!-- IndexSub = ", "-->", "$999, 000");  // update the price string

	cout << "after replaceBetween to update price" << endl << webString << endl;

// now get a substring
	long grabFrom = webString.findEnd("<!-- IndexMain =");
	if (grabFrom>=0) {
		long startTail = webString.find("-->", grabFrom);  // searches from grabFrom onwards
		assert(startTail >= 0);  // because our data says so!
		long subLen = startTail - grabFrom;
		oofString bitInMiddle = webString.subString(grabFrom, subLen);
		cout << "portion between '<!-- IndexMain =' and '-->' is: " << bitInMiddle << endl;
	}
	
// simple replace
	webString.replaceString("IndexSub", "Subbie");
	cout << "after replaceString 'IndexSub' with 'Subbie':" << endl << webString << endl;
		
// Possible Pathological testCases for replace
	oofString testStr = "oof" ;
	testStr.replaceString("o", "oofile");
	cout << "After  replaceString  'o' with 'oofile'  on the string \"oof\":" << endl << testStr << endl;

	testStr.replaceString("o", "MoreOOF");
	cout << "After  replaceString  'o' with 'MoreOOF'  on the string \"oofileoofilef\":" << endl << testStr << endl;
	
	testStr.replaceString("o", "jooffer");
	cout << "After  replaceString  'o' with 'oofile'  on the string \"MoreOOFMoreOOFfileMoreOOFMoreOOFfilef\":" << endl << testStr << endl;
	
	/////
	testStr = "sheepeep" ;
	testStr.replaceString("sheep", "sh");
	cout << "After  replaceString  'sheep' with 'sh'  on the string \"sheepeep\":" << endl << testStr << endl;

	testStr.replaceString("e", "");
	cout << "After  replaceString  'e' with ''  on the string \"sheep\":" << endl << testStr << endl;
	
	testStr.replaceString("", "jooffer");
	cout << "After  replaceString  '' with 'jooffer'  on the string \"shp\" (pathological case - no change):" << endl << testStr << endl;
	
		
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

