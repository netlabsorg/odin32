/* $Id: exceptutil.h,v 1.1 1999-08-22 12:55:57 sandervl Exp $ */

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

void  DisableFPUExceptions();
ULONG GetDllEntryPoint();

void  OS2SetExceptionHandler(void *exceptframe);
void  OS2UnsetExceptionHandler(void *exceptframe);

ULONG  getEAX();
ULONG  getEBX();
USHORT getSS();

#ifdef __cplusplus
}
#endif

#endif
