/* $Id: dib.h,v 1.1 2001-05-11 08:35:17 sandervl Exp $ */

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

//must use WIN32API as it's exported
int WIN32API DIB_GetDIBWidthBytes( int width, int depth );

int DIB_GetDIBImageBytes( int width, int height, int depth );
int DIB_BitmapInfoSize( BITMAPINFO * info, WORD coloruse );
int DIB_GetBitmapInfo( const BITMAPINFOHEADER *header, DWORD *width,
                              int *height, WORD *bpp, WORD *compr );
void DIB_FixColorsToLoadflags(BITMAPINFO * bmi, UINT loadflags, BYTE pix);
int  WIN32API BITMAP_GetWidthBytes( INT width, INT depth );

#endif
