/* $Id: hotkey.h,v 1.1 1999-05-24 20:19:12 ktk Exp $ */

/*
 * Hotkey class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_HOTKEY_H
#define __WINE_HOTKEY_H

typedef struct tagHOTKEY_INFO
{
    HFONT hFont;
    BOOL  bFocus;
    INT   nHeight;

} HOTKEY_INFO;


extern VOID HOTKEY_Register (VOID);
extern VOID HOTKEY_Unregister (VOID);

#endif  /* __WINE_HOTKEY_H */
