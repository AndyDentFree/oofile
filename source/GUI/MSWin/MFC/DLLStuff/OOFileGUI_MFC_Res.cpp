// COPYRIGHT 1997 A.D. Software, All rights reserved

// DllMain for resource-only DLL
#include <windows.h>

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    return 1;
}
