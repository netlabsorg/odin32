/* $Id: winimagelx.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

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
         Win32LxImage(HINSTANCE hInstance);
virtual ~Win32LxImage();

virtual HRSRC findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);
virtual ULONG getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);

virtual ULONG getVersionSize();
virtual BOOL  getVersionStruct(char *verstruct, ULONG bufLength);

protected:
private:
};

#endif //__WINIMAGELX_H__

