/* $Id: text.cpp,v 1.12 2001-05-27 19:01:35 sandervl Exp $ */

/*
 * Font and Text Functions
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include "winuser.h"
#include "user32.h"
#include "syscolor.h"
#include <winnls.h>

#define DBG_LOCALLOG    DBG_text
#include "dbglocal.h"

//WINE parts: wine-991031

INT WIN32API DrawTextA(HDC hDC,LPCSTR lpString,INT nCount,PRECT lpRect,UINT nFormat)
{
  dprintf(("USER32: DrawTextA %x %s %d (%d,%d)(%d,%d) %x",hDC, lpString, nCount, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, nFormat));

  return InternalDrawTextExA(hDC,lpString,nCount,lpRect,nFormat,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
INT WIN32API DrawTextW(HDC hDC,LPCWSTR lpString,INT nCount,PRECT lpRect,UINT nFormat)
{
  dprintf(("USER32: DrawTextA %x %ls %d (%d,%d)(%d,%d) %x",hDC, lpString, nCount, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, nFormat));

  return InternalDrawTextExW(hDC,lpString,nCount,lpRect,nFormat,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
INT WIN32API DrawTextExA(HDC hdc,LPCSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams)
{
  dprintf(("USER32: DrawTextExA %x %s %d (%d,%d)(%d,%d) %x",hdc, lpchText, cchText, lprc->left, lprc->top, lprc->right, lprc->bottom, dwDTFormat));

  return InternalDrawTextExA(hdc,lpchText,cchText,lprc,dwDTFormat,lpDTParams,TRUE);
}
//******************************************************************************
//******************************************************************************
int WIN32API DrawTextExW(HDC hdc,LPWSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams)
{
  dprintf(("USER32: DrawTextExA %x %ls %d (%d,%d)(%d,%d) %x",hdc, lpchText, cchText, lprc->left, lprc->top, lprc->right, lprc->bottom, dwDTFormat));

  return InternalDrawTextExW(hdc,lpchText,cchText,lprc,dwDTFormat,lpDTParams,TRUE);
}
#if 1
//Based on Wine version 20010510
/***********************************************************************
 *           TEXT_TabbedTextOut
 *
 * Helper function for TabbedTextOut() and GetTabbedTextExtent().
 * Note: this doesn't work too well for text-alignment modes other
 *       than TA_LEFT|TA_TOP. But we want bug-for-bug compatibility :-)
 */
static LONG TEXT_TabbedTextOut( HDC hdc, INT x, INT y, LPCSTR lpstr,
                                INT count, INT cTabStops, const INT16 *lpTabPos16,
                                const INT *lpTabPos32, INT nTabOrg,
                                BOOL fDisplayText )
{
    INT defWidth;
    SIZE extent;
    int i, tabPos = x;
    int start = x;

    extent.cx = 0;
    extent.cy = 0;

    if (cTabStops == 1)
    {
        defWidth = lpTabPos32 ? *lpTabPos32 : *lpTabPos16;
        cTabStops = 0;
    }
    else
    {
        TEXTMETRICA tm;
        GetTextMetricsA( hdc, &tm );
        defWidth = 8 * tm.tmAveCharWidth;
    }

    while (count > 0)
    {
        for (i = 0; i < count; i++)
            if (lpstr[i] == '\t') break;
        GetTextExtentPointA( hdc, lpstr, i, &extent );
        if (lpTabPos32)
        {
            while ((cTabStops > 0) &&
                   (nTabOrg + *lpTabPos32 <= x + extent.cx))
            {
                lpTabPos32++;
                cTabStops--;
            }
        }
        else
        {
            while ((cTabStops > 0) &&
                   (nTabOrg + *lpTabPos16 <= x + extent.cx))
            {
                lpTabPos16++;
                cTabStops--;
            }
        }
        if (i == count)
            tabPos = x + extent.cx;
        else if (cTabStops > 0)
            tabPos = nTabOrg + (lpTabPos32 ? *lpTabPos32 : *lpTabPos16);
        else
            tabPos = nTabOrg + ((x + extent.cx - nTabOrg) / defWidth + 1) * defWidth;
        if (fDisplayText)
        {
            RECT r;
            r.left   = x;
            r.top    = y;
            r.right  = tabPos;
            r.bottom = y + extent.cy;
            ExtTextOutA( hdc, x, y,
                           GetBkMode(hdc) == OPAQUE ? ETO_OPAQUE : 0,
                           &r, lpstr, i, NULL );
        }
        x = tabPos;
        count -= i+1;
        lpstr += i+1;
    }
    return MAKELONG(tabPos - start, extent.cy);
}



