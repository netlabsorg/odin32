/* $Id: initterm.cpp,v 1.23 2000-08-11 10:56:26 sandervl Exp $ */

/*
 * USER32 DLL entry point
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
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:29*/
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <spy.h>
#include <monitor.h>
#include "pmwindow.h"
#include "win32wdesktop.h"
#include "syscolor.h"
#include "initterm.h"
#include <exitlist.h>

#define DBG_LOCALLOG	DBG_initterm
#include "dbglocal.h"

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);

extern "C" {
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );

 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}
DWORD hInstanceUser32 = 0;

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
         ParseLogStatus();

         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

         hInstanceUser32 = RegisterLxDll(hModule, 0, (PVOID)&_Resource_PEResTab,
                                         USER32_MAJORIMAGE_VERSION, USER32_MINORIMAGE_VERSION,
                                         IMAGE_SUBSYSTEM_WINDOWS_GUI);
         if(hInstanceUser32 == 0)
                return 0UL;

         /*******************************************************************/
         /* A DosExitList routine must be used to clean up if runtime calls */
         /* are required and the runtime is dynamically linked.             */
         /*******************************************************************/

         rc = DosExitList(EXITLIST_USER32|EXLST_ADD, cleanup);
         if(rc)
                return 0UL;

         //SvL: Try to start communication with our message spy queue server
         InitSpyQueue();

         //SvL: Init win32 PM classes
         if(InitPM() == FALSE) {
                return 0UL;
         }

         //SvL: 18-7-'98, Register system window classes (button, listbox etc etc)
         //CB: register internal classes
         RegisterSystemClasses(hModule);

         //CB: initialize PM monitor driver
         MONITOR_Initialize(&MONITOR_PrimaryMonitor);

         break;
      case 1 :
         if(hInstanceUser32) {
         	UnregisterLxDll(hInstanceUser32);
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

static void APIENTRY cleanup(ULONG ulReason)
{
   dprintf(("user32 exit\n"));
   DestroyDesktopWindow();
   Win32BaseWindow::DestroyAll();
   UnregisterSystemClasses();
   Win32WndClass::DestroyAll();
   MONITOR_Finalize(&MONITOR_PrimaryMonitor);
   SYSCOLOR_Save();
   CloseSpyQueue();
   _ctordtorTerm();
   dprintf(("user32 exit done\n"));

   DosExitList(EXLST_EXIT, cleanup);
}

