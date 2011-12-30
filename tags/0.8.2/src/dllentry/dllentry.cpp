/* $Id: dllentry.cpp,v 1.7 2001-10-01 01:39:17 bird Exp $ */

/*
 * Ordinary DLL entry point
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
extern DWORD Resource_PEResTab;

static HMODULE dllHandle = 0;

#if 0

// Not currently needed

BOOL WINAPI LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return TRUE;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return TRUE;

    case DLL_PROCESS_DETACH:
        return TRUE;
    }

    return FALSE;
}

#endif

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
    dllHandle = RegisterLxDll(hModule, NULL, (PVOID)&Resource_PEResTab);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_Term(ULONG hModule)
{
    if(dllHandle)
        UnregisterLxDll(dllHandle);

    DLL_TermDefault(hModule);
}
