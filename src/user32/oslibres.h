/* $Id: oslibres.h,v 1.5 2000-11-09 18:15:18 sandervl Exp $ */
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

HANDLE  OSLibWinSetAccelTable(HWND hwnd, HANDLE hAccel, PVOID acceltemplate);
HANDLE  OSLibWinCreateIcon(PVOID iconbitmap, ULONG cxDesired, ULONG cyDesired);
HANDLE  OSLibWinCreatePointer(PVOID cursorbitmap, ULONG cxDesired, ULONG cyDesired);
HANDLE  OSLibWinQuerySysPointer(ULONG type,INT w,INT h);
HANDLE  OSLibWinQuerySysIcon(ULONG type,INT w,INT h);

#endif //__OSLIBGDI_H__
