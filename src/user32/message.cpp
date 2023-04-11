/* $Id: message.cpp,v 1.6 2003-08-22 13:16:44 sandervl Exp $ */
/*
 * Win32 window message APIs for OS/2
 *
 * Copyright 1999-2002 Sander van Leeuwen
 *
 * Based on Wine Windows code (dlls\user\message.c) 20020122
 *
 * Window messaging support
 *
 * Copyright 2001 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <misc.h>
#include "win32wbase.h"
#include <win.h>
#include <heapstring.h>
#include <handlemanager.h>
#include "oslibutil.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "hook.h"
#include "wndmsg.h"

#define DBG_LOCALLOG	DBG_message
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-MESSAGE)

#define WIN_GetFullHandle(a)	a

#define is_broadcast(hwnd) 	(hwnd == HWND_BROADCAST || hwnd == HWND_TOPMOST)


enum message_type
{
    MSG_ASCII,
    MSG_UNICODE,
    MSG_NOTIFY,
    MSG_CALLBACK_ASCII,
    MSG_CALLBACK_UNICODE,
    MSG_OTHER_PROCESS,
    MSG_POSTED_ASCII,
    MSG_POSTED_UNICODE
};

/* structure to group all parameters for sent messages of the various kinds */
struct send_message_info
{
    enum message_type type;
    HWND              hwnd;
    UINT              msg;
    WPARAM            wparam;
    LPARAM            lparam;
    UINT              flags;      /* flags for SendMessageTimeout */
    UINT              timeout;    /* timeout for SendMessageTimeout */
    SENDASYNCPROC     callback;   /* callback function for SendMessageCallback */
    ULONG_PTR         data;       /* callback data */
};

/***********************************************************************
 *		broadcast_message_callback
 *
 * Helper callback for broadcasting messages.
 */
static BOOL CALLBACK broadcast_message_callback( HWND hwnd, LPARAM lparam )
{
    struct send_message_info *info = (struct send_message_info *)lparam;
    if (!(GetWindowLongW( hwnd, GWL_STYLE ) & (WS_POPUP|WS_CAPTION))) return TRUE;
    switch(info->type)
    {
    case MSG_UNICODE:
        SendMessageTimeoutW( hwnd, info->msg, info->wparam, info->lparam,
                             info->flags, info->timeout, NULL );
        break;
    case MSG_ASCII:
        SendMessageTimeoutA( hwnd, info->msg, info->wparam, info->lparam,
                             info->flags, info->timeout, NULL );
        break;
    case MSG_NOTIFY:
        SendNotifyMessageW( hwnd, info->msg, info->wparam, info->lparam );
        break;
    case MSG_CALLBACK_ASCII:
        SendMessageCallbackA( hwnd, info->msg, info->wparam, info->lparam,
                              info->callback, info->data );
        break;
    case MSG_CALLBACK_UNICODE:
        SendMessageCallbackW( hwnd, info->msg, info->wparam, info->lparam,
                              info->callback, info->data );
        break;
    case MSG_POSTED_ASCII:
        PostMessageA( hwnd, info->msg, info->wparam, info->lparam );
        break;
    case MSG_POSTED_UNICODE:
        PostMessageW( hwnd, info->msg, info->wparam, info->lparam );
        break;
    default:
        dprintf(("!ERROR!: broadcast_message_callback: bad type %d\n", info->type ));
        DebugInt3();
        break;
    }
    return TRUE;
}
/***********************************************************************
 *           call_window_proc
 *
 * Call a window procedure and the corresponding hooks.
 */
static LRESULT call_window_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, BOOL unicode )
{
    LRESULT result;
    WNDPROC winproc;

    /* first the WH_CALLWNDPROC hook */
    if (HOOK_IsHooked( WH_CALLWNDPROC ))
    {
        CWPSTRUCT cwp;
        cwp.lParam  = lparam;
        cwp.wParam  = wparam;
        cwp.message = msg;
        cwp.hwnd    = WIN_GetFullHandle( hwnd );
        if (unicode) HOOK_CallHooksW( WH_CALLWNDPROC, HC_ACTION, 1, (LPARAM)&cwp );
        else HOOK_CallHooksA( WH_CALLWNDPROC, HC_ACTION, 1, (LPARAM)&cwp );
        lparam = cwp.lParam;
        wparam = cwp.wParam;
        msg    = cwp.message;
        hwnd   = cwp.hwnd;
    }

    /* now call the window procedure */
    if (unicode)
    {
        if (!(winproc = (WNDPROC)GetWindowLongW( hwnd, GWL_WNDPROC ))) return 0;
        result = CallWindowProcW( winproc, hwnd, msg, wparam, lparam );
    }
    else
    {
        if (!(winproc = (WNDPROC)GetWindowLongA( hwnd, GWL_WNDPROC ))) return 0;
        result = CallWindowProcA( winproc, hwnd, msg, wparam, lparam );
    }
#ifdef __WIN32OS2__
    //hittest caching
    if(msg == WM_NCHITTEST) 
    {
        Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);
        if(window) {
             window->setLastHitTestVal(result);
             RELEASE_WNDOBJ(window);
        }
        else DebugInt3();
    }
