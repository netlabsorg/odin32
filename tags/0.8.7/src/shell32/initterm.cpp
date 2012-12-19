/*
 * SHELL32 DLL entry point
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
extern DWORD shell32_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI Shell32LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad);

BOOL WINAPI OdinLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    BOOL ret;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return Shell32LibMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_PROCESS_DETACH:
        ret = Shell32LibMain(hinstDLL, fdwReason, fImpLoad);
        return ret;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitShell32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, OdinLibMain, (PVOID)&shell32_PEResTab,
                              SHELL32_MAJORIMAGE_VERSION, SHELL32_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
           return -1;

    return 0;
}

void SYSTEM DLL_TermShell32(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitShell32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermShell32(hModule);
    DLL_TermDefault(hModule);
}
