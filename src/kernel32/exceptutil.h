/* $Id: exceptutil.h,v 1.6 2000-02-16 14:25:40 sandervl Exp $ */

/*
 * Exception + misc functions for OS/2
 * Copyright 1998 Sander van Leeuwen
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#ifdef __cplusplus
extern "C" {
#endif

//SvL: Replace original startup code exception handler
#ifndef WIN32_TIBSEL
void  ReplaceExceptionHandler(void);
void  ChangeTIBStack();
#endif
void  __cdecl SetExceptionChain(DWORD);

extern "C" PVOID QueryExceptionChain();

void  OS2SetExceptionHandler(void *exceptframe);
void  OS2UnsetExceptionHandler(void *exceptframe);
void  SetOS2ExceptionChain(ULONG val);

ULONG  getEAX();
ULONG  getEBX();
USHORT getSS();
USHORT getDS();
USHORT getCS();
USHORT getSS();
USHORT getES();
USHORT getFS();
USHORT getGS();

#ifdef __cplusplus
}
#endif

#endif