#endif

    /* and finally the WH_CALLWNDPROCRET hook */
    if (HOOK_IsHooked( WH_CALLWNDPROCRET ))
    {
        CWPRETSTRUCT cwp;
        cwp.lResult = result;
        cwp.lParam  = lparam;
        cwp.wParam  = wparam;
        cwp.message = msg;
        cwp.hwnd    = WIN_GetFullHandle( hwnd );
        if (unicode) HOOK_CallHooksW( WH_CALLWNDPROCRET, HC_ACTION, 1, (LPARAM)&cwp );
        else HOOK_CallHooksA( WH_CALLWNDPROCRET, HC_ACTION, 1, (LPARAM)&cwp );
    }
    return result;
}
/***********************************************************************
 *		SendMessageTimeoutW  (USER32.@)
 */
LRESULT WINAPI SendMessageTimeoutW( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                    UINT flags, UINT timeout, LPDWORD res_ptr )
{
    struct send_message_info info;
    DWORD dest_tid, dest_pid;
    LRESULT ret, result;

    info.type    = MSG_UNICODE;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = flags;
    info.timeout = timeout;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        if (res_ptr) *res_ptr = 1;
        return 1;
    }

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, &dest_pid ))) return 0;

    if (dest_tid == GetCurrentThreadId())
    {
        result = call_window_proc( hwnd, msg, wparam, lparam, TRUE );
        ret = 1;
    }
    else
    {
        //otherwise use WinSendMsg to send it to the right process/thread
        dprintf(("SendMessages (inter-process/thread) %x %x %x %x", Win32ToOS2Handle(hwnd), msg, wparam, lparam));
        result = OSLibSendMessage(hwnd, Win32ToOS2Handle(hwnd), msg, wparam, lparam, TRUE);
        ret = 1;
    }

    if (ret && res_ptr) *res_ptr = result;
    return ret;
}


/***********************************************************************
 *		SendMessageTimeoutA  (USER32.@)
 */
LRESULT WINAPI SendMessageTimeoutA( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                    UINT flags, UINT timeout, LPDWORD res_ptr )
{
    struct send_message_info info;
    DWORD dest_tid, dest_pid;
    LRESULT ret, result;

    info.type    = MSG_ASCII;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = flags;
    info.timeout = timeout;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        if (res_ptr) *res_ptr = 1;
        return 1;
    }

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, &dest_pid ))) return 0;

    if (dest_tid == GetCurrentThreadId())
    {
        result = call_window_proc( hwnd, msg, wparam, lparam, FALSE );
        ret = 1;
    }
    else
    {
        //otherwise use WinSendMsg to send it to the right process/thread
        dprintf(("SendMessages (inter-process/thread) %x %x %x %x", Win32ToOS2Handle(hwnd), msg, wparam, lparam));
        result = OSLibSendMessage(hwnd, Win32ToOS2Handle(hwnd), msg, wparam, lparam, FALSE);
        ret = 1;
    }
    if (ret && res_ptr) *res_ptr = result;
    return ret;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    DebugPrintMessage(hwnd, msg, wParam, lParam, FALSE, FALSE);
    SendMessageTimeoutA(hwnd, msg, wParam, lParam, SMTO_NORMAL, INFINITE, (LPDWORD)&res);
    return res;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    DebugPrintMessage(hwnd, msg, wParam, lParam, TRUE, FALSE);
    SendMessageTimeoutW(hwnd, msg, wParam, lParam, SMTO_NORMAL, INFINITE, (LPDWORD)&res);
    return res;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;
  HWND hwndOS2;

    struct send_message_info info;
    DWORD dest_tid;

