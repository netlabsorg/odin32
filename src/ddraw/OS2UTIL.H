/* $Id: OS2UTIL.H,v 1.6 2001-03-18 21:44:43 mike Exp $ */

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
void InverseDC(HDC hdc, LONG lHeight);
int InitIO();
#endif
