/* $Id: initterm.cpp,v 1.3 2002-04-30 14:52:03 sandervl Exp $ */
/*
 * DINPUT DLL entry point
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
extern DWORD dinput_PEResTab;

extern "C" {
extern void mousedev_register();
extern void keyboarddev_register();
}

static HMODULE dllHandle = 0;

BOOL WINAPI DInputLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
        keyboarddev_register();
        mousedev_register();
        return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
        return TRUE;
   }
   return FALSE;
}

ULONG SYSTEM DLL_InitDInput(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, DInputLibMain, (PVOID)&dinput_PEResTab);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermDInput(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitDInput(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermDInput(hModule);
    DLL_TermDefault(hModule);
}
