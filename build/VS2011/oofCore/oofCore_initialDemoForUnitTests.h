// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OOFCORE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OOFCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef OOFCORE_EXPORTS
#define OOFCORE_API __declspec(dllexport)
#else
#define OOFCORE_API __declspec(dllimport)
#endif

// This class is exported from the oofCore.dll
class OOFCORE_API CoofCore {
public:
	CoofCore(void);
	int TestMethod() { return -997; }
};

extern OOFCORE_API int noofCore;

OOFCORE_API int fnoofCore(void);
