/* $Id: win32wfake.h,v 1.1 2003-03-20 13:20:46 sandervl Exp $ */
/*
 * Win32 Fake Window Class for OS/2
 *
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WFAKE_H__
#define __WIN32WFAKE_H__

#include <win32wbase.h>

class Win32FakeWindow : public Win32BaseWindow
{
public:
         Win32FakeWindow(HWND hwndOS2, ATOM classAtom);
virtual ~Win32FakeWindow();

virtual  BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags, BOOL fShowWindow = FALSE);
virtual  PRECT  getWindowRect();

protected:
private:
};


#endif //__WIN32WFAKE_H__
