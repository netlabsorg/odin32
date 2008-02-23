/* $Id: types.h,v 1.2 1999-11-10 01:45:34 bird Exp $
 *
 * Common typedeclarations.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
