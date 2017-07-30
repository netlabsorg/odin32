/*
 * WINSPOOL DLL entry point
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

#include "oslibspl.h"

// Win32 resource table (produced by wrc)
extern DWORD winspool_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI LibMainWinSpool(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        //Write default printer name to registry
        ExportPrintersToRegistry();
        return TRUE;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        return TRUE;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitWinspool(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMainWinSpool, (PVOID)&winspool_PEResTab,
                              0, 0,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
           return -1;

    return 0;
}

void SYSTEM DLL_TermWinspool(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitWinspool(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermWinspool(hModule);
    DLL_TermDefault(hModule);
}
