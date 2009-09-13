// Copyright 1996 A.D. Software. All Rights Reserved

// OOFTEST30

// Simple stream I/O is used to interact with the user.
// to demonstrate the use of all standalone field types

// see also ooftst05 for a wide variety of date input tests

#include "ooftest.h"

int 
main()
{
	cout << "OOFILE Validation Suite - Test 30\n"
		 << "Simple test to operations on all the standalone field types"
		 << endl << endl;

	cout << "change the default date order to Month, Day Year" << endl;

	dbDate::sDefaultDateOrder=dbDate::orderMDY;

	{
// date ctors
	oofDate first;
	first = dbDate::currentDate();
	oofDate second = first;
	oofDate third("Jan 15th '97");
// date ops
	second -= 14;
	++third;
	
	cout << "\nTesting oofDate:"
		 << "\nToday: " << first 
		 << "\nTwo weeks ago: " << second 
		 << "\nJan 16th '97 " << third 
		 << endl << endl;
	}
	
	
	{
// date-time ctors
	oofDateTime first;
	first = dbDateTime::currentTime();
	oofDateTime second = first;
	oofDateTime third("Jan 15th '97 3:15 pm");
// date ops
	second -= 14;
	++third;
	
	cout << "\nTesting oofDateTime:"
		 << "\nToday: " << first 
		 << "\nTwo weeks ago: " << second 
		 << "\nJan 16th '97 3:15 pm " << third 
		 << endl << endl;
	}
	
	
	{
// time ctors
	oofTime first;
	first = dbTime::currentTime();
	oofTime second = first;
	oofTime third("3:15:45.32 pm");
// time ops
	second -= 3000;  // milliseconds
	++third;
	
	cout << "\nTesting oofTime:"
		 << "\nNow: " << first 
		 << "\n3000 milliseconds (3 seconds) earlier: " << second 
		 << "\n3:15:45.32 pm " << third 
		 << endl << endl;
	}
	
	
	{
// text ctors
	oofText first; // length defaults to 80
	first = "Hi There";
	oofText second = first;
	oofText third("Jan 15th '97");
	oofText fourth;	// test append to empty
// text ops
	second += ' ';	// append char, not string
	second += "George";
	second += "";	// test pathological case - append an empty string	
	fourth += "appended to empty";
	
	cout << "\nTesting oofText:"
		 << "\n'Hi There' " << first 
		 << "\n'Hi There George' " << second 
		 << "\n'Jan 15th '97' " << third 
		 << "\n'appended to empty' " << fourth 
		 << endl << endl;
	}

	
	{
// string ctors - our simple string type, NOT a database type
	OOF_String first; 
	first = "Hi There";
	OOF_String second = first;
	OOF_String third("Jan 15th '97");
	OOF_String fourth;	// test append to empty
	OOF_String fifth, sixth, seventh;
// char ops
	second += ' ';	// append char, not string
	second += "George";
	second += "";	// test pathological case - append an empty string
	fourth += "appended to empty";
	fifth.convertNumber(12345);
	sixth.convertNumber(123.456);
	seventh.convertNumber(123456, "printf test: %d");
	cout << "\nTesting OOF_String simple string class:"
		 << "\n'Hi There' " << first 
		 << "\n'Hi There George' " << second 
		 << "\n'Jan 15th '97' " << third 
		 << "\n'appended to empty' " << fourth 
		 << "\n'123456' " << fifth 
		 << "\n'123.456' " << sixth 
		 << "\n'printf test: 123456' " << seventh 
		 << endl << endl;
	}
	
	
	{
// char ctors
	oofChar first; // length defaults to 80
	first = "Hi There";
	oofChar second = first;
	oofChar third("Jan 15th '97");
	oofChar fourth;	// test append to empty
// char ops
	second += ' ';	// append char, not string
	second += "George";
	second += "";	// test pathological case - append an empty string
	fourth += "appended to empty";
	
	cout << "\nTesting oofChar:"
		 << "\n'Hi There' " << first 
		 << "\n'Hi There George' " << second 
		 << "\n'Jan 15th '97' " << third 
		 << "\n'appended to empty' " << fourth 
		 << endl << endl;
	}
	
	
	{
// unsigned short ctors
	oofUshort first; 
	first = 99;
	oofUshort second = first;
	oofUshort third(1001);
// unsigned short ops
	second  = second + 2; //+= 2;	// later
	
	cout << "\nTesting oofUshort:"
		 << "\n'99' " << first 
		 << "\n'101' " << second 
		 << "\n'1001' " << third 
		 << endl << endl;
	}
	
	
	{
// short ctors
	oofShort first; 
	first = -99;
	oofShort second = first;
	oofShort third(1001);
// short ops
	second  = second - 2; //-= 2;	// later
	
	cout << "\nTesting oofShort:"
		 << "\n'-99' " << first 
		 << "\n'-101' " << second 
		 << "\n'1001' " << third 
		 << endl << endl;
	}
	
	
	{
// unsigned long ctors
	oofUlong first; 
	first = 99;
	oofUlong second = first;
	oofUlong third(1000001);
// unsigned long ops
	second  = second + 2; //+= 2;	// later
	
	cout << "\nTesting oofUlong:"
		 << "\n'99' " << first 
		 << "\n'101' " << second 
		 << "\n'1000001' " << third 
		 << endl << endl;
	}
	
	
	{
// long ctors
	oofLong first; 
	first = -99;
	oofLong second = first;
	oofLong third(1000001);
// long ops
	second  = second - 2; //+= 2;	// later
	
	cout << "\nTesting oofLong:"
		 << "\n'-99' " << first 
		 << "\n'-101' " << second 
		 << "\n'1000001' " << third 
		 << endl << endl;
	}
	
	
	{
// bool ctors
	oofBool first; 
	first = true;
	oofBool second = first;
	oofBool third("True");
#ifdef OOF_HAS_BOOL
	oofBool fourth("N");// cannot pass  chars like 'N', as as ansii  bool seems to treat all non zero values as true
#else
	oofBool fourth('N');  // pre-ANSI nice idiom for evaluating char constants
#endif
	
	oofBool fifth;
// bool ops
	second  = !second;
#ifdef OOF_HAS_BOOL
	fifth = 'F';	
	cout 	<< "If fifth is an oofBool.Then fifth = 'F' gives: "
			<< "\n  " << fifth << endl ;
	fifth = dbBool::char2bool('F');	
	cout	<< "Instead must use dbBool::char2bool('F')" 
			<< "Which gives :" 
			<< "\n  " << fifth << endl ;
			fifth.setString ("F");
	cout	<< " Or  fifth.setString(\"F\")" 
			<< "Which gives :" 
	 			<< "\n  " << fifth << endl << endl ;
#else	
	fifth = 'T';	
#endif
/*#ifdef _MSC_VER // usual overload resolution fun & games
	fifth = bool(0);	
	fifth = bool(1);
#else
*/
	fifth = 0;	
	fifth = 1;
	oofBool sixth;
	sixth.setString("n");
	oofBool seventh("1");
	
//#endif
	cout << "\nTesting oofBool:"
		 << "\n'T' " << first 
		 << "\n'F' " << second 
		 << "\n'T' " << third 
		 << "\n'F' " << fourth 
		 << "\n'T' " << fifth 
		 << "\n'F' " << sixth 
		 << "\n'T' " << seventh 
		 << endl << endl;
	}
	
	
	{
// real ctors
	oofReal first; 
	first = -0.99;
	oofReal second = first;
	oofReal third(1000001);
// real ops
	second  = second - 0.02; //-= 0.02;	// later
	
	cout << "\nTesting oofReal:"
		 << "\n'-0.99' " << first 
		 << "\n'-1.01' " << second 
		 << "\n'1000001' " << third 
		 << endl << endl;
	}
	
	
	{
	long testBuf[] = {0, 2, 4, 8, 16, 65535, 0xADADADAD};
	long bufLen = sizeof(testBuf);
	oofBLOB first;
	first.setBytes(testBuf, bufLen);
	oofBLOB second = first;
	oofBLOB third;	// test append to empty
// blob ops
	second += first;
	third.append(testBuf, bufLen);
	
	cout << "\nTesting oofBLOB:"
		 << "\n'0, 2, 4, 8, 16, 65535, 0xADADADAD'\n" << first 
		 << "\nsame as above, twice\n" << second 
		 << "\nsame as first\n" << third 
		 << endl << endl;
	}
	
//************************
// demonstrate using standalone fields in a dbView
// which technique is mainly used for report-writers

// Note the use of either a pointer or temporary object.
// The pointer will be adopted by the view, and the temporary copied
	dbView  justOneLook;
	justOneLook 
		<< "This is a text string, which will be automatically attached as an oofChar"
#ifdef _Unix
// g++ has problems with resolving ambiguity with const
		<< (dbField*)new oofReal(12.3)
#else
		<< new oofReal(12.3)
#endif
		<< dbDate::currentDate();
	cout << justOneLook;
			
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       

