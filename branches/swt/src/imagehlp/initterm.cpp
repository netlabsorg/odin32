/* $Id: initterm.cpp,v 1.7 2001-09-05 12:55:49 bird Exp $
 *
 * IMAGEHLP DLL entry point
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
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD imagehlp_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI IMAGEHLP_LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

BOOL WINAPI LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return IMAGEHLP_LibMain(hinstDLL, fdwReason, fImpLoad);

    case DLL_PROCESS_DETACH:
        IMAGEHLP_LibMain(hinstDLL, fdwReason, fImpLoad);
#ifdef __IBMC__
        ctordtorTerm();
#endif
        return TRUE;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitImageHlp(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&imagehlp_PEResTab);
    if(dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermImageHlp(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitImageHlp(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermImageHlp(hModule);
    DLL_TermDefault(hModule);
}
