/* $Id: winmouse.h,v 1.6 2000-03-04 19:54:03 sandervl Exp $ */
/*
 * Mouse handler for DINPUT
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINMOUSE_H__
#define __WINMOUSE_H__

extern LPMOUSE_EVENT_PROC mouseHandler;
extern WNDPROC            keyboardHandler;

#define ISMOUSE_CAPTURED()	(mouseHandler!=NULL)
#define ISKDB_CAPTURED()        (keyboardHandler!=0)

#define MOUSEMSG_BUTTON	0
#define MOUSEMSG_MOVE   1

BOOL DInputMouseHandler(HWND hwnd, ULONG msg, ULONG x, ULONG y);
BOOL DInputKeyBoardHandler(MSG *msg);

#endif //__WINMOUSE_H__
