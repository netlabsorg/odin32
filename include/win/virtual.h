/*
 * Some help functions
 *
 * Copyright 1999 Sander van Leeuwen
 */

#ifndef __VIRTUAL_H
#define __VIRTUAL_H

HANDLE WINAPI VIRTUAL_MapFileA( LPCSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory = FALSE);
HANDLE WINAPI VIRTUAL_MapFileW( LPCWSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory = FALSE);

LPVOID VirtualAllocShared(DWORD  cbSize, DWORD  fdwAllocationType,
                          DWORD  fdwProtect, LPSTR name);

#endif  /* __VIRTUAL_H */
