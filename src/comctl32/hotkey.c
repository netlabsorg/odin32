/* $Id: hotkey.c,v 1.7 1999-09-20 15:59:27 cbratschi Exp $ */
/*
 * Hotkey control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * NOTES
 *   Development in progress.
 *
 * TODO:
 *   - keyboard messages
 */

#include "winbase.h"
#include "commctrl.h"
#include "hotkey.h"
#include <string.h>

#define HOTKEY_GetInfoPtr(hwnd) ((HOTKEY_INFO*)GetWindowLongA(hwnd,0))

static VOID
HOTKEY_Refresh(HWND hwnd)
{
  InvalidateRect(hwnd,NULL,FALSE);
}


static BYTE
HOTKEY_Check(HOTKEY_INFO *infoPtr,BYTE bfMods)
{
  if ((infoPtr->fwCombInv & HKCOMB_A && bfMods == HOTKEYF_ALT) ||
      (infoPtr->fwCombInv & HKCOMB_C && bfMods == HOTKEYF_CONTROL) ||
      (infoPtr->fwCombInv & HKCOMB_CA && bfMods == HOTKEYF_CONTROL | HOTKEYF_ALT) ||
      (infoPtr->fwCombInv & HKCOMB_NONE && bfMods == 0) ||
      (infoPtr->fwCombInv & HKCOMB_S && bfMods == HOTKEYF_SHIFT) ||
      (infoPtr->fwCombInv & HKCOMB_SA && bfMods == HOTKEYF_SHIFT | HOTKEYF_ALT) ||
      (infoPtr->fwCombInv & HKCOMB_SC && bfMods == HOTKEYF_SHIFT | HOTKEYF_CONTROL) ||
      (infoPtr->fwCombInv & HKCOMB_SCA && bfMods == HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_ALT))
    return infoPtr->fwModInv;
  else
    return bfMods;
}


static LRESULT
HOTKEY_SetHotKey(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);

  infoPtr->bVKHotKey = wParam & 0xFF;
  infoPtr->bfMods = HOTKEY_Check(infoPtr,(wParam & 0xFF00)>>8);

  HOTKEY_Refresh(hwnd);

  return 0;
}


static LRESULT
HOTKEY_GetHotKey(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);

  return MAKEWORD(infoPtr->bVKHotKey,infoPtr->bfMods);
}


static LRESULT
HOTKEY_SetRules(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);
  BYTE oldMods = infoPtr->bfMods;

  infoPtr->fwCombInv = wParam;
  infoPtr->fwModInv = lParam;

  infoPtr->bfMods = HOTKEY_Check(infoPtr,infoPtr->bfMods);
  if (infoPtr->bfMods != oldMods) HOTKEY_Refresh(hwnd);

  return 0;
}


static LRESULT
HOTKEY_Char(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL sysKey)
{
 //CB:
  return 0;
}

static LRESULT
HOTKEY_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr;
    TEXTMETRICA tm;
    HDC hdc;

    /* allocate memory for info structure */
    infoPtr = (HOTKEY_INFO *)COMCTL32_Alloc (sizeof(HOTKEY_INFO));
    SetWindowLongA(hwnd,0,(DWORD)infoPtr);

    /* initialize info structure */

    infoPtr->hFont     = 0;
    infoPtr->bFocus    = FALSE;
    infoPtr->bVKHotKey = 0;
    infoPtr->bfMods    = 0;
    infoPtr->fwCombInv = 0;
    infoPtr->fwModInv  = 0;
    infoPtr->cursorPos.x = 3;
    infoPtr->cursorPos.y = 3;

    /* get default font height */
    hdc = GetDC (hwnd);
    GetTextMetricsA (hdc, &tm);
    infoPtr->nHeight = tm.tmHeight;
    ReleaseDC (hwnd, hdc);

    return 0;
}


static LRESULT
HOTKEY_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

    /* free hotkey info data */
    COMCTL32_Free (infoPtr);

    return 0;
}


