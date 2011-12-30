/*
 * Internal functions exported by user32
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINUSER32_H__
#define __WINUSER32_H__

#if !defined(NO_DCDATA) && (defined(INCL_GPI) && defined(OS2_INCLUDED))

#include <dcdata.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

int  WIN32API DIB_GetDIBWidthBytes( int width, int depth );
int  WIN32API BITMAP_GetWidthBytes( INT width, INT depth );

HWND WIN32API Win32ToOS2Handle(HWND hwndWin32);
HWND WIN32API Win32ToOS2FrameHandle(HWND hwndWin32);
HWND WIN32API OS2ToWin32Handle(HWND hwnd);

BOOL WIN32API IsSystemPen(HPEN hPen);
BOOL WIN32API IsSystemBrush(HBRUSH hBrush);

HICON WIN32API GetOS2Icon(HICON hIcon);

//Turns native OS/2 window into window recognized by Odin (with only very
//limited functionality)
//Useful for creating an Odin window with an OS/2 window as parent.
HWND WIN32API CreateFakeWindowEx(HWND hwndOS2, ATOM classAtom);
HWND WIN32API DestroyFakeWindow(HWND hwndWin32);

#define WIN32_APPEARANCE	0
#define OS2_APPEARANCE 		1  //OS/2 look with win32 system menu
#define OS2_APPEARANCE_SYSMENU	2  //OS/2 look with native system menu

void WIN32API SetWindowAppearance(int fOS2Looks);

BOOL WIN32API OSLibWinCreateObject(LPSTR pszPath, LPSTR pszArgs, LPSTR pszWorkDir, LPSTR pszName,
                                   LPSTR pszDescription, LPSTR pszIcoPath, INT iIcoNdx, BOOL fDesktop);

typedef BOOL (* WIN32API VISRGN_NOTIFY_PROC)(HWND hwnd, BOOL fDrawingAllowed, DWORD dwUserData);
//******************************************************************************
// WinSetVisibleRgnNotifyProc
//   To set a notification procedure for visible region changes of a specific window.
//   The procedure will be called when a WM_VRNENABLED message is posted
//   with ffVisRgnChanged set to TRUE
//
// Parameters:
//   HWND hwnd				window handle
//   VISRGN_NOTIFY_PROC lpNotifyProc	notification proc or NULL to clear proc
//   DWORD dwUserData			value used as 3rd parameter during
//                                      visible region callback
//
// NOTE: Internal API
//******************************************************************************
BOOL WIN32API WinSetVisibleRgnNotifyProc(HWND hwnd, VISRGN_NOTIFY_PROC lpNotifyProc, DWORD dwUserData);


//
// PM message definitions for Odin
//

//PostThreadMessage is done through Open32; which means the message id will be translated
//(0xc00 added)
#define OPEN32_MSGDIFF                  0xC00
#define WIN32APP_POSTMSG                (0x1000+OPEN32_MSGDIFF)
#define WIN32APP_FORWARDEDPOSTMSG       (0x1001+OPEN32_MSGDIFF)
#define WIN32APP_FORWARDEDPOSTMSG_MAGIC 0x12345677

//PM doesn't allow SetFocus during WM_SETFOCUS message processing; must delay
//this by posting a message
//NOTE Must be smaller than WIN32APP_POSTMSG!
#define WIN32APP_POSTPONEDESTROY  (WIN32APP_POSTMSG-2)
#define WIN32APP_DDRAWFULLSCREEN  (WIN32APP_POSTMSG-3)
#define WIN32APP_CHNGEFRAMECTRLS  (WIN32APP_POSTMSG-4)

#define WIN32MSG_MAGICA           0x12345678
#define WIN32MSG_MAGICW           0x12345679

typedef struct
{
   ULONG wParam;
   ULONG lParam;
} POSTMSG_PACKET;

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__WINUSER32_H__
