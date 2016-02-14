/* $Id: initterm.cpp,v 1.21 2016-02-09 10:00:00 rousseau Exp $
 *
 * GDIPLUS DLL entry point
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

///#include "region.h"
///#include "dibsect.h"
///#include "rgbcvt.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

// Win32 resource table (produced by wrc)
extern DWORD gdiplus_PEResTab;

static HMODULE dllHandle = 0;
void (_Optlink *pRGB555to565)(WORD *dest, WORD *src, ULONG num) = NULL;
void (_Optlink *pRGB565to555)(WORD *dest, WORD *src, ULONG num) = NULL;

BOOL WINAPI GdiplusLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
       return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
       return TRUE;

   case DLL_PROCESS_DETACH:
///       STATS_DumpStatsGDIPLUS();
       return TRUE;
   }
   return FALSE;
}

ULONG SYSTEM DLL_InitGdiplus(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","GDIPLUS",__FUNCTION__,DLL_InitGdiplus,hModule);

///    STATS_InitializeGDIPLUS ();

///    ParseLogStatusGDIPLUS();

    if (!InitializeKernel32())
        return -1;

///    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
///    if (InitRegionSpace() == FALSE)
///        return -1;
/**
    DIBSection::initDIBSection();
    if (CPUFeatures & CPUID_MMX)
    {
        pRGB555to565 = RGB555to565MMX;
        pRGB565to555 = RGB565to555MMX;
    }
    else
    {
        pRGB555to565 = RGB555to565;
        pRGB565to555 = RGB565to555;
    }
*/
    dllHandle = RegisterLxDll(hModule, GdiplusLibMain, (PVOID)&gdiplus_PEResTab,
                              GDIPLUS_MAJORIMAGE_VERSION, GDIPLUS_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_NATIVE);
    if (dllHandle == 0)
        return -1;

    dprintf(("gdiplus init %s %s (%x)", __DATE__, __TIME__, DLL_InitGdiplus));

///    RasEntry (RAS_EVENT_GdiplusInitComplete, &dllHandle, sizeof (dllHandle));


    return EXITLIST_GDIPLUS;
}

void SYSTEM DLL_TermGdiplus(ULONG hModule)
{
    __con_debug(2,"%s::%s@%08X(%08X)\n","GDIPLUS",__FUNCTION__,DLL_TermGdiplus,hModule);

    dprintf(("gdiplus exit"));
/**
    if (dllHandle)
    {
        DestroyRegionSpace();
        UnregisterLxDll(dllHandle);
    }
    STATS_UninitializeGDIPLUS();
*/
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitGdiplus(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermGdiplus(hModule);
    DLL_TermDefault(hModule);
}
