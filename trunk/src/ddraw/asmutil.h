/* $Id: asmutil.h,v 1.9 2002-07-03 15:44:38 sandervl Exp $ */

/*
 * asmutil.asm function definition
 *
 * Copyright 1998 Sander van Leeuwen
 *           1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __ASMUTIL_H__
#define __ASMUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void __cdecl BlitColorKey8(PBYTE dest, PBYTE src, ULONG key, ULONG linesize);
extern void __cdecl BlitColorKey16(PBYTE dest, PBYTE src, ULONG key, ULONG linesize);
extern void __cdecl BlitColorKey32(PBYTE dest, PBYTE src, ULONG key, ULONG linesize);
extern void __cdecl BlitColorKey8MMX(PBYTE dest, PBYTE src, ULONG key, ULONG linesize);
extern void __cdecl BlitColorKey16MMX(PBYTE dest, PBYTE src, ULONG key, ULONG linesize);
extern void __cdecl BltRec(PBYTE dest, PBYTE src, ULONG ulBltWidth, ULONG ulBltHeight,
                           ULONG ulDestPitch, ULONG ulSrcPitch);
extern void __cdecl MemFlip(PBYTE dest, PBYTE src, ULONG Size);
extern int __cdecl CPUHasMMX();

extern void CDECL ddmemfill16(PBYTE dest, ULONG value, ULONG size);
extern void CDECL ddmemfill32(PBYTE dest, ULONG value, ULONG size);

#ifdef __cplusplus
}
#endif

#endif
