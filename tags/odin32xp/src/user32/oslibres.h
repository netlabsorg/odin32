/* $Id: oslibres.h,v 1.9 2001-07-04 09:55:17 sandervl Exp $ */
/*
 * Window GUI resource wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBRES_H__
#define __OSLIBRES_H__

#ifdef OS2_INCLUDED
#include <win32type.h>
#endif

#include <win\cursoricon.h>

#ifdef OS2_INCLUDED
typedef struct
{
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;
#endif

typedef struct
{
    INT  bmType;
    INT  bmWidth;
    INT  bmHeight;
    INT  bmWidthBytes;
    WORD   bmPlanes;
    WORD   bmBitsPixel;
    LPVOID bmBits;
} BITMAP_W, *LPBITMAP_W;

HANDLE  OSLibWinSetAccelTable(HWND hwnd, HANDLE hAccel, PVOID acceltemplate);

HANDLE  OSLibWinCreatePointer(CURSORICONINFO *pInfo, char *pAndBits, BITMAP_W *pAndBmp, char *pXorBits, BITMAP_W *pXorBmp, BOOL fCursor);
HANDLE  OSLibWinQuerySysPointer(ULONG type,INT w,INT h);
HANDLE  OSLibWinQuerySysIcon(ULONG type,INT w,INT h);
VOID    OSLibWinDestroyPointer(HANDLE hPointer);
BOOL    OSLibWinSetPointer(HANDLE hPointer);
HANDLE  OSLibWinQueryPointer();
BOOL    OSLibWinClipCursor(const RECT * pRect);
BOOL    OSLibWinGetClipCursor(LPRECT pRect);



BOOL APIENTRY _WinSetPointerClipRect(HWND hwnd, PRECTL pRect);
BOOL APIENTRY _WinQueryPointerClipRect(HWND hwnd, PRECTL pRect);

inline BOOL APIENTRY WinSetPointerClipRect(HWND hwnd, PRECTL pRect)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _WinSetPointerClipRect(hwnd, pRect);
    SetFS(sel);

    return yyrc;
}

inline BOOL APIENTRY WinQueryPointerClipRect(HWND hwnd, PRECTL pRect)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _WinQueryPointerClipRect(hwnd, pRect);
    SetFS(sel);

    return yyrc;
}

#endif //__OSLIBGDI_H__
