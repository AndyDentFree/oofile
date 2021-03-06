// OOFileGUI_MFC.cpp : Defines the initialization routines for the DLL.
//

//#include "oofpch_m.h"  // for precompilation of MFC GUI files

#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE OOFileGUI_MFCDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("OOFILEGUI_MFC.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(OOFileGUI_MFCDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(OOFileGUI_MFCDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("OOFILEGUI_MFC.DLL Terminating!\n");
	}
	return 1;   // ok
}
