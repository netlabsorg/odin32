/* $Id: text.cpp,v 1.6 1999-12-27 22:52:39 cbratschi Exp $ */

/*
 * GDI32 text apis
 *
 * Based on Wine code (991031) (objects\text.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <misc.h>
#include <string.h>
#include "oslibgpi.h"

#define ELLIPSIS    "..."
#define ELLIPSISLEN 3

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
INT SYSTEM EXPORT InternalDrawTextExA(HDC hdc,LPCSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams,BOOL isDrawTextEx)
{
/*
  PVOID pHps = OSLibGpiQueryDCData(hdc);
  INT rc;
  ULONG flCmd;
  RECT localRectangle;
  PRECT rectPtr;
  LONG lTabs,xLeft,yTop;

  if ((!lpchText) || (cchText == 0) || (cchText < -1) || (!lprc == NULL))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  if (!pHps)
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }

  if (cchText == -1)
  {
    cchText = strlen(lpchText);
    if (cchText == 0)
      return 0; //CB: does Win32 return textheight in this case?
  }

  if (lpDTParams)
  {
    // set margins
    lprc->left += lpDTParams->iLeftMargin;
    lprc->right -= lpDTParams->iRightMargin;

    // just assume all the text has been drawn
    lpDTParams->uiLengthDrawn = cchText;
  }

  if (!(dwDTFormat & DT_CALCRECT))
  {
    BOOL bTopBottomIsOkay;

    if ((getIsTopTop(pHps) && lprc->top > lprc->bottom) ||
        (!getIsTopTop(pHps) && lprc->top < lprc->bottom))
      bTopBottomIsOkay = FALSE;
    else
      bTopBottomIsOkay = TRUE;

    if ((lprc->left >= lprc->right) || !bTopBottomIsOkay)
    {
      TEXTMETRICA txtMetrics;
      BOOL result;

      result = GetTextMetricsA(hdc,&txtMetrics);
      if (result)
        rc = (int)txtMetrics.tmHeight;
      else
        rc = 0;

      if (lpDTParams)
      {
        lprc->left -= lpDTParams->iLeftMargin;
        lprc->right += lpDTParams->iRightMargin;
      }

      return rc;
    }
  }

  flCmd = DTOS_INVERT | DTOS_WORLDRECT | DTOS_TEXTATTRS | DTOS_AMPERSAND | DTOS_VERTICALEXTENT;

  LONG lMixMode = OSLibGpiQueryBackMix(pHps);
  if (lMixMode == BMOS_OVERPAINT) flCmd |= DTOS_OPAQUE;

  if (dwDTFormat & DT_CALCRECT)
  {
    rectPtr = lprc;
    flCmd |= DTOS_QUERYEXTENT;

    xLeft = rectPtr->left;
    yTop = rectPtr->top;

    if (dwDTFormat & DT_RIGHT)
    {
      if (rectPtr->left >= rectPtr->right)
        rectPtr->left = rectPtr->right-1;
    } else
    {
      if (rectPtr->right <= rectPtr->left)
        rectPtr->right = rectPtr->left+1;
    }

    if (dwDTFormat & DT_BOTTOM)
    {
      if (rectPtr->top >= rectPtr->bottom)
        rectPtr->top = rectPtr->bottom-1;
    } else
    {
      if (rectPtr->bottom <= rectPtr->top)
        rectPtr->bottom = rectPtr->top+1;
    }
  } else
  {
    rectPtr = &localRectangle;

    if (getMapMode(pHps) == MMOS_ANISOTROPIC || getMapMode(pHps) == MMOS_ISOTROPIC)
    {
      if (doesYAxisGrowNorth(pHps))
      {
        flCmd &= ~DTOS_INVERT;
        flCmd |=  DTOS_INVERTCHAR;
      }
    }

    if (lprc->left > lprc->right)
    {
      rectPtr->left  = lprc->right;
      rectPtr->right = lprc->left;
    } else
    {
      rectPtr->left  = lprc->left;
      rectPtr->right = lprc->right;
    }
    if (lprc->top > lprc->bottom)
    {
      rectPtr->top    = lprc->bottom;
      rectPtr->bottom = lprc->top;
    } else
    {
      rectPtr->top    = lprc->top;
      rectPtr->bottom = lprc->bottom;
    }
  }

  if (dwDTFormat & DT_EXPANDTABS)
  {
    if (isDrawTextEx)
    {
      lTabs = (lpDTParams && dwDTFormat & DT_TABSTOP) ? lpDTParams->iTabLength:8;
    } else
    {
      lTabs = 8;

      if (dwDTFormat & DT_TABSTOP)
      {
        lTabs = (dwDTFormat >> 8) & 0x000000FF;
        dwDTFormat &= 0xFFFF00FF;
      }
    }
  } else lTabs = -1;

  if (dwDTFormat & DT_RIGHT)
    flCmd |= DTOS_RIGHT;
  else
    if (dwDTFormat & DT_CENTER) flCmd |= DTOS_CENTER;
  if (dwDTFormat & DT_BOTTOM)          flCmd |= DTOS_BOTTOM;
  if (dwDTFormat & DT_VCENTER)         flCmd |= DTOS_VCENTER;
  if (dwDTFormat & DT_WORDBREAK)       flCmd |= DTOS_WORDBREAK;
  if (dwDTFormat & DT_EXTERNALLEADING) flCmd |= DTOS_EXTERNALLEADING;
  if (!(dwDTFormat & DT_NOPREFIX))     flCmd |= DTOS_MNEMONIC;
  if (dwDTFormat & DT_SINGLELINE)
    flCmd |= DTOS_SINGLELINE;
  else
    flCmd |= DTOS_MULTILINE;
  if (dwDTFormat & DT_NOCLIP)          flCmd |= DTOS_NOCLIP;

  //CB: DT_EDITCONTROL, DT_RTLREADING ignored

  BOOL done = FALSE;

  if ((dwDTFormat & DT_END_ELLIPSIS) && (cchText > 1))
  {
    int textWidth,width;
    RECT rect;

    SetRectEmpty(&rect);
    OSLibWinDrawTabbedText(pHps,cchText,lTabs,lpchText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
    width = rectPtr->right-rectPtr->left;
    textWidth = rect.right-rect.left;
    if ((textWidth > width) && (width > 0))
    {
      char* newText;
      int newTextLen = cchText-1+ELLIPSISLEN;
      int preLen = cchText-1;

      newText = (char*)malloc(newTextLen+1);
      strncpy(newText,lpchText,cchText-1);
      do
      {
        strcpy(&newText[preLen],ELLIPSIS);
        OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
        textWidth = rect.right-rect.left;
        if (textWidth <= width || preLen == 1) break;
        newTextLen--;
        preLen--;
      } while (TRUE);
      rc = OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,rectPtr,0,0,flCmd);
      if (dwDTFormat & DT_MODIFYSTRING) strcpy((LPSTR)lpchText,newText); //check length?
      free(newText);

      done = TRUE;
    }
  } else if ((dwDTFormat & DT_PATH_ELLIPSIS) && (cchText > 1))
  {
    int textWidth,width;
    RECT rect;

//CB: code not yet checked (-> testcase)

    SetRectEmpty(&rect);
    OSLibWinDrawTabbedText(pHps,cchText,lTabs,lpchText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
    width = rectPtr->right-rectPtr->left;
    textWidth = rect.right-rect.left;
    if ((textWidth > width) && (width > 0))
    {
      char *newText,*slashPos;
      int newTextLen = cchText+ELLIPSISLEN;

      newText = (char*)malloc(newTextLen+1);
      strncpy(newText,lpchText,cchText);
      slashPos = strrchr(newText,(int)"\\");
      if (slashPos != NULL)
      {
        int preLen = slashPos-newText;
        char* endPtr = (char*)lpchText+preLen;
        int endLen = cchText-preLen;
        BOOL ok = FALSE;

        //delete start
        do
        {
          strcpy(&newText[preLen],ELLIPSIS);
          strncpy(&newText[preLen+ELLIPSISLEN],endPtr,endLen);
          OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
          textWidth = rect.right-rect.left;
          if (textWidth <= width)
          {
            ok = TRUE;
            break;
          }
          if (preLen == 0) break;
          newTextLen--;
          preLen--;
        } while (TRUE);

        if (!ok)
        {
          //delete end
          do
          {
            endPtr++;
            endLen--;
            newTextLen--;
            strncpy(&newText[ELLIPSISLEN],endPtr,endLen);
            OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
            textWidth = rect.right-rect.left;
            if ((textWidth <= width) || (endLen == 0)) break;
          } while (TRUE);
        }
      } else
      {
        int preLen,endLen;

        preLen = cchText/2;
        endLen = cchText-preLen; //endLen >= preLen
        char* endPtr = (char*)lpchText+preLen;

        do
        {
          strcpy(&newText[preLen],ELLIPSIS);
          strncpy(&newText[preLen+ELLIPSISLEN],endPtr,endLen);
          OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,&rect,0,0,flCmd | DTOS_QUERYEXTENT);
          textWidth = rect.right-rect.left;
          if ((textWidth <= width) || (preLen+endLen == 0)) break;
          if (endLen > preLen)
          {
            endLen--;
            endPtr++;
          } else preLen--;
          newTextLen--;
        } while (TRUE);
      }
      rc = OSLibWinDrawTabbedText(pHps,newTextLen,lTabs,newText,rectPtr,0,0,flCmd);
      if (dwDTFormat & DT_MODIFYSTRING) strcpy((LPSTR)lpchText,newText); //check length?
      free(newText);

      done = TRUE;
    }
  }

  if (!done)
    rc = OSLibWinDrawTabbedText(pHps,cchText,lTabs,lpchText,rectPtr,0,0,flCmd);

  if (dwDTFormat & DT_CALCRECT)
  {
    if (!(dwDTFormat & DT_RIGHT) && (rectPtr->left < xLeft))
    {
      rectPtr->right = xLeft+(rectPtr->right-rectPtr->left);
      rectPtr->left = xLeft;
    }
    if (!(dwDTFormat & DT_BOTTOM) && (rectPtr->top < yTop))
    {
      rectPtr->bottom = yTop+(rectPtr->bottom-rectPtr->top);
      rectPtr->top = yTop;
    }
  }

  if (lpDTParams)
  {
    // don't forget to restore the margins
    lprc->left -= lpDTParams->iLeftMargin;
    lprc->right += lpDTParams->iRightMargin;
  }

  return rc;
*/

  dwDTFormat &= ~(DT_END_ELLIPSIS | DT_PATH_ELLIPSIS);
  return O32_DrawText(hdc,lpchText,cchText,lprc,dwDTFormat);
}
//******************************************************************************
//******************************************************************************
INT SYSTEM EXPORT InternalDrawTextExW(HDC hdc,LPCWSTR lpchText,INT cchText,LPRECT lprc,UINT dwDTFormat,LPDRAWTEXTPARAMS lpDTParams,BOOL isDrawTextEx)
{
  char *astring = (cchText == -1) ? UnicodeToAsciiString((LPWSTR)lpchText):UnicodeToAsciiStringN((LPWSTR)lpchText,cchText);
  INT  rc;

  rc = InternalDrawTextExA(hdc,astring,cchText,lprc,dwDTFormat,lpDTParams,isDrawTextEx);
  if (dwDTFormat & DT_MODIFYSTRING && (dwDTFormat & (DT_END_ELLIPSIS | DT_PATH_ELLIPSIS))) AsciiToUnicode(astring,(LPWSTR)lpchText);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
// CB: USER32 function, but here is the better place
//******************************************************************************
DWORD SYSTEM EXPORT InternalGetTabbedTextExtentA(HDC hDC,LPCSTR lpString,INT nCount,INT nTabPositions,LPINT lpnTabStopPositions)
{
  PVOID pHps = OSLibGpiQueryDCData(hDC);
  BOOL result;
  POINTLOS2 pts[TXTBOXOS_COUNT];
  POINTLOS2 widthHeight = {0,0};

  if (!pHps || (nCount == 0) || (nTabPositions < 0))
    return 0;

  if (nCount < 0)
  {
    SetLastError(ERROR_STACK_OVERFLOW);
    return 0;
  }
  if ((lpString == NULL) || (nCount >  512) || ((nTabPositions > 0) && (lpnTabStopPositions == NULL)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  //CB: Win95 supports negative values for right aligned text
  for (INT i = 0;i < nTabPositions;i++)
  {
    if (lpnTabStopPositions[i] < 0)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }
  }

  result = OSLibGpiQueryTextBox(pHps,(nCount > 1) ? 1:nCount,lpString,TXTBOXOS_COUNT,pts);
  if (result == FALSE)
    return 0;
  calcDimensions(pts,&widthHeight);

  LONG rv1 = OSLibGpiQueryTabbedTextExtent(pHps,nCount,lpString,nTabPositions,lpnTabStopPositions);
  if (rv1 == GPIOS_ERROR)
    return 0;

  return (widthHeight.y <<16)+labs(rv1);
}
//******************************************************************************
//******************************************************************************
DWORD SYSTEM EXPORT InternalGetTabbedTextExtentW(HDC hDC,LPCWSTR lpString,INT nCount,INT nTabPositions,LPINT lpnTabStopPositions)
{
  char *astring = (nCount == -1) ? UnicodeToAsciiString((LPWSTR)lpString):UnicodeToAsciiStringN((LPWSTR)lpString,nCount);
  DWORD rc;

  rc = InternalGetTabbedTextExtentA(hDC,astring,nCount,nTabPositions,lpnTabStopPositions);
  FreeAsciiString(astring);

  return(rc);
}
//******************************************************************************
// CB: USER32 function, but here is the better place
//******************************************************************************
LONG SYSTEM EXPORT InternalTabbedTextOutA(HDC hdc,INT x,INT y,LPCSTR lpString,INT nCount,INT nTabPositions,LPINT lpnTabStopPositions,INT nTabOrigin)
{
  PVOID pHps = OSLibGpiQueryDCData(hdc);
  POINTLOS2 ptl;
  DWORD dimensions;

  if ((pHps == NULL) || (lpString == NULL))
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }
  if (nCount == -1)
    nCount = strlen(lpString);
  if (nCount < 1)
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }
  if (lpnTabStopPositions == NULL)
    nTabPositions = 0;
  if (nTabPositions == 0)
    lpnTabStopPositions = NULL;
  if (nTabPositions < 0)
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return 0;
  }
  if (getAlignUpdateCP(pHps) == TRUE)
    OSLibGpiQueryCurrentPosition(pHps,&ptl);
  else
  {
    ptl.x = x;
    ptl.y = y;
  }

  //CB: nTabOrigin is ignored! -> wrong size (width)!
  //    todo: change low word (width), height is ok
  dimensions = InternalGetTabbedTextExtentA(hdc,lpString,nCount,nTabPositions,lpnTabStopPositions);
  if (dimensions != 0)
  {
    LONG rcGPI;

    ptl.y += getWorldYDeltaFor1Pixel(pHps);

    rcGPI = OSLibGpiTabbedCharStringAt(pHps,&ptl,NULL,0,nCount,lpString,nTabPositions,lpnTabStopPositions,nTabOrigin);
    if (rcGPI == GPIOS_ALTERROR)
      return 0;
    if (getAlignUpdateCP(pHps))
    {
      OSLibGpiQueryCurrentPosition (pHps,&ptl);
      ptl.y -= getWorldYDeltaFor1Pixel(pHps);
      OSLibGpiSetCurrentPosition (pHps,&ptl);
    }

    return dimensions;
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
LONG SYSTEM EXPORT InternalTabbedTextOutW(HDC hdc,INT x,INT y,LPCWSTR lpString,INT nCount,INT nTabPositions,LPINT lpnTabStopPositions,INT nTabOrigin)
{
  char *astring = (nCount == -1) ? UnicodeToAsciiString((LPWSTR)lpString):UnicodeToAsciiStringN((LPWSTR)lpString,nCount);
  LONG rc;

  rc = InternalTabbedTextOutA(hdc,x,y,astring,nCount,nTabPositions,lpnTabStopPositions,nTabOrigin);
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

  if (!pHps || (cbCount < 0) || ((lpszString == NULL) && (cbCount != 0)))
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

  if (!getAlignUpdateCP(pHps))
  {
    ptl.x = X;
    ptl.y = Y;

    flOptions |= CHSOS_LEAVEPOS;
  } else OSLibGpiQueryCurrentPosition(pHps,&ptl);

  UINT align = GetTextAlign(hdc);
  LONG pmHAlign,pmVAlign;

  //CB: TA_RIGHT not supported by PM, only TA_CENTER and TA_LEFT
  if ((align & 0x6) == TA_RIGHT)
  {
    PPOINTLOS2 pts = (PPOINTLOS2)malloc((cbCount+1)*sizeof(POINTLOS2));

    OSLibGpiQueryCharStringPosAt(pHps,&ptl,flOptions,cbCount,lpszString,lpDx,pts);
    ptl.x -= pts[cbCount].x-pts[0].x;
    free(pts);
  }

  if (lprc && ((align & 0x18) == TA_BASELINE))
  {
    //CB: if TA_BASELINE is set, GPI doesn't fill rect
    //    TA_BOTTOM fills rect
    OSLibGpiQueryTextAlignment(pHps,&pmHAlign,&pmVAlign);
    OSLibGpiSetTextAlignment(pHps,pmHAlign,(pmVAlign & ~TAOS_BASE) | TAOS_BOTTOM);
  }

  ptl.y += getWorldYDeltaFor1Pixel(pHps);

  hits = OSLibGpiCharStringPosAt(pHps,&ptl,&pmRect,flOptions,cbCount,lpszString,lpDx);

  if (lprc && ((align & 0x18) == TA_BASELINE))
    OSLibGpiSetTextAlignment(pHps,pmHAlign,pmVAlign);

  if (hits == GPIOS_ERROR)
    return FALSE;

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
  char *astring = (cbCount == -1) ? UnicodeToAsciiString((LPWSTR)lpszString):UnicodeToAsciiStringN((LPWSTR)lpszString,cbCount);
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
BOOL WIN32API PolyTextOutA(HDC hdc,POLYTEXTA *pptxt,int cStrings)
{
  dprintf(("GDI32: PolyTextOutA"));

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
  dprintf(("GDI32: PolyTextOutW"));

  for (INT x = 0;x < cStrings;x++)
  {
    BOOL rc;

    rc = InternalTextOutW(hdc,pptxt[x].x,pptxt[x].y,pptxt[x].uiFlags,&pptxt[x].rcl,pptxt[x].lpstr,pptxt[x].n,pptxt[x].pdx,TRUE);
    if (!rc) return FALSE;
  }

  return TRUE;
}
