/* $Id: winimagelx.h,v 1.7 2002-11-18 13:53:55 sandervl Exp $ */

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

virtual ULONG     getApi(char *name);
virtual ULONG     getApi(int ordinal);

        LPVOID    buildHeader(DWORD MajorImageVersion, DWORD MinorImageVersion,
                              DWORD Subsystem);
        /** Get the OS/2 module handle.
         * @returns OS/2 module handle. */
        HINSTANCE   getHMOD() const { return hinstanceOS2; }
protected:
  LPVOID    header;
  HINSTANCE hinstanceOS2;
  DWORD     dwOrdinalBase;
  LPSTR     lpszExportPrefix;
private:
};

extern BYTE dosHeader[16*15];

#endif //__WINIMAGELX_H__

