/* $Id: pmkbdhk.cpp,v 1.1 2001-10-22 23:13:39 phaller Exp $ */
/*
 * OS/2 native Presentation Manager hooks
 *
 *
 * Large Portions (C) Ulrich M”ller, XWorkplace
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_WININPUT
#define  INCL_WINMESSAGEMGR
#define  INCL_WINHOOKS
#define  INCL_PM
#define  INCL_DOSERRORS
#define  INCL_DOSSEMAPHORES
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#include <pmscan.h>

#include <pmkbdhk.h>
#include "pmkbdhkp.h"

/*
 
 To improve the ODIN keyboard management, it is required to intercept
 any possible key from the Presentation Manager input queue.
 
 Sketch:
 
 - intercept keyboard messages before accelerators are translated
   (HK_PREACCEL)
 - determine if the message is targetted for an ODIN window
   - if no ODIN window, pass thru
   - if ODIN window
     - check content of message.
       - if sensible key event, rewrite message so PM won't handle it
         itself (F1, F10, PrtScr, Ctrl, Alt, AltGr)
         Ensure, GetKeyState() / GetAsyncKeyState() will still work properly!
         The so rewritten message is handled specially inside the ODIN
         message procedure. There, additionally required messages such as
         WM_MENU if ALT is pressed need to be generated.
       - if no sensible key, pass thru
       
   Installation:
   - automatically install hook on loading of the DLL
   - automatically uninstall the hook on DLL termination
   - DLL is GLOBAL INITTERM, used shared mem only!
       
*/
       

/**********
 * Prototypes
 **********/

BOOL EXPENTRY hookPreAccelHook(HAB hab, PQMSG pqmsg, ULONG option);

// _CRT_init is the C run-time environment initialization function.
// It will return 0 to indicate success and -1 to indicate failure.
int _Optlink _CRT_init(void);

// _CRT_term is the C run-time environment termination function.
// It only needs to be called when the C run-time functions are statically
// linked, as is the case with XWorkplace.
void _Optlink _CRT_term(void);



/******************************************************************
 *
 *  Module Global Variables
 *
 ******************************************************************/

HOOKDATA G_HookData;


/******************************************************************
 *
 *  Helper functions
 *
 ******************************************************************/

/*
 *@@ InitializeGlobalsForHooks:
 *      this gets called from hookInit to initialize
 *      the global variables. We query the PM desktop,
 *      the window list, and other stuff we need all
 *      the time.
 */

VOID InitializeGlobalsForHooks(VOID)
{
    HENUM   henum;
    HWND    hwndThis;
    BOOL    fFound;

    // PM desktop (the WPS desktop is handled by the daemon)
    G_HookData.hwndPMDesktop = WinQueryDesktopWindow(G_HookData.habDaemonObject,
                                                     NULLHANDLE);

    WinQueryWindowProcess(HWND_DESKTOP, &G_HookData.pidPM, NULL);
            // V0.9.7 (2001-01-21) [umoeller]

    // enumerate desktop window to find the window list:
    // according to PMTREE, the window list has the following
    // window hierarchy:
    //      WC_FRAME
    //        +--- WC_TITLEBAR
    //        +--- Menu
    //        +--- WindowList
    //        +---
    fFound = FALSE;
    henum = WinBeginEnumWindows(HWND_DESKTOP);
    while (     (!fFound)
             && (hwndThis = WinGetNextWindow(henum))
          )
    {
        CHAR    szClass[200];
        if (WinQueryClassName(hwndThis, sizeof(szClass), szClass))
        {
            if (!strcmp(szClass, "#1"))
            {
                // frame window: check the children
                HENUM   henumFrame;
                HWND    hwndChild;
                henumFrame = WinBeginEnumWindows(hwndThis);
                while (    (!fFound)
                        && (hwndChild = WinGetNextWindow(henumFrame))
                      )
                {
                    CHAR    szChildClass[200];
                    if (WinQueryClassName(hwndChild, sizeof(szChildClass), szChildClass))
                    {
                        if (!strcmp(szChildClass, "WindowList"))
                        {
                            // yup, found:
                            G_HookData.hwndWindowList = hwndThis;
                            fFound = TRUE;
                        }
                    }
                }
                WinEndEnumWindows(henumFrame);
            }
        }
    }
    WinEndEnumWindows(henum);

}


/******************************************************************
 *
 *  Hook interface
 *
 ******************************************************************/

