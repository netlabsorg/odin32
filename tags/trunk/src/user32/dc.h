/* $Id: dc.h,v 1.6 1999-10-30 18:40:44 cbratschi Exp $ */
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
extern void releaseOwnDC (HDC hps);


#ifdef INCLUDED_BY_DC
/*********************/
typedef struct
{
    HDC   hdc;
    BOOL  fErase;
    RECT  rcPaint;
    BOOL  fRestore;
    BOOL  IncUpdate;
    BYTE  rgbReserved[32];
} PAINTSTRUCT_W, *PPAINTSTRUCT_W, *LPPAINTSTRUCT_W;

#define PS_SOLID_W         0x00000000
#define PS_DASH_W          0x00000001
#define PS_DOT_W           0x00000002
#define PS_DASHDOT_W       0x00000003
#define PS_DASHDOTDOT_W    0x00000004
#define PS_NULL_W          0x00000005
#define PS_INSIDEFRAME_W   0x00000006
#define PS_USERSTYLE_W     0x00000007
#define PS_ALTERNATE_W     0x00000008
#define PS_STYLE_MASK_W    0x0000000f

typedef struct
{
    UINT   lopnStyle;
    POINT  lopnWidth;
    ULONG  lopnColor;
} LOGPEN_W, *LPLOGPEN_W;

typedef struct tagEXTLOGPEN
{
    DWORD elpPenStyle;
    DWORD elpWidth;
    DWORD elpBrushStyle;
    DWORD elpColor;
    DWORD elpNumEntries;
    DWORD elpStyleEntry[1];
} EXTLOGPEN_W, *PEXTLOGPEN_W, *NPEXTLOGPEN_W, *LPEXTLOGPEN_W;

typedef struct
{
    UINT   lbStyle;
    ULONG  lbColor;
    INT    lbHatch;
} LOGBRUSH_W, *LPLOGBRUSH_W;

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

/* DC Graphics Mode */
#define GM_COMPATIBLE_W     1
#define GM_ADVANCED_W       2

#define DCX_WINDOW_W                    0x00000001L
#define DCX_CACHE_W                     0x00000002L
#define DCX_NORESETATTRS_W              0x00000004L
#define DCX_CLIPCHILDREN_W              0x00000008L
#define DCX_CLIPSIBLINGS_W              0x00000010L
#define DCX_PARENTCLIP_W                0x00000020L
#define DCX_EXCLUDERGN_W                0x00000040L
#define DCX_INTERSECTRGN_W              0x00000080L
#define DCX_EXCLUDEUPDATE_W             0x00000100L
#define DCX_INTERSECTUPDATE_W           0x00000200L
#define DCX_LOCKWINDOWUPDATE_W          0x00000400L
#define DCX_VALIDATE_W                  0x00200000L

#define RDW_INVALIDATE_W       0x0001
#define RDW_INTERNALPAINT_W    0x0002
#define RDW_ERASE_W            0x0004
#define RDW_VALIDATE_W         0x0008
#define RDW_NOINTERNALPAINT_W  0x0010
#define RDW_NOERASE_W          0x0020
#define RDW_NOCHILDREN_W       0x0040
#define RDW_ALLCHILDREN_W      0x0080
#define RDW_UPDATENOW_W        0x0100
#define RDW_ERASENOW_W         0x0200
#define RDW_FRAME_W            0x0400
#define RDW_NOFRAME_W          0x0800

typedef struct _RGNDATAHEADER_W {
    DWORD       dwSize;
    DWORD       iType;
    DWORD       nCount;
    DWORD       nRgnSize;
    RECT        rcBound;
} RGNDATAHEADER_W, *LPRGNDATAHEADER_W;

typedef struct _RGNDATA_W {
    RGNDATAHEADER_W     rdh;
    char                Buffer[1];
} RGNDATA_W , *PRGNDATA_W , *LPRGNDATA_W ;


/* Xform FLAGS */
#define MWT_IDENTITY_W        1
#define MWT_LEFTMULTIPLY_W    2
#define MWT_RIGHTMULTIPLY_W   3

/* Mapping Modes */
#define MM_TEXT_W             1
#define MM_LOMETRIC_W         2
#define MM_HIMETRIC_W         3
#define MM_LOENGLISH_W        4
#define MM_HIENGLISH_W        5
#define MM_TWIPS_W            6
#define MM_ISOTROPIC_W        7
#define MM_ANISOTROPIC_W      8

#define RGN_OR_W              2

/* Window scrolling */
#define SW_SCROLLCHILDREN_W    0x0001
#define SW_INVALIDATE_W        0x0002
#define SW_ERASE_W             0x0004
#define SW_SMOOTHSCROLL_W      0x0010

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

//LONG    APIENTRY GpiQueryYInversion (HPS hps);

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

ULONG   OPEN32API _O32_GetRegionData (HRGN hrgn, ULONG count, PRGNDATA_W pData);

inline DWORD O32_GetRegionData(HRGN a, DWORD b, PRGNDATA_W c)
{
 DWORD yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetRegionData(a, b, c);
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

BOOL    OPEN32API _O32_SetRectRgn (HRGN dest, int left, int top, int right, int bottom);

inline BOOL O32_SetRectRgn(HRGN a, int b, int c, int d, int e)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_SetRectRgn(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

int     OPEN32API _O32_CombineRgn (HRGN dest, HRGN src1, HRGN src2, int mode);

inline int O32_CombineRgn(HRGN a, HRGN b, HRGN c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CombineRgn(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

HRGN    OPEN32API _O32_CreateRectRgn (int left, int top, int right, int bottom);

inline HRGN O32_CreateRectRgn(int a, int b, int c, int d)
{
 HRGN yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateRectRgn(a, b, c, d);
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

#endif //INCLUDED_BY_DC

#endif //__DC_H__

