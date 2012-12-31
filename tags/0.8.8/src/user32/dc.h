/* $Id: dc.h,v 1.19 2004-01-11 12:03:15 sandervl Exp $ */
/*
 * public dc functions
 *
 *
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DC_H__
#define __DC_H__

extern BOOL isYup (struct _DCData *pHps);
extern HDC sendEraseBkgnd (Win32BaseWindow *wnd);
BOOL setPageXForm(Win32BaseWindow *wnd, struct _DCData * pHps);

void selectClientArea(Win32BaseWindow *wnd, HDC hdc);

BOOL GetOS2UpdateRect(Win32BaseWindow *window,LPRECT pRect);

extern "C" int WIN32API GetUpdateRgnFrame(HWND hwnd, HRGN hrgn);

#ifdef DEBUG
void dprintfOrigin(HDC hdc);
#else
#define dprintfOrigin(a)
#endif

#ifdef INCLUDED_BY_DC
/*********************/

typedef struct _penobject
{
   ULONG filler[9];
   union {
     struct {
       PEXTLOGPEN_W      pExtLogPen;
       LOGBRUSH_W        logbrush;
       LOGPEN_W          logpen;
     } ExtPen;
     struct {
       LOGPEN_W          logpen;
     } Pen;
   };
} tPenObject, *pPenObject;


/*********************/

#ifndef OPEN32API
#define OPEN32API _System
#endif


BOOL    APIENTRY _GpiEnableYInversion (HPS hps, LONG lHeight);

inline BOOL APIENTRY GpiEnableYInversion (HPS hps, LONG lHeight)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiEnableYInversion(hps, lHeight);
    SetFS(sel);

    return yyrc;
}

LONG APIENTRY _GpiQueryYInversion(ULONG hps);

inline LONG GpiQueryYInversion(ULONG hps)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiQueryYInversion(hps);
    SetFS(sel);

    return yyrc;
}

PVOID   APIENTRY _GpiAllocateDCData (HPS GpiH, ULONG size);

inline PVOID APIENTRY GpiAllocateDCData (HPS GpiH, ULONG size)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiAllocateDCData(GpiH, size);
    SetFS(sel);

    return yyrc;
}

PVOID   APIENTRY _GpiQueryDCData (HPS hps);

inline PVOID APIENTRY GpiQueryDCData (HPS hps)
{
 PVOID yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _GpiQueryDCData(hps);
    SetFS(sel);

    return yyrc;
}

HDC     OPEN32API _HPSToHDC (HWND hwnd, HPS hps, HDC hdc, PVOID a);

inline HDC OPEN32API HPSToHDC (HWND hwnd, HPS hps, HDC hdc, PVOID a)
{
 HDC yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _HPSToHDC(hwnd, hps, hdc, a);
    SetFS(sel);

    return yyrc;
}

void    OPEN32API _DeleteHDC (HDC hdc);

inline void OPEN32API DeleteHDC (HDC hdc)
{
 USHORT sel = RestoreOS2FS();

    _DeleteHDC(hdc);
    SetFS(sel);
}

BOOL    OPEN32API _O32_EndPaint (HWND hwnd, const PAINTSTRUCT_W *lpps);

inline BOOL O32_EndPaint(HWND a, CONST PAINTSTRUCT_W *b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_EndPaint(a, b);
    SetFS(sel);

    return yyrc;
}

int     OPEN32API _O32_GetUpdateRgn (HWND hwnd, HRGN hrgn, BOOL erase);

inline int O32_GetUpdateRgn(HWND a, HRGN b, BOOL c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetUpdateRgn(a, b, c);
    SetFS(sel);

    return yyrc;
}


BOOL    OPEN32API _O32_DeleteObject (LHANDLE hgdiobj);

inline BOOL O32_DeleteObject(HANDLE a)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_DeleteObject(a);
    SetFS(sel);

    return yyrc;
}

int     OPEN32API _O32_ReleaseDC (HWND hwnd, HDC hdc);

inline int O32_ReleaseDC(HWND a, HDC b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ReleaseDC(a, b);
    SetFS(sel);

    return yyrc;
}

VOID    OPEN32API _O32_SetLastError( DWORD );

inline VOID O32_SetLastError(DWORD a)
{
 USHORT sel = RestoreOS2FS();

    _O32_SetLastError(a);
    SetFS(sel);
}

BOOL    OPEN32API _O32_ExcludeUpdateRgn( HDC, HWND );

inline BOOL O32_ExcludeUpdateRgn(HDC a, HWND b)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExcludeUpdateRgn(a, b);
    SetFS(sel);

    return yyrc;
}

BOOL OPEN32API _O32_CreateCaret (HWND hwnd, HBITMAP hbm, int width, int height);

inline BOOL O32_CreateCaret(HWND a, HBITMAP b, int c, int d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateCaret(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

BOOL OPEN32API _O32_SelectClipRgn(HDC hdc,HRGN hrgn);

inline BOOL O32_SelectClipRgn(HDC hdc,HRGN hrgn)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SelectClipRgn(hdc,hrgn);
    SetFS(sel);

    return yyrc;
}

BOOL    APIENTRY  __DestroyCaret (void);

inline BOOL APIENTRY _DestroyCaret (void)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __DestroyCaret();
    SetFS(sel);

    return yyrc;
}

UINT    APIENTRY  __GetCaretBlinkTime (void);

inline UINT APIENTRY _GetCaretBlinkTime (void)
{
 UINT yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __GetCaretBlinkTime();
    SetFS(sel);

    return yyrc;
}

BOOL    APIENTRY  __HideCaret (HWND hwnd);

inline BOOL APIENTRY _HideCaret (HWND hwnd)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __HideCaret(hwnd);
    SetFS(sel);

    return yyrc;
}

BOOL    APIENTRY  __SetCaretBlinkTime (UINT mSecs);

inline BOOL APIENTRY _SetCaretBlinkTime (UINT mSecs)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __SetCaretBlinkTime(mSecs);
    SetFS(sel);

    return yyrc;
}

BOOL    APIENTRY  __ShowCaret (HWND hwnd);

inline BOOL APIENTRY _ShowCaret (HWND hwnd)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __ShowCaret(hwnd);
    SetFS(sel);

    return yyrc;
}


int       APIENTRY _O32_GetClipRgn( HDC, HRGN );

inline int O32_GetClipRgn(HDC a, HRGN b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetClipRgn(a, b);
    SetFS(sel);

    return yyrc;
} 

int APIENTRY _O32_ExcludeClipRect(HDC a, int b, int c, int d, int e);

inline int O32_ExcludeClipRect(HDC a, int b, int c, int d, int e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExcludeClipRect(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

int APIENTRY _O32_IntersectClipRect(HDC a, int b, int c, int d, int e);

inline int O32_IntersectClipRect(HDC a, int b, int c, int d, int e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_IntersectClipRect(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

int APIENTRY _O32_ExtSelectClipRgn(HDC a, HRGN b, int c);

inline int O32_ExtSelectClipRgn(HDC a, HRGN b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_ExtSelectClipRgn(a, b, c);
    SetFS(sel);

    return yyrc;
} 

int APIENTRY _O32_OffsetClipRgn(HDC a, int b, int c);

inline int O32_OffsetClipRgn(HDC a, int b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_OffsetClipRgn(a, b, c);
    SetFS(sel);

    return yyrc;
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

#endif //INCLUDED_BY_DC

#endif //__DC_H__