static LRESULT
HOTKEY_EraseBackground (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush;
    RECT   rc;

    hBrush = (HBRUSH)SendMessageA(GetParent(hwnd),WM_CTLCOLOREDIT,wParam,(LPARAM)hwnd);
    if (!hBrush) hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    GetClientRect(hwnd,&rc);
    FillRect((HDC)wParam,&rc,hBrush);

    return -1;
}


static LRESULT
HOTKEY_GetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

    return infoPtr->hFont;
}


static LRESULT
HOTKEY_KeyDown (HWND hwnd, WPARAM wParam, LPARAM lParam,BOOL sysKey)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);
  INT newMods = sysKey ? HOTKEYF_ALT:0;

  switch (wParam)
  {
    case VK_RETURN:
    case VK_TAB:
    case VK_SPACE:
    case VK_DELETE:
    case VK_ESCAPE:
    case VK_BACK:
         return DefWindowProcA (hwnd,sysKey ? WM_SYSKEYDOWN:WM_KEYDOWN, wParam, lParam);

    case VK_SHIFT:
         newMods |= HOTKEYF_SHIFT;
         break;

    case VK_CONTROL:
         newMods |= HOTKEYF_CONTROL;
         break;

    default:
         break;
    }

    infoPtr->bfMods = newMods;
    HOTKEY_Refresh(hwnd);

    return TRUE;
}


static LRESULT
HOTKEY_KeyUp (HWND hwnd, WPARAM wParam, LPARAM lParam,BOOL sysKey)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

//    FIXME (hotkey, " %d\n", wParam);

  return 0;
}


static LRESULT
HOTKEY_KillFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

    infoPtr->bFocus = FALSE;
    DestroyCaret ();

    return 0;
}


static LRESULT
HOTKEY_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
/*    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd); */

    SetFocus (hwnd);

    return 0;
}


static LRESULT
HOTKEY_NCCreate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwExStyle = GetWindowLongA (hwnd, GWL_EXSTYLE);
    SetWindowLongA (hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_CLIENTEDGE);
    return DefWindowProcA (hwnd, WM_NCCREATE, wParam, lParam);
}


static VOID
HOTKEY_Draw(HWND hwnd,HDC hdc)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);
  RECT rect,newRect;
  char text[50];
  HFONT oldFont;

  HideCaret(hwnd);
  GetClientRect(hwnd,&rect);
  DrawEdge(hdc,&rect,EDGE_SUNKEN,BF_RECT | BF_ADJUST);

  //draw string
  text[0] = 0;
  if (infoPtr->bfMods & HOTKEYF_ALT) strcat(text,"ALT");
  if (infoPtr->bfMods & HOTKEYF_CONTROL)
  {
    if (text[0]) strcat(text,"+");
    strcat(text,"CONTROL");
  }
  if (infoPtr->bfMods & HOTKEYF_SHIFT)
  {
    if (text[0]) strcat(text,"+");
    strcat(text,"SHIFT");
  }

  if (infoPtr->bVKHotKey)
  {
    char char2[2];

    if (text[0]) strcat(text,"+");
    char2[0] = (char)infoPtr->bVKHotKey;
    char2[1] = 0;
    strcat(text,char2);
  }
  if(infoPtr->hFont) oldFont = SelectObject(hdc,infoPtr->hFont);
  SetBkMode(hdc,TRANSPARENT);
  CopyRect(&newRect,&rect);
  DrawTextA(hdc,text,strlen(text),&newRect,DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
  DrawTextA(hdc,text,strlen(text),&newRect,DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_CALCRECT);
  if (infoPtr->hFont) SelectObject(hdc,oldFont);
  infoPtr->cursorPos.x = (newRect.right >= rect.right)? 3:newRect.right;
  SetCaretPos(infoPtr->cursorPos.x,infoPtr->cursorPos.y);
  ShowCaret(hwnd);
}


static LRESULT
HOTKEY_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC         hdc;

  if (wParam == 0) hdc = BeginPaint(hwnd,&ps);
  else hdc = wParam;

  HOTKEY_Draw(hwnd,hdc);

  if (wParam == 0) EndPaint(hwnd,&ps);

  return 0;
}


