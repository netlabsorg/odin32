/* $Id: initwnaspi32.cpp,v 1.1 2002-06-08 11:42:03 sandervl Exp $
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
#include "aspilib.h"
#include <custombuild.h>

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD wnaspi32_PEResTab;
}
scsiObj *aspi = NULL;
static HMODULE dllHandle = 0;
static BOOL fDisableASPI = FALSE;

//******************************************************************************
//******************************************************************************
void WIN32API DisableASPI()
{
   dprintf(("DisableASPI"));
   fDisableASPI = TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI Wnaspi32LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
   {
       if(fDisableASPI) return TRUE;

       aspi = new scsiObj();
       if(aspi == NULL) {
           dprintf(("WNASPI32: LibMain; can't allocate aspi object! APIs will not work!"));
           // @@@AH 20011020 we shouldn't prevent DLL loading in this case
           // just make sure that all API calls fail
           return TRUE;
       }
       if(aspi->init(65535) == FALSE)
       {
           delete aspi;
           aspi = NULL;
           dprintf(("WNASPI32: LibMain; can't init aspi object!"));
           // @@@20011026 and also in this case we shouldn't prevent DLL loading...
           return TRUE;
       }
       dprintf(("WNASPI32: LibMain; aspi object created successfully"));
       return TRUE;
   }

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
       return TRUE;

   case DLL_PROCESS_DETACH:
       if(aspi) {
           aspi->close();
           delete aspi;
           aspi = NULL;
       }
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
ULONG APIENTRY inittermWnaspi32(ULONG hModule, ULONG ulFlag)
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
         dllHandle = RegisterLxDll(hModule, Wnaspi32LibMain, (PVOID)&wnaspi32_PEResTab);
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
//******************************************************************************
//******************************************************************************
