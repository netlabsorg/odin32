/* $Id: initterm.cpp,v 1.1 1999-12-07 20:19:55 achimha Exp $ */

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
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/

extern "C" {
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );
}

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);


/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
unsigned long SYSTEM _DLL_InitTerm(unsigned long hModule, unsigned long
                                   ulFlag)
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
         _ctordtorInit();

         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/

	 if(RegisterLxDll(hModule, 0, 0) == FALSE) 
		return 0UL;

         rc = DosExitList(0x0000F000|EXLST_ADD, cleanup);
         if(rc)
                return 0UL;

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


static void APIENTRY cleanup(ULONG ulReason)
{
   _ctordtorTerm();
   DosExitList(EXLST_EXIT, cleanup);
   return ;
}
