/* $Id: oslibthread.h,v 1.1 2003-03-27 14:00:54 sandervl Exp $ */

/*
 * OS/2 thread apis
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBTHREAD_H__
#define __OSLIBTHREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOL  OSLibQueryThreadContext(ULONG ulThreadId, ULONG teb_sel, CONTEXT *lpContext);
void  OSLibDosExitThread(ULONG retcode);
DWORD OSLibDosSetPriority(ULONG tid, int priority);
void  OSLibDosSetMaxPriority(ULONG tid);

void  OSLibDisableThreadSwitching();
void  OSLibEnableThreadSwitching();

#ifdef __cplusplus
}
#endif

#endif //__OSLIBTHREAD_H__