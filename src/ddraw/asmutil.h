/* $Id: asmutil.h,v 1.1 1999-05-24 20:19:37 ktk Exp $ */

/*
 * asmutil.asm function definition
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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

#ifdef __cplusplus
}
#endif

#endif