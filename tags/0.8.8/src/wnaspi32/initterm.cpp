/* $Id: initterm.cpp,v 1.8 2001-10-15 17:06:18 sandervl Exp $
 *
 * WNASPI322 DLL entry point
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
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <custombuild.h>
#include "cdio.h"

// Win32 resource table (produced by wrc)
extern DWORD wnaspi32_PEResTab;

static HMODULE dllHandle = 0;

BOOL fASPIAvailable = TRUE;

void WIN32API DisableASPI()
{
    dprintf(("DisableASPI"));
    fASPIAvailable = FALSE;
}

BOOL WINAPI Wnaspi32LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        if(fASPIAvailable == FALSE) return TRUE;

        if(OSLibCdIoInitialize() == FALSE)
        {
            dprintf(("WNASPI32: LibMain; can't allocate aspi object! APIs will not work!"));
            // @@@AH 20011020 we shouldn't prevent DLL loading in this case
            // just make sure that all API calls fail
            return TRUE;
        }
        fASPIAvailable = TRUE;
        dprintf(("WNASPI32: LibMain; os2cdrom.dmd ASPI interface available"));
        return TRUE;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return TRUE;

    case DLL_PROCESS_DETACH:
        OSLibCdIoTerminate();
        return TRUE;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitWinAspi32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, Wnaspi32LibMain, (PVOID)&wnaspi32_PEResTab);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermWinAspi32(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitWinAspi32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermWinAspi32(hModule);
    DLL_TermDefault(hModule);
}
