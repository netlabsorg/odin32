/* $Id: except.h,v 1.6 1999-06-20 12:46:08 sandervl Exp $ */

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

ULONG  getEAX();
ULONG  getEBX();
USHORT getSS();

#ifdef __cplusplus
}
#endif

#endif
