/* $Id: asmutil.h,v 1.7 2002-07-01 19:15:26 sandervl Exp $ */

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

void _Optlink DDrawRGB555to565(char *dest, char *src, ULONG num);
void _Optlink DDrawRGB555to565MMX(char *dest, char *src, ULONG num);

void _Optlink DDrawRGB565to555(char *dest, char *src, ULONG num);
void _Optlink DDrawRGB565to555MMX(char *dest, char *src, ULONG num);

#ifdef __cplusplus
}
#endif

#endif
