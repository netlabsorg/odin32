/* $Id: text.cpp,v 1.3 1999-12-09 16:49:45 cbratschi Exp $ */

/*
 * GDI32 text apis
 *
 * Based on Wine code (991031) (objects\text.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <misc.h>
#include <string.h>
#include "oslibgpi.h"

//******************************************************************************
//******************************************************************************
UINT WINAPI GetTextCharsetInfo(
    HDC hdc,          /* [in]  Handle to device context */
    LPFONTSIGNATURE fs, /* [out] Pointer to struct to receive data */
    DWORD flags)        /* [in]  Reserved - must be 0 */
{
    HGDIOBJ hFont;
    UINT charSet = DEFAULT_CHARSET;
    LOGFONTW lf;
    CHARSETINFO csinfo;

    dprintf(("GetTextCharsetInfo %x %x %x", hdc, fs, flags));

    hFont = GetCurrentObject(hdc, OBJ_FONT);
    if (hFont == 0)
        return(DEFAULT_CHARSET);
    if ( GetObjectW(hFont, sizeof(LOGFONTW), &lf) != 0 )
        charSet = lf.lfCharSet;

    if (fs != NULL) {
      if (!TranslateCharsetInfo((LPDWORD)charSet, &csinfo, TCI_SRCCHARSET))
           return  (DEFAULT_CHARSET);
      memcpy(fs, &csinfo.fs, sizeof(FONTSIGNATURE));
    }
    return charSet;
}
/***********************************************************************
 * GetTextCharset32 [GDI32.226]  Gets character set for font in DC
 *
 * NOTES
 *    Should it return a UINT32 instead of an INT32?
 *    => YES, as GetTextCharsetInfo returns UINT32
 *
 * RETURNS
 *    Success: Character set identifier
 *    Failure: DEFAULT_CHARSET
 */
