/*
 * USER definitions
 *
 * Copyright 1993 Alexandre Julliard
 */

#ifndef __WINE_USER_H
#define __WINE_USER_H

#include "ldt.h"
#include "local.h"

#define USER_HEAP_ALLOC(size) HeapAlloc(GetProcessHeap(),0,size)
#define USER_HEAP_FREE(handle) HeapFree(GetProcessHeap(),0,(LPVOID)handle)
#define USER_HEAP_REALLOC(handle,size) HeapRealloc(GetProcessHeap(),0,(LPVOID)handle, size)
#define USER_HEAP_LIN_ADDR(handle)  (PVOID)handle
#define USER_HEAP_SEG_ADDR(handle)  (PVOID)handle

#define USUD_LOCALALLOC        0x0001
#define USUD_LOCALFREE         0x0002
#define USUD_LOCALCOMPACT      0x0003
#define USUD_LOCALHEAP         0x0004
#define USUD_FIRSTCLASS        0x0005

typedef struct tagUSER_DRIVER {
  BOOL (*pInitialize)(void);
  void (*pFinalize)(void);
  void (*pBeginDebugging)(void);
  void (*pEndDebugging)(void);
} USER_DRIVER;

extern USER_DRIVER *USER_Driver;

void USER_ExitWindows(void);
WORD WINAPI UserSignalProc( UINT uCode, DWORD dwThreadOrProcessID,
                            DWORD dwFlags, HMODULE16 hModule );

#endif  /* __WINE_USER_H */
