/* $Id: oslibgpi.h,v 1.10 2001-05-19 19:43:54 sandervl Exp $ */

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

#include <dcdata.h>

#ifdef DEBUG
void dprintfOrigin(HDC hdc);
#else
#define dprintfOrigin(a)
#endif

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

BOOL APIENTRYOS2 _GpiEnableYInversion(ULONG hps, LONG lHeight);

inline BOOL GpiEnableYInversion(ULONG hps, LONG lHeight)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiEnableYInversion(hps, lHeight);
    SetFS(sel);

    return yyrc;
}

BOOL APIENTRYOS2 _GpiQueryYInversion(ULONG hps);

inline BOOL GpiQueryYInversion(ULONG hps)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiQueryYInversion(hps);
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
BOOL includeBottomRightPoint(PVOID pHps,PPOINTLOS2 pptl);
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

LONG OSLibGpiQueryTabbedTextExtent(pDCData pHps,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops);
LONG OSLibGpiTabbedCharStringAt(PVOID pHps,PPOINTLOS2 pPtStart,PRECTLOS2 prclRect,ULONG flOptions,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops,INT lTabOrigin);

#define TXTBOXOS_TOPLEFT                  0L
#define TXTBOXOS_BOTTOMLEFT               1L
#define TXTBOXOS_TOPRIGHT                 2L
#define TXTBOXOS_BOTTOMRIGHT              3L
#define TXTBOXOS_CONCAT                   4L
#define TXTBOXOS_COUNT                    5L

BOOL OSLibGpiQueryTextBox(pDCData pHps,LONG lCount1,LPCSTR pchString,LONG lCount2,PPOINTLOS2 aptlPoints);

VOID calcDimensions(POINTLOS2 box[],PPOINTLOS2 point);

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

#define DTOS_SYSTEMFONT       0x00000001
#define DTOS_SINGLELINE       0x00010000
#define DTOS_MULTILINE        0x00020000
#define DTOS_NOCLIP           0x00040000
#define DTOS_OPAQUE           0x00080000
#define DTOS_AMPERSAND        0x00100000
#define DTOS_VERTICALEXTENT   0x00200000
#define DTOS_INVERTCHAR       0x20000000
#define DTOS_WORLDRECT        0x40000000
#define DTOS_INVERT           0x80000000

LONG OSLibWinDrawTabbedText(PVOID pHps,LONG cchText,LONG lTabs,LPCSTR lpchText,PVOID prcl,LONG clrFore,LONG clrBack,ULONG flCmd);

int OSLibGpiQueryFontMaxHeight(HDC hdc);

BOOL OSLibGpiMove(PVOID pHps,PPOINTLOS2 pptlPoint);
LONG OSLibGpiLine(PVOID pHps,PPOINTLOS2 pptlEndPoint);

BOOL OSLibGpiEndPath(PVOID pHps);

BOOL drawLinePointCircle(PVOID pHps,INT width,INT height,LONG color);
BOOL drawLinePoint(PVOID pHps,PPOINTLOS2 pt,LONG color);

ULONG OSLibGpiQueryCp(HDC hdc);
BOOL  OSLibGpiSetCp(HDC hdc, ULONG codepage);

#define OSLIB_CAPS_HORIZONTAL_RESOLUTION      8L      /* pels per meter  */
#define OSLIB_CAPS_VERTICAL_RESOLUTION        9L      /* pels per meter  */

BOOL  OSLibDevQueryCaps(pDCData hHps, LONG lStart, LONG lCount, LONG *alArray);

   // from pmddi.h:
   /* CopyClipRegion */

   #define COPYCRGN_ALLINTERSECT        0L
   #define COPYCRGN_VISRGN              1L
   #define COPYCRGN_CLIPRGN             2L

   /* SetupDC */

   #define SETUPDC_VISRGN               0x00000001L
   #define SETUPDC_ORIGIN               0x00000002L
   #define SETUPDC_ACCUMBOUNDSON        0x00000004L
   #define SETUPDC_ACCUMBOUNDSOFF       0x00000008L
   #define SETUPDC_RECALCCLIP           0x00000010L
   #define SETUPDC_SETOWNER             0x00000020L
   #define SETUPDC_CLEANDC              0x00000040L

