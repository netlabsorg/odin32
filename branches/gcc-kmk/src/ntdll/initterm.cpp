/*
 * NTDLL DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <os2.h>    //Odin32 OS/2 api wrappers
#include <win32type.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <exitlist.h>

extern "C"
BOOL WIN32API NTDLL_LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

extern DWORD ntdll_PEResTab;

static HMODULE dllHandle = 0;

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;

    //Initialize kernel32 first (circular dependency between kernel32 & ntdll)
    if (!InitializeKernel32())
        return -1;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
    dllHandle = RegisterLxDll(hModule, (WIN32DLLENTRY)NTDLL_LibMain,
                              (PVOID)&ntdll_PEResTab,
                              0, 0, 0);
    if(dllHandle == 0)
        return -1;

    dprintf(("NTDLL INIT %s %s (%x)", __DATE__, __TIME__, DLL_Init));

    return EXITLIST_NONCRITDLL;
}

void SYSTEM DLL_Term(ULONG hModule)
{
    dprintf(("NTDLL TERM"));

    UnregisterLxDll(dllHandle);
    DLL_TermDefault(hModule);
}

