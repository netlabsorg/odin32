/* $Id: text.cpp,v 1.11 2001-05-24 19:26:59 sandervl Exp $ */

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

