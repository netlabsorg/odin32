/* $Id: pmkbdhk.cpp,v 1.10 2003-10-22 16:10:59 sandervl Exp $ */
/*
 * OS/2 native Presentation Manager hooks
 *
 *
 * Large Portions (C) Ulrich M”ller, XWorkplace
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 * Copyright 2002-2003 Innotek Systemberatung GmbH
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_WIN
#define  INCL_WININPUT
#define  INCL_WINMESSAGEMGR
#define  INCL_WINHOOKS
#define  INCL_PM
#define  INCL_BASE
#define  INCL_ERRORS
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
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
   - Whenever process terminates OS/2 frees all hooks process initiated,
     no matter whether DLL is still in memory or not. So in sequence:
     Load 1 VIO app, load 2 VIO app, close 1st VIO app - KBD hook stops working because
     it was released by OS/2 on 1 process. Creating hooks for each VIO app solves
     this problem. Hook processes needed messages and removes them from other
     hooks if needed. On termination of process 1 we still have hook of process 2.
*/


BOOL EXPENTRY hookPreAccelHook(HAB hab, PQMSG pqmsg, ULONG option);

HOOKDATA G_HookData = {0};

unsigned long _System _DLL_InitTerm(unsigned long hModule,
                                    unsigned long ulFlag)
{
    switch (ulFlag)
    {
        case 0:
        {
            // store the DLL handle in the global variable
            G_HookData.hmodDLL = hModule;
            break;
        }

        case 1:
            break;

        default:
            return (0);     // error
    }
    // a non-zero value must be returned to indicate success
    return (1);
}

/*
 *@@ hookInit:
 *  registers (sets) all the hooks and initializes data.
 *  we do call this for every process.
 */

BOOL WIN32API hookInit(HAB hab, PSZ pszWindowClassName)
{
   if (G_HookData.hmodDLL)       // initialized by _DLL_InitTerm 
   {
        BOOL fSuccess;

        // install hooks, for each app use its own hook
        // hooks automatically releases when process finishes
        // so we need to set hook for each vio app. The reason why
        // djmutex used one global hook is because it was loaded
        // by daemon.

        G_HookData.habDaemonObject = hab;
        G_HookData.fPreAccelHooked = WinSetHook(hab, NULLHANDLE, // system hook
                                                HK_PREACCEL,  // pre-accelerator table hook (undocumented)
                                                (PFN)hookPreAccelHook, 
                                                G_HookData.hmodDLL);

        strncpy(G_HookData.szWindowClass, pszWindowClassName, sizeof(G_HookData.szWindowClass));
        return TRUE;
    }
    return FALSE;
}

/*
 *@@ hookKill:
 *      deregisters the hook function and frees allocated
 *      resources.
 */

BOOL WIN32API hookKill()
{
    // PM will cleanup the hook when the the creators message queue is destroyed
    // or process terminates.

    if (G_HookData.fPreAccelHooked)
    {
        WinReleaseHook(G_HookData.habDaemonObject,
                       NULLHANDLE,
                       HK_PREACCEL,     // pre-accelerator table hook (undocumented)
                       (PFN)hookPreAccelHook,
                       G_HookData.hmodDLL);

        G_HookData.fPreAccelHooked = FALSE;
    }

    return TRUE;
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
  CHAR szClass[80];

  if (hwndLastWin32Window == hwnd)
    return TRUE;

  if (WinQueryClassName(hwnd, sizeof(szClass), szClass))
  {
    if (strcmp(szClass, G_HookData.szWindowClass) == 0)
    {
      hwndLastWin32Window = hwnd;
      return TRUE;
    }
  }
  return FALSE;
}


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
 * ODIN: Keep in mind that effort must be taken to make processing as
 *       optimal as possible as this is install by each and every Odin
 *       process and they will process and reject the exact same messages.
 *       Only the first called hook will actually do something useful.
 */

