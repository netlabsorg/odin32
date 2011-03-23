/*
 * Auxiliary thread API
 *
 * Used to execute code on a dedicated thread.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2wrap.h> // Odin32 OS/2 api wrappers

#include <win32api.h>

#include "auxthread.h"

#include "dbglog.h"

struct AUXMSG
{
    PAUXTHREADFN pfn;
    PVOID arg1;
    PVOID arg2;
    PVOID arg3;
    PVOID arg4;
    BOOL del;
};

static HEV hevAux = 0;
static HANDLE hthreadAux = 0;
static HWND hwndAux = NULLHANDLE;

static MRESULT EXPENTRY AuxWndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    if (msg == WM_USER)
    {
        AUXMSG *msg = (AUXMSG *)mp1;
        MRESULT mrc = msg->pfn(msg->arg1, msg->arg2, msg->arg3, msg->arg4);
        if (msg->del)
            delete msg;
        return mrc;
    }

    return FALSE;
}

static DWORD CALLBACK AuxThread(LPVOID arg)
{
    // take hev from the parameter instead of hevAux which will have gone right
    // after calling StopAuxThread() so that we won't be able to post it
    HEV hev = (HEV)arg;

    dprintf(("USER32: AuxThread: Start (hev %x)", hev));

    // Note: Since this thread is started by CreateThread(), hab and hmq are
    // already created

    if (WinRegisterClass(0, "OdinAuxWnd", AuxWndProc, 0, 0))
    {
        hwndAux = WinCreateWindow(HWND_OBJECT, "OdinAuxWnd", NULL,
                                  0, 0, 0, 0, 0, NULL, HWND_BOTTOM, 0,
                                  NULL, NULL);
        if (hwndAux)
        {
            // report becoming ready (non-NULL hwndAux will indicate success)
            DosPostEventSem(hev);

            dprintf(("USER32: AuxThread: Enter message loop"));

            QMSG qmsg;
            while (WinGetMsg(0, &qmsg, NULLHANDLE, 0, 0))
                WinDispatchMsg(0, &qmsg);

            dprintf(("USER32: AuxThread: Exit message loop"));

            WinDestroyWindow(hwndAux);
        }
        else
            dprintf(("USER32: AuxThread: WinCreateWindow() failed with %x",
                     WinGetLastError(0)));

    }
    else
        dprintf(("USER32: AuxThread: WinRegisterClass() failed with %x",
                 WinGetLastError(0)));

    dprintf(("USER32: AuxThread: Stop"));

    // report termination
    DosPostEventSem(hev);

    return 0;
}

static BOOL DoRunOnAuxThread(PAUXTHREADFN pfn, PVOID arg1, PVOID arg2,
                             PVOID arg3, PVOID arg4, PVOID *ret)
{
    APIRET arc;

    if (hevAux == 0)
    {
        HEV hev;
        arc = DosCreateEventSem(NULL, &hev, 0, FALSE);
        if (arc != NO_ERROR)
        {
            dprintf(("USER32: AuxThread: DosCreateEventSem failed with %d",
                     arc));
            return FALSE;
        }

        // protect from two or more concurrent "first" calls
        if (InterlockedCompareExchange((PLONG)&hevAux, hev, 0) == 0)
        {
            // we are the first, do the rest

            // Note: Use CreateThread() instead of DosCreateThread() so that the
            // thread gets all initialization necessary to call Win32 APIs which
            // may be potentially called form PAUXTHREADFN
            hthreadAux = CreateThread(NULL, 0, AuxThread, (LPVOID)hev, 0, NULL);
            if (hthreadAux)
            {
                // wait for the thread to get ready
                arc = DosWaitEventSem(hev, SEM_INDEFINITE_WAIT);
            }
            else
            {
                dprintf(("USER32: AuxThread: CreateThread() failed with %x",
                         GetLastError()));

                // the second caller may be already waiting so inform it
                DosPostEventSem(hev);
            }
        }
        else
        {
            // we are the second, give up our attempt
            DosCloseEventSem(hev);

            // wait for the thread to get ready
            arc = DosWaitEventSem(hevAux, SEM_INDEFINITE_WAIT);
        }

        if (hthreadAux == 0 || arc != NO_ERROR || hwndAux == NULLHANDLE)
        {
            StopAuxThread(); // cleanup
            return FALSE;
        }
    }

    if (ret)
    {
        AUXMSG msg = { pfn, arg1, arg2, arg3, arg4, FALSE };
        *ret = WinSendMsg(hwndAux, WM_USER, &msg, 0);
        return TRUE;
    }

    AUXMSG *msg = new AUXMSG;
    msg->pfn = pfn;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->arg3 = arg3;
    msg->arg4 = arg4;
    msg->del = TRUE;
    if (WinPostMsg(hwndAux, WM_USER, msg, 0))
        return TRUE;

    delete msg;
    return FALSE;
}

/**
 * Schedules execution of a given user function with given arguments on a
 * dedicated global helper thread and immediately returns to the caller.
 *
 * All calls to RunOnAuxThread() schedule execution of the user function on the
 * same thread which, once started, gets terminated automatically at process
 * termination.
 *
 * Note that since this thread is global (shared by all requests as described
 * above), the user function must return as soon as possible as other threads
 * may be trying to schedule a function call on this thread through
 * RunOnAuxThread() (or even waiting for the result of the function execution
 * through RunOnAuxThreadAndWait()) as well.
 *
 * The main purpose of this API is to make sure that a series of function calls
 * is guaranteedly performed on the same thread so that subsequent calls share
 * the context established by the fisrt call. This, in particular includes
 * situations when it is necessary to make sure that an object window is
 * destroyed on the same thead where it was created no matter what thread calls
 * the create or destroy code. This also implies that this dedicated thread runs
 * the message loop and therefore may respond to posted messages.
 *
 * @author dmik (3/23/2011)
 *
 * @param pfn   User function to execute.
 * @param arg1  Function argument 1.
 * @param arg2  Function argument 2.
 * @param arg3  Function argument 3.
 * @param arg4  Function argument 4.
 *
 * @return TRUE on success, FALSE otherwise.
 */
