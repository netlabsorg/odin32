/* $Id: initdsound.cpp,v 1.1 2002-09-14 08:31:25 sandervl Exp $
 *
 * DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
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

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}

static HMODULE dllHandle = 0;
static HMODULE MMPMLibraryHandle = 0;

BOOL fdsMMPMAvailable = TRUE;

DWORD (APIENTRY *pfnDSmciSendCommand)(WORD   wDeviceID,
                                   WORD   wMessage,
                                   DWORD  dwParam1,
                                   PVOID  dwParam2,
                                   WORD   wUserParm) = NULL;
DWORD (APIENTRY *pfnDSmciGetErrorString)(DWORD   dwError,
                                      LPSTR   lpstrBuffer,
                                      WORD    wLength) = NULL;


//******************************************************************************
//******************************************************************************
BOOL WINAPI LibMainDSound(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
  char   szError[CCHMAXPATH];
  HKEY   hKey;

   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:

        if(fdsMMPMAvailable == TRUE) 
        {//if audio access wasn't disabled already, check if mmpm2 is installed
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
/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG APIENTRY inittermDSound(ULONG hModule, ULONG ulFlag)
{

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
      {
         dllHandle = RegisterLxDll(hModule, LibMainDSound, (PVOID)&_Resource_PEResTab);
         if(dllHandle == 0)
                return 0UL;

         dprintf(("dsound init %s %s (%x)", __DATE__, __TIME__, inittermDSound));
         break;
      }

      case 1 :
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
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
