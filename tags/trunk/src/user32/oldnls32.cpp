/* $Id: oldnls32.cpp,v 1.2 2001-09-05 13:53:50 bird Exp $ */
/*
    winnls32.cpp

*/

#include <odin.h>
#include <os2win.h>
#include <odinwrap.h>
#include "oldnls32.h"


/*
    BOOL IMPGetIME(hwnd, lpimepro)

    Get Current IME Information
    (HWND, InstallDate, Version, Description, FileName, Options on start...)

    to invoke:
        hnwd ... NULL
    result:
    not 0 ... success (infomation will be stored to lpimepro)
    0 ... failure

    see also:
        ImmGetDefaultIMEWnd
        ImmGetDescription
        ImmGetIMEFileName
        ImmGetProperty
*/

BOOL WINAPI IMPGetIMEA(HWND hwnd, LPIMEPROA lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPGetIMEA not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI IMPGetIMEW(HWND hwnd, LPIMEPROW lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPGetIMEW not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}


/*
    BOOL IMPQueryIME(lpimepro)

    Get(Enum) IMEs Information
    (HWND, InstallDate, Version, Description, FileName, Options on start...)

    to invoke:
    first : lpimepro->szName[0] = 0
    next to last : (must not modify any contents in lpimepro)

    result:
    not 0 ... success
        lpimepro->hWnd ... HANDLE to the IME window (if loaded)
                           or NULL(if not)
    0 ... failure, or end of list

    see also:
        ImmGetDefaultIMEWnd
        ImmGetDescription
        ImmGetIMEFileName
        ImmGetProperty
*/

BOOL WINAPI IMPQueryIMEA(LPIMEPROA lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPQueryIMEA not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI IMPQueryIMEW(LPIMEPROW lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPQueryIMEW not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}


/*
    BOOL IMPSetIME(hwnd, lpimepro)

    Switch active IME

    to invoke:
        hwnd = NULL
        lpimepro = IME to switch
    result:
    not 0 ... success
    0 ... failure

    see also:
        ImmAssociateContext
*/

BOOL WINAPI IMPSetIMEA(HWND hwnd, LPIMEPROA lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPSetIMEA not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

BOOL WINAPI IMPSetIMEW(HWND hwnd, LPIMEPROW lpip)
{
#ifdef DEBUG
    dprintf(("USER32: IMPSetIMEW not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}


/*
    UINT WINNLSGetIMEHotkey(hwnd)

    Query Virtual Key code to open the IME

    to invoke:
        hnwd ... HANDLE to the IME window
    result:
        Virtual Key code, or NULL

    see also:
        ImmSimulateHotKey
*/

UINT WINAPI WINNLSGetIMEHotkey(HWND hwnd)
{
#ifdef DEBUG
    dprintf(("USER32: WINNLSGetIMEHotKey not implemented\n"));
#endif
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return (UINT)NULL;
}


/*
    BOOL WINNLSEnableIME(hwnd, bool)

    make IME open/close

    to invoke:
        hwnd ... NULL
        bool ... TRUE (open) or FALSE (close)
    result:
        previous state

    see also:
        ImmGetOpenStatus
        ImmSetOpenStatus
        ImmSimulateHotKey
*/

BOOL WINAPI WINNLSEnableIME(HWND hwnd, BOOL bOpen)
{
#ifdef DEBUG
    dprintf(("USER32: WINNLSEnableIME not implemented\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
#endif
    return FALSE;
}


/*
    BOOL WINNLSGetEnableStatus(hwnd)

    Query IME open/close

    to invoke:
        hwnd ... NULL
    result:
        not 0 ... open
        0 ... close

    see also:
        ImmGetOpenStatus
*/

BOOL WINAPI WINNLSGetEnableStatus(HWND hwnd)
{
#ifdef DEBUG
    dprintf(("USER32: WINNLSGetEnableStatus not implemented\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
#endif
    return FALSE;
}


/*
    LRESULT WINAPI SendIMEMessageEx(hwnd, lparam)
*/


LRESULT WINAPI SendIMEMessageExA(HWND hwnd, LPARAM lparam)
{
#ifdef DEBUG
    dprintf(("USER32: SendIMEMessageExA not implemented\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
#endif
    return (LRESULT)0;
}

LRESULT WINAPI SendIMEMessageExW(HWND hwnd, LPARAM lparam)
{
#ifdef DEBUG
    dprintf(("USER32: SendIMEMessageExW not implemented\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
#endif
    return (LRESULT)0;
}