/*
 *@@ _DLL_InitTerm:
 *      this function gets called automatically by the OS/2 DLL
 *      during DosLoadModule processing, on the thread which
 *      invoked DosLoadModule.
 *
 *      We override this function (which is normally provided by
 *      the runtime library) to intercept this DLL's module handle.
 *
 *      Since OS/2 calls this function directly, it must have
 *      _System linkage.
 *
 *      Note: You must then link using the /NOE option, because
 *      the VAC++ runtimes also contain a _DLL_Initterm, and the
 *      linker gets in trouble otherwise.
 *      The XWorkplace makefile takes care of this.
 *
 *      This function must return 0 upon errors or 1 otherwise.
 *
 *@@changed V0.9.0 [umoeller]: reworked locale initialization
 */

unsigned long _System _DLL_InitTerm(unsigned long hModule,
                                    unsigned long ulFlag)
{
    switch (ulFlag)
    {
        case 0:
        {
            // store the DLL handle in the global variable
            G_HookData.hmodDLL = hModule;

            // now initialize the C run-time environment before we
            // call any runtime functions
            if (_CRT_init() == -1)
               return (0);  // error

        break; }

        case 1:
            // DLL being freed: cleanup runtime
            _CRT_term();
            break;

        default:
            // other code: beep for error
            DosBeep(100, 100);
            return (0);     // error
    }

    // a non-zero value must be returned to indicate success
    return (1);
}

/*
 *@@ hookInit:
 *      registers (sets) all the hooks and initializes data.
 *
 *      In any case, a pointer to the DLL's static HOOKDATA
 *      structure is returned. In this struct, the caller
 *      can examine the two flags for whether the hooks
 *      were successfully installed.
 *
 *      Note: All the exported hook* interface functions must
 *      only be called by the same process, which is the
 *      XWorkplace daemon (XWPDAEMN.EXE).
 *
 *      This gets called by XWPDAEMN.EXE when
 *
 *@@changed V0.9.1 (2000-02-01) [umoeller]: fixed missing global updates
 *@@changed V0.9.2 (2000-02-21) [umoeller]: added new system hooks
 */

PHOOKDATA EXPENTRY hookInit(HAB hab)               
{
    APIRET arc = NO_ERROR;

    if (arc == NO_ERROR)
    {
        G_HookData.habDaemonObject = hab;

        // G_HookData.hmtxPageMage = hmtxPageMage;

        if (G_HookData.hmodDLL)       // initialized by _DLL_InitTerm
        {
            // BOOL fSuccess = FALSE;

            // initialize globals needed by the hook
            InitializeGlobalsForHooks();

            // install hooks, but only once...
#if 0
            if (!G_HookData.fLockupHooked)
                G_HookData.fLockupHooked = WinSetHook(G_HookData.habDaemonObject,
                                                      NULLHANDLE, // system hook
                                                      HK_LOCKUP,  // lockup hook
                                                      (PFN)hookLockupHook,
                                                      G_HookData.hmodDLL);
#endif
            if (!G_HookData.fPreAccelHooked)
                G_HookData.fPreAccelHooked = WinSetHook(G_HookData.habDaemonObject,
                                                        NULLHANDLE, // system hook
                                                        HK_PREACCEL,  // pre-accelerator table hook (undocumented)
                                                        (PFN)hookPreAccelHook,
                                                        G_HookData.hmodDLL);
        }

    }

    return (&G_HookData);
}

/*
 *@@ hookKill:
 *      deregisters the hook function and frees allocated
 *      resources.
 *
 *      Note: This function must only be called by the same
 *      process which called hookInit (that is, the daemon),
 *      or resources cannot be properly freed.
 *
 *@@changed V0.9.1 (2000-02-01) [umoeller]: fixed missing global updates
 *@@changed V0.9.2 (2000-02-21) [umoeller]: added new system hooks
 *@@changed V0.9.3 (2000-04-20) [umoeller]: added function keys support
 */

BOOL EXPENTRY hookKill(void)
{
    BOOL brc = FALSE;

    if (G_HookData.fPreAccelHooked)
    {
        WinReleaseHook(G_HookData.habDaemonObject,
                       NULLHANDLE,
                       HK_PREACCEL,     // pre-accelerator table hook (undocumented)
                       (PFN)hookPreAccelHook,
                       G_HookData.hmodDLL);
        brc = TRUE;
        G_HookData.fPreAccelHooked = FALSE;
    }
  
#if 0
    if (G_HookData.fLockupHooked)
    {
        WinReleaseHook(G_HookData.habDaemonObject,
                       NULLHANDLE,
                       HK_LOCKUP,       // lockup hook
                       (PFN)hookLockupHook,
                       G_HookData.hmodDLL);
        brc = TRUE;
        G_HookData.fLockupHooked = FALSE;
    }
#endif

    return (brc);
}


