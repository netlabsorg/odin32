/* $Id: dib.cpp,v 1.1 2001-05-11 08:36:26 sandervl Exp $ */

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
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <misc.h>
#include "dib.h"

#define DBG_LOCALLOG	DBG_dib
#include "dbglocal.h"

/***********************************************************************
 *           DIB_GetDIBWidthBytes
 *
 * Return the width of a DIB bitmap in bytes. DIB bitmap data is 32-bit aligned.
 * http://www.microsoft.com/msdn/sdk/platforms/doc/sdk/win32/struc/src/str01.htm
 */
int WIN32API DIB_GetDIBWidthBytes( int width, int depth )
{
    int words;

    switch(depth)
    {
	case 1:  words = (width + 31) / 32; break;
	case 4:  words = (width + 7) / 8; break;
	case 8:  words = (width + 3) / 4; break;
	case 15:
	case 16: words = (width + 1) / 2; break;
	case 24: words = (width * 3 + 3)/4; break;

	default:
            dprintf(("(%d): Unsupported depth\n", depth ));
	/* fall through */
	case 32:
	        words = width;
    }
    return 4 * words;
}

/***********************************************************************
 *           DIB_GetDIBImageBytes
 *
 * Return the number of bytes used to hold the image in a DIB bitmap.
 */
int DIB_GetDIBImageBytes( int width, int height, int depth )
{
    return DIB_GetDIBWidthBytes( width, depth ) * abs( height );
}


/***********************************************************************
 *           DIB_BitmapInfoSize
 *
 * Return the size of the bitmap info structure including color table.
 */
int DIB_BitmapInfoSize( BITMAPINFO * info, WORD coloruse )
{
    int colors;

    if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
    {
        BITMAPCOREHEADER *core = (BITMAPCOREHEADER *)info;
        colors = (core->bcBitCount <= 8) ? 1 << core->bcBitCount : 0;
        return sizeof(BITMAPCOREHEADER) + colors *
             ((coloruse == DIB_RGB_COLORS) ? sizeof(RGBTRIPLE) : sizeof(WORD));
    }
    else  /* assume BITMAPINFOHEADER */
    {
        colors = info->bmiHeader.biClrUsed;
        if (!colors && (info->bmiHeader.biBitCount <= 8))
            colors = 1 << info->bmiHeader.biBitCount;
        return sizeof(BITMAPINFOHEADER) + colors *
               ((coloruse == DIB_RGB_COLORS) ? sizeof(RGBQUAD) : sizeof(WORD));
    }
}


/***********************************************************************
 *           DIB_GetBitmapInfo
 *
 * Get the info from a bitmap header.
 * Return 1 for INFOHEADER, 0 for COREHEADER, -1 for error.
 */
int DIB_GetBitmapInfo( const BITMAPINFOHEADER *header, DWORD *width,
                              int *height, WORD *bpp, WORD *compr )
{
    if (header->biSize == sizeof(BITMAPINFOHEADER))
    {
        *width  = header->biWidth;
        *height = header->biHeight;
        *bpp    = header->biBitCount;
        *compr  = header->biCompression;
        return 1;
    }
    if (header->biSize == sizeof(BITMAPCOREHEADER))
    {
        BITMAPCOREHEADER *core = (BITMAPCOREHEADER *)header;
        *width  = core->bcWidth;
        *height = core->bcHeight;
        *bpp    = core->bcBitCount;
        *compr  = 0;
        return 0;
    }
    dprintf(("(%ld): wrong size for header\n", header->biSize ));
    return -1;
}

/***********************************************************************
 *           DIB_FixColorsToLoadflags
 *
 * Change color table entries when LR_LOADTRANSPARENT or LR_LOADMAP3DCOLORS
 * are in loadflags
 */
