/* $Id: oslibgpi.cpp,v 1.1 1999-12-09 16:49:45 cbratschi Exp $ */

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
#include "win32type.h"
#include "oslibgpi.h"
#include "dcdata.h"

#define GetDCData(a) ((pDCData)a)

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