/***********************************************************************
 *           TabbedTextOutA    (USER32.@)
 */
LONG WINAPI TabbedTextOutA( HDC hdc, INT x, INT y, LPCSTR lpstr, INT count,
                            INT cTabStops, INT *lpTabPos, INT nTabOrg )
{
    dprintf(("USER32: TabbedTextOutA %x (%d,%d) %s %d %d %x %d", hdc, x, y, lpstr, count, cTabStops, lpTabPos, nTabOrg));
    return TEXT_TabbedTextOut( hdc, x, y, lpstr, count, cTabStops,
                               NULL, lpTabPos, nTabOrg, TRUE );
}


/***********************************************************************
 *           TabbedTextOutW    (USER32.@)
 */
LONG WINAPI TabbedTextOutW( HDC hdc, INT x, INT y, LPCWSTR str, INT count,
                            INT cTabStops, INT *lpTabPos, INT nTabOrg )
{
    LONG ret;
    LPSTR p;
    INT acount;
    UINT codepage = CP_ACP; /* FIXME: get codepage of font charset */

    acount = WideCharToMultiByte(codepage,0,str,count,NULL,0,NULL,NULL);
    p = (LPSTR)HeapAlloc( GetProcessHeap(), 0, acount );
    if(p == NULL) return 0; /* FIXME: is this the correct return on failure */ 
    acount = WideCharToMultiByte(codepage,0,str,count,p,acount,NULL,NULL);
    ret = TabbedTextOutA( hdc, x, y, p, acount, cTabStops, lpTabPos, nTabOrg );
    HeapFree( GetProcessHeap(), 0, p );
    return ret;
}



/***********************************************************************
 *           GetTabbedTextExtentA    (USER32.@)
 */
DWORD WINAPI GetTabbedTextExtentA( HDC hdc, LPCSTR lpstr, INT count,
                                   INT cTabStops, INT *lpTabPos )
{
    dprintf(("USER32: GetTabbedTextExtentA %x %s %d %d %x",hdc, lpstr, count, cTabStops, lpTabPos));

    return TEXT_TabbedTextOut( hdc, 0, 0, lpstr, count, cTabStops,
                               NULL, lpTabPos, 0, FALSE );
}


/***********************************************************************
 *           GetTabbedTextExtentW    (USER32.@)
 */
