/* $Id: windowmsg.cpp,v 1.8 1999-07-26 20:03:49 sandervl Exp $ */
/*
 * Win32 window message APIs for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 * Parts based on Wine Windows code (windows\message.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <win32wnd.h>
#include <win.h>
#include <hooks.h>
#include "oslibwin.h"

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
BOOL WIN32API TranslateMessage( const MSG * arg1)
{
//    return O32_TranslateMessage(arg1);
   return TRUE;
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
LONG WIN32API GetMessageExtraInfo(void)
{
    dprintf(("USER32:  GetMessageExtraInfo\n"));
    return O32_GetMessageExtraInfo();
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMessagePos(void)
{
    dprintf(("USER32:  GetMessagePos\n"));
    return O32_GetMessagePos();
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetMessageTime(void)
{
    dprintf(("USER32:  GetMessageTime\n"));
    return O32_GetMessageTime();
}
//******************************************************************************
//TODO: hwnd == HWND_BROADCAST
//******************************************************************************
LRESULT WIN32API SendMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
	dprintf(("SendMessageA, window %x not found", hwnd));
	return 0;
    }
    return window->SendMessageA(msg, wParam, lParam);
}
//******************************************************************************
//TODO: hwnd == HWND_BROADCAST
//******************************************************************************
LRESULT WIN32API SendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
	dprintf(("SendMessageW, window %x not found", hwnd));
	return 0;
    }
    return window->SendMessageW(msg, wParam, lParam);
}
//******************************************************************************
//TODO: hwnd == HWND_BROADCAST
//******************************************************************************
BOOL WIN32API PostMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    if(hwnd == NULL)
	return PostThreadMessageA(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
	dprintf(("PostMessageA, window %x not found", hwnd));
	return 0;
    }
    dprintf(("PostMessageA, %x %x %x %x", hwnd, msg, wParam, lParam));
    return window->PostMessageA(msg, wParam, lParam);
}
//******************************************************************************
//TODO: hwnd == HWND_BROADCAST
//******************************************************************************
BOOL WIN32API PostMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  Win32Window *window;

    if(hwnd == NULL)
	return PostThreadMessageW(GetCurrentThreadId(), msg, wParam, lParam);

    window = Win32Window::GetWindowFromHandle(hwnd);
    if(!window) {
	dprintf(("PostMessageW, window %x not found", hwnd));
	return 0;
    }
    dprintf(("PostMessageW, %x %x %x %x", hwnd, msg, wParam, lParam));
    return window->PostMessageW(msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WaitMessage(void)
{
#ifdef DEBUG
    WriteLog("USER32:  WaitMessage\n");
#endif
    return O32_WaitMessage();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageA(LPMSG arg1, HWND arg2, UINT arg3, UINT arg4, UINT arg5)
{
#ifdef DEBUG
//    WriteLog("USER32:  PeekMessage\n");
#endif
    return O32_PeekMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageW( LPMSG arg1, HWND arg2, UINT arg3, UINT arg4, UINT  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  PeekMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PeekMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InSendMessage(void)
{
#ifdef DEBUG
    WriteLog("USER32:  InSendMessage\n");
#endif
    return O32_InSendMessage();
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
BOOL WIN32API ReplyMessage( LRESULT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  ReplyMessage\n");
#endif
    return O32_ReplyMessage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageA( DWORD arg1, UINT arg2, WPARAM arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostThreadMessageA\n");
#endif
    return O32_PostThreadMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostThreadMessageW( DWORD arg1, UINT arg2, WPARAM arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostThreadMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PostThreadMessage(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API RegisterWindowMessageA(LPCSTR arg1)
{
 UINT rc;

    rc = O32_RegisterWindowMessage(arg1);
#ifdef DEBUG
    WriteLog("USER32:  RegisterWindowMessageA %s returned %X\n", arg1, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterWindowMessageW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 UINT  rc;

#ifdef DEBUG
    WriteLog("USER32:  RegisterWindowMessageW\n");
#endif
    rc = O32_RegisterWindowMessage(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API CallWindowProcA(WNDPROC wndprcPrev,
                                 HWND       arg2,
                                 UINT       arg3,
                                 WPARAM     arg4,
                                 LPARAM     arg5)
{
#ifdef DEBUG
////    WriteLog("USER32:  CallWindowProcA %X hwnd=%X, msg = %X\n", wndprcPrev, arg2, arg3);
#endif

    return wndprcPrev(arg2, arg3, arg4, arg5);   //win32 callback (__stdcall)
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API CallWindowProcW(WNDPROC arg1,
                                 HWND       arg2,
                                 UINT       arg3,
                                 WPARAM     arg4,
                                 LPARAM     arg5)
{
  dprintf(("USER32:  CallWindowProcW(%08xh,%08xh,%08xh,%08xh,%08xh) not properly implemented.\n",
           arg1,
           arg2,
           arg3,
           arg4,
           arg5));

  return CallWindowProcA(arg1,
                            arg2,
                            arg3,
                            arg4,
                            arg5);
}
//******************************************************************************
//No need to support this
//******************************************************************************
BOOL WIN32API SetMessageQueue(int cMessagesMax)
{
#ifdef DEBUG
  WriteLog("USER32:  SetMessageQueue\n");
#endif
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageTimeoutA(HWND hwnd, UINT Msg, WPARAM wParam,
                    LPARAM lParam, UINT fuFlags, UINT uTimeOut,
                    LPDWORD lpdwResult)
{
#ifdef DEBUG
  WriteLog("USER32:  SendMessageTimeoutA, partially implemented\n");
#endif
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
#ifdef DEBUG
  WriteLog("USER32:  SendMessageTimeoutW, partially implemented\n");
#endif
  return(SendMessageTimeoutA(hwnd, Msg, wParam, lParam, fuFlags, uTimeOut, lpdwResult));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageA(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SendNotifyMessageA, not completely implemented\n");
#endif
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SendNotifyMessageW(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SendNotifyMessageW, not completely implemented\n");
#endif
  return(SendMessageA(hwnd, Msg, wParam, lParam));
}
//******************************************************************************
//******************************************************************************
LPARAM WIN32API SetMessageExtraInfo(LPARAM lParam)
{
#ifdef DEBUG
  WriteLog("USER32:  SetMessageExtraInfo, not implemented\n");
#endif
  return(0);
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
