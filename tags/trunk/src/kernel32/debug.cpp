/* $Id: debug.cpp,v 1.6 2001-04-05 05:54:51 sandervl Exp $ */

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
#include <unicode.h>
#include "oslibdebug.h"

#define DBG_LOCALLOG	DBG_debug
#include "dbglocal.h"

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

/**
 * Query if the calling process is running under the context of a debugger.
 * @returns     TRUE (non-zero)     Debugged.
 *              FALSE               Not debugged.
 * @status      STUB
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      An application could use this API to determin whether or not it is
 *              being debugged, so that it can change behaviour and for example
 *              provide more debug information using functions like OutputDebugString.
 *
 *              You could change the result of this by SET ODIN32.DEBUGGERPRESENT=1 on
 *              the commandline.
 */
BOOL WIN32API IsDebuggerPresent(VOID)
{
    BOOL fRet = getenv("ODIN32.DEBUGGERPRESENT") != NULL;
    dprintf(("KERNEL32:  IsDebuggerPresent() -> %s\n", fRet ? "TRUE" : "FALSE"));
    return fRet;
}


/**
 * Send a string to the debugger for the current application.
 * @param       lpszOutputString    Pointer to the string to send to the debugger. (intput)
 * @sketch      Convert and log the string.
 * @status      STUB
 * @author      Sander van Leeuwen
 * @remark      The string is send to the system debugger if there is no
 *              debugger available for this application.
 *              If the application has not debugger and the system debugger
 *              is not active this API does nothing.
 */
VOID WIN32API OutputDebugStringW(LPCWSTR lpszOutputString)
{
    char    *pszAscii = UnicodeToAsciiString((LPWSTR)lpszOutputString);
    dprintf(("KERNEL32:  OS2OutputDebugStringW %s\n", pszAscii));
    FreeAsciiString(pszAscii);
}


/**
 * Send a string to the debugger for the current application.
 * @param       lpszOutputString    Pointer to the string to send to the debugger. (input)
 * @sketch      Log the string.
 * @status      STUB
 * @author      Sander van Leeuwen
 * @remark      The string is send to the system debugger if there is no
 *              debugger available for this application.
 *              If the application has not debugger and the system debugger
 *              is not active this API does nothing.
 */
VOID WIN32API OutputDebugStringA(LPCSTR lpszOutputString)
{
    dprintf(("KERNEL32:  OutputDebugStringA: %s\n", lpszOutputString));
}

