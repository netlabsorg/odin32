/* $Id: gdi32.cpp,v 1.28 1999-12-30 18:51:48 sandervl Exp $ */

/*
 * GDI32 apis
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

static hFntDefaultGui = NULL;
HGDIOBJ WIN32API GetStockObject(int arg1)
{
 HGDIOBJ obj;

    switch(arg1)
    {
        case DEFAULT_GUI_FONT:
                if(NULL==hFntDefaultGui)
                  hFntDefaultGui = CreateFontA( 9, 0, 0, 0, FW_MEDIUM, FALSE,
                                                FALSE, FALSE, ANSI_CHARSET,
                                                OUT_DEFAULT_PRECIS,
                                                CLIP_DEFAULT_PRECIS,
                                                DEFAULT_QUALITY,
                                                FIXED_PITCH|FF_MODERN, "WarpSans");
                obj = hFntDefaultGui;
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
    dprintf(("GDI32: RealizePalette(0x%08X)\n",arg1));
    return O32_RealizePalette(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectA( HGDIOBJ hObject, int size, void *lpBuffer)
{
 int rc;

  if(size == 0 || lpBuffer == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
  }

  if(DIBSection::getSection() != NULL)
  {
    	DIBSection *dsect = DIBSection::find(hObject);
    	if(dsect)
    	{
      		rc = dsect->GetDIBSection(size, lpBuffer);
		if(rc == 0) {
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
		}
		SetLastError(ERROR_SUCCESS);
		return rc;
    	}
  }

  dprintf(("GDI32: GetObject %X %X %X\n", hObject, size, lpBuffer));
  return O32_GetObject(hObject, size, lpBuffer);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetObjectW( HGDIOBJ arg1, int arg2, void *  arg3)
{
    dprintf(("GDI32: GetObjectW %X, %d %X not complete!", arg1, arg2, arg3));
    return GetObjectA(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetObjectType( HGDIOBJ arg1)
{
    dprintf2(("GDI32: GetObjectType\n"));
    return O32_GetObjectType(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteObject(HANDLE hObj)
{
    dprintf(("GDI32: DeleteObject %x", hObj));
    DIBSection::deleteSection((DWORD)hObj);
    return O32_DeleteObject(hObj);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteDC( HDC hdc)
{
    dprintf(("GDI32: DeleteDC %x", hdc));
    return O32_DeleteDC(hdc);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreatePalette( const LOGPALETTE * arg1)
{
  HPALETTE rc;
    dprintf(("GDI32: CreatePalette\n"));
   for(int i=0; i<arg1->palNumEntries;i++)
   {
     dprintf2(("Index %d : 0x%08X\n",i, *((DWORD*)(&arg1->palPalEntry[i])) ));
   }
   rc = O32_CreatePalette(arg1);
   dprintf(("  returns 0x%08X\n",rc));

   return rc;
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
  int iHeight;
  HBITMAP rc;
    dprintf(("GDI32: CreateDIBitmap\n"));

    //TEMPORARY HACK TO PREVENT CRASH IN OPEN32 (WSeB GA)

    iHeight = arg2->biHeight;
    if(arg2->biHeight < 0)
    {
      ((BITMAPINFOHEADER *)arg2)->biHeight = -arg2->biHeight;
    }

    rc = O32_CreateDIBitmap(arg1, arg2, arg3, arg4, arg5, arg6);

    ((BITMAPINFOHEADER *)arg2)->biHeight = iHeight;

    return rc;
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
 HDC rc;

    rc = O32_CreateCompatibleDC(arg1);
    dprintf(("GDI32: CreateCompatibleDC %X returned %x", arg1, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API StretchDIBits(HDC hdc, INT xDst, INT yDst, INT widthDst,
                           INT heightDst, INT xSrc, INT ySrc, INT widthSrc,
                           INT heightSrc, const void *bits,
                           const BITMAPINFO *info, UINT wUsage, DWORD dwRop )
{
#if 1
    dprintf(("GDI32: StretchDIBits"));

    if(wUsage == DIB_PAL_COLORS && info->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
    {
      // workaround for open32 bug.
      // If syscolors > 256 and wUsage == DIB_PAL_COLORS.

      int i;
      USHORT *pColorIndex = (USHORT *)info->bmiColors;
      RGBQUAD *pColors = (RGBQUAD *) alloca(info->bmiHeader.biClrUsed *
                         sizeof(RGBQUAD));
      BITMAPINFO *infoLoc = (BITMAPINFO *) alloca(sizeof(BITMAPINFO) +
                             info->bmiHeader.biClrUsed * sizeof(RGBQUAD));

      memcpy(infoLoc, info, sizeof(BITMAPINFO));

      if(GetDIBColorTable(hdc, 0, info->bmiHeader.biClrUsed, pColors) == 0)
        return FALSE;

      for(i=0;i<info->bmiHeader.biClrUsed;i++, pColorIndex++)
      {
         infoLoc->bmiColors[i] = pColors[*pColorIndex];
      }

      return O32_StretchDIBits(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc,
                               widthSrc, heightSrc, (void *)bits,
                               (PBITMAPINFO)infoLoc, DIB_RGB_COLORS, dwRop);
    }

    return O32_StretchDIBits(hdc, xDst, yDst, widthDst, heightDst, xSrc, ySrc,
                             widthSrc, heightSrc, (void *)bits,
                             (PBITMAPINFO)info, wUsage, dwRop);
#else
    dprintf(("GDI32: StretchDIBits\n"));
    return O32_StretchDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, (void *)arg10, (PBITMAPINFO)arg11, arg12, arg13);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                            int nWidthDest, int nHeightDest,
                            HDC hdcSrc, int nXOriginSrc, int nYOriginSrc,
                            int nWidthSrc, int nHeightSrc, DWORD dwRop)
{
  dprintf(("GDI32: StretchBlt Dest: (%d, %d) size (%d, %d)\n",
           nXOriginDest, nYOriginDest, nWidthDest, nHeightDest));
  dprintf(("GDI32: StretchBlt Src : (%d, %d) size (%d, %d)\n",
           nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc));
  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(hdcSrc);
    if(dsect)
    {
      dprintf((" Do stretched DIB Blt\n"));
      return(dsect->BitBlt( hdcDest,
                            nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                            nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc,
                            dwRop));
    }
  }
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
  dprintf(("GDI32: SetStretchBltMode 0x%08X, 0x%08X\n",arg1, arg2));

  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(arg1);
    if(dsect)
    {
      dprintf(("       - DC is DIBSection\n"));
    }
  }
  return O32_SetStretchBltMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API SelectObject(HDC hdc, HGDIOBJ hObj)
{
 HGDIOBJ rc;

////    dprintf(("GDI32: SelectObject\n"));

    if(DIBSection::getSection() != NULL)
    {
      DIBSection *dsect;

      dsect = DIBSection::find(hdc);
      if(dsect)
      {
        //remove previously selected dibsection
        dsect->UnSelectDIBObject();
      }
      dsect = DIBSection::find((DWORD)hObj);
      if(dsect)
      {
        dsect->SelectDIBObject(hdc);
      }
    }
    rc = O32_SelectObject(hdc, hObj);
    if(rc != 0 && DIBSection::getSection != NULL)
    {
      DIBSection *dsect = DIBSection::find((DWORD)rc);
      if(dsect)
      {
        dsect->UnSelectDIBObject();
      }
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API SelectPalette(HDC arg1, HPALETTE arg2, BOOL arg3)
{
  dprintf(("GDI32: SelectPalette (0x%08X, 0x%08X, 0x%08X)\n", arg1, arg2, arg3));
  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(arg1);
    if(dsect)
    {
      PALETTEENTRY Pal[256];
      char PalSize = dsect->GetBitCount();
      dprintf(("       - Set Palette Values in DIBSection\n"));
      if(PalSize<=8)
      {
        GetPaletteEntries( arg2, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
        dsect->SetDIBColorTable(0, 1<< PalSize, (RGBQUAD*)&Pal);
      }

    }
  }
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
    dprintf(("GDI32: AbortDoc"));
    return O32_AbortDoc(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AbortPath( HDC arg1)
{
    dprintf(("GDI32: AbortPath"));
    return O32_AbortPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AngleArc( HDC arg1, int arg2, int arg3, DWORD arg4, float  arg5, float  arg6)
{
    dprintf(("GDI32: AngleArc"));
    return O32_AngleArc(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AnimatePalette( HPALETTE arg1, UINT arg2, UINT arg3, const PALETTEENTRY * arg4)
{
    dprintf(("GDI32: AnimatePalette"));
    return O32_AnimatePalette(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Arc( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int  arg9)
{
    dprintf(("GDI32: Arc"));
    return O32_Arc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ArcTo( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    dprintf(("GDI32: ArcTo"));
    return O32_ArcTo(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BeginPath( HDC arg1)
{
    dprintf(("GDI32: BeginPath"));
    return O32_BeginPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BitBlt(HDC hdcDest, int arg2, int arg3, int arg4, int arg5, HDC hdcSrc, int arg7, int arg8, DWORD  arg9)
{
    if(DIBSection::getSection() != NULL) {
        DIBSection *dsect = DIBSection::findHDC(hdcSrc);
        if(dsect) {
                return(dsect->BitBlt(hdcDest, arg2, arg3, arg4, arg5, arg7, arg8, arg4, arg5, arg9));
        }
    }
    dprintf(("GDI32: BitBlt to hdc %X from (%d,%d) to (%d,%d), (%d,%d) rop %X\n", hdcDest, arg7, arg8, arg2, arg3, arg4, arg5, arg9));
    return O32_BitBlt(hdcDest, arg2, arg3, arg4, arg5, hdcSrc, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Chord( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    dprintf(("GDI32: Chord"));
    return O32_Chord(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CloseEnhMetaFile( HDC arg1)
{
    dprintf(("GDI32: CloseEnhMetaFile"));
    return O32_CloseEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseFigure( HDC arg1)
{
    dprintf(("GDI32: CloseFigure"));
    return O32_CloseFigure(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CloseMetaFile( HDC arg1)
{
    dprintf(("GDI32: CloseMetaFile"));
    return O32_CloseMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API CombineRgn( HRGN arg1, HRGN  arg2, HRGN  arg3, int  arg4)
{
    dprintf(("GDI32: CombineRgn"));
    return O32_CombineRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileA( HENHMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: CopyEnhMetaFileA"));
    return O32_CopyEnhMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileW( HENHMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HENHMETAFILE rc;

    dprintf(("GDI32: CopyEnhMetaFileW"));
    rc = O32_CopyEnhMetaFile(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileA( HMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: CopyMetaFileA"));
    return O32_CopyMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileW( HMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HMETAFILE rc;

    dprintf(("GDI32: CopyMetaFileW"));
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
    dprintf(("GDI32: CreateBitmap (%d,%d) bps %d returned %d\n", nWidth, nHeight, cBitsPerPel, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API CreateBitmapIndirect( const BITMAP * arg1)
{
    dprintf(("GDI32: CreateBitmapIndirect"));
    return O32_CreateBitmapIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateBrushIndirect( const LOGBRUSH * arg1)
{
    dprintf(("GDI32: CreateBrushIndirect"));
    return O32_CreateBrushIndirect((LPLOGBRUSH)arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, const DEVMODEA * arg4)
{
    dprintf(("GDI32: CreateDCA"));
    return O32_CreateDC(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    char *astring4, *astring5;

    char *astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring3 = UnicodeToAsciiString((LPWSTR)arg3);

    if(arg4)
    {
      astring4 = UnicodeToAsciiString((LPWSTR)(arg4->dmDeviceName));
      astring5 = UnicodeToAsciiString((LPWSTR)(arg4->dmFormName));
    }

    HDC   rc;
    DEVMODEA devmode;

    dprintf(("GDI32: CreateDCW"));

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
      devmode.dmOrientation      = arg4->dmOrientation;
      devmode.dmPaperSize        = arg4->dmPaperSize;
      devmode.dmPaperLength      = arg4->dmPaperLength;
      devmode.dmPaperWidth       = arg4->dmPaperWidth;
      devmode.dmScale            = arg4->dmScale;
      devmode.dmCopies           = arg4->dmCopies;
      devmode.dmDefaultSource    = arg4->dmDefaultSource;
      devmode.dmPrintQuality     = arg4->dmPrintQuality;
      devmode.dmColor            = arg4->dmColor;
      devmode.dmDuplex           = arg4->dmDuplex;
      devmode.dmYResolution      = arg4->dmYResolution;
      devmode.dmTTOption         = arg4->dmTTOption;
      devmode.dmCollate          = arg4->dmCollate;
      devmode.dmLogPixels        = arg4->dmLogPixels;
      devmode.dmBitsPerPel       = arg4->dmBitsPerPel;
      devmode.dmPelsWidth        = arg4->dmPelsWidth;
      devmode.dmPelsHeight       = arg4->dmPelsHeight;
      devmode.dmDisplayFlags     = arg4->dmDisplayFlags;
      devmode.dmDisplayFrequency = arg4->dmDisplayFrequency;
      devmode.dmICMMethod        = arg4->dmICMMethod;
      devmode.dmICMIntent        = arg4->dmICMIntent;
      devmode.dmMediaType        = arg4->dmMediaType;
      devmode.dmDitherType       = arg4->dmDitherType;
      devmode.dmReserved1        = arg4->dmReserved1;
      devmode.dmReserved2        = arg4->dmReserved2;
      rc = O32_CreateDC(astring1,astring2,astring3,&devmode);
    }
    else
      rc = O32_CreateDC(astring1,astring2,astring3, NULL);

    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    FreeAsciiString(astring3);

    if(arg4)
    {
      FreeAsciiString(astring4);
      FreeAsciiString(astring5);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgn( int arg1, int arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: CreateEllipticRgn"));
    return O32_CreateEllipticRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgnIndirect( const RECT * arg1)
{
    dprintf(("GDI32: CreateEllipticRgnIndirect"));
    return O32_CreateEllipticRgnIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileA( HDC arg1, LPCSTR arg2, const RECT * arg3, LPCSTR  arg4)
{
    dprintf(("GDI32: CreateEnhMetaFileA"));
    return O32_CreateEnhMetaFile(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileW( HDC arg1, LPCWSTR arg2, const RECT * arg3, LPCWSTR  arg4)
{
    char *astring1 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg4);
    HENHMETAFILE   rc;

    dprintf(("GDI32: CreateMetaFileW"));
    rc = O32_CreateEnhMetaFile(arg1,astring1,arg3,astring2);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return rc;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateHatchBrush( int arg1, COLORREF  arg2)
{
    dprintf(("GDI32: CreateHatchBrush"));
    return O32_CreateHatchBrush(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, const DEVMODEA * arg4)
{
    dprintf(("GDI32: CreateICA"));
    return O32_CreateIC(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateICW( LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3, const DEVMODEW * arg4)
{
    char *astring4, *astring5;

    char *astring1 = UnicodeToAsciiString((LPWSTR)arg1);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring3 = UnicodeToAsciiString((LPWSTR)arg3);
    if(arg4)
    {
      astring4 = UnicodeToAsciiString((LPWSTR)(arg4->dmDeviceName));
      astring5 = UnicodeToAsciiString((LPWSTR)(arg4->dmFormName));
    }

    HDC   rc;
    DEVMODEA devmode;

    dprintf(("GDI32: CreateICW"));

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
      devmode.dmOrientation      = arg4->dmOrientation;
      devmode.dmPaperSize        = arg4->dmPaperSize;
      devmode.dmPaperLength      = arg4->dmPaperLength;
      devmode.dmPaperWidth       = arg4->dmPaperWidth;
      devmode.dmScale            = arg4->dmScale;
      devmode.dmCopies           = arg4->dmCopies;
      devmode.dmDefaultSource    = arg4->dmDefaultSource;
      devmode.dmPrintQuality     = arg4->dmPrintQuality;
      devmode.dmColor            = arg4->dmColor;
      devmode.dmDuplex           = arg4->dmDuplex;
      devmode.dmYResolution      = arg4->dmYResolution;
      devmode.dmTTOption         = arg4->dmTTOption;
      devmode.dmCollate          = arg4->dmCollate;
      devmode.dmLogPixels        = arg4->dmLogPixels;
      devmode.dmBitsPerPel       = arg4->dmBitsPerPel;
      devmode.dmPelsWidth        = arg4->dmPelsWidth;
      devmode.dmPelsHeight       = arg4->dmPelsHeight;
      devmode.dmDisplayFlags     = arg4->dmDisplayFlags;
      devmode.dmDisplayFrequency = arg4->dmDisplayFrequency;
      devmode.dmICMMethod        = arg4->dmICMMethod;
      devmode.dmICMIntent        = arg4->dmICMIntent;
      devmode.dmMediaType        = arg4->dmMediaType;
      devmode.dmDitherType       = arg4->dmDitherType;
      devmode.dmReserved1        = arg4->dmReserved1;
      devmode.dmReserved2        = arg4->dmReserved2;

      rc = O32_CreateIC(astring1,astring2,astring3,&devmode);
    }
    else
      rc = O32_CreateIC(astring1,astring2,astring3, NULL);

    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    FreeAsciiString(astring3);
    if(arg4)
    {
      FreeAsciiString(astring4);
      FreeAsciiString(astring5);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: CreateMetaFileA"));
    return O32_CreateMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HDC   rc;

    dprintf(("GDI32: CreateMetaFileW"));
    rc = O32_CreateMetaFile(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgn( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect)
{
    dprintf(("GDI32: CreateRectRgn (%d,%d)(%d,%d)", nLeftRect, nTopRect, nRightRect, nBottomRect));
    return O32_CreateRectRgn(nLeftRect, nTopRect, nRightRect, nBottomRect);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgnIndirect( const RECT * lpRect)
{
    if(lpRect == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
    }
    dprintf(("GDI32: CreateRectRgnIndirect (%d,%d)(%d,%d)", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom));
    return O32_CreateRectRgnIndirect(lpRect);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRoundRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, 
                                 int nWidthEllipse, int nHeightEllipse)
{
    dprintf(("GDI32: CreateRoundRectRgn (%d,%d)(%d,%d) (%d,%d)", nLeftRect, nTopRect, nRightRect, nBottomRect, nWidthEllipse, nHeightEllipse));
    return O32_CreateRoundRectRgn(nLeftRect, nTopRect, nRightRect, nBottomRect, nWidthEllipse, nHeightEllipse);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateSolidBrush( COLORREF arg1)
{
    dprintf(("GDI32: CreateSolidBrush\n"));
    return O32_CreateSolidBrush(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DPtoLP( HDC arg1, PPOINT arg2, int  arg3)
{
    dprintf(("GDI32: DPtoLP\n"));
    return O32_DPtoLP(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteEnhMetaFile( HENHMETAFILE arg1)
{
    dprintf(("GDI32: DeleteEnhMetaFile\n"));
    return O32_DeleteEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteMetaFile( HMETAFILE arg1)
{
    dprintf(("GDI32: DeleteMetaFile"));
    return O32_DeleteMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Ellipse( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: Ellipse"));
    return O32_Ellipse(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API EndDoc( HDC arg1)
{
    dprintf(("GDI32: EndDoc"));
    return O32_EndDoc(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API EndPage( HDC arg1)
{
    dprintf(("GDI32: EndPage"));
    return O32_EndPage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPath( HDC arg1)
{
    dprintf(("GDI32: EndPath"));
    return O32_EndPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumEnhMetaFile( HDC arg1, HENHMETAFILE arg2, ENHMFENUMPROC arg3, PVOID arg4, const RECT *  arg5)
{
    dprintf(("GDI32: EnumEnhMetaFile DOESN'T WORK!"));
//    return O32_EnumEnhMetaFile(arg1, arg2, arg3, arg4, arg5);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PatBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, DWORD  arg6)
{
 BOOL rc;

  rc = O32_PatBlt(arg1, arg2, arg3, arg4, arg5, arg6);
  dprintf(("GDI32: PatBlt (%d,%d) (%d,%d) returned %d\n", arg2, arg3, arg4, arg5, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Rectangle( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: Rectangle\n"));
    return O32_Rectangle(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetROP2( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetROP2"));
    return O32_SetROP2(arg1, arg2);
}
//******************************************************************************
//TODO: Callback
//******************************************************************************
BOOL WIN32API EnumMetaFile( HDC arg1, HMETAFILE arg2, MFENUMPROC arg3, LPARAM  arg4)
{
    dprintf(("GDI32: EnumMetaFile STUB"));
    //calling convention differences
//    return O32_EnumMetaFile(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumObjects( HDC arg1, int arg2, GOBJENUMPROC arg3, LPARAM  arg4)
{
    dprintf(("GDI32: EnumObjects STUB"));
    //calling convention differences
//    return O32_EnumObjects(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRgn( HRGN arg1, HRGN  arg2)
{
    dprintf(("GDI32: EqualRgn"));
    return O32_EqualRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API Escape( HDC arg1, int arg2, int arg3, LPCSTR arg4, PVOID  arg5)
{
    dprintf(("GDI32: Escape"));
    return O32_Escape(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExcludeClipRect( HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: ExcludeClipRect"));
    return O32_ExcludeClipRect(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API ExtCreatePen( DWORD arg1, DWORD arg2, const LOGBRUSH * arg3, DWORD arg4, const DWORD *  arg5)
{
    dprintf(("GDI32: ExtCreatePen"));
    return O32_ExtCreatePen(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API ExtCreateRegion( const XFORM * arg1, DWORD arg2, const RGNDATA *  arg3)
{
    dprintf(("GDI32: ExtCreateRegion"));
    return O32_ExtCreateRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtFloodFill( HDC arg1, int arg2, int arg3, COLORREF arg4, UINT  arg5)
{
    dprintf(("GDI32: ExtFloodFill"));
    return O32_ExtFloodFill(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExtSelectClipRgn( HDC arg1, HRGN arg2, int  arg3)
{
    dprintf(("GDI32: ExtSelectClipRgn"));
    return O32_ExtSelectClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillPath( HDC arg1)
{
    dprintf(("GDI32: FillPath"));
    return O32_FillPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillRgn( HDC arg1, HRGN arg2, HBRUSH  arg3)
{
    dprintf(("GDI32: FillRgn"));
    return O32_FillRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlattenPath( HDC arg1)
{
    dprintf(("GDI32: FlattenPath"));
    return O32_FlattenPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FloodFill(HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
    dprintf(("GDI32: FloodFill"));
    return O32_FloodFill(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FrameRgn( HDC arg1, HRGN arg2, HBRUSH arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: FrameRgn"));
    return O32_FrameRgn(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetArcDirection( HDC arg1)
{
    dprintf(("GDI32: GetArcDirection"));
    return O32_GetArcDirection(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetAspectRatioFilterEx( HDC arg1, PSIZE  arg2)
{
    dprintf(("GDI32: GetAspectRatioFilterEx"));
    return O32_GetAspectRatioFilterEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetBitmapBits( HBITMAP arg1, LONG arg2, PVOID  arg3)
{
    dprintf(("GDI32: GetBitmapBits"));
    return O32_GetBitmapBits(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBitmapDimensionEx( HBITMAP arg1, PSIZE  arg2)
{
    dprintf(("GDI32: GetBitmapDimensionEx"));
    return O32_GetBitmapDimensionEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetBkColor( HDC arg1)
{
    dprintf(("GDI32: GetBkColor"));
    return O32_GetBkColor(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetBkMode( HDC arg1)
{
    dprintf(("GDI32: GetBkMode"));
    return O32_GetBkMode(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetBoundsRect( HDC arg1, PRECT arg2, UINT  arg3)
{
    dprintf(("GDI32: GetBoundsRect"));
    return O32_GetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBrushOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: GetBrushOrgEx"));
    return O32_GetBrushOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsA( HDC arg1, UINT arg2, UINT arg3, LPABC arg4)
{
    dprintf(("GDI32: GetCharABCWidthsA"));
    return O32_GetCharABCWidths(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsW( HDC arg1, UINT arg2, UINT arg3, LPABC arg4)
{
    dprintf(("GDI32: GetCharABCWidthsW not properly implemented."));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetCharABCWidths(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthA( HDC arg1, UINT arg2, UINT arg3, PINT  arg4)
{
    dprintf(("GDI32: GetCharWidthA"));
    return O32_GetCharWidth(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//TODO: Cut off Unicode chars?
//******************************************************************************
BOOL WIN32API GetCharWidthW(HDC arg1, UINT iFirstChar, UINT iLastChar, PINT  arg4)
{
    dprintf(("GDI32: GetCharWidthW, not properly implemented"));
    return O32_GetCharWidth(arg1, iFirstChar, iLastChar, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipBox( HDC arg1, PRECT  arg2)
{
 int rc;

  rc = O32_GetClipBox(arg1, arg2);
  dprintf(("GDI32: GetClipBox of %X returned %d\n", arg1, rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: GetClipRgn"));
    return O32_GetClipRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentObject( HDC arg1, UINT arg2)
{
    dprintf(("GDI32: GetCurrentObject"));
    return (HANDLE)O32_GetCurrentObject(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCurrentPositionEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: GetCurrentPositionEx"));
    return O32_GetCurrentPositionEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDIBits( HDC arg1, HBITMAP arg2, UINT arg3, UINT arg4, void * arg5, PBITMAPINFO arg6, UINT  arg7)
{
    dprintf(("GDI32: GetDIBits"));
    return O32_GetDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetDeviceCaps(HDC hdc, int nIndex)
{
 int rc;

    rc = O32_GetDeviceCaps(hdc, nIndex);
    dprintf(("GDI32: GetDeviceCaps %X, %d returned %d\n", hdc, nIndex, rc));
    //SvL: 13-9-'98: NT returns -1 when using 16 bits colors, NOT 65536!
    if(nIndex == NUMCOLORS && rc > 256)
        return -1;
    return(rc);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: GetEnhMetaFileA"));
    return O32_GetEnhMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE  arg3)
{
    dprintf(("GDI32: GetEnhMetaFileBits"));
    return O32_GetEnhMetaFileBits(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileHeader( HENHMETAFILE arg1, UINT arg2, LPENHMETAHEADER  arg3)
{
    dprintf(("GDI32: GetEnhMetaFileHeader"));
    return O32_GetEnhMetaFileHeader(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFilePaletteEntries( HENHMETAFILE arg1, UINT arg2, PPALETTEENTRY  arg3)
{
    dprintf(("GDI32: GetEnhMetaFilePaletteEntries"));
    return O32_GetEnhMetaFilePaletteEntries(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: GetEnhMetaFileW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = O32_GetEnhMetaFile(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
int WIN32API GetGraphicsMode(HDC arg1)
{
    dprintf(("GDI32: GetGraphicsMode"));
    return O32_GetGraphicsMode(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetKerningPairsA( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    dprintf(("GDI32: GetKerningPairsA"));
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetKerningPairsW( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    dprintf(("GDI32: GetKerningPairsW"));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMapMode( HDC arg1)
{
    dprintf(("GDI32: GetMapMode"));
    return O32_GetMapMode(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: GetMetaFileA"));
    return O32_GetMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMetaFileBitsEx( HMETAFILE arg1, UINT arg2, LPVOID  arg3)
{
    dprintf(("GDI32: GetMetaFileBitsEx"));
    return O32_GetMetaFileBitsEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: GetMetaFileW"));
    rc = O32_GetMetaFile(astring);
    FreeAsciiString(astring);
    return rc;

}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMiterLimit( HDC arg1, float * arg2)
{
    dprintf(("GDI32: GetMiterLimit"));
    return O32_GetMiterLimit(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetNearestColor( HDC arg1, COLORREF  arg2)
{
    dprintf(("GDI32: GetNearestColor\n"));
    return O32_GetNearestColor(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetNearestPaletteIndex( HPALETTE arg1, COLORREF  arg2)
{
    UINT rc;
    dprintf(("GDI32: GetNearestPaletteIndex (0x%08X ,0x%08X) ",arg1,arg2));
    rc = O32_GetNearestPaletteIndex(arg1, arg2);
    dprintf(("Returns %d\n",rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsA( HDC arg1, UINT arg2, LPOUTLINETEXTMETRICA  arg3)
{
    dprintf(("GDI32: GetOutlineTextMetricsA"));
    return O32_GetOutlineTextMetrics(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOutlineTextMetricsW( HDC arg1, UINT arg2, LPOUTLINETEXTMETRICW  arg3)
{
    dprintf(("GDI32: GetOutlineTextMetricsW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_GetOutlineTextMetrics(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetPaletteEntries( HPALETTE arg1, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
    dprintf(("GDI32: GetPaletteEntries"));
    return O32_GetPaletteEntries(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetPath( HDC arg1, PPOINT arg2, PBYTE arg3, int  arg4)
{
    dprintf(("GDI32: GetPath"));
    return O32_GetPath(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPolyFillMode( HDC arg1)
{
    dprintf(("GDI32: GetPolyFillMode"));
    return O32_GetPolyFillMode(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetROP2( HDC arg1)
{
    dprintf(("GDI32: GetROP2"));
    return O32_GetROP2(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetRasterizerCaps(LPRASTERIZER_STATUS arg1, UINT  arg2)
{
    dprintf(("GDI32: GetRasterizerCaps"));
    return O32_GetRasterizerCaps(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetRegionData( HRGN arg1, DWORD arg2, PRGNDATA  arg3)
{
    dprintf(("GDI32: GetRegionData"));
    return O32_GetRegionData(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetRgnBox( HRGN arg1, PRECT  arg2)
{
    dprintf(("GDI32: GetRgnBox"));
    return O32_GetRgnBox(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetStretchBltMode( HDC arg1)
{
    dprintf(("GDI32: GetStretchBltMode"));
    return O32_GetStretchBltMode(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemPaletteEntries( HDC arg1, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
 UINT rc;

    dprintf(("GDI32: GetSystemPaletteEntries start %d nr %d pal ptr %X", arg2, arg3, arg4));
    rc = O32_GetSystemPaletteEntries(arg1, arg2, arg3, arg4);
    dprintf((" GetSystemPaletteEntries returned %d", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTextAlign( HDC arg1)
{
    dprintf(("GDI32: GetTextAlign"));
    return O32_GetTextAlign(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextCharacterExtra( HDC arg1)
{
    dprintf(("GDI32: GetTextCharacterExtra"));
    return O32_GetTextCharacterExtra(arg1);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetTextColor( HDC arg1)
{
    dprintf(("GDI32: GetTextColor"));
    return O32_GetTextColor(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32A( HDC arg1, LPCSTR arg2, int arg3, PSIZE  lpSize)
{
    dprintf(("GDI32: GetTextExtentPoint32A"));
    lpSize->cx = lpSize->cy = 0;
    return O32_GetTextExtentPoint32(arg1, arg2, arg3, lpSize);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32W(HDC arg1, LPCWSTR arg2, int arg3, PSIZE lpSize)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 BOOL  rc;

    dprintf(("GDI32: GetTextExtentPoint32W %s\n", astring));
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
  dprintf(("GDI32: GetTextExtentPointW %X %s (size %08xh) returned %d\n", hdc, astring, cbString, rc));
  dprintf(("GDI32: GetTextExtentPointW (%d,%d)\n", lpSize->cx, lpSize->cy));

  FreeAsciiString(astring);
  return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceA( HDC arg1, int arg2, LPSTR  arg3)
{
    dprintf(("GDI32: GetTextFaceA"));
    return O32_GetTextFace(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextFaceW( HDC arg1, int arg2, LPWSTR  arg3)
{
 char *astring = (char *)malloc(arg2+1);
 int   rc;

    dprintf(("GDI32: GetTextFaceW"));
    rc = O32_GetTextFace(arg1, arg2, astring);
    AsciiToUnicode(astring, arg3);
    free(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsA( HDC arg1, LPTEXTMETRICA  arg2)
{
 BOOL rc;

    rc = O32_GetTextMetrics(arg1, arg2);
    dprintf(("GDI32: GetTextMetricsA returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextMetricsW( HDC arg1, LPTEXTMETRICW pwtm)
{
 BOOL rc;
 TEXTMETRICA atm;

    dprintf(("GDI32: GetTextMetricsW"));

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
    dprintf(("GDI32: GetViewportExtEx"));
    return O32_GetViewportExtEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetViewportOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: GetViewportOrgEx"));
    return O32_GetViewportOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetWinMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE arg3, int arg4, HDC arg5)
{
    dprintf(("GDI32: GetWinMetaFileBits"));
    return O32_GetWinMetaFileBits(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWindowOrgEx( HDC arg1, PPOINT  arg2)
{
    dprintf(("GDI32: GetWindowOrgEx"));
    return O32_GetWindowOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetWorldTransform( HDC arg1, LPXFORM  arg2)
{
    dprintf(("GDI32: GetWorldTransform"));
    return O32_GetWorldTransform(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API IntersectClipRect(HDC arg1, int arg2, int arg3, int arg4, int  arg5)
{
 int rc;

  rc = O32_IntersectClipRect(arg1, arg2, arg3, arg4, arg5);
  dprintf(("GDI32: IntersectClipRect returned %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: InvertRgn"));
    return O32_InvertRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LPtoDP( HDC arg1, PPOINT arg2, int  arg3)
{
    dprintf(("GDI32: LPtoDP"));
    return O32_LPtoDP(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API MaskBlt( HDC arg1, int arg2, int arg3, int arg4, int arg5, HDC   arg6, int arg7, int arg8, HBITMAP arg9, int arg10, int arg11, DWORD  arg12)
{
    dprintf(("GDI32: MaskBlt"));
    return O32_MaskBlt(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyWorldTransform( HDC arg1, const XFORM *arg2, DWORD  arg3)
{
    dprintf(("GDI32: ModifyWorldTransform"));
    return O32_ModifyWorldTransform(arg1, (LPXFORM)arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetClipRgn( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OffsetClipRgn"));
    return O32_OffsetClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetRgn( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OffsetRgn"));
    return O32_OffsetRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetViewportOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OffsetViewportOrgEx"));
    return O32_OffsetViewportOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OffsetWindowOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: OffsetWindowOrgEx"));
    return O32_OffsetWindowOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PaintRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: PaintRgn"));
    return O32_PaintRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API PathToRegion( HDC arg1)
{
    dprintf(("GDI32: PathToRegion"));
    return O32_PathToRegion(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Pie(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                  int nBottomRect, int nXRadial1, int nYRadial1, int nXRadial2,
                  int nYRadial2)
{
    dprintf(("GDI32: Pie"));
    //CB: bug in O32_Pie
    if (nXRadial1 == nXRadial2 && nYRadial1 == nYRadial2)
      return O32_Ellipse(hdc,nLeftRect,nTopRect,nRightRect,nBottomRect);
    else
      return O32_Pie(hdc,nLeftRect,nTopRect,nRightRect,nBottomRect,nXRadial1,nYRadial1,nXRadial2,nYRadial2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayEnhMetaFile( HDC arg1, HENHMETAFILE arg2, const RECT *  arg3)
{
    dprintf(("GDI32: PlayEnhMetaFile"));
    return O32_PlayEnhMetaFile(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFile( HDC arg1, HMETAFILE  arg2)
{
    dprintf(("GDI32: PlayMetaFile"));
    return O32_PlayMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFileRecord( HDC arg1, LPHANDLETABLE arg2, LPMETARECORD arg3, UINT  arg4)
{
    dprintf(("GDI32: PlayMetaFileRecord"));
    return O32_PlayMetaFileRecord(arg1, arg2, arg3, (int)arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezier( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    dprintf(("GDI32: PolyBezier"));
    return O32_PolyBezier(arg1, arg2, (int)arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezierTo( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    dprintf(("GDI32: PolyBezierTo"));
    return O32_PolyBezierTo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyDraw( HDC arg1, const POINT * arg2, const BYTE * arg3, DWORD  arg4)
{
    dprintf(("GDI32: PolyDraw"));
    return O32_PolyDraw(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolygon( HDC arg1, const POINT * arg2, const INT * arg3, UINT  arg4)
{
    dprintf(("GDI32: PolyPolygon"));
    return O32_PolyPolygon(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolyline( HDC hdc, const POINT * lppt, const DWORD * lpdwPolyPoints, DWORD cCount)
{
    dprintf(("GDI32: PolyPolyline"));

    return O32_PolyPolyline(hdc,lppt,lpdwPolyPoints,cCount);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polygon( HDC arg1, const POINT * arg2, int  arg3)
{
    dprintf(("GDI32: Polygon"));
    return O32_Polygon(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRegion( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: PtInRegion"));
    return O32_PtInRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtVisible( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: PtVisible"));
    return O32_PtVisible(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectInRegion( HRGN arg1, const RECT *  arg2)
{
    dprintf(("GDI32: RectInRegion"));
    return O32_RectInRegion(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectVisible( HDC arg1, const RECT * arg2)
{
    dprintf(("GDI32: RectVisible\n"));
    return O32_RectVisible(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCA( HDC arg1, const DEVMODEA *  arg2)
{
    dprintf(("GDI32: ResetDCA\n"));
    return (HDC)O32_ResetDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCW( HDC arg1, const DEVMODEW *  arg2)
{
    dprintf(("GDI32: ResetDCW\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return (HDC)O32_ResetDC(arg1, (const DEVMODEA *)arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ResizePalette( HPALETTE arg1, UINT  arg2)
{
    dprintf(("GDI32: ResizePalette\n"));
    return O32_ResizePalette(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RestoreDC( HDC arg1, int  arg2)
{
    dprintf(("GDI32: RestoreDC\n"));
    return O32_RestoreDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RoundRect( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int  arg7)
{
    dprintf(("GDI32: RoundRect"));
    return O32_RoundRect(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
int WIN32API SaveDC( HDC arg1)
{
    dprintf(("GDI32: SaveDC"));
    return O32_SaveDC(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleViewportExtEx( HDC arg1, int arg2, int arg3, int arg4, int arg5, PSIZE  arg6)
{
    dprintf(("GDI32: ScaleViewportExtEx"));
    return O32_ScaleViewportExtEx(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScaleWindowExtEx( HDC arg1, int arg2, int arg3, int arg4, int arg5, PSIZE  arg6)
{
    dprintf(("GDI32: ScaleWindowExtEx"));
    return O32_ScaleWindowExtEx(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
int WIN32API SelectClipRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: SelectClipRgn"));
    return O32_SelectClipRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetArcDirection( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetArcDirection"));
    return O32_SetArcDirection(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SetBitmapBits( HBITMAP arg1, LONG arg2, const VOID *  arg3)
{
    dprintf(("GDI32: SetBitmapBits"));
    return O32_SetBitmapBits(arg1, (DWORD)arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBitmapDimensionEx( HBITMAP arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: SetBitmapDimensionEx"));
    return O32_SetBitmapDimensionEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetBoundsRect( HDC arg1, const RECT * arg2, UINT arg3)
{
    dprintf(("GDI32: SetBoundsRect"));
    return O32_SetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBrushOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
 BOOL rc;

    rc = O32_SetBrushOrgEx(arg1, arg2, arg3, arg4);
    dprintf(("GDI32: SetBrushOrgEx returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetDIBits( HDC arg1, HBITMAP arg2, UINT arg3, UINT arg4, const VOID * arg5, const BITMAPINFO * arg6, UINT  arg7)
{
    dprintf(("GDI32: SetDIBits %x %x %x %x %x %x %x\n", arg1, arg2, arg3, arg4, arg5, arg6, arg7));

    if(DIBSection::getSection() != NULL) {
        DIBSection *dsect;

        dsect = DIBSection::find((DWORD)arg2);
        if(dsect) {
           return dsect->SetDIBits(arg1, arg2, arg3, arg4, arg5, (WINBITMAPINFOHEADER *)&arg6->bmiHeader, arg7);
        }
    }
    return O32_SetDIBits(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
INT WIN32API SetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx, DWORD cy, INT xSrc, INT ySrc, UINT startscan, UINT lines, LPCVOID bits, const BITMAPINFO *info, UINT coloruse)
{
    INT result, imgsize, palsize;
    char *ptr;

    dprintf(("GDI32: SetDIBitsToDevice hdc:%X xDest:%d yDest:%d, cx:%d, cy:%d, xSrc:%d, ySrc:%d, startscan:%d, lines:%d, bits:%X, info%X, coloruse:%d",
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
    dprintf(("GDI32: SetEnhMetaFileBits"));
    return O32_SetEnhMetaFileBits(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetGraphicsMode(HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetGraphicsMode"));
    return O32_SetGraphicsMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetMapMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetMapMode"));
    return O32_SetMapMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SetMapperFlags( HDC arg1, DWORD  arg2)
{
    dprintf(("GDI32: SetMapperFlags"));
    return O32_SetMapperFlags(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API SetMetaFileBitsEx( UINT arg1, const BYTE * arg2)
{
    dprintf(("GDI32: SetMetaFileBitsEx"));
    return O32_SetMetaFileBitsEx(arg1, (PBYTE)arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMiterLimit( HDC arg1, float  arg2, float *  arg3)
{
    dprintf(("GDI32: SetMiterLimit"));
    return O32_SetMiterLimit(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetPaletteEntries( HPALETTE arg1, UINT arg2, UINT arg3, PALETTEENTRY * arg4)
{
    dprintf(("GDI32: SetPaletteEntries"));
    return O32_SetPaletteEntries(arg1, arg2, arg3, (const PALETTEENTRY *)arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetPolyFillMode( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetPolyFillMode"));
    return O32_SetPolyFillMode(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectRgn( HRGN arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: SetRectRgn"));
    return O32_SetRectRgn(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetTextAlign( HDC arg1, UINT  arg2)
{
    dprintf(("GDI32: SetTextAlign"));
    return O32_SetTextAlign(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetTextCharacterExtra( HDC arg1, int  arg2)
{
    dprintf(("GDI32: SetTextCharacterExtra"));
    return O32_SetTextCharacterExtra(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTextJustification( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: SetTextJustification"));
    return O32_SetTextJustification(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportExtEx( HDC arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: SetViewportExtEx"));
    return O32_SetViewportExtEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetViewportOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: SetViewportOrgEx"));
    return O32_SetViewportOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API SetWinMetaFileBits( UINT arg1, const BYTE * arg2, HDC arg3, const METAFILEPICT *  arg4)
{
    dprintf(("GDI32: SetWinMetaFileBits"));
    return O32_SetWinMetaFileBits(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowExtEx( HDC arg1, int arg2, int arg3, PSIZE  arg4)
{
    dprintf(("GDI32: SetWindowExtEx"));
    return O32_SetWindowExtEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWindowOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    dprintf(("GDI32: SetWindowOrgEx"));
    return O32_SetWindowOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetWorldTransform( HDC arg1, const XFORM *arg2)
{
    dprintf(("GDI32: SetWorldTransform"));
    return O32_SetWorldTransform(arg1, (LPXFORM)arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocA( HDC arg1, const DOCINFOA *arg2)
{
    dprintf(("GDI32: StartDocA"));
    return O32_StartDoc(arg1, (LPDOCINFOA)arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocW( HDC arg1, const DOCINFOW *arg2)
{
    dprintf(("GDI32: StartDocW STUB"));
    // NOTE: This will not work as is (needs UNICODE support)
//    return O32_StartDoc(arg1, arg2);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API StartPage( HDC arg1)
{
    dprintf(("GDI32: StartPage"));
    return O32_StartPage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnrealizeObject( HGDIOBJ arg1)
{
    dprintf(("GDI32: UnrealizeObject"));
    return O32_UnrealizeObject(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WidenPath( HDC arg1)
{
    dprintf(("GDI32: WidenPath"));
    return O32_WidenPath(arg1);
}
//******************************************************************************
//CreateDisardableBitmap is obsolete and can be replaced by CreateCompatibleBitmap
//******************************************************************************
HBITMAP WIN32API CreateDiscardableBitmap(HDC hDC, int nWidth, int nHeight)
{
    dprintf(("GDI32: CreateDisardableBitmap\n"));
    return O32_CreateCompatibleBitmap(hDC, nWidth, nHeight);
}
//******************************************************************************
//TODO: Not implemented
//******************************************************************************
int WIN32API SetAbortProc(HDC hdc, ABORTPROC lpAbortProc)
{
  dprintf(("GDI32: SetAbortProc - stub (1)w\n"));
  return(1);
}
//******************************************************************************
//Selects the current path as a clipping region for a device context, combining
//any existing clipping region by using the specified mode
//TODO: Can be emulated with SelectClipRegion??
//******************************************************************************
BOOL WIN32API SelectClipPath(HDC hdc, int iMode)
{
  dprintf(("GDI32: SelectClipPath, not implemented!(TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//TODO: Sets the color adjustment values for a device context. (used to adjust
//      the input color of the src bitmap for calls of StretchBlt & StretchDIBits
//      functions when HALFTONE mode is set
//******************************************************************************
BOOL WIN32API SetColorAdjustment(HDC hdc, CONST COLORADJUSTMENT *lpca)
{
  dprintf(("GDI32: SetColorAdjustment, not implemented!(TRUE)\n"));
  return(TRUE);
}
//******************************************************************************
//WINE: OBJECTS\DIB.C
//*********************************************************************************
HBITMAP WIN32API CreateDIBSection( HDC hdc, BITMAPINFO *pbmi, UINT iUsage,
                                   VOID **ppvBits, HANDLE hSection, DWORD dwOffset)
{
 HBITMAP res = 0;
 BOOL    fFlip = 0;
 int iHeight, iWidth;

  dprintf(("GDI32: CreateDIBSection %x %x %x %d", hdc, iUsage, hSection, dwOffset));
  if(hSection)
  {
    dprintf(("GDI32: CreateDIBSection, hSection != NULL, not supported!\n"));
    return NULL;
  }

  //SvL: 13-9-98: StarCraft uses bitmap with negative height
  iWidth = pbmi->bmiHeader.biWidth;
  if(pbmi->bmiHeader.biWidth < 0)
  {
    pbmi->bmiHeader.biWidth = -pbmi->bmiHeader.biWidth;
    fFlip = FLIP_HOR;
  }
  iHeight = pbmi->bmiHeader.biHeight;
  if(pbmi->bmiHeader.biHeight < 0)
  {
    pbmi->bmiHeader.biHeight = -pbmi->bmiHeader.biHeight;
    fFlip |= FLIP_VERT;
  }

  res = O32_CreateDIBitmap(hdc, &pbmi->bmiHeader, 0, NULL, pbmi, 0);
  if (res)
  {
    ULONG Pal[256];
    char PalSize;
    LOGPALETTE tmpPal = { 0x300,1,{0,0,0,0}};
    HPALETTE hpalCur, hpalTmp;
    DIBSection *dsect = new DIBSection((WINBITMAPINFOHEADER *)&pbmi->bmiHeader, (DWORD)res, fFlip);

    if(NULL!=dsect)
    {
      PalSize = dsect->GetBitCount();
      if(PalSize<=8)
      {
        // Now get the current Palette from the DC
        hpalTmp = CreatePalette(&tmpPal);
        hpalCur = SelectPalette(hdc, hpalTmp, FALSE);

        // and use it to set the DIBColorTable
        GetPaletteEntries( hpalCur, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
        dsect->SetDIBColorTable(0, 1<< PalSize, (RGBQUAD*)&Pal);

        // Restore the DC Palette
        SelectPalette(hdc,hpalCur,FALSE);
        DeleteObject(hpalTmp);
      }
      // Set the hdc in the DIBSection so we can update the palete if a new
      // Paletet etc. gets selected into the DC.

      dsect->SelectDIBObject(hdc);

      if(ppvBits!=NULL)
        *ppvBits = dsect->GetDIBObject();

      pbmi->bmiHeader.biWidth = iWidth;
      pbmi->bmiHeader.biHeight = iHeight;

      dprintf(("GDI32: return %08X\n",res));
      return(res);
    }
  }

  /* Error.  */
  if (res)
    DeleteObject(res);
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
UINT WIN32API GetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries,
                                  RGBQUAD *pColors)
{
 HPALETTE hpal = O32_GetCurrentObject(hdc, OBJ_PAL);
 UINT rc;
 int i;

  rc = O32_GetPaletteEntries(hpal,
                         uStartIndex,
                         cEntries,
                         (PALETTEENTRY *)pColors);
  for(i=0;
      i<cEntries;
      i++)
  {
    BYTE tmp;
    tmp = pColors[i].rgbBlue;
    pColors[i].rgbBlue = pColors[i].rgbRed;
    pColors[i].rgbRed = tmp;
    pColors[i].rgbReserved = 0;
  }
  dprintf(("GDI32: GetDIBColorTable returns %d\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetDIBColorTable(HDC hdc, UINT uStartIndex, UINT cEntries,
                                  RGBQUAD *pColors)
{
 DIBSection *dsect = DIBSection::findHDC(hdc);

  dprintf(("GDI32: SetDIBColorTable\n"));
  if(dsect)
  {
    return(dsect->SetDIBColorTable(uStartIndex, cEntries, pColors));
  }
  else
    return(0);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreateHalftonePalette(HDC hdc)
{
    dprintf(("GDI32: CreateHalftonePalette, not implemented\n"));
    return(NULL);
}
//******************************************************************************
//Maps colors to system palette; faster way to update window (instead of redrawing)
//We just redraw
//******************************************************************************
BOOL WIN32API UpdateColors(HDC hdc)
{
  dprintf(("GDI32: UpdateColors\n"));
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
int WIN32API DescribePixelFormat(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR)
{
  dprintf(("GDI32: DescribePixelFormat, not implemented (GDI_ERROR)\n"));
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
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *             Markus Montkowski [Wen, 1999/01/12 20:18]
 *****************************************************************************/

BOOL WIN32API CombineTransform(LPXFORM lLPXFORMResult,
                                  CONST   XFORM *lLPXFORM1,
                                  CONST   XFORM *lLPXFORM2)
{
  dprintf(("GDI32: CombineTransform(%08xh,%08xh,%08xh).\n",
           lLPXFORMResult,
           lLPXFORM1,
           lLPXFORM2));

  XFORM xfrm;
  if( O32_IsBadWritePtr( (void*)lLPXFORMResult, sizeof(XFORM)) ||
      O32_IsBadReadPtr(  (void*)lLPXFORM1, sizeof(XFORM)) ||
      O32_IsBadWritePtr( (void*)lLPXFORM2, sizeof(XFORM)) )
  return (FALSE);

  // Add the translations
  lLPXFORMResult->eDx = lLPXFORM1->eDx + lLPXFORM2->eDx;
  lLPXFORMResult->eDy = lLPXFORM1->eDy + lLPXFORM2->eDy;

  // Multiply the matrixes
  xfrm.eM11 = lLPXFORM1->eM11 * lLPXFORM2->eM11 + lLPXFORM1->eM21 * lLPXFORM1->eM12;
  xfrm.eM12 = lLPXFORM1->eM11 * lLPXFORM2->eM12 + lLPXFORM1->eM12 * lLPXFORM1->eM22;
  xfrm.eM21 = lLPXFORM1->eM21 * lLPXFORM2->eM11 + lLPXFORM1->eM22 * lLPXFORM1->eM21;
  xfrm.eM22 = lLPXFORM1->eM21 * lLPXFORM2->eM12 + lLPXFORM1->eM22 * lLPXFORM1->eM22;

  // Now copy to resulting XFROM as the pt must not be distinct
  lLPXFORMResult->eM11 = xfrm.eM11;
  lLPXFORMResult->eM12 = xfrm.eM12;
  lLPXFORMResult->eM21 = xfrm.eM21;
  lLPXFORMResult->eM22 = xfrm.eM22;

  return (TRUE);
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
 * Parameters: HGLOBAL hglbDIBPacked Identifies a global memory object containing
 *             a packed DIB, which consists of a BITMAPINFO structure immediately
 *             followed by an array of bytes defining the pixels of the bitmap.
 *             UINT    fuColorSpec   color table data
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *             Markus Montkowski [Wen, 1999/01/12 20:00]
 *****************************************************************************/

HBRUSH WIN32API CreateDIBPatternBrush( HGLOBAL hglbDIBPacked,
                                       UINT    fuColorSpec)
{
  LPVOID lpMem;
  HBRUSH ret = 0;
  dprintf(("GDI32: CreateDIBPatternBrush(%08xh, %08xh) \n",
           hglbDIBPacked,
           fuColorSpec));

  lpMem = GlobalLock(hglbDIBPacked);
  if(NULL!=lpMem)
  {

    ret = CreateDIBPatternBrushPt( lpMem,
                                   fuColorSpec);
    GlobalUnlock(hglbDIBPacked);
  }

  return (ret);
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



