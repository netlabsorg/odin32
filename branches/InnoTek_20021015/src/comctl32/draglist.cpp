/* $Id: draglist.cpp,v 1.3 2001-10-23 21:42:23 sandervl Exp $ */
/*
 * Drag List control
 *
 * Copyright 1999 Eric Kohl
 *
 * NOTES
 *   This is just a dummy control. An author is needed! Any volunteers?
 *   I will only improve this control once in a while.
 *     Eric <ekohl@abo.rhein-zeitung.de>
 *
 * TODO:
 *   - Everything.
 */

/* WINE 991212 level */

#include "commctrl.h"
#include <winnt.h>
#include <misc.h>

static DWORD dwLastScrollTime = 0;

extern LPSTR COMCTL32_aSubclass; /* global subclassing atom */

typedef struct {
  WNDPROC lpOldWindowProc;
  BOOL    isUnicode;
  UINT    uMsg;
} DRAGLIST_INFO, *LPDRAGLIST_INFO;

VOID WINAPI DrawInsert (HWND hwndParent, HWND hwndLB, INT nItem)
{
  dprintf(("COMCTL32: DrawInsert - empty stub!"));
}


INT WINAPI LBItemFromPt (HWND hwndLB, POINT pt, BOOL bAutoScroll)
{
    RECT rcClient;
    INT nIndex;
    DWORD dwScrollTime;

    dprintf(("COMCTL32: LBItemFromPt"));

    ScreenToClient (hwndLB, &pt);
    GetClientRect (hwndLB, &rcClient);
    nIndex = (INT)SendMessageA (hwndLB, LB_GETTOPINDEX, 0, 0);

    if (PtInRect (&rcClient, pt))
    {
        /* point is inside -- get the item index */
        while (TRUE)
        {
            if (SendMessageA (hwndLB, LB_GETITEMRECT, nIndex, (LPARAM)&rcClient) == LB_ERR)
                return -1;

            if (PtInRect (&rcClient, pt))
                return nIndex;

            nIndex++;
        }
    }
    else
    {
        /* point is outside */
        if (!bAutoScroll)
            return -1;

        if ((pt.x > rcClient.right) || (pt.x < rcClient.left))
            return -1;

        if (pt.y < 0)
            nIndex--;
        else
            nIndex++;

        dwScrollTime = GetTickCount ();

        if ((dwScrollTime - dwLastScrollTime) < 200)
            return -1;

        dwLastScrollTime = dwScrollTime;

        SendMessageA (hwndLB, LB_SETTOPINDEX, (WPARAM)nIndex, 0);
    }

    return -1;
}


static LRESULT CALLBACK
DRAGLIST_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LPDRAGLIST_INFO lpDragInfo;
  LRESULT ret;
  DRAGLISTINFO draglist;

  lpDragInfo = (LPDRAGLIST_INFO)GetPropA(hwnd, (LPCSTR)COMCTL32_aSubclass);
  if(!lpDragInfo) {
      if(lpDragInfo->isUnicode) {
           return DefWindowProcW(hwnd, uMsg, wParam, lParam);
      }
      else return DefWindowProcA(hwnd, uMsg, wParam, lParam);
  }
  switch(uMsg) {
  case WM_LBUTTONDOWN:
      draglist.uNotification = DL_BEGINDRAG;
      draglist.hWnd = hwnd;
      draglist.ptCursor.x = LOWORD(lParam);
      draglist.ptCursor.y = HIWORD(lParam);
      ret = SendMessageA(GetParent(hwnd), (UINT)lpDragInfo->uMsg, GetWindowLongA(hwnd, GWL_ID), (LPARAM)&draglist);
      goto defaultmsg; //correct?

  case WM_NCDESTROY:
      if(lpDragInfo->isUnicode) {
           ret = CallWindowProcW(lpDragInfo->lpOldWindowProc, hwnd, uMsg, wParam, lParam);
      }
      else ret = CallWindowProcA(lpDragInfo->lpOldWindowProc, hwnd, uMsg, wParam, lParam);

      SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)lpDragInfo->lpOldWindowProc);
      SetPropA(hwnd, (LPCSTR)COMCTL32_aSubclass, NULL);
      COMCTL32_Free(lpDragInfo);
      break;

defaultmsg:
  default:
      if(lpDragInfo->isUnicode) {
           return CallWindowProcW(lpDragInfo->lpOldWindowProc, hwnd, uMsg, wParam, lParam);
      }
      else return CallWindowProcA(lpDragInfo->lpOldWindowProc, hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

BOOL WINAPI MakeDragList (HWND hwndLB)
{
  LPDRAGLIST_INFO lpDragInfo;

  dprintf(("COMCTL32: MakeDragList %x - partly implemented", hwndLB));

  lpDragInfo = (LPDRAGLIST_INFO)COMCTL32_Alloc(sizeof(DRAGLIST_INFO));
  if(SetPropA(hwndLB, (LPCSTR)COMCTL32_aSubclass, (HANDLE)lpDragInfo) == FALSE) {
      COMCTL32_Free(lpDragInfo);
      return FALSE;
  }
  lpDragInfo->lpOldWindowProc = (WNDPROC)SetWindowLongA(hwndLB, GWL_WNDPROC, (LONG)DRAGLIST_WindowProc);
  lpDragInfo->isUnicode       = IsWindowUnicode(hwndLB);
  lpDragInfo->uMsg            = RegisterWindowMessageA(DRAGLISTMSGSTRINGA);
  return TRUE;
}
