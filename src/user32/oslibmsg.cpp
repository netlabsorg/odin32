/* $Id: oslibmsg.cpp,v 1.4 1999-10-15 10:03:14 sandervl Exp $ */
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

#define MAX_MSGTRANSTAB 12
MSGTRANSTAB MsgTransTab[MAX_MSGTRANSTAB] = {
   0x0000, 0x0000,  // WM_NULL,            WM_NULL
   0x0070, 0x0200,  // WM_MOUSEMOVE,       WM_MOUSEMOVE
   0x0071, 0x0201,  // WM_BUTTON1DOWN,     WM_LBUTTONDOWN
   0x0072, 0x0202,  // WM_BUTTON1UP,       WM_LBUTTONUP
   0x0073, 0x0203,  // WM_BUTTON1DBLCLK,   WM_LBUTTONDBLCLK
   0x0074, 0x0204,  // WM_BUTTON2DOWN,     WM_RBUTTONDOWN
   0x0075, 0x0205,  // WM_BUTTON2UP,       WM_RBUTTONUP
   0x0076, 0x0206,  // WM_BUTTON2DBLCLK,   WM_RBUTTONDBLCLK
   0x0077, 0x0207,  // WM_BUTTON3DOWN,     WM_MBUTTONDOWN
   0x0078, 0x0208,  // WM_BUTTON3UP,       WM_MBUTTONUP
   0x0079, 0x0209,  // WM_BUTTON3DBLCLK,   WM_MBUTTONDBLCLK
   0x020a, 0x020a,  // WM_???,             WM_???
};

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
  WinPostQueueMsg(NULLHANDLE, WM_QUIT, (MPARAM)nExitCode, 0);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode)
{
//TODO: What to do if app changed msg? (translate)
//  WinToOS2MsgTranslate(msg, &qmsg, isUnicode);

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
