/* $Id: wndmsg.h,v 1.6 2000-01-18 20:08:18 sandervl Exp $ */
/*
 * Win32 window message text function for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WNDMSG_H__
#define __WNDMSG_H__

#ifdef DEBUG
char *GetMsgText(int Msg);
void DebugPrintMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode, BOOL fInternalMsg);
#else
#define DebugPrintMessage(a, b, c, d, e, f)
#endif

#endif