/* $Id: initterm.cpp,v 1.5 1999-08-16 16:28:02 sandervl Exp $ */
/*
 * COMCTL32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1999 Achim Hasenmueller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:29*/

void CDECL RegisterCOMCTL32WindowClasses(unsigned long hinstDLL);
void CDECL UnregisterCOMCTL32WindowClasses(void);

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
   size_t i;
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

         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

         /* register Win32 window classes implemented in this DLL */
         RegisterCOMCTL32WindowClasses(hModule);

         break;
      case 1 :
   	 /* unregister Win32 window classes */
   	 UnregisterCOMCTL32WindowClasses();
         break;
      default  :
         return 0UL;
   }

   /***********************************************************/
   /* A non-zero value must be returned to indicate success.  */
   /***********************************************************/
   return 1UL;
}

