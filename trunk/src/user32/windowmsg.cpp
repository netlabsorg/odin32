/* $Id: windowmsg.cpp,v 1.23 2001-03-25 08:50:42 sandervl Exp $ */
/*
 * Win32 window message APIs for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 * Parts based on Wine Windows code (windows\message.c) 990508
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * TODO: GetQueueStatus: QS_HOTKEY (oslibmsg.cpp) & low word bits
 * TODO: MsgWaitForMultipleObjects: timeout isn't handled correctly (can return too late)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>
#include <win.h>
#include <heapstring.h>
#include <handlemanager.h>
#include "oslibutil.h"
#include "oslibwin.h"
#include "oslibmsg.h"

#define DBG_LOCALLOG	DBG_windowmsg
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
VOID WIN32API PostQuitMessage( int nExitCode)
{
    dprintf(("USER32:  PostQuitMessage\n"));
    OSLibWinPostQuitMessage(nExitCode);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageA(const MSG * msg)
{
  return OSLibWinDispatchMsg((MSG *)msg);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageW( const MSG * msg)
{
  return OSLibWinDispatchMsg((MSG *)msg, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMessage( const MSG * msg)
{
   return OSLibWinTranslateMessage((MSG *)msg);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageA( LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax)
{
    return OSLibWinGetMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageW( LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax)
{
    return OSLibWinGetMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageA(LPMSG msg, HWND hwndOwner, UINT uMsgFilterMin,
                           UINT uMsgFilterMax, UINT fuRemoveMsg)
{
 BOOL fFoundMsg;

    fFoundMsg = OSLibWinPeekMsg(msg, hwndOwner, uMsgFilterMin, uMsgFilterMax,
                                fuRemoveMsg, FALSE);
    if(fFoundMsg) {
        if (msg->message == WM_QUIT && (fuRemoveMsg & PM_REMOVE)) {
            //TODO: Post WM_QUERYENDSESSION message when WM_QUIT received and system is shutting down
        }
    }
    return fFoundMsg;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageW(LPMSG msg, HWND hwndOwner, UINT uMsgFilterMin,
                           UINT uMsgFilterMax, UINT fuRemoveMsg)
{
 BOOL fFoundMsg;

    fFoundMsg = OSLibWinPeekMsg(msg, hwndOwner, uMsgFilterMin, uMsgFilterMax,
                                fuRemoveMsg, FALSE);
    if(fFoundMsg) {
        if (msg->message == WM_QUIT && (fuRemoveMsg & (PM_REMOVE))) {
            //TODO: Post WM_QUERYENDSESSION message when WM_QUIT received and system is shutting down
        }
    }
    return fFoundMsg;
}
//******************************************************************************
//TODO:
//******************************************************************************
LONG WIN32API GetMessageExtraInfo()
{
    dprintf(("USER32: GetMessageExtraInfo"));
    return GetThreadMessageExtraInfo();
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMessagePos(void)
{
    dprintf(("USER32: GetMessagePos"));
    return OSLibWinGetMessagePos();
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetMessageTime(void)
{
    dprintf(("USER32: GetMessageTime"));
    return OSLibWinGetMessageTime();
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    if (hwnd == HWND_BROADCAST|| hwnd == HWND_TOPMOST)
    {
        Win32BaseWindow::BroadcastMessageA(BROADCAST_SEND, msg, wParam, lParam);
        return TRUE;
    }

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SendMessageA, %x %x %x window %x not found", msg, wParam, lParam, hwnd));
        return 0;
    }
    return window->SendMessageA(msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    if (hwnd == HWND_BROADCAST|| hwnd == HWND_TOPMOST)
    {
        Win32BaseWindow::BroadcastMessageW(BROADCAST_SEND, msg, wParam, lParam);
        return TRUE;
    }

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SendMessageW, window %x not found", hwnd));
        return 0;
    }
    return window->SendMessageW(msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    if (hwnd == HWND_BROADCAST) //Not HWND_TOPMOST???
    {
        Win32BaseWindow::BroadcastMessageA(BROADCAST_POST, msg, wParam, lParam);
        return TRUE;
    }

    if(hwnd == NULL)
        return PostThreadMessageA(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("PostMessageA, window %x not found", hwnd));
        return 0;
    }
    dprintf(("PostMessageA, %x %x %x %x", hwnd, msg, wParam, lParam));
    return OSLibPostMessage(window->getOS2WindowHandle(), msg, wParam, lParam, FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32BaseWindow *window;

    if (hwnd == HWND_BROADCAST) //Not HWND_TOPMOST???
    {
        Win32BaseWindow::BroadcastMessageW(BROADCAST_POST, msg, wParam, lParam);
        return TRUE;
    }

    if(hwnd == NULL)
        return PostThreadMessageW(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("PostMessageW, window %x not found", hwnd));
        return 0;
    }
    dprintf(("PostMessageW, %x %x %x %x", hwnd, msg, wParam, lParam));
    return OSLibPostMessage(window->getOS2WindowHandle(), msg, wParam, lParam, TRUE);
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
BOOL WIN32API WaitMessage(void)
{
    dprintf2(("USER32: WaitMessage"));
    return OSLibWinWaitMessage();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InSendMessage(void)
{
    dprintf(("USER32:  InSendMessage"));
    return OSLibWinInSendMessage();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReplyMessage(LRESULT result)
{
    dprintf(("USER32: ReplyMessage %x", result));
    return OSLibWinReplyMessage(result);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterWindowMessageA(LPCSTR lpString)
{
 UINT rc;

    rc = GlobalAddAtomA(lpString);
    dprintf(("USER32:  RegisterWindowMessageA %s returned %X\n", lpString, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterWindowMessageW( LPCWSTR lpString)
{
    dprintf(("USER32:  RegisterWindowMessageW\n"));
    return GlobalAddAtomW(lpString);
}
//******************************************************************************
//No need to support this (obsolete, not implemented by Win32)
//******************************************************************************
BOOL WIN32API SetMessageQueue(int cMessagesMax)
{
  dprintf(("USER32:  SetMessageQueue\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageTimeoutA(HWND hwnd, UINT Msg, WPARAM wParam,
                    LPARAM lParam, UINT fuFlags, UINT uTimeOut,
                    LPDWORD lpdwResult)
{
  dprintf(("USER32:  SendMessageTimeoutA, partially implemented\n"));
  //ignore fuFlags & wTimeOut
  *lpdwResult = SendMessageA(hwnd, Msg, wParam, lParam);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageTimeoutW(HWND hwnd, UINT Msg, WPARAM wParam,
                    LPARAM lParam, UINT fuFlags, UINT uTimeOut,
                    LPDWORD lpdwResult)
{
  dprintf(("USER32:  SendMessageTimeoutW, partially implemented\n"));
  return(SendMessageTimeoutA(hwnd, Msg, wParam, lParam, fuFlags, uTimeOut, lpdwResult));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  dprintf(("USER32:  SendNotifyMessageA, not completely implemented\n"));
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  dprintf(("USER32:  SendNotifyMessageW, not completely implemented\n"));
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//******************************************************************************
LPARAM WIN32API SetMessageExtraInfo(LPARAM lParam)
{
  dprintf(("USER32:  SetMessageExtraInfo\n"));
  return SetThreadMessageExtraInfo(lParam);
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
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackA(HWND          hWnd,
                                   UINT          uMsg,
                                   WPARAM        wParam,
                                   LPARAM        lParam,
                                   SENDASYNCPROC lpResultCallBack,
                                   DWORD         dwData)
{
  dprintf(("USER32:SendMessageCallBackA (%08xh,%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         uMsg,
         wParam,
         lParam,
         lpResultCallBack,
         dwData));

  return (FALSE);
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
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API SendMessageCallbackW(HWND          hWnd,
                                      UINT          uMsg,
                                      WPARAM        wParam,
                                      LPARAM        lParam,
                                      SENDASYNCPROC lpResultCallBack,
                                      DWORD         dwData)
{
  dprintf(("USER32:SendMessageCallBackW (%08xh,%08xh,%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         uMsg,
         wParam,
         lParam,
         lpResultCallBack,
         dwData));

  return (FALSE);
}
/*****************************************************************************
 * Name      : long WIN32API BroadcastSystemMessage
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

long WIN32API BroadcastSystemMessage(DWORD   dwFlags,
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
//******************************************************************************
//******************************************************************************
/**********************************************************************
 *           WINPROC_TestCBForStr
 *
 * Return TRUE if the lparam is a string
 */
