/* $Id: initterm.cpp,v 1.14 2001-09-05 12:12:02 bird Exp $ */
/*
 * COMDLG32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_WINSHELLDATA
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD comdlg32_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI COMDLG32_DllEntryPoint(HINSTANCE hInstance, DWORD Reason, LPVOID Reserved);

BOOL WINAPI LibMainComdlg32(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return COMDLG32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        COMDLG32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);
        return TRUE;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitComdlg32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMainComdlg32, (PVOID)&comdlg32_PEResTab,
                              COMDLG32_MAJORIMAGE_VERSION, COMDLG32_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
           return -1;

    return 0;
}

void SYSTEM DLL_TermComdlg32(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitComdlg32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermComdlg32(hModule);
    DLL_TermDefault(hModule);
}
