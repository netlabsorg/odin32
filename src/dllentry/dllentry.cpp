/* $Id: dllentry.cpp,v 1.3 2000-12-16 23:31:07 bird Exp $ */

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
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/


extern "C" {
#ifdef __IBMCPP__
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );
#endif
 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}

static HMODULE dllHandle = 0;

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
#ifdef __IBMCPP__
         _ctordtorTerm();
#endif
         return TRUE;
   }
   return FALSE;
}


#ifdef __IBMCPP__

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
unsigned long SYSTEM _DLL_InitTerm(unsigned long hModule, unsigned long ulFlag)
{
   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag)
   {
      case 0:
         _ctordtorInit();
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
         dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&_Resource_PEResTab);
         if (dllHandle == 0)
            return 0UL;
         break;

      case 1:
         if (dllHandle)
            UnregisterLxDll(dllHandle);
         break;

      default:
         return 0UL;
   }

   /***********************************************************/
   /* A non-zero value must be returned to indicate success.  */
   /***********************************************************/
   return 1UL;
}
#elif defined(__WATCOM_CPLUSPLUS__)
/*
 * Watcom dll init and term routines.
 */

int __dll_initialize(unsigned long hModule, unsigned long ulFlag)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule);
    dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&_Resource_PEResTab);
    if (dllHandle == 0)
        return 0;
    return 1;
}

int __dll_terminate(unsigned long hModule, unsigned long ulFlag)
{
    if (dllHandle)
        UnregisterLxDll(dllHandle);
    return 1;
}

#else
#error message("compiler is not supported!\n");
#endif
