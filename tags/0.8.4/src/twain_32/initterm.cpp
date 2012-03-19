/* $Id: initterm.cpp,v 1.13 2001-09-05 13:52:13 bird Exp $
 *
 * TWAIN_32 DLL entry point
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
#include <twain.h>
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD twain_32_PEResTab;

#if 0
extern FARPROC   WINAPI GetProcAddress(HMODULE,LPCSTR);
extern HMODULE   WINAPI LoadLibraryA(LPCSTR);
extern BOOL      WINAPI FreeLibrary(HMODULE);
#endif
extern int WINAPI PROFILE_GetOdinIniInt(LPCSTR section,LPCSTR key_name,int def);
TW_UINT16 (APIENTRY *TWAINOS2_DSM_Entry)( pTW_IDENTITY, pTW_IDENTITY,
                                          TW_UINT32, TW_UINT16, TW_UINT16, TW_MEMREF) = 0;
#if 0
  static HINSTANCE hTWAIN = 0;
#else
  static HMODULE hTWAIN = 0;
  char szLoadError[256];
#endif

static HMODULE dllHandle = 0;

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
#ifdef __IBMC__
        ctordtorTerm();
#endif
        return TRUE;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitTwain32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    ULONG twaintype = PROFILE_GetOdinIniInt("TWAIN","TwainIF",1);
    switch(twaintype)
    {
    case 1:
    default:
    {
        dprintf(("TWAIN_32: Using CFM-Twain as Twain Source.\n\n"));
#if 0
        hTWAIN = LoadLibraryA("TWAINOS2.DLL");
        if (hTWAIN)
        {
            *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hTWAIN, (LPCSTR)"DSM_Entry");
        }
        else
        {
            return -1;
        }
#else
        APIRET rc = DosLoadModule( szLoadError, sizeof(szLoadError), "TWAINOS2", &hTWAIN);
        if (rc == 0)
        {
            rc = DosQueryProcAddr(hTWAIN, 0, "DSM_Entry",(PFN*)&TWAINOS2_DSM_Entry);
        }
        else
        {
            dprintf(("TWAIN_32: Error Loading DLL: %s",szLoadError));
        }
        if (rc != 0)
        {
            return -1;
        }
#endif
        break;
    }
    case 2:
    {
        dprintf(("TWAIN_32: Using STI-Twain as Twain Source.\n\n"));
#if 0
        hTWAIN = LoadLibraryA("TWAIN.DLL");
        if (hTWAIN)
        {
            *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hTWAIN, (LPCSTR)"DSM_ENTRY");
        }
        else
        {
            return -1;
        }
#else
        APIRET rc = DosLoadModule( szLoadError, sizeof(szLoadError), "TWAIN", &hTWAIN);
        if (rc == 0)
        {
            rc = DosQueryProcAddr(hTWAIN, 0, "DSM_Entry",(PFN*)&TWAINOS2_DSM_Entry);
        }
        else
        {
            dprintf(("TWAIN_32: Error Loading DLL: %s",szLoadError));
        }
        if (rc != 0)
        {
            return -1;
        }
#endif
        break;
    }
    case 3:
    {
        dprintf(("TWAIN_32: Using SANE as Twain Source  (currently not supported).\n\n"));
        return -1;
    }
    }

    dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&twain_32_PEResTab);
    if(dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermTwain32(ULONG hModule)
{
    if (hTWAIN)
    {
#if 0
        FreeLibrary(hTWAIN);
#else
        DosFreeModule(hTWAIN);
#endif
        hTWAIN = 0;
    }

    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitTwain32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermTwain32(hModule);
    DLL_TermDefault(hModule);
}
