/*
 * Exception + misc functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
