/* $Id: hotkey.cpp,v 1.3 2001-03-31 13:25:26 sandervl Exp $ */
/*
 * Hotkey control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * Status: Complete
 * Version: 5.00
 */

#include "winbase.h"
#include "commctrl.h"
#include "ccbase.h"
#include "hotkey.h"
#include <string.h>
#include "comctl32.h"

#define HOTKEY_GetInfoPtr(hwnd) ((HOTKEY_INFO*)getInfoPtr(hwnd))

static VOID
HOTKEY_Refresh(HWND hwnd,BOOL notify)
{
  InvalidateRect(hwnd,NULL,FALSE);

  if (notify) sendCommand(hwnd,EN_CHANGE);
}

static char*
HOTKEY_ExtKey2Name(BYTE bVKHotKey)
{
  switch(bVKHotKey)
  {
    case VK_RETURN:
      return "RETURN";

    case VK_PRIOR:
      return "PRIOR";

    case VK_NEXT:
      return "NEXT";

    case VK_END:
      return "END";

    case VK_HOME:
      return "HOME";

    case VK_LEFT:
      return "LEFT";

    case VK_UP:
      return "UP";

    case VK_RIGHT:
      return "RIGHT";

    case VK_DOWN:
      return "DOWN";

    case VK_HELP:
      return "HELP";

    case VK_0:
      return "0";

    case VK_1:
      return "1";

    case VK_2:
      return "2";

    case VK_3:
      return "3";

    case VK_4:
      return "4";

    case VK_5:
      return "5";

    case VK_6:
      return "6";

    case VK_7:
      return "7";

    case VK_8:
      return "8";

    case VK_9:
      return "9";

    case VK_A:
      return "A";

    case VK_B:
      return "B";

    case VK_C:
      return "C";

    case VK_D:
      return "D";

    case VK_E:
      return "E";

    case VK_F:
      return "F";

    case VK_G:
      return "G";

    case VK_H:
      return "H";

    case VK_I:
      return "I";

    case VK_J:
      return "J";

    case VK_K:
      return "K";

    case VK_L:
      return "L";

    case VK_M:
      return "M";

    case VK_N:
      return "N";

    case VK_O:
      return "O";

    case VK_P:
      return "P";

    case VK_Q:
      return "Q";

    case VK_R:
      return "R";

    case VK_S:
      return "S";

    case VK_T:
      return "T";

    case VK_U:
      return "U";

    case VK_V:
      return "V";

    case VK_W:
      return "W";

    case VK_X:
      return "Y";

    case VK_Y:
      return "X";

    case VK_Z:
      return "Z";

    case VK_NUMPAD0:
      return "NUMPAD0";

    case VK_NUMPAD1:
      return "NUMPAD1";

    case VK_NUMPAD2:
      return "NUMPAD2";

    case VK_NUMPAD3:
      return "NUMPAD3";

    case VK_NUMPAD4:
      return "NUMPAD4";

    case VK_NUMPAD5:
      return "NUMPAD5";

    case VK_NUMPAD6:
      return "NUMPAD6";

    case VK_NUMPAD7:
      return "NUMPAD7";

    case VK_NUMPAD8:
      return "NUMPAD8";

    case VK_NUMPAD9:
      return "NUMPAD9";

    case VK_MULTIPLY:
      return "MULTIPLY";

    case VK_ADD:
      return "ADD";

    case VK_SEPARATOR:
      return "SEPARATOR";

    case VK_SUBTRACT:
      return "SUBTRACT";

    case VK_DECIMAL:
      return "DECIMAL";

    case VK_DIVIDE:
      return "DIVIDE";

    case VK_F1:
      return "F1";

    case VK_F2:
      return "F2";

    case VK_F3:
      return "F3";

    case VK_F4:
      return "F4";

    case VK_F5:
      return "F5";

    case VK_F6:
      return "F6";

    case VK_F7:
      return "F7";

    case VK_F8:
      return "F8";

    case VK_F9:
      return "F9";

    case VK_F10:
      return "F10";

    case VK_F11:
      return "F11";

    case VK_F12:
      return "F12";

    case VK_F13:
      return "F13";

    case VK_F14:
      return "F14";

    case VK_F15:
      return "F15";

    case VK_F16:
      return "F16";

    case VK_F17:
      return "F17";

    case VK_F18:
      return "F18";

    case VK_F19:
      return "F19";

    case VK_F20:
      return "F20";

    case VK_F21:
      return "F21";

    case VK_F22:
      return "F22";

    case VK_F23:
      return "F23";

    case VK_F24:
      return "F24";

    default:
      return NULL;
  }
}

