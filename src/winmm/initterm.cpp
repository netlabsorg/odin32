/* $Id: initterm.cpp,v 1.20 2001-09-05 10:30:39 bird Exp $
 *
 * WINMM DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 2000 Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_OS2MM
#include <os2wrap.h>   //Odin32 OS/2 api wrappers
#include <os2mewrap.h> //Odin32 OS/2 MMPM/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef __GNUC__
#include <builtin.h>
#endif
#include <misc.h>       /*PLF Wed  98-03-18 23:19:26*/
#include <odin.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <initdll.h>
#include "auxiliary.h"
#include "winmmtype.h"
#include "waveoutbase.h"
#include <win/options.h>
#include "initterm.h"
#undef fMMPMAvailable      //inside this file use the real flag (see MMPMEnsure)
#include <custombuild.h>
#include "mixer.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

BOOL MULTIMEDIA_MciInit(void);
BOOL MULTIMEDIA_CreateIData(HINSTANCE hinstDLL);
void MULTIMEDIA_DeleteIData(void);

extern "C" void IRTMidiShutdown(); // IRTMidi shutdown routine

// Win32 resource table (produced by wrc)
extern DWORD winmm_PEResTab;

static HMODULE dllHandle = 0;
static HMODULE MMPMLibraryHandle = 0;

static const char *szBuggyAudio[] =
{
    "CWAUD",
    "BSAUD",
    "CRYSTAL"
};

BOOL fMMPMReal = TRUE;            //MMPM/2 usable?  (only meaningful after MMPMEnsure())
static BOOL fMMPMInitDone = FALSE;

DWORD (APIENTRY *pfnmciSendCommand)(WORD   wDeviceID,
                                    WORD   wMessage,
                                    DWORD  dwParam1,
                                    PVOID  dwParam2,
                                    WORD   wUserParm) = NULL;
DWORD (APIENTRY *pfnmciGetErrorString)(DWORD   dwError,
                                       LPSTR   lpstrBuffer,
                                       WORD    wLength) = NULL;

void WIN32API DisableWaveAudio()
{
    fMMPMInitDone        = TRUE;
    fMMPMReal            = FALSE;
    pfnmciGetErrorString = NULL;
    pfnmciSendCommand    = NULL;
}

//MMPM/2 (MDM.DLL and the MMPM DLLs it pulls in) is only loaded when audio is first needed.
//Loading it from LibMain made the load of WINMM -- and with it COMCTL32 and every application
//importing them -- fail right after a boot, because FILT.DLL faults while MMPM/2 is not yet
//initialised (the fault aborts the whole DosLoadModule chain).
BOOL MMPMEnsure()
{
    char   szError[CCHMAXPATH];
    char   szPDDName[128];
    HKEY   hKey;

    if(fMMPMInitDone) return fMMPMReal;
    fMMPMInitDone = TRUE;

        if(fMMPMReal == TRUE)
        {//if audio access wasn't disabled already, check if mmpm2 is installed
            // try to load the MDM library, not MMPM directly!!!
            if (DosLoadModule(szError, sizeof(szError),
                              "MDM", &MMPMLibraryHandle) != NO_ERROR)
            {
                // this system has no MMPM :-(
                fMMPMReal = FALSE;
            }
            else
            {
                /* detect if MMPM is available */
                if (DosQueryProcAddr(MMPMLibraryHandle,
                                     1, /* ORD_MCISENDCOMMAND */
                                     NULL,
                                    (PFN*)&pfnmciSendCommand) != NO_ERROR)
                {
                    fMMPMReal = FALSE;
                }
                else
                {
                    fMMPMReal = TRUE;
                }

                /* see if we can get the address for the mciGetErrorString function */
                if (fMMPMReal == TRUE)
                {
                    if (DosQueryProcAddr(MMPMLibraryHandle,
                                         3, /* ORD_MCIGETERRORSTRING */
                                         NULL,
                                         (PFN*)&pfnmciGetErrorString) != NO_ERROR)
                        pfnmciGetErrorString = NULL;
                }
                dprintf(("MMPM/2 is available; hmod %x", MMPMLibraryHandle));
                dprintf(("mciSendCommand    %x", pfnmciSendCommand));
                dprintf(("mciGetErrorString %x", pfnmciGetErrorString));
            }
        }

        if(fMMPMReal && RegOpenKeyA(HKEY_LOCAL_MACHINE, CUSTOM_BUILD_OPTIONS_KEY, &hKey) == 0)
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
                    fMMPMReal = FALSE;
                    pfnmciGetErrorString = NULL;
                    pfnmciSendCommand = NULL;
                }
            }
            RegCloseKey(hKey);
        }
        if(!fMMPMReal || OSLibGetAudioPDDName(szPDDName) == FALSE) {
            fMMPMReal = FALSE;
        }
        else
        {
            // Test for buggy audio drivers to turn off audio automagically
            for(int i=0;i<sizeof(szBuggyAudio)/sizeof(szBuggyAudio[0]);i++)
            {
                if(!strncmp(szPDDName, szBuggyAudio[i], strlen(szBuggyAudio[i]))) {
                    dprintf(("Detected driver %s -> turning off audio!!", szPDDName));
                    fMMPMReal = FALSE;
                    break;
                }
            }
        }
        mixerInit();
    return fMMPMReal;
}