static LRESULT
HOTKEY_SetFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

    infoPtr->bFocus = TRUE;

    CreateCaret(hwnd,(HBITMAP)0,1,infoPtr->nHeight);
    SetCaretPos(infoPtr->cursorPos.x,infoPtr->cursorPos.y);
    ShowCaret(hwnd);

    return 0;
}


static LRESULT
HOTKEY_SetFont (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);
    TEXTMETRICA tm;
    HDC hdc;
    HFONT hOldFont = 0;

    infoPtr->hFont = (HFONT)wParam;

    hdc = GetDC (hwnd);
    if (infoPtr->hFont)
        hOldFont = SelectObject (hdc, infoPtr->hFont);

    GetTextMetricsA (hdc, &tm);
    infoPtr->nHeight = tm.tmHeight;

    if (infoPtr->hFont)
        SelectObject (hdc, hOldFont);
    ReleaseDC (hwnd, hdc);

    if (LOWORD(lParam)) HOTKEY_Refresh(hwnd);

    return 0;
}


static LRESULT WINAPI
HOTKEY_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case HKM_GETHOTKEY:
            return HOTKEY_GetHotKey(hwnd,wParam,lParam);

        case HKM_SETHOTKEY:
            return HOTKEY_SetHotKey(hwnd,wParam,lParam);

        case HKM_SETRULES:
            return HOTKEY_SetRules(hwnd,wParam,lParam);

        case WM_CHAR:
            return HOTKEY_Char(hwnd,wParam,lParam,FALSE);

        case WM_SYSCHAR:
            return HOTKEY_Char(hwnd,wParam,lParam,TRUE);

        case WM_CREATE:
            return HOTKEY_Create (hwnd, wParam, lParam);

        case WM_DESTROY:
            return HOTKEY_Destroy (hwnd, wParam, lParam);

        case WM_ERASEBKGND:
            return HOTKEY_EraseBackground (hwnd, wParam, lParam);

        case WM_GETDLGCODE:
            return DLGC_WANTCHARS | DLGC_WANTARROWS;

        case WM_GETFONT:
            return HOTKEY_GetFont (hwnd, wParam, lParam);

        case WM_KEYDOWN:
            return HOTKEY_KeyDown(hwnd,wParam,lParam,FALSE);

        case WM_SYSKEYDOWN:
            return HOTKEY_KeyDown(hwnd,wParam,lParam,TRUE);

        case WM_KEYUP:
            return HOTKEY_KeyUp(hwnd,wParam,lParam,FALSE);

        case WM_SYSKEYUP:
            return HOTKEY_KeyUp(hwnd,wParam,lParam,TRUE);

        case WM_KILLFOCUS:
            return HOTKEY_KillFocus (hwnd, wParam, lParam);

        case WM_LBUTTONDOWN:
            return HOTKEY_LButtonDown (hwnd, wParam, lParam);

        case WM_NCCREATE:
            return HOTKEY_NCCreate (hwnd, wParam, lParam);

        case WM_PAINT:
            return HOTKEY_Paint(hwnd,wParam,lParam);

        case WM_SETFOCUS:
            return HOTKEY_SetFocus (hwnd, wParam, lParam);

        case WM_SETFONT:
            return HOTKEY_SetFont (hwnd, wParam, lParam);

        default:
//          if (uMsg >= WM_USER)
//              ERR (hotkey, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
HOTKEY_Register (VOID)
{
    WNDCLASSA wndClass;

    if (GlobalFindAtomA (HOTKEY_CLASSA)) return;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)HOTKEY_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(HOTKEY_INFO *);
    wndClass.hCursor       = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = HOTKEY_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
HOTKEY_Unregister (VOID)
{
    if (GlobalFindAtomA (HOTKEY_CLASSA))
        UnregisterClassA (HOTKEY_CLASSA, (HINSTANCE)NULL);
}

