/* $Id: hmthread.cpp,v 1.18 2003-02-04 11:29:00 sandervl Exp $ */

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
#include "winexebase.h"

#define DBG_LOCALLOG	DBG_hmthread
#include "dbglocal.h"


typedef struct {
  HANDLE hDupThread;	//original thread handle if duplicated
  DWORD  dwState;       //THREAD_ALIVE, THREAD_TERMINATED
} OBJ_THREAD;

#define GET_THREADHANDLE(hThread) (threadobj && threadobj->hDupThread) ? threadobj->hDupThread : hThread

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
    OBJ_THREAD *threadobj = (OBJ_THREAD *)malloc(sizeof(OBJ_THREAD));
    if(threadobj == 0) {
        DebugInt3();
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return(0);
    }
    threadobj->hDupThread = 0;	//not a duplicate
    threadobj->dwState = THREAD_ALIVE;
    pHMHandleData->dwUserData = (DWORD)threadobj;

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
    if (cbStack > 0) {
        cbStack <<= 1;     // double stack
    }
    else {
        cbStack = (WinExe) ? WinExe->getDefaultStackSize() : 0x100000; // per default 1MB stack per thread
    }
    dprintf(("Thread stack size 0x%x", cbStack));

    //************************************************************************************
    //NOTE: If we ever decide to allocate our own stack, then we MUST use VirtualAlloc!!!!
    //      (alignment reasons)
    //************************************************************************************
    pHMHandleData->hHMHandle = O32_CreateThread(lpsa, cbStack, winthread->GetOS2Callback(),
                                                (LPVOID)winthread, fdwCreate, lpIDThread);

    if(pHMHandleData->hHMHandle == 0) {
        dprintf(("Thread creation failed!!"));
        DebugInt3();
    }

    *lpIDThread = MAKE_THREADID(O32_GetCurrentProcessId(), *lpIDThread);
    
    TEB *teb = GetTEBFromThreadHandle(hThread);
    if(teb) {
        //store thread id in TEB
        teb->o.odin.threadId  = *lpIDThread;
        teb->o.odin.dwSuspend = (fdwCreate & CREATE_SUSPENDED) ? 1 : 0;
    }
    else DebugInt3();

    dprintf(("CreateThread created %08x, id %x", pHMHandleData->hHMHandle, *lpIDThread));
  
    return pHMHandleData->hHMHandle;
}
/*****************************************************************************
 * Name      : HMDeviceFileClass::DuplicateHandle
 * Purpose   :
 * Parameters:
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : DUPLICATE_CLOSE_SOURCE flag handled in HMDuplicateHandle
 *
 * Status    : partially implemented
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceThreadClass::DuplicateHandle(HANDLE srchandle, PHMHANDLEDATA pHMHandleData,
                                          HANDLE  srcprocess,
                                          PHMHANDLEDATA pHMSrcHandle,
                                          HANDLE  destprocess,
                                          PHANDLE desthandle,
                                          DWORD   fdwAccess,
                                          BOOL    fInherit,
                                          DWORD   fdwOptions,
                                          DWORD   fdwOdinOptions)
{
  BOOL ret;
  OBJ_THREAD *threadsrc = (OBJ_THREAD *)pHMSrcHandle->dwUserData;

  dprintf(("KERNEL32:HMDeviceThreadClass::DuplicateHandle (%08x,%08x,%08x,%08x,%08x)",
           pHMHandleData, srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle));

  if(destprocess != srcprocess)
  {
      dprintf(("ERROR: DuplicateHandle; different processes not supported!!"));
      SetLastError(ERROR_INVALID_HANDLE); //??
      return FALSE;
  }
  ret = O32_DuplicateHandle(srcprocess, pHMSrcHandle->hHMHandle, destprocess, desthandle, fdwAccess, fInherit, fdwOptions);

  if(ret == TRUE) {
       OBJ_THREAD *threaddest = (OBJ_THREAD *)malloc(sizeof(OBJ_THREAD));
       if(threaddest == NULL) {
           O32_CloseHandle(*desthandle);
           SetLastError(ERROR_NOT_ENOUGH_MEMORY);
           return FALSE;
       }
       threaddest->hDupThread = 0;
       threaddest->dwState    = THREAD_ALIVE;
       pHMHandleData->dwUserData = (DWORD)threaddest;

       if(threadsrc) {
           threaddest->hDupThread = (threadsrc->hDupThread) ? threadsrc->hDupThread : srchandle;
           threaddest->dwState    = threadsrc->dwState;
       }

       pHMHandleData->hHMHandle = *desthandle;
       return TRUE;
  }
  else return FALSE;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::SuspendThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    DWORD dwSuspend;
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
    if(teb) {
        teb->o.odin.dwSuspend++;
        dprintf(("SuspendThread %08xh): count %d", pHMHandleData->hHMHandle, teb->o.odin.dwSuspend));
    }
    dwSuspend = O32_SuspendThread(pHMHandleData->hHMHandle);
    if(dwSuspend == -1) {
        teb->o.odin.dwSuspend--;
        dprintf(("!ERROR!: SuspendThread FAILED"));
    }
    return dwSuspend;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::ResumeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    DWORD dwSuspend;
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
    if(teb) {
        teb->o.odin.dwSuspend--;
        dprintf(("ResumeThread (%08xh) : count %d", pHMHandleData->hHMHandle, teb->o.odin.dwSuspend));
    }
    dwSuspend = O32_ResumeThread(pHMHandleData->hHMHandle);
    if(dwSuspend == -1) {
        teb->o.odin.dwSuspend++;
        dprintf(("!ERROR!: ResumeThread FAILED"));
    }
    return dwSuspend;
}
//******************************************************************************
//******************************************************************************
INT HMDeviceThreadClass::GetThreadPriority(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    dprintf(("GetThreadPriority(%08xh)\n", pHMHandleData->hHMHandle));

    TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
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
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    dprintf(("SetThreadPriority (%08xh,%08xh)", pHMHandleData->hHMHandle, priority));

    TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
    if(teb == NULL) {
        dprintf(("!WARNING!: TEB not found!!"));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
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
/*****************************************************************************
 * Name      : BOOL GetThreadTimes
 * Purpose   : The GetThreadTimes function obtains timing information about a specified thread.
 * Parameters: HANDLE     hThread       specifies the thread of interest
 *             LPFILETIME lpCreationTime when the thread was created
 *             LPFILETIME lpExitTime     when the thread exited
 *             LPFILETIME lpKernelTime   time the thread has spent in kernel mode
 *             LPFILETIME lpUserTime     time the thread has spent in user mode
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL HMDeviceThreadClass::GetThreadTimes(HANDLE        hThread,
                                         PHMHANDLEDATA pHMHandleData,
                                         LPFILETIME lpCreationTime,
                                         LPFILETIME lpExitTime,
                                         LPFILETIME lpKernelTime,
                                         LPFILETIME lpUserTime)
{
  dprintf(("Kernel32: GetThreadTimes(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hThread,
           lpCreationTime,
           lpExitTime,
           lpKernelTime,
           lpUserTime));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::TerminateThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, DWORD exitcode)
{
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    dprintf(("TerminateThread (%08xh,%08xh)\n",
             pHMHandleData->hHMHandle,
             exitcode));

    if(threadobj) {
        threadobj->dwState = THREAD_TERMINATED;
    }
    else DebugInt3();
    return O32_TerminateThread(pHMHandleData->hHMHandle, exitcode);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::SetThreadTerminated(HANDLE hThread, PHMHANDLEDATA pHMHandleData)
{
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    if(threadobj) {
        threadobj->dwState = THREAD_TERMINATED;
    }
    else DebugInt3();

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::GetExitCodeThread(HANDLE hThread, PHMHANDLEDATA pHMHandleData, LPDWORD lpExitCode)
{
    dprintf(("GetExitCodeThread (%08xh,%08xh)\n",
             pHMHandleData->hHMHandle,
             lpExitCode));

#if 0
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    if(threadobj && threadobj->dwState == THREAD_ALIVE) {
	lpExitCode == STILL_ALIVE;
	return TRUE;
    }
    else DebugInt3();
#endif
    return O32_GetExitCodeThread(pHMHandleData->hHMHandle, lpExitCode);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    dprintf(("HMThread::CloseHandle %08x", pHMHandleData->hHMHandle));

    if(threadobj) {
        pHMHandleData->dwUserData = 0;
        free(threadobj);
    }
    return O32_CloseHandle(pHMHandleData->hHMHandle);
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceThreadClass::WaitForSingleObject(PHMHANDLEDATA pHMHandleData,
                                               DWORD  dwTimeout)
{
  OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

  dprintf(("HMThread::WaitForSingleObject (%08xh,%08xh)\n",
           pHMHandleData->hHMHandle,
           dwTimeout));
  
  //This doesn't work very well in Open32 (object's state never signaled)
  if(threadobj && threadobj->dwState == THREAD_TERMINATED) {
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
  OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

  //This doesn't work very well in Open32 (object's state never signaled)
  if(threadobj && threadobj->dwState == THREAD_TERMINATED) {
	return WAIT_OBJECT_0;
  }
  return HMDeviceOpen32Class::WaitForSingleObjectEx(pHMHandleData, dwTimeout, fAlertable);
}
//******************************************************************************
//******************************************************************************
