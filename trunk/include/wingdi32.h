/* $Id: wingdi32.h,v 1.1 2000-06-14 13:19:45 sandervl Exp $ */

/*
 * Internal functions exported by gdi32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINGDI32_H__
#define __WINGDI32_H__

BOOL setWinDeviceRegionFromPMDeviceRegion(HRGN winHrgn, HRGN pmHrgn, pDCData pHpsPmHrgn, HWND hwndPmHrgn);

#endif //__WINGDI32_H__
