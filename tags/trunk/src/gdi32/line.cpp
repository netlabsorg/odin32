/* $Id: line.cpp,v 1.8 2001-01-18 18:13:17 sandervl Exp $ */
/*
 * Line API's
 *
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <math.h>
#include "misc.h"
#include "callback.h"
#include "oslibgpi.h"
#include <dcdata.h>

#define DBG_LOCALLOG	DBG_line
#include "dbglocal.h"

#define ROUND_FLOAT(x) ((INT)((x < 0) ? x-0.5:x+0.5))

VOID toWin32LineEnd(PPOINTLOS2 startPt,INT nXEnd,INT nYEnd,PPOINTLOS2 pt)
{
  if ((startPt->x != nXEnd) || (startPt->y != nYEnd))
  {
    if (nXEnd == startPt->x)
    {
      pt->x = nXEnd;
      pt->y = (nYEnd > startPt->y) ? nYEnd-1:nYEnd+1;
    } else if (nYEnd == startPt->y)
    {
      pt->x = (nXEnd > startPt->x) ? nXEnd-1:nXEnd+1;
      pt->y = nYEnd;
    } else
    {
      INT w = nXEnd-startPt->x,h = nYEnd-startPt->y;
      DOUBLE len = hypot(w,h);
      DOUBLE lenQuot = (len-1)/len;

      pt->x = startPt->x+ROUND_FLOAT(w*lenQuot);
      pt->y = startPt->y+ROUND_FLOAT(h*lenQuot);
    }
  } else
  {
    pt->x = nXEnd;
    pt->y = nYEnd;
  }
}

BOOL drawSingleLinePoint(HDC hdc,pDCData pHps,PPOINTLOS2 pt)
{
  LOGPEN penInfo;

  if (!GetObjectA(GetCurrentObject(hdc,OBJ_PEN),sizeof(penInfo),(LPVOID)&penInfo)) return FALSE;

  if ((penInfo.lopnWidth.x > 1) || (penInfo.lopnWidth.y > 1))
  {
    if ((penInfo.lopnStyle != PS_INSIDEFRAME) && (penInfo.lopnStyle != PS_SOLID))
      return FALSE;

    LONG color = GetBValue(penInfo.lopnColor) | (GetGValue(penInfo.lopnColor)<<8) | (GetRValue(penInfo.lopnColor)<<16);

    return drawLinePointCircle(pHps,penInfo.lopnWidth.x,penInfo.lopnWidth.y,color);
  } else
  {
    LONG color = GetBValue(penInfo.lopnColor) | (GetGValue(penInfo.lopnColor)<<8) | (GetRValue(penInfo.lopnColor)<<16);

    return drawLinePoint(pHps,pt,color);
  }
}

BOOL WIN32API MoveToEx( HDC hdc, int X, int Y, LPPOINT lpPoint)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);

  dprintf(("GDI32: MoveToEx %x (%d,%d)", hdc, X, Y));

  if (pHps)
  {
    POINTLOS2 newPoint = {X,Y};

#ifndef INVERT
    if(pHps->yInvert > 0) {
         newPoint.y =  pHps->yInvert - newPoint.y;
         if (lpPoint) {
            lpPoint->y = pHps->yInvert - lpPoint->y;
         }
    }
#endif

    if (lpPoint)
    {
      POINTLOS2 lastPoint;

      OSLibGpiQueryCurrentPosition(pHps,&lastPoint);
      lpPoint->x = lastPoint.x;
      lpPoint->y = lastPoint.y;
    }

    if (OSLibGpiMove(pHps,&newPoint))
    {
      //CB: add metafile info
      return TRUE;
    }

    return FALSE;
  }

  SetLastError(ERROR_INVALID_HANDLE);
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LineTo( HDC hdc, int nXEnd, int  nYEnd)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);
  BOOL rc = TRUE;

  dprintf(("GDI32: LineTo %x (%d,%d)", hdc, nXEnd, nYEnd));

  if (pHps)
  {
    POINTLOS2 oldPoint,newPoint;
    BOOL bWideLine;

#ifndef INVERT
    if (pHps->yInvert > 0) {
       nYEnd = pHps->yInvert - nYEnd;
    }
#endif

    //CB: add metafile info

    OSLibGpiQueryCurrentPosition(pHps,&oldPoint);
    toWin32LineEnd(&oldPoint,nXEnd,nYEnd,&newPoint);

    if ((oldPoint.x == newPoint.x) && (oldPoint.y == newPoint.y))
    {
      rc = drawSingleLinePoint(hdc,pHps,&newPoint);
    } else
    {
      if (getIsWideLine(pHps))
      {
        rc = O32_LineTo(hdc,newPoint.x,newPoint.y); //CB: wide line not supported
      } else
      {
        if (OSLibGpiLine(pHps,&newPoint) == 0)
          rc = FALSE;
      }
    }

    newPoint.x = nXEnd;
    newPoint.y = nYEnd;
    OSLibGpiMove(pHps,&newPoint);
  } else
  {
    SetLastError(ERROR_INVALID_HANDLE);
    rc = FALSE;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LineDDA( int nXStart, int nYStart, int nXEnd, int nYEnd, LINEDDAPROC lpLineFunc, LPARAM lpData)
{
  BOOL                 rc;
#ifndef STDCALL_ENUMPROCS
  LineDDAProcCallback *callback = new LineDDAProcCallback(lpLineFunc, lpData);
#endif
  POINTLOS2 startPt,endPt;

  dprintf(("GDI32: LineDDA\n"));
#if 0 //CB: the Open32 function is ok -> to check
  startPt.x = nXStart;
  startPt.y = nYStart;
  toWin32LineEnd(&startPt,nXEnd,nYEnd,&endPt);

  rc = O32_LineDDA(startPt.x,startPt.y,endPt.x,endPt.y, lpLineFunc, lpData);
#else
#ifdef STDCALL_ENUMPROCS
  //should change os2win.h
  rc = O32_LineDDA(nXStart,nYStart,nXEnd,nYEnd, (LINEDDAPROC_O32)lpLineFunc, lpData);
#else
  rc = O32_LineDDA(nXStart,nYStart,nXEnd,nYEnd,callback->GetOS2Callback(),(LPARAM)callback);
  if(callback)
        delete callback;
#endif

#endif
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polyline( HDC hdc, const POINT *lppt, int cPoints)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);

  dprintf(("GDI32: Polyline %x %x %d", hdc, lppt, cPoints));

  if (!pHps)
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  if (cPoints == 0) return TRUE;
  if (cPoints < 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  if (cPoints == 1)
  {
    drawSingleLinePoint(hdc,pHps,(PPOINTLOS2)lppt); //CB: check metafile recording
    return TRUE;
  }

  POINT *points = (POINT*)lppt;
  POINT lastPt = lppt[cPoints-1];
  BOOL rc;

  toWin32LineEnd((PPOINTLOS2)&lppt[cPoints-2],lastPt.x,lastPt.y,(PPOINTLOS2)&points[cPoints-1]);
  rc = O32_Polyline(hdc,lppt,cPoints);
  points[cPoints-1] = lastPt;

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolylineTo( HDC hdc, const POINT * lppt, DWORD cCount)
{
  pDCData pHps = (pDCData)OSLibGpiQueryDCData(hdc);

  dprintf(("GDI32: PolylineTo"));

  if (!pHps)
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  if (cCount == 0) return TRUE;

  //CB: add metafile info

  if (cCount == 1)
  {
    drawSingleLinePoint(hdc,pHps,(PPOINTLOS2)lppt);
    return TRUE;
  }

  POINT *points = (POINT*)lppt;
  POINT lastPt = lppt[cCount-1];
  BOOL rc;

  toWin32LineEnd((PPOINTLOS2)&lppt[cCount-2],lastPt.x,lastPt.y,(PPOINTLOS2)&points[cCount-1]);
  rc = O32_PolylineTo(hdc,lppt,cCount);
  points[cCount-1] = lastPt;
  OSLibGpiMove(pHps,(PPOINTLOS2)&lastPt);

  return rc;
}
//******************************************************************************
//******************************************************************************

