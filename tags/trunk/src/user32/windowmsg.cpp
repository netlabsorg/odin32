/* $Id: windowmsg.cpp,v 1.40 2003-02-27 14:22:22 sandervl Exp $ */
/*
 * Win32 window message APIs for OS/2
 *
 * Copyright 1999-2001 Sander van Leeuwen
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

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>
#include <win.h>
#include <heapstring.h>
#include <handlemanager.h>
#include <wprocess.h>
#include "oslibutil.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "hook.h"

#define DBG_LOCALLOG	DBG_windowmsg
#include "dbglocal.h"

ODINDEBUGCHANNEL(USER32-WINDOWMSG)


//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageA(const MSG * msg)
{
    dprintf2(("DispatchMessageA %x %x %x %x %x", msg->hwnd, msg->message, msg->wParam, msg->lParam, msg->time));
    return OSLibWinDispatchMsg((MSG *)msg);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API DispatchMessageW( const MSG * msg)
{
    dprintf2(("DispatchMessageW %x %x %x %x %x", msg->hwnd, msg->message, msg->wParam, msg->lParam, msg->time));
    return OSLibWinDispatchMsg((MSG *)msg, TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMessage(const MSG *msg)
{
    // check the message code
    if ( (msg->message <  WM_KEYDOWN) ||
         (msg->message >  WM_SYSKEYUP)||
         (msg->message == WM_CHAR)    ||
         (msg->message == WM_DEADCHAR) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
  
    // only WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP
    // can go into TranslateMessage
  
    return OSLibWinTranslateMessage((MSG *)msg);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageA( LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax)
{
    BOOL ret;

    dprintf2(("GetMessageA %x %x-%x", hwnd, uMsgFilterMin, uMsgFilterMax));
    ret = OSLibWinGetMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax);
    if(ret) dprintf2(("GetMessageA %x %x %x %x", hwnd, pMsg->message, pMsg->wParam, pMsg->lParam));
    HOOK_CallHooksA(WH_GETMESSAGE, HC_ACTION, PM_REMOVE, (LPARAM)pMsg);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMessageW( LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax)
{
    BOOL ret;

    dprintf2(("GetMessageW %x %x-%x", hwnd, uMsgFilterMin, uMsgFilterMax));
    ret = OSLibWinGetMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, TRUE);
    HOOK_CallHooksW(WH_GETMESSAGE, HC_ACTION, PM_REMOVE, (LPARAM)pMsg);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PeekMessageA(LPMSG msg, HWND hwndOwner, UINT uMsgFilterMin,
                           UINT uMsgFilterMax, UINT fuRemoveMsg)
{
    BOOL fFoundMsg;
 
    dprintf2(("PeekMessageA %x %d-%d %d", hwndOwner, uMsgFilterMin, uMsgFilterMax, fuRemoveMsg));
    fFoundMsg = OSLibWinPeekMsg(msg, hwndOwner, uMsgFilterMin, uMsgFilterMax,
                                fuRemoveMsg, FALSE);
    if(fFoundMsg) {
        dprintf2(("PeekMessageA %x %d-%d %d found message %x %d %x %x", hwndOwner, uMsgFilterMin, uMsgFilterMax, fuRemoveMsg, msg->hwnd, msg->message, msg->wParam, msg->lParam));
        HOOK_CallHooksA(WH_GETMESSAGE, HC_ACTION, fuRemoveMsg & PM_REMOVE, (LPARAM)msg );
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

    dprintf2(("PeekMessageW %x %d-%d %d", hwndOwner, uMsgFilterMin, uMsgFilterMax, fuRemoveMsg));
    fFoundMsg = OSLibWinPeekMsg(msg, hwndOwner, uMsgFilterMin, uMsgFilterMax,
                                fuRemoveMsg, TRUE);
    if(fFoundMsg) {
        dprintf2(("PeekMessageW %x %d-%d %d found message %x %d %x %x", hwndOwner, uMsgFilterMin, uMsgFilterMax, fuRemoveMsg, msg->hwnd, msg->message, msg->wParam, msg->lParam));
        HOOK_CallHooksW(WH_GETMESSAGE, HC_ACTION, fuRemoveMsg & PM_REMOVE, (LPARAM)msg );
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
    dprintf(("USER32: GetMessageExtraInfo %x", GetThreadMessageExtraInfo()));
    return GetThreadMessageExtraInfo();
}
//******************************************************************************
//******************************************************************************
LPARAM WIN32API SetMessageExtraInfo(LPARAM lParam)
{
    dprintf(("USER32: SetMessageExtraInfo %x", lParam));
    return SetThreadMessageExtraInfo(lParam);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMessagePos(void)
{
    DWORD pos;

    pos = OSLibWinGetMessagePos();
    dprintf(("USER32: GetMessagePos -> (%d,%d)", LOWORD(pos), HIWORD(pos)));
    return pos;
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
VOID WIN32API PostQuitMessage( int nExitCode)
{
    dprintf(("USER32:  PostQuitMessage\n"));
    OSLibWinPostQuitMessage(nExitCode);
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

    queueStatus = OSLibWinQueryQueueStatus();
    queueStatus = MAKELONG(queueStatus, queueStatus);

    dprintf(("USER32: GetQueueStatus %x returned %x", flags, queueStatus & MAKELONG(flags, flags)));

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
DWORD WIN32API MsgWaitForMultipleObjects(DWORD nCount, LPHANDLE pHandles, BOOL fWaitAll,
                                         DWORD dwMilliseconds, DWORD dwWakeMask)
{
 DWORD curtime, endtime, ret;
 MSG msg;

    //TODO: Functions such as GetMessage, PeekMessage and WaitMessage can mark messages as old
    //      MsgWaitForMultipleObjects shouldn't return until new input has arrived (MSDN)
    //      We are not 100% correct with this implementation. GetQueueStatus checks all messages
    //      in the queue.
    //      Very obscure behaviour, so it's unlikely any application depends on it

    //4 cases:
    //1: Wait for all   -> check for message arrival, call WaitForMultipleObjects
    //2: Timeout = 0 ms -> call WaitForMultipleObjects with timeout 0, check for message arrival
    //3: nCount = 0     -> check for message arrival
    //4: rest           -> check for either message arrival or signalled object

    dprintf(("MsgWaitForMultipleObjects %x %x %d %d %x", nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask));
    if(fWaitAll) //case 1
    {   //wait for message arrival first
        curtime = GetCurrentTime();
        endtime = curtime + dwMilliseconds;
        while(curtime < endtime || dwMilliseconds == INFINITE) {
              if(OSLibWinWaitMessage() == FALSE) {
                      dprintf(("OSLibWinWaitMessage returned FALSE!"));
                      return WAIT_ABANDONED;
              }
              if(GetQueueStatus(dwWakeMask) != 0) {
                      break;
              }
              //TODO: Ignoring all messages could be dangerous. But processing them,
              //while the app doesn't expect any, isn't safe either.
              if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
              {
                  if (msg.message == WM_QUIT) {
                       dprintf(("ERROR: MsgWaitForMultipleObjects call abandoned because WM_QUIT msg was received!!"));
                       return WAIT_ABANDONED;
                  }

                  /* otherwise dispatch it */
                  DispatchMessageA(&msg);
              }
              curtime = GetCurrentTime();
        }
        if(dwMilliseconds != INFINITE && curtime > endtime) {
            dprintf(("No messages found in specified time"));
            return WAIT_TIMEOUT;
        }
        //ok, the right message has arrived, now try to grab all objects
        ret = WaitForMultipleObjects(nCount, pHandles, fWaitAll, dwMilliseconds);
        if(ret >= WAIT_OBJECT_0 + nCount) {
            //failure
            dprintf(("WaitForMultipleObjects failed with %d", ret));
            return ret;
        }
        return ret;
    }
    if(dwMilliseconds == 0) { //case 2
        if(nCount) {
            ret = WaitForMultipleObjects(nCount, pHandles, fWaitAll, dwMilliseconds);
            if(ret < WAIT_OBJECT_0 + nCount) {
                return ret;
            }
        }
        if(GetQueueStatus(dwWakeMask) == 0) {
            return WAIT_TIMEOUT;
        }
        return WAIT_OBJECT_0 + nCount;  //right message has arrived
    }
    if(nCount == 0) //case 3
    {
        //SvL: Check time, wait for any message, check msg type and determine if
        //     we have to return
        //TODO: Timeout isn't handled correctly (can return too late)
        curtime = GetCurrentTime();
        endtime = curtime + dwMilliseconds;
        while(curtime < endtime || dwMilliseconds == INFINITE) {
                if(OSLibWinWaitMessage() == FALSE) {
                        dprintf(("OSLibWinWaitMessage returned FALSE!"));
                        return WAIT_ABANDONED;
                }
                if(GetQueueStatus(dwWakeMask) != 0) {
                        return WAIT_OBJECT_0;
                }
                //TODO: Ignoring all messages could be dangerous. But processing them,
                //while the app doesn't expect any, isn't safe either.
                if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
                {
                    if (msg.message == WM_QUIT) {
                         dprintf(("ERROR: MsgWaitForMultipleObjects call abandoned because WM_QUIT msg was received!!"));
                         return WAIT_ABANDONED;
                    }
   
                    /* otherwise dispatch it */
                    DispatchMessageA(&msg);
                }
                curtime = GetCurrentTime();
        }
        return WAIT_TIMEOUT;
    }

    //Case 4:
