/*
 * Internal functions exported by gdi32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINGDI32_H__
#define __WINGDI32_H__

#include <dcdata.h>

BOOL WIN32API setWinDeviceRegionFromPMDeviceRegion(HRGN winHrgn, HRGN pmHrgn, pDCData pHpsPmHrgn, HWND hwndPmHrgn);
VOID WIN32API UnselectGDIObjects(HDC hdc);
BOOL OSLibGpiSetCp(HDC hdc, ULONG codepage);
void WIN32API ConvertRGB555to565(LPVOID dest, LPVOID src, UINT imgsize);

#endif //__WINGDI32_H__
