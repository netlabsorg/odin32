/* $Id: oslibmenu.h,v 1.1 1999-08-31 14:38:08 sandervl Exp $ */
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

HWND  OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate);
int   OSLibGetMenuItemCount(HWND hMenu);
 
#endif //__OSLIBGDI_H__
