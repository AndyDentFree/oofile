// ooftest.h

// Use this file in test programs instead of oofile.h to get the other defines
// shown below. If you don't want to use defines such as TEST_CONNECT then 
// normally you would just include oofile.h

// if you are using the report-writer, you also need oofrep.h

// Note: if you relied on OOFILE in previous versions to include stream headers 
// defining ostrstream and cout etc. you now need to also #include "oofios.h"
#ifndef H_OOFTEST
#define H_OOFTEST

#ifndef H_OOFFILE
	#include "oofile.h"
#endif

// ****** IMPORTANT - CHANGE THIS DEFINITION FOR TEST PROGRAMS
// the majority of the ooftstXX programs use the following definitions to 
// allow testing of the different backends

#ifndef TEST_CONNECT
// this allows a build file to define in advance, eg: our projects
// that just build the Personal version

// USER INSTRUCTIONS - UNCOMMENT JUST ONE PAIR BELOW
	//#define TEST_CONNECT dbConnect_ramp
	//#define TESTING_RAMP

	//#define TEST_CONNECT dbConnect_ram
	//#define TESTING_RAM

	//#define TEST_CONNECT dbConnect_dbase
	//#define TESTING_DBASE

	#define TEST_CONNECT dbConnect_ctree
	#define TESTING_CTREE
	
#endif

#endif

