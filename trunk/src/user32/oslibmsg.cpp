/* $Id: oslibmsg.cpp,v 1.32 2000-04-13 18:50:44 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Some messages that are sent to the frame window are directly passed on to the client
 *       -> Get/PeekMessage never gets them as we return a dummy message for non-client windows
 *       (i.e. menu WM_COMMAND messages)
 *
 * TODO: Filter translation isn't correct! (for posted messages or messages that don't have
 *       a PM version.
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_DOSPROCESS
#include <os2wrap.h>
#include <string.h>
#include <misc.h>
#include "oslibmsg.h"
#include <winconst.h>
#include <win32api.h>
#include <win32wnd.h>
#include "oslibutil.h"
#include "timer.h"
#include <thread.h>
#include <wprocess.h>
#include "pmwindow.h"
#include "oslibwin.h"
#include <win\hook.h>

#define DBG_LOCALLOG	DBG_oslibmsg
#include "dbglocal.h"

typedef BOOL (EXPENTRY FNTRANS)(MSG *, QMSG *);
typedef FNTRANS *PFNTRANS;

typedef struct
{
   ULONG   msgOS2;
   ULONG   msgWin32;
// PFNTRANS toOS2;
// PFNTRANS toWIN32;
} MSGTRANSTAB, *PMSGTRANSTAB;

//NOTE: Must be ordered by win32 message id!!
MSGTRANSTAB MsgTransTab[] = {
   WM_NULL,          WINWM_NULL,
   WM_CREATE,        WINWM_CREATE,
   WM_DESTROY,       WINWM_DESTROY,
   WM_MOVE,          WINWM_MOVE,            //TODO: Sent directly
   WM_SIZE,          WINWM_SIZE,            //TODO: Sent directly
   WM_ACTIVATE,      WINWM_ACTIVATE,
   WM_SETFOCUS,      WINWM_SETFOCUS,
   WM_SETFOCUS,      WINWM_KILLFOCUS,
   WM_ENABLE,        WINWM_ENABLE,
   WM_PAINT,         WINWM_PAINT,
   WM_CLOSE,         WINWM_CLOSE,
   WM_QUIT,          WINWM_QUIT,
   WM_SHOW,          WINWM_SHOWWINDOW,

   WM_HITTEST,       WINWM_NCHITTEST,

   //TODO: Needs better translation!
   WM_CHAR,          WINWM_KEYDOWN,
   WM_CHAR,          WINWM_KEYUP,
   WM_CHAR,          WINWM_CHAR,
   WM_CHAR,          WINWM_DEADCHAR,
   WM_CHAR,          WINWM_SYSKEYDOWN,
   WM_CHAR,          WINWM_SYSKEYUP,
   WM_CHAR,          WINWM_SYSCHAR,
   WM_CHAR,          WINWM_SYSDEADCHAR,
   WM_CHAR,          WINWM_KEYLAST,

   WM_COMMAND,       WINWM_COMMAND,
   WM_SYSCOMMAND,    WINWM_SYSCOMMAND,
   //
   WM_TIMER,         WINWM_TIMER,
   WM_INITMENU,      WINWM_INITMENU,
   //
   WM_MOUSEMOVE,     WINWM_MOUSEMOVE,
   WM_BUTTON1DOWN,   WINWM_LBUTTONDOWN,
   WM_BUTTON1UP,     WINWM_LBUTTONUP,
   WM_BUTTON1DBLCLK, WINWM_LBUTTONDBLCLK,
   WM_BUTTON2DOWN,   WINWM_RBUTTONDOWN,
   WM_BUTTON2UP,     WINWM_RBUTTONUP,
   WM_BUTTON2DBLCLK, WINWM_RBUTTONDBLCLK,
   WM_BUTTON3DOWN,   WINWM_MBUTTONDOWN,
   WM_BUTTON3UP,     WINWM_MBUTTONUP,
   WM_BUTTON3DBLCLK, WINWM_MBUTTONDBLCLK,

   999999999,        999999999,
};
#define MAX_MSGTRANSTAB (sizeof(MsgTransTab)/sizeof(MsgTransTab[0]))

QMSG *MsgThreadPtr = 0;

LRESULT WIN32API SendMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//******************************************************************************
//******************************************************************************
BOOL OSLibInitMsgQueue()
{
   if(DosAllocThreadLocalMemory(sizeof(QMSG)/sizeof(ULONG), (PULONG *)&MsgThreadPtr) != 0)
   {
        dprintf(("OSLibInitMsgQueue: local thread memory alloc failed!!"));
        DebugInt3();
        return FALSE;
   }
   return TRUE;
}
//******************************************************************************
//******************************************************************************
void WinToOS2MsgTranslate(MSG *winMsg, QMSG *os2Msg, BOOL isUnicode)
{
//  memcpy(os2Msg, winMsg, sizeof(MSG));
//  os2Msg->hwnd = Win32Window::Win32ToOS2Handle(winMsg->hwnd);
//  os2Msg->reserved = 0;
}
//******************************************************************************
//TODO: NOT COMPLETE nor 100% CORRECT!!!
//If both the minimum & maximum message are unknown, the result can be wrong (max > min)!
//******************************************************************************
ULONG TranslateWinMsg(ULONG msg, BOOL fMinFilter)
{
    if(msg == 0)
        return 0;

    if(msg >= WINWM_USER)
        return WIN32APP_POSTMSG;

    for(int i=0;i<MAX_MSGTRANSTAB;i++)
    {
        if(fMinFilter && MsgTransTab[i].msgWin32 >= msg) {
            return MsgTransTab[i].msgOS2;
        }
        else
        if(!fMinFilter && MsgTransTab[i].msgWin32 >= msg) {
            if(MsgTransTab[i].msgWin32 == msg)
                    return MsgTransTab[i].msgOS2;
            else    return MsgTransTab[i-1].msgOS2;
        }
    }

    return 0;
}
//******************************************************************************
//******************************************************************************
void OSLibWinPostQuitMessage(ULONG nExitCode)
{
 APIRET rc;

  rc = WinPostQueueMsg(NULLHANDLE, WM_QUIT, MPFROMLONG(nExitCode), 0);
  dprintf(("WinPostQueueMsg %d returned %d", nExitCode, rc));
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode)
{
 THDB *thdb;
 QMSG  os2msg;
 LONG  rc;

  thdb = GetThreadTHDB();
  if(thdb == NULL) {
        DebugInt3();
        return FALSE;
  }

  //TODO: What to do if app changed msg? (translate)
  //  WinToOS2MsgTranslate(msg, &qmsg, isUnicode);

  if(msg->time == MsgThreadPtr->time || msg->hwnd == 0) {
        memcpy(&os2msg, MsgThreadPtr, sizeof(QMSG));
        MsgThreadPtr->time = -1;
        if(msg->hwnd) {
            	thdb->nrOfMsgs = 1;
            	thdb->msgstate++; //odd -> next call to our PM window handler should dispatch the translated msg
            	memcpy(&thdb->msg, msg, sizeof(MSG));
        }
        if(os2msg.hwnd || os2msg.msg == WM_QUIT) {
		memset(MsgThreadPtr, 0, sizeof(*MsgThreadPtr));
            	return (LONG)WinDispatchMsg(thdb->hab, &os2msg);
        }
        //SvL: Don't dispatch messages sent by PostThreadMessage (correct??)
        //     Or WM_TIMER msgs with no window handle or timer proc
        return 0;

  }
  else {//is this allowed?
//        dprintf(("WARNING: OSLibWinDispatchMsg: called with own message!"));
        return SendMessageA(msg->hwnd, msg->message, msg->wParam, msg->lParam);
  }
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                    BOOL isUnicode)
{
 BOOL rc, eaten;
 THDB *thdb;
 QMSG  os2msg;
 HWND  hwndOS2 = 0;

  if(hwnd) {
  	hwndOS2 = Win32BaseWindow::Win32ToOS2Handle(hwnd);
	if(hwndOS2 == NULL) {
		memset(pMsg, 0, sizeof(MSG));
		dprintf(("GetMsg: window %x NOT FOUND!", hwnd));
		SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
		return TRUE;
	}
  }

  thdb = GetThreadTHDB();
  if(thdb == NULL) {
        DebugInt3();
	return TRUE;
  }

  if(thdb->fTranslated && (!hwnd || hwnd == thdb->msgWCHAR.hwnd)) {
        if(uMsgFilterMin) {
            if(thdb->msgWCHAR.message < uMsgFilterMin)
                goto continuegetmsg;
        }
        if(uMsgFilterMax) {
            if(thdb->msgWCHAR.message > uMsgFilterMax)
                goto continuegetmsg;
        }
        thdb->fTranslated = FALSE;
        memcpy(pMsg, &thdb->msgWCHAR, sizeof(MSG));
        MsgThreadPtr->msg  = 0;
        MsgThreadPtr->hwnd = 0;
        return (pMsg->message != WINWM_QUIT);
  }

continuegetmsg:
  if(hwnd) {
        do {
            WinWaitMsg(thdb->hab, TranslateWinMsg(uMsgFilterMin, TRUE), TranslateWinMsg(uMsgFilterMax, FALSE));
            rc = OSLibWinPeekMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, PM_REMOVE_W, isUnicode);
        }
        while(rc == FALSE);

        return (pMsg->message != WINWM_QUIT);
  }
  else
  {
    do {
        eaten = FALSE;
        rc = WinGetMsg(thdb->hab, &os2msg, TranslateWinMsg(uMsgFilterMin, TRUE), TranslateWinMsg(uMsgFilterMax, FALSE), 0);
        if (os2msg.msg == WM_TIMER)
            eaten = TIMER_HandleTimer(&os2msg);
    } while (eaten);
  }

  OS2ToWinMsgTranslate((PVOID)thdb, &os2msg, pMsg, isUnicode, MSG_REMOVE);
  memcpy(MsgThreadPtr, &os2msg, sizeof(QMSG));

  if(pMsg->message <= WINWM_KEYLAST && pMsg->message >= WINWM_KEYDOWN)
  {
    	if(ProcessKbdHook(pMsg, TRUE))
      		goto continuegetmsg;
  }
  return rc;
}
//******************************************************************************
//PeekMessage retrieves only messages associated with the window identified by the 
//hwnd parameter or any of its children as specified by the IsChild function, and within 
//the range of message values given by the uMsgFilterMin and uMsgFilterMax 
//parameters. If hwnd is NULL, PeekMessage retrieves messages for any window that 
//belongs to the current thread making the call. (PeekMessage does not retrieve 
//messages for windows that belong to other threads.) If hwnd is -1, PeekMessage only 
//returns messages with a hwnd value of NULL, as posted by the PostAppMessage 
//function. If uMsgFilterMin and uMsgFilterMax are both zero, PeekMessage returns all 
//available messages (no range filtering is performed). 
//TODO: Not working as specified right now!
//******************************************************************************
BOOL OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                     DWORD fRemove, BOOL isUnicode)
{
 BOOL  rc, eaten;
 THDB *thdb;
 QMSG  os2msg;
 HWND  hwndOS2 = 0;

  if(hwnd && hwnd != -1) {
  	hwndOS2 = Win32BaseWindow::Win32ToOS2Handle(hwnd);
	if(hwndOS2 == NULL) {
		dprintf(("PeekMsg: window %x NOT FOUND!", hwnd));
		SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
		return FALSE;
	}
  }

  thdb = GetThreadTHDB();
  if(thdb == NULL) {
        DebugInt3();
        return FALSE;
  }

  if(thdb->fTranslated && (!hwnd || hwnd == thdb->msgWCHAR.hwnd)) {
        if(uMsgFilterMin) {
            if(thdb->msgWCHAR.message < uMsgFilterMin)
                goto continuepeekmsg;
        }
        if(uMsgFilterMax) {
            if(thdb->msgWCHAR.message > uMsgFilterMax)
                goto continuepeekmsg;
        }

        if(fRemove & PM_REMOVE_W) {
            thdb->fTranslated = FALSE;
            MsgThreadPtr->msg  = 0;
            MsgThreadPtr->hwnd = 0;
        }
        memcpy(pMsg, &thdb->msgWCHAR, sizeof(MSG));
        return TRUE;
  }

continuepeekmsg:
  do {
        eaten = FALSE;
        rc = WinPeekMsg(thdb->hab, &os2msg, hwndOS2, TranslateWinMsg(uMsgFilterMin, TRUE),
                        TranslateWinMsg(uMsgFilterMax, FALSE), (fRemove & PM_REMOVE_W) ? PM_REMOVE : PM_NOREMOVE);

        if (rc && (fRemove & PM_REMOVE_W) && os2msg.msg == WM_TIMER) {
            eaten = TIMER_HandleTimer(&os2msg);
        }
  }
  while (eaten && rc);

  if(rc == FALSE) {
    return FALSE;
  }

  OS2ToWinMsgTranslate((PVOID)thdb, &os2msg, pMsg, isUnicode, (fRemove & PM_REMOVE_W) ? MSG_REMOVE : MSG_NOREMOVE);
  //TODO: This is not safe! There's no guarantee this message will be dispatched and it might overwrite a previous message
  if(fRemove & PM_REMOVE_W) {
        memcpy(MsgThreadPtr, &os2msg, sizeof(QMSG));
  }

  if(pMsg->message <= WINWM_KEYLAST && pMsg->message >= WINWM_KEYDOWN)
  {
    	if(ProcessKbdHook(pMsg, fRemove))
      		goto continuepeekmsg;
  }

  return rc;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinQueryMsgTime()
{
  return WinQueryMsgTime(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinWaitMessage()
{
  return WinWaitMsg(GetThreadHAB(), 0, 0);
}
//******************************************************************************
//TODO: QS_HOTKEY
//******************************************************************************
ULONG OSLibWinQueryQueueStatus()
{
 ULONG statusOS2, statusWin32 = 0;

   statusOS2 = WinQueryQueueStatus(HWND_DESKTOP);

   if(statusOS2 & QS_KEY)
    statusWin32 |= QS_KEY_W;
   if(statusOS2 & QS_MOUSEBUTTON)
    statusWin32 |= QS_MOUSEBUTTON_W;
   if(statusOS2 & QS_MOUSEMOVE)
    statusWin32 |= QS_MOUSEMOVE_W;
   if(statusOS2 & QS_TIMER)
    statusWin32 |= QS_TIMER_W;
   if(statusOS2 & QS_PAINT)
    statusWin32 |= QS_PAINT_W;
   if(statusOS2 & QS_POSTMSG)
    statusWin32 |= QS_POSTMESSAGE_W;
   if(statusOS2 & QS_SENDMSG)
    statusWin32 |= QS_SENDMESSAGE_W;

   return statusWin32;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinInSendMessage()
{
   return WinInSendMsg(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
DWORD OSLibWinGetMessagePos()
{
 APIRET rc;
 POINTL ptl;

   rc = WinQueryMsgPos(GetThreadHAB(), &ptl);
   if(!rc) {
      return 0;
   }
   //convert to windows coordinates
   return MAKEULONG(ptl.x,mapScreenY(ptl.y));
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinGetMessageTime()
{
   return (LONG)WinQueryMsgTime(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinReplyMessage(ULONG result)
{
   return (BOOL)WinReplyMsg( NULLHANDLE, NULLHANDLE, HMQ_CURRENT, (MRESULT)result);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibSendMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode)
{
 POSTMSG_PACKET *packet = (POSTMSG_PACKET *)_smalloc(sizeof(POSTMSG_PACKET));

    packet->Msg    = msg;
    packet->wParam = wParam;
    packet->lParam = lParam;

    return (ULONG)WinSendMsg(hwnd, WIN32APP_POSTMSG, (MPARAM)((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA), (MPARAM)packet);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinBroadcastMsg(ULONG msg, ULONG wParam, ULONG lParam, BOOL fSend)
{
    return WinBroadcastMsg(HWND_DESKTOP, msg, (MPARAM)wParam, (MPARAM)lParam,
                           (fSend) ? BMSG_SEND : BMSG_POST);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibPostMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode)
{
 POSTMSG_PACKET *packet = (POSTMSG_PACKET *)_smalloc(sizeof(POSTMSG_PACKET));

    packet->Msg = msg;
    packet->wParam = wParam;
    packet->lParam = lParam;
    return WinPostMsg(hwnd, WIN32APP_POSTMSG, (MPARAM)((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA), (MPARAM)packet);
}
//******************************************************************************
BOOL    _System _O32_PostThreadMessage( DWORD, UINT, WPARAM, LPARAM );

inline BOOL O32_PostThreadMessage(DWORD a, UINT b, WPARAM c, LPARAM d)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_PostThreadMessage(a, b, c, d);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
BOOL OSLibPostThreadMessage(ULONG threadid, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode)
{
// THDB *thdb = GetTHDBFromThreadId(threadid);
 POSTMSG_PACKET *packet = (POSTMSG_PACKET *)_smalloc(sizeof(POSTMSG_PACKET));

//    if(thdb == NULL) {
//        dprintf(("OSLibPostThreadMessage: thread %x not found!", threadid));
//        return FALSE;
//    }
    dprintf(("PostThreadMessageA %x %x %x %x", threadid, msg, wParam, lParam));
    packet->Msg = msg;
    packet->wParam = wParam;
    packet->lParam = lParam;
    return O32_PostThreadMessage(threadid, WIN32APP_POSTMSG-OPEN32_MSGDIFF, ((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA), (LPARAM)packet);
}
//******************************************************************************
//******************************************************************************

