/* $Id: oslibdebug.h,v 1.2 2002-05-28 09:53:34 sandervl Exp $ */

/*
 * OS/2 debug apis
 *
 * Copyright 1999 Edgar Buerkle
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBDEBUG_H__
#define __OSLIBDEBUG_H__

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

//Debug apis
VOID OSLibStartDebugger(ULONG *pid);
BOOL OSLibContinueDebugEvent(DWORD dwProcessId, DWORD dwThreadId, DWORD dwContinueStatus);
BOOL OSLibWaitForDebugEvent(LPDEBUG_EVENT lpde, DWORD dwTimeout);
VOID OSLibDebugReadMemory(LPCVOID lpBaseAddress,LPVOID lpBuffer, DWORD cbRead, LPDWORD lpNumberOfBytesRead);
BOOL OSLibAddWin32Event(LPDEBUG_EVENT lpde);
#endif //__OSLIBDEBUG_H__