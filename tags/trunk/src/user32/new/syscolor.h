/* $Id: syscolor.h,v 1.4 2000-01-09 17:13:39 cbratschi Exp $ */

/*
 * WINE:
 * System color objects
 *
 * Copyright  Alexandre Julliard, 1994
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_SYSCOLOR_H
#define __WINE_SYSCOLOR_H

BOOL   WIN32API OS2UpdateWindow(HWND hwnd);
HBRUSH WIN32API OS2GetSysColorBrush(int nIndex);

extern void SYSCOLOR_Init(void);
extern void SYSCOLOR_Save(void);

extern INT SYSCOLOR_GetLastColor(VOID);
extern INT SYSCOLOR_GetNumColors(VOID);
extern BOOL SYSCOLOR_GetUseWinColors(VOID);

#endif  /* __WINE_SYSCOLOR_H */
