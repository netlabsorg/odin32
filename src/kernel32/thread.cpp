/* $Id: thread.cpp,v 1.1 1999-05-24 20:19:48 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Thread API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include "thread.h"
#include "except.h"

static DWORD OPEN32API Win32ThreadProc(LPVOID lpData);


//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateThread(LPSECURITY_ATTRIBUTES lpsa, DWORD cbStack,
                   LPTHREAD_START_ROUTINE lpStartAddr,
                   LPVOID lpvThreadParm, DWORD fdwCreate,
                   LPDWORD lpIDThread)
{
 Win32Thread *winthread = new Win32Thread(lpStartAddr, lpvThreadParm);

  if(winthread == 0)
    return(0);

  dprintf(("CreateThread\n"));
  return(O32_CreateThread(lpsa, cbStack, winthread->GetOS2Callback(), (LPVOID)winthread, fdwCreate, lpIDThread));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetCurrentThreadId()
{
  dprintf(("GetCurrentThreadId\n"));
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
    dprintf(("GetExitCodeThread\n"));
    return O32_GetExitCodeThread(hThread, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TerminateThread(HANDLE hThread, DWORD arg2)
{
    dprintf(("TerminateThread\n"));
    return O32_TerminateThread(hThread, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API ResumeThread(HANDLE hThread)
{
    dprintf(("ResumeThread\n"));
    return O32_ResumeThread(hThread);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetThreadPriority(HANDLE hThread)
{
    dprintf(("OS2GetThreadPriority\n"));
    return O32_GetThreadPriority(hThread);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SuspendThread(HANDLE hThread)
{
    dprintf(("OS2SuspendThread\n"));
    return O32_SuspendThread(hThread);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetThreadPriority(HANDLE hThread, int priority)
{
    dprintf(("OS2SetThreadPriority\n"));
    return O32_SetThreadPriority(hThread, priority);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitThread(DWORD exitcode)
{
  dprintf(("ExitThread\n"));
  O32_ExitThread(exitcode);
}
//******************************************************************************
//******************************************************************************
Win32Thread::Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData)
{
  lpUserData = lpData;
  pCallback  =pUserCallback;
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
 Win32Thread     *me = (Win32Thread *)lpData;
 WIN32THREADPROC  winthread = me->pCallback;
 LPVOID           userdata  = me->lpUserData;

  delete me;    //only called once

  dprintf(("Win32ThreadProc %d\n", GetCurrentThreadId()));
  ReplaceExceptionHandler();
  return(winthread(userdata));
}
//******************************************************************************
//******************************************************************************
