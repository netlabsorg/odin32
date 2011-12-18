/* $Id: initterm.cpp,v 1.20 2003-01-21 11:20:35 sandervl Exp $
 *
 * DDRAW DLL entry point
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
#include <dbglog.h>
#include <exitlist.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>

#ifdef FULLSCREEN_DDRAW
#include "os2fsdd.h"    // For RestorePM()
#endif
#include "divewrap.h"

// Win32 resource table (produced by wrc)
extern DWORD ddraw_PEResTab;

char ddrawPath[CCHMAXPATH] = "";
static HMODULE dllHandle = 0;

#if 0 // not currently needed
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

ULONG SYSTEM DLL_InitDDraw(ULONG hModule)
{
    APIRET rc;

    DosQueryModuleName(hModule, CCHMAXPATH, ddrawPath);
    char *endofpath = strrchr(ddrawPath, '\\');
    if (endofpath)
       *(endofpath+1) = '\0';

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, NULL, (PVOID)&ddraw_PEResTab,
                              DDRAW_MAJORIMAGE_VERSION, DDRAW_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
       return -1;

    DiveLoad();

#ifdef FULLSCREEN_DDRAW
    return EXITLIST_NONCOREDLL;
#else
    return 0;
#endif
}

void SYSTEM DLL_TermDDraw(ULONG hModule)
{
#ifdef FULLSCREEN_DDRAW
    dprintf(("DDRAW processing exitlist"));
    RestorePM();
    dprintf(("DDRAW exitlist done"));
#endif

    DiveUnload();

    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitDDraw(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermDDraw(hModule);
    DLL_TermDefault(hModule);
}
