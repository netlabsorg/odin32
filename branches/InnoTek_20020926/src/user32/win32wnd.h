/* $Id: win32wnd.h,v 1.4 2001-09-19 15:39:52 sandervl Exp $ */
/*
 * Win32 Window Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WND_H__
#define __WIN32WND_H__

#include <win32wbase.h>

//******************************************************************************
//******************************************************************************
class Win32Window : public Win32BaseWindow
{
public:
                Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode);
virtual        ~Win32Window();

private:
};

#endif //__WIN32WND_H__
