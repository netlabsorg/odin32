/* $Id: oslibres.h,v 1.1 1999-07-20 15:46:53 sandervl Exp $ */
/*
 * Window GUI resource wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBRES_H__
#define __OSLIBRES_H__

#ifdef OS2_INCLUDED
#include "win32type.h"
#endif
 
HWND    OSLibWinCreateMenu(HWND hwndParent, PVOID menutemplate);
HANDLE  OSLibWinSetAccelTable(HWND hwnd, HANDLE hAccel, PVOID acceltemplate);
HANDLE  OSLibWinSetIcon(HWND hwnd, HANDLE hIcon, PVOID iconbitmap);

#endif //__OSLIBGDI_H__
