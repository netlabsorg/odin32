/*
 * Internal functions exported by user32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINUSER32_H__
#define __WINUSER32_H__

#ifndef NO_DCDATA

#include <dcdata.h>

ULONG OSLibGetScreenHeight();
ULONG OSLibGetScreenWidth();

void WIN32API Calculate1PixelDelta(pDCData pHps);
void WIN32API TestWideLine (pDCData pHps);
BOOL WIN32API changePageXForm(pDCData pHps, POINTL *pValue, int x, int y, POINTL *pPrev);
BOOL WIN32API setPageXForm(pDCData pHps);
VOID WIN32API selectClientArea(pDCData pHps);
VOID WIN32API checkOrigin(pDCData pHps);
VOID WIN32API removeClientArea(pDCData pHps);
LONG WIN32API clientHeight(HWND hwnd, pDCData pHps);
int  WIN32API setMapModeDC(pDCData pHps, int mode);
#endif

int  WIN32API DIB_GetDIBWidthBytes( int width, int depth );
int  WIN32API BITMAP_GetWidthBytes( INT width, INT depth );

HWND WIN32API Win32ToOS2Handle(HWND hwndWin32);
HWND WIN32API OS2ToWin32Handle(HWND hwnd);

BOOL WIN32API IsSystemPen(HPEN hPen);
BOOL WIN32API IsSystemBrush(HBRUSH hBrush);

HICON WIN32API GetOS2Icon(HICON hIcon);

//Turns native OS/2 window into window recognized by Odin (with only very
//limited functionality)
//Useful for creating an Odin window with an OS/2 window as parent.
HWND WIN32API CreateFakeWindowEx(HWND hwndOS2);

#define WIN32_APPEARANCE	0
#define OS2_APPEARANCE 		1
void WIN32API SetWindowAppearance(BOOL fOS2Looks);

BOOL WIN32API OSLibWinCreateObject(LPSTR pszPath, LPSTR pszArgs, LPSTR pszWorkDir, LPSTR pszName, 
                                   LPSTR pszDescription, LPSTR pszIcoPath, INT iIcoNdx, BOOL fDesktop);

#endif //__WINUSER32_H__