#if 1
    //Note: The WGSS implementation of this function is flawed. Returns
    //      when a message is sent to the msg queue, regardless of dwWakeMask
    TEB *teb = GetTEBFromThreadId(GetCurrentThreadId());
    if(teb == NULL) {
       DebugInt3();
       return WAIT_ABANDONED;
    }
    //check if any object is already signalled
    ret = WaitForMultipleObjects(nCount, pHandles, fWaitAll, 0);
    if(ret < WAIT_OBJECT_0 + nCount) {
        return ret;
    }
    //if the msg queue already contains the messages defined by dwWakeMask,
    //then return immediately
    if(GetQueueStatus(dwWakeMask) != 0) {
        return WAIT_OBJECT_0+nCount;
    }
    if(dwWakeMask & QS_POSTMESSAGE) {
        HANDLE *pHandlesTmp = (HANDLE *)alloca((nCount+1)*sizeof(HANDLE));
        if(pHandlesTmp == NULL || !teb->o.odin.hPostMsgEvent) {
            DebugInt3();
            return WAIT_ABANDONED;
        }
        memcpy(pHandlesTmp, pHandles, nCount*sizeof(HANDLE));
        pHandlesTmp[nCount] = teb->o.odin.hPostMsgEvent;

        //mark this thread as waiting for window messages
        teb->o.odin.dwWakeMask = dwWakeMask;

        ResetEvent(teb->o.odin.hPostMsgEvent);
        ret = HMMsgWaitForMultipleObjects(nCount+1,pHandlesTmp,fWaitAll,dwMilliseconds,dwWakeMask);

        //unmark thread
        teb->o.odin.dwWakeMask = 0;

        //nCount + 2 -> message event -> return nCount + 1
        return (ret == nCount + 2) ? (nCount + 1) : ret;
    }
    //Call handlemanager function as we need to translate handles (KERNEL32)
    ret = HMMsgWaitForMultipleObjects(nCount,pHandles,fWaitAll,dwMilliseconds,dwWakeMask);
    return ret;
