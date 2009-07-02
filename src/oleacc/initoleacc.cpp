/* $Id: initgdi32.cpp,v 1.16 2004/01/11 11:42:17 sandervl Exp $
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
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <cpuhlp.h>
#include <dbglog.h>
#include <initdll.h>

extern "C" {
 //Win32 resource table (produced by wrc)
    extern DWORD oleacc_PEResTab;
    BOOL WINAPI OleaccDllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved);
}

static HMODULE dllHandle = 0;

static BOOL WINAPI OdinLibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   BOOL ret;

   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
       return OleaccDllMain(hinstDLL, fdwReason, fImpLoad);

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
       return OleaccDllMain(hinstDLL, fdwReason, fImpLoad);

   case DLL_PROCESS_DETACH:
       ret = OleaccDllMain(hinstDLL, fdwReason, fImpLoad);
       return ret;
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
ULONG APIENTRY inittermOleacc(ULONG hModule, ULONG ulFlag)
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

         InitializeKernel32();
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
         dllHandle = RegisterLxDll(hModule, OdinLibMain, (PVOID)&oleacc_PEResTab);
         if(dllHandle == 0)
             return 0UL;

         dprintf(("oleacc init %s %s (%x)", __DATE__, __TIME__, inittermOleacc));

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
