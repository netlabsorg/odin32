/* $Id: win32wnd.h,v 1.3 2000-04-18 11:09:53 sandervl Exp $ */

/*
 * Win32 window utility procedures (in USER32)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WND_H__
#define __WIN32WND_H__

HWND Win32ToOS2Handle(HWND hwndWin32);
HWND OS2ToWin32Handle(HWND hwnd);

BOOL IsOS2FrameWindowHandle(HWND hwndWin32, HWND hwndOS2);

#endif //__WIN32WND_H__