BOOL WINPROC_TestCBForStr ( HWND hwnd )
{
    BOOL retvalue;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    retvalue = ( !(LOWORD(dwStyle) & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) ||
              (LOWORD(dwStyle) & CBS_HASSTRINGS) );
    return retvalue;
}
/**********************************************************************
 *           WINPROC_TestLBForStr
 *
 * Return TRUE if the lparam is a string
 */
BOOL WINPROC_TestLBForStr ( HWND hwnd )
{
    BOOL retvalue;
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
    retvalue = ( !(LOWORD(dwStyle) & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) ||
            (LOWORD(dwStyle) & LBS_HASSTRINGS) );
    return retvalue;
}

/**********************************************************************
 *           WINPROC_MapMsg32ATo32W
 *
 * Map a message from Ansi to Unicode.
 * Return value is -1 on error, 0 if OK, 1 if an UnmapMsg call is needed.
 *
 * FIXME:
 *  WM_CHAR, WM_CHARTOITEM, WM_DEADCHAR, WM_MENUCHAR, WM_SYSCHAR, WM_SYSDEADCHAR
 *
 * FIXME:
 *  WM_GETTEXT/WM_SETTEXT and static control with SS_ICON style:
 *  the first four bytes are the handle of the icon
 *  when the WM_SETTEXT message has been used to set the icon
 */