BOOL MMPMInitialized()
{
    return fMMPMInitDone;
}

BOOL WINAPI LibMainWinmm(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
  static BOOL bInitDone = FALSE;

  switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (!MULTIMEDIA_CreateIData(hinstDLL))
            return FALSE;

        if (!bInitDone) { /* to be done only once */
            if (!MULTIMEDIA_MciInit() /*|| !MMDRV_Init() */ ) {
                MULTIMEDIA_DeleteIData();
                return FALSE;
            }
            bInitDone = TRUE;
        }
        DWORD dwVolume;

        dwVolume = PROFILE_GetOdinIniInt(WINMM_SECTION, DEFVOL_KEY, 100);
        dwVolume = (dwVolume*0xFFFF)/100;
        dwVolume = (dwVolume << 16) | dwVolume;
        WaveOut::setDefaultVolume(dwVolume);

        return TRUE;
   }

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
        return TRUE;

   case DLL_PROCESS_DETACH:
        WaveInOut::shutdown();
        MULTIMEDIA_DeleteIData();
        auxOS2Close();    /* Close aux device if necessary */
        IRTMidiShutdown;  /* Shutdown RT Midi subsystem, if running. */

        if(MMPMInitialized()) mixerExit();
        if(MMPMLibraryHandle) DosFreeModule(MMPMLibraryHandle);
        return TRUE;
   }
   return FALSE;
}

DWORD APIENTRY mymciSendCommand(WORD   wDeviceID,
                                WORD   wMessage,
                                DWORD  dwParam1,
                                PVOID  dwParam2,
                                WORD   wUserParm)
{
    if(pfnmciSendCommand == NULL) {
        DebugInt3();
        return MCIERR_CANNOT_LOAD_DRIVER;
    }
    USHORT sel = RestoreOS2FS();
    DWORD ret = pfnmciSendCommand(wDeviceID, wMessage, dwParam1, dwParam2, wUserParm);
    SetFS(sel);
    return ret;
}

DWORD APIENTRY mymciGetErrorString(DWORD   dwError,
                                   LPSTR   lpstrBuffer,
                                   WORD    wLength)
{
    if(pfnmciGetErrorString == NULL) {
        DebugInt3();
        return MCIERR_CANNOT_LOAD_DRIVER;
    }
    USHORT sel = RestoreOS2FS();
    DWORD ret = pfnmciGetErrorString(dwError, lpstrBuffer, wLength);
    SetFS(sel);
    return ret;
}

ULONG SYSTEM DLL_InitWinMM32(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    ParseLogStatusWINMM();

    dllHandle = RegisterLxDll(hModule, LibMainWinmm, (PVOID)&winmm_PEResTab);
    if (dllHandle == 0)
        return -1;

    dprintf(("winmm init %s %s (%x)", __DATE__, __TIME__, DLL_InitWinMM32));

    return 0;
}

void SYSTEM DLL_TermWinMM32(ULONG hModule)
{
    auxOS2Close(); /* SvL: Close aux device if necessary */

    if(dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitWinMM32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermWinMM32(hModule);
    DLL_TermDefault(hModule);
}
