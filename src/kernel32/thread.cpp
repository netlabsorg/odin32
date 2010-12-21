/* $Id: thread.cpp,v 1.55 2004-05-24 08:56:07 sandervl Exp $ */

/*
 * Win32 Thread API functions
 *
 * TODO: Initialize threadInfo structure during thread creation
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdarg.h>
#include <string.h>
#include "thread.h"
#include <misc.h>
#include <cpuhlp.h>
#include <wprocess.h>
#include <windllbase.h>
#include <winexebase.h>
#include "exceptutil.h"
#include "oslibmisc.h"
#include "oslibdos.h"
#include "oslibthread.h"
#include <handlemanager.h>
#include <codepage.h>
#include <heapstring.h>

#include "hmhandle.h"
#include "hmthread.h"
#include <kbdhook.h>

#include <FastInfoBlocks.h>
#include <custombuild.h>

#define DBG_LOCALLOG    DBG_thread
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-THREAD)

static ULONG priorityclass = NORMAL_PRIORITY_CLASS;

#define MQP_INSTANCE_PERMQ              0x00000001 // from os2im.h

//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateThread(LPSECURITY_ATTRIBUTES  lpsa,
                             DWORD                  cbStack,
                             LPTHREAD_START_ROUTINE lpStartAddr,
                             LPVOID                 lpvThreadParm,
                             DWORD                  fdwCreate,
                             LPDWORD                lpIDThread)
{
    return HMCreateThread(lpsa, cbStack, lpStartAddr, lpvThreadParm, fdwCreate, lpIDThread);
}
/*****************************************************************************
 * Name      : HMCreateThread
 * Purpose   : router function for CreateThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
HANDLE HMCreateThread(LPSECURITY_ATTRIBUTES  lpsa,
                      DWORD                  cbStack,
                      LPTHREAD_START_ROUTINE lpStartAddr,
                      LPVOID                 lpvThreadParm,
                      DWORD                  fdwCreate,
                      LPDWORD                lpIDThread,
                      BOOL                   fRegisterThread)
{
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLE       pHandle;
  HANDLE          rc;                                     /* API return code */

  SetLastError(ERROR_SUCCESS);

  pHandle = HMHandleGetFreePtr(HMTYPE_THREAD);                         /* get free handle */
  if (pHandle == NULL)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0; //according to MSDN
  }

  /* call the device handler */
  rc = pHandle->pDeviceHandler->CreateThread(&pHandle->hmHandleData,
                                    lpsa,  cbStack, lpStartAddr,
                                    lpvThreadParm, fdwCreate, lpIDThread, fRegisterThread);

  if (rc == 0)     /* oops, creation failed within the device handler */
  {
      HMHandleFree(pHandle->hmHandleData.hWin32Handle);
      return 0;                                           /* signal error */
  }

  return pHandle->hmHandleData.hWin32Handle;
}
/*****************************************************************************
 * Name      : HMGetThreadPriority
 * Purpose   : router function for GetThreadPriority
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
INT WIN32API GetThreadPriority(HANDLE hThread)
{
  INT       lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return -1; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->GetThreadPriority(hThread, &pHMHandle->hmHandleData);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSuspendThread
 * Purpose   : router function for SuspendThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD WIN32API SuspendThread(HANDLE hThread)
{
  HANDLE    lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                         /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return -1; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->SuspendThread(hThread, &pHMHandle->hmHandleData);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadPriority
 * Purpose   : router function for SetThreadPriority
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API SetThreadPriority(HANDLE hThread, int priority)
{
  BOOL      lpResult;                   /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }
  lpResult = pHMHandle->pDeviceHandler->SetThreadPriority(hThread, &pHMHandle->hmHandleData, priority);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMGetThreadContext
 * Purpose   : router function for GetThreadContext
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API GetThreadContext(HANDLE hThread, CONTEXT *lpContext)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->GetThreadContext(hThread, &pHMHandle->hmHandleData, lpContext);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadContext
 * Purpose   : router function for SetThreadContext
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API SetThreadContext(HANDLE hThread, const CONTEXT *lpContext)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }
  lpResult = pHMHandle->pDeviceHandler->SetThreadContext(hThread, &pHMHandle->hmHandleData, lpContext);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMGetThreadTimes
 * Purpose   : router function for HMGetThreadTimes
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API GetThreadTimes(HANDLE hThread, LPFILETIME lpCreationTime,
                      LPFILETIME lpExitTime, LPFILETIME lpKernelTime,
                      LPFILETIME lpUserTime)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->GetThreadTimes(hThread, &pHMHandle->hmHandleData,
                                                       lpCreationTime, lpExitTime,
                                                       lpKernelTime, lpUserTime);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMTerminateThread
 * Purpose   : router function for TerminateThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API TerminateThread(HANDLE hThread, DWORD exitcode)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->TerminateThread(hThread, &pHMHandle->hmHandleData, exitcode);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMResumeThread
 * Purpose   : router function for ResumeThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD WIN32API ResumeThread(HANDLE hThread)
{
  DWORD     lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                 /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return -1; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->ResumeThread(hThread, &pHMHandle->hmHandleData);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMGetExitCodeThread
 * Purpose   : router function for GetExitCodeThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL WIN32API GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  lpResult = pHMHandle->pDeviceHandler->GetExitCodeThread(hThread, &pHMHandle->hmHandleData, lpExitCode);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadTerminated
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMSetThreadTerminated(HANDLE hThread)
{
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hThread);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }
  lpResult = pHMHandle->pDeviceHandler->SetThreadTerminated(hThread, &pHMHandle->hmHandleData);

  return (lpResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetCurrentThreadId()
{
    // check cached identifier
    TEB *teb = GetThreadTEB();
    if(teb != NULL && teb->o.odin.threadId != 0xFFFFFFFF)
    {
        // this is set in InitializeTIB() already.
        return teb->o.odin.threadId;
    }

////  dprintf(("GetCurrentThreadId\n"));
    return MAKE_THREADID(O32_GetCurrentProcessId(), O32_GetCurrentThreadId());
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentThread()
{
    TEB *teb;

    teb = GetThreadTEB();
    if(teb == 0) {
        DebugInt3();
        SetLastError(ERROR_INVALID_HANDLE); //todo
        return 0;
    }
    return teb->o.odin.hThread;
}
//******************************************************************************
// these two debugging functions allow access to a
// calldepth counter inside the TEB block of each thread
//******************************************************************************
ULONG WIN32API dbg_GetThreadCallDepth()
{
#ifdef DEBUG
  TEB *teb;

  teb = GetThreadTEB();
  if(teb == NULL)
    return 0;
  else
    return teb->o.odin.dbgCallDepth;
#else
  return 0;
#endif
}
//******************************************************************************
//******************************************************************************
void WIN32API dbg_IncThreadCallDepth()
{
#ifdef DEBUG
  TEB *teb;

  teb = GetThreadTEB();
  if(teb != NULL)
    teb->o.odin.dbgCallDepth++;
#endif
}
//******************************************************************************
#define MAX_CALLSTACK_SIZE 128
#ifdef DEBUG
static char *pszLastCaller = NULL;
#endif
//******************************************************************************
void WIN32API dbg_ThreadPushCall(char *pszCaller)
{
#ifdef DEBUG
  TEB *teb;

  // embedded dbg_IncThreadCallDepth
  teb = GetThreadTEB();
  if(teb == NULL)
    return;

  // add caller name to call stack trace
  int iIndex = teb->o.odin.dbgCallDepth;

  // allocate callstack on demand
  if (teb->o.odin.arrstrCallStack == NULL)
    teb->o.odin.arrstrCallStack = (PVOID*)malloc( sizeof(LPSTR) * MAX_CALLSTACK_SIZE);

  // insert entry
  if (iIndex < MAX_CALLSTACK_SIZE)
    teb->o.odin.arrstrCallStack[iIndex] = (PVOID)pszCaller;

  teb->o.odin.dbgCallDepth++;

  pszLastCaller = pszCaller;
#endif
}
//******************************************************************************
//******************************************************************************
void WIN32API dbg_DecThreadCallDepth()
{
#ifdef DEBUG
  TEB *teb;

  teb = GetThreadTEB();
  if(teb != NULL)
    --(teb->o.odin.dbgCallDepth);
#endif
}
//******************************************************************************
//******************************************************************************
void WIN32API dbg_ThreadPopCall()
{
#ifdef DEBUG
  TEB *teb;

  // embedded dbg_DecThreadCallDepth
  teb = GetThreadTEB();
  if(teb == NULL)
    return;

  --(teb->o.odin.dbgCallDepth);

  // add caller name to call stack trace
  int iIndex = teb->o.odin.dbgCallDepth;

  // insert entry
  if (teb->o.odin.arrstrCallStack)
    if (iIndex < MAX_CALLSTACK_SIZE)
      teb->o.odin.arrstrCallStack[iIndex] = NULL;
#endif
}
//******************************************************************************
//******************************************************************************
char* WIN32API dbg_GetLastCallerName()
{
#ifdef DEBUG
  // retrieve last caller name from stack
  TEB *teb;

  // embedded dbg_DecThreadCallDepth
  teb = GetThreadTEB();
  if(teb != NULL)
  {
    int iIndex = teb->o.odin.dbgCallDepth - 1;
    if ( (iIndex > 0) &&
         (iIndex < MAX_CALLSTACK_SIZE) )
    {
      return (char*)teb->o.odin.arrstrCallStack[iIndex];
    }
  }
#endif

  return NULL;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitThread(DWORD exitcode)
{
    EXCEPTION_FRAME *exceptFrame;
    TEB             *teb;

    dprintf(("ExitThread %x (%x)", GetCurrentThread(), exitcode));

    teb = GetThreadTEB();
    if(teb != 0) {
         exceptFrame = (EXCEPTION_FRAME *)teb->o.odin.exceptFrame;
    }
    else DebugInt3();

    HMSetThreadTerminated(GetCurrentThread());
    Win32DllBase::detachThreadFromAllDlls();    //send DLL_THREAD_DETACH message to all dlls
    Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
    if(WinExe)  WinExe->tlsDetachThread();      //destroy TLS structure of main exe

    if(teb) DestroyTEB(teb);

    if(exceptFrame) OS2UnsetExceptionHandler((void *)exceptFrame);

    O32_ExitThread(exitcode);
}
/*****************************************************************************
 * Name      : DWORD SetThreadAffinityMask
 * Purpose   : The SetThreadAffinityMask function sets a processor affinity
 *             mask for a specified thread.
 *             A thread affinity mask is a bit vector in which each bit
 *             represents the processors that a thread is allowed to run on.
 *             A thread affinity mask must be a proper subset of the process
 *             affinity mask for the containing process of a thread. A thread
 *             is only allowed to run on the processors its process is allowed to run on.
 * Parameters: HANDLE hThread              handle to the thread of interest
 *             DWORD  dwThreadAffinityMask a thread affinity mask
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : Fully functional
 *
 * Author    : SvL
 *****************************************************************************/