INT WINPROC_MapMsg32ATo32W( HWND hwnd, UINT msg, WPARAM wParam, LPARAM *plparam )
{
    switch(msg)
    {
    case WM_GETTEXT:
        {
            LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0,
                                     wParam * sizeof(WCHAR) + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
        }
        return 1;
    /* lparam is string (0-terminated) */
    case WM_SETTEXT:
    case WM_WININICHANGE:
    case CB_DIR:
    case LB_DIR:
    case LB_ADDFILE:
#ifndef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
    case LB_FINDSTRING:
    case LB_SELECTSTRING:
#endif
    case EM_REPLACESEL:
        *plparam = (LPARAM)HEAP_strdupAtoW( GetProcessHeap(), 0, (LPCSTR)*plparam );
        return (*plparam ? 1 : -1);

    case WM_NCCREATE:
    case WM_CREATE:
        {
            CREATESTRUCTW *cs = (CREATESTRUCTW *)HeapAlloc( GetProcessHeap(), 0,
                                                            sizeof(*cs) );
            if (!cs) return -1;
            *cs = *(CREATESTRUCTW *)*plparam;
            if (HIWORD(cs->lpszName))
                cs->lpszName = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                                (LPCSTR)cs->lpszName );
            if (HIWORD(cs->lpszClass))
                cs->lpszClass = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                                 (LPCSTR)cs->lpszClass );
            *plparam = (LPARAM)cs;
        }
        return 1;
    case WM_MDICREATE:
        {
            MDICREATESTRUCTW *cs =
                (MDICREATESTRUCTW *)HeapAlloc( GetProcessHeap(), 0, sizeof(*cs) );
            if (!cs) return -1;
            *cs = *(MDICREATESTRUCTW *)*plparam;
            if (HIWORD(cs->szClass))
                cs->szClass = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                               (LPCSTR)cs->szClass );
            if (HIWORD(cs->szTitle))
                cs->szTitle = HEAP_strdupAtoW( GetProcessHeap(), 0,
                                               (LPCSTR)cs->szTitle );
            *plparam = (LPARAM)cs;
        }
        return 1;

/* Listbox */
    case LB_ADDSTRING:
#ifdef __WIN32OS2__
    case LB_FINDSTRING:
    case LB_FINDSTRINGEXACT:
    case LB_SELECTSTRING:
