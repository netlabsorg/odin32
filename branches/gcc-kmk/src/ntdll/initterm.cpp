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
#include <os2.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <exitlist.h>

extern "C" {
 void __ctordtorInit (void);
 void __ctordtorTerm (void);
 void __ehInit();
 //InitOverrides deals with EMX runtime FS setting and allows us to
 //switch to using our own funcs namely _errno & _thread in runtime
 void InitOverrides(void);
 int _CRT_init (void);
 void _CRT_term (void);
}


static void APIENTRY cleanup(ULONG reason);

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
extern "C"
unsigned long _DLL_InitTerm(unsigned long hModule, unsigned long
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
         if (_CRT_init () != 0)
           return 0;
         __ctordtorInit ();
         __ehInit();
#if 0
        InitOverrides();
#endif

        rc = DosExitList(EXITLIST_NONCRITDLL|EXLST_ADD, cleanup);
         if (rc)
            return 0UL;

         return inittermNTDLL(hModule, ulFlag);

      case 1 :
         return inittermNTDLL(hModule, ulFlag);

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
static void APIENTRY cleanup(ULONG ulReason)
{
    dprintf(("NTDLL exit"));
    __ctordtorTerm();
    _CRT_term ();

    DosExitList(EXLST_EXIT, cleanup);
    return ;
}
//******************************************************************************
//******************************************************************************
