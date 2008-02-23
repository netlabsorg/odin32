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
#include <windows.h>
#include <win32type.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <exitlist.h>
#include <os2sel.h>

BOOL WIN32API MSVCRT_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
extern DWORD msvcrt_PEResTab;

static HMODULE dllHandle = 0;

//******************************************************************************
//******************************************************************************
ULONG inittermMSVCRT(ULONG hModule, ULONG ulFlag)
{
   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
         dllHandle = RegisterLxDll(hModule, (WIN32DLLENTRY)MSVCRT_Init, (PVOID)&msvcrt_PEResTab,0,0,0);
         if(dllHandle == 0)
             return 0UL;

         dprintf(("msvcrt init %s %s (%x)", __DATE__, __TIME__, inittermMSVCRT));

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
