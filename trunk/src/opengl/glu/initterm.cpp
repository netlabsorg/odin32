/* $Id: initterm.cpp,v 1.7 2000-03-04 19:10:12 jeroen Exp $ */

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
#include <os2wrap.h>                   /* Odin32 OS/2 api wrappers         */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>

extern "C" {
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );

 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}

//******************************************************************************
//******************************************************************************
BOOL WINAPI LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
        return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
        return TRUE;

   case DLL_PROCESS_DETACH:
        _ctordtorTerm();
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
unsigned long _System _DLL_InitTerm(unsigned long hModule, unsigned long
                                   ulFlag)
{
   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         _ctordtorInit();

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/

         if(RegisterLxDll(hModule, LibMain, (PVOID)&_Resource_PEResTab) == FALSE)
                return 0UL;

         CheckVersionFromHMOD(PE2LX_VERSION, hModule);/* PLF Wed  98-03-18 05:28:48*/

         break;

      case 1 :
         UnregisterLxDll(hModule);
         break;

      default  :
         return 0UL;
   }

   /***********************************************************/
   /* A non-zero value must be returned to indicate success.  */
   /***********************************************************/
   return 1UL;
}
