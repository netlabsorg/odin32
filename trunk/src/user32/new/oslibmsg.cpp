/* $Id: oslibmsg.cpp,v 1.1 1999-07-17 11:52:22 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Simply copy for now. Probably need to make a real translation
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2.h>
#include <os2wrap.h>
#include <string.h>
#include <misc.h>
#include "oslibmsg.h"

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