BOOL WIN32API RunOnAuxThread(PAUXTHREADFN pfn, PVOID arg1, PVOID arg2,
                             PVOID arg3, PVOID arg4)
{
    return DoRunOnAuxThread(pfn, arg1, arg2, arg3, arg4, NULL);
}

/**
 * Schedules execution of a given user function with given arguments on a
 * dedicated global helper thread and waits for it to return a value.
 *
 * See RunOnAuxThread() for more details on the dedicated thread and purpose of
 * this API.
 *
 * @author dmik (3/23/2011)
 *
 * @param pfn   User function to execute.
 * @param arg1  Function argument 1.
 * @param arg2  Function argument 2.
 * @param arg3  Function argument 3.
 * @param arg4  Function argument 4.
 * @param ret   Where to store the user function's return value. May be NULL if
 *              the caller is not interested in the return value.
 *
 * @return TRUE on success, FALSE otherwise.
 */
BOOL WIN32API RunOnAuxThreadAndWait(PAUXTHREADFN pfn, PVOID arg1, PVOID arg2,
                                    PVOID arg3, PVOID arg4, PPVOID ret)
{
    PVOID ret2;
    BOOL brc = DoRunOnAuxThread(pfn, arg1, arg2, arg3, arg4, &ret2);
    if (ret)
        *ret = ret2;
    return brc;
}

//******************************************************************************
//
//******************************************************************************
VOID StopAuxThread()
{
    if (hevAux != 0)
    {
        HEV hev = hevAux;

        // protect from two or more concurrent "first" calls
        if (InterlockedCompareExchange((PLONG)&hevAux, 0, hev) == hev)
        {
            // we are the first, do the rest

            // ask to exit the message loop
            if (hwndAux)
            {
                ULONG dummy;
                DosResetEventSem(hev, &dummy);

                WinPostMsg(hwndAux, WM_QUIT, 0, 0);
                hwndAux = NULLHANDLE;
            }

            if (hthreadAux != 0)
            {
                // wait for the normal thread termination
                DosWaitEventSem(hev, 3000);
                CloseHandle(hthreadAux);
                hthreadAux = 0;
            }

            DosCloseEventSem(hev);
        }
    }
}
