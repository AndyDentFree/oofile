// oofCore.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "oofCore_initialDemoForUnitTests.h"


// This is an example of an exported variable
OOFCORE_API int noofCore=0;

// This is an example of an exported function.
OOFCORE_API int fnoofCore(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see oofCore.h for the class definition
CoofCore::CoofCore()
{
	return;
}
