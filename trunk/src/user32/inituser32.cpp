/* $Id: inituser32.cpp,v 1.6 2001-10-22 23:13:58 phaller Exp $ */
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
#define  INCL_DOSMISC
#define  INCL_DOSERRORS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:29*/
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <spy.h>
#include <monitor.h>
#include "pmwindow.h"
#include "win32wdesktop.h"
#include "syscolor.h"
#include "initterm.h"
#include <exitlist.h>
#include <initdll.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD user32_PEResTab;
}
DWORD hInstanceUser32 = 0;


/**************************************************************/
/* Try to load the Presentation Manager Keyboard Hook module. */
/* If this fails, some hotkeys may not arrive properly at the */
/* targetted window, but no more harmful things will happen.  */
/**************************************************************/
#define PMKBDHK_MODULE "PMKBDHK"
#define PMKBDHK_HOOK_INIT "hookInit"
#define PMKBDHK_HOOK_TERM "hookKill"

static BOOL pmkbdhk_installed = FALSE;
static HMODULE hmodPMKBDHK;

static PVOID (*APIENTRY pfnHookInit)(HAB);
static BOOL  (*APIENTRY pfnHookTerm)(void);

// defined initialized in pmwindow.cpp: InitPM()
extern HAB hab;

void pmkbdhk_initialize(HAB _hab)
{
  APIRET rc;
  
  if (pmkbdhk_installed == FALSE)
  {
    CHAR szBuf[260];
    
    // load the DLL
    rc = DosLoadModule(szBuf,
                       sizeof(szBuf),
                       PMKBDHK_MODULE,
                       &hmodPMKBDHK);
    if (NO_ERROR != rc)
    {
      dprintf(("USER32: pmkbdhk_initalize(%08xh) failed rc=%d\n",
               _hab,
               rc));
      
      return;
    }
    
    // get the entry points
    rc = DosQueryProcAddr(hmodPMKBDHK,
                          0,
                          PMKBDHK_HOOK_INIT,
                          (PFN*)&pfnHookInit);
    if (NO_ERROR == rc)
      rc = DosQueryProcAddr(hmodPMKBDHK,
                            0,
                            PMKBDHK_HOOK_TERM,
                            (PFN*)&pfnHookTerm);
    
    if (NO_ERROR != rc)
    {
      dprintf(("USER32: pmkbdhk_initalize(%08xh) failed importing functions, rc=%d\n",
               _hab,
               rc));
      
      // free the DLL again
      DosFreeModule(hmodPMKBDHK);
      hmodPMKBDHK = NULLHANDLE;
      
      return;
    }
    
    // now finally call the initializer function
    pfnHookInit(_hab);
    
    // OK, hook is armed
    pmkbdhk_installed = TRUE;
  }
}

void pmkbdhk_terminate(void)
{
  if (pmkbdhk_installed == TRUE)
  {
    // call the terminator function
    pfnHookTerm();
    
    // OK, hook is disarmed
    pmkbdhk_installed = TRUE;
  }
  
  // unload the dll
  if (NULLHANDLE != hmodPMKBDHK)
  {
    APIRET rc = DosFreeModule(hmodPMKBDHK);
    if (NO_ERROR != rc)
    {
      dprintf(("USER32: pmkbdhk_terminate() failed rc=%d\n",
               rc));
    
      hmodPMKBDHK = NULLHANDLE;
    }
  }
}


/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG APIENTRY inittermUser32(ULONG hModule, ULONG ulFlag)
{
   size_t i;
   APIRET rc;
   ULONG  version[2];

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         ParseLogStatusUSER32();

         InitializeKernel32();
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

         hInstanceUser32 = RegisterLxDll(hModule, 0, (PVOID)&user32_PEResTab,
                                         USER32_MAJORIMAGE_VERSION, USER32_MINORIMAGE_VERSION,
                                         IMAGE_SUBSYSTEM_WINDOWS_GUI);
         if(hInstanceUser32 == 0)
                return 0UL;

         dprintf(("user32 init %s %s (%x)", __DATE__, __TIME__, inittermUser32));

         //SvL: Try to start communication with our message spy queue server
         InitSpyQueue();

         //SvL: Init win32 PM classes
         //PH:  initializes HAB!
         if (FALSE == InitPM())
           return 0UL;
     
         // try to install the keyboard hook
         pmkbdhk_initialize(hab);

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

void APIENTRY cleanupUser32(ULONG ulReason)
{
  dprintf(("user32 exit\n"));
  
  // try to unistall the keyboard hook
  pmkbdhk_terminate();

//SvL: Causes PM hangs on some (a lot?) machines. Reason unknown.
////   RestoreCursor();
   DestroyDesktopWindow();
   Win32BaseWindow::DestroyAll();
   UnregisterSystemClasses();
   Win32WndClass::DestroyAll();
   MONITOR_Finalize(&MONITOR_PrimaryMonitor);
   SYSCOLOR_Save();
   CloseSpyQueue();
   dprintf(("user32 exit done\n"));
}

