/* $Id: text.cpp,v 1.42 2004-01-14 16:53:25 sandervl Exp $ */

/*
 * GDI32 text apis
 *
 * Based on Wine/ReWind code (objects\text.c, objects\font.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *           1997 Alex Korobka
 *
 * Copyright 1999-2000 Christoph Bratschi
 * Copyright 2002-2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
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
#include "dibsect.h"
#include "ft2supp.h"
#include "font.h"
#include <math.h>

#define DBG_LOCALLOG    DBG_text
#include "dbglocal.h"

#define ELLIPSIS    "..."
#define ELLIPSISLEN 3

//******************************************************************************
//******************************************************************************
CHAR getBrokenDBCS( LPCSTR strA, INT lenA )
{
    int i;

    for( i = 0; i < lenA; i++ )
        if( IsDBCSLeadByte( strA[ i ]))
            i++;

    if( lenA < i ) // terminated at DBCS lead byte
        return strA[ lenA ];

    return 0;
}
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
BOOL InternalTextOutAW(HDC hdc,int X,int Y,UINT fuOptions,
                       CONST RECT *lprc, LPCSTR lpszStringA, LPCWSTR lpszStringW,
                       INT cbCount,CONST INT *lpDx,BOOL IsExtTextOut, BOOL fUnicode)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);
  ULONG flOptions = 0;
  RECTLOS2 pmRect;
  POINTLOS2 ptl;
  LONG hits;
  RECT rect;

  if (!pHps || (cbCount < 0) || (((lpszStringA == NULL && !fUnicode) || (lpszStringW == NULL && fUnicode)) && (cbCount != 0)))
  {
        dprintf(("InternalTextOutA: invalid parameter"));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
  }

  if(cbCount == -1) {
       if(fUnicode)
            cbCount = lstrlenW(lpszStringW);
       else cbCount = lstrlenA(lpszStringA);
  }

  if (cbCount > 512)
  {
        dprintf(("InternalTextOutA: invalid parameter cbCount"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
  }
  if (fuOptions & ~((UINT)(ETO_CLIPPED | ETO_OPAQUE | ETO_GLYPH_INDEX)))
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

  // When using font association, the height of DBCS and SBCS chars may be different.
  // In this case, background color make stair below chars
  if( IsDBCSEnv() && !lprc && ( GetBkMode( hdc ) & OPAQUE ))
  {
      SIZE   size;
      TEXTMETRICA tmA;

      if( fUnicode )
        GetTextExtentPointW( hdc, lpszStringW, cbCount, &size );
      else
        GetTextExtentPointA( hdc, lpszStringA, cbCount, &size );

      GetTextMetricsA( hdc, &tmA );

      rect.left = X;
      rect.right = X + size.cx;
      rect.top = Y;
      rect.bottom = Y + tmA.tmHeight;

      lprc = &rect;
      fuOptions |= ETO_OPAQUE;
  }

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
    if (fuOptions & ~ETO_GLYPH_INDEX)
    {
        dprintf(("InternalTextOutA: ERROR_INVALID_PARAMETER"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
  }

  if((lpszStringA || lpszStringW) && cbCount) {
      DIBSECTION_CHECK_IF_DIRTY(hdc);
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

        DIBSECTION_MARK_INVALID(hdc);

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

  if(fUnicode)
       hits = FT2Module.Ft2CharStringPosAtW(pHps->hps,&ptl,&pmRect,flOptions,cbCount,lpszStringW,lpDx, fuOptions & ETO_GLYPH_INDEX);
  else hits = FT2Module.Ft2CharStringPosAtA(pHps->hps,&ptl,&pmRect,flOptions,cbCount,lpszStringA,lpDx, fuOptions & ETO_GLYPH_INDEX);

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

  DIBSECTION_MARK_INVALID(hdc);

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutA(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCSTR lpszString,UINT cbCount,CONST INT *lpDx)
{
  BOOL  rc;
  SIZE  size;
  int   newy;

  if(lprc)
  {
        dprintf(("GDI32: ExtTextOutA %x %.*s (%d,%d) %x %d %x rect (%d,%d)(%d,%d)", hdc, cbCount, lpszString, X, Y, fuOptions, cbCount, lpDx, lprc->left, lprc->top, lprc->right, lprc->bottom));
  }
  else  dprintf(("GDI32: ExtTextOutA %x %.*s (%d,%d) %x %d %x", hdc, cbCount, lpszString, X, Y, fuOptions, cbCount, lpDx));

  rc = InternalTextOutAW(hdc, X, Y, fuOptions, lprc, lpszString, NULL, cbCount, lpDx, TRUE, FALSE);

  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ExtTextOutW(HDC hdc,int X,int Y,UINT fuOptions,CONST RECT *lprc,LPCWSTR lpszString,UINT cbCount,CONST int *lpDx)
{
  if(lprc) {
        dprintf(("GDI32: ExtTextOutW %x %.*ls (%d,%d) %x %d %x rect (%d,%d)(%d,%d)", hdc, cbCount, lpszString, X, Y, fuOptions, cbCount, lpDx, lprc->left, lprc->top, lprc->right, lprc->bottom));
  }
  else  dprintf(("GDI32: ExtTextOutW %x %.*ls (%d,%d) %x %d %x", hdc, cbCount, lpszString, X, Y, fuOptions, cbCount, lpDx));

  return InternalTextOutAW(hdc, X, Y, fuOptions, lprc, NULL, lpszString, cbCount, lpDx, TRUE, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutA(HDC hdc,int nXStart,int nYStart,LPCSTR lpszString,int cbString)
{
   dprintf(("GDI32: TextOutA %x (%d,%d) %d %.*s", hdc, nXStart, nYStart, cbString, cbString, lpszString));
   return InternalTextOutAW(hdc,nXStart,nYStart,0,NULL,lpszString,NULL,cbString,NULL,FALSE, FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TextOutW(HDC hdc,int nXStart,int nYStart,LPCWSTR lpszString,int cbString)
{
   dprintf(("GDI32: TextOutW %x (%d,%d) %d %.*ls", hdc, nXStart, nYStart, cbString, cbString, lpszString));
   return InternalTextOutAW(hdc,nXStart,nYStart,0,NULL, NULL, lpszString,cbString,NULL,FALSE, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolyTextOutA(HDC hdc,POLYTEXTA *pptxt,int cStrings)
{
  dprintf(("GDI32: PolyTextOutA %x %x %d", hdc, pptxt, cStrings));

  for (INT x = 0;x < cStrings;x++)
  {
      BOOL rc;

      rc = ExtTextOutA(hdc,pptxt[x].x,pptxt[x].y,pptxt[x].uiFlags,&pptxt[x].rcl,pptxt[x].lpstr, pptxt[x].n,pptxt[x].pdx);
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

      rc = ExtTextOutW(hdc,pptxt[x].x,pptxt[x].y,pptxt[x].uiFlags,&pptxt[x].rcl, pptxt[x].lpstr,pptxt[x].n,pptxt[x].pdx);
      if (!rc) return FALSE;
  }

  return TRUE;
}
//******************************************************************************
// Note: GetTextExtentPoint behaves differently under certain circumstances
//       compared to GetTextExtentPoint32 (due to bugs).
//       We are treating both as the same thing which is not entirely correct.
//
//******************************************************************************
BOOL WIN32API GetTextExtentPointA(HDC hdc, LPCTSTR lpsz, int cbString,
                                  LPSIZE lpsSize)
{
   BOOL ret = FALSE;
   INT  wlen;
   LPWSTR p;
   CHAR brokenDBCS = 0;

   if( IsDBCSEnv())
      brokenDBCS = getBrokenDBCS( lpsz, cbString );

   if( brokenDBCS )
      cbString--;

   p = FONT_mbtowc(hdc, lpsz, cbString, &wlen, NULL);
   if (p) {
       ret = GetTextExtentPointW( hdc, p, wlen, lpsSize );
       // For broken DBCS. Correct for FIXED WIDTH, but approx. for VARIABLE WIDTH
       if( brokenDBCS )
       {
          TEXTMETRICA tmA;

          GetTextMetricsA( hdc, &tmA );
          lpsSize->cx += tmA.tmAveCharWidth;

          if( cbString == 0 )
            lpsSize->cy = tmA.tmHeight;
       }

       HeapFree( GetProcessHeap(), 0, p );
   }
   else DebugInt3();
   return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentPointW(HDC    hdc,
                                  LPCWSTR lpString,
                                  int    cbString,
                                  PSIZE  lpSize)
{
   BOOL       rc;
   POINTLOS2  widthHeight = { 0, 0};
   pDCData    pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);

   dprintf(("GDI32: GetTextExtentPointW %.*ls", cbString, lpString));
   if(pHps == NULL)
   {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
   }

   if(lpString == NULL || cbString < 0 || lpSize == NULL)
   {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
   }

   lpSize->cx = 0;
   lpSize->cy = 0;

   // Verified with NT4, SP6
   if(cbString == 0)
   {
      dprintf(("!WARNING!: GDI32: GetTextExtentPointW invalid parameter!"));
      SetLastError(ERROR_SUCCESS);
      return TRUE;
   }

   if(pHps->isPrinter)
       ReallySetCharAttrs(pHps);

   if(cbString > 512)
   {
      DWORD cbStringNew;
      SIZE  newSize;

      dprintf(("WARNING: string longer than 512 chars; splitting up"));
      while(cbString) {
         cbStringNew = min(500, cbString);
         rc = GetTextExtentPointW(hdc, lpString, cbStringNew, &newSize);
         if(rc == FALSE) {
             return FALSE;
         }
         lpSize->cx += newSize.cx;
         lpSize->cy  = max(newSize.cy, lpSize->cy);
         lpString    += cbStringNew;
         cbString -= cbStringNew;
      }
      return TRUE;
   }

   rc = FT2Module.Ft2GetTextExtentW(pHps->hps, cbString, lpString, &widthHeight);
   if(rc == FALSE)
   {
      SetLastError(ERROR_INVALID_PARAMETER);    //todo wrong error
      return FALSE;
   }
   lpSize->cx = widthHeight.x;
   lpSize->cy = widthHeight.y;

   if(pHps && pHps->isPrinter && pHps->hdc)
   {//scale for printer dcs
       LONG alArray[2];

       if(OSLibDevQueryCaps(pHps, OSLIB_CAPS_HORIZONTAL_RESOLUTION, 2, &alArray[0]))
           lpSize->cx = lpSize->cx * alArray[0] / alArray[1];
   }

   dprintf(("GDI32: GetTextExtentPointW %x %ls %d returned %d (%d,%d)", hdc, lpString, cbString, rc, lpSize->cx, lpSize->cy));
   SetLastError(ERROR_SUCCESS);
   return TRUE;
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
BOOL WIN32API GetTextExtentExPointA(HDC hdc,
                                    LPCSTR  str,
                                    int     count,
                                    int     maxExt,
                                    LPINT   lpnFit,
                                    LPINT   alpDx,
                                    LPSIZE  size)
{
    BOOL ret;
    INT wlen;
    LPWSTR p;
    INT nFit;
    TEXTMETRICA tmA;
    CHAR brokenDBCS = 0;

    if( IsDBCSEnv())
    {
        brokenDBCS = getBrokenDBCS( str, count );

        GetTextMetricsA( hdc, &tmA );
    }

    if( brokenDBCS )
       count--;

    p = FONT_mbtowc( hdc, str, count, &wlen, NULL);
    ret = GetTextExtentExPointW( hdc, p, wlen, maxExt, &nFit, alpDx, size);
    nFit = WideCharToMultiByte(CP_ACP,0,p,nFit,NULL,0,NULL,NULL);
    if( IsDBCSEnv() && alpDx ) // index of alpDx between ansi and wide may not match in DBCS !!!
    {
        LPINT alpDxNew = ( LPINT )HeapAlloc( GetProcessHeap(), 0, sizeof( alpDx[ 0 ] ) * ( nFit + 1 ));
        INT prevDx;
        int i, j;

        for( i = j = 0; i < nFit; i++, j++ )
        {
            if( IsDBCSLeadByte( str[ i ]))
            {
                prevDx = ( i > 0 ) ? alpDxNew[ i - 1 ] : 0;
                alpDxNew[ i++ ] = prevDx + tmA.tmAveCharWidth;
                if( i >= nFit )
                    break;
            }
            alpDxNew[ i ] = alpDx[ j ];
        }

        if(( nFit < count ) && IsDBCSLeadByte( str[ nFit ]))
        {
            prevDx = ( nFit > 0 ) ? alpDxNew[ nFit - 1 ] : 0;
            if( maxExt >= prevDx + tmA.tmAveCharWidth )
                alpDxNew[ nFit++ ] = prevDx + tmA.tmAveCharWidth;
        }

        memcpy( alpDx, alpDxNew, sizeof( alpDx[ 0 ] ) * nFit );

        HeapFree( GetProcessHeap(), 0, alpDxNew );
    }

    // for broken DBCS. correct for FIXED WIDTH, not approx. for VARIABLE WIDTH
    if( brokenDBCS )
    {
       size->cx += tmA.tmAveCharWidth;
       if( count == 0 )
          size->cy = tmA.tmHeight;

       if(( maxExt > size->cx ) && ( nFit <= count )) // decreaed count by 1 above
       {
          if( alpDx )
            alpDx[ nFit ] = size->cx;

          nFit++;
       }
    }

    if (lpnFit) *lpnFit = nFit;

    HeapFree( GetProcessHeap(), 0, p );
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetTextExtentExPointW(HDC hdc,
                                    LPCWSTR str,
                                    int     count,
                                    int     maxExt,
                                    LPINT   lpnFit,
                                    LPINT   alpDx,
                                    LPSIZE  size)
{
    int i, nFit, extent;
    SIZE tSize;
    BOOL ret = FALSE;

    size->cx = size->cy = nFit = extent = 0;

    for( i = 1; i <= count; i++ )
    {
        if( !GetTextExtentPoint32W( hdc, str, i, &tSize )) goto done;

        if( maxExt < tSize.cx )
            break;

        if( alpDx )
            alpDx[ nFit ] = tSize.cx;

        nFit++;
    }

    if( i >= count )
        size->cx = tSize.cx;
    else if( !GetTextExtentPoint32W( hdc, str, count, size )) goto done;

    size->cy = tSize.cy; // The height of a font is constant.

    if(lpnFit) *lpnFit = nFit;
    ret = TRUE;

    dprintf(("returning %d %ld x %ld\n",nFit,size->cx,size->cy));

done:
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidth32A( HDC hdc, UINT iFirstChar, UINT iLastChar, PINT pWidthArray)
{
    BOOL ret = FALSE;

    for (int i = iFirstChar; i <= iLastChar; i++)
    {
        SIZE size;
        CHAR c = i;

        if (GetTextExtentPointA(hdc, &c, 1, &size))
        {
            pWidthArray[i-iFirstChar] = size.cx;
            // at least one character was processed
            ret = TRUE;
        }
        else
        {
            // default value for unprocessed characters
            pWidthArray[i-iFirstChar] = 0;
        }

        dprintf2(("Char 0x%x('%c') -> width %d", i, i<256? i: '.', pWidthArray[i-iFirstChar]));
    }

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidth32W(HDC hdc, UINT iFirstChar, UINT iLastChar, PINT pWidthArray)
{
    BOOL ret = FALSE;

    for (int i = iFirstChar; i <= iLastChar; i++)
    {
        SIZE size;
        WCHAR wc = i;

        if (GetTextExtentPointW(hdc, &wc, 1, &size))
        {
            pWidthArray[i-iFirstChar] = size.cx;
            // at least one character was processed
            ret = TRUE;
        }
        else
        {
            // default value for unprocessed characters
            pWidthArray[i-iFirstChar] = 0;
        }

        dprintf2(("Char 0x%x('%c') -> width %d", i, i<256? i: '.', pWidthArray[i-iFirstChar]));
    }

    return ret;
}
//******************************************************************************
// GetStringWidthW
//
// Return the width of each character in the string
//
// Parameters:
//    HDC    hdc            - device context handle
//    LPWSTR lpszString     - unicod string pointer
//    UINT   cbString       - number of valid characters in string
//    PINT   pWidthArray    - array that receives the character width (must be
//                            large enough to contain cbString elements
//
// Returns:
//    FALSE                 - failure
//    TRUE                  - success
//
//******************************************************************************
BOOL WIN32API GetStringWidthW(HDC hdc, LPWSTR lpszString, UINT cbString, PINT pWidthArray)
{
    return FT2Module.Ft2GetStringWidthW(hdc, lpszString, cbString, pWidthArray);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthFloatA(HDC hdc, UINT iFirstChar, UINT iLastChar, PFLOAT pxBUffer)
{
    dprintf(("ERROR: GDI32: GetCharWidthFloatA, not implemented\n"));
    DebugInt3();
    return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharWidthFloatW(HDC hdc, UINT iFirstChar, UINT iLastChar, PFLOAT pxBUffer)
{
    dprintf(("ERROR: GDI32: GetCharWidthFloatW, not implemented\n"));
    DebugInt3();
    return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsA(HDC hdc, UINT firstChar, UINT lastChar, LPABC abc)
{
    if(FT2Module.isEnabled() == FALSE)
    {//fallback method
        return O32_GetCharABCWidths(hdc, firstChar, lastChar, abc);
    }

    INT i, wlen, count = (INT)(lastChar - firstChar + 1);
    LPSTR str;
    LPWSTR wstr;
    BOOL ret = TRUE;

    if(count <= 0) {
        dprintf(("ERROR: Invalid parameter!!"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    str = (LPSTR)HeapAlloc(GetProcessHeap(), 0, count);
    for(i = 0; i < count; i++)
    str[i] = (BYTE)(firstChar + i);

    wstr = FONT_mbtowc(hdc, str, count, &wlen, NULL);

    for(i = 0; i < wlen; i++)
    {
    if(!GetCharABCWidthsW(hdc, wstr[i], wstr[i], abc))
    {
        ret = FALSE;
        break;
    }
    abc++;
    }

    HeapFree(GetProcessHeap(), 0, str);
    HeapFree(GetProcessHeap(), 0, wstr);

    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsW( HDC hdc, UINT firstChar, UINT lastChar, LPABC abc)
{
    if(FT2Module.isEnabled() == FALSE)
    {//no fallback method (yet)
        DebugInt3();
        return FALSE;
    }

    int i;
    GLYPHMETRICS gm;

    for (i=firstChar;i<=lastChar;i++)
    {
        if(GetGlyphOutlineW(hdc, i, GGO_METRICS, &gm, 0, NULL, NULL) == GDI_ERROR)
        {
            dprintf(("ERROR: GetGlyphOutlineW failed!!"));
            return FALSE;
        }
        abc[i-firstChar].abcA = gm.gmptGlyphOrigin.x;
        abc[i-firstChar].abcB = gm.gmBlackBoxX;
        abc[i-firstChar].abcC = gm.gmCellIncX - gm.gmptGlyphOrigin.x - gm.gmBlackBoxX;
        dprintf2(("GetCharABCWidthsW %d (%d,%d,%d)", i, abc[i-firstChar].abcA, abc[i-firstChar].abcB, abc[i-firstChar].abcC));
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsFloatA(HDC hdc, UINT iFirstChar, UINT iLastChar, LPABCFLOAT pxBUffer)
{
    dprintf(("ERROR: GDI32: GetCharABCWidthsFloatA, not implemented\n"));
    DebugInt3();
    return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCharABCWidthsFloatW(HDC hdc,
                                     UINT iFirstChar,
                                     UINT iLastChar,
                                     LPABCFLOAT pxBUffer)
{
    dprintf(("ERROR: GDI32: GetCharABCWidthsFloatA, not implemented\n"));
    DebugInt3();
    return(FALSE);
}
//******************************************************************************
//******************************************************************************

