/* $Id: text.cpp,v 1.32 2003-01-28 16:21:49 sandervl Exp $ */

/*
 * GDI32 text apis
 *
 * Based on Wine code (991031) (objects\text.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 * Copyright 1999-2000 Christoph Bratschi
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdio.h>
#include <misc.h>
#include <string.h>
#include <float.h>
#include "oslibgpi.h"
#include <dcdata.h>
#include <unicode.h>

#define DBG_LOCALLOG    DBG_text
#include "dbglocal.h"

#define ELLIPSIS    "..."
#define ELLIPSISLEN 3

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
// todo: metafile support
//#undef INVERT
//#define INVERT_SETYINVERSION
//******************************************************************************
BOOL InternalTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);
  ULONG flOptions = 0;
  RECTLOS2 pmRect;
  POINTLOS2 ptl;
  LONG hits;

  if (!pHps || (cbCount < 0) || ((lpszString == NULL) && (cbCount != 0)))
  {
        dprintf(("InternalTextOutA: invalid parameter"));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
  }

  if (cbCount > 512)
  {
        dprintf(("InternalTextOutA: invalid parameter cbCount"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
  }
  if (fuOptions & ~((UINT)(ETO_CLIPPED | ETO_OPAQUE)))
  {
        dprintf(("InternalTextOutA: invalid fuOptions"));
        //ETO_GLYPH_INDEX, ETO_RTLLEADING, ETO_NUMERICSLOCAL, ETO_NUMERICSLATIN, ETO_IGNORELANGUAGE, ETO_PDY  are ignored
        return TRUE;
  }

#if defined(INVERT) && !defined(INVERT_SETYINVERSION)
  if(pHps->yInvert > 0) {
     Y = pHps->yInvert - Y;
  } 
#endif

#ifdef INVERT_SETYINVERSION
  int oldyinv = GpiQueryYInversion(pHps->hps);
  Y = oldyinv - Y;
#endif

  //CB: add metafile info

  if (lprc)
  {
    if (fuOptions)
    {
      MapWin32ToOS2Rect(*lprc,pmRect);
      if (excludeBottomRightPoint(pHps,(PPOINTLOS2)&pmRect) == 0)
      {
        dprintf(("InternalTextOutA: excludeBottomRightPoint returned 0"));
        return TRUE;
      }
#ifndef INVERT
#ifdef INVERT_SETYINVERSION
      if (oldyinv) {
          int temp       = oldyinv - pmRect.yTop;
          pmRect.yTop    = oldyinv - pmRect.yBottom;
          pmRect.yBottom = temp;
      }
#else
      if (pHps->yInvert > 0) {
          int temp       = pHps->yInvert - pmRect.yTop;
          pmRect.yTop    = pHps->yInvert - pmRect.yBottom;
          pmRect.yBottom = temp;
      }    
#endif
#endif

      if (fuOptions & ETO_CLIPPED) flOptions |= CHSOS_CLIP;
      if (fuOptions & ETO_OPAQUE)  flOptions |= CHSOS_OPAQUE;
    }
  }
  else
  {
    if (fuOptions)
    {
        dprintf(("InternalTextOutA: ERROR_INVALID_HANDLE"));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
  }

  if (cbCount == 0)
  {
    if (fuOptions & ETO_OPAQUE)
    {
//SvL: This doesn't seem to work (anymore). Look at MFC apps for the missing border
//     between menu & button bar (all white). (e.g. odin app & acrobat reader 4.05)
#if 0
        lpszString = " ";
        cbCount = 1;
        flOptions |= CHSOS_CLIP;
#else
        HBRUSH hbrush = CreateSolidBrush(GetBkColor(hdc));
        HBRUSH oldbrush;

        oldbrush = SelectObject(hdc, hbrush);
        FillRect(hdc, lprc, hbrush);
        SelectObject(hdc, oldbrush);
        DeleteObject(hbrush);
        return TRUE;
#endif
    }
    else {
        dprintf(("InternalTextOutA: cbCount == 0"));
        return TRUE;
    }
  }

#ifdef INVERT_SETYINVERSION
  GpiEnableYInversion(pHps->hps, 0);
#endif

  if (lpDx)
    flOptions |= CHSOS_VECTOR;

  if (!getAlignUpdateCP(pHps))
  {
    ptl.x = X;
    ptl.y = Y;

    flOptions |= CHSOS_LEAVEPOS;
  }
  else OSLibGpiQueryCurrentPosition(pHps,&ptl);

  UINT align = GetTextAlign(hdc);
  LONG pmHAlign,pmVAlign;

#if 0
  //SvL: This code is broken. TODO: Investigate
  //CB: TA_RIGHT not supported by PM, only TA_CENTER and TA_LEFT
  if ((align & 0x6) == TA_RIGHT)
  {
    BOOL rc;
    PPOINTLOS2 pts = (PPOINTLOS2)malloc((cbCount+1)*sizeof(POINTLOS2));
 
    rc = OSLibGpiQueryCharStringPosAt(pHps,&ptl,flOptions & CHSOS_VECTOR,cbCount,lpszString,lpDx,pts);
    if(rc) {
        for(int i=0;i<cbCount+1;i++) {
            dprintf(("OSLibGpiQueryCharStringPosAt %d (%d,%d)", pts[i].x, pts[i].y));
        }
        ptl.x -= pts[cbCount].x-pts[0].x;
    }
    free(pts);
  }
#endif

  if (lprc && ((align & 0x18) == TA_BASELINE))
  {
    //CB: if TA_BASELINE is set, GPI doesn't fill rect
    //    TA_BOTTOM fills rect
    OSLibGpiQueryTextAlignment(pHps,&pmHAlign,&pmVAlign);
    OSLibGpiSetTextAlignment(pHps,pmHAlign,(pmVAlign & ~TAOS_BASE) | TAOS_BOTTOM);
  }

#ifdef INVERT
  ptl.y += getWorldYDeltaFor1Pixel(pHps);
#else
  ptl.y -= getWorldYDeltaFor1Pixel(pHps);

  int vertAdjust = 0;
  if ((pHps->taMode & 0x18) != TA_TOP) 
  {
      vertAdjust = OSLibGpiQueryFontMaxHeight(pHps->hps);
  }
  ptl.y -= vertAdjust;
#endif

  hits = OSLibGpiCharStringPosAt(pHps,&ptl,&pmRect,flOptions,cbCount,lpszString,lpDx);

  if (lprc && ((align & 0x18) == TA_BASELINE))
    OSLibGpiSetTextAlignment(pHps,pmHAlign,pmVAlign);

  if(hits == GPIOS_ERROR) {
        dprintf(("InternalTextOutA: OSLibGpiCharStringPosAt returned GPIOS_ERROR"));
#ifdef INVERT_SETYINVERSION
        GpiEnableYInversion(pHps->hps, oldyinv);
#endif
        return FALSE;
  }

  if (getAlignUpdateCP(pHps))
  {
    OSLibGpiQueryCurrentPosition(pHps,&ptl);
    ptl.y -= getWorldYDeltaFor1Pixel(pHps);
#ifndef INVERT
    ptl.y += vertAdjust;
#endif
    OSLibGpiSetCurrentPosition(pHps,&ptl);
  }

#ifdef INVERT_SETYINVERSION
  GpiEnableYInversion(pHps->hps, oldyinv);
#endif
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL InternalTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut)
{
  char *astring = NULL;
  BOOL  rc;

  if(cbCount == -1) {
     astring = UnicodeToAsciiString((LPWSTR)lpszString);
  }
  else 
  if(cbCount) {
     astring = (char *)HEAP_malloc(cbCount+1);
     UnicodeToAsciiN((LPWSTR)lpszString, astring, cbCount+1);
     astring[cbCount] = 0;
  }
  rc = InternalTextOutA(hdc,X,Y,fuOptions,lprc,(LPCSTR)astring,cbCount,lpDx,IsExtTextOut);
  if(astring) {
      FreeAsciiString(astring);
  }

  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,UINT cbCount,CONST INT *lpDx)
{
  if(lprc) {
        dprintf(("GDI32: ExtTextOutA %x %s (%d,%d) %x %d %x rect (%d,%d)(%d,%d)", hdc, lpszString, X, Y, fuOptions, cbCount, lpDx, lprc->left, lprc->top, lprc->right, lprc->bottom));
  }
  else  dprintf(("GDI32: ExtTextOutA %x %s (%d,%d) %x %d %x", hdc, lpszString, X, Y, fuOptions, cbCount, lpDx));

  return InternalTextOutA(hdc, X, Y, fuOptions, lprc, lpszString, cbCount, lpDx, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,UINT cbCount,CONST int *lpDx)
{
  if(lprc) {
        dprintf(("GDI32: ExtTextOutW %x %ls (%d,%d) %x %d %x rect (%d,%d)(%d,%d)", hdc, lpszString, X, Y, fuOptions, cbCount, lpDx, lprc->left, lprc->top, lprc->right, lprc->bottom));
  }
  else  dprintf(("GDI32: ExtTextOutW %x %ls (%d,%d) %x %d %x", hdc, lpszString, X, Y, fuOptions, cbCount, lpDx));
  return InternalTextOutW(hdc, X, Y, fuOptions, lprc, lpszString, cbCount, lpDx, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutA(HDC hdc,int nXStart,int nYStart,LPCSTR lpszString,int cbString)
{
   dprintf(("GDI32: TextOutA %x (%d,%d) %d %.*s", hdc, nXStart, nYStart, cbString, cbString, lpszString));
   return InternalTextOutA(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutW(HDC hdc,int nXStart,int nYStart,LPCWSTR lpszString,int cbString)
{
   dprintf(("GDI32: TextOutW %x (%d,%d) %d %.*ls", hdc, nXStart, nYStart, cbString, cbString, lpszString));
   return InternalTextOutW(hdc,nXStart,nYStart,0,NULL,lpszString,cbString,NULL,FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyTextOutA(HDC hdc,POLYTEXTA *pptxt,int cStrings)
{
  dprintf(("GDI32: PolyTextOutA %x %x %d", hdc, pptxt, cStrings));

  for (INT x = 0;x < cStrings;x++)
  {
    BOOL rc;

    rc = InternalTextOutA(hdc,pptxt[x].x,pptxt[x].y,pptxt[x].uiFlags,&pptxt[x].rcl,pptxt[x].lpstr,pptxt[x].n,pptxt[x].pdx,TRUE);
    if (!rc) return FALSE;
  }

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyTextOutW(HDC hdc,POLYTEXTW *pptxt,int cStrings)
{
  dprintf(("GDI32: PolyTextOutW %x %x %d", hdc, pptxt, cStrings));

  for (INT x = 0;x < cStrings;x++)
  {
    BOOL rc;

    rc = InternalTextOutW(hdc,pptxt[x].x,pptxt[x].y,pptxt[x].uiFlags,&pptxt[x].rcl,pptxt[x].lpstr,pptxt[x].n,pptxt[x].pdx,TRUE);
    if (!rc) return FALSE;
  }

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPointA(HDC hdc, LPCTSTR lpsz, int cbString,
                                  LPSIZE lpsSize)
{
#if 1
   dprintf(("GDI32: GetTextExtentPointA %x %.*s %d", hdc, cbString, lpsz, cbString));

   if(lpsz == NULL || cbString < 0 || lpsSize == NULL)
   {
      dprintf(("!WARNING!: GDI32: GetTextExtentPointA invalid parameter!"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
   }

   lpsSize->cx = 0;
   lpsSize->cy = 0;

   // Verified with NT4, SP6
   if(cbString == 0)
   {
      dprintf(("GDI32: GetTextExtentPointW cbString == 0"));
      SetLastError(ERROR_SUCCESS);
      return TRUE;
   }

   //SvL: This works better than the code below. Can been seen clearly
   //     in the Settings dialog box of VirtualPC. Strings are clipped.
   //     (e.g.: Hard Disk 1 -> Hard Disk)
   BOOL rc = O32_GetTextExtentPoint(hdc, lpsz, cbString, lpsSize);
   if(rc) {
      dprintf(("GDI32: GetTextExtentPointA returned (%d,%d)", lpsSize->cx, lpsSize->cy));
      SetLastError(ERROR_SUCCESS);
      return TRUE;
   }
   return FALSE;
#else
   BOOL       rc;
   POINTLOS2  pts[TXTBOXOS_COUNT];
   POINTLOS2  widthHeight = { 0, 0};
   pDCData    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

   dprintf(("GDI32: GetTextExtentPointA %s\n", lpsz));
   if(pHps == NULL)
   {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
   }

   if(lpsz == NULL || cbString < 0 || lpsSize == NULL)
   {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
   }

   lpsSize->cx = 0;
   lpsSize->cy = 0;

   // Verified with NT4, SP6
   if(cbString == 0)
   {
      SetLastError(ERROR_SUCCESS);
      return TRUE;
   }
   if(cbString > 512) 
   {
      DWORD cbStringNew;
      SIZE  newSize;

      dprintf(("WARNING: string longer than 512 chars; splitting up"));
      lpsSize->cx = 0;
      lpsSize->cy = 0;
      while(cbString) {
         cbStringNew = min(500, cbString);
         rc = GetTextExtentPointA(hdc, lpsz, cbStringNew, &newSize);
         if(rc == FALSE) {
             return FALSE;
         }
         lpsSize->cx += newSize.cx;
         lpsSize->cy  = max(newSize.cy, lpsSize->cy);
         lpsz     += cbStringNew;
         cbString -= cbStringNew;
      }
      return TRUE;
   }

   rc = OSLibGpiQueryTextBox(pHps, cbString, lpsz, TXTBOXOS_COUNT, pts);
   if(rc == FALSE)
   {
      SetLastError(ERROR_INVALID_PARAMETER);	//todo wrong error
      return FALSE;
   }
   calcDimensions(pts, &widthHeight);
   lpsSize->cx = widthHeight.x;
   lpsSize->cy = widthHeight.y;

   if(pHps && pHps->isPrinter && pHps->hdc)
   {//scale for printer dcs
       LONG alArray[2];

       if (OSLibDevQueryCaps(pHps, OSLIB_CAPS_HORIZONTAL_RESOLUTION, 2, &alArray[0]))
         lpsSize->cx = lpsSize->cx * alArray[0] / alArray[1];
   }

   dprintf(("GDI32: GetTextExtentPointA %x %s %d returned %d (%d,%d)", hdc, lpsz, cbString, rc, lpsSize->cx, lpsSize->cy));
   SetLastError(ERROR_SUCCESS);
   return TRUE;
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPointW(HDC    hdc,
                                  LPCWSTR lpString,
                                  int    cbString,
                                  PSIZE  lpSize)
{
  char *astring;
  BOOL  rc;

   if(lpString == NULL || cbString < 0 || lpSize == NULL)
   {
      dprintf(("!WARNING!: GDI32: GetTextExtentPointW invalid parameter!"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
   }

   lpSize->cx = 0;
   lpSize->cy = 0;

   // Verified with NT4, SP6
   if(cbString == 0)
   {
      dprintf(("GDI32: GetTextExtentPointW cbString == 0"));
      SetLastError(ERROR_SUCCESS);
      return TRUE;
   }

   dprintf(("GDI32: GetTextExtentPointW %x %.*ls %d %x", hdc, cbString, lpString, cbString, lpSize));

   astring = (char *)malloc((cbString+1)*sizeof(WCHAR));
   UnicodeToAsciiN(lpString, astring, cbString+1);

   rc = GetTextExtentPointA(hdc, astring,
                            cbString, lpSize);

   free(astring);
   return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32A( HDC hdc, LPCSTR lpsz, int cbString, PSIZE  lpSize)
{
    return GetTextExtentPointA(hdc, lpsz, cbString, lpSize);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPoint32W(HDC hdc, LPCWSTR lpsz, int cbString, PSIZE lpSize)
{
    return GetTextExtentPointW(hdc, lpsz, cbString, lpSize);
}
//******************************************************************************
//******************************************************************************
