/* $Id: oslibres.h,v 1.2 1999-08-20 20:09:51 sandervl Exp $ */
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
HANDLE  OSLibWinCreateIcon(PVOID iconbitmap);
HANDLE  OSLibWinCreatePointer(PVOID cursorbitmap);
HANDLE  OSLibWinQuerySysPointer(ULONG type);
HANDLE  OSLibWinQuerySysIcon(ULONG type);

#endif //__OSLIBGDI_H__