static BYTE
HOTKEY_Check(HOTKEY_INFO *infoPtr,BYTE bfMods)
{
  BYTE mods = bfMods & ~HOTKEYF_EXT;

  if (((infoPtr->fwCombInv & HKCOMB_A) && (mods == HOTKEYF_ALT)) ||
      ((infoPtr->fwCombInv & HKCOMB_C) && (mods == HOTKEYF_CONTROL)) ||
      ((infoPtr->fwCombInv & HKCOMB_CA) && (mods == HOTKEYF_CONTROL | HOTKEYF_ALT)) ||
      ((infoPtr->fwCombInv & HKCOMB_NONE) && (mods == 0)) ||
      ((infoPtr->fwCombInv & HKCOMB_S) && (mods == HOTKEYF_SHIFT)) ||
      ((infoPtr->fwCombInv & HKCOMB_SA) && (mods == HOTKEYF_SHIFT | HOTKEYF_ALT)) ||
      ((infoPtr->fwCombInv & HKCOMB_SC) && (mods == HOTKEYF_SHIFT | HOTKEYF_CONTROL)) ||
      ((infoPtr->fwCombInv & HKCOMB_SCA) && (mods == HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_ALT)))
    return infoPtr->fwModInv | bfMods;
  else
    return bfMods;
}


static LRESULT
HOTKEY_SetHotKey(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);

  infoPtr->bVKHotKey = wParam & 0xFF;
  infoPtr->bfMods = HOTKEY_Check(infoPtr,(wParam & 0xFF00)>>8);

  HOTKEY_Refresh(hwnd,TRUE);

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
  if (infoPtr->bfMods != oldMods) HOTKEY_Refresh(hwnd,TRUE);

  return 0;
}


static LRESULT
HOTKEY_Char(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL sysKey)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);

  if (wParam != infoPtr->bVKHotKey)
  {
    infoPtr->bVKHotKey = wParam & 0xFF;
    infoPtr->bfMods &= ~HOTKEYF_EXT;
    infoPtr->bfMods = HOTKEY_Check(infoPtr,infoPtr->bfMods);
    HOTKEY_Refresh(hwnd,TRUE);
  }
  infoPtr->bfModsDown = 0;

  return 0;
}

static LRESULT
HOTKEY_Create (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr;
    TEXTMETRICA tm;
    HDC hdc;
    RECT rect;

    /* allocate memory for info structure */
    infoPtr = (HOTKEY_INFO*)initControl(hwnd,sizeof(HOTKEY_INFO));
    if (!infoPtr) return (LRESULT)-1;

    /* initialize info structure */

    infoPtr->hFont     = 0;
    infoPtr->bFocus    = FALSE;
    infoPtr->bVKHotKey = 0;
    infoPtr->bfMods    = 0;
    infoPtr->fwCombInv = 0;
    infoPtr->fwModInv  = 0;
    infoPtr->bfModsDown = 0;

    /* get default font height */
    hdc = GetDC (hwnd);
    GetTextMetricsA (hdc, &tm);
    infoPtr->nHeight = tm.tmHeight;
    ReleaseDC (hwnd, hdc);

    GetClientRect(hwnd,&rect);
    infoPtr->cursorPos.x = 0;
    infoPtr->cursorPos.y = rect.bottom-tm.tmHeight;

    return 0;
}


