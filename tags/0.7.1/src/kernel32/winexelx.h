/* $Id: winexelx.h,v 1.1 2000-03-10 16:12:02 sandervl Exp $ */

/*
 * Win32 LX Exe class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXELX_H__
#define __WINEXELX_H__

#include <winexebase.h>
#include <winimagelx.h>
#include <odinlx.h>

//Class for Odin executables (win32 apps compiled in OS/2 using the Odin32 api)
class Win32LxExe : public Win32LxImage, public Win32ExeBase
{
public:
	Win32LxExe(HINSTANCE hInstance, PVOID pResData);
virtual ~Win32LxExe();

virtual ULONG start();

protected:
private:
};

#endif //__WINEXELX_H__
