/* $Id: initterm.cpp,v 1.17 2001/09/05 10:26:30 bird Exp $
 *
 * SCHANNEL DLL entry point
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
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <cpuhlp.h>
#include <dbglog.h>
#include <initdll.h>

//#define DBG_LOCALLOG    DBG_initterm
//#include "dbglocal.h"

// Win32 resource table (produced by wrc)
//extern DWORD rsaenh_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI SChannelDllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved);

static BOOL WINAPI OdinLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    BOOL ret;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return SChannelDllMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return SChannelDllMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_PROCESS_DETACH:
        ret = SChannelDllMain(hinstDLL, fdwReason, fImpLoad);
        return ret;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitSChannel(ULONG hModule)
{
    if (!InitializeKernel32())
        return -1;

    //CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, OdinLibMain, NULL);
    if (dllHandle == 0)
        return -1;

    dprintf(("schannel init %s %s (%x)", __DATE__, __TIME__, DLL_InitSChannel));

    return 0;
}

void SYSTEM DLL_TermSChannel(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitSChannel(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermSChannel(hModule);
    DLL_TermDefault(hModule);
}
