/* $Id: oslibmsg.cpp,v 1.7 1999-11-05 09:16:22 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Simply copy for now. Need to make a real translation
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
};
#define MAX_MSGTRANSTAB (sizeof(MsgTransTab)/sizeof(MsgTransTab[0]))

QMSG *MsgThreadPtr = 0;

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
  int i;

  memcpy(os2Msg, winMsg, sizeof(MSG));
  os2Msg->hwnd = Win32Window::Win32ToOS2Handle(winMsg->hwnd);
  os2Msg->reserved = 0;
  for(i=0;i<MAX_MSGTRANSTAB;i++)
  {
    if(MsgTransTab[i].msgWin32 == winMsg->message)
    {
      os2Msg->msg = MsgTransTab[i].msgOS2;
      break;
    }
  }
}
//******************************************************************************
//******************************************************************************
void OS2ToWinMsgTranslate(QMSG *os2Msg, MSG *winMsg, BOOL isUnicode)
{
  int i;

  memcpy(winMsg, os2Msg, sizeof(MSG));
  winMsg->hwnd = Win32Window::OS2ToWin32Handle(os2Msg->hwnd);
  for(i=0;i<MAX_MSGTRANSTAB;i++)
  {
    if(MsgTransTab[i].msgOS2 == os2Msg->msg)
    {
      winMsg->message = MsgTransTab[i].msgWin32;
      break;
    }
  }
}
//******************************************************************************
//TODO!!!
//Signal that the incoming messages in pmwindow need to be translated
//(i.e. PM WM_CHAR when translated generates WM_CHAR messages, otherwise
// WM_KEYUP/DOWN (etc))
//******************************************************************************
ULONG TranslateWinMsg(ULONG msg)
{
 THDB *thdb;

  thdb = GetThreadTHDB();
  if(thdb) {
	thdb->fMsgTranslated = TRUE;
  }

  if(msg >= WIN32APP_USERMSGBASE)
    return msg - WIN32APP_USERMSGBASE;

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
 BOOL eaten = 0;

//TODO: What to do if app changed msg? (translate)
//  WinToOS2MsgTranslate(msg, &qmsg, isUnicode);

  //SvL: Some apps use PeeKMessage(remove) & DispatchMessage instead of 
  //     GetMessage/DispatchMessage
  if (MsgThreadPtr->msg == WM_TIMER)
      eaten = TIMER_HandleTimer (MsgThreadPtr);
  
  if(eaten)	return 0;

  return (LONG)WinDispatchMsg(GetThreadHAB(), MsgThreadPtr);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                    BOOL isUnicode)
{
 BOOL rc, eaten;

  do {
    eaten = FALSE;
    rc = WinGetMsg(GetThreadHAB(), MsgThreadPtr, TranslateWinMsg(uMsgFilterMin), TranslateWinMsg(uMsgFilterMax), 0);
    if (MsgThreadPtr->msg == WM_TIMER)
      eaten = TIMER_HandleTimer (MsgThreadPtr);
  } while (eaten);

  OS2ToWinMsgTranslate(MsgThreadPtr, pMsg, isUnicode);
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                      BOOL fRemove, BOOL isUnicode)
{
 BOOL rc;

  rc = WinPeekMsg(GetThreadHAB(), MsgThreadPtr, hwnd, TranslateWinMsg(uMsgFilterMin),
                  TranslateWinMsg(uMsgFilterMax), (fRemove == MSG_REMOVE) ? PM_REMOVE : PM_NOREMOVE);
  OS2ToWinMsgTranslate(MsgThreadPtr, pMsg, isUnicode);
  return rc;
}
//******************************************************************************
//******************************************************************************
