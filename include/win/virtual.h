/* $Id: virtual.h,v 1.2 1999-08-25 12:29:26 sandervl Exp $ */

/*
 * Some help functions
 *
 * Copyright 1999 Sander van Leeuwen
 */

#ifndef __VIRTUAL_H
#define __VIRTUAL_H

HANDLE WINAPI VIRTUAL_MapFileA( LPCSTR name , LPVOID *lpMapping);
HANDLE WINAPI VIRTUAL_MapFileW( LPCWSTR name , LPVOID *lpMapping);

#ifndef _OS2WIN_H
void WINAPI CloseHandle(HANDLE hHandle);
#endif

#endif  /* __VIRTUAL_H */