#else
    //This method has a high latency (too high for some apps)
    //TODO: Timeout isn't handled correctly (can return too late)
    curtime = GetCurrentTime();
    endtime = curtime + dwMilliseconds;
    while(curtime < endtime || dwMilliseconds == INFINITE) {
        //check queue status for presence of requested message types
        if(GetQueueStatus(dwWakeMask) != 0) {
            dprintf(("Found message(s) we were looking for. Return success"));
            return WAIT_OBJECT_0 + nCount;  //present, return success
        }
        //TODO: Ignoring all messages could be dangerous. But processing them,
        //while the app doesn't expect any, isn't safe either.
        if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
            if (msg.message == WM_QUIT) {
                dprintf(("ERROR: MsgWaitForMultipleObjects call abandoned because WM_QUIT msg was received!!"));
                return WAIT_ABANDONED;
            }

            /* otherwise dispatch it */
            DispatchMessageA(&msg);
        }
        //check if any object is signalled (timeout 10ms)
        ret = WaitForMultipleObjects(nCount, pHandles, fWaitAll, 4);
        if(ret < WAIT_OBJECT_0 + nCount) {
            //an object was signalled, return immediately
            dprintf(("WaitForMultipleObjects success with %d", ret));
            return ret;
        }
        else
        if(ret != WAIT_TIMEOUT) {
            //failure, abort
            dprintf(("WaitForMultipleObjects failed with %d", ret));
            return ret;
        }
        curtime = GetCurrentTime();
    }
    return WAIT_TIMEOUT;
#endif
}
