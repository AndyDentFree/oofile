// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST27

// this sample tests the word parser, and could be used to test any parsers
// that you create

// Simple stream I/O is used to interact with the user.
#include "oofwords.h"
#include "ooftest.h"	// just to get EXIT_SUCCESS on all platforms


int main()
{
	cout << "OOFILE Validation Suite - Test 27\n"
		 << "This tests the word parser used in generating keyword indices.\n\n";
	
	const char testStr[] = "This is a test Cabbage";
	const char * stopList[] = {"cabbage"};
	
	oofWordParser wp(testStr,3,stopList,1);

	cout << "The following parses '" << testStr << "' skipping:\n"
		 << "   'is' and 'a' because they are less than the minimum word length of 3\n"
		 << "   'cabbage' because it is on the list of 'stop words' which are ignored\n\n";
	for (wp.start();wp.more();wp.next()) {
		cout << wp.word() << endl;
	}
	
	char testStr2[] = "Even longer and sillier string about cabbage and cabbages and Kings";
	
	cout << "\n\n\nNow reparse with a different string, retaining the stoplist & min length\n"
		 << "'" << testStr2 << "'\n"
		 << "Note: a fancier word parser that does 'stemming' would also remove 'cabbages'\n"
		 << "by trimming it down to 'cabbage' and then dropping it because 'cabbage' is on\n"
		 << "the stop words list.\n\n";
	wp.start(testStr2);
	cout << wp << endl;		// just to show the direct output operator
	cout << endl <<"Test Completed" << endl;
	
	return EXIT_SUCCESS;
} 

