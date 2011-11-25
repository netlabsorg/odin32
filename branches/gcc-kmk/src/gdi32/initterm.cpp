/* $Id: initterm.cpp,v 1.21 2002-07-29 11:26:49 sandervl Exp $
 *
 * GDI32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_GPI
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
#include <stats.h>
#include <exitlist.h>

#include "region.h"
#include "dibsect.h"
#include "rgbcvt.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

// Win32 resource table (produced by wrc)
extern DWORD gdi32_PEResTab;

static HMODULE dllHandle = 0;
void (_Optlink *pRGB555to565)(WORD *dest, WORD *src, ULONG num) = NULL;
void (_Optlink *pRGB565to555)(WORD *dest, WORD *src, ULONG num) = NULL;

BOOL WINAPI GdiLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
       return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
       return TRUE;

   case DLL_PROCESS_DETACH:
       STATS_DumpStatsGDI32();
       return TRUE;
   }
   return FALSE;
}

ULONG SYSTEM DLL_InitGdi32(ULONG hModule)
{
    STATS_InitializeGDI32 ();

    ParseLogStatusGDI32();

    if (!InitializeKernel32())
        return 0;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
    if(InitRegionSpace() == FALSE)
    {
        return 0UL;
    }

    DIBSection::initDIBSection();
    if(CPUFeatures & CPUID_MMX)
    {
        pRGB555to565 = RGB555to565MMX;
        pRGB565to555 = RGB565to555MMX;
    }
    else
    {
        pRGB555to565 = RGB555to565;
        pRGB565to555 = RGB565to555;
    }

    dllHandle = RegisterLxDll(hModule, GdiLibMain, (PVOID)&gdi32_PEResTab,
                              GDI32_MAJORIMAGE_VERSION, GDI32_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_NATIVE);
    if(dllHandle == 0)
        return 0UL;

    dprintf(("gdi32 init %s %s (%x)", __DATE__, __TIME__, DLL_InitGdi32));

    RasEntry (RAS_EVENT_Gdi32InitComplete, &dllHandle, sizeof (dllHandle));


    return EXITLIST_GDI32;
}

void SYSTEM DLL_TermGdi32(ULONG hModule)
{
    dprintf(("gdi32 exit"));

    if (dllHandle)
    {
        DestroyRegionSpace();
        UnregisterLxDll(dllHandle);
    }

    STATS_UninitializeGDI32();
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitGdi32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermGdi32(hModule);
    DLL_TermDefault(hModule);
}
