/* $Id: windlllx.h,v 1.2 1999-09-18 17:45:23 sandervl Exp $ */

/*
 * Win32 LX Dll class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINDLLLX_H__
#define __WINDLLLX_H__

#include <windllbase.h>
#include <winimagelx.h>

class Win32LxDll : public Win32LxImage, public Win32DllBase
{
public:
	Win32LxDll(HINSTANCE hInstance, WIN32DLLENTRY DllEntryPoint, PVOID pResData);
virtual ~Win32LxDll();

virtual	ULONG     Release();

virtual	ULONG     getApi(char *name);
virtual ULONG     getApi(int ordinal);

virtual BOOL      isLxDll();

protected:
private:
};

#endif //__WINDLLLX_H__
