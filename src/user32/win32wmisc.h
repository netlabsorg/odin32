/* $Id: win32wmisc.h,v 1.1 2001-09-19 15:39:51 sandervl Exp $ */
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

MDICLIENTINFO *get_client_info( HWND client );
HMENU WIN32API getSysMenu(HWND hwnd);
VOID setSysMenu(HWND hwnd,HMENU hMenu);
void GetWindowRectParent(HWND hwnd, RECT *pRect);
void NC_GetSysPopupPos( HWND hwnd, RECT* rect );
HWND *WIN_ListChildren( HWND hwnd );

#ifdef __cplusplus
}
#endif
