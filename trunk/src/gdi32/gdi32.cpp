/* $Id: gdi32.cpp,v 1.88 2003-11-12 14:12:01 sandervl Exp $ */

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
#include <odinwrap.h>
#include <misc.h>
#include "callback.h"
#include "unicode.h"
#include "dibsect.h"
#include <codepage.h>
#include "oslibgpi.h"
#include "oslibgdi.h"
#include <dcdata.h>
#include <winuser32.h>
#include "font.h"
#include <stats.h>
#include <objhandle.h>

#define DBG_LOCALLOG    DBG_gdi32
#include "dbglocal.h"

ODINDEBUGCHANNEL(GDI32-GDI32)

//******************************************************************************
//******************************************************************************
int WIN32API FillRect(HDC hDC, const RECT * lprc, HBRUSH hbr)
{
    int ret;

    //SvL: brush 0 means current selected brush (verified in NT4)
    if(hbr == 0) {
        hbr = GetCurrentObject(hDC, OBJ_BRUSH);
    }
    else
    if (hbr <= (HBRUSH) (COLOR_MAX + 1)) {
	hbr = GetSysColorBrush( (INT) hbr - 1 );
    }
    dprintf(("USER32:  FillRect %x (%d,%d)(%d,%d) brush %X", hDC, lprc->left, lprc->top, lprc->right, lprc->bottom, hbr));
    ret = O32_FillRect(hDC,lprc,hbr);
    return ret;
}
//******************************************************************************
//******************************************************************************
int WIN32API FrameRect( HDC hDC, const RECT * lprc, HBRUSH  hbr)
{
    int ret;

    dprintf(("USER32: FrameRect %x (%d,%d)(%d,%d) brush %x", hDC, lprc->top, lprc->left, lprc->bottom, lprc->right, hbr));
    ret = O32_FrameRect(hDC,lprc,hbr);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRect( HDC hDC, const RECT * lprc)
{
    int ret;

    if(lprc) {
         dprintf(("USER32: InvertRect %x (%d,%d)(%d,%d)", hDC, lprc->left, lprc->top, lprc->right, lprc->bottom));
    }
    else dprintf(("USER32: InvertRect %x NULL", hDC));
    ret = O32_InvertRect(hDC,lprc);
    return ret;
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetBkColor(HDC hdc, COLORREF crColor)
{
  return(O32_SetBkColor(hdc, crColor));
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetTextColor(HDC hdc, COLORREF crColor)
{
  return O32_SetTextColor(hdc, crColor);
}
//******************************************************************************
//******************************************************************************
HGDIOBJ WIN32API GetStockObject(int arg1)
{
 HGDIOBJ obj;

    switch(arg1)
    {
        case DEFAULT_GUI_FONT:
                if(NULL==hFntDefaultGui) {
                    hFntDefaultGui = CreateFontA( -9, 0, 0, 0, FW_MEDIUM, FALSE,
                                                 FALSE, FALSE, ANSI_CHARSET,
                                                 OUT_DEFAULT_PRECIS,
                                                 CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY,
                                                 FIXED_PITCH|FF_MODERN, "WarpSans");
                    //This object can't be deleted by applications
                    ObjSetHandleFlag(hFntDefaultGui, OBJHANDLE_FLAG_NODELETE, TRUE);
                }
                obj = hFntDefaultGui;
                break;
        default:
                obj = O32_GetStockObject(arg1);
                break;
    }
    return(obj);
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
{
 HPEN hPen;

    //CB: todo: PS_DOT is different in Win32 (. . . . and not - - - -)
    //    Open32 looks like LINETYPE_SHORTDASH instead of LINETYPE_DOT!!!
    //    -> difficult to fix without performance decrease!

    hPen = O32_CreatePen(fnPenStyle,nWidth,crColor);
    if(hPen) STATS_CreatePen(hPen, fnPenStyle,nWidth,crColor);
    return hPen;
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API CreatePenIndirect(const LOGPEN * lplgpn)
{
 HPEN hPen;

    hPen = O32_CreatePenIndirect(lplgpn);
    if(hPen) STATS_CreatePenIndirect(hPen, lplgpn);
    return hPen;
}
//******************************************************************************
//******************************************************************************
HPEN WIN32API ExtCreatePen(DWORD dwPenStyle, DWORD dwWidth, const LOGBRUSH *lplb, 
                           DWORD dwStyleCount, const DWORD *lpStyle)
{
 HPEN hPen;

    hPen = O32_ExtCreatePen(dwPenStyle, dwWidth, lplb, dwStyleCount, lpStyle);
    if(hPen) STATS_ExtCreatePen(hPen, dwPenStyle, dwWidth, lplb, dwStyleCount, lpStyle);
    return hPen;
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreatePatternBrush(HBITMAP hBitmap)
{
 HBRUSH hBrush;

    hBrush = O32_CreatePatternBrush(hBitmap);
    if(hBrush) STATS_CreatePatternBrush(hBrush, hBitmap);
    return(hBrush);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateSolidBrush(COLORREF color)
{
    HBRUSH hBrush;

    hBrush = O32_CreateSolidBrush(color);
    if(hBrush) STATS_CreateSolidBrush(hBrush, color);
    return(hBrush);
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateBrushIndirect( const LOGBRUSH *pLogBrush)
{
    HBRUSH hBrush;

    hBrush = O32_CreateBrushIndirect((LPLOGBRUSH)pLogBrush);
    dprintf(("GDI32: CreateBrushIndirect %x %x %x returned %x", pLogBrush->lbStyle, pLogBrush->lbColor, pLogBrush->lbHatch, hBrush));
    if(hBrush) STATS_CreateBrushIndirect(hBrush, (LPLOGBRUSH)pLogBrush);
    return hBrush;
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateHatchBrush(int fnStyle, COLORREF clrref)
{
 HBRUSH hBrush;
 
    hBrush = O32_CreateHatchBrush(fnStyle, clrref);
    if(hBrush) STATS_CreateHatchBrush(hBrush, fnStyle, clrref);
    return hBrush;
}
//******************************************************************************
//******************************************************************************
HBRUSH WIN32API CreateDIBPatternBrushPt( const VOID * buffer, UINT usage)
{
 HBRUSH hBrush;

    hBrush = O32_CreateDIBPatternBrushPt(buffer, usage);
    if(hBrush) STATS_CreateDIBPatternBrushPt(hBrush, buffer, usage);
    return hBrush;
}
/*****************************************************************************
 * Name      : HBRUSH CreateDIBPatternBrush
 * Purpose   : The CreateDIBPatternBrush function creates a logical brush that
 *             has the pattern specified by the specified device-independent
 *             bitmap (DIB). The brush can subsequently be selected into any
 *             device context that is associated with a device that supports
 *             raster operations.
 *
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
 * Status    : ODIN32 COMPLETELY UNTESTED
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *             Markus Montkowski [Wen, 1999/01/12 20:00]
 *****************************************************************************/

HBRUSH WIN32API CreateDIBPatternBrush( HGLOBAL hglbDIBPacked,
                                       UINT    fuColorSpec)
{
  BITMAPINFO *lpMem;
  HBRUSH ret = 0;

  lpMem = (BITMAPINFO *)GlobalLock(hglbDIBPacked);
  if(NULL!=lpMem)
  {
      dprintf(("GDI32: CreateDIBPatternBrush (%08xh, %08xh) %x (%d,%d) bpp %d",
                hglbDIBPacked, fuColorSpec, lpMem, lpMem->bmiHeader.biWidth, 
                lpMem->bmiHeader.biHeight, lpMem->bmiHeader.biBitCount));

      ret = CreateDIBPatternBrushPt( lpMem,
                                    fuColorSpec);
      GlobalUnlock(hglbDIBPacked);
  }
  else {
      dprintf(("!ERROR!: CreateDIBPatternBrush (%08xh, %08xh) -> INVALID memory handle!!",
                hglbDIBPacked, fuColorSpec));
  }
  return (ret);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateCompatibleDC( HDC hdc)
{
 HDC newHdc;

    newHdc = O32_CreateCompatibleDC(hdc);
    ULONG oldcp = OSLibGpiQueryCp(hdc);
    if (!oldcp) /* If new DC is to be created */
        oldcp = GetDisplayCodepage();

    if(newHdc) STATS_CreateCompatibleDC(hdc, newHdc);
    OSLibGpiSetCp(newHdc, oldcp);
    //PF Open32 seems not to move coordinates to 0,0 in newHdc
    MoveToEx(newHdc, 0, 0 , NULL);

    return newHdc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteDC(HDC hdc)
{
  pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
  if(!pHps)
  {
      dprintf(("WARNING: DeleteDC %x; invalid hdc!", hdc));
      SetLastError(ERROR_INVALID_HANDLE);
      return 0;
  }
  SetLastError(ERROR_SUCCESS);

  DIBSection *dsect = DIBSection::findHDC(hdc);
  if(dsect)
  {
      //remove previously selected dibsection
      dprintf(("DeleteDC %x, unselect DIB section %x", hdc, dsect->GetBitmapHandle()));
      dsect->UnSelectDIBObject();
  }

  //Must call ReleaseDC for window dcs
  if(pHps->hdcType == TYPE_1) {
      return ReleaseDC(OS2ToWin32Handle(pHps->hwnd), hdc);
  }

  STATS_DeleteDC(hdc);
  return O32_DeleteDC(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StrokeAndFillPath(HDC hdc)
{
    return O32_StrokeAndFillPath(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API StrokePath(HDC hdc)
{
    return O32_StrokePath(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetBkMode( HDC hdc, int mode)
{
    return O32_SetBkMode(hdc, mode);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetPixel( HDC hdc, int x, int y)
{
    return O32_GetPixel(hdc, x, y);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API SetPixel( HDC hdc, int x, int y, COLORREF color)
{
    return O32_SetPixel(hdc, x, y, color);
}
//******************************************************************************
//Faster version of SetPixel (since it doesn't need to return the original color)
//Just use SetPixel for now
//******************************************************************************
BOOL WIN32API SetPixelV(HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
 COLORREF rc;

    rc = O32_SetPixel(arg1, arg2, arg3, arg4);
    if(rc == GDI_ERROR) // || rc == COLOR_INVALID)
        return(FALSE);
    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetDCOrgEx(HDC hdc, PPOINT lpPoint)
{
    if(lpPoint == NULL) {
        dprintf(("WARNING: GDI32: GetDCOrgEx %x NULL", hdc));
        return FALSE;
    }
    return O32_GetDCOrgEx(hdc, lpPoint);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AbortPath(HDC hdc)
{
    return O32_AbortPath(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AngleArc( HDC arg1, int arg2, int arg3, DWORD arg4, float  arg5, float  arg6)
{
    return O32_AngleArc(arg1, arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Arc( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int  arg9)
{
    return O32_Arc(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ArcTo( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    return O32_ArcTo(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API BeginPath(HDC hdc)
{
    return O32_BeginPath(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Chord( HDC arg1, int arg2, int arg3, int arg4, int arg5, int  arg6, int  arg7, int  arg8, int  arg9)
{
    return O32_Chord(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseFigure(HDC hdc)
{
    return O32_CloseFigure(hdc);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateDCA(LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput, const DEVMODEA *lpInitData)
{
    HDC hdc;

    // 2001-05-28 PH
    // Ziff Davis Benchmarks come in here with "display".
    // Obviously, Windows does accept case-insensitive driver names,
    // whereas Open32 doesn't.
    if (*lpszDriver == 'd') // quick check
    {
        // then do a double-check and use the uppercase constant
        // instead
        if (stricmp(lpszDriver, "DISPLAY") == 0)
            lpszDriver = "DISPLAY";
    }

    hdc = O32_CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);
    if(hdc) {
        OSLibGpiSetCp(hdc, GetDisplayCodepage());
        STATS_CreateDCA(hdc, lpszDriver, lpszDevice, lpszOutput, lpInitData);
    }

    dprintf(("GDI32: CreateDCA %s %s %s %x returned %x", lpszDriver, lpszDevice, lpszOutput, lpInitData, hdc));
    return hdc;
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

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
#if (__IBMCPP__ == 360)
      devmode.dmOrientation   = arg4->dmOrientation;
      devmode.dmPaperSize     = arg4->dmPaperSize;
      devmode.dmPaperLength   = arg4->dmPaperLength;
      devmode.dmPaperWidth    = arg4->dmPaperWidth; 
#else
      devmode.s1.dmOrientation   = arg4->s1.dmOrientation;
      devmode.s1.dmPaperSize     = arg4->s1.dmPaperSize;
      devmode.s1.dmPaperLength   = arg4->s1.dmPaperLength;
      devmode.s1.dmPaperWidth    = arg4->s1.dmPaperWidth; 
#endif
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
      rc = CreateDCA(astring1,astring2,astring3,&devmode);
    }
    else
      rc = CreateDCA(astring1,astring2,astring3, NULL);

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
HDC WIN32API CreateICA(LPCSTR lpszDriver, LPCSTR lpszDevice, LPCSTR lpszOutput,
                       const DEVMODEA *lpdvmInit)
{
 static char *szDisplay = "DISPLAY";
 HDC          hdc;

    //SvL: Open32 tests for "DISPLAY"
    if(lpszDriver && !strcmp(lpszDriver, "display")) {
        lpszDriver = szDisplay;
    }
    //SvL: Open32 tests lpszDriver for NULL even though it's ignored
    if(lpszDriver == NULL) {
        lpszDriver = lpszDevice;
    }
    hdc = O32_CreateIC(lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    if(hdc) STATS_CreateICA(hdc, lpszDriver, lpszDevice, lpszOutput, lpdvmInit);
    return hdc;
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

    if(arg4)
    {
      strcpy((char*)devmode.dmDeviceName, astring4);
      strcpy((char*)devmode.dmFormName, astring5);

      devmode.dmSpecVersion      = arg4->dmSpecVersion;
      devmode.dmDriverVersion    = arg4->dmDriverVersion;
      devmode.dmSize             = arg4->dmSize;
      devmode.dmDriverExtra      = arg4->dmDriverExtra;
      devmode.dmFields           = arg4->dmFields;
#if (__IBMCPP__ == 360)
      devmode.dmOrientation      = arg4->dmOrientation;
      devmode.dmPaperSize        = arg4->dmPaperSize;
      devmode.dmPaperLength      = arg4->dmPaperLength;
      devmode.dmPaperWidth       = arg4->dmPaperWidth; 
#else
      devmode.s1.dmOrientation   = arg4->s1.dmOrientation;
      devmode.s1.dmPaperSize     = arg4->s1.dmPaperSize;
      devmode.s1.dmPaperLength   = arg4->s1.dmPaperLength;
      devmode.s1.dmPaperWidth    = arg4->s1.dmPaperWidth;
#endif
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

      rc = CreateICA(astring1,astring2,astring3,&devmode);
    }
    else
      rc = CreateICA(astring1,astring2,astring3, NULL);

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
BOOL WIN32API Ellipse(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                      int nBottomRect)
{
    dprintf(("GDI32: Ellipse %x (%d,%d)(%d,%d)", nLeftRect, nTopRect, nRightRect, nBottomRect));
    return O32_Ellipse(hdc, nLeftRect, nTopRect, nRightRect, nBottomRect);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EndPath( HDC hdc)
{
    return O32_EndPath(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Rectangle(HDC hdc, int left, int top, int right, int bottom)
{
    return O32_Rectangle(hdc, left, top, right, bottom);
}
//******************************************************************************
//******************************************************************************
#ifdef DEBUG
VOID dumpROP2(INT rop2)
{
  CHAR *name;

  switch (rop2)
  {
    case R2_BLACK:
      name = "R2_BLACK";
      break;

    case R2_COPYPEN:
      name = "R2_COPYPEN";
      break;

    case R2_MASKNOTPEN:
      name = "R2_MASKNOTPEN";
      break;

    case R2_MASKPEN:
      name = "R2_MASKPEN";
      break;

    case R2_MASKPENNOT:
      name = "R2_MASKPENNOT";
      break;

    case R2_MERGENOTPEN:
      name = "R2_MERGENOTPEN";
      break;

    case R2_MERGEPEN:
      name = "R2_MERGEPEN";
      break;

    case R2_MERGEPENNOT:
      name = "R2_MERGEPENNOT";
      break;

    case R2_NOP:
      name = "R2_NOP";
      break;

    case R2_NOT:
      name = "R2_NOT";
      break;

    case R2_NOTCOPYPEN:
      name = "R2_NOTCOPYPEN";
      break;

    case R2_NOTMASKPEN:
      name = "R2_NOTMASKPEN";
      break;

    case R2_NOTMERGEPEN:
      name = "R2_NOTMERGEPEN";
      break;

    case R2_WHITE:
      name = "R2_WHITE";
      break;

    case R2_XORPEN:
      name = "R2_XORPEN";
      break;

    case R2_NOTXORPEN:
      name = "R2_NOTXORPEN";
      break;

    default:
      name = "unknown mode!!!";
      break;
  }

  dprintf(("  ROP2 mode = %s",name));
}
#endif
//******************************************************************************
//******************************************************************************
int WIN32API SetROP2( HDC hdc, int rop2)
{
    #ifdef DEBUG
    dumpROP2(rop2);
    #endif
    return O32_SetROP2(hdc, rop2);
}
//******************************************************************************
//******************************************************************************
int WIN32API Escape( HDC hdc, int nEscape, int cbInput, LPCSTR lpvInData, PVOID lpvOutData)
{
 int rc;

#ifdef DEBUG
  if(cbInput && lpvInData) {
        ULONG *tmp = (ULONG *)lpvInData;
        for(int i=0;i<cbInput/4;i++) {
                dprintf(("GDI32: Escape par %d: %x", i, *tmp++));
        }
  }
#endif
    rc = O32_Escape(hdc, nEscape, cbInput, lpvInData, lpvOutData);
    if(rc == 0) {
         dprintf(("GDI32: Escape %x %d %d %x %x returned %d (WARNING: might not be implemented!!) ", hdc, nEscape, cbInput, lpvInData, lpvOutData, rc));
    }
    else dprintf(("GDI32: Escape %x %d %d %x %x returned %d ", hdc, nEscape, cbInput, lpvInData, lpvOutData, rc));

    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtFloodFill( HDC arg1, int arg2, int arg3, COLORREF arg4, UINT  arg5)
{
    return O32_ExtFloodFill(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillPath( HDC arg1)
{
    return O32_FillPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlattenPath( HDC arg1)
{
    return O32_FlattenPath(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FloodFill(HDC arg1, int arg2, int arg3, COLORREF  arg4)
{
    return O32_FloodFill(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetArcDirection( HDC arg1)
{
    return O32_GetArcDirection(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetAspectRatioFilterEx( HDC arg1, PSIZE  arg2)
{
    return O32_GetAspectRatioFilterEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetBkColor(HDC hdc)
{
    return O32_GetBkColor(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetBkMode(HDC hdc)
{
    return O32_GetBkMode(hdc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetBoundsRect( HDC arg1, PRECT arg2, UINT  arg3)
{
    return O32_GetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBrushOrgEx( HDC arg1, PPOINT  arg2)
{
    return O32_GetBrushOrgEx(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsA( HDC arg1, UINT arg2, UINT arg3, LPABC arg4)
{
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
BOOL WIN32API GetCharWidth32A( HDC hdc, UINT iFirstChar, UINT iLastChar, PINT pWidthArray)
{
 BOOL ret;

    dprintf(("GDI32: GetCharWidth32A %x %x %x %x", hdc, iFirstChar, iLastChar, pWidthArray));
    ret = O32_GetCharWidth(hdc, iFirstChar, iLastChar, pWidthArray);
    dprintf(("GDI32: GetCharWidth32A returned %d", ret));
#ifdef DEBUG
    if(ret) {
      for(int i=iFirstChar;i<iLastChar;i++) {
          if((i >= 'a' && i <= 'z') || (i >= 'A' && i <= 'Z')) {
                dprintf2(("Char %c -> width %d", i, pWidthArray[i]));
          } 
          else  dprintf2(("Char %x -> width %d", i, pWidthArray[i]));
      }
    }
#endif
    return ret;
}
//******************************************************************************
//TODO: Cut off Unicode chars?
//******************************************************************************
BOOL WIN32API GetCharWidth32W(HDC hdc, UINT iFirstChar, UINT iLastChar, PINT pWidthArray)
{
    dprintf(("GDI32: GetCharWidth32W might not work properly %x %x %x %x", hdc, iFirstChar, iLastChar, pWidthArray));
    return O32_GetCharWidth(hdc, iFirstChar, iLastChar, pWidthArray);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCurrentPositionEx( HDC hdc, PPOINT lpPoint)
{
 BOOL rc;

    rc = O32_GetCurrentPositionEx(hdc, lpPoint);
    dprintf(("GDI32: GetCurrentPositionEx returned %d (%d,%d)", rc, lpPoint->x, lpPoint->y));
    return rc;
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
DWORD WIN32API GetKerningPairsA( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetKerningPairsW( HDC arg1, DWORD arg2, LPKERNINGPAIR  arg3)
{
    dprintf(("GDI32: GetKerningPairsW; might not work"));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_GetKerningPairs(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMiterLimit( HDC arg1, float * arg2)
{
    return O32_GetMiterLimit(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetNearestColor( HDC arg1, COLORREF  arg2)
{
    return O32_GetNearestColor(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetPath( HDC hdc, PPOINT arg2, PBYTE arg3, int  arg4)
{
    return O32_GetPath(hdc, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPolyFillMode( HDC hdc)
{
    return O32_GetPolyFillMode(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetROP2( HDC hdc)
{
    return O32_GetROP2(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetRasterizerCaps(LPRASTERIZER_STATUS arg1, UINT  arg2)
{
    return O32_GetRasterizerCaps(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetTextAlign( HDC hdc)
{
    return O32_GetTextAlign(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetTextCharacterExtra( HDC hdc)
{
    return O32_GetTextCharacterExtra(hdc);
}
//******************************************************************************
//******************************************************************************
COLORREF WIN32API GetTextColor( HDC hdc)
{
    return O32_GetTextColor(hdc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Pie(HDC hdc, int nLeftRect, int nTopRect, int nRightRect,
                  int nBottomRect, int nXRadial1, int nYRadial1, int nXRadial2,
                  int nYRadial2)
{
    dprintf(("GDI32: Pie %x (%d,%d)(%d,%d) (%d,%d) (%d,%d)", hdc, nLeftRect, nTopRect, nRightRect,
              nBottomRect, nXRadial1, nYRadial1, nXRadial2, nYRadial2));

    //CB: bug in O32_Pie
    if (nXRadial1 == nXRadial2 && nYRadial1 == nYRadial2)
      return O32_Ellipse(hdc,nLeftRect,nTopRect,nRightRect,nBottomRect);
    else
      return O32_Pie(hdc,nLeftRect,nTopRect,nRightRect,nBottomRect,nXRadial1,nYRadial1,nXRadial2,nYRadial2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezier( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    return O32_PolyBezier(arg1, arg2, (int)arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyBezierTo( HDC arg1, const POINT * arg2, DWORD  arg3)
{
    return O32_PolyBezierTo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyDraw( HDC arg1, const POINT * arg2, const BYTE * arg3, DWORD  arg4)
{
    return O32_PolyDraw(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolygon( HDC arg1, const POINT * arg2, const INT * arg3, UINT  arg4)
{
    return O32_PolyPolygon(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyPolyline( HDC hdc, const POINT * lppt, const DWORD * lpdwPolyPoints, DWORD cCount)
{
    return O32_PolyPolyline(hdc,lppt,lpdwPolyPoints,cCount);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polygon( HDC hdc, const POINT *lpPoints, int count)
{
    return O32_Polygon(hdc, lpPoints, count);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtVisible( HDC hdc, int x, int  y)
{
    dprintf(("GDI32: PtVisible %x (%d,%d)", hdc, x, y));
    return O32_PtVisible(hdc, x, y);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectVisible( HDC hdc, const RECT *lpRect)
{
    if(lpRect == NULL) {
       dprintf(("WARNING: GDI32: RectVisible %x lpRect == NULL!"));
       return FALSE;
    }    
    dprintf(("GDI32: RectVisible %x (%d,%d)(%d,%d)", hdc, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom));
    return O32_RectVisible(hdc, lpRect);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCA( HDC arg1, const DEVMODEA *  arg2)
{
    return (HDC)O32_ResetDC(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API ResetDCW( HDC arg1, const DEVMODEW *  arg2)
{
    dprintf(("GDI32: ResetDCW: not properly implemented"));
    DebugInt3();
    // NOTE: This will not work as is (needs UNICODE support)
    return (HDC)O32_ResetDC(arg1, (const DEVMODEA *)arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RestoreDC(HDC hdc, int id)
{
 BOOL ret;

    ret = O32_RestoreDC(hdc, id);
    if(ret == FALSE) {
        dprintf(("ERROR: GDI32: RestoreDC %x %d FAILED", hdc, id));
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RoundRect( HDC arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int  arg7)
{
    return O32_RoundRect(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
int WIN32API SaveDC( HDC hdc)
{
 int id;

    id = O32_SaveDC(hdc);
    if(id == 0) {
         dprintf(("ERROR: GDI32: SaveDC %x FAILED", hdc));
    }
    else dprintf(("GDI32: SaveDC %x returned %d", hdc, id));
    return id;
}
//******************************************************************************
//******************************************************************************
int WIN32API SetArcDirection( HDC arg1, int  arg2)
{
    return O32_SetArcDirection(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetBoundsRect( HDC arg1, const RECT * arg2, UINT arg3)
{
    return O32_SetBoundsRect(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetBrushOrgEx( HDC arg1, int arg2, int arg3, PPOINT  arg4)
{
    return O32_SetBrushOrgEx(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SetMapperFlags(HDC hdc, DWORD dwFlag)
{
    return O32_SetMapperFlags(hdc, dwFlag);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMiterLimit(HDC hdc, float eNewLimit, float* peOldLimit)
{
    return O32_SetMiterLimit(hdc, eNewLimit, peOldLimit);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetPolyFillMode(HDC hdc, int iPolyFillMode)
{
    return O32_SetPolyFillMode(hdc, iPolyFillMode);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetTextAlign(HDC hdc, UINT fMode)
{
    return O32_SetTextAlign(hdc, fMode);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetTextCharacterExtra(HDC hdc, int nCharExtra)
{
    return O32_SetTextCharacterExtra(hdc, nCharExtra);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetTextJustification(HDC hdc, int nBreakExtra, int nBreakCount)
{
    return O32_SetTextJustification(hdc, nBreakExtra, nBreakCount);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WidenPath( HDC hdc)
{
    return O32_WidenPath(hdc);
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
//Maps colors to system palette; faster way to update window (instead of redrawing)
//We just redraw
//******************************************************************************
BOOL WIN32API UpdateColors(HDC hdc)
{
  return InvalidateRect(WindowFromDC(hdc), NULL, FALSE);
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
  dprintf(("GDI32: GdiComment %x %d %x NOT IMPLEMENTED", hdc, cbSize, lpData));
//  return O32_GdiComment(hdc, cbSize, lpData);
  return TRUE;
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
  dprintf(("GDI32: ExtEscape, %x %x %d %x %d %x not implemented", hdc, nEscape, cbInput, lpszInData, cbOutput, lpszOutData));
#ifdef DEBUG
  if(cbInput && lpszInData) {
        ULONG *tmp = (ULONG *)lpszInData;
        for(int i=0;i<cbInput/4;i++) {
                dprintf(("GDI32: ExtEscape par %d: %x", i, *tmp++));
        }
  }
#endif
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
 * Name      : BOOL CombineTransform
 * Purpose   : The CombineTransform function concatenates two world-space to
 *             page-space transformations.
 * Parameters: LLPXFORM lLPXFORMResult address of combined transformation
 *             XFORM  *lLPXFORM1      address of 1st transformation
 *             XFORM  *lLPXFORM2      address of 2nd transformation
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : COMPLETELY UNTESTED
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
  if( IsBadWritePtr( (void*)lLPXFORMResult, sizeof(XFORM)) ||
      IsBadReadPtr(  (void*)lLPXFORM1, sizeof(XFORM)) ||
      IsBadWritePtr( (void*)lLPXFORM2, sizeof(XFORM)) )
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