#if 0
    if (is_pointer_message( msg ))
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
#endif

    info.type   = MSG_POSTED_ASCII;
    info.hwnd   = hwnd;
    info.msg    = msg;
    info.wparam = wParam;
    info.lparam = lParam;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        return TRUE;
    }

    if(hwnd == (HWND) NULL)
        return PostThreadMessageA(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("PostMessageA, window %x not found", hwnd));
        return FALSE;
    }
    hwndOS2 = window->getOS2WindowHandle();
    RELEASE_WNDOBJ(window);
    DebugPrintMessage(hwnd, msg, wParam, lParam, FALSE, TRUE);
    return OSLibPostMessage(hwnd, hwndOS2, msg, wParam, lParam, FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;
  HWND hwndOS2;

    struct send_message_info info;
    DWORD dest_tid;

    info.type   = MSG_POSTED_UNICODE;
    info.hwnd   = hwnd;
    info.msg    = msg;
    info.wparam = wParam;
    info.lparam = lParam;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        return TRUE;
    }

    if(hwnd == (HWND) NULL)
        return PostThreadMessageW(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("PostMessageW, window %x not found", hwnd));
        return FALSE;
    }
    hwndOS2 = window->getOS2WindowHandle();
    RELEASE_WNDOBJ(window);
    DebugPrintMessage(hwnd, msg, wParam, lParam, TRUE, TRUE);
    return OSLibPostMessage(hwnd, hwndOS2, msg, wParam, lParam, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageA( DWORD threadid, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return OSLibPostThreadMessage(threadid, msg, wParam, lParam, FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageW( DWORD threadid, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return OSLibPostThreadMessage(threadid, msg, wParam, lParam, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    DWORD dest_tid;

    dprintf(("USER32: SendNotifyMessageA %x %x %x %x", hwnd, Msg, wParam, lParam));

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, NULL ))) return FALSE;

    if (dest_tid == GetCurrentThreadId())
    {
        SendMessageA( hwnd, Msg, wParam, lParam);
        return TRUE;
    }
    //Must not block when window thread != current thread
    return PostMessageA(hwnd, Msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    DWORD dest_tid;

    dprintf(("USER32: SendNotifyMessageW %x %x %x %x", hwnd, Msg, wParam, lParam));

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, NULL ))) return FALSE;

    if (dest_tid == GetCurrentThreadId())
    {
        SendMessageW( hwnd, Msg, wParam, lParam);
        return TRUE;
    }
    //Must not block when window thread != current thread
    return PostMessageW(hwnd, Msg, wParam, lParam);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SendMessageCallbackA
 * Purpose   : The SendMessageCallback function sends the specified message to
 *             a window or windows. The function calls the window procedure for
 *             the specified window and returns immediately. After the window
 *             procedure processes the message, the system calls the specified
 *             callback function, passing the result of the message processing
 *             and an application-defined value to the callback function.
 * Parameters: HWND  hwnd                      handle of destination window
 *             UINT  uMsg                      message to send
 *             WPARAM  wParam                  first message parameter
 *             LPARAM  lParam                  second message parameter
 *             SENDASYNCPROC  lpResultCallBack function to receive message value
 *             DWORD  dwData                   value to pass to callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : 
 *
 * Author    : Wine
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackA(HWND          hwnd,
                                   UINT          uMsg,
                                   WPARAM        wParam,
                                   LPARAM        lParam,
                                   SENDASYNCPROC lpResultCallBack,
                                   DWORD         dwData)
{
    struct send_message_info info;
    LRESULT result;
    DWORD dest_tid;

#if 0
    if (is_pointer_message(msg))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

    dprintf(("USER32:SendMessageCallBackW (%08xh,%08xh,%08xh,%08xh,%08xh,%08x)",
             hwnd, uMsg, wParam, lParam, lpResultCallBack, dwData));

    info.type     = MSG_CALLBACK_ASCII;
    info.hwnd     = hwnd;
    info.msg      = uMsg;
    info.wparam   = wParam;
    info.lparam   = lParam;
    info.callback = lpResultCallBack;
    info.data     = dwData;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        return TRUE;
    }

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, NULL ))) return FALSE;

    if (dest_tid == GetCurrentThreadId())
    {
        result = call_window_proc( hwnd, uMsg, wParam, lParam, FALSE );
        lpResultCallBack( hwnd, uMsg, dwData, result );
        return TRUE;
    }
    else
    {
        dprintf(("!WARNING!: callback will not be called" ));

        //otherwise use PostMessage to send it to the right process/thread
        dprintf(("SendMessageCallbackA (inter-process/thread) %x %x %x %x", hwnd, uMsg, wParam, lParam));
        PostMessageA(hwnd, uMsg, wParam, lParam);
        return TRUE;
    }
}


