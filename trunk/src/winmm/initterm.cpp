/*
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
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
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

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

BOOL MULTIMEDIA_MciInit(void);
BOOL MULTIMEDIA_CreateIData(HINSTANCE hinstDLL);
void MULTIMEDIA_DeleteIData(void);

extern "C" {
void IRTMidiShutdown();  // IRTMidi shutdown routine

 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}
static HMODULE dllHandle = 0;

//******************************************************************************
//******************************************************************************
BOOL WINAPI OdinLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
  static BOOL           bInitDone = FALSE;

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
        MULTIMEDIA_DeleteIData();
        auxOS2Close(); /* SvL: Close aux device if necessary */
        IRTMidiShutdown;  /* JT: Shutdown RT Midi subsystem, if running. */
        ctordtorTerm();
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
ULONG DLLENTRYPOINT_CCONV DLLENTRYPOINT_NAME(ULONG hModule, ULONG ulFlag)
{
    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
    case 0 :
        ctordtorInit();

        ParseLogStatus();

        CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
        dllHandle = RegisterLxDll(hModule, OdinLibMain, (PVOID)&_Resource_PEResTab);
        if(dllHandle == 0)
            return 0UL;/* Error */

        dprintf(("winmm init %s %s (%x)", __DATE__, __TIME__, DLLENTRYPOINT_NAME));
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



