/*
 * ImageList definitions
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 */
 
#ifndef _IMAGELIST_H
#define _IMAGELIST_H

#include "wingdi.h"

struct _IMAGELIST
{
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    HBRUSH  hbrBlend25;
    HBRUSH  hbrBlend50;
    COLORREF  clrBk;
    COLORREF  clrFg;
    INT     cInitial;
    INT     cGrow;
    INT     cMaxImage;
    INT     cCurImage;
    INT     cx;
    INT     cy;
    UINT    flags;
    UINT    uBitsPixel;
    INT     nOvlIdx[15];
};
 
typedef struct _IMAGELIST *HIMAGELIST;

#endif  /* _IMAGELIST_H */

