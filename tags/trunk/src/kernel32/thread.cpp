/* $Id: thread.cpp,v 1.50 2003-02-04 11:29:03 sandervl Exp $ */

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
#include <handlemanager.h>
#include <codepage.h>

#define DBG_LOCALLOG	DBG_thread
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-THREAD)

static ULONG priorityclass = NORMAL_PRIORITY_CLASS;

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

    winteb->flags = me->dwFlags;

    winteb->entry_point = (void *)threadCallback;
    winteb->entry_arg   = (void *)userdata;

    winteb->o.odin.hab = OSLibWinInitialize();
    dprintf(("Thread HAB %x", winteb->o.odin.hab));
    winteb->o.odin.hmq = OSLibWinQueryMsgQueue(winteb->o.odin.hab);
    rc = OSLibWinSetCp(winteb->o.odin.hmq, GetDisplayCodepage());
    dprintf(("WinSetCP was %sOK", rc ? "" : "not "));

    dprintf(("Win32ThreadProc: hab %x hmq %x", winteb->o.odin.hab, winteb->o.odin.hmq));
    dprintf(("Stack top 0x%x, stack end 0x%x", winteb->stack_top, winteb->stack_low));

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

    if(WinExe) WinExe->tlsAttachThread();	    //setup TLS structure of main exe
    Win32DllBase::tlsAttachThreadToAllDlls(); //setup TLS structures of all dlls
    Win32DllBase::attachThreadToAllDlls();    //send DLL_THREAD_ATTACH message to all dlls

    //Set FPU control word to 0x27F (same as in NT)
    CONTROL87(0x27F, 0xFFF);
    rc = AsmCallThreadHandler(threadCallback, userdata);

    if(fExitProcess) {
        OSLibDosExitThread(rc);
    }
    else {
        HMSetThreadTerminated(GetCurrentThread());
        winteb->o.odin.exceptFrame = 0;
        Win32DllBase::detachThreadFromAllDlls();  //send DLL_THREAD_DETACH message to all dlls
        Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
        if(WinExe) WinExe->tlsDetachThread();		  //destroy TLS structure of main exe
        DestroyTEB(winteb);  //destroys TIB and restores FS
        OS2UnsetExceptionHandler((void *)&exceptFrame);
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
