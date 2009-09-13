// Copyright 1999 A.D. Software. All Rights Reserved

// OOFTEST49

// This sample test the oofstring oofString::relativePathxxx functions
#include "ooftest.h"
#include "ooffiles.h"
#include "oofstr.h"



int main()
{
		
	
	cout << "OOFILE Validation Suite - Test 49\n"
		 << "Simple test to verify the function of"
		 << " oofPathString::relativePathMac\n"
		 << " oofPathString::relativePathDos\n"
		 << " oofPathString::relativePathUnix\n"
		 << " oofPathString::relativePathPlatform\n"
		 << " isRelativePath \n"
		 << endl;
		
		
		
		
		cout << "First we test the static functions" << endl << endl;
		 int len = 5;
		// to DOS Conversions
		 cout << "Mac Path :blah converts to DOS path" << endl; 
		 cout  << oofPathString::relativePathDOS(":blah",len)<< endl ;
		 
		 cout << "Mac Path blah:nasty:evilname\\/ converts to DOS path" << endl; 
		 len = 22;
		 cout  << oofPathString::relativePathDOS(":blah:nasty:evilname\\/ ",len)<< endl ;
		
		// cout  "MAC Path blah converts to DOS path" << endl 
		// cout  << oofPathString::relativePathDos(":blah")<< endl ;
		len =6;
		 cout  <<"Unix Path ./blah converts to DOS path" << endl; 
		 cout  << oofPathString::relativePathDOS("./blah",len)<< endl ;
		 cout  <<"DOS Path .\\blah converts to DOS path" << endl ;
		 cout  << oofPathString::relativePathDOS(".\\blah",len)<< endl << endl ;
		
		//To mac conversion
		len = 6;
		 cout  <<"Unix Path ./blah converts to Mac path" << endl; 
		 cout  << oofPathString::relativePathMac("./blah",len)<< endl ;
		
		 len = 6;		
		 cout  <<"DOS Path .\\blah converts to Mac path" << endl ;
		 cout  << oofPathString::relativePathMac(".\\blah",len)<< endl ;
		
		 len = 	5;	
		 cout << "Mac Path :blah converts to Mac path" << endl; 
		 cout  << oofPathString::relativePathMac(":blah",len)<< endl << endl ;
		
		//to unix 
			 cout << "Mac Path :blah converts to Unix path" << endl; 
		 cout  << oofPathString::relativePathUnix(":blah",len)<< endl ;
		 
		 cout << "Mac Path blah:nasty:evilname\\/ converts to Unix path" << endl; 
		 len = 22;
		 cout  << oofPathString::relativePathUnix(":blah:nasty:evilname\\/ ",len)<< endl ;
		
		// cout  "MAC Path blah converts to DOS path" << endl 
		// cout  << oofPathString::relativePathDos(":blah")<< endl ;
		len =6;
		 cout  <<"DOS Path .\\blah converts to Unix path" << endl ;
		 cout  << oofPathString::relativePathUnix(".\\blah",len)<< endl << endl ;
		 cout  <<"Unix Path ./blah converts to Unix path" << endl; 
		 cout  << oofPathString::relativePathUnix("./blah",len)<< endl<<endl ;
		
		oofPathString testString = "./blah";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << "converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << " converts to DOS string: "<<endl; 
		testString.relativePathDOS();
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix();
		cout << testString << endl;
		
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix();
		cout << testString << endl<<endl;
		
		
		cout << "Now try out the string content function" << endl;
		
		testString = "../blah";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << "converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << " converts to DOS string: "<<endl; 
		testString.relativePathDOS();
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix();
		cout << testString << endl;
		
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac();
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix();
		cout << testString << endl<<endl;
		
		cout << "Now try out the suppress apphendSeperator functionality" << endl;
		//testString ":ooftst01:People.dat";
		testString = ".\\TestDir\\TestSubDir\\People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac(false);
		cout << testString << endl;
			cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac(false);
		cout << testString << endl;
		
		cout << testString << " converts to DOS string: "<<endl; 
		testString.relativePathDOS(false);
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix(false);
		cout << testString << endl;
		
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathMac(false);
		cout << testString << endl;
		
		cout << testString << " converts to Unix string: "<<endl; 
		testString.relativePathUnix(false);
		cout << testString << endl<<endl;
		
		cout << "Test try out relativePathPlatform() function apphend on" << endl<<endl;
		testString = ".\\TestDir\\TestSubDir\\People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform();
		cout << testString << endl<<endl;
		
		testString = "./TestDir/TestSubDir/People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform();
		cout << testString << endl<<endl;
		
		testString = ":TestDir:TestSubDir:People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform();
		cout << testString << endl<<endl;
		
		cout << "Test try out relativePathPlatform() function apphend off" << endl<<endl;
		testString = ".\\TestDir\\TestSubDir\\People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform(false);
		cout << testString << endl<<endl;
		
		testString = "./TestDir/TestSubDir/People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform(false);
		cout << testString << endl<<endl;
		
		testString = ":TestDir:TestSubDir:People.dat";
		cout << testString << " converts to mac string: "<<endl; 
		testString.relativePathPlatform(false);
		cout << testString << endl<<endl;
		
		cout << "Finally test the path test functions " << endl<<endl;
		
		testString = ":TestDir:TestSubDir:People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl;
		testString = "TestDir:TestSubDir:People.dat";
		cout << testString << (oofPathString::isRelativePath(testString )? ("is relative") :("is not relative")) << endl;
		testString = "::TestDir:TestSubDir:People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl<< endl;
		
		testString = "./TestDir/TestSubDir/People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl;
		testString = "../TestDir/TestSubDir/People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl;
		testString = "TestDir/TestSubDir/People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl<< endl;
		
		testString = ".\\'TestDir\\TestSubDir\\People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl;
		testString = "..\\TestDir\\TestSubDir\\People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl;
		testString = "TestDir\\TestSubDir\\People.dat";
		cout << testString << (oofPathString::isRelativePath(testString) ? ("is relative") :("is not relative")) << endl<< endl;
		
		testString = ":TestDir:TestSubDir:People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl;
		testString = "TestDir:TestSubDir:People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
	
		testString = "./TestDir/TestSubDir/People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl;
		testString = "TestDir/TestSubDir/People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
	
		testString = ".\\TestDir\\TestSubDir\\People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl;
		testString = "TestDir\\TestSubDir\\People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
	
		//
		testString = "::TestDir:TestSubDir:People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
		
		testString = "../TestDir/TestSubDir/People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
		
		testString = "..\\TestDir\\TestSubDir\\People.dat";
		cout << testString << " is of type " << oofPathString::pathType(testString) << endl<< endl;
		
	
		cout << "Test Finnished."<<endl<<endl;
		
		
	return EXIT_SUCCESS;
}       