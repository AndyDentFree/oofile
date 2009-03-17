#include <windows.h>

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
#ifdef OOF_LOAD_MFC_LIBRARY
	// The HMODULE of the MFC DLL that we successfully loaded, or 0 if the 
	// load was either never done or failed
	static HMODULE sl_hMFCModule = NULL;
#endif

    switch(ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:

#ifdef OOF_LOAD_MFC_LIBRARY
		// This horrible kludge is necessary because when the DLL's are
		// unloaded normally, MFC will unload before this DLL.  (There 
		// is no dependency to keep it around.)  When the debug version 
		// of MFC unloads it reports any 'still-allocated' heap as a 
		// memory leak.  To avoid erroneous reporting of heap used by 
		// libraries in this DLL, we are going to force the debug MFC DLL 
		// to stay resident (by bumping it's usage count) until we unload.
		if (GetModuleHandle("MFC42D"))
			sl_hMFCModule = LoadLibrary("MFC42D.DLL");
#endif
		break;

    case DLL_THREAD_ATTACH:
		break;

    case DLL_THREAD_DETACH:
		break;

    case DLL_PROCESS_DETACH:

#ifdef OOF_LOAD_MFC_LIBRARY
		// Conditionally unload the MFC library so that it can now check 
		// for memory leaks.

		if (sl_hMFCModule != NULL)
			FreeLibrary(sl_hMFCModule);
#endif
		break;
    }
    return TRUE;
}
