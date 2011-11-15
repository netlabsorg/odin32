/* $Id: timer.cpp,v 1.17 2004-03-18 13:18:33 sandervl Exp $ */

/*
 * timer functions for USER32
 *
 * Copyright 1993 Alexandre Julliard
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_WIN
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#include <os2wrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include "win32type.h"
#include <winconst.h>
#include <misc.h>
#include "win32wbase.h"
#include "oslibutil.h"
#include "timer.h"
#include <odincrt.h>

#define DBG_LOCALLOG	DBG_timer
#include "dbglocal.h"

#ifndef OPEN32API
#define OPEN32API _System
#endif

typedef struct tagTIMER
{
    enum {free = 0, UserTimer, SystemTimer} inUse;
    HWND           hwnd;
    UINT           id;
    HWND           PMhwnd;
    ULONG          PMid;
    TIMERPROC      proc;
} TIMER;

#define NB_TIMERS            34
#define NB_RESERVED_TIMERS    2  /* for SetSystemTimer */

#define SYS_TIMER_RATE  54925

static TIMER TimersArray[NB_TIMERS];

static CRITICAL_SECTION_OS2 timercritsect = {0};

inline void EnterCriticalSection (void)
{
   if (DosValidateCriticalSection (&timercritsect) != 0)
      DosInitializeCriticalSection(&timercritsect, NULL);

   DosEnterCriticalSection(&timercritsect);
}

inline void LeaveCriticalSection (void)
{
   DosLeaveCriticalSection(&timercritsect);
}

BOOL TIMER_GetTimerInfo(HWND PMhwnd,ULONG PMid,PBOOL sys,PULONG id, PULONG proc)
{
  int i;
  TIMER *pTimer;

  EnterCriticalSection ();
  for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
      if (pTimer->inUse && (pTimer->PMhwnd == PMhwnd) && (pTimer->PMid == PMid))
          break;
  LeaveCriticalSection();

  if (i == NB_TIMERS)  /* no matching timer found */
      return (FALSE);  /* forward message */

  *sys = pTimer->inUse == TIMER::SystemTimer;
  *id  = pTimer->id;
  *proc = (ULONG)pTimer->proc;

  return TRUE;
}

/***********************************************************************
 *           TIMER_IsTimerValid
 */
BOOL TIMER_IsTimerValid( HWND hwnd, UINT id, ULONG proc )
{
    int i;
    TIMER *pTimer;
    BOOL ret = FALSE;

    EnterCriticalSection ();

    for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
        if ((pTimer->hwnd == hwnd) && (pTimer->id == id) && (pTimer->proc == (TIMERPROC)proc))
        {
            ret = TRUE;
            break;
        }

    LeaveCriticalSection();
    return ret;
}


BOOL TIMER_HandleTimer (PQMSG pMsg)
{
    int i;
    TIMER *pTimer;
    HWND  PMhwnd = pMsg->hwnd;
    ULONG PMid   = (ULONG)(pMsg->mp1);

    EnterCriticalSection ();
    for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
        if (pTimer->inUse && (pTimer->PMhwnd == PMhwnd) && (pTimer->PMid == PMid))
            break;
    LeaveCriticalSection();

    if (i == NB_TIMERS)  /* no matching timer found */
        return (FALSE);  /* forward message */

    pMsg->mp2 = MPFROMLONG (TRUE);   /* mark for Win32 */
    if (!pTimer->proc)
        return (FALSE);  /* forward message */

    if (!WinInSendMsg (GetThreadHAB())) {
	dprintf2(("TIMER_HandleTimer %x %x %x", pTimer->hwnd, pTimer->id, pMsg->time));
        pTimer->proc (pTimer->hwnd, (pTimer->inUse == TIMER::SystemTimer) ? WM_SYSTIMER_W:WM_TIMER_W, pTimer->id, pMsg->time);
    }
    return (TRUE);
}