BOOL EXPENTRY hookPreAccelHook(HAB hab, PQMSG pqmsg, ULONG option)
{
    if (pqmsg == NULL)
        return (FALSE);

    switch(pqmsg->msg)
    {

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

             // Intercept PM Window Hotkeys such as
             // Alt-F7 do enable window moving by keyboard.
             case PMSCAN_F1:
             case PMSCAN_F2:
             case PMSCAN_F3:
             case PMSCAN_F4:
             case PMSCAN_F5:
             case PMSCAN_F6:
             case PMSCAN_F7:
             case PMSCAN_F8:
             case PMSCAN_F9:
             case PMSCAN_F10:
             case PMSCAN_F11:
             case PMSCAN_F12: 

             // Try to prevent Ctrl-Esc, etc. from being intercepted by PM
             case PMSCAN_ESC:
             case PMSCAN_CTRLLEFT:
             case PMSCAN_CTRLRIGHT:

             case PMSCAN_PRINT:
             case PMSCAN_ALTLEFT:
             case PMSCAN_SCROLLLOCK:
             case PMSCAN_ENTER:
             case PMSCAN_PADENTER:
             case PMSCAN_CAPSLOCK:
               // OK, as we've got a special key here, we've got
               // to rewrite the message so PM will ignore the key
               // and won't translate the message to anything else.

               pqmsg->msg = WM_CHAR_SPECIAL;

               break;

        //
        // AltGr needs special handling
        //
        // AltGr -> WM_KEYDOWN (VK_CONTROL), WM_KEYDOWN (VK_MENU)
        //          WM_SYSKEYUP (VK_CONTROL)
        //          WM_KEYUP (VK_MENU)
        //
        // Ctrl+AltGr -> WM_KEYDOWN (VK_CONTROL), WM_KEYUP (VK_CONTROL)
        //               WM_KEYDOWN (VK_CONTROL)
        //               WM_KEYDOWN (VK_MENU)
        //               WM_KEYUP (VK_MENU)
        //               WM_KEYUP (VK_CONTROL)
        //
        // AltGr+Ctrl -> WM_KEYDOWN (VK_CONTROL), WM_KEYDOWN (VK_MENU)
        //               WM_KEYDOWN (VK_CONTROL)
        //               WM_SYSKEYUP (VK_CONTROL)
        //               WM_SYSKEYUP (VK_CONTROL)
        //               WM_KEYUP (VK_MENU)
        //
        // AltGr down -> if Ctrl down, send WM_KEYUP (VK_CONTROL)
        //               endif
        //               Send WM_KEYDOWN (VK_CONTROL)
        //               Send WM_KEYDOWN (VK_MENU)
        // AltGr up ->   if !(Ctrl down before AltGr was pressed || Ctrl up)
        //                   Send WM_SYSKEYUP (VK_CONTROL)
        //               endif
        //               Send WM_KEYDOWN (VK_MENU)
        //
        // NOTE: Ctrl = Ctrl-Left; AltGr doesn't care about the right Ctrl key
        //
             case PMSCAN_ALTRIGHT:
             {
                QMSG   msg = *pqmsg;
                ULONG  ctrlstate;
                ULONG  flags;
                ULONG  mp1, mp2;

                flags = SHORT1FROMMP(pqmsg->mp1);

                pqmsg->msg = WM_CHAR_SPECIAL;

                if (flags & KC_KEYUP)
                {   
                  //AltGr up
                  ctrlstate  = WinGetPhysKeyState(HWND_DESKTOP, PMSCAN_CTRLLEFT);
                  if (!(ctrlstate & 0x8000))
                  {  
                      //ctrl is up, translate this message to Ctrl key up
                      mp1  = (PMSCAN_CTRLLEFT << 24);	//scancode
                      mp1 |= (1 << 16);			//repeat count
                      mp1 |= (KC_ALT | KC_KEYUP | KC_VIRTUALKEY | KC_SCANCODE);
                      mp2  = (VK_CTRL << 16);		//virtual keycode
                      pqmsg->msg = WM_CHAR_SPECIAL_ALTGRCONTROL;
                      pqmsg->mp1 = (MPARAM)mp1;
                      pqmsg->mp2 = (MPARAM)mp2;

                      //and finally, post the AltGr WM_CHAR message
                      WinPostMsg(msg.hwnd, WM_CHAR_SPECIAL, msg.mp1, msg.mp2);
                  }
                  //else do nothing
               }
               else
               {
                  //AltGr down
                  ctrlstate  = WinGetPhysKeyState(HWND_DESKTOP, PMSCAN_CTRLLEFT);
                  if (ctrlstate & 0x8000)
                  {
                      //ctrl is down, translate this message to Ctrl key up
                      mp1  = (PMSCAN_CTRLLEFT << 24);	//scancode
                      mp1 |= (1 << 16);			//repeat count
                      mp1 |= (KC_KEYUP | KC_VIRTUALKEY | KC_SCANCODE);
                      mp2  = (VK_CTRL << 16);		//virtual keycode
                      pqmsg->msg = WM_CHAR_SPECIAL_ALTGRCONTROL;
                      pqmsg->mp1 = (MPARAM)mp1;
                      pqmsg->mp2 = (MPARAM)mp2;
                  }
                  //send left control key down message
                  mp1  = (PMSCAN_CTRLLEFT << 24);	//scancode
                  mp1 |= (1 << 16);			//repeat count
                  mp1 |= (KC_CTRL | KC_VIRTUALKEY | KC_SCANCODE);
                  mp2  = (VK_CTRL << 16);		//virtual keycode

                  if (ctrlstate & 0x8000)
                  {
                      //ctrl is down, must post this message
                      WinPostMsg(msg.hwnd, WM_CHAR_SPECIAL_ALTGRCONTROL, (MPARAM)mp1, (MPARAM)mp2);
                  }
                  else
                  {
                      //translate this message into control key down
                      pqmsg->msg = WM_CHAR_SPECIAL_ALTGRCONTROL;
                      pqmsg->mp1 = (MPARAM)mp1;
                      pqmsg->mp2 = (MPARAM)mp2;
                  }
                  //and finally, post the AltGr WM_CHAR message
                  WinPostMsg(msg.hwnd, WM_CHAR_SPECIAL, msg.mp1, msg.mp2);
              }
              break;
           }
        }
        break; // WM_CHAR
      }
    } // end switch(msg)
    return FALSE;
}
