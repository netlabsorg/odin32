/* $Id: oslibmsg.h,v 1.1 1999-07-17 11:52:22 sandervl Exp $ */
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

#endif //__OSLIBMSG_H__
