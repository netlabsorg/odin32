/* $Id: oslibgpi.h,v 1.1 2001-09-30 22:23:46 sandervl Exp $ */

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

LPRGNDATA OSLibQueryVisibleRegion(HWND hwnd, DWORD screenHeight);

#ifdef OS2_INCLUDED

#define MapOS2ToWin32Rect(rectOS2,rectWin32)  \
{                                             \
  (rectWin32).left   = (rectOS2).xLeft;       \
  (rectWin32).bottom = (rectOS2).yBottom;     \
  (rectWin32).right  = (rectOS2).xRight;      \
  (rectWin32).top    = (rectOS2).yTop;        \
}


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

#endif

#endif
