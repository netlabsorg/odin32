/* $Id: win32wfake.h,v 1.3 2003-04-23 18:01:01 sandervl Exp $ */
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

#include "win32wbase.h"

class Win32FakeWindow : public Win32BaseWindow
{
public:
         Win32FakeWindow(HWND hwndOS2, ATOM classAtom);
virtual ~Win32FakeWindow();

virtual  BOOL   SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags, BOOL fShowWindow = FALSE);
virtual  PRECT  getWindowRect();

static Win32FakeWindow *GetWindowFromOS2Handle(HWND hwnd);
static void     init();
static char     szPropOdinHandle[64];
static BOOL     fInited;

protected:
private:
};


#endif //__WIN32WFAKE_H__
