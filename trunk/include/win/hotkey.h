/* $Id: hotkey.h,v 1.3 1999-09-19 13:26:47 cbratschi Exp $ */

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
} HOTKEY_INFO;


extern VOID HOTKEY_Register (VOID);
extern VOID HOTKEY_Unregister (VOID);

#endif  /* __WINE_HOTKEY_H */