/*****************************************************************************
 * Name      : BOOL WIN32API SendMessageCallbackW
 * Purpose   : The SendMessageCallback function sends the specified message to
 *             a window or windows. The function calls the window procedure for
 *             the specified window and returns immediately. After the window
 *             procedure processes the message, the system calls the specified
 *             callback function, passing the result of the message processing
 *             and an application-defined value to the callback function.
 * Parameters: HWND  hwnd                      handle of destination window
 *             UINT  uMsg                      message to send
 *             WPARAM  wParam                  first message parameter
 *             LPARAM  lParam                  second message parameter
 *             SENDASYNCPROC  lpResultCallBack function to receive message value
 *             DWORD  dwData                   value to pass to callback function
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, call GetLastError.
 * Remark    :
 * Status    : 
 *
 * Author    : Wine
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackW(HWND          hwnd,
                                   UINT          uMsg,
                                   WPARAM        wParam,
                                   LPARAM        lParam,
                                   SENDASYNCPROC lpResultCallBack,
                                   DWORD         dwData)
{
    struct send_message_info info;
    LRESULT result;
    DWORD dest_tid;

#if 0
    if (is_pointer_message(msg))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif

    dprintf(("USER32:SendMessageCallBackW (%08xh,%08xh,%08xh,%08xh,%08xh,%08x)",
             hwnd, uMsg, wParam, lParam, lpResultCallBack, dwData));

    info.type     = MSG_CALLBACK_UNICODE;
    info.hwnd     = hwnd;
    info.msg      = uMsg;
    info.wparam   = wParam;
    info.lparam   = lParam;
    info.callback = lpResultCallBack;
    info.data     = dwData;

    if (is_broadcast(hwnd))
    {
        EnumWindows( broadcast_message_callback, (LPARAM)&info );
        return TRUE;
    }

    if (!(dest_tid = GetWindowThreadProcessId( hwnd, NULL ))) return FALSE;

    if (dest_tid == GetCurrentThreadId())
    {
        result = call_window_proc( hwnd, uMsg, wParam, lParam, TRUE );
        lpResultCallBack( hwnd, uMsg, dwData, result );
        return TRUE;
    }
    else
    {
        dprintf(("!WARNING!: callback will not be called" ));

        //otherwise use PostMessage to send it to the right process/thread
        dprintf(("SendMessageCallbackW (inter-process/thread) %x %x %x %x", hwnd, uMsg, wParam, lParam));
        PostMessageW(hwnd, uMsg, wParam, lParam);
        return TRUE;
    }
}
/*****************************************************************************
 * Name      : long WIN32API BroadcastSystemMessageA
 * Purpose   : The BroadcastSystemMessage function sends a message to the given
 *             recipients. The recipients can be applications, installable
 *             drivers, Windows-based network drivers, system-level device
 *             drivers, or any combination of these system components.
 * Parameters: DWORD   dwFlags,
               LPDWORD lpdwRecipients,
               UINT    uiMessage,
               WPARAM  wParam,
               LPARAM  lParam
 * Variables :
 * Result    : If the function succeeds, the return value is a positive value.
 *             If the function is unable to broadcast the message, the return value is -1.
 *             If the dwFlags parameter is BSF_QUERY and at least one recipient returned FALSE to the corresponding message, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

long WIN32API BroadcastSystemMessageA(DWORD   dwFlags,
                                     LPDWORD lpdwRecipients,
                                     UINT    uiMessage,
                                     WPARAM  wParam,
                                     LPARAM  lParam)
{
  dprintf(("USER32:BroadcastSystemMessage(%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
        dwFlags,
        lpdwRecipients,
        uiMessage,
        wParam,
        lParam));

  return (-1);
}
/*********************************************************************** 
*              BroadcastSystemMessageW (USER32.@) 
*/ 
long WIN32API BroadcastSystemMessageW(DWORD   dwFlags, 
                                     LPDWORD lpdwRecipients, 
                                     UINT    uiMessage, 
                                     WPARAM  wParam, 
                                     LPARAM  lParam) 
 { 
  dprintf(("USER32:BroadcastSystemMessage(%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n", 
        dwFlags, 
        lpdwRecipients, 
        uiMessage, 
        wParam, 
        lParam)); 
 
  return (-1); 
} 
#if 0
//******************************************************************************
//******************************************************************************
VOID WIN32API PostQuitMessage( int nExitCode)
{
    dprintf(("USER32: PostQuitMessage %d", nExitCode));
    PostThreadMessageW(GetCurrentThreadId(), WM_QUIT, nExitCode, 0);
}
//******************************************************************************
//******************************************************************************
#endif