/* $Id: flatsb.cpp,v 1.3 2000-03-21 17:30:41 cbratschi Exp $ */
/*
 * Flat Scrollbar control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1998 Alex Priem
 * Copyright 1999 Achim Hasenmueller
 *
 * NOTES
 *   This is just a dummy control. An author is needed! Any volunteers?
 *   I will only improve this control once in a while.
 *     Eric <ekohl@abo.rhein-zeitung.de>
 *
 * TODO:
 *   - All messages.
 *   - All notifications.
 *
 */

#include "winbase.h"
#include "commctrl.h"
#include "ccbase.h"
#include "flatsb.h"
#include <misc.h>

#define FlatSB_GetInfoPtr(hwnd) ((FLATSB_INFO*)getInfoPtr(hwnd))


BOOL WINAPI FlatSB_EnableScrollBar(HWND hwnd, INT dummy, UINT dummy2)
{
  dprintf(("COMCTL32: FlatSB_EnableScrollBar - empty stub!!!"));

  return 0;
}

BOOL WINAPI FlatSB_ShowScrollBar(HWND hwnd, INT code, BOOL flag)
{
  dprintf(("COMCTL32: FlatSB_ShowScrollBar - empty stub!!!"));

  return 0;
}

BOOL WINAPI FlatSB_GetScrollRange(HWND hwnd, INT code, LPINT min, LPINT max)
{
  dprintf(("COMCTL32: FlatSB_GetScrollRange - empty stub!!!"));

  return 0;
}

BOOL WINAPI FlatSB_GetScrollInfo(HWND hwnd, INT code, LPSCROLLINFO info)
{
  dprintf(("COMCTL32: FlatSB_GetScrollInfo - empty stub!!!"));

  return 0;
}

INT WINAPI FlatSB_GetScrollPos(HWND hwnd, INT code)
{
  dprintf(("COMCTL32: FlatSB_GetScrollPos - empty stub!!!"));

  return 0;
}

BOOL WINAPI FlatSB_GetScrollProp(HWND hwnd, INT propIndex, LPINT prop)
{
  dprintf(("COMCTL32: FlatSB_GetScrollProp - empty stub!!!"));

  return 0;
}


INT WINAPI FlatSB_SetScrollPos(HWND hwnd, INT code, INT pos, BOOL fRedraw)
{
  dprintf(("COMCTL32: FlatSB_SetScrollPos - empty stub!!!"));

  return 0;
}

INT WINAPI FlatSB_SetScrollInfo(HWND hwnd, INT code, LPSCROLLINFO info, BOOL fRedraw)
{
  dprintf(("COMCTL32: FlatSB_EnableScrollBar - empty stub!!!"));

  return 0;
}

INT WINAPI FlatSB_SetScrollRange(HWND hwnd, INT code, INT min, INT max, BOOL fRedraw)
{
  dprintf(("COMCTL32: FlatSB_SetScrollRange - empty stub!!!"));

  return 0;
}

BOOL WINAPI FlatSB_SetScrollProp(HWND hwnd, UINT index, INT newValue, BOOL flag)
{
  dprintf(("COMCTL32: FlatSB_SetScrollProp - empty stub!!!"));

  return 0;
}


BOOL WINAPI InitializeFlatSB(HWND hwnd)
{
  dprintf(("COMCTL32: FlatSB_InitializeFlatSB - empty stub!!!"));

  return 0;
}

HRESULT WINAPI UninitializeFlatSB(HWND hwnd)
{
  dprintf(("COMCTL32: FlatSB_UninitializeFlatSB - empty stub!!!"));

  return 0;
}



static LRESULT FLATSB_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  return 0;
}

static LRESULT FLATSB_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  return 0;
}

static VOID FLATSB_Draw(HWND hwnd,HDC hdc,RECT *updateRect)
{
  drawStubControl(hwnd,hdc);
}

static LRESULT FLATSB_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HDC hdc = (HDC)wParam;

  if (!hdc)
  {
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd,&ps);
    FLATSB_Draw(hwnd, hdc,&ps.rcPaint);
    EndPaint(hwnd,&ps);
  } else
      FLATSB_Draw(hwnd,hdc,NULL);

  return 0;
}

static LRESULT WINAPI FLATSB_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
      return FLATSB_Create (hwnd, wParam, lParam);

    case WM_DESTROY:
      return FLATSB_Destroy (hwnd, wParam, lParam);

    case WM_PAINT:
      return FLATSB_Paint(hwnd,wParam,lParam);

    default:
      //if (uMsg >= WM_USER)
      //    ERR_(datetime)("unknown msg %04x wp=%08x lp=%08lx\n",
      //         uMsg, wParam, lParam);
      return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
  }
  return 0;
}


VOID
FLATSB_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)FLATSB_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(FLATSB_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = FLATSB_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
FLATSB_Unregister (VOID)
{
    UnregisterClassA (FLATSB_CLASSA, (HINSTANCE)NULL);
}

