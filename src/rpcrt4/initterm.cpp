/* $Id: initterm.cpp,v 1.7 2001-09-05 13:37:51 bird Exp $ */
/*
 * RPCRT4 DLL entry point
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
#include "uuidp.h"
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD rpcrt4_PEResTab;

static HMODULE dllHandle = 0;

ULONG SYSTEM DLL_InitRpcRt4(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, NULL, (PVOID)&rpcrt4_PEResTab);
    if (dllHandle == 0)
           return -1;

    //SvL: Must be done here as the socket calls trash FS!
    /* Init the Uuid subsystem */
    UuidInit();

    return 0;
}

void SYSTEM DLL_TermRpcRt4(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitRpcRt4(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermRpcRt4(hModule);
    DLL_TermDefault(hModule);
}
