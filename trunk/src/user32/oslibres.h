/* $Id: oslibres.h,v 1.6 2001-03-27 16:17:52 sandervl Exp $ */
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
HANDLE  OSLibWinCreateIcon(PVOID iconbitmap, ULONG cxDesired, ULONG cyDesired);

//NOTE: Depends on origin of bitmap data!!!
//      Assumes 1 bpp bitmaps have a top left origin and all others have a bottom left origin
HANDLE  OSLibWinCreatePointer(PVOID cursorbitmap, ULONG cxDesired, ULONG cyDesired);

HANDLE  OSLibWinCreateCursor(CURSORICONINFO *pInfo, char *pAndBits, BITMAP_W *pAndBmp, char *pXorBits, BITMAP_W *pXorBmp);
HANDLE  OSLibWinQuerySysPointer(ULONG type,INT w,INT h);
HANDLE  OSLibWinQuerySysIcon(ULONG type,INT w,INT h);
VOID    OSLibWinDestroyPointer(HANDLE hPointer);
BOOL    OSLibWinSetPointer(HANDLE hPointer);

#endif //__OSLIBGDI_H__