#endif
    case LB_INSERTSTRING:
        if ( WINPROC_TestLBForStr( hwnd ))
          *plparam = (LPARAM)HEAP_strdupAtoW( GetProcessHeap(), 0, (LPCSTR)*plparam );
        return (*plparam ? 1 : -1);

    case LB_GETTEXT:                /* fixme: fixed sized buffer */
        { if ( WINPROC_TestLBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0, 256 * sizeof(WCHAR) + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
          }
        }
        return 1;

/* Combobox */
    case CB_ADDSTRING:
#ifdef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
#endif
    case CB_INSERTSTRING:
        if ( WINPROC_TestCBForStr( hwnd ))
          *plparam = (LPARAM)HEAP_strdupAtoW( GetProcessHeap(), 0, (LPCSTR)*plparam );
        return (*plparam ? 1 : -1);

    case CB_GETLBTEXT:    /* fixme: fixed sized buffer */
        { if ( WINPROC_TestCBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0, 256 * sizeof(WCHAR) + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
          }
        }
        return 1;

/* Multiline edit */
    case EM_GETLINE:
        { WORD len = (WORD)*plparam;
          LPARAM *ptr = (LPARAM *) HEAP_xalloc( GetProcessHeap(), 0, sizeof(LPARAM) + sizeof (WORD) + len*sizeof(WCHAR) );
          if (!ptr) return -1;
          *ptr++ = *plparam;  /* Store previous lParam */
          *((WORD *) ptr) = len;   /* Store the length */
          *plparam = (LPARAM)ptr;
        }
        return 1;

    case WM_ASKCBFORMATNAME:
    case WM_DEVMODECHANGE:
    case WM_PAINTCLIPBOARD:
    case WM_SIZECLIPBOARD:
    case EM_SETPASSWORDCHAR:
        // FIXME_(msg)("message %s (0x%x) needs translation, please report\n", SPY_GetMsgName(msg), msg );
        return -1;
    default:  /* No translation needed */
        return 0;
    }
}


/**********************************************************************
 *           WINPROC_UnmapMsg32ATo32W
 *
 * Unmap a message that was mapped from Ansi to Unicode.
 */
void WINPROC_UnmapMsg32ATo32W( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg)
    {
    case WM_GETTEXT:
        {
            LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpynWtoA( (LPSTR)*ptr, (LPWSTR)lParam, wParam );
            HeapFree( GetProcessHeap(), 0, ptr );
        }
        break;

    case WM_NCCREATE:
    case WM_CREATE:
        {
            CREATESTRUCTW *cs = (CREATESTRUCTW *)lParam;
            if (HIWORD(cs->lpszName))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->lpszName );
            if (HIWORD(cs->lpszClass))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->lpszClass );
            HeapFree( GetProcessHeap(), 0, cs );
        }
        break;

    case WM_MDICREATE:
        {
            MDICREATESTRUCTW *cs = (MDICREATESTRUCTW *)lParam;
            if (HIWORD(cs->szTitle))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->szTitle );
            if (HIWORD(cs->szClass))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->szClass );
            HeapFree( GetProcessHeap(), 0, cs );
        }
        break;

    case WM_SETTEXT:
    case WM_WININICHANGE:
    case CB_DIR:
    case LB_DIR:
    case LB_ADDFILE:
#ifndef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
    case LB_FINDSTRING:
    case LB_SELECTSTRING:
#endif
    case EM_REPLACESEL:
        HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

/* Listbox */
    case LB_ADDSTRING:
#ifdef __WIN32OS2__
    case LB_FINDSTRING:
    case LB_FINDSTRINGEXACT:
    case LB_SELECTSTRING:
#endif
    case LB_INSERTSTRING:
        if ( WINPROC_TestLBForStr( hwnd ))
          HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

    case LB_GETTEXT:
        { if ( WINPROC_TestLBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpyWtoA( (LPSTR)*ptr, (LPWSTR)(lParam) );
            HeapFree( GetProcessHeap(), 0, ptr );
          }
        }
        break;

/* Combobox */
    case CB_ADDSTRING:
#ifdef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
#endif
    case CB_INSERTSTRING:
        if ( WINPROC_TestCBForStr( hwnd ))
          HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

    case CB_GETLBTEXT:
        { if ( WINPROC_TestCBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpyWtoA( (LPSTR)*ptr, (LPWSTR)(lParam) );
            HeapFree( GetProcessHeap(), 0, ptr );
          }
        }
        break;

