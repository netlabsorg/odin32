/* $Id: line.cpp,v 1.1 1999-12-03 17:31:51 cbratschi Exp $ */
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

//CB: todo: rewrite them without using Open32

#define ROUND_FLOAT(x) ((INT)((x < 0) ? x-0.5:x+0.5))
#define sqr(x) (pow(x,2))

POINT ToWin32LineEnd(POINT startPt,INT nXEnd,INT nYEnd)
{
  POINT pt;

  if (startPt.x != nXEnd || startPt.y != nYEnd)
  {
    if (nXEnd == startPt.x)
    {
      pt.x = nXEnd;
      pt.y = (nYEnd > startPt.y) ? nYEnd-1:nYEnd+1;
    } else if (nYEnd == startPt.y)
    {
      pt.x = (nXEnd > startPt.x) ? nXEnd-1:nXEnd+1;
      pt.y = nYEnd;
    } else
    {
      DOUBLE len = sqrt(sqr(nXEnd-startPt.x)+sqr(nYEnd-startPt.y));
      DOUBLE lenDif = (len-1)/len;
      INT w = nXEnd-startPt.x,h = nYEnd-startPt.y;

      pt.x = startPt.x+ROUND_FLOAT(w*lenDif);
      pt.y = startPt.y+ROUND_FLOAT(h*lenDif);
    }
  } else
  {
    pt.x = nXEnd;
    pt.y = nYEnd;
  }

  return pt;
}

VOID DrawSingleLinePoint(HDC hdc,POINT pt)
{
  LOGPEN penInfo;

  if (!GetObjectA(GetCurrentObject(hdc,OBJ_PEN),sizeof(penInfo),(LPVOID)&penInfo)) return;
  if (penInfo.lopnWidth.x <= 1 && penInfo.lopnWidth.y <= 1) SetPixel(hdc,pt.x,pt.y,penInfo.lopnColor); else
  {
    INT x = pt.x-penInfo.lopnWidth.x/2;
    INT y = pt.y-penInfo.lopnWidth.y/2;
    Ellipse(hdc,x,y,x+penInfo.lopnWidth.x,y+penInfo.lopnWidth.y);
  }
}

BOOL WIN32API LineTo( HDC hdc, int nXEnd, int  nYEnd)
{
  POINT oldPt,pt;

  dprintf(("GDI32: LineTo"));

  //CB: Open32 draws a pixel too much!
  GetCurrentPositionEx(hdc,&oldPt);
  pt = ToWin32LineEnd(oldPt,nXEnd,nYEnd);

  BOOL rc;

  if (oldPt.x == pt.x && oldPt.y == pt.y)
  {
    DrawSingleLinePoint(hdc,pt);

    rc = TRUE;
  } else rc = O32_LineTo(hdc,pt.x,pt.y);
  MoveToEx(hdc,nXEnd,nYEnd,NULL);

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API LineDDA( int nXStart, int nYStart, int nXEnd, int nYEnd, LINEDDAPROC lpLineFunc, LPARAM lpData)
{
 BOOL                 rc;
 LineDDAProcCallback *callback = new LineDDAProcCallback(lpLineFunc, lpData);
 POINT startPt,endPt;

  dprintf(("GDI32: LineDDA\n"));

  //CB: don't know if Open32 reports the last pixel, but all other line functions do
  startPt.x = nXStart;
  startPt.y = nYStart;
  endPt = ToWin32LineEnd(startPt,nXEnd,nYEnd);

  rc = O32_LineDDA(startPt.x,startPt.y,endPt.x,endPt.y,callback->GetOS2Callback(),(LPARAM)callback);
  if(callback)
        delete callback;
  return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Polyline( HDC hdc, const POINT *lppt, int cPoints)
{
    dprintf(("GDI32: Polyline"));

    if (cPoints == 0) return TRUE;
    if (cPoints < 0)
    {
      SetLastError(ERROR_INVALID_PARAMETER);

      return FALSE;
    }

    if (cPoints == 1)
    {
      DrawSingleLinePoint(hdc,*lppt); //CB: check metafile recording

      return TRUE;
    }

    //CB: Open32 draw a pixel too much!
    POINT *points = (POINT*)lppt;
    POINT lastPt = lppt[cPoints-1];
    BOOL rc;

    points[cPoints-1] = ToWin32LineEnd(lppt[cPoints-2],lastPt.x,lastPt.y);
    rc = O32_Polyline(hdc,lppt,cPoints);
    points[cPoints-1] = lastPt;

    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PolylineTo( HDC hdc, const POINT * lppt, DWORD cCount)
{
    dprintf(("GDI32: PolylineTo"));

    if (cCount == 0) return TRUE;

    if (cCount == 1)
    {
      DrawSingleLinePoint(hdc,*lppt);

      return TRUE; //CB: check metafile recording
    }

    //CB: Open32 draw a pixel too much!
    POINT *points = (POINT*)lppt;
    POINT lastPt = lppt[cCount-1];
    BOOL rc;

    points[cCount-1] = ToWin32LineEnd(lppt[cCount-2],lastPt.x,lastPt.y);
    rc = O32_PolylineTo(hdc,lppt,cCount);
    points[cCount-1] = lastPt;
    MoveToEx(hdc,lastPt.x,lastPt.y,NULL);

    return rc;
}
//******************************************************************************
//******************************************************************************

