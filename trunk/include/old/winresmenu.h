/* $Id: winresmenu.h,v 1.1 1999-09-15 23:30:45 sandervl Exp $ */

/*
 * Win32 Menu resource class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINRESMENU_H__
#define __WINRESMENU_H__

#include <winres.h>

class Win32MenuRes : public Win32Resource
{
public:
                   // Constructors and destructors
                   Win32MenuRes(HWND hwndObjectMenu);
                   Win32MenuRes(Win32Image *module, HRSRC hRes, ULONG id, ULONG type);
                   Win32MenuRes(Win32Image *module, ULONG id, ULONG type,
	                        ULONG size, char *resdata);
		   Win32MenuRes(LPVOID menutemplate);

    virtual       ~Win32MenuRes();

protected:

private:
};


#endif //__WINRESMENU_H__
