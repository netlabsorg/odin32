/* $Id: windllpeldr.h,v 1.2 2001-06-15 09:42:49 bird Exp $ */

/*
 * Win32 PE loader Dll class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINDLLPELDR_H__
#define __WINDLLPELDR_H__

#include <windllbase.h>
#include <winimagepeldr.h>

class Win32PeLdrDll : public Win32PeLdrImage, public Win32DllBase
{
public:
    Win32PeLdrDll(char *szDllName, Win32ImageBase *parentImage = NULL);
virtual ~Win32PeLdrDll();

virtual BOOL  init(ULONG reservedMem);

        BOOL  isLxDll() const;
        BOOL  isPe2LxDll() const;

protected:
private:
};

#endif //__WINDLLPELDR_H__