UINT WINAPI GetTextCharset(HDC hdc) /* [in] Handle to device context */
{
    /* MSDN docs say this is equivalent */
    return GetTextCharsetInfo(hdc, NULL, 0);
}
//******************************************************************************
// CB: USER32 function, but here is the better place
//******************************************************************************
INT WIN32API InternalDrawTextExA(HDC hdc,LPCSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams)
{
  //CB: todo

  return 0;
}
//******************************************************************************
//******************************************************************************
INT WIN32API InternalDrawTextExW(HDC hdc,LPWSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams)
{
  char *astring = UnicodeToAsciiStringN((LPWSTR)lpchText,cchText);
  INT  rc;

  rc = InternalDrawTextExA(hdc,astring,cchText,lprc,dwDTFormat,lpDTParams);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
// CB: USER32 function, but here is the better place
//******************************************************************************
LONG WIN32API InternalTabbedTextOutA( HDC hdc, int x, int y, LPCSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int  nTabOrigin)
{
  //CB: todo

  return 0;
}
//******************************************************************************
//******************************************************************************
LONG WIN32API InternalTabbedTextOutW( HDC hdc, int x, int y, LPCWSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int  nTabOrigin)
{
  char *astring = UnicodeToAsciiStringN((LPWSTR)lpString,nCount);
  LONG rc;

  rc = InternalTabbedTextOutA(hdc,x,y,astring,nCount,nTabPositions,lpnTabStopPositions,nTabOrigin);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
// CB: USER32 function, but here is the better place
//******************************************************************************
DWORD WIN32API InternalGetTabbedTextExtentA( HDC hDC, LPCSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions)
{
  //CB: todo

  return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API InternalGetTabbedTextExtentW( HDC hDC, LPCWSTR lpString, int nCount, int nTabPositions, LPINT lpnTabStopPositions)
{
  char *astring = UnicodeToAsciiStringN((LPWSTR)lpString,nCount);
  DWORD rc;

  rc = InternalGetTabbedTextExtentA(hDC,astring,nCount,nTabPositions,lpnTabStopPositions);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
// todo: metafile support
//******************************************************************************
BOOL InternalTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  PVOID pHps = OSLibGpiQueryDCData(hdc);
  ULONG flOptions = 0;
  RECTLOS2 pmRect;
  POINTLOS2 ptl;
  LONG hits;

  if (!pHps || cbCount < 0 || (lpszString == NULL && cbCount != 0))
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  if (cbCount > 512)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if (fuOptions & ~((UINT)(ETO_CLIPPED | ETO_OPAQUE)))
  {
    //ETO_GLYPH_INDEX, ETO_RTLLEADING, ETO_NUMERICSLOCAL, ETO_NUMERICSLATIN, ETO_IGNORELANGUAGE, ETO_PDY  are ignored
    return TRUE;
  }

  //CB: add metafile info

  if (lprc)
  {
    if (fuOptions)
    {
      MapWin32ToOS2Rect(*lprc,pmRect);
      if (excludeBottomRightPoint(pHps,(PPOINTLOS2)&pmRect) == 0)
      {
        return TRUE;
      }

      if (fuOptions & ETO_CLIPPED) flOptions |= CHSOS_CLIP;
      if (fuOptions & ETO_OPAQUE)  flOptions |= CHSOS_OPAQUE;
    }
  } else
  {
    if (fuOptions)
    {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
    }
  }

  if (cbCount == 0)
  {
    if (fuOptions & ETO_OPAQUE)
    {
      lpszString = " ";
      cbCount = 1;
      flOptions |= CHSOS_CLIP;
    } else return TRUE;
  }
  if (lpDx)
    flOptions |= CHSOS_VECTOR;

  if (getAlignUpdateCP(pHps) == FALSE)
  {
    ptl.x = X;
    ptl.y = Y;

    flOptions |= CHSOS_LEAVEPOS;
  } else OSLibGpiQueryCurrentPosition(pHps,&ptl);

  UINT align = GetTextAlign(hdc);
  LONG pmHAlign,pmVAlign;

  //CB: TA_RIGHT not supported, only TA_CENTER and TA_LEFT
  if ((align & 0x6) == TA_RIGHT)
  {
    PPOINTLOS2 pts = (PPOINTLOS2)malloc((cbCount+1)*sizeof(POINTLOS2));

    OSLibGpiQueryCharStringPosAt(pHps,&ptl,flOptions,cbCount,lpszString,lpDx,pts);
    ptl.x -= pts[cbCount].x-pts[0].x;
    free(pts);
  }

  if (lprc && (align & 0x18) == TA_BASELINE)
  {
    //CB: if TA_BASELINE is set, GPI doesn't fill rect
    //    TA_BOTTOM fills rect
    OSLibGpiQueryTextAlignment(pHps,&pmHAlign,&pmVAlign);
    OSLibGpiSetTextAlignment(pHps,pmHAlign,(pmVAlign & ~TAOS_BASE) | TAOS_BOTTOM);
  }

  ptl.y += getWorldYDeltaFor1Pixel(pHps);

  hits = OSLibGpiCharStringPosAt(pHps,&ptl,&pmRect,flOptions,cbCount,lpszString,lpDx);

  if (lprc && (align & 0x18) == TA_BASELINE)
    OSLibGpiSetTextAlignment(pHps,pmHAlign,pmVAlign);

  if (hits == GPIOS_ERROR)
  {
    return FALSE;
  }
  if (getAlignUpdateCP(pHps))
  {
    OSLibGpiQueryCurrentPosition(pHps,&ptl);
    ptl.y -= getWorldYDeltaFor1Pixel(pHps);
    OSLibGpiSetCurrentPosition(pHps,&ptl);
  }

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL InternalTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  char *astring = UnicodeToAsciiStringN((LPWSTR)lpszString,cbCount);
  BOOL  rc;

  rc = InternalTextOutA(hdc,X,Y,fuOptions,lprc,(LPCSTR)astring,cbCount,lpDx,IsExtTextOut);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,UINT cbCount,CONST INT *lpDx)
{
  dprintf(("GDI32: ExtTextOutA\n"));

  return InternalTextOutA(hdc,X,Y,fuOptions,lprc,lpszString,cbCount,lpDx,TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,UINT cbCount,CONST int *lpDx)
{
  dprintf(("GDI32: ExtTextOutW\n"));

  return InternalTextOutW(hdc,X,Y,fuOptions,lprc,lpszString,cbCount,lpDx,TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutA(HDC hdc,int nXStart,int nYStart,LPCSTR lpszString,int cbString)
{
  dprintf(("GDI32: TextOutA"));

  return InternalTextOutA(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutW(HDC hdc,int nXStart,int nYStart,LPCWSTR lpszString,int cbString)
{
  dprintf(("GDI32: TextOutW"));

  return InternalTextOutW(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
