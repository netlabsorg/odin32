/* $Id: oslibinput.cpp,v 1.4 2000-01-17 17:18:38 sandervl Exp $ */

#define INCL_WIN
#include <os2wrap.h>
#include <win32type.h>
#include <winconst.h>
#include "oslibinput.h"
#include <winkeyboard.h>

RECTL desktopRectl = {0};
ULONG ScreenWidth  = 0;
ULONG ScreenHeight = 0;

//******************************************************************************
//******************************************************************************
void OSLibInit()
{
   WinQueryWindowRect(HWND_DESKTOP, &desktopRectl);
   ScreenWidth  = desktopRectl.xRight;
   ScreenHeight = desktopRectl.yTop;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibGetDIState(DWORD len, LPVOID ptr)
{
 BYTE   PMKeyState[256];
 BYTE  *winkeybuf = (BYTE *)ptr;
 APIRET rc;

  rc = WinSetKeyboardStateTable( HWND_DESKTOP, (PBYTE)&PMKeyState, FALSE );

  if(rc == TRUE && len==256)
  {
      	KeyTranslatePMToWinBuf((BYTE *)&PMKeyState[0], (BYTE *)ptr, len);
  	for(int i=0;i<256;i++) {
    		winkeybuf[i] &= 0x80; //only high bit
  	}
      	return TRUE;
  }
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibQueryMousePos(DWORD *rx, DWORD *ry, DWORD *state)
{
 POINTL point;
 BOOL   rc;

  rc = WinQueryPointerPos(HWND_DESKTOP, &point);

  *rx = point.x;
  *ry = ScreenHeight - point.y;

  *state = 0;
  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON1) & 0x8000)
       *state |= MK_LBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON2) & 0x8000)
       *state |= MK_RBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_BUTTON3) & 0x8000)
       *state |= MK_MBUTTON_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)
       *state |= MK_SHIFT_W;
  if(WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)
       *state |= MK_CONTROL_W;

  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibMoveCursor(DWORD x, DWORD y)
{
  y = ScreenHeight - y;

  return WinSetPointerPos(HWND_DESKTOP, x, y);
}
//******************************************************************************
//******************************************************************************
