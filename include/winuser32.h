/* $Id: winuser32.h,v 1.7 2000-12-30 13:26:53 sandervl Exp $ */

/*
 * Internal functions exported by user32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINUSER32_H__
#define __WINUSER32_H__

#include <dcdata.h>

ULONG OSLibGetScreenHeight();
ULONG OSLibGetScreenWidth();

void WIN32API Calculate1PixelDelta(pDCData pHps);
void WIN32API TestWideLine (pDCData pHps);
BOOL WIN32API changePageXForm(pDCData pHps, POINTL *pValue, int x, int y, POINTL *pPrev);
BOOL WIN32API setPageXForm(pDCData pHps);
VOID WIN32API selectClientArea(pDCData pHps);
VOID WIN32API removeClientArea(pDCData pHps);
LONG WIN32API clientHeight(HWND hwnd, pDCData pHps);
int  WIN32API setMapMode(pDCData pHps, int mode);

int  WIN32API DIB_GetDIBWidthBytes( int width, int depth );
int  WIN32API BITMAP_GetWidthBytes( INT width, INT depth );

HWND WIN32API Win32ToOS2Handle(HWND hwndWin32);
HWND WIN32API OS2ToWin32Handle(HWND hwnd);

BOOL WIN32API IsSystemPen(HPEN hPen);
BOOL WIN32API IsSystemBrush(HBRUSH hBrush);

#endif //__WINUSER32_H__
