/* $Id: thread.cpp,v 1.23 2000-02-16 14:23:12 sandervl Exp $ */

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
#include "exceptutil.h"
#include <misc.h>
#include <wprocess.h>
#include <windllbase.h>
#include <winexebase.h>
#include "exceptutil.h"
#include "oslibmisc.h"

#define DBG_LOCALLOG	DBG_thread
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-THREAD)


static DWORD OPEN32API Win32ThreadProc(LPVOID lpData);


//******************************************************************************
//******************************************************************************
ODINFUNCTION6(HANDLE,CreateThread,LPSECURITY_ATTRIBUTES,  lpsa,
                                  DWORD,                  cbStack,
                                  LPTHREAD_START_ROUTINE, lpStartAddr,
                                  LPVOID,                 lpvThreadParm,
                                  DWORD,                  fdwCreate,
                                  LPDWORD,                lpIDThread)
{
  Win32Thread *winthread;

  winthread = new Win32Thread(lpStartAddr, lpvThreadParm, fdwCreate);

  if(winthread == 0)
    return(0);

#ifdef DEBUG
  // @@@PH Note: with debug code enabled, ODIN might request more stack space!
  if (cbStack > 0)
     cbStack <<= 1;     // double stack
  else
     cbStack = 1048576; // per default 1MB stack per thread
#endif

  return(O32_CreateThread(lpsa,
                          cbStack,
                          winthread->GetOS2Callback(),
                          (LPVOID)winthread,
                          fdwCreate,
                          lpIDThread));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetCurrentThreadId()
{
////  dprintf(("GetCurrentThreadId\n"));
  return(O32_GetCurrentThreadId());
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetCurrentThread()
{
////  dprintf(("GetCurrentThread\n"));
  return(O32_GetCurrentThread());
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetExitCodeThread(HANDLE hThread, LPDWORD arg2)
{
    dprintf(("GetExitCodeThread (%08xh,%08xh)\n",
             hThread,
             arg2));

    return O32_GetExitCodeThread(hThread, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TerminateThread(HANDLE hThread, DWORD arg2)
{
    dprintf(("TerminateThread (%08xh,%08xh)\n",
             hThread,
             arg2));

    return O32_TerminateThread(hThread, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ResumeThread(HANDLE hThread)
{
    dprintf(("ResumeThread (%08xh)\n",
             hThread));

    return O32_ResumeThread(hThread);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetThreadPriority(HANDLE hThread)
{
    dprintf(("OS2GetThreadPriority(%08xh)\n",
             hThread));

    return O32_GetThreadPriority(hThread);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SuspendThread(HANDLE hThread)
{
    dprintf(("OS2SuspendThread %08xh)\n",
             hThread));

    return O32_SuspendThread(hThread);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadPriority(HANDLE hThread, int priority)
{
    dprintf(("OS2SetThreadPriority (%08xh,%08xh)\n",
             hThread,
             priority));

    return O32_SetThreadPriority(hThread, priority);
}
//******************************************************************************
//TODO: Implement this??
//******************************************************************************
BOOL WIN32API GetThreadContext(HANDLE hThread, PCONTEXT lpContext)
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
BOOL WIN32API SetThreadContext(HANDLE hThread, const CONTEXT *lpContext)
{
  dprintf(("SetThreadContext NOT IMPLEMENTED!!\n"));

  return FALSE;
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitThread(DWORD exitcode)
{
  dprintf(("ExitThread (%08xu)\n",
           exitcode));

  Win32DllBase::detachThreadFromAllDlls(); 	//send DLL_THREAD_DETACH message to all dlls
  Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
  WinExe->tlsDetachThread();			//destroy TLS structure of main exe
  DestroyTIB();
  O32_ExitThread(exitcode);
}
//******************************************************************************
//******************************************************************************
Win32Thread::Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData, DWORD dwFlags)
{
  lpUserData = lpData;
  pCallback  = pUserCallback;
  this->dwFlags = dwFlags;
}
//******************************************************************************
//******************************************************************************
Win32Thread::~Win32Thread()
{

}
//******************************************************************************
//******************************************************************************
PTHREAD_START_ROUTINE_O32 Win32Thread::GetOS2Callback()
{
  return Win32ThreadProc;
}
//******************************************************************************
//******************************************************************************
static DWORD OPEN32API Win32ThreadProc(LPVOID lpData)
{
 EXCEPTION_FRAME exceptFrame;
 Win32Thread     *me = (Win32Thread *)lpData;
 WIN32THREADPROC  winthread = me->pCallback;
 LPVOID           userdata  = me->lpUserData;
 DWORD            rc;

  delete(me);    //only called once
  dprintf(("Win32ThreadProc %d\n", GetCurrentThreadId()));

  TEB *winteb = (TEB *)InitializeTIB();
  if(winteb == NULL) {
	dprintf(("Win32ThreadProc: InitializeTIB failed!!"));
	DebugInt3();
	return 0;
  }
  winteb->flags = me->dwFlags;

  THDB *thdb   = (THDB *)(winteb+1);
  thdb->entry_point = (void *)winthread;
  thdb->entry_arg   = (void *)userdata;

  thdb->hab = OSLibWinInitialize();
  thdb->hmq = OSLibWinQueryMsgQueue(thdb->hab);
  dprintf(("Win32ThreadProc: hab %x hmq %x", thdb->hab, thdb->hmq));

  //Note: The Win32 exception structure referenced by FS:[0] is the same
  //      in OS/2
  OS2SetExceptionHandler((void *)&exceptFrame);

  SetWin32TIB();
  WinExe->tlsAttachThread();		  //setup TLS structure of main exe
  Win32DllBase::tlsAttachThreadToAllDlls(); //setup TLS structures of all dlls
  Win32DllBase::attachThreadToAllDlls();	  //send DLL_THREAD_ATTACH message to all dlls

  rc = winthread(userdata);

  Win32DllBase::detachThreadFromAllDlls();  //send DLL_THREAD_DETACH message to all dlls
  Win32DllBase::tlsDetachThreadFromAllDlls(); //destroy TLS structures of all dlls
  WinExe->tlsDetachThread();		  //destroy TLS structure of main exe
  DestroyTIB();
  OS2UnsetExceptionHandler((void *)&exceptFrame);

  return rc;
}
//******************************************************************************
//******************************************************************************
