/* $Id: except.h,v 1.1 1999-05-24 20:19:46 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Exception + misc functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#ifdef __cplusplus
extern "C" {
#endif

//SvL: Replace original startup code exception handler
void  ReplaceExceptionHandler(void);
void  DisableFPUExceptions();
void __cdecl SetExceptionChain(DWORD);
void  ChangeTIBStack();
ULONG GetDllEntryPoint();

ULONG getEAX();
ULONG getEBX();

#ifdef __cplusplus
}
#endif

#endif
