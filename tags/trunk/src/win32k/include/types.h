/* $Id: types.h,v 1.1 1999-09-06 02:20:00 bird Exp $
 *
 * Common typedeclarations.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _types_h_
#define _types_h_
    #define DWORD   ULONG
    #define LPVOID  VOID *
    #define WORD    USHORT
    #define UNALIGNED
    #define PDWORD  DWORD *
    #define PWORD   WORD *
    #define NTAPI
    #define BOOLEAN ULONG
    #define WCHAR   USHORT
    #define HANDLE  ULONG
    #define WIN32API __stdcall
#endif
