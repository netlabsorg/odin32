/* $Id: debug.cpp,v 1.1 1999-12-31 10:47:11 sandervl Exp $ */

/*
 * Win32 debug Subsystem for OS/2
 *
 * Copyright 1999 Edgar Buerkle
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <os2win.h>
#include <misc.h>
#include "oslibdebug.h"

/*****************************************************************************
 * Name      : BOOL WaitForDebugEvent
 * Purpose   : The WaitForDebugEvent function waits for a debugging event to
 *             occur in a process being debugged.
 * Parameters: LPDEBUG_EVENT lpde      address of structure for event information
 *             DWORD         dwTimeout number of milliseconds to wait for event
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/
BOOL WIN32API WaitForDebugEvent(LPDEBUG_EVENT lpde, DWORD dwTimeout)
{
  dprintf(("KERNEL32: WaitForDebugEvent timeout:%d\n", dwTimeout));

  return OSLibWaitForDebugEvent(lpde, dwTimeout);
}
/*****************************************************************************
 * Name      : BOOL WIN32API ContinueDebugEvent
 * Purpose   : The ContinueDebugEvent function enables a debugger to continue
 *             a thread that previously reported a debugging event.
 * Parameters: DWORD dwProcessId       process to continue
 *             DWORD dwThreadId        thread to continue
 *             DWORD dwContinueStatus  continuation status
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Only the thread that created dwProcessId with the CreateProcess
 *             function can call ContinueDebugEvent.
 *             After the ContinueDebugEvent function succeeds, the specified
 *             thread continues. Depending on the debugging event previously
 *             reported by the thread, different actions occur. If the continued
 *             thread previously reported an EXIT_THREAD_DEBUG_EVENT
 *             debugging event, ContinueDebugEvent closes the handle the
 *             debugger has to the thread. If the continued thread previously
 *             reported an EXIT_PROCESS_DEBUG_EVENT debugging event,
 *             ContinueDebugEvent closes the handles the debugger has to the
 *             process and to the thread.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/
BOOL WIN32API ContinueDebugEvent( DWORD dwProcessId, DWORD dwThreadId,
                                  DWORD dwContinueStatus)
{

  dprintf(("KERNEL32:  ContinueDebugEvent(%08x,%08x,%08x)not correctly implemented\n",
           dwProcessId, dwThreadId, dwContinueStatus));

  return OSLibContinueDebugEvent(dwProcessId, dwThreadId, dwContinueStatus);
}
//******************************************************************************
//******************************************************************************
