/* $Id: imagelist.h,v 1.4 2000-02-04 17:00:23 cbratschi Exp $ */
/*
 * ImageList definitions
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 */

#ifndef _IMAGELIST_H
#define _IMAGELIST_H

#include "wingdi.h"

/* the ones with offsets at the end are the same as in Windows */
struct _IMAGELIST
{
    DWORD	magic; 			/* 00:	'LMIH' */
    INT		cCurImage;		/* 04: ImageCount */
    INT		cMaxImage;		/* 08: maximages */
    DWORD	x3;
    INT		cx;			/* 10: cx */
    INT		cy;			/* 14: cy */
    DWORD	x4;
    UINT	flags;			/* 1c: flags */
    DWORD   	x5;
    COLORREF	clrBk;			/* 24: bkcolor */


    /* not yet found out */
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    HBRUSH  hbrBlend25;
    HBRUSH  hbrBlend50;
    COLORREF  clrFg;
    INT     cInitial;
    INT     cGrow;
    UINT    uBitsPixel;
    INT     nOvlIdx[15];
};

typedef struct _IMAGELIST *HIMAGELIST;

/* Header used by ImageList_Read() and ImageList_Write() */
typedef struct _ILHEAD
{
    USHORT	usMagic;
    USHORT	usVersion;
    WORD	cCurImage;
    WORD	cMaxImage;
    WORD	cGrow;
    WORD	cx;
    WORD	cy;
    COLORREF	bkcolor;
    WORD	flags;
    SHORT	ovls[4];
} ILHEAD;

#endif  /* _IMAGELIST_H */

