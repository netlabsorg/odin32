/* $Id: winpos.h,v 1.2 1999-10-17 15:26:12 sandervl Exp $ */

/*
 * *DeferWindowPos() structure and definitions
 *
 * Copyright 1994 Alexandre Julliard
 */

#ifndef __WINE_WINPOS_H
#define __WINE_WINPOS_H

#ifndef __WIN32OS2__
#include "win.h"
#include "wine/winuser16.h" /* for WINDOWPOS16 */
#endif //__WIN32OS2__

#define DWP_MAGIC  ((INT)('W' | ('P' << 8) | ('O' << 16) | ('S' << 24)))

/* undocumented SWP flags - from SDK 3.1 */
#define SWP_NOCLIENTSIZE	0x0800
#define SWP_NOCLIENTMOVE	0x1000

typedef struct
{
    INT       actualCount;
    INT       suggestedCount;
    BOOL      valid;
    INT       wMagic;
    HWND      hwndParent;
    WINDOWPOS winPos[1];
} DWP;

#endif  /* __WINE_WINPOS_H */
