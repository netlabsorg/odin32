/* $Id: asmutil.h,v 1.1 2002-05-10 14:55:10 sandervl Exp $ */

/*
 * Misc assembly functions for OS/2
 * Copyright 1998-2000 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __ASMUTIL_H__
#define __ASMUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

ULONG  getEAX();
ULONG  getEBX();
ULONG  getESP();
USHORT getSS();
USHORT getDS();
USHORT getCS();
USHORT getSS();
USHORT getES();
USHORT getFS();
USHORT getGS();

void CDECL Mul32x32to64(PVOID result, DWORD op1, DWORD op2);
void CDECL Sub64(LARGE_INTEGER *a, LARGE_INTEGER *b, LARGE_INTEGER *result);
void CDECL Add64(LARGE_INTEGER *a, LARGE_INTEGER *b, LARGE_INTEGER *result);

#ifdef __cplusplus
}
#endif

#endif //__ASMUTIL_H__
