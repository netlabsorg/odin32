/* $Id: static.h,v 1.3 1999-07-23 17:55:27 sandervl Exp $ */

/*
 * Static-class extra info
 *
 * Copyright 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 */

#define STATICCLASSNAME "STATIC"

#ifndef __WINE_STATIC_H
#define __WINE_STATIC_H

#include "windef.h"

  /* Extra info for STATIC windows */
typedef struct
{
    HFONT    hFont;   /* Control font (or 0 for system font) */
    WORD     dummy;   /* Don't know what MS-Windows puts in there */
    HICON    hIcon;   /* Icon handle for SS_ICON controls */
} STATICINFO;

extern LRESULT WINAPI StaticWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

BOOL STATIC_Register();
BOOL STATIC_Unregister();

#endif  /* __WINE_STATIC_H */

