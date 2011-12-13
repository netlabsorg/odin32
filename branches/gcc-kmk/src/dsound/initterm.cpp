/* $Id: initterm.cpp,v 1.18 2002-09-14 08:31:25 sandervl Exp $
 *
 * DSOUND DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSERRORS
#define  INCL_OS2MM
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
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
#include "initdsound.h"

// Win32 resource table (produced by wrc)
extern DWORD dsound_PEResTab;

static HMODULE dllHandle = 0;
static HMODULE MMPMLibraryHandle = 0;

char dsoundPath[CCHMAXPATH] = "";

BOOL fdsMMPMAvailable = TRUE;

DWORD (APIENTRY *pfnDSmciSendCommand)(WORD   wDeviceID,
                                   WORD   wMessage,
                                   DWORD  dwParam1,
                                   PVOID  dwParam2,
                                   WORD   wUserParm) = NULL;
DWORD (APIENTRY *pfnDSmciGetErrorString)(DWORD   dwError,
                                      LPSTR   lpstrBuffer,
                                      WORD    wLength) = NULL;

BOOL WINAPI LibMainDSound(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    char   szError[CCHMAXPATH];
    HKEY   hKey;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:

        if(fdsMMPMAvailable == TRUE)
        {
            // if audio access wasn't disabled already, check if mmpm2 is installed
            // try to load the MDM library, not MMPM directly!!!
            if (DosLoadModule(szError, sizeof(szError),
                              "MDM", &MMPMLibraryHandle) != NO_ERROR)
            {
                // this system has no MMPM :-(
                fdsMMPMAvailable = FALSE;
            }
            else
            {
                /* detect if MMPM is available */
                if (DosQueryProcAddr(MMPMLibraryHandle,
                                     1, /* ORD_MCISENDCOMMAND */
                                     NULL,
                                     (PFN*)&pfnDSmciSendCommand) != NO_ERROR)
                {
                    fdsMMPMAvailable = FALSE;
                }
                else
                {
                    fdsMMPMAvailable = TRUE;
                }

                /* see if we can get the address for the mciGetErrorString function */
                if (fdsMMPMAvailable == TRUE)
                {
                    if (DosQueryProcAddr(MMPMLibraryHandle,
                                         3, /* ORD_MCIGETERRORSTRING */
                                         NULL,
                                         (PFN*)&pfnDSmciGetErrorString) != NO_ERROR)
                        pfnDSmciGetErrorString = NULL;
                }
                dprintf(("MMPM/2 is available; hmod %x", MMPMLibraryHandle));
                dprintf(("mciSendCommand    %x", pfnDSmciSendCommand));
                dprintf(("mciGetErrorString %x", pfnDSmciGetErrorString));
            }
        }

        if(fdsMMPMAvailable && RegOpenKeyA(HKEY_LOCAL_MACHINE, CUSTOM_BUILD_OPTIONS_KEY, &hKey) == 0)
        {
            DWORD dwSize, dwType;
            DWORD dwFlag;

            dwSize = sizeof(dwFlag);
            LONG rc = RegQueryValueExA(hKey, DISABLE_AUDIO_KEY,
                                       NULL, &dwType,
                                       (LPBYTE)&dwFlag,
                                       &dwSize);

            if(rc == 0 && dwType == REG_DWORD) {
                if(dwFlag) {
                    fdsMMPMAvailable = FALSE;
                    pfnDSmciGetErrorString = NULL;
                    pfnDSmciSendCommand = NULL;
                }
            }
            RegCloseKey(hKey);
        }
        return TRUE;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        return TRUE;

    case DLL_PROCESS_DETACH:
        if(MMPMLibraryHandle) DosFreeModule(MMPMLibraryHandle);
        return TRUE;
    }
    return FALSE;
}

DWORD APIENTRY dsmciSendCommand(WORD   wDeviceID,
                                WORD   wMessage,
                                DWORD  dwParam1,
                                PVOID  dwParam2,
                                WORD   wUserParm)
{
    if(pfnDSmciSendCommand == NULL) {
        DebugInt3();
        return MCIERR_CANNOT_LOAD_DRIVER;
    }
    USHORT sel = RestoreOS2FS();
    DWORD ret = pfnDSmciSendCommand(wDeviceID, wMessage, dwParam1, dwParam2, wUserParm);
    SetFS(sel);
    return ret;
}

DWORD APIENTRY dsmciGetErrorString(DWORD   dwError,
                                   LPSTR   lpstrBuffer,
                                   WORD    wLength)
{
    if(pfnDSmciGetErrorString == NULL) {
        DebugInt3();
        return MCIERR_CANNOT_LOAD_DRIVER;
    }
    USHORT sel = RestoreOS2FS();
    DWORD ret = pfnDSmciGetErrorString(dwError, lpstrBuffer, wLength);
    SetFS(sel);
    return ret;
}

ULONG SYSTEM DLL_InitDSound(ULONG hModule)
{
    DosQueryModuleName(hModule, CCHMAXPATH, dsoundPath);
    char *endofpath = strrchr(dsoundPath, '\\');
    if (endofpath)
        *(endofpath+1) = 0;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    dllHandle = RegisterLxDll(hModule, LibMainDSound, (PVOID)&dsound_PEResTab);
    if(dllHandle == 0)
        return -1;

    dprintf(("dsound init %s %s (%x)", __DATE__, __TIME__, DLL_InitDSound));

    return 0;
}

void SYSTEM DLL_TermDSound(ULONG hModule)
{
    if (dllHandle)
       UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitDSound(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermDSound(hModule);
    DLL_TermDefault(hModule);
}
