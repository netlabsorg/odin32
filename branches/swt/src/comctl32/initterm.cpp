/* $Id: initterm.cpp,v 1.16 2001-09-05 12:04:59 bird Exp $ */
/*
 * COMCTL32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1999 Achim Hasenmueller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:29*/
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD comctl32_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI COMCTL32_DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

BOOL WINAPI LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return COMCTL32_DllMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_PROCESS_DETACH:
        return COMCTL32_DllMain(hinstDLL, fdwReason, fImpLoad);
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitComCtl32(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","comctl32.dll",__FUNCTION__,DLL_InitComCtl32,hModule);
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&comctl32_PEResTab,
                              COMCTL32_MAJORIMAGE_VERSION, COMCTL32_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermComCtl32(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","comctl32.dll",__FUNCTION__,DLL_TermComCtl32,hModule);
    if (dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitComCtl32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermComCtl32(hModule);
    DLL_TermDefault(hModule);
}
