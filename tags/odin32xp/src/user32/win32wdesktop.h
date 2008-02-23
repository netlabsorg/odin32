/* $Id: win32wdesktop.h,v 1.9 2004-01-11 12:03:21 sandervl Exp $ */
/*
 * Win32 Desktop Window for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32WDESKTOP_H__
#define __WIN32WDESKTOP_H__

#if defined(__cplusplus)

#include <win32wbase.h>

class Win32Desktop : public Win32BaseWindow
{
public:
         Win32Desktop();
virtual ~Win32Desktop();

 virtual BOOL   EnableWindow(BOOL fEnable);
 virtual BOOL   DestroyWindow();
 virtual BOOL   isDesktopWindow();
 virtual HWND   GetWindow(UINT uCmd);

protected:
private:
};

/* Built-in class names (see _Undocumented_Windows_ p.418) */
#define DESKTOP_CLASS_NAMEA    "#32769"  /* Desktop */
#define DESKTOP_CLASS_NAMEW   L"#32769"  /* Desktop */
#undef DESKTOP_CLASS_ATOM
#define DESKTOP_CLASS_ATOM       32769   /* Desktop */

extern Win32Desktop *windowDesktop;
#endif //__cplusplus

BOOL CreateWin32Desktop();
void DestroyDesktopWindow();
BOOL DESKTOP_Register();
BOOL DESKTOP_Unregister();

#endif //__WIN32WDESKTOP_H__
