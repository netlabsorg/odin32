/* $Id: exceptutil.h,v 1.3 1999-10-22 18:07:19 sandervl Exp $ */

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

void  DisableFPUExceptions();
ULONG GetDllEntryPoint();

void  OS2SetExceptionHandler(void *exceptframe);
void  OS2UnsetExceptionHandler(void *exceptframe);
void  SetOS2ExceptionChain(ULONG val);

ULONG  getEAX();
ULONG  getEBX();
USHORT getSS();

#ifdef __cplusplus
}
#endif

#endif
