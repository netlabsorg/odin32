/* $Id: initterm.cpp,v 1.17 2001/09/05 10:26:30 bird Exp $
 *
 * OLEACC DLL entry point
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

// Win32 resource table (produced by wrc)
extern DWORD oleacc_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI OleaccDllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved);

static BOOL WINAPI OdinLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    BOOL ret;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return OleaccDllMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return OleaccDllMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_PROCESS_DETACH:
        ret = OleaccDllMain(hinstDLL, fdwReason, fImpLoad);
        return ret;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitOleAcc(ULONG hModule)
{
    if (!InitializeKernel32())
        return -1;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, OdinLibMain, (PVOID)&oleacc_PEResTab);
    if (dllHandle == 0)
        return -1;

    dprintf(("oleacc init %s %s (%x)", __DATE__, __TIME__, DLL_InitOleAcc));

    return 0;
}

void SYSTEM DLL_TermOleAcc(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitOleAcc(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermOleAcc(hModule);
    DLL_TermDefault(hModule);
}
