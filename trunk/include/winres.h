/* $Id: winres.h,v 1.13 2000-11-09 18:23:01 sandervl Exp $ */

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
PVOID WIN32API ConvertCursorToOS2(LPVOID lpWinResData);
VOID  WIN32API FreeOS2Resource(LPVOID lpResource);

#endif
