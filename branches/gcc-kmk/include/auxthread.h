/*
 * Auxiliary thread API
 *
 * Used to execute code on a dedicated thread.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __AUXTHREAD_H__
#define __AUXTHREAD_H__

#ifndef VOID
#define VOID void
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef PVOID (* PAUXTHREADFN)(PVOID arg1, PVOID arg2, PVOID arg3, PVOID arg4);

BOOL WIN32API RunOnAuxThread(PAUXTHREADFN pfn, PVOID arg1, PVOID arg2,
                             PVOID arg3, PVOID arg4);

BOOL WIN32API RunOnAuxThreadAndWait(PAUXTHREADFN pfn, PVOID arg1, PVOID arg2,
                                    PVOID arg3, PVOID arg4, PVOID *ret);

VOID StopAuxThread();

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__AUXTHREAD_H__