/* Multiline edit */
    case EM_GETLINE:
        { LPARAM * ptr = (LPARAM *)lParam - 1;  /* get the old lParam */
          WORD len = *(WORD *) lParam;
          lstrcpynWtoA( (LPSTR)*ptr , (LPWSTR)lParam, len );
          HeapFree( GetProcessHeap(), 0, ptr );
        }
        break;
    }
}


/**********************************************************************
 *           WINPROC_MapMsg32WTo32A
 *
 * Map a message from Unicode to Ansi.
 * Return value is -1 on error, 0 if OK, 1 if an UnmapMsg call is needed.
 */
INT WINPROC_MapMsg32WTo32A( HWND hwnd, UINT msg, WPARAM wParam, LPARAM *plparam)
{   switch(msg)
    {
    case WM_GETTEXT:
        {
            LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0,
                                               wParam + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
        }
        return 1;

    case WM_SETTEXT:
    case WM_WININICHANGE:
    case CB_DIR:
    case LB_DIR:
    case LB_ADDFILE:
#ifndef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
    case LB_FINDSTRING:
    case LB_SELECTSTRING:
#endif
    case EM_REPLACESEL:
        *plparam = (LPARAM)HEAP_strdupWtoA( GetProcessHeap(), 0, (LPCWSTR)*plparam );
        return (*plparam ? 1 : -1);

    case WM_NCCREATE:
    case WM_CREATE:
        {
            CREATESTRUCTA *cs = (CREATESTRUCTA *)HeapAlloc( GetProcessHeap(), 0,
                                                            sizeof(*cs) );
            if (!cs) return -1;
            *cs = *(CREATESTRUCTA *)*plparam;
            if (HIWORD(cs->lpszName))
                cs->lpszName  = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                                 (LPCWSTR)cs->lpszName );
            if (HIWORD(cs->lpszClass))
                cs->lpszClass = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                                 (LPCWSTR)cs->lpszClass);
            *plparam = (LPARAM)cs;
        }
        return 1;
    case WM_MDICREATE:
        {
            MDICREATESTRUCTA *cs =
                (MDICREATESTRUCTA *)HeapAlloc( GetProcessHeap(), 0, sizeof(*cs) );

            if (!cs) return -1;
            *cs = *(MDICREATESTRUCTA *)*plparam;
            if (HIWORD(cs->szTitle))
                cs->szTitle = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                               (LPCWSTR)cs->szTitle );
            if (HIWORD(cs->szClass))
                cs->szClass = HEAP_strdupWtoA( GetProcessHeap(), 0,
                                               (LPCWSTR)cs->szClass );
            *plparam = (LPARAM)cs;
        }
        return 1;

/* Listbox */
    case LB_ADDSTRING:
#ifdef __WIN32OS2__
    case LB_FINDSTRING:
    case LB_FINDSTRINGEXACT:
    case LB_SELECTSTRING:
#endif
    case LB_INSERTSTRING:
        if ( WINPROC_TestLBForStr( hwnd ))
          *plparam = (LPARAM)HEAP_strdupWtoA( GetProcessHeap(), 0, (LPCWSTR)*plparam );
        return (*plparam ? 1 : -1);

    case LB_GETTEXT:                    /* fixme: fixed sized buffer */
        { if ( WINPROC_TestLBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0, 256 + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
          }
        }
        return 1;

/* Combobox */
    case CB_ADDSTRING:
#ifdef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
#endif
    case CB_INSERTSTRING:
        if ( WINPROC_TestCBForStr( hwnd ))
          *plparam = (LPARAM)HEAP_strdupWtoA( GetProcessHeap(), 0, (LPCWSTR)*plparam );
        return (*plparam ? 1 : -1);

    case CB_GETLBTEXT:          /* fixme: fixed sized buffer */
        { if ( WINPROC_TestCBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)HeapAlloc( GetProcessHeap(), 0, 256 + sizeof(LPARAM) );
            if (!ptr) return -1;
            *ptr++ = *plparam;  /* Store previous lParam */
            *plparam = (LPARAM)ptr;
          }
        }
        return 1;

