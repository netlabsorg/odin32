/* $Id: initgdi32.cpp,v 1.13 2002-07-29 11:26:48 sandervl Exp $
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
#define  INCL_GPI
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include "region.h"
#include <initdll.h>
#include <stats.h>
#include "dibsect.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD gdi32_PEResTab;
}
static HMODULE dllHandle = 0;
//******************************************************************************
//******************************************************************************
BOOL WINAPI GdiLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
       return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
       return TRUE;

   case DLL_PROCESS_DETACH:
       STATS_DumpStatsGDI32();
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
ULONG APIENTRY inittermGdi32(ULONG hModule, ULONG ulFlag)
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
         ParseLogStatusGDI32();

         InitializeKernel32();
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
         if(InitRegionSpace() == FALSE) {
             return 0UL;
         }
         DIBSection::initDIBSection();
         dllHandle = RegisterLxDll(hModule, GdiLibMain, (PVOID)&gdi32_PEResTab,
                                   GDI32_MAJORIMAGE_VERSION, GDI32_MINORIMAGE_VERSION,
                                   IMAGE_SUBSYSTEM_NATIVE);
         if(dllHandle == 0)
             return 0UL;

         dprintf(("gdi32 init %s %s (%x)", __DATE__, __TIME__, inittermGdi32));

         break;
      case 1 :
         if(dllHandle) {
             DestroyRegionSpace();
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