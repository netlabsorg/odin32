/* $Id: oslibgdi.cpp,v 1.1 1999-10-01 10:14:59 sandervl Exp $ */

/*
 * GDI32 support code
 *
 * Copyright 1999 Edgar Buerkle (Edgar.Buerkle@gmx.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_GPI
#define  INCL_WIN
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#define  OS2_ONLY
#include "dibsect.h"
#include "oslibgdi.h"

//******************************************************************************
//******************************************************************************
INT OSLibSetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx, DWORD cy, 
                           INT xSrc, INT ySrc, UINT startscan, UINT lines, 
                           LPCVOID bits, WINBITMAPINFOHEADER *info, 
                           UINT coloruse)
{
  INT result;
  POINTL  points[4];

  // This is a quick hack. Only tested with winmine. Need much more testing.
  // TODO: check parameter and info structure
  lines = (int)lines >= 0 ? (int)lines : (int)-lines;

#if 0     // WINE990508
    tmpheight = height = info->bmiHeader.biHeight;
    width = info->bmiHeader.biWidth;
    if (height < 0) height = -height;
    if (!lines || (startscan >= height)) return 0;
    if (startscan + lines > height) lines = height - startscan;
    if (ySrc < startscan) ySrc = startscan;
    else if (ySrc >= startscan + lines) return 0;
    if (xSrc >= width) return 0;
    if (ySrc + cy >= startscan + lines) cy = startscan + lines - ySrc;
    if (xSrc + cx >= width) cx = width - xSrc;
    if (!cx || !cy) return 0;

    descr.dc        = dc;
    descr.bits      = bits;
    descr.image     = NULL;
    descr.lines     = tmpheight >= 0 ? lines : -lines;
    descr.infoWidth = width;
    descr.depth     = dc->w.bitsPerPixel;
    descr.drawable  = physDev->drawable;
    descr.gc        = physDev->gc;
    descr.xSrc      = xSrc;
    descr.ySrc      = tmpheight >= 0 ? lines-(ySrc-startscan)-cy+(oldcy-cy) 
                                     : ySrc - startscan;
    descr.xDest     = dc->w.DCOrgX + XLPTODP( dc, xDest );
    descr.yDest     = dc->w.DCOrgY + YLPTODP( dc, yDest ) +
                                     (tmpheight >= 0 ? oldcy-cy : 0);
    descr.width     = cx;
    descr.height    = cy;
#endif

  points[0].x = xDest;
  points[0].y = yDest + cy - 1;        // Y-inverted
  points[1].x = xDest + cx - 1;
  points[1].y = yDest;                 // Y-inverted
  points[2].x = xSrc;
  points[2].y = ySrc + startscan;
  points[3].x = xSrc + cx;
  points[3].y = ySrc + lines;

  // WINBITMAPINFOHEADER and BITMAPINFO2 are identical
  GpiDrawBits((HPS)hdc, (VOID *)bits, (BITMAPINFO2 *)info, 4,
              points, ROP_SRCCOPY, BBO_IGNORE);
 
  return lines;
}
//******************************************************************************
//******************************************************************************
