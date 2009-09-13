// Copyright 1999 A.D. Software. All Rights Reserved

// OOFTEST52

// This sample tests the copy and move file methods of oofFileRef

#include "ooftest.h"
#include "ooffiles.h"
#include "ooftst02.h"

// global variables that define the database using the ooftst02 classes
// summary: 
/*
create a directory source:
add a file testfile to source

create a directory dest:

copy test file from source to dest
check source:testfile ==  dest:testfile

delete dest:testfile

move test file from source to dest
check source : empty
check dest: has file and file is ok
*/	


#ifdef _Macintosh
	#include <StandardFile.h>
#endif

int main()
{
		
	
	cout << "OOFILE Validation Suite - Test 52\n"
		 << "Simple test to verify the function of"
		 << " oofFileRef::copyToDirectory\n"
		 << " oofFileRef::moveToDirectory\n"
		 << "also it will verify oofdir constructor with nested relative names." << endl;


	
	
	oofDirectory testSource(":testDirectory:source:",0,true);
	oofDirectory testDestination(":anothertestDirectory:aNest:destination:",0,true);
	oofDirectory testDestinationTwo(":anothertestDirectory:aNest:aBurrow:destination:",0,true);
	
	// make the test src file
		oofFileRef srcRef(testSource,"testFile.txt");
		FILE *fp = srcRef.fopen("w");
		fprintf( fp,"The quick brown fox jumps over the lazy dog\n");
		fclose( fp );
	
	// copy it
		srcRef.copyToDirectory(testDestination, true);
		char s[100];
		oofFileRef testRefA(testDestination,"testFile.txt");
	
		cout << "The test file should contain the string:\n"
			 << "The quick brown fox jumps over the lazy dog\n";
	// and verify 
		fp = testRefA.fopen("r");  // shows our fopen method which calls the ANSI fopen
		printf("The copied test file contains the string:\n");
		while (fscanf( fp, "%s", s ) != EOF)
				printf( "%s ", s );
		printf( "\n\n");
		fclose( fp );
	
	// move a file
		testRefA.moveToDirectory(testDestinationTwo,true);
	// and verify
		cout << "The test file should contain the string:\n"
			 << "The quick brown fox jumps over the lazy dog\n";
		
		oofFileRef testRefB(testDestinationTwo,"testFile.txt");
	
		const oofString fileContents = testRefA.readTextFile();
		cout << "The moved test file contains the string:\n"
			 << fileContents << endl;
	
#ifdef _Macintosh
		cout << "This optional test allows the move and copy functions to be tested across drives"
			 << "press cancel to bypass this test if required\n" 
			 << "manually check results" << endl;

		cout << endl << "CR to continue" << flush;
		cin.ignore(1);
			
	
		StandardFileReply reply;
		StandardGetFile(0,-1,0,&reply);
		if(reply.sfGood){
			oofDirectory RemoteCopySource(reply.sfFile);
			oofFileRef remoteSrcRef(RemoteCopySource,reply.sfFile.name);
			remoteSrcRef.copyToDirectory(testDestination, true);
		}		
		StandardGetFile(0,-1,0,&reply);
		if(reply.sfGood){
			oofDirectory RemoteMoveSource(reply.sfFile);
			oofFileRef remoteSrcReftwo(RemoteMoveSource,reply.sfFile.name);
			remoteSrcReftwo.moveToDirectory(testDestinationTwo, true);
		}
#endif // _Macintosh

	return EXIT_SUCCESS;
}       
