/* $Id: wingdi32.h,v 1.4 2001-08-02 14:49:03 sandervl Exp $ */

/*
 * Internal functions exported by gdi32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINGDI32_H__
#define __WINGDI32_H__

BOOL WIN32API setWinDeviceRegionFromPMDeviceRegion(HRGN winHrgn, HRGN pmHrgn, pDCData pHpsPmHrgn, HWND hwndPmHrgn);
VOID WIN32API UnselectGDIObjects(HDC hdc);
BOOL OSLibGpiSetCp(HDC hdc, ULONG codepage);

#endif //__WINGDI32_H__
