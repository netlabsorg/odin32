/* $Id: syscolor.h,v 1.6 1999-10-20 16:27:01 cbratschi Exp $ */

/*
 * WINE:
 * System color objects
 *
 * Copyright  Alexandre Julliard, 1994
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_SYSCOLOR_H
#define __WINE_SYSCOLOR_H

//GetSysColor extra colors
#define COLOR_3DDKSHADOW        21
#define COLOR_3DLIGHT           22
#define COLOR_INFOTEXT          23
#define COLOR_INFOBK            24
/* win98 colors */
#define COLOR_ALTERNATEBTNFACE         25  /* undocumented, constant's name unknown */
#define COLOR_HOTLIGHT                 26
#define COLOR_GRADIENTACTIVECAPTION    27
#define COLOR_GRADIENTINACTIVECAPTION  28

#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT


BOOL   WIN32API OS2UpdateWindow(HWND hwnd);
HBRUSH WIN32API OS2GetSysColorBrush(int nIndex);

extern void SYSCOLOR_Init(void);
extern void SYSCOLOR_Save(void);

#endif  /* __WINE_SYSCOLOR_H */