//
// This function detects if the current window is
// a win32 window
//

static HWND hwndLastWin32Window = 0;

BOOL i_isWin32Window(HWND hwnd)
{
  // Note: this hook is called on the stack of the current process
  // so we rather keep the buffer small ...
  CHAR    szClass[80];
  
  if (hwndLastWin32Window == hwnd)
    return TRUE;
  
  for(;;)
  {
    if (WinQueryClassName(hwnd, sizeof(szClass), szClass))
    {
      if (strcmp(szClass, WIN32_STDFRAMECLASS) == 0)
      {
        hwndLastWin32Window = hwnd;
        return TRUE;
      }
      else
      {
        // walk up the tree
        hwnd = WinQueryWindow(hwnd,
                              QW_PARENT);
        
        // no parent window found?
        if (NULLHANDLE == hwnd)
        {
          hwndLastWin32Window = 0;
          return FALSE;
        }
      }
    }
    else
    {
      hwndLastWin32Window = 0;
      return FALSE;
    }
  }
}


/******************************************************************
 *
 *  Pre-accelerator hook
 *
 ******************************************************************/

/*
 *@@ hookPreAccelHook:
 *      this is the pre-accelerator-table hook. Like
 *      hookInputHook, this gets called when messages
 *      are coming in from the system input queue, but
 *      as opposed to a "regular" input hook, this hook
 *      gets called even before translation from accelerator
 *      tables occurs.
 *
 *      Pre-accel hooks are not documented in the PM Reference.
 *      I have found this idea (and part of the implementation)
 *      in the source code of ProgramCommander/2, so thanks
 *      go out (once again) to Roman Stangl.
 *
 *      In this hook, we check for the global object hotkeys
 *      so that we can use certain key combinations regardless
 *      of whether they might be currently used in application
 *      accelerator tables. This is especially useful for
 *      "Alt" key combinations, which are usually intercepted
 *      when menus have corresponding shortcuts.
 *
 *      As a result, as opposed to other hotkey software you
 *      might know, XWorkplace does properly react to "Ctrl+Alt"
 *      keyboard combinations, even if a menu would get called
 *      with the "Alt" key. ;-)
 *
 *      As with hookInputHook, we return TRUE if the message is
 *      to be swallowed, or FALSE if the current application (or
 *      the next hook in the hook chain) should still receive the
 *      message.
 *
 *@@changed V0.9.3 (2000-04-09) [umoeller]: added check for system lockup
 *@@changed V0.9.3 (2000-04-09) [umoeller]: added PageMage hotkeys
 *@@changed V0.9.3 (2000-04-09) [umoeller]: added KC_SCANCODE check
 *@@changed V0.9.3 (2000-04-10) [umoeller]: moved debug code to hook
 */

BOOL EXPENTRY hookPreAccelHook(HAB hab, PQMSG pqmsg, ULONG option)
{
    // set return value:
    // per default, pass message on to next hook or application
    BOOL        brc = FALSE;

    if (pqmsg == NULL)
        return (FALSE);

    switch(pqmsg->msg)
    {
        /*
         * WM_CHAR:
         *      keyboard activity. 
         */

        case WM_CHAR:
        {
          // Note:
          // what happens it the system is locked up?
          // do we have to detect the screensaver to kick in?
          // if (  (!G_HookData.hwndLockupFrame)    // system not locked up
          // ...
          
      
          // is this an WIN32 window?
          if (!i_isWin32Window(pqmsg->hwnd))
          {
            // no, so pass thru
            return FALSE;
          }
          
          // check if we've encountered a so called critical key
          // (this is the scan code which is supposed to be valid
          // always for the hooks! */
          switch ( CHAR4FROMMP(pqmsg->mp1) )
          {
            default:
              return FALSE;
            
            case PMSCAN_F1:
            case PMSCAN_F10:
            case PMSCAN_PRINT:
            case PMSCAN_ALTLEFT:
            case PMSCAN_ALTRIGHT:
            case PMSCAN_SCROLLLOCK:
            case PMSCAN_CAPSLOCK:
            case PMSCAN_SHIFTLEFT:
            case PMSCAN_SHIFTRIGHT:
              // OK, as we've got a special key here, we've got
              // to rewrite the message so PM will ignore the key
              // and won't translate the message to anything else.
            
              pqmsg->msg = WM_CHAR_SPECIAL;
            
              break;
          }
        }
      
        break; // WM_CHAR
    } // end switch(msg)

    return (brc);
}

