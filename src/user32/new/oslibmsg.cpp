/* $Id: oslibmsg.cpp,v 1.2 1999-07-20 07:42:35 sandervl Exp $ */
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
//******************************************************************************
ULONG TranslateWinMsg(ULONG msg)
{
  return 0;
}
//******************************************************************************
//******************************************************************************
void OSLibWinPostQuitMessage(ULONG nExitCode)
{
  WinPostQueueMsg(GetThreadMessageQueue(), WM_QUIT, (MPARAM)nExitCode, 0);
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
BOOL OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, BOOL isUnicode)
{
 BOOL rc;

  rc = WinGetMsg(GetThreadHAB(), MsgThreadPtr, TranslateWinMsg(uMsgFilterMin), TranslateWinMsg(uMsgFilterMax), 0);
  OS2ToWinMsgTranslate(MsgThreadPtr, pMsg, isUnicode);
  return rc;
}
//******************************************************************************
//******************************************************************************
