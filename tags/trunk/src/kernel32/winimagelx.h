/* $Id: winimagelx.h,v 1.2 2000-08-11 10:56:19 sandervl Exp $ */

/*
 * Win32 LX Image base class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGELX_H__
#define __WINIMAGELX_H__

#include <winimagebase.h>

class Win32LxImage : public virtual Win32ImageBase
{
public:
         Win32LxImage(HINSTANCE hInstance, PVOID pResData);
virtual ~Win32LxImage();

virtual	ULONG     getApi(char *name);
virtual ULONG     getApi(int ordinal);

        LPVOID    buildHeader(DWORD MajorImageVersion, DWORD MinorImageVersion,
                              DWORD Subsystem);

protected:
  LPVOID header;
  HINSTANCE hinstanceOS2;
private:
};

#endif //__WINIMAGELX_H__

