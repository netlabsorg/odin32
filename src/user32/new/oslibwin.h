/* $Id: oslibwin.h,v 1.9 1999-07-18 10:39:51 sandervl Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBWIN_H__
#define __OSLIBWIN_H__

#define OSLIB_HWND_DESKTOP      0
BOOL  OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw = TRUE);


HWND  OSLibWinCreateWindow(HWND hwndParent, ULONG dwWinStyle, ULONG dwFrameStyle,
                           char *pszName, HWND Owner, ULONG fHWND_BOTTOM, HWND *hwndFrame);

BOOL  OSLibWinConvertStyle(ULONG dwStyle, ULONG *OSWinStyle, ULONG *OSFrameStyle);

BOOL  OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value);
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset);

BOOL  OSLibPostMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam);

HWND  OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate);

#define WA_WARNING                 0
#define WA_NOTE                    1
#define WA_ERROR                   2
#define WA_CWINALARMS             13     /* count of valid alarms     53304*/

BOOL OSLibWinAlarm(HWND hwndDeskTop,ULONG flStyle);
ULONG OSLibDosBeep(ULONG freg,ULONG dur);

#define SWPOS_SIZE                   0x0001
#define SWPOS_MOVE                   0x0002
#define SWPOS_ZORDER                 0x0004
#define SWPOS_SHOW                   0x0008
#define SWPOS_HIDE                   0x0010
#define SWPOS_NOREDRAW               0x0020
#define SWPOS_NOADJUST               0x0040
#define SWPOS_ACTIVATE               0x0080
#define SWPOS_DEACTIVATE             0x0100
#define SWPOS_EXTSTATECHANGE         0x0200
#define SWPOS_MINIMIZE               0x0400
#define SWPOS_MAXIMIZE               0x0800
#define SWPOS_RESTORE                0x1000
#define SWPOS_FOCUSACTIVATE          0x2000
#define SWPOS_FOCUSDEACTIVATE        0x4000
#define SWPOS_NOAUTOCLOSE            0x8000    /* Valid in PROGDETAILS struct only */

#define HWNDOS_TOP                      (HWND)3
#define HWNDOS_BOTTOM                   (HWND)4

BOOL  OSLibWinSetWindowPos(HWND hwnd, HWND hwndInsertBehind, LONG x, LONG y, LONG cx, LONG cy, ULONG fl);
BOOL  OSLibWinShowWindow(HWND hwnd, ULONG fl);

BOOL  OSLibWinDestroyWindow(HWND hwnd);
BOOL  OSLibWinQueryUpdateRect(HWND hwnd, PRECT pRect);
BOOL  OSLibWinIsIconic(HWND hwnd);
BOOL  OSLibWinSetActiveWindow(HWND hwnd);
BOOL  OSLibWinSetFocus(HWND hwnd);
BOOL  OSLibWinEnableWindow(HWND hwnd, BOOL fEnable);
BOOL  OSLibWinIsWindowEnabled(HWND hwnd);
BOOL  OSLibWinIsWindowVisible(HWND hwnd);
BOOL  OSLibWinQueryActiveWindow();

#define QWOS_NEXT         0
#define QWOS_PREV         1
#define QWOS_TOP          2
#define QWOS_BOTTOM       3
#define QWOS_OWNER        4
#define QWOS_PARENT       5
#define QWOS_NEXTTOP      6
#define QWOS_PREVTOP      7
#define QWOS_FRAMEOWNER   8

HWND  OSLibWinQueryWindow(HWND hwnd, ULONG lCode);


void  OSLibWinPostQuitMessage(ULONG nExitCode);
LONG  OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode = FALSE);

BOOL  OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, BOOL isUnicode = FALSE);

LONG  OSLibWinQueryWindowTextLength(HWND hwnd);
LONG  OSLibWinQueryWindowText(HWND hwnd, LONG length, LPSTR lpsz);
BOOL  OSLibWinSetWindowText(HWND hwnd, LPSTR lpsz);

#endif //__OSLIBWIN_H__
