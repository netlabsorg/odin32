/* $Id: asmutil.h,v 1.2 2003-03-27 14:13:10 sandervl Exp $ */

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

ULONG  CDECL getEAX();
ULONG  CDECL getEBX();
ULONG  CDECL getESP();
USHORT CDECL getSS();
USHORT CDECL getDS();
USHORT CDECL getCS();
USHORT CDECL getSS();
USHORT CDECL getES();
USHORT CDECL getFS();
USHORT CDECL getGS();

void CDECL Mul32x32to64(PVOID result, DWORD op1, DWORD op2);
void CDECL Sub64(LARGE_INTEGER *a, LARGE_INTEGER *b, LARGE_INTEGER *result);
void CDECL Add64(LARGE_INTEGER *a, LARGE_INTEGER *b, LARGE_INTEGER *result);


void CDECL set_bit(int bitnr, void *addr);
void CDECL clear_bit(int bitnr, void *addr);
int  CDECL test_bit(int bitnr, void *addr);

#ifdef __cplusplus
}
#endif

#endif //__ASMUTIL_H__