static UINT TIMER_SetTimer (HWND hwnd, UINT id, UINT timeout, TIMERPROC proc, BOOL sys)
{
    int i;
    TIMER *pTimer;
    HWND hwndOS2;
    Win32BaseWindow *wnd = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if (hwnd && !wnd) {
        dprintf(("TIMER_SetTimer invalid window handle %x", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
        return 0;
    }

    hwndOS2 = hwnd ? wnd->getOS2WindowHandle() : 0;
    if(wnd) RELEASE_WNDOBJ(wnd);
    wnd = NULL;

    EnterCriticalSection ();

    /* Check if there's already a timer with the same hwnd and id */

    for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
        if (pTimer->inUse && (pTimer->hwnd == hwnd) && (pTimer->id == id) && ((sys && pTimer->inUse == TIMER::SystemTimer) || !sys))
            break;

    if (i == NB_TIMERS)  /* no matching timer found */
    {
        /* Find a free timer */

        for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
            if (!pTimer->inUse) break;

        if ((i >= NB_TIMERS) ||
            (!sys && (i >= NB_TIMERS-NB_RESERVED_TIMERS)))
        {
            LeaveCriticalSection();
            return 0;
        }

        if (!hwnd) id = i + 1;

        /* Add the timer */

        pTimer->inUse   = sys ? TIMER::SystemTimer : TIMER::UserTimer;
        pTimer->hwnd    = hwnd;
        pTimer->id      = id;
        pTimer->proc    = proc;
        pTimer->PMhwnd  = hwnd ? hwndOS2 : NULLHANDLE;
        pTimer->PMid    = WinStartTimer (GetThreadHAB(), pTimer->PMhwnd,
                                         i + 1, timeout);

        if (!pTimer->PMid) id = pTimer->id = 0;
    } else {
        WinStartTimer (GetThreadHAB(), pTimer->PMhwnd, pTimer->PMid, timeout);
    }

    LeaveCriticalSection();

    if (!id) return TRUE;
    else return id;
}

static BOOL TIMER_KillTimer (HWND hwnd, UINT id, BOOL sys)
{
    int i;
    TIMER * pTimer;

    EnterCriticalSection();

    /* Find the timer */

    for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
        if (pTimer->inUse &&
            (pTimer->hwnd == hwnd) && (pTimer->id == id) && ((sys && pTimer->inUse == TIMER::SystemTimer) || !sys)) break;

    if ((i >= NB_TIMERS) ||
        (!sys && (i >= NB_TIMERS-NB_RESERVED_TIMERS)) ||
        (!sys && (pTimer->inUse != TIMER::UserTimer)) ||
        (sys && (pTimer->inUse != TIMER::SystemTimer)) )
    {
        LeaveCriticalSection();
        return FALSE;
    }

    /* Delete the timer */

    WinStopTimer (GetThreadHAB(), pTimer->PMhwnd, pTimer->PMid);

    pTimer->inUse   = TIMER::free;
    pTimer->PMhwnd  = 0;
    pTimer->PMid    = 0;

    LeaveCriticalSection();

    return TRUE;
}

VOID TIMER_KillTimerFromWindow(HWND hwnd)
{
    int i;
    TIMER * pTimer;

    if (!IsWindow(hwnd)) return;

    EnterCriticalSection();

    for (i = 0, pTimer = TimersArray; i < NB_TIMERS; i++, pTimer++)
      if (pTimer->inUse && pTimer->hwnd == hwnd)
      {
        pTimer->inUse   = TIMER::free;
        pTimer->PMhwnd  = 0;
        pTimer->PMid    = 0;
      }

    LeaveCriticalSection();
}

extern "C" {

/***********************************************************************
 *           SetTimer32   (USER32.511)
 */
UINT WIN32API SetTimer (HWND hwnd, UINT id, UINT timeout, TIMERPROC proc)
{
    UINT rc;

    dprintf(("USER32: SetTimer %x %d %d %08lx", hwnd, id, timeout, (LONG)proc));

    rc = TIMER_SetTimer (hwnd, id, timeout, proc, FALSE);
    return (rc);
}

/***********************************************************************
 *           SetSystemTimer32   (USER32.509)
 */
UINT WIN32API SetSystemTimer (HWND hwnd, UINT id, UINT timeout, TIMERPROC proc)
{
    UINT rc;

    dprintf(("USER32: SetSystemTimer %04x %d %d %08lx", hwnd, id, timeout, (LONG)proc));

    rc = TIMER_SetTimer (hwnd, id, timeout, proc, TRUE);
    return (rc);
}

/***********************************************************************
 *           KillTimer32   (USER32.354)
 */
BOOL WIN32API KillTimer (HWND hwnd, UINT id)
{
    BOOL rc;

    dprintf(("USER32: KillTimer %x %d", hwnd, id));

    rc = TIMER_KillTimer (hwnd, id, FALSE);
    return (rc);
}

/***********************************************************************
 *           KillSystemTimer32   (USER32.353)
 */
BOOL WIN32API KillSystemTimer (HWND hwnd, UINT id)
{
    BOOL rc;

    dprintf(("USER32: KillSystemTimer %x %d", hwnd, id));

    rc = TIMER_KillTimer (hwnd, id, TRUE);
    return (rc);
}

} // extern "C"
