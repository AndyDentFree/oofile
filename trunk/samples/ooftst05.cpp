// Copyright 1994 A.D. Software. All Rights Reserved

// OOFTEST5

// Simple stream I/O is used to interact with the user.
// to demonstrate the date and date-time parsing

#include "ooftest.h"

void getStringMaybeBlank(char* readInto);
void testDate();
void testTime();
void testDateTime();

int
main()
{
	cout << "OOFILE Validation Suite - Test 5\n"
		 << "Today's date is : " << dbDate::today << endl << endl
		 << "Simple test to try the input routines of the dbDate, dbTime & dbDateTime types\n\n" 
		 << "A number of sample entries are shown.\n\n"; 

	testDate();
	testTime();
	testDateTime();
	cout << endl << "done" << endl;
	return EXIT_SUCCESS;
}       


void testDate()
{
	unsigned short year, month, day;

	cout << "change the default date order to Month, Day Year" << endl;

	dbDate::sDefaultDateOrder=dbDate::orderMDY;
	dbDateTime::sBaseYear = 1900;


	char * 	test[] = {	"Jan 15 2001",
						"Feb, 24, 1923",
						"3/09/67",
						"4-30-1988",
						"MAY12:1971",
						"June17",
						"september 4th",
						"oct 1st 38",
						"11/19"		};
	unsigned short numtests = 9;
	
	oofDate aDate;  // define a standalone date variable
	aDate = dbDate::currentDate();
	
	cout << "The current date is: " << aDate << endl;
	oofDate twoWeeksAgo = aDate-14;
	
	cout << "Two weeks ago was: " << twoWeeksAgo << endl;
	
	for (int i = 0; i < numtests; ++i) {
		cout << "Attempting to process the date: " << test[i] << endl;
		if (dbDate::str2ymd(test[i],year,month,day))
			cout << " = (YMD) " << year << "/" << month << "/" << day << endl << endl;
		else
			cout << "bad date" << endl;
	}

/*
// uncomment this section if you want to try entering dates yourself	
	for (;;) {
		cout << "Enter a date (MDY): " << flush;
		if (dbDate::istream2ymd(cin, year, month, day)) {
			cout << " = (YMD) " << year << "/" << month << "/" << day << endl << endl;
			cin.ignore(INT_MAX, '\n');
			cin.clear();
		}
		else {
			cout << "bad date" << endl;
			break;
		}
	}
*/
}


void 
testTime()
{
	//dbTime::long2dhmsm( -1, day, hour, minute, second, millisecond );
	//cout << "dbTime::long2dhmsm test with a long of: -1" << endl;
	//cout << "day " << day << "  "<< hour << ':' << minute << '.' << second << ' ' << millisecond << endl;

	short day;
	unsigned short hour, minute, second, millisecond;
	char * 	test[] = {	"12:30.30am",					// 00:30:30
						"2.20 pm",						// 14:20:00
						"5:21",							// 5:21:00
						"2:30.5 PM",					// 14:30:05
						"23:59:59 AM",					// 23:59:59 (ignore AM)
						"12:00:00 AM",					// 00:00:00
						"5Pm",							// 17:00:00
						"5 o'clock",					// 5:00:00
						"5 pm 30 minutes, 5 seconds",	// 17:30:05
						"12pm",							// 12:00:00
						"1pm"							// 13:00:00
					};
	unsigned short numtests = 11;
	
	oofTime aTime;  // define a standalone date variable
	aTime = dbTime::currentTime();
	
	cout << "The current time is: " << aTime << endl << endl;
	
	for (int i = 0; i < numtests; ++i) {
		cout << "Processing the time: " << test[i] << endl;
		if (dbTime::str2dhmsm(test[i],day,hour,minute,second,millisecond)) {
			cout << " = (H:M:S)  " << hour << ':';
			if( minute < 10 ) cout << '0';
			cout << minute << ':';
			if( second < 10 ) cout << '0';
			cout << second << endl << endl;
		}
		else {
			cout << " = bad date" << endl << endl;
		}
	}
}


void 
testDateTime()
{
	char * 	test[] = {	"Jan 15 2001 12:30.30am",
						"Feb, 24, 1923. 2.20 pm",
						"3/09/67 5:21",
						"4-30-1988 2:30.5 PM",
						"MAY12:1971 - 12:00:00 AM",
						"June17 '95 5Pm",
						"september 4th",
						"oct 1st 38 12pm",
						"11/19/11 1pm"		};
	unsigned short numtests = 9;
	unsigned short year, month, day, hour, minute, second;
	
	oofDateTime aDateTime;  // define a standalone date variable
	aDateTime = dbDateTime::currentTime();
	
	cout << "The current time is: " << aDateTime << endl << endl;
	
	for (int i = 0; i < numtests; ++i) {
		cout << "Processing the time: " << test[i] << endl;
		if (dbDateTime::str2ymdhms(test[i],year,month,day,hour,minute,second)) {
			cout << " = (DD/MM/YY HH:MM:SS)  ";
			
			cout << day << '/' << month << '/' << year << ' ';

			cout << hour << ':';
			if( minute < 10 ) cout << '0';
			cout << minute << ':';
			if( second < 10 ) cout << '0';
			cout << second << endl << endl;
		}
		else {
			cout << " = bad date" << endl << endl;
		}
	}


	dbDateTime::ulong2ymdhms(dbDateTime::ymdhms2ulong(1970, 2, 10, 0, 0, 0),year,month,day,hour,minute,second);
	
	cout << "Testing dbDateTime::ulong2ymdhms parsing...1970, 2, 10, 0, 0, 0" << endl;
	cout << day << '/' << month << '/' << year << ' ';
	cout << hour << ':';
	if( minute < 10 ) cout << '0';
	cout << minute << ':';
	if( second < 10 ) cout << '0';
	cout << second << endl << endl;

	cout << "Testing dbDateTime::ymdhms2stream parsing...1975,10,5,13,40,29" << endl;
	dbDateTime::ymdhms2stream("DD/MM/YY hh:MIN:SEC pm",1975,10,5,13,40,29,cout);
	cout << endl<< endl;
	
	cout << "Testing dbDate:ymd2Stream parsing...1975,10,5" << endl;
	dbDate::ymd2Stream("YYYY/MM/DD",1975,10,5,cout);
	cout << endl<< endl;
}	