static LRESULT
HOTKEY_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    /* free hotkey info data */
    doneControl(hwnd);

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
    HideCaret(hwnd);
    FillRect((HDC)wParam,&rc,hBrush);
    ShowCaret(hwnd);

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
  INT newMods = (sysKey ? HOTKEYF_ALT:0) | infoPtr->bfModsDown;
  INT newKey = (infoPtr->bfModsDown == 0) ? 0:infoPtr->bVKHotKey;

  if (sysKey) infoPtr->bfModsDown |= HOTKEYF_ALT;
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
         infoPtr->bfModsDown |= HOTKEYF_SHIFT;
         break;

    case VK_CONTROL:
         newMods |= HOTKEYF_CONTROL;
         infoPtr->bfModsDown |= HOTKEYF_CONTROL;
         break;

    default:
         if ((newKey != wParam) && HOTKEY_ExtKey2Name(wParam))
         {
           infoPtr ->bVKHotKey = wParam & 0xFF;
           newMods |= HOTKEYF_EXT;

           infoPtr->bfMods = HOTKEY_Check(infoPtr,newMods);
           HOTKEY_Refresh(hwnd,TRUE);
           infoPtr->bfModsDown = 0;

           return 0;
         }
         break;
    }

    newMods = HOTKEY_Check(infoPtr,newMods);
    if (infoPtr->bfMods != newMods)
    {
      infoPtr->bVKHotKey = newKey;
      infoPtr->bfMods = newMods;
      HOTKEY_Refresh(hwnd,TRUE);
    }

    return 0;
}


static LRESULT
HOTKEY_KeyUp (HWND hwnd, WPARAM wParam, LPARAM lParam,BOOL sysKey)
{
  HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr(hwnd);
  INT newMods = infoPtr->bfModsDown;

  if (!infoPtr->bfModsDown) return 0;

  if (sysKey)
  {
    newMods &= ~HOTKEYF_ALT;
    infoPtr->bfModsDown &= ~HOTKEYF_ALT;
  }
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
         newMods &= ~HOTKEYF_SHIFT;
         infoPtr->bfModsDown &= ~HOTKEYF_SHIFT;
         break;

    case VK_CONTROL:
         newMods &= ~HOTKEYF_CONTROL;
         infoPtr->bfModsDown &= ~HOTKEYF_CONTROL;
         break;

    default:
         break;
    }

    newMods = HOTKEY_Check(infoPtr,newMods);
    if (infoPtr->bfMods != newMods)
    {
      infoPtr->bfMods = newMods;
      HOTKEY_Refresh(hwnd,TRUE);
    }

    return 0;
}


static LRESULT
HOTKEY_KillFocus (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HOTKEY_INFO *infoPtr = HOTKEY_GetInfoPtr (hwnd);

    infoPtr->bfModsDown = 0;
    infoPtr->bFocus = FALSE;
    DestroyCaret();

    return 0;
}


static LRESULT
HOTKEY_LButtonDown (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
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
    if (infoPtr->bfMods & HOTKEYF_EXT)
    {
      char* name = HOTKEY_ExtKey2Name(infoPtr->bVKHotKey);

      if (name)
      {
        if (text[0]) strcat(text,"+");
        strcat(text,name);
      }
    } else
    {
      char char2[2];

      if (text[0]) strcat(text,"+");
      char2[0] = (char)infoPtr->bVKHotKey;
      char2[1] = 0;
      strcat(text,char2);
    }
  } else if (infoPtr->bfModsDown) strcat(text,"+");
  if(infoPtr->hFont) oldFont = SelectObject(hdc,infoPtr->hFont);
  SetBkMode(hdc,TRANSPARENT);
  CopyRect(&newRect,&rect);
  DrawTextA(hdc,text,strlen(text),&newRect,DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOCLIP);
  DrawTextA(hdc,text,strlen(text),&newRect,DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_CALCRECT);
  if (infoPtr->hFont) SelectObject(hdc,oldFont);
  infoPtr->cursorPos.x = (newRect.right >= rect.right)? 0:newRect.right;
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

    if (LOWORD(lParam)) HOTKEY_Refresh(hwnd,FALSE);

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
            return defComCtl32ProcA (hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


VOID
HOTKEY_Register (VOID)
{
    WNDCLASSA wndClass;

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
    UnregisterClassA (HOTKEY_CLASSA, (HINSTANCE)NULL);
}

