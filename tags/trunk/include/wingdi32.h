/* $Id: wingdi32.h,v 1.3 2001-03-27 20:46:34 sandervl Exp $ */

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

#endif //__WINGDI32_H__
