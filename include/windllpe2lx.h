/* $Id: windllpe2lx.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

/*
 * Win32 PE2LX Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINDLLPE2LX_H__
#define __WINDLLPE2LX_H__

#include <windllbase.h>
#include <winimagepe2lx.h>

class Win32Pe2LxDll : public Win32Pe2LxImage, public Win32DllBase
{
public:
	Win32Pe2LxDll(HINSTANCE hinstance, int NameTableId, int Win32TableId, WIN32DLLENTRY DllEntryPoint);
virtual ~Win32Pe2LxDll();

virtual	ULONG getApi(char *name);
virtual ULONG getApi(int ordinal);

virtual BOOL  isLxDll();

protected:
private:
};

#endif