/*
 *  OS/2 System Tray support.
 *
 *  Common definitions.
 */

#ifndef __WINE_SYSTRAY_OS2_H
#define __WINE_SYSTRAY_OS2_H

// map some Win32 types used in systray.h to OS/2 types
// to avoid Win32 header inclusion and conflicts

#define HICON HPOINTER

#include "systray.h"

// define some Win32 structs in terms of OS/2 types
// to avoid Win32 header inclusion and conflicts

typedef struct _NOTIFYICONDATAA
{   ULONG    cbSize;
    HWND     hWnd;
    ULONG    uID;
    ULONG    uFlags;
    ULONG    uCallbackMessage;
    HPOINTER hIcon;
    CHAR     szTip[64];
} NOTIFYICONDATAA, *PNOTIFYICONDATAA;

struct _SystrayItem {
  HWND                  hWndFrame;
  HWND                  hWndClient;
  NOTIFYICONDATAA       notifyIcon;
  struct _SystrayItem   *nextTrayItem;
};

BOOL DoWin32PostMessage(HWND, ULONG, MPARAM, MPARAM);
BOOL DoWin32CharToOem(const char *s, char *t);

BOOL SYSTRAY_Ex_Init(void);

#endif // __WINE_SYSTRAY_OS2_H
