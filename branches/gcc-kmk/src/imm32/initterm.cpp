/*
 * IMM32OS2 DLL entry point
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
#include <initdll.h>
#include <exitlist.h>
#include <dbglog.h>

#include "im32.h"

// Win32 resource table (produced by wrc)
extern DWORD imm32os2_PEResTab;

static HMODULE dllHandle = 0;

BOOL WINAPI ImmLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
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

ULONG SYSTEM DLL_InitImm32(ULONG hModule)
{
    if (!IM32Init())
       dprintf(("IM32Init failed"));

    CheckVersionFromHMOD(PE2LX_VERSION, hModule);
    dllHandle = RegisterLxDll(hModule, ImmLibMain, (PVOID)&imm32os2_PEResTab,
                              IMM32_MAJORIMAGE_VERSION, IMM32_MINORIMAGE_VERSION,
                              IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (dllHandle == 0)
        return 0UL;

    dprintf(("imm32 init %s %s (%x)", __DATE__, __TIME__, DLL_InitImm32));

    return EXITLIST_NONCRITDLL;
}

void SYSTEM DLL_TermImm32(ULONG hModule)
{
    dprintf(("imm32 exit"));

    if(dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitImm32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermImm32(hModule);
    DLL_TermDefault(hModule);
}
