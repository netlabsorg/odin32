/*
 *  OS/2 System Tray support.
 *
 *  Common definitions.
 */

#ifndef __WINE_SYSTRAY_OS2_H
#define __WINE_SYSTRAY_OS2_H

#include "systray.h"

// define some Win32 structs and types in terms of OS/2 types
// to avoid inclusion of shellapi.h that would create conflicts

#define NIF_MESSAGE             0x00000001
#define NIF_ICON                0x00000002
#define NIF_TIP                 0x00000004

typedef struct _NOTIFYICONDATAA
{   ULONG    cbSize;
    HWND     hWnd;
    ULONG    uID;
    ULONG    uFlags;
    ULONG    uCallbackMessage;
    HPOINTER hIcon;
    CHAR     szTip[64];
} NOTIFYICONDATAA, *PNOTIFYICONDATAA;

// Note: This must match _SystrayItem from systray.c
struct _SystrayItem {
  HWND                  hWndFrame;
  HWND                  hWndClient;
  NOTIFYICONDATAA       notifyIcon;
  ULONG                 uIdx;
  struct _SystrayItem   *nextTrayItem;
};

BOOL DoWin32PostMessage(HWND, ULONG, MPARAM, MPARAM);

BOOL SYSTRAY_Ex_Init(void);

#endif // __WINE_SYSTRAY_OS2_H
