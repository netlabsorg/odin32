/* $Id: oslibmsg.cpp,v 1.3 1999-09-26 22:24:28 sandervl Exp $ */
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
  memcpy(os2Msg, winMsg, sizeof(MSG));
  os2Msg->reserved = 0;
}
//******************************************************************************
//******************************************************************************
void OS2ToWinMsgTranslate(QMSG *os2Msg, MSG *winMsg, BOOL isUnicode)
{
  memcpy(winMsg, os2Msg, sizeof(MSG));
  winMsg->hwnd = Win32Window::OS2ToWin32Handle(os2Msg->hwnd);
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
