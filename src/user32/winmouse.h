/* $Id: winmouse.h,v 1.1 1999-11-08 13:44:41 sandervl Exp $ */
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

#include <win\mouse.h>

extern LPMOUSE_EVENT_PROC mouseHandler;

#define ISMOUSE_CAPTURED()	(mouseHandler!=NULL)

#define MOUSEMSG_BUTTON	0
#define MOUSEMSG_MOVE   1

BOOL DInputMouseHandler(HWND hwnd, ULONG msg, ULONG x, ULONG y, ULONG info);

#endif //__WINMOUSE_H__
