/* $Id: initcomdlg32.cpp,v 1.5 2003-04-08 12:43:29 sandervl Exp $ */
/*
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
#define  INCL_WINSHELLDATA
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
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

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD comdlg32_PEResTab;

 BOOL WINAPI COMDLG32_DllEntryPoint(HINSTANCE hInstance, DWORD Reason, LPVOID Reserved);
}

static HMODULE dllHandle = 0;

//******************************************************************************
//******************************************************************************
BOOL WINAPI LibMainComdlg32(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
    case DLL_PROCESS_ATTACH:
    {
        /*
         * Find default printer and write name to win.ini
         * Format: queuename,driver,portname
         */
        char szWinDefPrn[256];
        szWinDefPrn[0] = '\0';

        /*
         * OS/2 stores the default printer as a combination of printerdevice and queue.
         *
         *   Printer Device is related to one port. It may have multiple printer
         *       drivers because the port may serve multiple queues with different drivers.
         *   The Ports are related to multiple queues.
         *
         * So we take the default default printer+queue combination and finds the
         */
        char szDefPrnDev[20];
        char szDefPrnQue[20];
        if (PrfQueryProfileString(HINI_PROFILE, "PM_SPOOLER", "PRINTER", "",
                                  szDefPrnDev, sizeof(szDefPrnDev)) > 1
            &&  PrfQueryProfileString(HINI_PROFILE, "PM_SPOOLER", "QUEUE", "",
                                      szDefPrnQue, sizeof(szDefPrnQue)) > 1
            &&  szDefPrnDev[0]
            &&  szDefPrnQue[0]
           )
        {
            char *psz;
            /* remove everything beyond the first ';' */
            if ((psz = strchr(szDefPrnDev, ';')) != NULL)
                *psz = '\0';
            if ((psz = strchr(szDefPrnQue, ';')) != NULL)
                *psz = '\0';

            /*
             * Now we must lookup the port name from the device settings.
             *  This is a string of this form:
             *      <port>;<driver1>[,<driver2>;<queue1>[,<queue2>];?;?;
             */
            ULONG   cb = 0;
            if (PrfQueryProfileSize(HINI_SYSTEMPROFILE, "PM_SPOOLER_PRINTER", szDefPrnDev, &cb)
                &&  cb > 0)
            {
                char *pszBufD = (char*)malloc(cb + 1);
                if (pszBufD
                    &&  PrfQueryProfileString(HINI_SYSTEMPROFILE, "PM_SPOOLER_PRINTER", szDefPrnDev,
                                              NULL, pszBufD, cb + 1)
                    > 1
                   )
                {
                    /*
                     * Now get the Default printer driver for the queue.
                     *  This is stored as a ';' separated list of drivers, the first one is the default.
                     */
                    if (PrfQueryProfileSize(HINI_SYSTEMPROFILE, "PM_SPOOLER_QUEUE_DD", szDefPrnQue, &cb)
                        &&  cb > 0)
                    {
                        char *pszBufQ = (char*)malloc(cb + 1);
                        if (pszBufQ
                            &&  PrfQueryProfileString(HINI_SYSTEMPROFILE, "PM_SPOOLER_QUEUE_DD", szDefPrnQue,
                                                      NULL, pszBufQ, cb + 1)
                            > 1
                           )
                        {
                            /*
                             * We got everything now. just find the parts we need.
                             *  First printer driver from QUEUE_DD
                             *  Port name of the device.
                             */
                            if ((psz = strchr(pszBufQ, ';')) != NULL)
                                *psz = '\0';
                            if ((psz = strchr(pszBufQ, ',')) != NULL) //paranoia! in case comman separated list of some kind.
                                *psz = '\0';
                            if ((psz = strchr(pszBufD, ';')) != NULL)
                                *psz = '\0';
                            if ((psz = strchr(pszBufD, ',')) != NULL) //paranoia in case comman separated list of some kind.
                                *psz = '\0';

                            /*
                             * Now make default printer string for the win.ini.
                             */
                            strcpy(szWinDefPrn, szDefPrnQue);
                            strcat(strcat(szWinDefPrn, ","), pszBufQ);
                            strcat(strcat(szWinDefPrn, ","), pszBufD);
                            dprintf(("LibMainComdlg32: Successfully found default printer.'%s'", szWinDefPrn));
                            free(pszBufQ);
                        }
                    }
                    else
                    {
                        /* OS/2 the device may exist though the default queue is destroyed.
                         * it may still exist even if there are no queues on the system at all!
                         */
                        dprintf(("LibMainComdlg32: no queue driver entry for '%s'.", szDefPrnQue));
                    }

                    free(pszBufD);
                }
            }
            else
            {
                /* OS/2 doesn't remove the default settings if the default queue/printer is deleted. */
                dprintf(("LibMainComdlg32: can't find device settings for '%s'.", szDefPrnDev));
            }
        }
        else
        {
            dprintf(("LibMainComdlg32: no default printer? szDefPrnDev='%s' szDefPrnQue='%s'.", szDefPrnDev, szDefPrnQue));
        }

        //Now get real printer name the one that will be used in DC calls
        WriteProfileStringA("windows", "device", szWinDefPrn);

        return COMDLG32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);
    }

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
       COMDLG32_DllEntryPoint(hinstDLL, fdwReason, fImpLoad);
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
ULONG APIENTRY inittermComdlg32(ULONG hModule, ULONG ulFlag)
{
   size_t i;
   APIRET rc;

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         dllHandle = RegisterLxDll(hModule, LibMainComdlg32, (PVOID)&comdlg32_PEResTab,
                                   COMDLG32_MAJORIMAGE_VERSION, COMDLG32_MINORIMAGE_VERSION,
                                   IMAGE_SUBSYSTEM_WINDOWS_GUI);
         if(dllHandle == 0)
             return 0UL;

         break;
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
