/* $Id: dib.h,v 1.3 2000-01-01 14:54:51 cbratschi Exp $ */

/*
 * Win32 DIB functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 Port)
 *
 * Based on Wine code (objects\dib.c):
 *
 * GDI device-independent bitmaps
 *
 * Copyright 1993,1994  Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __DIB_H__
#define __DIB_H__

int DIB_GetDIBWidthBytes( int width, int depth );
int DIB_GetDIBImageBytes( int width, int height, int depth );
int DIB_BitmapInfoSize( BITMAPINFO * info, WORD coloruse );
int DIB_GetBitmapInfo( const BITMAPINFOHEADER *header, DWORD *width,
                              int *height, WORD *bpp, WORD *compr );
void DIB_FixColorsToLoadflags(BITMAPINFO * bmi, UINT loadflags, BYTE pix);

#endif
