/* $Id: thread.cpp,v 1.3 1999-05-31 22:08:13 phaller Exp $ */

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
#include "misc.h"

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

  dprintf(("CreateThread (%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           lpsa,
           cbStack,
           lpStartAddr,
           lpvThreadParm,
           fdwCreate,
           lpIDThread));

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
             hThread
             priority));

    return O32_SetThreadPriority(hThread, priority);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API ExitThread(DWORD exitcode)
{
  dprintf(("ExitThread (%08xu)\n",
           exitcode));

  O32_ExitThread(exitcode);
}
//******************************************************************************
//******************************************************************************
Win32Thread::Win32Thread(LPTHREAD_START_ROUTINE pUserCallback, LPVOID lpData)
{
  lpUserData = lpData;
  pCallback  = pUserCallback;
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

  dprintf(("Win32ThreadProc %d\n", 
           GetCurrentThreadId()));

  ReplaceExceptionHandler();
  return(winthread(userdata));
}
//******************************************************************************
//******************************************************************************
