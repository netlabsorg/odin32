/* $Id: oslibdebug.h,v 1.1 1999-12-31 10:47:11 sandervl Exp $ */

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

#endif //__OSLIBDEBUG_H__