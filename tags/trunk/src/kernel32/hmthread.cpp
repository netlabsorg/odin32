/* $Id: hmthread.cpp,v 1.14 2002-07-15 14:28:51 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 thread handle class
 *
 *
 * TODO: Handle is not destroyed when thread terminates (or else GetExitCodeThread won't work)
 *       Create thread token during thread creation??
 *       Fix for WaitForSingleObject when thread is already terminated, but
 *       WaitForMultipleObjects can still fail!
 *       WaitForSingle/MultipleObjects needs to be rewritten! (not using
 *       Open32)
 *
 ************************************************************************************
 * NOTE: If we ever decide to allocate our own stack, then we MUST use VirtualAlloc!!!!
 *       (alignment reasons)
 ************************************************************************************
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <wprocess.h>

#include <HandleManager.H>
#include "HMThread.h"
#include "oslibdos.h"

#include <win\thread.h>
#include "thread.h"
#include "asmutil.h"

#define DBG_LOCALLOG	DBG_hmthread
#include "dbglocal.h"


//******************************************************************************
//******************************************************************************
HANDLE HMDeviceThreadClass::CreateThread(PHMHANDLEDATA          pHMHandleData,
                                         LPSECURITY_ATTRIBUTES  lpsa,
                                         DWORD                  cbStack,
                                         LPTHREAD_START_ROUTINE lpStartAddr,
                                         LPVOID                 lpvThreadParm,
                                         DWORD                  fdwCreate,
                                         LPDWORD                lpIDThread,
                                         BOOL                   fFirstThread)
{
    Win32Thread *winthread;
    DWORD        threadid;
    HANDLE       hThread = pHMHandleData->hHMHandle;

    if(lpIDThread == NULL) { 
        lpIDThread = &threadid;
    }
    pHMHandleData->dwInternalType = HMTYPE_THREAD;
    pHMHandleData->dwUserData     = THREAD_ALIVE;

    //SvL: This doesn't really create a thread, but only sets up the
    //     handle of thread 0
    if(fFirstThread) {
	    pHMHandleData->hHMHandle = O32_GetCurrentThread(); //return Open32 handle of thread
	    return pHMHandleData->hHMHandle;
    }
    winthread = new Win32Thread(lpStartAddr, lpvThreadParm, fdwCreate, hThread);

    if(winthread == 0) {
        DebugInt3();
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return(0);
    }
    // @@@PH Note: with debug code enabled, ODIN might request more stack space!
    //SvL: Also need more stack in release build (RealPlayer 7 sometimes runs
    //     out of stack
    if (cbStack > 0)
        cbStack <<= 1;     // double stack
    else
        cbStack = 1048576; // per default 1MB stack per thread

    //************************************************************************************
    //NOTE: If we ever decide to allocate our own stack, then we MUST use VirtualAlloc!!!!
    //      (alignment reasons)
    //************************************************************************************
    pHMHandleData->hHMHandle = O32_CreateThread(lpsa, cbStack, winthread->GetOS2Callback(),
                                                (LPVOID)winthread, fdwCreate, lpIDThread);

    *lpIDThread = MAKE_THREADID(O32_GetCurrentProcessId(), *lpIDThread);
    
    TEB *teb = GetTEBFromThreadHandle(hThread);
    if(teb) {
        //store thread id in TEB
        teb->o.odin.threadId = *lpIDThread;
    }
    dprintf(("CreateThread created %08x, id %x", pHMHandleData->hHMHandle, *lpIDThread));
  
    return pHMHandleData->hHMHandle;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::SuspendThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    dprintf(("SuspendThread %08xh)\n", pHMHandleData->hHMHandle));

    return O32_SuspendThread(pHMHandleData->hHMHandle);
}
//******************************************************************************
//******************************************************************************
INT HMDeviceThreadClass::GetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    TEB *teb;

    dprintf(("GetThreadPriority(%08xh)\n", pHMHandleData->hHMHandle));

    teb = GetTEBFromThreadHandle(hThread);
    if(teb == NULL) {
        dprintf(("!WARNING!: TEB not found!!"));
        SetLastError(ERROR_INVALID_HANDLE);
        return THREAD_PRIORITY_ERROR_RETURN;
    }
    return teb->delta_priority;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::SetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData, int priority)
{
    TEB  *teb;

    dprintf(("SetThreadPriority (%08xh,%08xh)", pHMHandleData->hHMHandle, priority));

    teb = GetTEBFromThreadHandle(hThread);
    if(teb == NULL) {
        dprintf(("!WARNING!: TEB not found!!"));
        SetLastError(ERROR_INVALID_HANDLE);
        return THREAD_PRIORITY_ERROR_RETURN;
    }
    DWORD ret = OSLibDosSetPriority(ODIN_TO_OS2_THREADID(teb->o.odin.threadId), priority);
    if(ret == ERROR_SUCCESS) {
        teb->delta_priority = priority;
        return TRUE;
    }
    else {
        dprintf(("DosSetPriority failed with rc %d", ret));
        return FALSE;
    }
}
//******************************************************************************
//TODO: Implement this??
//******************************************************************************
BOOL HMDeviceThreadClass::GetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, PCONTEXT lpContext)
{
  dprintf(("GetThreadContext NOT IMPLEMENTED!! (TRUE)\n"));
  memset(lpContext, 0, sizeof(CONTEXT));

  /* make up some plausible values for segment registers */
  lpContext->SegCs   = getCS();
  lpContext->SegDs   = getDS();
  lpContext->SegSs   = getSS();
  lpContext->SegEs   = getES();
  lpContext->SegGs   = getGS();
  lpContext->SegFs   = GetFS();

  return TRUE;
}
//******************************************************************************
//TODO: Implement this??
//******************************************************************************
BOOL HMDeviceThreadClass::SetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, const CONTEXT *lpContext)
{
  dprintf(("SetThreadContext NOT IMPLEMENTED!!\n"));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::TerminateThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, DWORD exitcode)
{
    dprintf(("TerminateThread (%08xh,%08xh)\n",
             pHMHandleData->hHMHandle,
             exitcode));

    pHMHandleData->dwUserData = THREAD_TERMINATED;
    return O32_TerminateThread(pHMHandleData->hHMHandle, exitcode);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::SetThreadTerminated(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    pHMHandleData->dwUserData = THREAD_TERMINATED;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::ResumeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    dprintf(("ResumeThread (%08xh)\n",
             pHMHandleData->hHMHandle));

    return O32_ResumeThread(pHMHandleData->hHMHandle);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::GetExitCodeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, LPDWORD lpExitCode)
{
    dprintf(("GetExitCodeThread (%08xh,%08xh)\n",
             pHMHandleData->hHMHandle,
             lpExitCode));

#if 0
    if(pHMHandleData->dwUserData == THREAD_ALIVE) {
	lpExitCode == STILL_ALIVE;
	return TRUE;
    }
#endif
    return O32_GetExitCodeThread(pHMHandleData->hHMHandle, lpExitCode);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                               DWORD  dwTimeout)
{
  dprintf(("HMThread::WaitForSingleObject (%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwTimeout));
  
  //This doesn't work very well in Open32 (object's state never signaled)
  if(pHMHandleData->dwUserData == THREAD_TERMINATED) {
	return WAIT_OBJECT_0;
  }
  return HMDeviceOpen32Class::WaitForSingleObject(pHMHandleData, dwTimeout);
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::WaitForSingleObjectEx(PHMHANDLEDATA pHMHandleData,
                                                 DWORD  dwTimeout,
                                                 BOOL   fAlertable)
{
  if(pHMHandleData->dwUserData == THREAD_TERMINATED) {
	return WAIT_OBJECT_0;
  }
  return HMDeviceOpen32Class::WaitForSingleObjectEx(pHMHandleData, dwTimeout, fAlertable);
}
//******************************************************************************
//******************************************************************************
