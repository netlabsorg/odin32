/* $Id: exceptutil.h,v 1.15 2003-03-06 12:49:08 sandervl Exp $ */

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

#ifdef DEBUG
void  PrintExceptionChain();
#else
#define PrintExceptionChain()
#endif

ULONG CDECL AsmCallThreadHandler(BOOL fAlignStack, ULONG handler, LPVOID parameter);

DWORD CDECL CallEntryPoint(DWORD entryPoint, DWORD parameter);

#ifdef __cplusplus
}
#endif

#define ENTER_EXCEPTION 0
#define LEAVE_EXCEPTION 1
int LogException(int state, int prevlock = 0);


#endif
