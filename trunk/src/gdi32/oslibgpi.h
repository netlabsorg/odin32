/* $Id: oslibgpi.h,v 1.1 1999-12-09 16:49:45 cbratschi Exp $ */

/*
 * GPI interface code
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBGPI_H__
#define __OSLIBGPI_H__

/* DC Graphics Mode */
#define GM_COMPATIBLE_W     1
#define GM_ADVANCED_W       2

PVOID APIENTRY _GpiQueryDCData(ULONG hps);

inline PVOID OSLibGpiQueryDCData(ULONG hps)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiQueryDCData(hps);
    SetFS(sel);

    return yyrc;
}

typedef struct
{
  LONG  xLeft;
  LONG  yBottom;
  LONG  xRight;
  LONG  yTop;
} RECTLOS2, *PRECTLOS2;

typedef struct
{
  LONG  x;
  LONG  y;
} POINTLOS2, *PPOINTLOS2;

#define MapWin32ToOS2Rect(rectWin32,rectOS2)  \
{                                             \
  (rectOS2).xLeft   = (rectWin32).left;       \
  (rectOS2).yBottom = (rectWin32).bottom;     \
  (rectOS2).xRight  = (rectWin32).right;      \
  (rectOS2).yTop    = (rectWin32).top;        \
}

#define MapOS2ToWin32Rect(rectOS2,rectWin32)  \
{                                             \
  (rectWin32).left   = (rectOS2).xLeft;       \
  (rectWin32).bottom = (rectOS2).yBottom;     \
  (rectWin32).right  = (rectOS2).xRight;      \
  (rectWin32).top    = (rectOS2).yTop;        \
}

BOOL excludeBottomRightPoint(PVOID pHps,PPOINTLOS2 pptl);
BOOL getAlignUpdateCP(PVOID pHps);
INT getWorldYDeltaFor1Pixel(PVOID pHps);
INT getWorldXDeltaFor1Pixel(PVOID pHps);

BOOL OSLibGpiQueryCurrentPosition(PVOID pHps,PPOINTLOS2 ptl);
BOOL OSLibGpiSetCurrentPosition(PVOID pHps,PPOINTLOS2 ptl);

#define CHSOS_OPAQUE                 0x0001L
#define CHSOS_VECTOR                 0x0002L
#define CHSOS_LEAVEPOS               0x0008L
#define CHSOS_CLIP                   0x0010L
#define CHSOS_UNDERSCORE             0x0200L
#define CHSOS_STRIKEOUT              0x0400L

#define GPIOS_ERROR                       0L
#define GPIOS_OK                          1L
#define GPIOS_ALTERROR                  (-1L)

BOOL OSLibGpiCharStringPosAt(PVOID pHps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx);
BOOL OSLibGpiQueryCharStringPosAt(PVOID pHps,PPOINTLOS2 ptl,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx,PPOINTLOS2 aptlPos);

#define TAOS_NORMAL_HORIZ     0x0001
#define TAOS_LEFT             0x0002
#define TAOS_CENTER           0x0003
#define TAOS_RIGHT            0x0004
#define TAOS_STANDARD_HORIZ   0x0005
#define TAOS_NORMAL_VERT      0x0100
#define TAOS_TOP              0x0200
#define TAOS_HALF             0x0300
#define TAOS_BASE             0x0400
#define TAOS_BOTTOM           0x0500
#define TAOS_STANDARD_VERT    0x0600

BOOL OSLibGpiSetTextAlignment(PVOID pHps,LONG lHoriz,LONG lVert);
BOOL OSLibGpiQueryTextAlignment(PVOID pHps,PLONG plHoriz,PLONG plVert);

#endif
