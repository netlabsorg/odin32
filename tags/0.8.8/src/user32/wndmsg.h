/* $Id: wndmsg.h,v 1.8 2004-01-30 22:10:24 bird Exp $ */
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_LOGGING
const char *GetMsgText(int Msg);

void DebugPrintMessage(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode, BOOL fInternalMsg);
#else
#define GetMsgText(Msg)     ""
#define DebugPrintMessage(a, b, c, d, e, f)
#endif

#ifdef __cplusplus
}
#endif

#endif
