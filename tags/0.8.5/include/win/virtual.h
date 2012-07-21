/*
 * Some help functions
 *
 * Copyright 1999 Sander van Leeuwen
 */

#ifndef __VIRTUAL_H
#define __VIRTUAL_H

#ifdef __cplusplus
extern "C" {
#endif

HANDLE WINAPI VIRTUAL_MapFileA( LPCSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory = FALSE);
HANDLE WINAPI VIRTUAL_MapFileW( LPCWSTR name , LPVOID *lpMapping, BOOL fReadIntoMemory = FALSE);

LPVOID VirtualAllocShared(DWORD  cbSize, DWORD  fdwAllocationType,
                          DWORD  fdwProtect, LPSTR name);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* __VIRTUAL_H */