ULONG APIENTRY Gre32Entry4(ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry4 (ULONG a, ULONG b, ULONG c, ULONG d)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry4(a,b,c,d);
    SetFS(sel);

    return yyrc;
}

ULONG APIENTRY Gre32Entry5(ULONG,ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry5 (ULONG a, ULONG b, ULONG c, ULONG d, ULONG e)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry5(a,b,c,d,e);
    SetFS(sel);

    return yyrc;
}


ULONG APIENTRY Gre32Entry6(ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry6 (ULONG a, ULONG b, ULONG c, ULONG d, ULONG e, ULONG f)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry6(a,b,c,d,e,f);
    SetFS(sel);

    return yyrc;
}

ULONG APIENTRY Gre32Entry7(ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry7 (ULONG a, ULONG b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry7(a,b,c,d,e,f,g);
    SetFS(sel);

    return yyrc;
}

ULONG APIENTRY Gre32Entry8(ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);

inline ULONG APIENTRY _Gre32Entry8 (ULONG a, ULONG b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g, ULONG h)
{
 ULONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Gre32Entry8(a,b,c,d,e,f,g,h);
    SetFS(sel);

    return yyrc;
}


   #define GreSetupDC(a,b,c,d,e,f) (BOOL) _Gre32Entry8((ULONG)(HDC)(a),(ULONG)(HRGN)(b),(ULONG)(LONG)(c),(ULONG)(LONG)(d),(ULONG)(PRECTL)(e),(ULONG)(ULONG)(f),0L,0x00004081L)
   #define GreCreateRectRegion(a,b,c) (HRGN) _Gre32Entry5((ULONG)(HDC)(a),(ULONG)(PRECTL)(b),(ULONG)(LONG)(c),0L,0x00004062L)
   #define GreGetPageUnits(a,b) (LONG) _Gre32Entry4((ULONG)(HDC)(a),(ULONG)(PULONG)(b),0L,0x00004083L)
   #define GreCopyClipRegion(a,b,c,d) (INT) _Gre32Entry6((ULONG)(HDC)(a),(ULONG)(HRGN)(b),(ULONG)(PRECTL)(c),(ULONG)(ULONG)(d),0L,0x00004080L)
   #define GreCombineRegion(a,b,c,d,e) (INT) _Gre32Entry7((ULONG)(HDC)(a),(ULONG)(HRGN)(b),(ULONG)(HRGN)(c),(ULONG)(HRGN)(d),(ULONG)(ULONG)(e),0L,0x00004065L)
   #define GreDestroyRegion(a,b) (BOOL) _Gre32Entry4((ULONG)(HDC)(a),(ULONG)(HRGN)(b),0L,0x00004063L)
   #define GreGetDCOrigin(a,b) (BOOL) _Gre32Entry4((ULONG)(HDC)(a),(ULONG)(PPOINTL)(b),0L,0x000040BAL)
   #define GreDeviceSetDCOrigin(a,b) (BOOL) _Gre32Entry4((ULONG)(HDC)(a),(ULONG)(PPOINTL)(b),0L,0x000040BBL)
   #define GreSelectClipRegion(a,b,c) (INT) _Gre32Entry5((ULONG)(HDC)(a),(ULONG)(HRGN)(b),(ULONG)(PHRGN)(c),0L,0x00004074L)
   #define GreIntersectClipRectangle(a,b) (INT) _Gre32Entry4((ULONG)(HDC)(a),(ULONG)(PRECTL)(b),0L,0x00004075L)

#ifndef _OS2WIN_H
#define OPEN32API       _System

BOOL    OPEN32API _O32_LPtoDP( HDC, PPOINT, int );

inline BOOL O32_LPtoDP(HDC a, PPOINT b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_LPtoDP(a, b, c);
    SetFS(sel);

    return yyrc;
} 
BOOL    OPEN32API _O32_DPtoLP( HDC, PPOINT, int );

inline BOOL O32_DPtoLP(HDC a, PPOINT b, int c)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DPtoLP(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#endif
#endif
