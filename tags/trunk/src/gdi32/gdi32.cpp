/* $Id: gdi32.cpp,v 1.7 1999-06-21 18:59:21 phaller Exp $ */

/*
 * GDI32 DIB sections
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"
#include "callback.h"
#include "unicode.h"
#include "dibsect.h"


typedef struct _POLYTEXTA
{
    int     x;
    int     y;
    UINT    n;
    LPCSTR lpstr;
    UINT    uiFlags;
    RECT    rcl;
    int     *pdx;
} POLYTEXTA;


typedef struct _POLYTEXTW
{
    int     x;
    int     y;
    UINT    n;
    LPCWSTR lpstr;
    UINT    uiFlags;
    RECT    rcl;
    int     *pdx;
} POLYTEXTW;


static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr)
{
        ULONG cbPalette;

        switch (pBHdr->biBitCount)
        {
                case 1:
                case 4:
                case 8:
                        cbPalette = (1 << pBHdr->biBitCount) * sizeof(RGBQUAD);
                        break;

                case 16:
                case 24:
                case 32:
                        cbPalette = 0;
                        break;

                default:
                        dprintf(("QueryPaletteSize: error pBHdr->biBitCount = %d", pBHdr->biBitCount));
                        cbPalette = -1;
        }

   return cbPalette;
}

static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy)
{
        ULONG alignment;
        ULONG factor;
        BOOL flag = TRUE;       //true: '*'     false: '/'

        cy = cy < 0 ? -cy : cy;

        switch(cBits)
        {
                case 1:
                        factor = 8;
                        flag = FALSE;
                        break;

                case 4:
                        factor = 2;
                        flag = FALSE;
                        break;

                case 8:
                        factor = 1;
                        break;

                case 16:
                        factor = 2;
                        break;

                case 24:
                        factor = 3;
                        break;

                case 32:
                        return cx*cy;

                default:
                        return 0;
        }

        if (flag)
                alignment = (cx = (cx*factor)) % 4;
        else
                alignment = (cx = ((cx+factor-1)/factor)) % 4;

        if (alignment != 0)
                cx += 4 - alignment;

        return cx*cy;
}

//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPointA(HDC hdc, LPCSTR lpsz, int cbString, LPSIZE lpSize)
{
 BOOL rc;

  lpSize->cx = lpSize->cy = 0;
  rc = O32_GetTextExtentPoint(hdc, lpsz, cbString, lpSize);
  dprintf(("GDI32: GetTextExtentPointA of %s returned %d\n", lpsz, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetBkColor(HDC hdc, COLORREF crColor)
{
  dprintf(("GDI32: SetBkColor to %X\n", crColor));
  return(O32_SetBkColor(hdc, crColor));
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetTextColor(HDC hdc, COLORREF crColor)
{
 COLORREF clr;

  clr = O32_SetTextColor(hdc, crColor);
  dprintf(("GDI32: SetTextColor from %X to %X\n", clr, crColor));
  return(clr);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutA(HDC hdc, int nXStart, int nYStart,
                             LPCSTR lpszString, int cbString)
{
 BOOL rc;

  rc = O32_TextOut(hdc, nXStart, nYStart, lpszString, cbString);
  dprintf(("GDI32: TextOut %s returned %d\n", lpszString, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API GetStockObject(int arg1)
{
 HGDIOBJ obj;

    switch(arg1) {
        case DEFAULT_GUI_FONT:
                obj = NULL;
                break;
        default:
                obj = O32_GetStockObject(arg1);
                break;
    }
    dprintf(("GDI32: GetStockObject %d returned %X\n", arg1, obj));
    return(obj);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RealizePalette( HDC arg1)
{
    dprintf(("GDI32: RealizePalette\n"));
    return O32_RealizePalette(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectA( HGDIOBJ arg1, int arg2, void *  arg3)
{
    dprintf(("GDI32: GetObject %X %X %X\n", arg1, arg2, arg3));
    return O32_GetObject(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetObjectType( HGDIOBJ arg1)
{
    dprintf(("GDI32: GetObjectType\n"));
    return O32_GetObjectType(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteObject(HANDLE hObj)
{
    dprintf(("GDI32: DeleteObject\n"));
    DIBSection::deleteSection((DWORD)hObj);
    return O32_DeleteObject(hObj);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteDC( HDC arg1)
{
    dprintf(("GDI32: DeleteDC\n"));
    return O32_DeleteDC(arg1);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreatePalette( const LOGPALETTE * arg1)
{
    dprintf(("GDI32: CreatePalette\n"));
    return O32_CreatePalette(arg1);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreatePatternBrush(HBITMAP arg1)
{
 HBRUSH brush;

    brush = O32_CreatePatternBrush(arg1);
    dprintf(("GDI32: CreatePatternBrush from bitmap %X returned %X\n", arg1, brush));
    return(brush);
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API CreatePen( int arg1, int arg2, COLORREF  arg3)
{
    dprintf(("GDI32: CreatePen\n"));
    return O32_CreatePen(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API CreatePenIndirect( const LOGPEN * arg1)
{
    dprintf(("GDI32: CreatePenIndirect\n"));
    return O32_CreatePenIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolyPolygonRgn( const POINT * arg1, const INT * arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: CreatePolyPolygonRgn\n"));
    return O32_CreatePolyPolygonRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolygonRgn(const POINT * arg1, int arg2, int arg3)
{
    dprintf(("GDI32: CreatePolygonRgn"));
    return O32_CreatePolygonRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateDIBPatternBrushPt( const VOID * arg1, UINT  arg2)
{
    dprintf(("GDI32: CreateDIBPatternBrushPt\n"));
    return O32_CreateDIBPatternBrushPt(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateDIBitmap(HDC arg1, const BITMAPINFOHEADER * arg2, DWORD arg3, const void * arg4, const BITMAPINFO * arg5, UINT  arg6)
{
    dprintf(("GDI32: CreateDIBitmap\n"));
    return O32_CreateDIBitmap(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateCompatibleBitmap( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: CreateCompatibleBitmap\n"));
    return O32_CreateCompatibleBitmap(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateCompatibleDC( HDC arg1)
{
    dprintf(("GDI32: CreateCompatibleDC %X\n", arg1));
    return O32_CreateCompatibleDC(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API StretchDIBits( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, const void * arg10, const BITMAPINFO *arg11, UINT arg12, DWORD arg13)
{
    dprintf(("GDI32: StretchDIBits\n"));
    return O32_StretchDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, (void *)arg10, (PBITMAPINFO)arg11, arg12, arg13);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                            int nWidthDest, int nHeightDest,
                            HDC hdcSrc, int nXOriginSrc, int nYOriginSrc,
                            int nWidthSrc, int nHeightSrc, DWORD dwRop)
{
    dprintf(("GDI32: StretchBlt Dest: (%d, %d) size (%d, %d)\n", nXOriginDest, nYOriginDest, nWidthDest, nHeightDest));
    dprintf(("GDI32: StretchBlt Src : (%d, %d) size (%d, %d)\n", nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc));
    return O32_StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StrokeAndFillPath( HDC arg1)
{
    dprintf(("GDI32: StrokeAndFillPath\n"));
    return O32_StrokeAndFillPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StrokePath( HDC arg1)
{
    dprintf(("GDI32: StrokePath\n"));
    return O32_StrokePath(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetStretchBltMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetStretchBltMode\n"));
    return O32_SetStretchBltMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API SelectObject(HDC hdc, HGDIOBJ hObj)
{
 HGDIOBJ rc;

////    dprintf(("GDI32: SelectObject\n"));

    if(DIBSection::getSection() != NULL) {
        DIBSection *dsect;

        dsect = DIBSection::find(hdc);
        if(dsect) {//remove previously selected dibsection
                dsect->UnSelectDIBObject();
        }
        dsect = DIBSection::find((DWORD)hObj);
        if(dsect) {
                dsect->SelectDIBObject(hdc);
        }
    }
    rc = O32_SelectObject(hdc, hObj);
    if(rc != 0 && DIBSection::getSection != NULL) {
        DIBSection *dsect = DIBSection::find((DWORD)rc);
        if(dsect) {
                dsect->UnSelectDIBObject();
        }
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API SelectPalette(HDC arg1, HPALETTE arg2, BOOL arg3)
{
    dprintf(("GDI32: SelectPalette\n"));
    return O32_SelectPalette(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetBkMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetBkMode\n"));
    return O32_SetBkMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutA(HDC hdc, int X, int Y, UINT fuOptions, CONST RECT *lprc,
                          LPCSTR lpszString, UINT cbCount, CONST INT *lpDx)
{
  if(lpszString && strlen(lpszString) > cbCount)
        ((LPSTR)lpszString)[cbCount] = 0;
  dprintf(("GDI32: ExtTextOutA %s\n", lpszString));
  return(O32_ExtTextOut(hdc, X, Y, fuOptions, lprc, lpszString, cbCount, lpDx));
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetPixel( HDC arg1, int arg2, int  arg3)
{
////    dprintf(("GDI32: GetPixel\n"));
    return O32_GetPixel(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetPixel( HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
////    dprintf(("GDI32: SetPixel\n"));
    return O32_SetPixel(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//Faster version of SetPixel (since it doesn't need to return the original color)
//Just use SetPixel for now
//******************************************************************************
BOOL WIN32API SetPixelV(HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
 COLORREF rc;

////    dprintf(("GDI32: SetPixelV\n"));
    rc = O32_SetPixel(arg1, arg2, arg3, arg4);
    if(rc == GDI_ERROR) // || rc == COLOR_INVALID)
        return(FALSE);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontA(int arg1, int arg2, int arg3, int arg4, int arg5,
                               DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9,
                               DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13, LPCSTR arg14)
{
 HFONT hfont;

    hfont = O32_CreateFont(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
    dprintf(("GDI32: CreateFontA '%s' returned %D\n", arg14, hfont));
    return(hfont);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDCOrgEx(HDC arg1, PPOINT arg2)
{
    dprintf(("GDI32: GetDCOrgEx\n"));
    return O32_GetDCOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowExtEx(HDC arg1, PSIZE arg2)
{
    dprintf(("GDI32: GetWindowExtEx\n"));
    return O32_GetWindowExtEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API AbortDoc( HDC arg1)
{
    dprintf(("GDI32: OS2AbortDoc"));
    return O32_AbortDoc(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AbortPath( HDC arg1)
{
    dprintf(("GDI32: OS2AbortPath"));
    return O32_AbortPath(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2AddFontResourceA"));
    return O32_AddFontResource(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceW( LPCWSTR arg1)
{
    dprintf(("GDI32: OS2AddFontResourceW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_AddFontResource(arg1);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AngleArc( HDC arg1, int arg2, int arg3, DWORD arg4, float  arg5, float  arg6)
{
    dprintf(("GDI32: OS2AngleArc"));
    return O32_AngleArc(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AnimatePalette( HPALETTE arg1, UINT arg2, UINT arg3, const PALETTEENTRY * arg4)
{
    dprintf(("GDI32: OS2AnimatePalette"));
    return O32_AnimatePalette(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Arc( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int  arg9)
{
    dprintf(("GDI32: OS2Arc"));
    return O32_Arc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ArcTo( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    dprintf(("GDI32: OS2ArcTo"));
    return O32_ArcTo(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BeginPath( HDC arg1)
{
    dprintf(("GDI32: OS2BeginPath"));
    return O32_BeginPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BitBlt(HDC hdcDest, int arg2, int arg3, int arg4, int arg5, HDC hdcSrc, int arg7, int arg8, DWORD  arg9)
{
    if(DIBSection::getSection() != NULL) {
        DIBSection *dsect = DIBSection::findHDC(hdcSrc);
        if(dsect) {
                return(dsect->BitBlt(hdcDest, O32_WindowFromDC(hdcDest), arg2, arg3, arg4,
                                     arg5, arg7, arg8, arg9));
        }
    }
    dprintf(("GDI32: OS2BitBlt to hdc %X from (%d,%d) to (%d,%d), (%d,%d) rop %X\n", hdcDest, arg7, arg8, arg2, arg3, arg4, arg5, arg9));
    return O32_BitBlt(hdcDest, arg2, arg3, arg4, arg5, hdcSrc, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Chord( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    dprintf(("GDI32: OS2Chord"));
    return O32_Chord(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CloseEnhMetaFile( HDC arg1)
{
    dprintf(("GDI32: OS2CloseEnhMetaFile"));
    return O32_CloseEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseFigure( HDC arg1)
{
    dprintf(("GDI32: OS2CloseFigure"));
    return O32_CloseFigure(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CloseMetaFile( HDC arg1)
{
    dprintf(("GDI32: OS2CloseMetaFile"));
    return O32_CloseMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API CombineRgn( HRGN arg1, HRGN  arg2, HRGN  arg3, int  arg4)
{
    dprintf(("GDI32: OS2CombineRgn"));
    return O32_CombineRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileA( HENHMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: OS2CopyEnhMetaFileA"));
    return O32_CopyEnhMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileW( HENHMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HENHMETAFILE rc;

    dprintf(("GDI32: OS2CopyEnhMetaFileW"));
    rc = O32_CopyEnhMetaFile(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileA( HMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: OS2CopyMetaFileA"));
    return O32_CopyMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileW( HMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HMETAFILE rc;

    dprintf(("GDI32: OS2CopyMetaFileW"));
    rc = O32_CopyMetaFile(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmap(int nWidth, int nHeight, UINT cPlanes,
                                 UINT cBitsPerPel, const void *lpvBits)
{
 HBITMAP rc;

    rc = O32_CreateBitmap(nWidth, nHeight, cPlanes, cBitsPerPel, lpvBits);
    dprintf(("GDI32: OS2CreateBitmap (%d,%d) bps %d returned %d\n", nWidth, nHeight, cBitsPerPel, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmapIndirect( const BITMAP * arg1)
{
    dprintf(("GDI32: OS2CreateBitmapIndirect"));
    return O32_CreateBitmapIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateBrushIndirect( const LOGBRUSH * arg1)
{
    dprintf(("GDI32: OS2CreateBrushIndirect"));
    return O32_CreateBrushIndirect((LPLOGBRUSH)arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, const DEVMODEA * arg4)
{
    dprintf(("GDI32: OS2CreateDCA"));
    return O32_CreateDC(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    dprintf(("GDI32: OS2CreateDCW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_CreateDC(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgn( int arg1, int arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: OS2CreateEllipticRgn"));
    return O32_CreateEllipticRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgnIndirect( const RECT * arg1)
{
    dprintf(("GDI32: OS2CreateEllipticRgnIndirect"));
    return O32_CreateEllipticRgnIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileA( HDC arg1, LPCSTR arg2, const RECT * arg3, LPCSTR  arg4)
{
    dprintf(("GDI32: OS2CreateEnhMetaFileA"));
    return O32_CreateEnhMetaFile(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileW( HDC arg1, LPCWSTR arg2, const RECT * arg3, LPCWSTR  arg4)
{
    dprintf(("GDI32: OS2CreateEnhMetaFileW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_CreateEnhMetaFile(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontIndirectA(const LOGFONTA *lplf)
{
 HFONT rc;

    dprintf(("GDI32: CreateFontIndirectA\n"));
    dprintf(("GDI32: lfHeight        = %d\n", lplf->lfHeight));
    dprintf(("GDI32: lfWidth          = %d\n", lplf->lfWidth));
    dprintf(("GDI32: lfEscapement    = %d\n", lplf->lfEscapement));
    dprintf(("GDI32: lfOrientation   = %d\n", lplf->lfOrientation));
    dprintf(("GDI32: lfWeight        = %d\n", lplf->lfWeight));
    dprintf(("GDI32: lfItalic        = %d\n", lplf->lfItalic));
    dprintf(("GDI32: lfUnderline     = %d\n", lplf->lfUnderline));
    dprintf(("GDI32: lfStrikeOut     = %d\n", lplf->lfStrikeOut));
    dprintf(("GDI32: lfCharSet       = %X\n", lplf->lfCharSet));
    dprintf(("GDI32: lfOutPrecision  = %X\n", lplf->lfOutPrecision));
    dprintf(("GDI32: lfClipPrecision = %X\n", lplf->lfClipPrecision));
    dprintf(("GDI32: lfQuality       = %X\n", lplf->lfQuality));
    dprintf(("GDI32: lfPitchAndFamily= %X\n", lplf->lfPitchAndFamily));
    dprintf(("GDI32: lfFaceName      = %s\n", lplf->lfFaceName));
    rc = O32_CreateFontIndirect(lplf);
    dprintf(("GDI32: OS2CreateFontIndirectA returned %X\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontIndirectW(const LOGFONTW *lplf)
{
 LOGFONTA afont;
 HFONT    hfont;

    memcpy(&afont, lplf, ((int)&afont.lfFaceName - (int)&afont));
    UnicodeToAscii((WCHAR *)lplf->lfFaceName, afont.lfFaceName);

    hfont = O32_CreateFontIndirect(&afont);
    dprintf(("GDI32: CreateFontIndirectW\n"));
    dprintf(("GDI32: lfHeight        = %d\n", lplf->lfHeight));
    dprintf(("GDI32: lfWidth          = %d\n", lplf->lfWidth));
    dprintf(("GDI32: lfHeight        = %d\n", afont.lfHeight));
    dprintf(("GDI32: lfWidth          = %d\n", afont.lfWidth));
    dprintf(("GDI32: lfEscapement    = %d\n", afont.lfEscapement));
    dprintf(("GDI32: lfOrientation   = %d\n", afont.lfOrientation));
    dprintf(("GDI32: lfWeight        = %d\n", afont.lfWeight));
    dprintf(("GDI32: lfItalic        = %d\n", afont.lfItalic));
    dprintf(("GDI32: lfUnderline     = %d\n", afont.lfUnderline));
    dprintf(("GDI32: lfStrikeOut     = %d\n", afont.lfStrikeOut));
    dprintf(("GDI32: lfCharSet       = %X\n", afont.lfCharSet));
    dprintf(("GDI32: lfOutPrecision  = %X\n", afont.lfOutPrecision));
    dprintf(("GDI32: lfClipPrecision = %X\n", afont.lfClipPrecision));
    dprintf(("GDI32: lfQuality       = %X\n", afont.lfQuality));
    dprintf(("GDI32: lfPitchAndFamily= %X\n", afont.lfPitchAndFamily));
    dprintf(("GDI32: lfFaceName      = %s\n", afont.lfFaceName));
    dprintf(("GDI32: CreateFontIndirectW %s returned %X\n", afont.lfFaceName, hfont));

    return(hfont);
}
//******************************************************************************
//******************************************************************************
HFONT WIN32API CreateFontW(int arg1, int arg2, int arg3, int arg4, int arg5,
                              DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9,
                              DWORD arg10, DWORD arg11, DWORD arg12, DWORD arg13, LPCWSTR arg14)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg14);
 HFONT rc;

   dprintf(("GDI32: OS2CreateFontW\n"));
   rc = O32_CreateFont(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, astring);
   FreeAsciiString(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateHatchBrush( int arg1, COLORREF  arg2)
{
    dprintf(("GDI32: OS2CreateHatchBrush"));
    return O32_CreateHatchBrush(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, const DEVMODEA * arg4)
{
    dprintf(("GDI32: OS2CreateICA"));
    return O32_CreateIC(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    dprintf(("GDI32: OS2CreateICW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_CreateIC(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2CreateMetaFileA"));
    return O32_CreateMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HDC   rc;

    dprintf(("GDI32: OS2CreateMetaFileW"));
    rc = O32_CreateMetaFile(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgn( int arg1, int arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: OS2CreateRectRgn"));
    return O32_CreateRectRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgnIndirect( const RECT * arg1)
{
    dprintf(("GDI32: OS2CreateRectRgnIndirect"));
    return O32_CreateRectRgnIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRoundRectRgn( int arg1, int arg2, int arg3, int arg4, int arg5, int  arg6)
{
    dprintf(("GDI32: OS2CreateRoundRectRgn"));
    return O32_CreateRoundRectRgn(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateSolidBrush( COLORREF arg1)
{
    dprintf(("GDI32: OS2CreateSolidBrush\n"));
    return O32_CreateSolidBrush(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DPtoLP( HDC arg1, PPOINT arg2, int  arg3)
{
    dprintf(("GDI32: OS2DPtoLP\n"));
    return O32_DPtoLP(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteEnhMetaFile( HENHMETAFILE arg1)
{
    dprintf(("GDI32: OS2DeleteEnhMetaFile\n"));
    return O32_DeleteEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteMetaFile( HMETAFILE arg1)
{
    dprintf(("GDI32: OS2DeleteMetaFile"));
    return O32_DeleteMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Ellipse( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: OS2Ellipse"));
    return O32_Ellipse(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API EndDoc( HDC arg1)
{
    dprintf(("GDI32: OS2EndDoc"));
    return O32_EndDoc(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API EndPage( HDC arg1)
{
    dprintf(("GDI32: OS2EndPage"));
    return O32_EndPage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPath( HDC arg1)
{
    dprintf(("GDI32: OS2EndPath"));
    return O32_EndPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumEnhMetaFile( HDC arg1, HENHMETAFILE arg2, ENHMFENUMPROC arg3, PVOID arg4, const RECT *  arg5)
{
    dprintf(("GDI32: OS2EnumEnhMetaFile DOESN'T WORK!"));
//    return O32_EnumEnhMetaFile(arg1, arg2, arg3, arg4, arg5);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesA(HDC          arg1,
                                  LPCSTR       arg2,
                                  FONTENUMPROCA arg3,
                                  LPARAM       arg4)
{
    dprintf(("GDI32: OS2EnumFontFamiliesA DOESN'T WORK!"));
//    return O32_EnumFontFamilies(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumFontFamiliesW(HDC          arg1,
                               LPCWSTR       arg2,
                               FONTENUMPROCW arg3,
                               LPARAM       arg4)
{
  dprintf(("GDI32: OS2EnumFontFamiliesW not implemented\n"));

  // NOTE: This will not work as is (needs UNICODE support)

  /* @@@PH 98/06/07 EnumFontFamilies will crash upon return from a
                    wide-character FONTENUMPROC. */
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LineTo( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2LineTo"));
    return O32_LineTo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MoveToEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OS2MoveToEx\n"));
    return O32_MoveToEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PatBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, DWORD  arg6)
{
 BOOL rc;

  rc = O32_PatBlt(arg1, arg2, arg3, arg4, arg5, arg6);
  dprintf(("GDI32: OS2PatBlt (%d,%d) (%d,%d) returned %d\n", arg2, arg3, arg4, arg5, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Rectangle( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: OS2Rectangle\n"));
    return O32_Rectangle(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetROP2( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetROP2"));
    return O32_SetROP2(arg1, arg2);
}
//******************************************************************************
//TODO: Callback
//******************************************************************************
int WIN32API EnumFontsA( HDC arg1, LPCSTR arg2, FONTENUMPROCA arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumFontsA"));
//    return O32_EnumFonts(arg1, arg2, arg3, arg4);
    return 1;
}
//******************************************************************************
//TODO: Callback
//******************************************************************************
int WIN32API EnumFontsW( HDC arg1, LPCWSTR arg2, FONTENUMPROCW arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumFontsW - stub (1)"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_EnumFonts(arg1, arg2, arg3, arg4);
    return 1;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumMetaFile( HDC arg1, HMETAFILE arg2, MFENUMPROC arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumMetaFile STUB"));
    //calling convention differences
//    return O32_EnumMetaFile(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumObjects( HDC arg1, int arg2, GOBJENUMPROC arg3, LPARAM  arg4)
{
    dprintf(("GDI32: OS2EnumObjects STUB"));
    //calling convention differences
//    return O32_EnumObjects(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRgn( HRGN arg1, HRGN  arg2)
{
    dprintf(("GDI32: OS2EqualRgn"));
    return O32_EqualRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API Escape( HDC arg1, int arg2, int arg3, LPCSTR arg4, PVOID  arg5)
{
    dprintf(("GDI32: OS2Escape"));
    return O32_Escape(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExcludeClipRect( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: OS2ExcludeClipRect"));
    return O32_ExcludeClipRect(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API ExtCreatePen( DWORD arg1, DWORD arg2, const LOGBRUSH * arg3, DWORD arg4, const DWORD *  arg5)
{
    dprintf(("GDI32: OS2ExtCreatePen"));
    return O32_ExtCreatePen(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API ExtCreateRegion( const XFORM * arg1, DWORD arg2, const RGNDATA *  arg3)
{
    dprintf(("GDI32: OS2ExtCreateRegion"));
    return O32_ExtCreateRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtFloodFill( HDC arg1, int arg2, int arg3, COLORREF arg4, UINT  arg5)
{
    dprintf(("GDI32: OS2ExtFloodFill"));
    return O32_ExtFloodFill(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExtSelectClipRgn( HDC arg1, HRGN arg2, int  arg3)
{
    dprintf(("GDI32: OS2ExtSelectClipRgn"));
    return O32_ExtSelectClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutW(HDC hdc, int X, int Y, UINT fuOptions,
                          const RECT *lprc, LPCWSTR lpString, UINT cbCount,
                          const int *lpDx)
{
 char *astring = UnicodeToAsciiString((LPWSTR)lpString);
 BOOL  rc;

    if(lprc)
            dprintf(("GDI32: OS2ExtTextOutW (%d,%d) %X, (%d,%d)(%d,%d)\n", X, Y, fuOptions, lprc->left, lprc->top, lprc->right, lprc->bottom));
    else    dprintf(("GDI32: OS2ExtTextOutW (%d,%d) %X\n", X, Y, fuOptions));
    rc = O32_ExtTextOut(hdc, X, Y, fuOptions, lprc, astring, cbCount, lpDx);
    dprintf(("GDI32: OS2ExtTextOutW %s (%X) length %d rc %d\n", astring, lpString, cbCount, rc));
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillPath( HDC arg1)
{
    dprintf(("GDI32: OS2FillPath"));
    return O32_FillPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillRgn( HDC arg1, HRGN arg2, HBRUSH  arg3)
{
    dprintf(("GDI32: OS2FillRgn"));
    return O32_FillRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlattenPath( HDC arg1)
{
    dprintf(("GDI32: OS2FlattenPath"));
    return O32_FlattenPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FloodFill(HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
    dprintf(("GDI32: OS2FloodFill"));
    return O32_FloodFill(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FrameRgn( HDC arg1, HRGN arg2, HBRUSH arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: OS2FrameRgn"));
    return O32_FrameRgn(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetArcDirection( HDC arg1)
{
    dprintf(("GDI32: OS2GetArcDirection"));
    return O32_GetArcDirection(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetAspectRatioFilterEx( HDC arg1, PSIZE  arg2)
{
    dprintf(("GDI32: OS2GetAspectRatioFilterEx"));
    return O32_GetAspectRatioFilterEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetBitmapBits( HBITMAP arg1, LONG arg2, PVOID  arg3)
{
    dprintf(("GDI32: OS2GetBitmapBits"));
    return O32_GetBitmapBits(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBitmapDimensionEx( HBITMAP arg1, PSIZE  arg2)
{
    dprintf(("GDI32: OS2GetBitmapDimensionEx"));
    return O32_GetBitmapDimensionEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetBkColor( HDC arg1)
{
    dprintf(("GDI32: OS2GetBkColor"));
    return O32_GetBkColor(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetBkMode( HDC arg1)
{
    dprintf(("GDI32: OS2GetBkMode"));
    return O32_GetBkMode(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetBoundsRect( HDC arg1, PRECT arg2, UINT  arg3)
{
    dprintf(("GDI32: OS2GetBoundsRect"));
    return O32_GetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBrushOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: OS2GetBrushOrgEx"));
    return O32_GetBrushOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsA( HDC arg1, UINT arg2, UINT arg3, LPABC arg4)
{
    dprintf(("GDI32: OS2GetCharABCWidthsA"));
    return O32_GetCharABCWidths(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsW( HDC arg1, UINT arg2, UINT arg3, LPABC arg4)
{
    dprintf(("GDI32: OS2GetCharABCWidthsW not properly implemented."));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetCharABCWidths(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthA( HDC arg1, UINT arg2, UINT arg3, PINT  arg4)
{
    dprintf(("GDI32: OS2GetCharWidthA"));
    return O32_GetCharWidth(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//TODO: Cut off Unicode chars?
//******************************************************************************
BOOL WIN32API GetCharWidthW(HDC arg1, UINT iFirstChar, UINT iLastChar, PINT  arg4)
{
    dprintf(("GDI32: OS2GetCharWidthW, not properly implemented"));
    return O32_GetCharWidth(arg1, iFirstChar, iLastChar, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipBox( HDC arg1, PRECT  arg2)
{
 int rc;

  rc = O32_GetClipBox(arg1, arg2);
  dprintf(("GDI32: OS2GetClipBox of %X returned %d\n", arg1, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: OS2GetClipRgn"));
    return O32_GetClipRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentObject( HDC arg1, UINT arg2)
{
    dprintf(("GDI32: OS2GetCurrentObject"));
    return (HANDLE)O32_GetCurrentObject(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCurrentPositionEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: OS2GetCurrentPositionEx"));
    return O32_GetCurrentPositionEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDIBits( HDC arg1, HBITMAP arg2, UINT arg3, UINT arg4, void * arg5, PBITMAPINFO arg6, UINT  arg7)
{
    dprintf(("GDI32: OS2GetDIBits"));
    return O32_GetDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDeviceCaps(HDC hdc, int nIndex)
{
 int rc;

    rc = O32_GetDeviceCaps(hdc, nIndex);
    dprintf(("GDI32: OS2GetDeviceCaps %X, %d returned %d\n", hdc, nIndex, rc));
    //SvL: 13-9-'98: NT returns -1 when using 16 bits colors, NOT 65536!
    if(nIndex == NUMCOLORS && rc > 256)
        return -1;
    return(rc);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2GetEnhMetaFileA"));
    return O32_GetEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE  arg3)
{
    dprintf(("GDI32: OS2GetEnhMetaFileBits"));
    return O32_GetEnhMetaFileBits(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileHeader( HENHMETAFILE arg1, UINT arg2, LPENHMETAHEADER  arg3)
{
    dprintf(("GDI32: OS2GetEnhMetaFileHeader"));
    return O32_GetEnhMetaFileHeader(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFilePaletteEntries( HENHMETAFILE arg1, UINT arg2, PPALETTEENTRY  arg3)
{
    dprintf(("GDI32: OS2GetEnhMetaFilePaletteEntries"));
    return O32_GetEnhMetaFilePaletteEntries(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: OS2GetEnhMetaFileW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_GetEnhMetaFile(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetGraphicsMode(HDC arg1)
{
    dprintf(("GDI32: OS2GetGraphicsMode"));
    return O32_GetGraphicsMode(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetKerningPairsA( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    dprintf(("GDI32: OS2GetKerningPairsA"));
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetKerningPairsW( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    dprintf(("GDI32: OS2GetKerningPairsW"));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMapMode( HDC arg1)
{
    dprintf(("GDI32: OS2GetMapMode"));
    return O32_GetMapMode(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2GetMetaFileA"));
    return O32_GetMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMetaFileBitsEx( HMETAFILE arg1, UINT arg2, LPVOID  arg3)
{
    dprintf(("GDI32: OS2GetMetaFileBitsEx"));
    return O32_GetMetaFileBitsEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: OS2GetMetaFileW"));
    rc = O32_GetMetaFile(astring);
    FreeAsciiString(astring);
    return rc;

}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMiterLimit( HDC arg1, float * arg2)
{
    dprintf(("GDI32: OS2GetMiterLimit"));
    return O32_GetMiterLimit(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetNearestColor( HDC arg1, COLORREF  arg2)
{
    dprintf(("GDI32: OS2GetNearestColor\n"));
    return O32_GetNearestColor(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetNearestPaletteIndex( HPALETTE arg1, COLORREF  arg2)
{
    dprintf(("GDI32: OS2GetNearestPaletteIndex\n"));
    return O32_GetNearestPaletteIndex(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectW( HGDIOBJ arg1, int arg2, void *  arg3)
{
    dprintf(("GDI32: OS2GetObjectW %X, %d %X\n", arg1, arg2, arg3));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetObject(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsA( HDC arg1, UINT arg2, LPOUTLINETEXTMETRICA  arg3)
{
    dprintf(("GDI32: OS2GetOutlineTextMetricsA"));
    return O32_GetOutlineTextMetrics(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsW( HDC arg1, UINT arg2, LPOUTLINETEXTMETRICW  arg3)
{
    dprintf(("GDI32: OS2GetOutlineTextMetricsW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_GetOutlineTextMetrics(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetPaletteEntries( HPALETTE arg1, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
    dprintf(("GDI32: OS2GetPaletteEntries"));
    return O32_GetPaletteEntries(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetPath( HDC arg1, PPOINT arg2, PBYTE arg3, int  arg4)
{
    dprintf(("GDI32: OS2GetPath"));
    return O32_GetPath(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPolyFillMode( HDC arg1)
{
    dprintf(("GDI32: OS2GetPolyFillMode"));
    return O32_GetPolyFillMode(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetROP2( HDC arg1)
{
    dprintf(("GDI32: OS2GetROP2"));
    return O32_GetROP2(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetRasterizerCaps(LPRASTERIZER_STATUS arg1, UINT  arg2)
{
    dprintf(("GDI32: OS2GetRasterizerCaps"));
    return O32_GetRasterizerCaps(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetRegionData( HRGN arg1, DWORD arg2, PRGNDATA  arg3)
{
    dprintf(("GDI32: OS2GetRegionData"));
    return O32_GetRegionData(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetRgnBox( HRGN arg1, PRECT  arg2)
{
    dprintf(("GDI32: OS2GetRgnBox"));
    return O32_GetRgnBox(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetStretchBltMode( HDC arg1)
{
    dprintf(("GDI32: OS2GetStretchBltMode"));
    return O32_GetStretchBltMode(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemPaletteEntries( HDC arg1, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
 UINT rc;

    dprintf(("GDI32: OS2GetSystemPaletteEntries start %d nr %d pal ptr %X", arg2, arg3, arg4));
    rc = O32_GetSystemPaletteEntries(arg1, arg2, arg3, arg4);
    dprintf((" GetSystemPaletteEntries returned %d", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTextAlign( HDC arg1)
{
    dprintf(("GDI32: OS2GetTextAlign"));
    return O32_GetTextAlign(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextCharacterExtra( HDC arg1)
{
    dprintf(("GDI32: OS2GetTextCharacterExtra"));
    return O32_GetTextCharacterExtra(arg1);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetTextColor( HDC arg1)
{
    dprintf(("GDI32: OS2GetTextColor"));
    return O32_GetTextColor(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32A( HDC arg1, LPCSTR arg2, int arg3, PSIZE  lpSize)
{
    dprintf(("GDI32: OS2GetTextExtentPoint32A"));
    lpSize->cx = lpSize->cy = 0;
    return O32_GetTextExtentPoint32(arg1, arg2, arg3, lpSize);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32W(HDC arg1, LPCWSTR arg2, int arg3, PSIZE lpSize)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 BOOL  rc;

    dprintf(("GDI32: OS2GetTextExtentPoint32W %s\n", astring));
    lpSize->cx = lpSize->cy = 0;
    rc = O32_GetTextExtentPoint32(arg1, astring, arg3, lpSize);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPointW(HDC    hdc,
                                  LPCWSTR lpString,
                                  int    cbString,
                                  PSIZE  lpSize)
{
  char *astring = UnicodeToAsciiString((LPWSTR)lpString);
  BOOL  rc;

  lpSize->cx = lpSize->cy = 0;
  rc = O32_GetTextExtentPoint(hdc,
                         astring,
                         cbString,
                         lpSize);
  dprintf(("GDI32: OS2GetTextExtentPointW %X %s (size %08xh) returned %d\n", hdc, astring, cbString, rc));
  dprintf(("GDI32: OS2GetTextExtentPointW (%d,%d)\n", lpSize->cx, lpSize->cy));

  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceA( HDC arg1, int arg2, LPSTR  arg3)
{
    dprintf(("GDI32: OS2GetTextFaceA"));
    return O32_GetTextFace(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceW( HDC arg1, int arg2, LPWSTR  arg3)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg3);
 int   rc;

    dprintf(("GDI32: OS2GetTextFaceW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_GetTextFace(arg1, arg2, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsA( HDC arg1, LPTEXTMETRICA  arg2)
{
 BOOL rc;

    rc = O32_GetTextMetrics(arg1, arg2);
    dprintf(("GDI32: OS2GetTextMetricsA returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsW( HDC arg1, LPTEXTMETRICW pwtm)
{
 BOOL rc;
 TEXTMETRICA atm;

    dprintf(("GDI32: OS2GetTextMetricsW"));

    rc = O32_GetTextMetrics(arg1, &atm);
    pwtm->tmHeight = atm.tmHeight;
    pwtm->tmAscent = atm.tmAscent;
    pwtm->tmDescent = atm.tmDescent;
    pwtm->tmInternalLeading = atm.tmInternalLeading;
    pwtm->tmExternalLeading = atm.tmExternalLeading;
    pwtm->tmAveCharWidth = atm.tmAveCharWidth;
    pwtm->tmMaxCharWidth = atm.tmMaxCharWidth;
    pwtm->tmWeight = atm.tmWeight;
    pwtm->tmOverhang = atm.tmOverhang;
    pwtm->tmDigitizedAspectX = atm.tmDigitizedAspectX;
    pwtm->tmDigitizedAspectY = atm.tmDigitizedAspectY;
    pwtm->tmFirstChar = atm.tmFirstChar;
    pwtm->tmLastChar = atm.tmLastChar;
    pwtm->tmDefaultChar = atm.tmDefaultChar;
    pwtm->tmBreakChar = atm.tmBreakChar;
    pwtm->tmItalic = atm.tmItalic;
    pwtm->tmUnderlined = atm.tmUnderlined;
    pwtm->tmStruckOut = atm.tmStruckOut;
    pwtm->tmPitchAndFamily = atm.tmPitchAndFamily;
    pwtm->tmCharSet = atm.tmCharSet;
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetViewportExtEx( HDC arg1, PSIZE  arg2)
{
    dprintf(("GDI32: OS2GetViewportExtEx"));
    return O32_GetViewportExtEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetViewportOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: OS2GetViewportOrgEx"));
    return O32_GetViewportOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetWinMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE arg3, int arg4, HDC arg5)
{
    dprintf(("GDI32: OS2GetWinMetaFileBits"));
    return O32_GetWinMetaFileBits(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: OS2GetWindowOrgEx"));
    return O32_GetWindowOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWorldTransform( HDC arg1, LPXFORM  arg2)
{
    dprintf(("GDI32: OS2GetWorldTransform"));
    return O32_GetWorldTransform(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API IntersectClipRect(HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
 int rc;

  rc = O32_IntersectClipRect(arg1, arg2, arg3, arg4, arg5);
  dprintf(("GDI32: OS2IntersectClipRect returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: OS2InvertRgn"));
    return O32_InvertRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LPtoDP( HDC arg1, PPOINT arg2, int  arg3)
{
    dprintf(("GDI32: OS2LPtoDP"));
    return O32_LPtoDP(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LineDDA( int arg1, int arg2, int arg3, int arg4, LINEDDAPROC lpLineFunc, LPARAM lpData)
{
 BOOL                 rc;
 LineDDAProcCallback *callback = new LineDDAProcCallback(lpLineFunc, lpData);

  dprintf(("GDI32: OS2LineDDA\n"));
  rc = O32_LineDDA(arg1, arg2, arg3, arg4, callback->GetOS2Callback(), (LPARAM)callback);
  if(callback)
        delete callback;
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MaskBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, HDC   arg6, int arg7, int arg8, HBITMAP arg9, int arg10, int arg11, DWORD  arg12)
{
    dprintf(("GDI32: OS2MaskBlt"));
    return O32_MaskBlt(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyWorldTransform( HDC arg1, const XFORM *arg2, DWORD  arg3)
{
    dprintf(("GDI32: OS2ModifyWorldTransform"));
    return O32_ModifyWorldTransform(arg1, (LPXFORM)arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetClipRgn( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2OffsetClipRgn"));
    return O32_OffsetClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetRgn( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2OffsetRgn"));
    return O32_OffsetRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetViewportOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OS2OffsetViewportOrgEx"));
    return O32_OffsetViewportOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetWindowOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OS2OffsetWindowOrgEx"));
    return O32_OffsetWindowOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PaintRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: OS2PaintRgn"));
    return O32_PaintRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API PathToRegion( HDC arg1)
{
    dprintf(("GDI32: OS2PathToRegion"));
    return O32_PathToRegion(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Pie( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int  arg9)
{
    dprintf(("GDI32: OS2Pie"));
    return O32_Pie(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayEnhMetaFile( HDC arg1, HENHMETAFILE arg2, const RECT *  arg3)
{
    dprintf(("GDI32: OS2PlayEnhMetaFile"));
    return O32_PlayEnhMetaFile(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFile( HDC arg1, HMETAFILE  arg2)
{
    dprintf(("GDI32: OS2PlayMetaFile"));
    return O32_PlayMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFileRecord( HDC arg1, LPHANDLETABLE arg2, LPMETARECORD arg3, UINT  arg4)
{
    dprintf(("GDI32: OS2PlayMetaFileRecord"));
    return O32_PlayMetaFileRecord(arg1, arg2, arg3, (int)arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezier( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    dprintf(("GDI32: OS2PolyBezier"));
    return O32_PolyBezier(arg1, arg2, (int)arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezierTo( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    dprintf(("GDI32: OS2PolyBezierTo"));
    return O32_PolyBezierTo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyDraw( HDC arg1, const POINT * arg2, const BYTE * arg3, DWORD  arg4)
{
    dprintf(("GDI32: OS2PolyDraw"));
    return O32_PolyDraw(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolygon( HDC arg1, const POINT * arg2, const INT * arg3, UINT  arg4)
{
    dprintf(("GDI32: OS2PolyPolygon"));
    return O32_PolyPolygon(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolyline( HDC arg1, const POINT * arg2, const DWORD * arg3, DWORD  arg4)
{
    dprintf(("GDI32: OS2PolyPolyline"));
    return O32_PolyPolyline(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polygon( HDC arg1, const POINT * arg2, int  arg3)
{
    dprintf(("GDI32: OS2Polygon"));
    return O32_Polygon(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polyline( HDC arg1, const POINT * arg2, int  arg3)
{
    dprintf(("GDI32: OS2Polyline"));
    return O32_Polyline(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolylineTo( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    dprintf(("GDI32: OS2PolylineTo"));
    return O32_PolylineTo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRegion( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2PtInRegion"));
    return O32_PtInRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtVisible( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2PtVisible"));
    return O32_PtVisible(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectInRegion( HRGN arg1, const RECT *  arg2)
{
    dprintf(("GDI32: OS2RectInRegion"));
    return O32_RectInRegion(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectVisible( HDC arg1, const RECT * arg2)
{
    dprintf(("GDI32: OS2RectVisible\n"));
    return O32_RectVisible(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceA( LPCSTR arg1)
{
    dprintf(("GDI32: OS2RemoveFontResourceA %s\n", arg1));
    return O32_RemoveFontResource(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceW(LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 BOOL  rc;

    dprintf(("GDI32: OS2RemoveFontResourceW\n"));
    rc = O32_RemoveFontResource(astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCA( HDC arg1, const DEVMODEA *  arg2)
{
    dprintf(("GDI32: OS2ResetDCA\n"));
    return (HDC)O32_ResetDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCW( HDC arg1, const DEVMODEW *  arg2)
{
    dprintf(("GDI32: OS2ResetDCW\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return (HDC)O32_ResetDC(arg1, (const DEVMODEA *)arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ResizePalette( HPALETTE arg1, UINT  arg2)
{
    dprintf(("GDI32: OS2ResizePalette\n"));
    return O32_ResizePalette(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RestoreDC( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2RestoreDC\n"));
    return O32_RestoreDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RoundRect( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int  arg7)
{
    dprintf(("GDI32: OS2RoundRect"));
    return O32_RoundRect(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
int WIN32API SaveDC( HDC arg1)
{
    dprintf(("GDI32: OS2SaveDC"));
    return O32_SaveDC(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleViewportExtEx( HDC arg1, int arg2, int arg3, int arg4, int arg5, PSIZE  arg6)
{
    dprintf(("GDI32: OS2ScaleViewportExtEx"));
    return O32_ScaleViewportExtEx(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleWindowExtEx( HDC arg1, int arg2, int arg3, int arg4, int arg5, PSIZE  arg6)
{
    dprintf(("GDI32: OS2ScaleWindowExtEx"));
    return O32_ScaleWindowExtEx(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
int WIN32API SelectClipRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: OS2SelectClipRgn"));
    return O32_SelectClipRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetArcDirection( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetArcDirection"));
    return O32_SetArcDirection(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetBitmapBits( HBITMAP arg1, LONG arg2, const VOID *  arg3)
{
    dprintf(("GDI32: OS2SetBitmapBits"));
    return O32_SetBitmapBits(arg1, (DWORD)arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBitmapDimensionEx( HBITMAP arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: OS2SetBitmapDimensionEx"));
    return O32_SetBitmapDimensionEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetBoundsRect( HDC arg1, const RECT * arg2, UINT arg3)
{
    dprintf(("GDI32: OS2SetBoundsRect"));
    return O32_SetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBrushOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
 BOOL rc;

    rc = O32_SetBrushOrgEx(arg1, arg2, arg3, arg4);
    dprintf(("GDI32: OS2SetBrushOrgEx returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetDIBits( HDC arg1, HBITMAP arg2, UINT arg3, UINT arg4, const VOID * arg5, const BITMAPINFO * arg6, UINT  arg7)
{
    dprintf(("GDI32: OS2SetDIBits\n"));
    return O32_SetDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
INT WIN32API SetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx,	DWORD cy, INT xSrc, INT ySrc, UINT startscan, UINT lines, LPCVOID bits, const BITMAPINFO *info, UINT coloruse)
{
    INT result, imgsize, palsize;
    char *ptr;

    dprintf(("GDI32: OS2SetDIBitsToDevice hdc:%X xDest:%d yDest:%d, cx:%d, cy:%d, xSrc:%d, ySrc:%d, startscan:%d, lines:%d, bits:%X, info%X, coloruse:%d",
                 hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (LPVOID) bits, (PBITMAPINFO)info, coloruse));

    // EB: ->>> Crazy. Nobody seen this Open32 bug ?
    // Dont't like dirty pointers, but Open32 needs a bit help.
    // Only tested with winmine.
    palsize = QueryPaletteSize((BITMAPINFOHEADER*)&info->bmiHeader);
    imgsize = CalcBitmapSize(info->bmiHeader.biBitCount,
                             info->bmiHeader.biWidth, info->bmiHeader.biHeight);
    ptr = ((char *)info) + palsize + sizeof(BITMAPINFOHEADER);
    if(bits >= ptr && bits < ptr + imgsize)
    {
        bits = (char *)bits - imgsize +
                   CalcBitmapSize(info->bmiHeader.biBitCount,
                                  info->bmiHeader.biWidth, lines);
    }
    // EB: <<<-

    result = O32_SetDIBitsToDevice(hdc, xDest, yDest, cx, cy, xSrc, ySrc, startscan, lines, (PVOID) bits, (PBITMAPINFO)info, coloruse);
    return result;
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API SetEnhMetaFileBits( UINT arg1, const BYTE *  arg2)
{
    dprintf(("GDI32: OS2SetEnhMetaFileBits"));
    return O32_SetEnhMetaFileBits(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetGraphicsMode(HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetGraphicsMode"));
    return O32_SetGraphicsMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetMapMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetMapMode"));
    return O32_SetMapMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SetMapperFlags( HDC arg1, DWORD  arg2)
{
    dprintf(("GDI32: OS2SetMapperFlags"));
    return O32_SetMapperFlags(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API SetMetaFileBitsEx( UINT arg1, const BYTE * arg2)
{
    dprintf(("GDI32: OS2SetMetaFileBitsEx"));
    return O32_SetMetaFileBitsEx(arg1, (PBYTE)arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMiterLimit( HDC arg1, float  arg2, float *  arg3)
{
    dprintf(("GDI32: OS2SetMiterLimit"));
    return O32_SetMiterLimit(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetPaletteEntries( HPALETTE arg1, UINT arg2, UINT arg3, PALETTEENTRY * arg4)
{
    dprintf(("GDI32: OS2SetPaletteEntries"));
    return O32_SetPaletteEntries(arg1, arg2, arg3, (const PALETTEENTRY *)arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetPolyFillMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetPolyFillMode"));
    return O32_SetPolyFillMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectRgn( HRGN arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: OS2SetRectRgn"));
    return O32_SetRectRgn(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetTextAlign( HDC arg1, UINT  arg2)
{
    dprintf(("GDI32: OS2SetTextAlign"));
    return O32_SetTextAlign(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetTextCharacterExtra( HDC arg1, int  arg2)
{
    dprintf(("GDI32: OS2SetTextCharacterExtra"));
    return O32_SetTextCharacterExtra(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTextJustification( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OS2SetTextJustification"));
    return O32_SetTextJustification(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportExtEx( HDC arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: OS2SetViewportExtEx"));
    return O32_SetViewportExtEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OS2SetViewportOrgEx"));
    return O32_SetViewportOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API SetWinMetaFileBits( UINT arg1, const BYTE * arg2, HDC arg3, const METAFILEPICT *  arg4)
{
    dprintf(("GDI32: OS2SetWinMetaFileBits"));
    return O32_SetWinMetaFileBits(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowExtEx( HDC arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: OS2SetWindowExtEx"));
    return O32_SetWindowExtEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OS2SetWindowOrgEx"));
    return O32_SetWindowOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWorldTransform( HDC arg1, const XFORM *arg2)
{
    dprintf(("GDI32: OS2SetWorldTransform"));
    return O32_SetWorldTransform(arg1, (LPXFORM)arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocA( HDC arg1, const DOCINFOA *arg2)
{
    dprintf(("GDI32: OS2StartDocA"));
    return O32_StartDoc(arg1, (LPDOCINFOA)arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocW( HDC arg1, const DOCINFOW *arg2)
{
    dprintf(("GDI32: OS2StartDocW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_StartDoc(arg1, arg2);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API StartPage( HDC arg1)
{
    dprintf(("GDI32: OS2StartPage"));
    return O32_StartPage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutW( HDC arg1, int arg2, int arg3, LPCWSTR arg4, int  arg5)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg4);
 BOOL  rc;

    dprintf(("GDI32: OS2TextOutW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_TextOut(arg1, arg2, arg3, astring, arg5);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnrealizeObject( HGDIOBJ arg1)
{
    dprintf(("GDI32: OS2UnrealizeObject"));
    return O32_UnrealizeObject(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WidenPath( HDC arg1)
{
    dprintf(("GDI32: OS2WidenPath"));
    return O32_WidenPath(arg1);
}
//******************************************************************************
//CreateDisardableBitmap is obsolete and can be replaced by CreateCompatibleBitmap
//******************************************************************************
HBITMAP WIN32API CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
{
    dprintf(("GDI32: OS2CreateDisardableBitmap\n"));
    return O32_CreateCompatibleBitmap(hDC, nWidth, nHeight);
}
//******************************************************************************
//TODO: Not implemented
//******************************************************************************
int WIN32API SetAbortProc(HDC hdc, ABORTPROC lpAbortProc)
{
  dprintf(("GDI32: OS2SetAbortProc - stub (1)w\n"));
  return(1);
}
//******************************************************************************
//TODO:
//Should return the character set of the font currently selected into the hdc
//******************************************************************************
UINT WIN32API GetTextCharset(HDC hdc)
{
  dprintf(("GDI32: OS2GetTextCharset, not complete\n"));
  return(ANSI_CHARSET);
}
//******************************************************************************
//Selects the current path as a clipping region for a device context, combining
//any existing clipping region by using the specified mode
//TODO: Can be emulated with SelectClipRegion??
//******************************************************************************
BOOL WIN32API SelectClipPath(HDC hdc, int iMode)
{
  dprintf(("GDI32: OS2SelectClipPath, not implemented!(TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//TODO: Sets the color adjustment values for a device context. (used to adjust
//      the input color of the src bitmap for calls of StretchBlt & StretchDIBits
//      functions when HALFTONE mode is set
//******************************************************************************
BOOL WIN32API SetColorAdjustment(HDC hdc, CONST COLORADJUSTMENT *lpca)
{
  dprintf(("GDI32: OS2SetColorAdjustment, not implemented!(TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//WINE: OBJECTS\DIB.C
//******************************************************************************
HBITMAP WIN32API CreateDIBSection(HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                                     VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
 HBITMAP res = 0;
 BOOL    fFlip = 0;

  dprintf(("GDI32: OS2CreateDIBSection, partly implemented!\n"));
  if(hSection) {
        dprintf(("GDI32: OS2CreateDIBSection, hSection != NULL, not supported!\n"));
        return NULL;
  }

  //SvL: 13-9-98: StarCraft uses bitmap with negative height
  if(pbmi->bmiHeader.biWidth < 0) {
        pbmi->bmiHeader.biWidth = -pbmi->bmiHeader.biWidth;
        fFlip = FLIP_HOR;
  }
  if(pbmi->bmiHeader.biHeight < 0) {
        pbmi->bmiHeader.biHeight = -pbmi->bmiHeader.biHeight;
        fFlip |= FLIP_VERT;
  }

  res = O32_CreateDIBitmap(hdc, &pbmi->bmiHeader, 0, NULL, pbmi, 0);
  if (res)
  {
        DIBSection *dsect = new DIBSection((WINBITMAPINFOHEADER *)&pbmi->bmiHeader, (DWORD)res, fFlip);
        *ppvBits = dsect->GetDIBObject();
        return(res);
  }

  /* Error.  */
  if (res) DeleteObject(res);
  *ppvBits = NULL;
#ifdef DEBUG
  dprintf(("GDI32: CreateDIBSection, error!\n"));
  dprintf(("pbmi->biWidth    %d", pbmi->bmiHeader.biWidth));
  dprintf(("pbmi->biHeight   %d", pbmi->bmiHeader.biHeight));
  dprintf(("pbmi->biBitCount %d", pbmi->bmiHeader.biBitCount));
#endif

  return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries,
                                  RGBQUAD *pColors)
{
 HPALETTE hpal = O32_GetCurrentObject(hdc, OBJ_PAL);
 UINT rc;
 int i;

  dprintf(("GDI32: OS2GetDIBColorTable, not implemented?\n"));
  rc = O32_GetPaletteEntries(hpal,
                         uStartIndex,
                         cEntries,
                         (PALETTEENTRY *)pColors);
  for(i=0;
      i<cEntries;
      i++)
  {
    pColors[i].rgbReserved = 0;
  }
  dprintf(("GDI32: GetDIBColor returns %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries,
                                  RGBQUAD *pColors)
{
 DIBSection *dsect = DIBSection::findHDC(hdc);

  dprintf(("GDI32: OS2SetDIBColorTable\n"));
  if(dsect) {
        return(dsect->SetDIBColorTable(uStartIndex, cEntries, pColors));
  }
  else  return(0);
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExA( HDC arg1, LPLOGFONTA arg2, FONTENUMPROCEXA arg3, LPARAM  arg4, DWORD dwFlags)
{
    dprintf(("GDI32: OS2EnumFontFamiliesExA, not implemented\n"));
    return 0;
}
//******************************************************************************
//******************************************************************************
INT WIN32API EnumFontFamiliesExW( HDC arg1, LPLOGFONTW arg2, FONTENUMPROCEXW arg3, LPARAM  arg4, DWORD dwFlags)
{
    dprintf(("GDI32: OS2EnumFontFamiliesW, not implemented\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreateHalftonePalette(HDC hdc)
{
    dprintf(("GDI32: OS2CreateHalftonePalette, not implemented\n"));
    return(NULL);
}
//******************************************************************************
//******************************************************************************
INT WIN32API TranslateCharsetInfo(DWORD *lpSrc, LPCHARSETINFO lpCs, DWORD dwFlags)
{
   dprintf(("GDI32: OS2TranslateCharsetInfo, not implemented\n"));
   return(0);
}
//******************************************************************************
//Maps colors to system palette; faster way to update window (instead of redrawing)
//We just redraw
//******************************************************************************
BOOL WIN32API UpdateColors(HDC hdc)
{
  dprintf(("GDI32: OS2UpdateColors\n"));
  return O32_InvalidateRect(O32_WindowFromDC(hdc), NULL, FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GdiFlush()
{
  dprintf(("GDI32: GdiFlush, not implemented (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GdiComment(HDC hdc, UINT cbSize, CONST BYTE *lpData)
{
  dprintf(("GDI32: GdiComment, not implemented (TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthFloatA(HDC hdc, UINT iFirstChar, UINT iLastChar, PFLOAT pxBUffer)
{
  dprintf(("GDI32: GetCharWidthFloatA, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthFloatW(HDC hdc, UINT iFirstChar, UINT iLastChar, PFLOAT pxBUffer)
{
  dprintf(("GDI32: GetCharWidthFloatW, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsFloatA(HDC hdc, UINT iFirstChar, UINT iLastChar, LPABCFLOAT pxBUffer)
{
  dprintf(("GDI32: GetCharABCWidthsFloatA, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsFloatW(HDC hdc,
                                     UINT iFirstChar,
                                     UINT iLastChar,
                                     LPABCFLOAT pxBUffer)
{
  dprintf(("GDI32: GetCharABCWidthsFloatA, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
INT WIN32API ExtEscape(HDC hdc, INT nEscape, INT cbInput, LPCSTR lpszInData,
                       INT cbOutput, LPSTR lpszOutData)
{
  dprintf(("GDI32: ExtEscape, not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
int WIN32API DrawEscape(HDC hdc, int nEscape, int cbInput, LPCSTR lpszInData)
{
  dprintf(("GDI32: DrawEscape, not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetColorAdjustment(HDC hdc, COLORADJUSTMENT *lpca)
{
  dprintf(("GDI32: GetColorAdjustment, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlgBlt(HDC hdcDest, CONST POINT *lpPoint, HDC hdcSrc, int nXSrc,
                        int nYSrc, int nWidth, int nHeight, HBITMAP hbmMask,
                        int xMast, int yMask)
{
  dprintf(("GDI32: PlgBlt, not implemented\n"));
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetGlyphOutlineA(HDC hdc, UINT uChar, UINT uFormat, LPGLYPHMETRICS lpgm,
                                   DWORD cbBuffer, LPVOID lpvBuffer, CONST MAT2 *lpmat2)
{
  dprintf(("GDI32: GetGlyphOutLineA, not implemented (GDI_ERROR)\n"));
  return(GDI_ERROR);
}
//******************************************************************************

//******************************************************************************
/*KSO Thu 21.05.1998*/
DWORD WIN32API GetGlyphOutlineW(HDC hdc, UINT uChar, UINT uFormat, LPGLYPHMETRICS lpgm,
                                   DWORD cbBuffer, LPVOID lpvBuffer, CONST MAT2 *lpmat2)
{
  dprintf(("GDI32: GetGlyphOutLineW, not implemented\n"));
  return(GDI_ERROR);
}
//******************************************************************************

//******************************************************************************
DWORD WIN32API GetFontData(HDC hdc, DWORD dwTable, DWORD dwOffset, LPVOID lpvBuffer,
                              DWORD dbData)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
int WIN32API DescribePixelFormat(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetSystemPaletteUse(HDC hdc, UINT flags)
{
  dprintf(("GDI32: SetSystemPaletteUse %X %X, not implemented (GDI_ERROR)\n", hdc, flags));
  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetObjectOwner( HGDIOBJ arg1, int arg2 )
{
  // Here is a guess for a undocumented entry
  dprintf(("GDI32: SetObjectOwner - stub (TRUE)\n"));
  return TRUE;
}
//******************************************************************************


/* Office 97 stubs - KSO Thu 21.05.1998*/
//******************************************************************************
BOOL WIN32API GetTextExtentExPointA(/*KSO Thu 21.05.1998*/
        HDC     hdc,
        LPCSTR  str,
        int     count,
        int     maxExt,
        LPINT   lpnFit,
        LPINT   alpDx,
        LPSIZE  size)
{
    int index, nFit, extent;
    SIZE tSize;

    dprintf(("GDI32: GetTextExtendExPointA\n"));

    size->cx = size->cy = nFit = extent = 0;
    for(index = 0; index < count; index++)
    {
      if(!O32_GetTextExtentPoint( hdc, str, 1, &tSize )) return FALSE;
      if( extent+tSize.cx < maxExt )
      {
        extent+=tSize.cx;
        nFit++;
        str++;
        if( alpDx )
          alpDx[index] = extent;
        if( tSize.cy > size->cy ) size->cy = tSize.cy;
      }
      else break;
    }
    size->cx = extent;

    if (lpnFit != NULL)  // check if result is desired
      *lpnFit = nFit;

    dprintf(("GDI32: GetTextExtendExPointA(%08x '%.*s' %d) returning %d %d %d\n",
             hdc,count,str,maxExt,nFit, size->cx,size->cy));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentExPointW(                                 /*KSO Thu 21.05.1998*/
        HDC     arg1,
        LPCWSTR arg2,
        int     arg3,
        int             arg4,
        LPINT   arg5,
        LPINT   arg6,
        LPSIZE  arg7
        )
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  BOOL  rc;

  dprintf(("GDI32: GetTextExtendExPointW\n"));
  rc = GetTextExtentExPointA(arg1, astring, arg3, arg4, arg5, arg6, arg7);
  FreeAsciiString(astring);
  return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTextCharsetInfo(                                             /*KSO Thu 21.05.1998*/
        HDC     hdc,
        LPFONTSIGNATURE lpSig,
        DWORD   dwFlags
        )
{
        dprintf(("GDI32: GetTextCharsetInfo - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemPaletteUse(                                   /*KSO Thu 21.05.1998*/
        HDC             hdc
        )
{
        dprintf(("GDI32: GetSystemPaletteUse - stub\n"));
        return FALSE; /*?*/
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayEnhMetaFileRecord(                                 /*KSO Thu 21.05.1998*/
        HDC                                     arg1,
        LPHANDLETABLE           arg2,
        CONST ENHMETARECORD *arg3,
        UINT                            arg4
        )
{
        dprintf(("GDI32: PlayEnhMetaFileRecord - stub\n"));
        return FALSE;
}
//******************************************************************************
UINT WIN32API GetEnhMetaFileDescriptionA(                    /*KSO Thu 21.05.1998*/
        HENHMETAFILE    arg1,
        UINT                    arg2,
        LPSTR                   arg3
        )
{
        dprintf(("GDI32: GetEnhMetaFileDescriptionA - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileDescriptionW(                    /*KSO Thu 21.05.1998*/
        HENHMETAFILE    arg1,
        UINT                    arg2,
        LPWSTR                  arg3
        )
{
        dprintf(("GDI32: GetEnhMetaFileDescriptionW - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DeleteColorSpace(                                              /*KSO Thu 21.05.1998*/
        HCOLORSPACE hColorSpace
        )
{
        dprintf(("GDI32: DeleteColorSpace - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetColorSpace(                                                 /*KSO Thu 21.05.1998*/
        HDC     hdc,
        HCOLORSPACE hColorSpace
        )
{
        dprintf(("GDI32: SetColorSpace - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
 HCOLORSPACE WIN32API CreateColorSpaceA(                             /*KSO Thu 21.05.1998*/
        LPLOGCOLORSPACEA lpLogColorSpace
        )
{
        dprintf(("GDI32: CreateColorSpaceA - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
HCOLORSPACE WIN32API CreateColorSpaceW(                              /*KSO Thu 21.05.1998*/
        LPLOGCOLORSPACEW lpwLogColorSpace
        )
{
        dprintf(("GDI32: CreateColorSpaceW - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetColorSpace(                                               /*KSO Thu 21.05.1998*/
        HDC hdc
        )
{
        dprintf(("GDI32: GetColorSpace - stub\n"));
        return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetICMMode(                                                             /*KSO Thu 21.05.1998*/
        HDC hdc,
        int mode
        )
{
        dprintf(("GDI32: SetICMMode - stub\n"));
        return 0;
}
//******************************************************************************




/*****************************************************************************
 * Name      : BOOL CancelDC
 * Purpose   : The CancelDC function cancels any pending operation on the
 *             specified device context (DC).
 * Parameters: HDC hdc   handle of device context
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CancelDC(HDC hdc)
{
  dprintf(("GDI32: CancelDC(%08xh) not implemented.\n",
           hdc));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL CheckColorsInGamut
 * Purpose   : The CheckColorsInGamut function indicates whether the specified
 *             color values are within the gamut of the specified device.
 * Parameters: HDC    hdc        handle of device context
 *             LPVOID lpaRGBQuad
 *             LPVOID lpResult
 *             DWORD  dwResult
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CheckColorsInGamut(HDC    hdc,
                                    LPVOID lpaRGBQuad,
                                    LPVOID lpResult,
                                    DWORD  dwResult)
{
  dprintf(("GDI32: CheckColorsInGamut(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           lpaRGBQuad,
           lpResult,
           dwResult));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ColorMatchToTarget
 * Purpose   : The ColorMatchToTarget function enables or disables preview for
 *             the specified device context. When preview is enabled, colors
 *             in subsequent output to the specified device context are
 *             displayed as they would appear on the target device. This is
 *             useful for checking how well the target maps the specified
 *             colors in an image. To enable preview, image color matching
 *             must be enabled for both the target and the preview device context.
 * Parameters: HDC    hdc        handle of device context
 *             HDC    hdcTarget  handle of target device context
 *             DWORD  uiAction
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API ColorMatchToTarget(HDC   hdc,
                                    HDC   hdcTarget,
                                    DWORD uiAction)
{
  dprintf(("GDI32: ColorMatchToTarget(%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           hdcTarget,
           uiAction));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL CombineTransform
 * Purpose   : The CombineTransform function concatenates two world-space to
 *             page-space transformations.
 * Parameters: LLPXFORM lLPXFORMResult address of combined transformation
 *             XFORM  *lLPXFORM1      address of 1st transformation
 *             XFORM  *lLPXFORM2      address of 2nd transformation
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CombineTransform(LPXFORM lLPXFORMResult,
                                  CONST   XFORM *lLPXFORM1,
                                  CONST   XFORM *lLPXFORM2)
{
  dprintf(("GDI32: CombineTransform(%08xh,%08xh,%08xh) not implemented.\n",
           lLPXFORMResult,
           lLPXFORM1,
           lLPXFORM2));

  return (FALSE);
}



/*****************************************************************************
 * Name      : HBRUSH CreateDIBPatternBrush
 * Purpose   : The CreateDIBPatternBrush function creates a logical brush that
 *             has the pattern specified by the specified device-independent
 *             bitmap (DIB). The brush can subsequently be selected into any
 *             device context that is associated with a device that supports
 *             raster operations.
 *             This function is provided only for compatibility with applications
 *             written for versions of Windows earlier than 3.0. For Win32-based
 *             applications, use the CreateDIBPatternBrushPt function.
 * Parameters: HGLOBAL hglbDIBPacked handle of device-independent bitmap
 *             UINT    fuColorSpec   color table data
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

HBRUSH WIN32API CreateDIBPatternBrush(HGLOBAL hglbDIBPacked,
                                         UINT    fuColorSpec)
{
  dprintf(("GDI32: CreateDIBPatternBrush(%08xh, %08xh) not implemented.\n",
           hglbDIBPacked,
           fuColorSpec));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceA
 * Purpose   : The CreateScalableFontResourceA function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceA(DWORD   fdwHidden,
                                             LPCSTR lpszFontRes,
                                             LPCSTR lpszFontFile,
                                             LPCSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           fdwHidden,
           lpszFontRes,
           lpszFontFile,
           lpszCurrentPath));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceW
 * Purpose   : The CreateScalableFontResourceW function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceW(DWORD   fdwHidden,
                                             LPCWSTR lpszFontRes,
                                             LPCWSTR lpszFontFile,
                                             LPCWSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           fdwHidden,
           lpszFontRes,
           lpszFontFile,
           lpszCurrentPath));

  return (FALSE);
}


/*****************************************************************************
 * Name      : int EnumICMProfilesA
 * Purpose   : The EnumICMProfilesA function enumerates the different color
 *             profiles that the system supports for the specified device context.
 * Parameters: HDC         hdc
 *             ICMENUMPROC lpICMEnumFunc
 *             LPARAM      lParam
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API EnumICMProfilesA(HDC         hdc,
                                 ICMENUMPROCA lpICMEnumProc,
                                 LPARAM      lParam)
{
  dprintf(("GDI32: EnumICMProfilesA(%08xh, %08xh, %08xh) not implemented(-1).\n",
           hdc,
           lpICMEnumProc,
           lParam));

  return (-1);
}


/*****************************************************************************
 * Name      : int EnumICMProfilesW
 * Purpose   : The EnumICMProfilesW function enumerates the different color
 *             profiles that the system supports for the specified device context.
 * Parameters: HDC         hdc
 *             ICMENUMPROC lpICMEnumFunc
 *             LPARAM      lParam
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API EnumICMProfilesW(HDC          hdc,
                              ICMENUMPROCW lpICMEnumProc,
                              LPARAM       lParam)
{
  dprintf(("GDI32: EnumICMProfilesW(%08xh, %08xh, %08xh) not implemented (-1).\n",
           hdc,
           lpICMEnumProc,
           lParam));

  return (-1);
}


/*****************************************************************************
 * Name      : BOOL FixBrushOrgEx
 * Purpose   : The FixBrushOrgEx function is not implemented in the Win32 API.
 *             It is provided for compatibility with Win32s. If called, the
 *             function does nothing, and returns FALSE.
 * Parameters: HDC, int, int, LPPOINT
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    : not implemented in Win32
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API FixBrushOrgEx(HDC     hdc,
                               int     iDummy1,
                               int     iDummy2,
                               LPPOINT lpPoint)
{
  dprintf(("GDI32: FixBrushOrgEx(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           iDummy1,
           iDummy2,
           lpPoint));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD GdiGetBatchLimit
 * Purpose   : The GdiGetBatchLimit function returns the maximum number of
 *             function calls that can be accumulated in the calling thread's
 *             current batch. The system flushes the current batch whenever
 *             this limit is exceeded.
 * Parameters:
 * Variables :
 * Result    : 1
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GdiGetBatchLimit(VOID)
{
  dprintf(("GDI32: GdiGetBatchLimit() not implemented (1).\n"));

  return (1);
}


/*****************************************************************************
 * Name      : DWORD GdiSetBatchLimit
 * Purpose   : The GdiSetBatchLimit function sets the maximum number of
 *             functions that can be accumulated in the calling thread's current
 *             batch. The system flushes the current batch whenever this limit
 *             is exceeded.
 * Parameters: DWORD dwLimit
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GdiSetBatchLimit(DWORD dwLimit)
{
  dprintf(("GDI32: GdiSetBatchLimit(%08xh) not implemented (1).\n",
           dwLimit));

  return (1);
}


/*****************************************************************************
 * Name      : DWORD GetCharacterPlacementA
 * Purpose   : The GetCharacterPlacementA function retrieves information about
 *             a character string, such as character widths, caret positioning,
 *             ordering within the string, and glyph rendering. The type of
 *             information returned depends on the dwFlags parameter and is
 *             based on the currently selected font in the given display context.
 *             The function copies the information to the specified GCP_RESULTSA
 *             structure or to one or more arrays specified by the structure.
 * Parameters: HDC     hdc        handle to device context
 *             LPCSTR lpString   pointer to string
 *             int     nCount     number of characters in string
 *             int     nMaxExtent maximum extent for displayed string
 *             LPGCP_RESULTSA *lpResults  pointer to buffer for placement result
 *             DWORD   dwFlags    placement flags
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetCharacterPlacementA(HDC           hdc,
                                         LPCSTR       lpString,
                                         int           nCount,
                                         int           nMaxExtent,
                                         GCP_RESULTSA * lpResults,
                                         DWORD         dwFlags)
{
  dprintf(("GDI32: GetCharacterPlacementA(%08xh,%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           lpString,
           nCount,
           nMaxExtent,
           lpResults,
           dwFlags));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD GetCharacterPlacementW
 * Purpose   : The GetCharacterPlacementW function retrieves information about
 *             a character string, such as character widths, caret positioning,
 *             ordering within the string, and glyph rendering. The type of
 *             information returned depends on the dwFlags parameter and is
 *             based on the currently selected font in the given display context.
 *             The function copies the information to the specified GCP_RESULTSW
 *             structure or to one or more arrays specified by the structure.
 * Parameters: HDC     hdc        handle to device context
 *             LPCSTR lpString   pointer to string
 *             int     nCount     number of characters in string
 *             int     nMaxExtent maximum extent for displayed string
 *             GCP_RESULTSW *lpResults  pointer to buffer for placement result
 *             DWORD   dwFlags    placement flags
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetCharacterPlacementW(HDC           hdc,
                                         LPCWSTR       lpString,
                                         int           nCount,
                                         int           nMaxExtent,
                                         GCP_RESULTSW *lpResults,
                                         DWORD         dwFlags)
{
  dprintf(("GDI32: GetCharacterPlacementW(%08xh,%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hdc,
           lpString,
           nCount,
           nMaxExtent,
           lpResults,
           dwFlags));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD GetDeviceGammaRamp
 * Purpose   : The GetDeviceGammaRamp function retrieves the gamma ramp on
 *             direct color display boards.
 * Parameters: HDC     hdc        handle to device context
 *             LPVOID  lpRamp     Gamma ramp array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetDeviceGammaRamp(HDC    hdc,
                                     LPVOID lpRamp)
{
  dprintf(("GDI32: GetDeviceGammaRamp(%08xh, %08xh) not implemented.\n",
           hdc,
           lpRamp));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD GetFontLanguageInfo
 * Purpose   : The GetFontLanguageInfo function returns information about the
 *             currently selected font for the specified display context.
 *             Applications typically use this information and the
 *             GetCharacterPlacement function to prepare a character string for display.
 * Parameters: HDC     hdc        handle to device context
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetFontLanguageInfo(HDC hdc)
{
  dprintf(("GDI32: GetFontLanguageInfo(%08xh) not implemented.\n",
           hdc));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL GetICMProfileA
 * Purpose   : The GetICMProfileA function retrieves the name of the color
 *             profile file for the device associated with the specified device
 *             context.
 * Parameters: HDC     hdc        handle to device context
 *             DWORD   cbName
 *             LPTSTR  lpszFilename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetICMProfileA(HDC    hdc,
                                DWORD  cbName,
                                LPTSTR lpszFilename)
{
  dprintf(("GDI32: GetICMProfileA(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           cbName,
           lpszFilename));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetICMProfileW
 * Purpose   : The GetICMProfileW function retrieves the name of the color
 *             profile file for the device associated with the specified device
 *             context.
 * Parameters: HDC     hdc        handle to device context
 *             DWORD   cbName
 *             LPWSTR  lpszFilename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetICMProfileW(HDC    hdc,
                                DWORD  cbName,
                                LPTSTR lpszFilename)
{
  dprintf(("GDI32: GetICMProfileW(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           cbName,
           lpszFilename));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetLogColorSpaceA
 * Purpose   : The GetLogColorSpace function retrieves information about the
 *             logical color space identified by the specified handle.
 * Parameters: HCOLORSPACE     hColorSpace
 *             LPLOGCOLORSPACE lpbuffer
 *             DWORD           nSize
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

#define LPLOGCOLORSPACE LPVOID
BOOL WIN32API GetLogColorSpaceA(HCOLORSPACE     hColorSpace,
                                   LPLOGCOLORSPACE lpBuffer,
                                   DWORD           nSize)
{
  dprintf(("GDI32: GetLogColorSpaceA(%08xh, %08xh, %08xh) not implemented.\n",
           hColorSpace,
           lpBuffer,
           nSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetLogColorSpaceW
 * Purpose   : The GetLogColorSpace function retrieves information about the
 *             logical color space identified by the specified handle.
 * Parameters: HCOLORSPACE     hColorSpace
 *             LPLOGCOLORSPACE lpbuffer
 *             DWORD           nSize
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetLogColorSpaceW(HCOLORSPACE     hColorSpace,
                                   LPLOGCOLORSPACE lpBuffer,
                                   DWORD           nSize)
{
  dprintf(("GDI32: GetLogColorSpaceW(%08xh, %08xh, %08xh) not implemented.\n",
           hColorSpace,
           lpBuffer,
           nSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : int GetMetaRgn
 * Purpose   : The GetMetaRgn function retrieves the current metaregion for
 *             the specified device context.
 * Parameters: HDC  hdc   handle of device context
 *             HRGN hrgn  handle of region
 * Variables :
 * Result    : 0 / 1
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetMetaRgn(HDC  hdc,
                           HRGN hrgn)
{
  dprintf(("GDI32: GetMetaRgn(%08xh, %08xh) not implemented.\n",
           hdc,
           hrgn));

  return (0);
}


/*****************************************************************************
 * Name      : int GetPixelFormat
 * Purpose   : The GetPixelFormat function obtains the index of the specified
 *             device context's currently selected pixel format.
 * Parameters: HDC  hdc   handle of device context
 * Variables :
 * Result    : 0 / 1
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetPixelFormat(HDC hdc)
{
  dprintf(("GDI32: GetPixelFormat(%08xh) not implemented.\n",
           hdc));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL PolyTextOutA
 * Purpose   : The PolyTextOutA function draws several strings using the font
 *             and text colors currently selected in the specified device context.
 * Parameters: HDC  hdc   handle of device context
 *             CONST POLYTEXT *pptxt address of array of structures that identify strings
 *             int            cStrings number of structures in array
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API PolyTextOutA(HDC       hdc,
                              POLYTEXTA *pptxt,
                              int       cStrings)
{
  dprintf(("GDI32: PolyTextOutA(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           pptxt,
           cStrings));

  return (FALSE);
}


#if 0

The POLYTEXT structure describes how the PolyTextOut function should draw a string of text.

Members

x

Specifies the horizontal reference point for the string. The string is aligned to this point using the current text-alignment mode.

y

Specifies the vertical reference point for the string. The string is aligned to this point using the current text-alignment mode.

n

Specifies the number of characters in the string.

uiFlags

Specifies whether the string is to be opaque or clipped and whether the string is accompanied by an array of character-width values. This member can be one or more of the following values:

Value   Meaning
ETO_OPAQUE      The rectangles given for each string is to be opaqued with the current background color.
ETO_CLIPPED     Each string is to be clipped to its given rectangle.
lpstr

Points to a string of text to be drawn by the PolyTextOut function.

rcl

Specifies a rectangle structure that contains the dimensions of the opaquing or clipping rectangle. This member is ignored if neither of the ETO_OPAQUE nor the ETO_CLIPPED value is specified for the uiFlags member.

pdx

Specifies in an array the width value for each character in the string.

See Also

PolyTextOut
#endif


/*****************************************************************************
 * Name      : BOOL PolyTextOutW
 * Purpose   : The PolyTextOutW function draws several strings using the font
 *             and text colors currently selected in the specified device context.
 * Parameters: HDC  hdc   handle of device context
 *             CONST POLYTEXT *pptxt address of array of structures that identify strings
 *             int            cStrings number of structures in array
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API PolyTextOutW(HDC       hdc,
                              POLYTEXTW *pptxt,
                              int       cStrings)
{
  dprintf(("GDI32: PolyTextOutW(%08xh, %08xh, %08xh) not implemented.\n",
           hdc,
           pptxt,
           cStrings));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetDeviceGammaRamp
 * Purpose   : The SetDeviceGammaRamp function sets the gamma ramp on direct
 *             color display boards.
 * Parameters: HDC    hdc   handle of device context
 *             LPVOID lpRamp
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetDeviceGammaRamp(HDC    hdc,
                                    LPVOID lpRamp)
{
  dprintf(("GDI32: SetDeviceGammaRamp(%08xh, %08xh) not implemented.\n",
           hdc,
           lpRamp));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetICMProfileA
 * Purpose   : The SetICMProfileA function sets the color profile for the
 *             specified device context.
 * Parameters: HDC    hdc   handle of device context
 *             LPTSTR lpFileName
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetICMProfileA(HDC    hdc,
                                LPTSTR lpFileName)
{
  dprintf(("GDI32: SetICMProfileA(%08xh, %s) not implemented.\n",
           hdc,
           lpFileName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetICMProfileW
 * Purpose   : The SetICMProfileW function sets the color profile for the
 *             specified device context.
 * Parameters: HDC    hdc   handle of device context
 *             LPTSTR lpFileName
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetICMProfileW(HDC    hdc,
                                LPWSTR lpFileName)
{
  dprintf(("GDI32: SetICMProfileW(%08xh, %s) not implemented.\n",
           hdc,
           lpFileName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : int SetMetaRgn
 * Purpose   : The SetMetaRgn function intersects the current clipping region
 *             for the specified device context with the current metaregion
 *             and saves the combined region as the new metaregion for the
 *             specified device context. The clipping region is reset to a null region.
 * Parameters: HDC    hdc   handle of device context
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetMetaRgn(HDC hdc)
{
  dprintf(("GDI32: SetMetaRgn(%08xh) not implemented.\n",
           hdc));

  return (NULLREGION);
}


/*****************************************************************************
 * Name      : BOOL UpdateICMRegKeyA
 * Purpose   : The UpdateICMRegKeyA function installs, removes, or queries
 *             registry entries that identify ICC color profiles or color-matching
 *             DLLs. The function carries out the action specified by the nCommand
 *             parameter.
 * Parameters: DWORD   dwReserved
 *             DWORD   CMID
 *             LPTSTR  lpszFileName
 *             UINT    nCommand
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateICMRegKeyA(DWORD  dwReserved,
                                  DWORD  CMID,
                                  LPTSTR lpszFileName,
                                  UINT   nCommand)
{
  dprintf(("GDI32: UpdateICMRegKeyA(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           dwReserved,
           CMID,
           lpszFileName,
           nCommand));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL UpdateICMRegKeyW
 * Purpose   : The UpdateICMRegKeyW function installs, removes, or queries
 *             registry entries that identify ICC color profiles or color-matching
 *             DLLs. The function carries out the action specified by the nCommand
 *             parameter.
 * Parameters: DWORD   dwReserved
 *             DWORD   CMID
 *             LPWSTR  lpszFileName
 *             UINT    nCommand
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateICMRegKeyW(DWORD  dwReserved,
                                  DWORD  CMID,
                                  LPWSTR lpszFileName,
                                  UINT   nCommand)
{
  dprintf(("GDI32: UpdateICMRegKeyW(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           dwReserved,
           CMID,
           lpszFileName,
           nCommand));

  return (FALSE);
}


