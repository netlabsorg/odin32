/* $Id: oslibmsg.h,v 1.2 1999-07-20 07:42:35 sandervl Exp $ */
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

ULONG TranslateWinMsg(ULONG msg);
void  OS2ToWinMsgTranslate(QMSG *os2Msg, MSG *winMsg, BOOL isUnicode);
void  WinToOS2MsgTranslate(MSG *winMsg, QMSG *os2Msg, BOOL isUnicode);

BOOL  OSLibInitMsgQueue();

#endif //__OSLIBMSG_H__