DWORD WIN32API SetThreadAffinityMask(HANDLE hThread,
                                     DWORD  dwThreadAffinityMask)
{
    dprintf(("KERNEL32: SetThreadAffinityMask(%08xh,%08xh)", hThread, dwThreadAffinityMask));

    if(hThread != GetCurrentThread()) {
        dprintf(("WARNING: Setting the affinity mask for another thread than the current one is not supported!!"));
        return FALSE;
    }
    return OSLibDosSetThreadAffinity(dwThreadAffinityMask);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API Sleep(DWORD mSecs)
{
    dprintf2(("KERNEL32: Sleep %d", mSecs));
    OSLibDosSleep(mSecs);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetPriorityClass(HANDLE hProcess)
{
    dprintf(("KERNEL32: GetPriorityClass %x", hProcess));
    return priorityclass;
//    return O32_GetPriorityClass(hProcess);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPriorityClass(HANDLE hProcess, DWORD dwPriority)
{
    dprintf(("KERNEL32: SetPriorityClass %x %x", hProcess, dwPriority));
    priorityclass = dwPriority;
    return TRUE;
//    return O32_SetPriorityClass(hProcess, dwPriority);
}
//******************************************************************************
//******************************************************************************
/*****************************************************************************
 * Name      : BOOL SetThreadPriorityBoost
 * Purpose   : The SetThreadPriorityBoost function disables or enables
 *             the ability of the system to temporarily boost the priority
 *             of a thread.
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/08 21:44]
 *****************************************************************************/

BOOL WIN32API SetThreadPriorityBoost(HANDLE hThread,
                                     BOOL DisablePriorityBoost)
{
  dprintf(("KERNEL32: SetThreadPriorityBoost(%08xh, %08xh) not implemented\n",
           hThread,DisablePriorityBoost));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
Win32Thread::Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData, DWORD dwFlags, HANDLE hThread)
{
    lpUserData = lpData;
    pCallback  = pUserCallback;
    this->dwFlags = dwFlags;
    this->hThread = hThread;

    teb = CreateTEB(hThread, 0xFFFFFFFF);
    if(teb == NULL) {
        DebugInt3();
    }
}
//******************************************************************************
//******************************************************************************
DWORD OPEN32API Win32ThreadProc(LPVOID lpData)
{
    EXCEPTION_FRAME  exceptFrame;
    Win32Thread     *me = (Win32Thread *)lpData;
    ULONG            dwFlags = me->dwFlags;
    ULONG            threadCallback = (ULONG)me->pCallback;
    LPVOID           userdata  = me->lpUserData;
    DWORD            rc;
    TEB             *winteb    = (TEB *)me->teb;

    delete(me);    //only called once

    if(InitializeThread(winteb) == FALSE) {
        dprintf(("Win32ThreadProc: InitializeTIB failed!!"));
        DebugInt3();
        return 0;
    }
    dprintf(("Win32ThreadProc: Thread handle 0x%x, thread id %d", GetCurrentThread(), GetCurrentThreadId()));

    winteb->flags = dwFlags;

    winteb->entry_point = (void *)threadCallback;
    winteb->entry_arg   = (void *)userdata;

    winteb->o.odin.hab = OSLibWinInitialize();
    dprintf(("Thread HAB %x", winteb->o.odin.hab));
    winteb->o.odin.hmq = OSLibWinQueryMsgQueue(winteb->o.odin.hab);
    rc = OSLibWinSetCp(winteb->o.odin.hmq, GetDisplayCodepage());
    dprintf(("WinSetCP(%d) was %sOK", GetDisplayCodepage(), rc ? "" : "not "));
    hookInit(winteb->o.odin.hab);

    dprintf(("Win32ThreadProc: hab %x hmq %x", winteb->o.odin.hab, winteb->o.odin.hmq));
    dprintf(("Stack top 0x%x, stack end 0x%x", winteb->stack_top, winteb->stack_low));

    if( IsDBCSEnv())
        /* IM instace is created per message queue, that is, thread */
        OSLibImSetMsgQueueProperty( winteb->o.odin.hmq, MQP_INSTANCE_PERMQ );

    //Note: The Win32 exception structure referenced by FS:[0] is the same
    //      in OS/2
    OS2SetExceptionHandler((void *)&exceptFrame);
    winteb->o.odin.exceptFrame = (ULONG)&exceptFrame;

    //Determine if thread callback is inside a PE dll; if true, then force
    //switch to win32 TIB (FS selector)
    //(necessary for Opera when loading win32 plugins that create threads)
    Win32DllBase *dll;
    dll = Win32DllBase::findModuleByAddr(threadCallback);
    if(dll && dll->isPEImage()) {
         dprintf(("Win32ThreadProc: Force win32 TIB switch"));
         SetWin32TIB(TIB_SWITCH_FORCE_WIN32);
    }
    else SetWin32TIB(TIB_SWITCH_DEFAULT); //executable type determines whether or not FS is changed

    DWORD dwProcessAffinityMask, dwSystemAffinityMask;

    //Change the affinity mask of this thread to the mask for the whole process
    if(GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinityMask, &dwSystemAffinityMask) == TRUE) {
        SetThreadAffinityMask(GetCurrentThread(), dwProcessAffinityMask);
    }

    if(WinExe) WinExe->tlsAttachThread();       //setup TLS structure of main exe
    Win32DllBase::tlsAttachThreadToAllDlls(); //setup TLS structures of all dlls
    Win32DllBase::attachThreadToAllDlls();    //send DLL_THREAD_ATTACH message to all dlls

    BOOL fAlignStack = ((ULONG)winteb->stack_top - (ULONG)winteb->stack_low) >= 128*1024;

    //Set FPU control word to 0x27F (same as in NT)
    CONTROL87(0x27F, 0xFFF);
    rc = AsmCallThreadHandler(fAlignStack, threadCallback, userdata);

    if(fExitProcess) {
        OSLibDosExitThread(rc);
    }
    else {
        hookKill(winteb->o.odin.hab);
        HMSetThreadTerminated(GetCurrentThread());
        winteb->o.odin.exceptFrame = 0;
        Win32DllBase::detachThreadFromAllDlls();  //send DLL_THREAD_DETACH message to all dlls
        Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
        if(WinExe) WinExe->tlsDetachThread();         //destroy TLS structure of main exe
        DestroyTEB(winteb);  //destroys TIB and restores FS
        OS2UnsetExceptionHandler((void *)&exceptFrame);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************

/**
 * Enter odin context with this thread.
 *
 * Is called when an OS/2 process is calling into an Odin32 DLL.
 * This may be called also in a nested fashion and supports that.
 * The conterpart of ODIN_ThreadLeaveOdinContext().
 *
 * @returns The old FS selector.
 * @returns 0 if TEB creation failed.
 * @param   pExceptionRegRec    OS/2 Exception Registration Record (2 ULONGs)
 *                              must be located on the callers stack.
 * @param   fForceFSSwitch      If set we will force switching to Odin32 FS selector.
 *                              If clear it depends on defaults.
 * @deprecated
 */
USHORT WIN32API ODIN_ThreadEnterOdinContext(void *pExceptionRegRec, BOOL fForceFSSwitch)
{
    USHORT  selFSOld = 0;

    /*
     * Get TEB pointer, create it if necessary.
     * @todo    Check if this really is the thread which the TEB was created
     *          for. If not create the TEB.
     */
    TEB *pTeb = GetThreadTEB();
    if (!pTeb)
    {
        HANDLE hThreadMain = HMCreateThread(NULL, 0, 0, 0, 0, 0, TRUE);
        pTeb = CreateTEB(hThreadMain, ODIN_GetCurrentThreadId());
        if (!pTeb || InitializeThread(pTeb, fibGetTid() == 1) == FALSE)
        {
            dprintf(("ODIN_ThreadEnterOdinContext: Failed to create TEB!"));
        }
    }

    /*
     * Install the Odin32 exception handler.
     * Note: The Win32 exception structure referenced by FS:[0] is the same in OS/2
     */
    if (pExceptionRegRec)
        OS2SetExceptionHandler(pExceptionRegRec);
    if (    pTeb
        &&  !pTeb->o.odin.exceptFrame)  /* if allready present, we'll keep the first one. */
        pTeb->o.odin.exceptFrame = (ULONG)pExceptionRegRec;

    /*
     * Switch Selector if TIB was created.
     */
    if (pTeb)
        selFSOld = SetWin32TIB(fForceFSSwitch ? TIB_SWITCH_FORCE_WIN32 : TIB_SWITCH_DEFAULT);

    return selFSOld;
}


/**
 * Leave odin context with this thread.
 *
 * Is called when an OS/2 process is returning from an Odin32 DLL.
 * This may be called also in a nested fashion and supports that.
 * The conterpart of ODIN_ThreadEnterOdinContext().
 *
 * @returns The old FS selector.
 * @returns 0 if TEB creation failed.
 * @param   pExceptionRegRec    OS/2 Exception Registration Record (2 ULONGs)
 *                              must be located on the callers stack.
 * @param   fForceFSSwitch      If set we will force switching to Odin32 FS selector.
 *                              If clear it depends on defaults.
 * @deprecated
 */
void   WIN32API ODIN_ThreadLeaveOdinContext(void *pExceptionRegRec, USHORT selFSOld)
{
    /*
     * Install the Odin32 exception handler.
     * Note: The Win32 exception structure referenced by FS:[0] is the same in OS/2
     */
    if (pExceptionRegRec)
        OS2UnsetExceptionHandler(pExceptionRegRec);
    TEB *pTeb = GetThreadTEB();
    if (    pTeb
        &&  pTeb->o.odin.exceptFrame == (ULONG)pExceptionRegRec)
        pTeb->o.odin.exceptFrame = 0;

    /*
     * Switch Back FS Selector.
     */
    if (selFSOld)
        SetFS(selFSOld);
}


/**
 * Leave odin context to call back into OS/2 code.
 *
 * Is called when and Odin32 Dll/Exe calls back into the OS/2 code.
 * The conterpart of ODIN_ThreadEnterOdinContextNested().
 *
 * @returns The old FS selector.
 * @returns 0 on failure.
 * @param   pExceptionRegRec    New OS/2 exception handler frame which are to be registered
 *                              before the Odin handler in the chain.
 *                              Must be located on the callers stack.
 * @param   fRemoveOdinExcpt    Remove the odin exception handler.
 * @deprecated
 */
USHORT WIN32API ODIN_ThreadLeaveOdinContextNested(void *pExceptionRegRec, BOOL fRemoveOdinExcpt)
{
    /*
     * Set OS/2 FS Selector.
     */
    USHORT  selFSOld = RestoreOS2FS();

    /*
     * Remove the Odin exception handler (if requested).
     */
    if (fRemoveOdinExcpt)
    {
        TEB *pTeb = GetThreadTEB();
        if (pTeb)
            OS2UnsetExceptionHandler((void*)pTeb->o.odin.exceptFrame);
        /* else: no TEB created propbably no exception handler to remove. */
    }

    /*
     * Change exception handler if required.
     */
    if (pExceptionRegRec)
    {
        extern unsigned long _System DosSetExceptionHandler(void *);
        DosSetExceptionHandler(pExceptionRegRec);
    }

    return selFSOld;
}


/**
 * Re-enter Odin context after being back in OS/2 code.
 *
 * Is called when returning to Odin from OS/2 code.
 * The conterpart of ODIN_ThreadLeaveOdinContextNested().
 *
 * @param   pExceptionRegRec    The exception registration record for the OS/2
 *                              exception handler used with Nested Leave. NULL
 *                              if not used.
 * @param   fRestoreOdinExcpt   Restore the Odin exception handler.
 *                              This flag must not be set unless fRemoveOdinExcpt
 *                              was set when leaving the Odin context!
 * @param   selFSOld            The Odin FS selector returned by the Nested Leave api.
 *
 * @deprecated
 */
void   WIN32API ODIN_ThreadEnterOdinContextNested(void *pExceptionRegRec, BOOL fRestoreOdinExcpt, USHORT selFSOld)
{
    /*
     * Remove the exception handler registered in ODIN_ThreadLeaveOdinContextNested
     */
    if (pExceptionRegRec)
        OS2UnsetExceptionHandler(pExceptionRegRec);

    /*
     * Restore Odin exception handler (if requested).
     */
    if (fRestoreOdinExcpt)
    {
        TEB *pTeb = GetThreadTEB();
        if (pTeb)
            OS2SetExceptionHandler((void*)pTeb->o.odin.exceptFrame);
    }

    /*
     * Switch Back FS Selector.
     */
    if (selFSOld)
        SetFS(selFSOld);
}


/** Save thread context and/or load other thread context.
 * @param   pCtx    Where to save the current thread context.
 * @param   fFlags  Flags telling what to do.
 * @todo Need to do special handling of NESTED flag?
 */
void    WIN32API ODIN_ThreadContextSave(PODINTHREADCTX pCtx, unsigned fFlags)
{
    TEB *pTeb = NULL;

    /*
     * Do requested saves.
     */
    if (pCtx)
    {
        memset(pCtx, 0, sizeof(*pCtx));
        pCtx->fFlags = fFlags;
        if (fFlags & OTCTXF_SAVE_FPU)
            pCtx->cw = _control87(0, 0);
        if (fFlags & OTCTXF_SAVE_FS)
            pCtx->fs = GetFS();
    }

    /*
     * Get Odin32 TEB.
     */
    if (fFlags & (OTCTXF_LOAD_FS_ODIN32 | OTCTXF_LOAD_XCPT_ODIN32))
    {
        /*
         * Get TEB pointer, create it if necessary.
         * @todo    Check if this really is the thread which the TEB was created
         *          for. If not create the TEB. This is rather unlikely..
         */
        pTeb = GetThreadTEB();
        if (!pTeb)
        {
            HANDLE hThreadMain = HMCreateThread(NULL, 0, 0, 0, 0, 0, TRUE);
            dprintf(("Setup external thread %x!", hThreadMain));
            pTeb = CreateTEB(hThreadMain, ODIN_GetCurrentThreadId());
            if (!pTeb ||  !InitializeThread(pTeb, fibGetTid() == 1))
            {
                dprintf(("ODIN_ThreadContextSave: Failed to create TEB!"));
                DebugInt3();
            }
        }
    }

    /*
     * Install exception handler if requested.
     */
    if (fFlags & OTCTXF_LOAD_XCPT_ODIN32)
    {
        OS2UnsetExceptionHandler(&pCtx->XctpRegRec);
        if (    pTeb
            &&  !pTeb->o.odin.exceptFrame)  /* if allready present, we'll keep the first one. */
            pTeb->o.odin.exceptFrame = (ULONG)&pCtx->XctpRegRec;
    }

    /*
     * Do requested loads.
     */
    if (fFlags & OTCTXF_LOAD_FPU_ODIN32)
        CONTROL87(0x27F, 0xFFFF);   //Set FPU control word to 0x27F (same as in NT)
    if (fFlags & OTCTXF_LOAD_FPU_OS2)
        CONTROL87(0x37F, 0xFFFF);   //Set FPU control word to 0x37F as that's most common on OS/2.
    if ((fFlags & OTCTXF_LOAD_FS_ODIN32) && pTeb)
        SetFS(pTeb->teb_sel);
}


/** Restore saved thread context and/or do additional loads.
 * @param   pCtx    Where to save the current thread context.
 * @param   fFlags  Flags telling extra stuff to load.
 *                  Only CTCTXF_LOAD_* flags will be evaluated.
 * @todo Need to do special handling of NESTED flag?
 */
void    WIN32API ODIN_ThreadContextRestore(PODINTHREADCTX pCtx, unsigned fFlags)
{
    /*
     * Restore context.
     */
    if (pCtx)
    {
        if (pCtx->fFlags & OTCTXF_SAVE_FPU)
            CONTROL87(pCtx->cw, 0xffff);
        if (pCtx->fFlags & OTCTXF_SAVE_FS)
            SetFS(pCtx->fs);
        if (pCtx->fFlags & OTCTXF_LOAD_XCPT_ODIN32)
        {
            TEB *pTeb = GetThreadTEB();
            if (pTeb && pTeb->o.odin.exceptFrame == (ULONG)&pCtx->XctpRegRec)
                pTeb->o.odin.exceptFrame = 0;
            OS2UnsetExceptionHandler(&pCtx->XctpRegRec);
        }
        memset(pCtx, 0, sizeof(*pCtx));
    }

    /*
     * Do requested loads.
     */
    if (fFlags & OTCTXF_LOAD_FPU_ODIN32)
        CONTROL87(0x27F, 0xFFFF);   //Set FPU control word to 0x27F (same as in NT)
    if (fFlags & OTCTXF_LOAD_FPU_OS2)
        CONTROL87(0x37F, 0xFFFF);   //Set FPU control word to 0x37F as that's most common on OS/2.
    if ((fFlags & OTCTXF_LOAD_FS_ODIN32))
    {
        /*
         * Get TEB pointer, create it if necessary.
         * @todo    Check if this really is the thread which the TEB was created
         *          for. If not create the TEB. This is rather unlikely..
         */
        TEB *pTeb = GetThreadTEB();
        if (!pTeb)
        {
            HANDLE hThreadMain = HMCreateThread(NULL, 0, 0, 0, 0, 0, TRUE);
            pTeb = CreateTEB(hThreadMain, ODIN_GetCurrentThreadId());
            if (    !pTeb
                ||  !InitializeThread(pTeb, fibGetTid() == 1))
            {
                dprintf(("ODIN_ThreadContextRestore: Failed to create TEB!"));
                DebugInt3();
            }
        }
        if (pTeb)
            SetFS(pTeb->teb_sel);
    }
}

