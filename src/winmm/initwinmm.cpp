/* $Id: initwinmm.cpp,v 1.4 2001-10-24 22:47:41 sandervl Exp $
 *
 * WINMM DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 2000 Chris Wohlgemuth
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*-------------------------------------------------------------*/
/* INITERM.C -- Source for a custom dynamic link library       */
/*              initialization and termination (_DLL_InitTerm) */
/*              function.                                      */
/*                                                             */
/* When called to perform initialization, this sample function */
/* gets storage for an array of integers, and initializes its  */
/* elements with random integers.  At termination time, it     */
/* frees the array.  Substitute your own special processing.   */
/*-------------------------------------------------------------*/


/* Include files */
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
#include <builtin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:19:26*/
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <initdll.h>
#include "auxiliary.h"
#include "winmmtype.h"
#include "waveoutbase.h"
#include <win\options.h>
#include "initwinmm.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

BOOL MULTIMEDIA_MciInit(void);
BOOL MULTIMEDIA_CreateIData(HINSTANCE hinstDLL);
void MULTIMEDIA_DeleteIData(void);

extern "C" {
void IRTMidiShutdown();  // IRTMidi shutdown routine

 //Win32 resource table (produced by wrc)
 extern DWORD winmm_PEResTab;
}
static HMODULE dllHandle = 0;
static HMODULE MMPMLibraryHandle = 0;

BOOL fMMPMAvailable = FALSE;

DWORD (APIENTRY *pfnmciSendCommand)(WORD   wDeviceID,
                                   WORD   wMessage,
                                   DWORD  dwParam1,
                                   PVOID  dwParam2,
                                   WORD   wUserParm) = NULL;
DWORD (APIENTRY *pfnmciGetErrorString)(DWORD   dwError,
                                      LPSTR   lpstrBuffer,
                                      WORD    wLength) = NULL;

//******************************************************************************
//******************************************************************************
BOOL WINAPI LibMainWinmm(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
  static BOOL bInitDone = FALSE;
  char   szError[CCHMAXPATH];

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

        // try to load the MDM library, not MMPM directly!!!
        if (DosLoadModule(szError, sizeof(szError),
                          "MDM.DLL", &MMPMLibraryHandle) != NO_ERROR)
        {
            // this system has no MMPM :-(
            fMMPMAvailable = FALSE;
        } 
        else
        {
            /* detect if MMPM is available */
            if (DosQueryProcAddr(MMPMLibraryHandle,
                                 1, /* ORD_MCISENDCOMMAND */
                                 NULL,
                                (PFN*)&pfnmciSendCommand) != NO_ERROR)
            {
                fMMPMAvailable = FALSE;
            } 
            else
            {
                fMMPMAvailable = TRUE;
            }

            /* see if we can get the address for the mciGetErrorString function */
            if (fMMPMAvailable == TRUE)
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

        return TRUE;
   }

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
        return TRUE;

   case DLL_PROCESS_DETACH:
        MULTIMEDIA_DeleteIData();
        auxOS2Close(); /* SvL: Close aux device if necessary */
        IRTMidiShutdown;  /* JT: Shutdown RT Midi subsystem, if running. */

        if(MMPMLibraryHandle) DosFreeModule(MMPMLibraryHandle);
        return TRUE;
   }
   return FALSE;
}
/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG APIENTRY inittermWinmm(ULONG hModule, ULONG ulFlag)
{
    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
    case 0 :
        ParseLogStatusWINMM();

        dllHandle = RegisterLxDll(hModule, LibMainWinmm, (PVOID)&winmm_PEResTab);
        if(dllHandle == 0)
            return 0UL;/* Error */

        dprintf(("winmm init %s %s (%x)", __DATE__, __TIME__, inittermWinmm));
        break;
    case 1 :
        auxOS2Close(); /* SvL: Close aux device if necessary */
        if(dllHandle) {
            UnregisterLxDll(dllHandle);
        }
        break;
    default  :
        return 0UL;
    }

    /***********************************************************/
    /* A non-zero value must be returned to indicate success.  */
    /***********************************************************/
    return 1UL;
}
//******************************************************************************
//******************************************************************************
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
    USHORT sel = GetFS();
    DWORD ret = pfnmciSendCommand(wDeviceID, wMessage, dwParam1, dwParam2, wUserParm);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
DWORD APIENTRY mymciGetErrorString(DWORD   dwError,
                                   LPSTR   lpstrBuffer,
                                   WORD    wLength)
{
    if(pfnmciGetErrorString == NULL) {
        DebugInt3();
        return MCIERR_CANNOT_LOAD_DRIVER;
    }
    USHORT sel = GetFS();
    DWORD ret = pfnmciGetErrorString(dwError, lpstrBuffer, wLength);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
