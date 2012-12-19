/* $Id: winexedummy.h,v 1.1 2002-02-03 13:16:22 sandervl Exp $ */

/*
 * Win32 LX Exe class (compiled in OS/2 using Odin32 api)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXEDUMMY_H__
#define __WINEXEDUMMY_H__

#include "winexebase.h"
#include "winimagelx.h"
#include <odinlx.h>

//Class for dummy Odin executables
class Win32DummyExe : public Win32ExeBase
{
public:
         Win32DummyExe(LPSTR pszExeName, PVOID pResData);
virtual ~Win32DummyExe();

virtual ULONG start();

virtual ULONG getApi(char *name);
virtual ULONG getApi(int ordinal);

protected:
        LPVOID buildHeader(DWORD MajorImageVersion, DWORD MinorImageVersion,
                           DWORD Subsystem);

private:
  LPVOID header;
};

#endif //__WINEXEDUMMY_H__
