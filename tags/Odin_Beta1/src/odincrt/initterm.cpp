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
#define  INCL_DOSERRORS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <exitlist.h>
#include <initdll.h>

#ifdef __IBMCPP__
extern "C" {

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);
}

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
   APIRET rc;

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :

         /*******************************************************************/
         /* The C run-time environment initialization function must be      */
         /* called before any calls to C run-time functions that are not    */
         /* inlined.                                                        */
         /*******************************************************************/

         if (_CRT_init() == -1)
            return 0UL;
         ctordtorInit();

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/

         rc = DosExitList(EXITLIST_ODINCRT|EXLST_ADD, cleanup);
         if(rc)
                return 0UL;
         #if 1 /*
                * Experimental console hack. Sets apptype to PM anyhow.
                * First Dll to be initiated should now allways be OdinCrt!
                * So include odincrt first!
                */
            PPIB pPIB;
            PTIB pTIB;
            rc = DosGetInfoBlocks(&pTIB, &pPIB);
            if (rc != NO_ERROR)
                return 0UL;
            pPIB->pib_ultype = 3;
         #endif
         break;
      case 1 :
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
   ctordtorTerm();
   _CRT_term();
   DosExitList(EXLST_EXIT, cleanup);
   return ;
}

#elif defined(__WATCOM_CPLUSPLUS__)

/*
 * Watcom dll init and term routines.
 */

int __dll_initialize(unsigned long hModule, unsigned long ulFlag)
{
    APIRET rc;
    #if 1 /*
          * Experimental console hack. Sets apptype to PM anyhow.
          * First Dll to be initiated should now allways be OdinCrt!
          * So include odincrt first!
          */
    PPIB pPIB;
    PTIB pTIB;
    rc = DosGetInfoBlocks(&pTIB, &pPIB);
    if (rc != NO_ERROR)
        return 0UL;
    pPIB->pib_ultype = 3;
    #endif
    return 1;
}

int __dll_terminate(unsigned long hModule, unsigned long ulFlag)
{
    return 1;
}

#else
#error message("compiler is not supported");
#endif

