/* $Id: exceptutil.h,v 1.11 2001-03-22 18:16:41 sandervl Exp $ */

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

ULONG  getEAX();
ULONG  getEBX();
ULONG  getESP();
USHORT getSS();
USHORT getDS();
USHORT getCS();
USHORT getSS();
USHORT getES();
USHORT getFS();
USHORT getGS();

void CDECL Mul32x32to64(PVOID result, DWORD op1, DWORD op2);

ULONG CDECL AsmCallThreadHandler(ULONG handler, LPVOID parameter);

#ifdef __cplusplus
}
#endif

#define ENTER_EXCEPTION 0
#define LEAVE_EXCEPTION 1
void LogException(int state);


#endif
