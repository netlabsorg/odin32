/* $Id: oslibmsg.cpp,v 1.15 1999-12-24 18:39:10 sandervl Exp $ */
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
 * TODO: Filter translation isn't correct for posted messages
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_DOSPROCESS
#include <os2.h>
#include <os2wrap.h>
#include <string.h>
#include <misc.h>
#include "oslibmsg.h"
#include <win32wnd.h>
#include "oslibutil.h"
#include "timer.h"
#include <thread.h>
#include <wprocess.h>
#include "pmwindow.h"
#include "oslibwin.h"

typedef BOOL (EXPENTRY FNTRANS)(MSG *, QMSG *);
typedef FNTRANS *PFNTRANS;

typedef struct
{
   ULONG   msgOS2;
   ULONG   msgWin32;
// PFNTRANS toOS2;
// PFNTRANS toWIN32;
} MSGTRANSTAB, *PMSGTRANSTAB;

MSGTRANSTAB MsgTransTab[] = {
   WM_NULL,          WINWM_NULL,
   WM_CREATE,        WINWM_CREATE,
   WM_DESTROY,       WINWM_DESTROY,
   WM_TIMER,         WINWM_TIMER,
   WM_CLOSE,         WINWM_CLOSE,
   WM_QUIT,          WINWM_QUIT,

   WM_ENABLE,        WINWM_ENABLE,
   WM_SHOW,          WINWM_SHOWWINDOW,
   WM_MOVE,          WINWM_MOVE,
   WM_SIZE,          WINWM_SIZE,
   //
   WM_HITTEST,       WINWM_NCHITTEST,
   //
   WM_ACTIVATE,      WINWM_ACTIVATE,
   WM_SETFOCUS,      WINWM_SETFOCUS,
   //
   WM_COMMAND,       WINWM_COMMAND,
   WM_SYSCOMMAND,    WINWM_SYSCOMMAND,
   //
   WM_PAINT,         WINWM_PAINT,
   WM_TIMER,         WINWM_TIMER,
   //
   WM_CLOSE,         WINWM_CLOSE,
   WM_QUIT,          WINWM_QUIT,
   //
   WM_CONTROL,       WINWM_COMMAND,
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
   0x020a, 0x020a,   // WM_???,             WM_???
   WM_CHAR,          WINWM_CHAR,

   //TODO: Needs better translation!
   WM_CHAR,          WINWM_KEYDOWN,
   WM_CHAR,          WINWM_KEYUP,
   WM_CHAR,          WINWM_SYSKEYDOWN,
   WM_CHAR,          WINWM_SYSKEYUP,
   WM_CHAR,          WINWM_KEYLAST
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
//TODO!!!
//Signal that the incoming messages in pmwindow need to be translated
//(i.e. PM WM_CHAR when translated generates WM_CHAR messages, otherwise
// WM_KEYUP/DOWN (etc))
//******************************************************************************
ULONG TranslateWinMsg(ULONG msg)
{
 POSTMSG_PACKET *packet;

  if(msg >= WINWM_USER)
    return WIN32APP_POSTMSG;

  for(int i=0;i<MAX_MSGTRANSTAB;i++)
  {
    if(MsgTransTab[i].msgWin32 == msg)
    {
      return MsgTransTab[i].msgOS2;
    }
  }

  return 0;
}
//******************************************************************************
//******************************************************************************
void OSLibWinPostQuitMessage(ULONG nExitCode)
{
 APIRET rc;

  rc = WinPostQueueMsg(NULLHANDLE, WM_QUIT, (MPARAM)nExitCode, 0);
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
        return (LONG)WinDispatchMsg(thdb->hab, &os2msg);
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

  thdb = GetThreadTHDB();
  if(thdb == NULL) {
        DebugInt3();
        return FALSE;
  }

  if(thdb->fTranslated && (!hwnd || hwnd == thdb->msgWCHAR.hwnd)) {
        thdb->fTranslated = FALSE;
        memcpy(pMsg, &thdb->msgWCHAR, sizeof(MSG));
        MsgThreadPtr->msg  = 0;
        MsgThreadPtr->hwnd = 0;
        return TRUE;
  }
  if(hwnd) {
        do {
            WinWaitMsg(thdb->hab, TranslateWinMsg(uMsgFilterMin), TranslateWinMsg(uMsgFilterMax));
            rc = OSLibWinPeekMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, PM_REMOVE_W, isUnicode);
        }
        while(rc == FALSE);
  }
  else
  {
    do {
        eaten = FALSE;
        rc = WinGetMsg(thdb->hab, MsgThreadPtr, TranslateWinMsg(uMsgFilterMin), TranslateWinMsg(uMsgFilterMax), 0);
        if (MsgThreadPtr->msg == WM_TIMER)
            eaten = TIMER_HandleTimer (MsgThreadPtr);
    } while (eaten);
  }
  OS2ToWinMsgTranslate((PVOID)thdb, MsgThreadPtr, pMsg, isUnicode);
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                     DWORD fRemove, BOOL isUnicode)
{
 BOOL  rc, eaten;
 THDB *thdb;
 QMSG  os2msg;

  thdb = GetThreadTHDB();
  if(thdb == NULL) {
        DebugInt3();
        return FALSE;
  }

  if(thdb->fTranslated && (!hwnd || hwnd == thdb->msgWCHAR.hwnd)) {
        if(fRemove == PM_REMOVE_W) {
            thdb->fTranslated = FALSE;
            MsgThreadPtr->msg  = 0;
            MsgThreadPtr->hwnd = 0;
        }
        memcpy(pMsg, &thdb->msgWCHAR, sizeof(MSG));
        return TRUE;
  }

  do {
        eaten = FALSE;
        rc = WinPeekMsg(thdb->hab, &os2msg, Win32BaseWindow::OS2ToWin32Handle(hwnd), TranslateWinMsg(uMsgFilterMin),
                        TranslateWinMsg(uMsgFilterMax), (fRemove == PM_REMOVE_W) ? PM_REMOVE : PM_NOREMOVE);

        if (rc && fRemove == PM_REMOVE_W && os2msg.msg == WM_TIMER)
            eaten = TIMER_HandleTimer(&os2msg);
  }
  while (eaten && rc);

  OS2ToWinMsgTranslate((PVOID)thdb, &os2msg, pMsg, isUnicode);
  //TODO: This is not safe! There's no guarantee this message will be dispatched and it might overwrite a previous message
  if(fRemove == PM_REMOVE_W) {
        memcpy(MsgThreadPtr, &os2msg, sizeof(QMSG));
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
   return MAKEULONG(ptl.x, ScreenHeight - ptl.y - 1);
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