DWORD WINAPI GetTabbedTextExtentW( HDC hdc, LPCWSTR lpstr, INT count,
                                   INT cTabStops, INT *lpTabPos )
{
    LONG ret;
    LPSTR p;
    INT acount;
    UINT codepage = CP_ACP; /* FIXME: get codepage of font charset */

    acount = WideCharToMultiByte(codepage,0,lpstr,count,NULL,0,NULL,NULL);
    p = (LPSTR)HeapAlloc( GetProcessHeap(), 0, acount );
    if(p == NULL) return 0; /* FIXME: is this the correct failure value? */
    acount = WideCharToMultiByte(codepage,0,lpstr,count,p,acount,NULL,NULL);
    ret = GetTabbedTextExtentA( hdc, p, acount, cTabStops, lpTabPos );
    HeapFree( GetProcessHeap(), 0, p );
    return ret;
}
#else
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTabbedTextExtentA( HDC hDC, LPCSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions)
{
  dprintf(("USER32: GetTabbedTextExtentA %x",hDC));

  return InternalGetTabbedTextExtentA(hDC,lpString,nCount,nTabPositions,lpnTabStopPositions);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetTabbedTextExtentW(HDC hDC,LPCWSTR lpString,INT nCount,INT nTabPositions,LPINT lpnTabStopPositions)
{
  dprintf(("USER32: GetTabbedTextExtentW %x",hDC));

  return InternalGetTabbedTextExtentW(hDC,lpString,nCount,nTabPositions,lpnTabStopPositions);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API TabbedTextOutA(HDC hdc,INT x,INT y,LPCSTR lpString,INT nCount,INT nTabPositions, LPINT lpnTabStopPositions,INT nTabOrigin)
{
  dprintf(("USER32: TabbedTextOutA %x",hdc));

  return InternalTabbedTextOutA(hdc,x,y,lpString,nCount,nTabPositions,lpnTabStopPositions,nTabOrigin);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API TabbedTextOutW( HDC hdc, int x, int y, LPCWSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int  nTabOrigin)
{
  dprintf(("USER32: TabbedTextOutW %x",hdc));

  return InternalTabbedTextOutW(hdc,x,y,lpString,nCount,nTabPositions,lpnTabStopPositions,nTabOrigin);
}
#endif
//******************************************************************************
// WINE/objects/text.c
//******************************************************************************
static BOOL InternalGrayString(HDC hdc,HBRUSH hBrush,GRAYSTRINGPROC lpOutputFunc,LPARAM lpData,INT nCount,INT x,INT y,INT nWidth,INT nHeight,BOOL isUnicode)
{
    HBITMAP hbm, hbmsave;
    HBRUSH hbsave;
    HFONT hfsave;
    HDC memdc = CreateCompatibleDC(hdc);
    BOOL retval = TRUE;
    RECT r;
    COLORREF fg, bg;

  if (!hdc) return FALSE;

  if (nCount == 0)
    nCount = isUnicode ? lstrlenW((LPCWSTR)lpData):lstrlenA((LPCSTR)lpData);

  if((nWidth == 0 || nHeight == 0) && nCount != -1)
  {
    SIZE s;

    if (isUnicode)
      GetTextExtentPoint32W(hdc,(LPCWSTR)lpData,nCount,&s);
    else
      GetTextExtentPoint32A(hdc,(LPCSTR)lpData,nCount,&s);
    if (nWidth == 0) nWidth = s.cx;
    if (nHeight == 0) nHeight = s.cy;
  }

  r.left = r.top = 0;
  r.right = nWidth;
  r.bottom = nHeight;

  hbm = CreateBitmap(nWidth,nHeight,1,1,NULL);
  hbmsave = (HBITMAP)SelectObject(memdc,hbm);
  FillRect(memdc,&r,(HBRUSH)GetStockObject(BLACK_BRUSH));
  SetTextColor(memdc,RGB(255,255,255));
  SetBkColor(memdc,RGB(0,0,0));
  hfsave = (HFONT)SelectObject(memdc,GetCurrentObject(hdc,OBJ_FONT));

  if (lpOutputFunc)
    retval = lpOutputFunc(memdc,lpData,nCount);
  else
    if (isUnicode)
      TextOutW(memdc,0,0,(LPCWSTR)lpData,nCount);
    else
      TextOutA(memdc,0,0,(LPCSTR)lpData,nCount);
  SelectObject(memdc, hfsave);

  /*
   * Windows doc says that the bitmap isn't grayed when len == -1 and
   * the callback function returns FALSE. However, testing this on
   * win95 showed otherwise...
   */
#ifdef GRAYSTRING_USING_DOCUMENTED_BEHAVIOUR
  if(retval || nCount != -1)
#endif
  {
    hbsave = (HBRUSH)SelectObject(memdc,GetPattern55AABrush());
    PatBlt(memdc,0,0,nWidth,nHeight,0x000A0329);
    SelectObject(memdc, hbsave);
  }

  if (hBrush) hbsave = (HBRUSH)SelectObject(hdc,hBrush);
  fg = SetTextColor(hdc, RGB(0, 0, 0));
  bg = SetBkColor(hdc, RGB(255, 255, 255));
  BitBlt(hdc,x,y,nWidth,nHeight,memdc,0,0,0x00E20746);
  SetTextColor(hdc, fg);
  SetBkColor(hdc, bg);
  if (hBrush) SelectObject(hdc,hbsave);

  SelectObject(memdc, hbmsave);
  DeleteObject(hbm);
  DeleteDC(memdc);

  return retval;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GrayStringA(HDC hdc,HBRUSH hBrush,GRAYSTRINGPROC lpOutputFunc,LPARAM lpData,int nCount,int X,int Y,int nWidth,int nHeight)
{
  dprintf(("USER32: GrayStringA %x",hdc));

  return InternalGrayString(hdc,hBrush,lpOutputFunc,lpData,nCount,X,Y,nWidth,nHeight,FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GrayStringW(HDC hdc,HBRUSH hBrush,GRAYSTRINGPROC lpOutputFunc,LPARAM lpData,int nCount,int X,int Y,int nWidth,int nHeight)
{
  dprintf(("USER32: GrayStringW %x",hdc));

  return InternalGrayString(hdc,hBrush,lpOutputFunc,lpData,nCount,X,Y,nWidth,nHeight,TRUE);
}

