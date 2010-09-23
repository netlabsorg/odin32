/* $Id: hmthread.cpp,v 1.19 2003-03-27 15:27:37 sandervl Exp $ */

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
#include "oslibthread.h"
#include "oslibmem.h"

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
                                         BOOL                   fRegisterThread)
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
    //     handle of the current thread.
    if(fRegisterThread) {
        pHMHandleData->hHMHandle = O32_GetCurrentThread(); //return Open32 handle of thread
        return pHMHandleData->hHMHandle;
    }
    winthread = new Win32Thread(lpStartAddr, lpvThreadParm, fdwCreate, hThread);

    if(winthread == 0) {
        dprintf(("Win32Thread creation failed, no more memory"));
        DebugInt3();
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return(0);
    }
    if(winthread->_GetTEB() == 0) {
        dprintf(("Win32Thread->teb creation failed, no more memory"));
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
                                          DWORD   fdwAccess,
                                          BOOL    fInherit,
                                          DWORD   fdwOptions,
                                          DWORD   fdwOdinOptions)
{
  BOOL ret;
  OBJ_THREAD *threadsrc = (OBJ_THREAD *)pHMSrcHandle->dwUserData;

  dprintf(("KERNEL32:HMDeviceThreadClass::DuplicateHandle (%08x,%08x,%08x,%08x)",
           pHMHandleData, srcprocess, pHMSrcHandle->hHMHandle, destprocess));

  if(destprocess != srcprocess)
  {
      dprintf(("ERROR: DuplicateHandle; different processes not supported!!"));
      SetLastError(ERROR_INVALID_HANDLE); //??
      return FALSE;
  }
  pHMHandleData->hHMHandle = 0;
  ret = O32_DuplicateHandle(srcprocess, pHMSrcHandle->hHMHandle, destprocess, &pHMHandleData->hHMHandle, fdwAccess, fInherit, fdwOptions);

  if(ret == TRUE) {
       OBJ_THREAD *threaddest = (OBJ_THREAD *)malloc(sizeof(OBJ_THREAD));
       if(threaddest == NULL) {
           O32_CloseHandle(pHMHandleData->hHMHandle);
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

       return TRUE;
  }
  else
  {
      dprintf(("O32_DuplicateHandle failed for handle %x!!", pHMSrcHandle->hHMHandle));
      return FALSE;
  }
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
    CONTEXT     context;
    OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;

    TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
    if(teb == NULL) {
        dprintf(("ERROR: invalid thread handle"));
        SetLastError(ERROR_INVALID_THREAD_ID); //??
        return -1;
    }

    context.Eip = 0;
    if(teb->o.odin.context.ContextFlags)
    {
        context.ContextFlags = CONTEXT_CONTROL;
        if(GetThreadContext(hThread, pHMHandleData, &context) == FALSE)
        {
             DebugInt3();
             context.Eip = 0;
        }
        if(teb->o.odin.dwSuspend == 1 && teb->o.odin.context.ContextFlags && context.Eip)
        {//SetThreadContext was called for this thread and it's about to be restored

            //Since there's no equivalent of SetThreadContext in OS/2, we put an
            //illegal instruction at the instruction pointer of this thread to
            //make sure an exception is triggered. Inside the exception handler
            //of the thread we can change the registers.
            //(XCPT_PRIVILEGED_INSTRUCTION exception handler in exceptions.cpp)
            //(see detailed description in the HMDeviceThreadClass::SetThreadContext method)
            USHORT *lpEIP = (USHORT *)context.Eip;

            if(*lpEIP != SETTHREADCONTEXT_INVALID_LOCKOPCODE)
            {
                int size;

                teb->o.odin.dwAliasOffset = (DWORD)lpEIP & 0xFFF;
                if(teb->o.odin.dwAliasOffset + 2 >= PAGE_SIZE) {
                     size = 8192;
                }
                else size = teb->o.odin.dwAliasOffset + 2;

                lpEIP  = (USHORT *)((DWORD)lpEIP & ~0xFFF);

                if(OSLibDosAliasMem(lpEIP, size, &teb->o.odin.lpAlias, PAG_READ|PAG_WRITE) == 0)
                {
                    teb->o.odin.savedopcode = *(USHORT*)((char *)teb->o.odin.lpAlias+teb->o.odin.dwAliasOffset);

                    //sti -> undefined opcode exception
                    *(USHORT *)((char *)teb->o.odin.lpAlias+teb->o.odin.dwAliasOffset) = SETTHREADCONTEXT_INVALID_LOCKOPCODE;
                }
                else DebugInt3();

                //temporarily boost priority to ensure this thread is scheduled first
                //we reduce the priority in the exception handler
                OSLibDosSetMaxPriority(ODIN_TO_OS2_THREADID(teb->o.odin.threadId));
            }
            else {
                dprintf(("already patched!?!"));
                DebugInt3();
            }
        }
    }

        teb->o.odin.dwSuspend--;
        dprintf(("ResumeThread (%08xh) : count %d", pHMHandleData->hHMHandle, teb->o.odin.dwSuspend));

    dwSuspend = O32_ResumeThread(pHMHandleData->hHMHandle);
    if(dwSuspend == -1)
    {
        teb->o.odin.dwSuspend++;
        dprintf(("!ERROR!: ResumeThread FAILED"));

        if(teb->o.odin.dwSuspend == 1 && teb->o.odin.context.ContextFlags && context.Eip)
        {//Undo previous patching
            char *lpEIP = (char *)context.Eip;

            if(*lpEIP == SETTHREADCONTEXT_INVALID_LOCKOPCODE)
            {
                dprintf(("Undo SetThreadContext patching!!"));

                USHORT *lpAlias = (USHORT*)((char *)teb->o.odin.lpAlias + teb->o.odin.dwAliasOffset);
                //put back old byte
                *lpAlias = teb->o.odin.savedopcode;

                //restore the original priority (we boosted it to ensure this thread was scheduled first)
                ::SetThreadPriority(teb->o.odin.hThread, ::GetThreadPriority(teb->o.odin.hThread));

                OSLibDosFreeMem(teb->o.odin.lpAlias);
            }
            else {
                dprintf(("not patched!?!"));
                DebugInt3();
            }
            teb->o.odin.dwAliasOffset = 0;
            teb->o.odin.lpAlias = NULL;
            teb->o.odin.context.ContextFlags = 0;
    }
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
//******************************************************************************
#ifdef DEBUG
void DumpContext(CONTEXT *lpContext)
{
    dprintf(("************************ THREAD CONTEXT ************************"));
    if(lpContext->ContextFlags & CONTEXT_CONTROL) {
        dprintf(("CS:EIP %04x:%08x FLAGS %08x", lpContext->SegCs, lpContext->Eip, lpContext->EFlags));
        dprintf(("SS:ESP %04x:%08x EBP   %08x", lpContext->SegSs, lpContext->Esp, lpContext->Ebp));
    }
    if(lpContext->ContextFlags & CONTEXT_INTEGER) {
        dprintf(("EAX %08x EBX %08x ECX %08x EDX %08x", lpContext->Eax, lpContext->Ebx, lpContext->Ecx, lpContext->Edx));
        dprintf(("ESI %08x EDI %08x", lpContext->Esi, lpContext->Edi));
    }
    if(lpContext->ContextFlags & CONTEXT_SEGMENTS) {
        dprintf(("DS %04x ES %04x FS %04x GS %04x", (ULONG)lpContext->SegDs, (ULONG)lpContext->SegEs, (ULONG)lpContext->SegFs, (ULONG)lpContext->SegGs));
    }
    dprintf(("************************ THREAD CONTEXT ************************"));
//    if(lpContext->ContextFlags & CONTEXT_FLOATING_POINT) {
//        //TODO: First 7 dwords the same?
//        memcpy(&lpContext->FloatSave, ctxrec.ctx_env, sizeof(ctxrec.ctx_env));
//        memcpy(&lpContext->FloatSave.RegisterArea, ctxrec.ctx_stack, sizeof(ctxrec.ctx_stack));
//    }
}
#else
#define DumpContext(a)
#endif
//******************************************************************************
//******************************************************************************
BOOL HMDeviceThreadClass::GetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, PCONTEXT lpContext)
{
  OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;
  BOOL        ret;

  TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
  if(teb) {
     if(teb->o.odin.dwSuspend == 0) {
         dprintf(("ERROR: thread not suspended!!"));
         DebugInt3();
         SetLastError(ERROR_INVALID_OPERATION); //???
         return FALSE;
     }

     ret = OSLibQueryThreadContext(ODIN_TO_OS2_THREADID(teb->o.odin.threadId), teb->teb_sel, lpContext);
     if(ret == TRUE) {
         DumpContext(lpContext);
     }
     return ret;
  }
  dprintf(("!WARNING!: TEB not found!!"));
  SetLastError(ERROR_INVALID_HANDLE);
  return FALSE;
}
//******************************************************************************
// HMDeviceThreadClass::SetThreadContext
//
// Change the context (registers) of a suspended thread
//
// Parameters:
//
//   HANDLE hThread               - thread handle
//   PHMHANDLEDATA pHMHandleData  - handle data
//   const CONTEXT *lpContext     - context record (IN)
//
// Returns:
//   TRUE                       - success
//   FALSE                      - failure
//
// Remarks:
//
// Since OS/2 doesn't provide an equivalent for this function, we need to change
// the thread context manually. (DosDebug isn't really an option)
//
// We save the new context in the TEB structure. When this thread is
// activated by ResumeThread, we'll change the instruction addressed by the
// thread's EIP to an invalid instruction (sti).
// When the thread is activated, it will generate an exception. Inside the
// exception handler we change the registers, restore the original memory
// and continue.
// To make sure the thread is executed first and noone else will execute the
// invalid instruction, we temporarily boost the thread's priority to the max.
// (time critical, delta +31; max priority of win32 threads is time critical,
//  delta 0)
// The priority is restored in the exception handler.
//
//******************************************************************************
BOOL HMDeviceThreadClass::SetThreadContext(HANDLE hThread, PHMHANDLEDATA pHMHandleData, const CONTEXT *lpContext)
{
  OBJ_THREAD *threadobj = (OBJ_THREAD *)pHMHandleData->dwUserData;
  BOOL        ret;

  DumpContext((CONTEXT *)lpContext);

  TEB *teb = GetTEBFromThreadHandle(GET_THREADHANDLE(hThread));
  if(teb)
  {
      if(teb->o.odin.dwSuspend == 0) {
          dprintf(("ERROR: thread not suspended!!"));
          SetLastError(ERROR_INVALID_OPERATION); //???
          return FALSE;
      }
      if(lpContext->ContextFlags & CONTEXT_CONTROL) {
          teb->o.odin.context.ContextFlags |= CONTEXT_CONTROL;
          teb->o.odin.context.Ebp     = lpContext->Ebp;
          teb->o.odin.context.Eip     = lpContext->Eip;
          teb->o.odin.context.SegCs   = lpContext->SegCs;
          teb->o.odin.context.EFlags  = lpContext->EFlags;
          teb->o.odin.context.Esp     = lpContext->Esp;
          teb->o.odin.context.SegSs   = lpContext->SegSs;
      }
      if(lpContext->ContextFlags & CONTEXT_INTEGER) {
          teb->o.odin.context.ContextFlags |= CONTEXT_INTEGER;
          teb->o.odin.context.Edi     = lpContext->Edi;
          teb->o.odin.context.Esi     = lpContext->Esi;
          teb->o.odin.context.Ebx     = lpContext->Ebx;
          teb->o.odin.context.Edx     = lpContext->Edx;
          teb->o.odin.context.Ecx     = lpContext->Ecx;
          teb->o.odin.context.Eax     = lpContext->Eax;
      }
      if(lpContext->ContextFlags & CONTEXT_SEGMENTS) {
          teb->o.odin.context.ContextFlags |= CONTEXT_SEGMENTS;
          teb->o.odin.context.SegGs   = lpContext->SegGs;
          teb->o.odin.context.SegFs   = lpContext->SegFs;
          teb->o.odin.context.SegEs   = lpContext->SegEs;
          teb->o.odin.context.SegDs   = lpContext->SegDs;
      }
      if(lpContext->ContextFlags & CONTEXT_FLOATING_POINT) {
          teb->o.odin.context.ContextFlags |= CONTEXT_FLOATING_POINT;
          memcpy(&teb->o.odin.context.FloatSave, &lpContext->FloatSave, sizeof(lpContext->FloatSave));
      }
      SetLastError(ERROR_SUCCESS);
      return TRUE;
  }
  dprintf(("!WARNING!: TEB not found!!"));
  SetLastError(ERROR_INVALID_HANDLE);
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