/* Multiline edit */
    case EM_GETLINE:
        { WORD len = (WORD)*plparam;
          LPARAM *ptr = (LPARAM *) HEAP_xalloc( GetProcessHeap(), 0, sizeof(LPARAM) + sizeof (WORD) + len*sizeof(CHAR) );
          if (!ptr) return -1;
          *ptr++ = *plparam;  /* Store previous lParam */
          *((WORD *) ptr) = len;   /* Store the length */
          *plparam = (LPARAM)ptr;
        }
        return 1;

    case WM_ASKCBFORMATNAME:
    case WM_DEVMODECHANGE:
    case WM_PAINTCLIPBOARD:
    case WM_SIZECLIPBOARD:
    case EM_SETPASSWORDCHAR:
        // FIXME_(msg)("message %s (%04x) needs translation, please report\n",SPY_GetMsgName(msg),msg );
        return -1;
    default:  /* No translation needed */
        return 0;
    }
}


/**********************************************************************
 *           WINPROC_UnmapMsg32WTo32A
 *
 * Unmap a message that was mapped from Unicode to Ansi.
 */
void WINPROC_UnmapMsg32WTo32A( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg)
    {
    case WM_GETTEXT:
        {
            LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpynAtoW( (LPWSTR)*ptr, (LPSTR)lParam, wParam );
            HeapFree( GetProcessHeap(), 0, ptr );
        }
        break;

    case WM_SETTEXT:
    case WM_WININICHANGE:
    case CB_DIR:
    case LB_DIR:
    case LB_ADDFILE:
#ifndef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
    case LB_FINDSTRING:
    case LB_SELECTSTRING:
#endif
    case EM_REPLACESEL:
        HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

    case WM_NCCREATE:
    case WM_CREATE:
        {
            CREATESTRUCTA *cs = (CREATESTRUCTA *)lParam;
            if (HIWORD(cs->lpszName))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->lpszName );
            if (HIWORD(cs->lpszClass))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->lpszClass );
            HeapFree( GetProcessHeap(), 0, cs );
        }
        break;

    case WM_MDICREATE:
        {
            MDICREATESTRUCTA *cs = (MDICREATESTRUCTA *)lParam;
            if (HIWORD(cs->szTitle))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->szTitle );
            if (HIWORD(cs->szClass))
                HeapFree( GetProcessHeap(), 0, (LPVOID)cs->szClass );
            HeapFree( GetProcessHeap(), 0, cs );
        }
        break;

/* Listbox */
    case LB_ADDSTRING:
#ifdef __WIN32OS2__
    case LB_FINDSTRING:
    case LB_FINDSTRINGEXACT:
    case LB_SELECTSTRING:
#endif
    case LB_INSERTSTRING:
        if ( WINPROC_TestLBForStr( hwnd ))
          HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

    case LB_GETTEXT:
        { if ( WINPROC_TestLBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpyAtoW( (LPWSTR)*ptr, (LPSTR)(lParam) );
            HeapFree(GetProcessHeap(), 0, ptr );
          }
        }
        break;

/* Combobox */
    case CB_ADDSTRING:
#ifdef __WIN32OS2__
    case CB_FINDSTRING:
    case CB_FINDSTRINGEXACT:
    case CB_SELECTSTRING:
#endif
    case CB_INSERTSTRING:
        if ( WINPROC_TestCBForStr( hwnd ))
          HeapFree( GetProcessHeap(), 0, (void *)lParam );
        break;

    case CB_GETLBTEXT:
        { if ( WINPROC_TestCBForStr( hwnd ))
          { LPARAM *ptr = (LPARAM *)lParam - 1;
            lstrcpyAtoW( (LPWSTR)*ptr, (LPSTR)(lParam) );
            HeapFree( GetProcessHeap(), 0, ptr );
          }
        }
        break;

/* Multiline edit */
    case EM_GETLINE:
        { LPARAM * ptr = (LPARAM *)lParam - 1;  /* get the old lparam */
          WORD len = *(WORD *)ptr;
          lstrcpynAtoW( (LPWSTR) *ptr, (LPSTR)lParam, len );
          HeapFree( GetProcessHeap(), 0, ptr );
        }
        break;
    }
}

