/* $Id: font.h,v 1.2 2003-07-28 11:25:51 sandervl Exp $ */

/*
 * GDI font definitions
 *
 * Copyright 1994 Alexandre Julliard
 */

#ifndef __WINE_FONT_H
#define __WINE_FONT_H

#include "pshpack1.h"

typedef struct {
    WORD	dfVersion;
    DWORD	dfSize;
    CHAR	dfCopyright[60];
    WORD	dfType;
    WORD	dfPoints;
    WORD	dfVertRes;
    WORD	dfHorizRes;
    WORD	dfAscent;
    WORD	dfInternalLeading;
    WORD	dfExternalLeading;
    BYTE	dfItalic;
    BYTE	dfUnderline;
    BYTE	dfStrikeOut;
    WORD	dfWeight;
    BYTE	dfCharSet;
    WORD	dfPixWidth;
    WORD	dfPixHeight;
    BYTE	dfPitchAndFamily;
    WORD	dfAvgWidth;
    WORD	dfMaxWidth;
    BYTE	dfFirstChar;
    BYTE	dfLastChar;
    BYTE	dfDefaultChar;
    BYTE	dfBreakChar;
    WORD	dfWidthBytes;
    DWORD	dfDevice;
    DWORD	dfFace;
    DWORD	dfReserved;
    CHAR	szDeviceName[60]; /* FIXME: length unknown */
    CHAR	szFaceName[60];   /* dito */
} FONTDIR16, *LPFONTDIR16;

#include "poppack.h"

#define FONTCACHE 	32	/* dynamic font cache size */




#endif /* __WINE_FONT_H */
