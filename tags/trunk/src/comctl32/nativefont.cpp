/* $Id: nativefont.cpp,v 1.4 2001-03-31 13:25:26 sandervl Exp $ */
/*
 * Native Font control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 2000 Christoph Bratschi
 *
 * NOTES
 *   This is just a dummy control. An author is needed! Any volunteers?
 *   I will only improve this control once in a while.
 *     Eric <ekohl@abo.rhein-zeitung.de>
 *
 * TODO:
 *   - All messages.
 *   - All notifications.
 */

#include "winbase.h"
#include "commctrl.h"
#include "ccbase.h"
#include "nativefont.h"
#include "comctl32.h"

#define NATIVEFONT_GetInfoPtr(hwnd) ((NATIVEFONT_INFO*)getInfoPtr(hwnd))

static LRESULT
NATIVEFONT_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    NATIVEFONT_INFO *infoPtr;

    /* allocate memory for info structure */
    infoPtr = (NATIVEFONT_INFO*)initControl(hwnd,sizeof(NATIVEFONT_INFO));


    /* initialize info structure */


    return 0;
}


static LRESULT
NATIVEFONT_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    NATIVEFONT_INFO *infoPtr = NATIVEFONT_GetInfoPtr (hwnd);




    /* free comboex info data */
    doneControl(hwnd);

    return 0;
}

static VOID NATIVEFONT_Draw(HWND hwnd,HDC hdc,RECT *updateRect)
{
  drawStubControl(hwnd,hdc);
}

static LRESULT NATIVEFONT_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HDC hdc = (HDC)wParam;

  if (!hdc)
  {
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd,&ps);
    NATIVEFONT_Draw(hwnd, hdc,&ps.rcPaint);
    EndPaint(hwnd,&ps);
  } else
      NATIVEFONT_Draw(hwnd,hdc,NULL);

  return 0;
}

static LRESULT WINAPI NATIVEFONT_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
      return NATIVEFONT_Create(hwnd,wParam,lParam);

    case WM_DESTROY:
      return NATIVEFONT_Destroy(hwnd,wParam,lParam);

    case WM_PAINT:
      return NATIVEFONT_Paint(hwnd,wParam,lParam);

    default:
      return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
  }
  return 0;
}


VOID
NATIVEFONT_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)NATIVEFONT_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(NATIVEFONT_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = WC_NATIVEFONTCTLA;

    RegisterClassA (&wndClass);
}


VOID
NATIVEFONT_Unregister (VOID)
{
    UnregisterClassA (WC_NATIVEFONTCTLA, (HINSTANCE)NULL);
}

