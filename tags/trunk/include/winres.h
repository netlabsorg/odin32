/* $Id: winres.h,v 1.12 2000-05-28 16:40:24 sandervl Exp $ */

/*
 * Win32 to OS/2 resource conversion 
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINRES_H__
#define __WINRES_H__

PVOID WIN32API ConvertResourceToOS2(HINSTANCE hInstance, LPSTR restype, HRSRC hResource);
ULONG WIN32API QueryConvertedResourceSize(HINSTANCE hInstance, LPSTR restype, HRSRC hResource);

#endif
