/* $Id: hotkey.h,v 1.4 1999-09-23 15:49:57 cbratschi Exp $ */

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


extern VOID HOTKEY_Register (VOID);
extern VOID HOTKEY_Unregister (VOID);

#endif  /* __WINE_HOTKEY_H */
