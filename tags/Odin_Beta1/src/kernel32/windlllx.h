/* $Id: windlllx.h,v 1.6 2001-08-06 19:36:42 sandervl Exp $ */

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
    Win32LxDll(HINSTANCE hInstance, WIN32DLLENTRY DllEntryPoint, PVOID pResData,
                   DWORD MajorImageVersion, DWORD MinorImageVersion,
                   DWORD Subsystem);
virtual ~Win32LxDll();

#ifdef DEBUG
virtual ULONG     AddRef(char *parentname = NULL);
#else
virtual ULONG     AddRef();
#endif
virtual ULONG     Release();

        void      setDllHandleOS2(HINSTANCE hInstanceOS2);

virtual BOOL      isPe2LxDll() const;
virtual BOOL      isLxDll() const;

static  Win32LxDll *findModuleByOS2Handle(HINSTANCE hinstance);

protected:
        DWORD     MajorImageVersion;
        DWORD     MinorImageVersion;
        DWORD     Subsystem;
private:
};

extern char *lpszCustomDllName;
extern DWORD dwOrdinalBase;

#endif //__WINDLLLX_H__
