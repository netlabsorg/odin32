/* $Id: oslibmsg.h,v 1.3 1999-09-05 15:53:08 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBMSG_H__
#define __OSLIBMSG_H__

#define MSG_NOREMOVE  0
#define MSG_REMOVE    1

ULONG TranslateWinMsg(ULONG msg);

#ifdef OS2DEF_INCLUDED
void  OS2ToWinMsgTranslate(QMSG *os2Msg, MSG *winMsg, BOOL isUnicode);
void  WinToOS2MsgTranslate(MSG *winMsg, QMSG *os2Msg, BOOL isUnicode);
#endif

BOOL  OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, BOOL isUnicode = FALSE);
BOOL  OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, BOOL fRemove, BOOL isUnicode = FALSE);
void  OSLibWinPostQuitMessage(ULONG nExitCode);
LONG  OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode = FALSE);

BOOL  OSLibInitMsgQueue();

#endif //__OSLIBMSG_H__
