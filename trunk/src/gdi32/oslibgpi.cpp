/* $Id: oslibgpi.cpp,v 1.11 2001-05-10 17:03:18 sandervl Exp $ */

/*
 * GPI interface code
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "win32type.h"
#include <winconst.h>
#include "oslibgpi.h"
#include <dcdata.h>
#include <misc.h>

#define DBG_LOCALLOG    DBG_oslibgpi
#include "dbglocal.h"

#define GetDCData(a) ((pDCData)a)

LONG APIENTRY _GpiQueryTabbedTextExtent(HPS hps,LONG lCount,PCH pchString,LONG lTabCount,PULONG puTabStops);

inline LONG GpiQueryTabbedTextExtent(HPS hps,LONG lCount,PCH pchString,LONG lTabCount,PULONG puTabStops)
{
  LONG yyrc;
  USHORT sel = RestoreOS2FS();

  yyrc = _GpiQueryTabbedTextExtent(hps,lCount,pchString,lTabCount,puTabStops);
  SetFS(sel);

  return yyrc;
}

LONG APIENTRY _GpiTabbedCharStringAt(HPS hps,PPOINTL pPtStart,PRECTL prclRect,ULONG flOptions,LONG lCount,PCH pchString,LONG lTabCount,PULONG puTabStops,LONG lTabOrigin);

inline LONG GpiTabbedCharStringAt(HPS hps,PPOINTL pPtStart,PRECTL prclRect,ULONG flOptions,LONG lCount,PCH pchString,LONG lTabCount,PULONG puTabStops,LONG lTabOrigin)
{
  LONG yyrc;
  USHORT sel = RestoreOS2FS();

  yyrc = _GpiTabbedCharStringAt(hps,pPtStart,prclRect,flOptions,lCount,pchString,lTabCount,puTabStops,lTabOrigin);
  SetFS(sel);

  return yyrc;
}

LONG APIENTRY WinDrawTabbedText(HPS hps,LONG cchText,LONG lTabWidth,PCH lpchText,PRECTL prcl,LONG clrFore,LONG clrBack,ULONG flCmd);

inline LONG _WinDrawTabbedText(HPS hps,LONG cchText,LONG lTabWidth,PCH lpchText,PRECTL prcl,LONG clrFore,LONG clrBack,ULONG flCmd)
{
 LONG yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = WinDrawTabbedText(hps,cchText,lTabWidth,lpchText,prcl,clrFore,clrBack,flCmd);
    SetFS(sel);

    return yyrc;
}

#undef  WinDrawTabbedText
#define WinDrawTabbedText _WinDrawTabbedText

void inline swap(LONG &a,LONG &b)
{
  LONG temp = a;

  a = b;
  b = temp;
}

void inline swap(int &a,int &b)
{
  int temp = a;

  a = b;
  b = temp;
}

void inline sortAscending(LONG &a,LONG &b)
{
  if (a > b) swap(a,b);
}

void inline sortAscending(int &a,int &b)
{
  if (a > b) swap(a,b);
}

void inline sortAscending(POINTLOS2 &a,POINTLOS2 &b)
{
  sortAscending(a.x,b.x);
  sortAscending(a.y,b.y);
}

BOOL excludeBottomRightPoint(PVOID pHps,PPOINTLOS2 pptl)
{
  sortAscending(pptl[0],pptl[1]);

  if (GetDCData(pHps)->graphicsMode != GM_COMPATIBLE_W)
  {
    return TRUE;
  }

  if (pptl[0].x == pptl[1].x || pptl[0].y == pptl[1].y)
  {
    return FALSE;
  }

  if (abs((int)GetDCData(pHps)->viewportXExt) <= abs((int)GetDCData(pHps)->windowExt.cx))
  {
    if (GetDCData(pHps)->isLeftLeft)
      pptl[1].x -= abs(GetDCData(pHps)->worldXDeltaFor1Pixel);
    else
      pptl[0].x += abs(GetDCData(pHps)->worldXDeltaFor1Pixel);
  }

  if (abs((int)GetDCData(pHps)->viewportYExt) <= abs((int)GetDCData(pHps)->windowExt.cy))
  {
    if (GetDCData(pHps)->isTopTop)
      pptl[1].y -= abs(GetDCData(pHps)->worldYDeltaFor1Pixel);
    else
      pptl[0].y += abs(GetDCData(pHps)->worldYDeltaFor1Pixel);
  }

  sortAscending(pptl[0], pptl[1]);

  return TRUE;
}

BOOL includeBottomRightPoint(PVOID pHps,PPOINTLOS2 pptl)
{
    if(GetDCData(pHps)->graphicsMode != GM_COMPATIBLE_W)
    {
        return TRUE;  // already inclusive/inclusive
    }

    if(GetDCData(pHps)->isLeftLeft)
    {
        pptl[1].x += abs(GetDCData(pHps)->worldXDeltaFor1Pixel);
        sortAscending(pptl[0].x, pptl[1].x);
    }
    else
    {
        pptl[0].x -= abs(GetDCData(pHps)->worldXDeltaFor1Pixel);
        sortAscending(pptl[1].x, pptl[0].x);
    }

    if(GetDCData(pHps)->isTopTop)
    {
        pptl[1].y += abs(GetDCData(pHps)->worldYDeltaFor1Pixel);
        sortAscending(pptl[1].y, pptl[0].y);
    }
    else
    {
        pptl[0].y -= abs(GetDCData(pHps)->worldYDeltaFor1Pixel);
        sortAscending(pptl[0].y, pptl[1].y);
    }
    return TRUE;
}

BOOL getAlignUpdateCP(PVOID pHps)
{
  return GetDCData(pHps)->alignUpdateCP;
}

INT getWorldYDeltaFor1Pixel(PVOID pHps)
{
  return GetDCData(pHps)->worldYDeltaFor1Pixel;
}

INT getWorldXDeltaFor1Pixel(PVOID pHps)
{
  return GetDCData(pHps)->worldXDeltaFor1Pixel;
}

BOOL getInPath(PVOID pHps)
{
  return GetDCData(pHps)->inPath;
}

VOID setInPath(PVOID pHps,BOOL inPath)
{
  GetDCData(pHps)->inPath = inPath;
}

BOOL getIsWideLine(PVOID pHps)
{
  return GetDCData(pHps)->isWideLine;
}

BOOL getIsTopTop(PVOID pHps)
{
  return GetDCData(pHps)->isTopTop;
}

ULONG getMapMode(PVOID pHps)
{
  return GetDCData(pHps)->MapMode;
}

BOOL OSLibGpiQueryCurrentPosition(PVOID pHps,PPOINTLOS2 ptl)
{
  return GpiQueryCurrentPosition(GetDCData(pHps)->hps,(PPOINTL)ptl);
}

BOOL OSLibGpiSetCurrentPosition(PVOID pHps,PPOINTLOS2 ptl)
{
  return GpiSetCurrentPosition(GetDCData(pHps)->hps,(PPOINTL)ptl);
}

BOOL OSLibGpiCharStringPosAt(PVOID pHps,PPOINTLOS2 ptl,PRECTLOS2 rct,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx)
{
  return GpiCharStringPosAt(GetDCData(pHps)->hps,(PPOINTL)ptl,(PRECTL)rct,flOptions,lCount,(PCH)pchString,(PLONG)alAdx);
}

BOOL OSLibGpiQueryCharStringPosAt(PVOID pHps,PPOINTLOS2 ptl,ULONG flOptions,LONG lCount,LPCSTR pchString,CONST INT *alAdx,PPOINTLOS2 aptlPos)
{
  return GpiQueryCharStringPosAt(GetDCData(pHps)->hps,(PPOINTL)ptl,flOptions,lCount,(PCH)pchString,(PLONG)alAdx,(PPOINTL)aptlPos);
}

BOOL OSLibGpiSetTextAlignment(PVOID pHps,LONG lHoriz,LONG lVert)
{
  return GpiSetTextAlignment(GetDCData(pHps)->hps,lHoriz,lVert);
}

BOOL OSLibGpiQueryTextAlignment(PVOID pHps,PLONG plHoriz,PLONG plVert)
{
  return GpiQueryTextAlignment(GetDCData(pHps)->hps,plHoriz,plVert);
}

LONG OSLibGpiQueryTabbedTextExtent(pDCData pHps,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops)
{
  return GpiQueryTabbedTextExtent(pHps->hps,lCount,(PCH)pchString,lTabCount,(PULONG)puTabStops);
}

LONG OSLibGpiTabbedCharStringAt(PVOID pHps,PPOINTLOS2 pPtStart,PRECTLOS2 prclRect,ULONG flOptions,INT lCount,LPCSTR pchString,INT lTabCount,PINT puTabStops,INT lTabOrigin)
{
  return GpiTabbedCharStringAt(GetDCData(pHps)->hps,(PPOINTL)pPtStart,(PRECTL)prclRect,flOptions,lCount,(PCH)pchString,lTabCount,(PULONG)puTabStops,lTabOrigin);
}

BOOL OSLibGpiQueryTextBox(pDCData pHps,LONG lCount1,LPCSTR pchString,LONG lCount2,PPOINTLOS2 aptlPoints)
{
  return GpiQueryTextBox(pHps->hps,lCount1,(PCH)pchString,lCount2,(PPOINTL)aptlPoints);
}

VOID calcDimensions(POINTLOS2 box[],PPOINTLOS2 point)
{
  ULONG    cx;
  ULONG    cy;

  if (box[TXTBOX_BOTTOMLEFT].y == box[TXTBOX_BOTTOMRIGHT].y)
  {
    point->y = labs (box[TXTBOX_BOTTOMLEFT].y-box[TXTBOX_TOPLEFT].y);
    point->x = labs (box[TXTBOX_BOTTOMRIGHT].x-box[TXTBOX_BOTTOMLEFT].x);

    if (box[TXTBOX_BOTTOMLEFT].x != box[TXTBOX_TOPLEFT].x)
    {
      if (point->y < 25)
        cx = 2;
      else
        cx = ((point->y*10)+50)/100;
      point->x += cx;
    }
  } else
  {
    cx = labs (box[TXTBOX_BOTTOMLEFT].x-box[TXTBOX_TOPLEFT].x);
    cy = labs (box[TXTBOX_BOTTOMLEFT].y-box[TXTBOX_TOPLEFT].y);
    point->y = (ULONG)hypot(cx,cy);

    cx = labs (box[TXTBOX_TOPRIGHT].x-box[TXTBOX_TOPLEFT].x);
    cy = labs (box[TXTBOX_TOPRIGHT].y-box[TXTBOX_TOPLEFT].y);
    point->x  = (ULONG)hypot(cx,cy);
  }
}

LONG OSLibGpiQueryBackMix(PVOID pHps)
{
  return GpiQueryBackMix(GetDCData(pHps)->hps);
}

BOOL doesYAxisGrowNorth(PVOID pHps)
{
  if ((GetDCData(pHps)->windowExt.cy < 0  && GetDCData(pHps)->viewportYExt > 0.0) ||
      (GetDCData(pHps)->windowExt.cy > 0  && GetDCData(pHps)->viewportYExt < 0.0))
  {
    if (GetDCData(pHps)->graphicsMode == GM_COMPATIBLE_W ||
        (GetDCData(pHps)->graphicsMode == GM_ADVANCED_W && GetDCData(pHps)->xform.eM22 >= 0.0))
      return TRUE;
  } else
  {
    if (GetDCData(pHps)->graphicsMode == GM_ADVANCED_W && GetDCData(pHps)->xform.eM22 < 0.0)
      return TRUE;
  }

  return FALSE;
}

LONG OSLibWinDrawTabbedText(PVOID pHps,LONG cchText,LONG lTabs,LPCSTR lpchText,PVOID prcl,LONG clrFore,LONG clrBack,ULONG flCmd)
{
  return WinDrawTabbedText(GetDCData(pHps)->hps,cchText,lTabs,(PCH)lpchText,(PRECTL)prcl,clrFore,clrBack,flCmd);
}

BOOL OSLibGpiMove(PVOID pHps,PPOINTLOS2 pptlPoint)
{
  return GpiMove(GetDCData(pHps)->hps,(PPOINTL)pptlPoint);
}

LONG OSLibGpiLine(PVOID pHps,PPOINTLOS2 pptlEndPoint)
{
  return GpiLine(GetDCData(pHps)->hps,(PPOINTL)pptlEndPoint);
}

#define FSP_ENDPATH   0x00000010
#define FSP_FILL      0x00000020
#define FSP_CLOSEPATH 0x00000040

BOOL APIENTRY _PaxStrokeAndFillPath(HPS hPS,ULONG ulAction,ULONG ulStrokeAttrs,PAREABUNDLE pPenStroke);

inline BOOL PaxStrokeAndFillPath(HPS hPS,ULONG ulAction,ULONG ulStrokeAttrs,PAREABUNDLE pPenStroke)
{
  BOOL yyrc;
  USHORT sel = RestoreOS2FS();

  yyrc = _PaxStrokeAndFillPath(hPS,ulAction,ulStrokeAttrs,pPenStroke);
  SetFS(sel);

  return yyrc;
}


BOOL OSLibGpiEndPath(PVOID pHps)
{
  return GpiEndPath(GetDCData(pHps)->hps);
}

BOOL drawLinePointCircle(PVOID pHps,INT width,INT height,LONG color)
{
  ARCPARAMS arcp;
  BOOL rc = TRUE;

  arcp.lP = 1;
  arcp.lQ = 1;
  arcp.lR = 0;
  arcp.lS = 0;
  if (!GpiSetArcParams(GetDCData(pHps)->hps,&arcp))
    return FALSE;

  AREABUNDLE newAreaBundle, oldAreaBundle;
  LINEBUNDLE lineBundle;

  GpiQueryAttrs(GetDCData(pHps)->hps,PRIM_AREA,ABB_COLOR | ABB_MIX_MODE | ABB_SET | ABB_SYMBOL,(PBUNDLE)&oldAreaBundle);
  GpiQueryAttrs(GetDCData(pHps)->hps,PRIM_LINE,LBB_MIX_MODE, (PBUNDLE)&lineBundle);

  newAreaBundle           = oldAreaBundle;
  newAreaBundle.lColor    = color;
  newAreaBundle.usMixMode = lineBundle.usMixMode;
  newAreaBundle.usSet     = LCID_DEFAULT;
  newAreaBundle.usSymbol  = PATSYM_SOLID;

  if (!GpiSetAttrs(GetDCData(pHps)->hps,PRIM_AREA,ABB_COLOR | ABB_MIX_MODE | ABB_SET | ABB_SYMBOL,0,(PBUNDLE)&newAreaBundle))
    return FALSE;

  if (GpiFullArc(GetDCData(pHps)->hps,DRO_FILL,MAKEFIXED((width-1)>>1,0)) == GPI_ERROR)
    rc = FALSE;
  GpiSetAttrs(GetDCData(pHps)->hps,PRIM_AREA,ABB_COLOR | ABB_MIX_MODE | ABB_SET | ABB_SYMBOL,0,(PBUNDLE)&oldAreaBundle);

  return rc;
}

BOOL drawLinePoint(PVOID pHps,PPOINTLOS2 pt,LONG color)
{
  LINEBUNDLE lbOld, lbNew;
  LONG defaults = GpiQueryAttrs(GetDCData(pHps)->hps, PRIM_LINE, LBB_COLOR, &lbOld);

  lbNew.lColor = color;
  BOOL rc = GpiSetAttrs(GetDCData(pHps)->hps,PRIM_LINE,LBB_COLOR,0,&lbNew) && GpiSetPel(GetDCData(pHps)->hps,(PPOINTL)pt) != GPI_ERROR;

  GpiSetAttrs(GetDCData(pHps)->hps,PRIM_LINE,LBB_COLOR,defaults,&lbOld);

  return rc;
}

ULONG OSLibGpiQueryCp(HDC hdc)
{
  return GpiQueryCp(hdc);
}

BOOL OSLibGpiSetCp(HDC hdc, ULONG codepage)
{
  return GpiSetCp(hdc, codepage);
}


int OSLibGpiQueryFontMaxHeight(HDC hdc) 
{
  FONTMETRICS metrics;
  BOOL rc;

  rc = GpiQueryFontMetrics(hdc, sizeof(metrics), &metrics);
  if(rc) {
  	return metrics.lMaxAscender;
  }
  else {
	dprintf(("GpiQueryFontMetrics returned FALSE!!"));
	return 0;
  }
}

#ifdef DEBUG
void dprintfOrigin(HDC hdc)
{
   POINTL point;

    pDCData  pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
    if(!pHps)
    {
	return;
    }

    GreGetDCOrigin(pHps->hps, &point);
    dprintf2(("HDC %x origin (%d,%d) org (%d,%d)", hdc, point.x, point.y, pHps->ptlOrigin.x, pHps->ptlOrigin.y));
}
#endif

BOOL OSLibDevQueryCaps(pDCData pHps, LONG lStart, LONG lCount, LONG *alArray)
{
   return DevQueryCaps(pHps->hdc, lStart, lCount, alArray);
}
