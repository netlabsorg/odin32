/* $Id: oslibwin.h,v 1.2 1999-07-15 18:03:02 sandervl Exp $ */
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

#define OS2TOWIN32POINT(height, y)	(height - y)

#define OSLIB_HWND_DESKTOP	0
BOOL  OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw = TRUE);


HWND  OSLibWinCreateWindow(HWND hwndParent, ULONG dwWinStyle, ULONG dwFrameStyle,
                           char *pszName, ULONG x, ULONG y, ULONG cx, ULONG cy,
                           HWND Owner, ULONG fHWND_BOTTOM);

BOOL  OSLibWinConvertStyle(ULONG dwStyle, ULONG *OSWinStyle, ULONG *OSFrameStyle);

BOOL  OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value);
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset);

BOOL  OSLibPostMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam);

#endif //__OSLIBWIN_H__