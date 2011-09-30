/* $Id: oslibgdi.h,v 1.3 2000-02-02 23:45:07 sandervl Exp $ */

/*
 * GDI32 support code
 *
 * Copyright 1999 Edgar Buerkle (Edgar.Buerkle@gmx.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBGDI_H__
#define __OSLIBGDI_H__

INT OSLibSetDIBitsToDevice(HDC hdc, INT xDest, INT yDest, DWORD cx, DWORD cy,
                           INT xSrc, INT ySrc, UINT startscan, UINT lines,
                           LPCVOID bits, BITMAPINFOHEADER_W *info,
                           UINT coloruse);

#endif
