/* $Id: windowmsg.cpp,v 1.1 1999-07-14 08:35:37 sandervl Exp $ */
/*
 * Win32 window message APIs for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>


//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageA( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageA\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageW( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageW\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API PostQuitMessage( int nExitCode)
{
    dprintf(("USER32:  PostQuitMessage\n"));

    O32_PostQuitMessage(arg1);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageA( const MSG * arg1)
{
////    dprintf(("USER32:  DispatchMessage\n"));
    return O32_DispatchMessage(arg1);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageW( const MSG * arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DispatchMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_DispatchMessage(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMessage( const MSG * arg1)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateMessage\n");
#endif
    return O32_TranslateMessage(arg1);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageA(HWND arg1, UINT arg2, WPARAM  arg3, LPARAM arg4)
{
 LRESULT rc;

#ifdef DEBUG1
    WriteLog("USER32:  SendMessage....\n");
#endif
    rc = O32_SendMessage(arg1, arg2, arg3, arg4);
#ifdef DEBUG1
    WriteLog("USER32:  *****SendMessage %X %X %X %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
LRESULT WIN32API SendMessageW( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
LRESULT rc;

#ifdef DEBUG
    WriteLog("USER32:  SendMessageW....\n");
#endif
    rc = O32_SendMessage(arg1, arg2, arg3, arg4);
#ifdef DEBUG
    WriteLog("USER32:  SendMessageW %X %X %X %X returned %d\n", arg1, arg2, arg3, arg4, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageA( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  IsDialogMessage\n");
#endif
    return O32_IsDialogMessage(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PostMessageA( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostMessageA %X %X %X %X\n", arg1, arg2, arg3, arg4);
#endif
    return O32_PostMessage(arg1, arg2, arg3, arg4);
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
// NOTE: Open32 function doesn't have the 'W'.
BOOL WIN32API PostMessageW( HWND arg1, UINT arg2, WPARAM  arg3, LPARAM  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  PostMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_PostMessage(arg1, arg2, arg3, arg4);
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
BOOL WIN32API IsDialogMessageW( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsDialogMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsDialogMessage(arg1, arg2);
}
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
BOOL WIN32API CallMsgFilterA( LPMSG arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterA\n");
#endif
    return O32_CallMsgFilter(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CallMsgFilterW( LPMSG arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CallMsgFilterW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_CallMsgFilter(arg1, arg2);
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
//******************************************************************************
//******************************************************************************
