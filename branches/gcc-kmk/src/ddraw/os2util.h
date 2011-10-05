/* $Id: OS2UTIL.H,v 1.7 2002-12-04 15:21:40 sandervl Exp $ */

/*
 * OS/2 Utility functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2UTIL_H__
#define __OS2UTIL_H__

char *OS2AllocMem(ULONG size);
void  OS2FreeMem(char *lpMem);
void  OS2MaximizeWindow(HWND hwndClient);
BOOL  OSLibPostMessage(HWND hwnd, ULONG msg, ULONG mp1, ULONG mp2);
void  InverseDC(HDC hdc, LONG lHeight);
int   InitIO();

#endif
