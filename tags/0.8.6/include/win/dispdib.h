/* $Id: dispdib.h,v 1.1 1999-05-24 20:19:11 ktk Exp $ */

/*
 * DISPDIB.dll
 * 
 * Copyright 1998 Ove K�ven
 *
 */

#ifndef __WINE_DISPDIB_H
#define __WINE_DISPDIB_H

#include "wingdi.h" /* for LPBITMAPINFO */

/* error codes */
#define DISPLAYDIB_NOERROR        0x0000
#define DISPLAYDIB_NOTSUPPORTED   0x0001
#define DISPLAYDIB_INVALIDDIB     0x0002
#define DISPLAYDIB_INVALIDFORMAT  0x0003
#define DISPLAYDIB_INVALIDTASK    0x0004

/* flags */
#define DISPLAYDIB_NOPALETTE      0x0010
#define DISPLAYDIB_NOCENTER       0x0020
#define DISPLAYDIB_NOWAIT         0x0040
#define DISPLAYDIB_BEGIN          0x8000
#define DISPLAYDIB_END            0x4000
#define DISPLAYDIB_MODE           0x000F /* mask */
#define DISPLAYDIB_MODE_DEFAULT   0x0000
#define DISPLAYDIB_MODE_320x200x8 0x0001
#define DISPLAYDIB_MODE_320x240x8 0x0005

WORD WINAPI DisplayDib( LPBITMAPINFO lpbi, LPSTR lpBits, WORD wFlags );

#endif /* __WINE_DISPDIB_H */
