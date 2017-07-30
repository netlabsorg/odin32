/*
 * OLE32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD ole32_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI OLE32_DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad);

BOOL WINAPI LibMainOLE32(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
	return OLE32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);

   case DLL_PROCESS_DETACH:
        OLE32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);
	return TRUE;
   }
   return FALSE;
}

ULONG SYSTEM DLL_InitOle32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMainOLE32, (PVOID)&ole32_PEResTab);
    if (dllHandle == 0)
           return -1;

    return 0;
}

void SYSTEM DLL_TermOle32(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitOle32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermOle32(hModule);
    DLL_TermDefault(hModule);
}
