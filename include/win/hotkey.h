/* $Id: hotkey.h,v 1.6 2001-03-31 13:23:33 sandervl Exp $ */

/*
 * Hotkey class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_HOTKEY_H
#define __WINE_HOTKEY_H

typedef struct tagHOTKEY_INFO
{
    COMCTL32_HEADER header;

    HFONT hFont;
    BOOL  bFocus;
    INT   nHeight;
    BYTE  bVKHotKey;
    BYTE  bfMods;
    INT   fwCombInv;
    INT   fwModInv;
    POINT cursorPos;
    BYTE  bfModsDown;
} HOTKEY_INFO;



#endif  /* __WINE_HOTKEY_H */