/**********************************************************************
 *           WINPROC_CallProc32ATo32W
 *
 * Call a window procedure, translating args from Ansi to Unicode.
 */
LRESULT WINPROC_CallProc32ATo32W( WNDPROC func, HWND hwnd,
                                  UINT msg, WPARAM wParam,
                                  LPARAM lParam )
{
    LRESULT result;

    if (WINPROC_MapMsg32ATo32W( hwnd, msg, wParam, &lParam ) == -1) return 0;
    result = func( hwnd, msg, wParam, lParam );
    WINPROC_UnmapMsg32ATo32W( hwnd, msg, wParam, lParam );
    return result;
}

/**********************************************************************
 *           WINPROC_CallProc32WTo32A
 *
 * Call a window procedure, translating args from Unicode to Ansi.
 */
LRESULT WINPROC_CallProc32WTo32A( WNDPROC func, HWND hwnd,
                                  UINT msg, WPARAM wParam,
                                  LPARAM lParam )
{
    LRESULT result;

    if (WINPROC_MapMsg32WTo32A( hwnd, msg, wParam, &lParam ) == -1) return 0;

    result = func( hwnd, msg, wParam, lParam );
    WINPROC_UnmapMsg32WTo32A( hwnd, msg, wParam, lParam );
    return result;
}
//******************************************************************************
//TODO: QS_HOTKEY (oslibmsg.cpp) & low word bits
//high word = messages currently in queue
//low word  = messages that have been added to the queue and are still in the
//            queue since the last call to GetQueueStatus
//******************************************************************************
DWORD WIN32API GetQueueStatus( UINT flags)
{
 DWORD queueStatus;

    dprintf(("USER32:  GetQueueStatus"));
    queueStatus = OSLibWinQueryQueueStatus();

    queueStatus = MAKELONG(queueStatus, queueStatus);
    return queueStatus & MAKELONG(flags, flags);
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetInputState
 * Purpose   : The GetInputState function determines whether there are
 *             mouse-button or keyboard messages in the calling thread's message queue.
 * Parameters:
 * Variables :
 * Result    : If the queue contains one or more new mouse-button or keyboard
 *               messages, the return value is TRUE.
 *             If the function fails, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API GetInputState(VOID)
{
 DWORD queueStatus;
 BOOL  rc;

  queueStatus = OSLibWinQueryQueueStatus();

  rc = (queueStatus & (QS_KEY | QS_MOUSEBUTTON)) ? TRUE : FALSE;
  dprintf(("USER32:GetInputState() returned %d", rc));
  return rc;
}
//******************************************************************************
/* Synchronization Functions */
//******************************************************************************
DWORD MsgWaitForMultipleObjects(DWORD nCount, LPHANDLE pHandles, BOOL fWaitAll,
                                DWORD dwMilliseconds, DWORD dwWakeMask)
{
 DWORD curtime, endtime;

  dprintf(("MsgWaitForMultipleObjects %x %x %d %d %x", nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask));
  // @@@PH this is a temporary bugfix for WINFILE.EXE
  if (nCount == 0)
  {
        if(dwMilliseconds == 0) {
                if(GetQueueStatus(dwWakeMask) == 0) {
                        return WAIT_TIMEOUT;
                }
                return WAIT_OBJECT_0;
        }
        //SvL: Check time, wait for any message, check msg type and determine if
        //     we have to return
        //TODO: Timeout isn't handled correctly (can return too late)
        curtime = GetCurrentTime();
        endtime = curtime + dwMilliseconds;
        while(curtime < endtime || dwMilliseconds == INFINITE) {
                if(OSLibWinWaitMessage() == FALSE) {
                        dprintf(("OSLibWinWaitMessage returned FALSE!"));
                        return -1;
                }
                if(GetQueueStatus(dwWakeMask) != 0) {
                        return WAIT_OBJECT_0;
                }
                curtime = GetCurrentTime();
        }
        return WAIT_TIMEOUT;
  }
  //SvL: Call handlemanager function as we need to translate handles
  return HMMsgWaitForMultipleObjects(nCount,pHandles,fWaitAll,dwMilliseconds,dwWakeMask);
}
