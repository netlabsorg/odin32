/* $Id: windllpe2lx.h,v 1.2 1999-10-14 01:39:12 bird Exp $ */

/*
 * Win32 PE2LX Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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
    /** @cat Constructor/Destructor */
    Win32Pe2LxDll(HINSTANCE hinstance, BOOL fWin32k) throw(ULONG);
    virtual ~Win32Pe2LxDll();

    /** @cat Exports */
    virtual ULONG getApi(char *name);
    virtual ULONG getApi(int ordinal);

    /** @cat Query */
    virtual BOOL  isLxDll();
};

#endif