/* $Id: oslibgdi.cpp,v 1.7 2001-06-08 11:03:33 sandervl Exp $ */

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
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#define  OS2_ONLY
#include "dibsect.h"
#include "oslibgdi.h"

#define DBG_LOCALLOG	DBG_oslibgdi
#include "dbglocal.h"

#if 0
//******************************************************************************
//******************************************************************************
INT OSLibSetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx, DWORD cy,
                           INT xSrc, INT ySrc, UINT startscan, UINT lines,
                           LPCVOID bits, BITMAPINFOHEADER_W *info,
                           UINT coloruse)
{
  INT result, rc;
  POINTL  points[4];
  BITMAPINFO2 os2bmpinfo;

  // This is a quick hack. Only tested with winmine. Need much more testing.
  // TODO: check parameter and info structure
  lines = (int)lines >= 0 ? (int)lines : (int)-lines;

  points[0].x = xDest;
  points[0].y = yDest + cy - 1;        // Y-inverted
  points[1].x = xDest + cx - 1;
  points[1].y = yDest;                 // Y-inverted
  points[2].x = xSrc;
  points[2].y = ySrc + startscan;
  points[3].x = xSrc + cx;
  points[3].y = ySrc + lines;

  memset(&os2bmpinfo, 0, sizeof(os2bmpinfo));  
  os2bmpinfo.cbFix         = sizeof(BITMAPINFO2) - sizeof(RGB2);
  os2bmpinfo.cx            = info->biWidth;
  os2bmpinfo.cy            = info->biHeight;
  os2bmpinfo.cPlanes       = info->biPlanes;
  os2bmpinfo.cBitCount     = info->biBitCount;
  os2bmpinfo.ulCompression = info->biCompression;
  os2bmpinfo.cbImage       = info->biSizeImage;
  os2bmpinfo.cxResolution  = info->biXPelsPerMeter;
  os2bmpinfo.cyResolution  = info->biYPelsPerMeter;
  os2bmpinfo.cclrUsed      = info->biClrUsed;
  os2bmpinfo.cclrImportant = info->biClrImportant;

  rc = GpiDrawBits((HPS)hdc, (VOID *)bits, &os2bmpinfo, 4,
                   points, ROP_SRCCOPY, BBO_IGNORE);

  if(rc != GPI_OK) {
	dprintf(("GpiDrawBits returned %d", rc));
  }
  return lines;
}
//******************************************************************************
//******************************************************************************
#endif