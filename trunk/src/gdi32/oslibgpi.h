/* $Id: oslibgpi.h,v 1.3 2000-01-20 21:39:36 sandervl Exp $ */

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

#define APIENTRYOS2 _System

PVOID APIENTRYOS2 _GpiQueryDCData(ULONG hps);

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

typedef struct //same as Win32 POINT
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
INT  getWorldYDeltaFor1Pixel(PVOID pHps);
INT  getWorldXDeltaFor1Pixel(PVOID pHps);
BOOL getInPath(PVOID pHps);
VOID setInPath(PVOID pHps,BOOL inPath);
BOOL getIsWideLine(PVOID pHps);
BOOL getIsTopTop(PVOID pHps);
ULONG getMapMode(PVOID pHps);

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

LONG OSLibGpiQueryTabbedTextExtent(PVOID pHps,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops);
LONG OSLibGpiTabbedCharStringAt(PVOID pHps,PPOINTLOS2 pPtStart,PRECTLOS2 prclRect,ULONG flOptions,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops,INT lTabOrigin);

#define TXTBOXOS_TOPLEFT                  0L
#define TXTBOXOS_BOTTOMLEFT               1L
#define TXTBOXOS_TOPRIGHT                 2L
#define TXTBOXOS_BOTTOMRIGHT              3L
#define TXTBOXOS_CONCAT                   4L
#define TXTBOXOS_COUNT                    5L

BOOL OSLibGpiQueryTextBox(PVOID pHps,LONG lCount1,LPCSTR pchString,LONG lCount2,PPOINTLOS2 aptlPoints);

VOID calcDimensions(POINTLOS2 box[],PPOINTLOS2 point);

#define DTOS_LEFT                    0x00000000
#define DTOS_QUERYEXTENT             0x00000002
#define DTOS_UNDERSCORE              0x00000010
#define DTOS_STRIKEOUT               0x00000020
#define DTOS_TEXTATTRS               0x00000040
#define DTOS_EXTERNALLEADING         0x00000080
#define DTOS_CENTER                  0x00000100
#define DTOS_RIGHT                   0x00000200
#define DTOS_TOP                     0x00000000
#define DTOS_VCENTER                 0x00000400
#define DTOS_BOTTOM                  0x00000800
#define DTOS_HALFTONE                0x00001000
#define DTOS_MNEMONIC                0x00002000
#define DTOS_WORDBREAK               0x00004000
#define DTOS_ERASERECT               0x00008000

#define BMOS_ERROR                      (-1L)
#define BMOS_DEFAULT                      0L
#define BMOS_OR                           1L
#define BMOS_OVERPAINT                    2L
#define BMOS_LEAVEALONE                   5L

LONG OSLibGpiQueryBackMix(PVOID pHps);

#define MMOS_TEXT             1
#define MMOS_LOMETRIC         2
#define MMOS_HIMETRIC         3
#define MMOS_LOENGLISH        4
#define MMOS_HIENGLISH        5
#define MMOS_TWIPS            6
#define MMOS_ISOTROPIC        7
#define MMOS_ANISOTROPIC      8

BOOL doesYAxisGrowNorth(PVOID pHps);

LONG OSLibWinDrawTabbedText(PVOID pHps,LONG cchText,LONG lTabs,LPCSTR lpchText,PVOID prcl,LONG clrFore,LONG clrBack,ULONG flCmd);


BOOL OSLibGpiMove(PVOID pHps,PPOINTLOS2 pptlPoint);
LONG OSLibGpiLine(PVOID pHps,PPOINTLOS2 pptlEndPoint);

BOOL OSLibGpiEndPath(PVOID pHps);

BOOL drawLinePointCircle(PVOID pHps,INT width,INT height,LONG color);
BOOL drawLinePoint(PVOID pHps,PPOINTLOS2 pt,LONG color);

ULONG OSLibGpiQueryCp(HDC hdc);
BOOL  OSLibGpiSetCp(HDC hdc, ULONG codepage);

#endif
