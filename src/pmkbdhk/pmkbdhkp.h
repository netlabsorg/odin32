/* $Id: pmkbdhkp.h,v 1.2 2001-10-23 13:56:27 achimha Exp $ */
/*
 * Window Menu wrapper functions for OS/2
 *
 *
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 *
 *
 *      Copyright (C) 1999-2000 Ulrich M”ller.
 */
 
#ifndef __PMKBDHK_PRIVATE_H__
#define __PMKBDHK_PRIVATE_H__


/*
* HK_PREACCEL:
*      additional undocumented PM hook type,
*      for pre-accelerator table hooks
*      (see xwphook.c for details);
*      this definition taken out of the
*      ProgramCommander/2 source (thanks,
*      Roman Stangl).
*/

#ifndef HK_PREACCEL
#define HK_PREACCEL             17
#endif


/*
 * Note: this won't interfere with any WM_USER value,
 * WM_PENFIRST is 0x04c0
 */
#ifndef WM_CHAR_SPECIAL
#define WM_CHAR_SPECIAL 0x04bf
#endif


/*
 * These definitions come from user32\pmframe.h.
 * They're repeated here to avoid header conflicts
 * with the USER32 module
 */

#ifndef WIN32_STDCLASS
#define WIN32_STDCLASS      "Win32WindowClass"
#endif

#ifndef WIN32_STDFRAMECLASS
#define WIN32_STDFRAMECLASS "Win32FrameClass"
#endif

    /* ******************************************************************
     *                                                                  *
     *   Structures                                                     *
     *                                                                  *
     ********************************************************************/


    /*
     *@@ HOOKDATA:
     *      global hook data structure. Only one instance
     *      of this is in the shared data segment of
     *      XWPHOOK.DLL, and a pointer to that structure
     *      is returned to the daemon which initially loads
     *      that DLL by hookInit and then stored by the
     *      daemon. As a result, this structure is shared
     *      between the hook and the daemon, and both can
     *      access it at any time.
     *
     *      This is statically initialized to 0 when the hook
     *      DLL is loaded. hookInit will then set up most
     *      fields in here.
     *
     *      This contains setup data (the state of the
     *      hook), some data which needs to be cached,
     *      as well as the HOOKCONFIG structure which
     *      is used to configure the hook and the daemon.
     *      That sub-structure gets (re)loaded from OS2.INI
     *      upon daemon startup and when XDM_HOOKCONFIG is
     *      received by fnwpDaemonObject.
     */

    typedef struct _HOOKDATA
    {
        BOOL        fPreAccelHooked;    // pre-accelerator table hook installed?

        HAB         habDaemonObject;
                // anchor block of hwndDaemonObject; cached for speed

        HMODULE     hmodDLL;
                // XWPHOOK.DLL module handle

        HWND        hwndPMDesktop;
                // desktop window handle (WinQueryDesktopWindow)
      
        HWND        hwndWindowList;
                // window list handle
      
        ULONG       pidPM;
                // process ID of first PMSHELL.EXE V0.9.7 (2001-01-21) [umoeller]

//        HWND        hwndLockupFrame;
                // current lockup window, if any
    } HOOKDATA, *PHOOKDATA;


    /* ******************************************************************
     *
     *   Hook DLL prototypes
     *
     ********************************************************************/

    PHOOKDATA EXPENTRY hookInit(HWND hwndDaemonObject);

    BOOL EXPENTRY hookKill(VOID);

    /* ******************************************************************
     *
     *   Internal prototypes
     *
     ********************************************************************/

    extern HOOKDATA G_HookData;

#endif //__PMKBDHK_PRIVATE_H__