void DIB_FixColorsToLoadflags(BITMAPINFO * bmi, UINT loadflags, BYTE pix)
{
  int colors, bitcount;
  COLORREF c_W, c_S, c_F, c_L, c_C;
  int incr,i;
  RGBQUAD *ptr;
  char *colorptr;

  //SvL: Wine code doesn't work for OS/2 1.3 bitmaps
  if (bmi->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
  {
    if (bmi->bmiHeader.biBitCount > 8) return;
    colors   = bmi->bmiHeader.biClrUsed;
    bitcount = bmi->bmiHeader.biBitCount;
    colorptr = (char*)bmi->bmiColors;
    incr     = 4;
  }
  else
    if (bmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
    {
      BITMAPCOREHEADER *core = (BITMAPCOREHEADER *)bmi;

      if (core->bcBitCount > 8) return;
      colors   = (1 << core->bcBitCount);
      bitcount = core->bcBitCount;
      colorptr = (char*)(core + 1);
      incr     = 3;
    }
    else
    {
      dprintf(("Wrong bitmap header size!\n"));
      return;
    }

  if (!colors && (bitcount <= 8))
    colors = 1 << bitcount;

  c_W = GetSysColor(COLOR_WINDOW);
  c_S = GetSysColor(COLOR_3DSHADOW);
  c_F = GetSysColor(COLOR_3DFACE);
  c_L = GetSysColor(COLOR_3DLIGHT);
  if (loadflags & LR_LOADTRANSPARENT) {
    	switch (bitcount) {
      	case 1: pix = pix >> 7; break;
      	case 4: pix = pix >> 4; break;
      	case 8: break;
      	default:
        	dprintf(("(%d): Unsupported depth\n", bitcount));
		return;
    	}
    	if (pix >= colors) {
      		dprintf(("pixel has color index greater than biClrUsed!\n"));
      		return;
    	}
    	if (loadflags & LR_LOADMAP3DCOLORS) c_W = c_F;
    		ptr = (RGBQUAD*)((char*)colorptr+pix*incr);

    	ptr->rgbBlue = GetBValue(c_W);
  	ptr->rgbGreen = GetGValue(c_W);
  	ptr->rgbRed = GetRValue(c_W);
  }
  if (loadflags & LR_LOADMAP3DCOLORS) {
  	for (i=0; i<colors; i++) {
  		ptr = (RGBQUAD*)(colorptr +i*incr);
  		c_C = RGB(ptr->rgbRed, ptr->rgbGreen, ptr->rgbBlue);
  		if (c_C == RGB(128, 128, 128)) {
			ptr->rgbRed = GetRValue(c_S);
			ptr->rgbGreen = GetGValue(c_S);
			ptr->rgbBlue = GetBValue(c_S);
      		} else if (c_C == RGB(192, 192, 192)) {
			ptr->rgbRed = GetRValue(c_F);
			ptr->rgbGreen = GetGValue(c_F);
			ptr->rgbBlue = GetBValue(c_F);
      		} else if (c_C == RGB(223, 223, 223)) {
			ptr->rgbRed = GetRValue(c_L);
			ptr->rgbGreen = GetGValue(c_L);
			ptr->rgbBlue = GetBValue(c_L);
      		}
    	}
  }

}

/***********************************************************************
 *           BITMAP_GetWidthBytes
 *
 * Return number of bytes taken by a scanline of 16-bit aligned Windows DDB
 * data.
 */
int WIN32API BITMAP_GetWidthBytes( INT bmWidth, INT bpp )
{
    switch(bpp)
    {
    case 1:
	return 2 * ((bmWidth+15) >> 4);

    case 24:
	bmWidth *= 3; /* fall through */
    case 8:
	return bmWidth + (bmWidth & 1);

    case 32:
	return bmWidth * 4;

    case 16:
    case 15:
	return bmWidth * 2;

    case 4:
	return 2 * ((bmWidth+3) >> 2);

    default:
	dprintf(("BITMAP_GetWidthBytes: Unknown depth %d, please report.\n", bpp ));
    }
    return -1;
}
