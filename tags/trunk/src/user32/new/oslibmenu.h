/* $Id: oslibmenu.h,v 1.3 2000-01-01 14:54:52 cbratschi Exp $ */
/*
 * Window Menu wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBMENU_H__
#define __OSLIBMENU_H__

#ifdef OS2_INCLUDED
#include "win32type.h"
#endif

HWND  OSLibWinSetMenu(HWND hwndParent, HMENU hMenu);
HMENU OSLibWinCreateMenu(PVOID menutemplate);
HMENU OSLibWinCreateEmptyMenu();
HMENU OSLibWinCreateEmptyPopupMenu();
int   OSLibGetMenuItemCount(HWND hMenu);
BOOL  OSLibGetMenuItemRect(HWND hMenu, int index, LPRECT pRect);
 
#endif //__OSLIBGDI_H__
