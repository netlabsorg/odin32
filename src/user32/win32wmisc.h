/* $Id: win32wmisc.h,v 1.3 2003-11-12 14:10:21 sandervl Exp $ */
/*
 * Misc. functions for window management
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Parts based on Wine (windows\mdi.c; windows\win.c)
 *
 * Copyright 1994, Bob Amstadt
 *           1995,1996 Alex Korobka
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include "mdi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define get_win_sys_menu( hwnd ) getSysMenu(hwnd)

MDICLIENTINFO *get_client_info( HWND client );
HMENU WIN32API getSysMenu(HWND hwnd);
VOID setSysMenu(HWND hwnd,HMENU hMenu);
void GetWindowRectParent(HWND hwnd, RECT *pRect);
HWND *WIN_ListChildren( HWND hwnd );

void NC_GetSysPopupPos( HWND hwnd, RECT* rect );
BOOL NC_DrawSysButton95 (HWND hwnd, HDC hdc, BOOL down);
INT  NC_HandleNCHitTest( HWND hwnd, POINT pt);

#ifdef __cplusplus
}
#endif
