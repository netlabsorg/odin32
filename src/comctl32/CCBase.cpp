/* $Id: CCBase.cpp,v 1.7 2000-04-12 16:38:58 cbratschi Exp $ */
/*
 * COMCTL32 Base Functions and Macros for all Controls
 *
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * parts from WINE code
 */

#include <string.h>
#include <wcstr.h>
#include "winbase.h"
#include "comctl32.h"
#include "ccbase.h"

BOOL checkVersion(INT iVersion)
{
  return iVersion <= COMCTL32_VERSION;
}

//init-done

PVOID initControl(HWND hwnd,DWORD dwSize)
{
  COMCTL32_HEADER *infoPtr;

  if (dwSize < sizeof(COMCTL32_HEADER)) return NULL;

  infoPtr = (COMCTL32_HEADER*)COMCTL32_Alloc(dwSize);

  if (!infoPtr) return NULL;

  setInfoPtr(hwnd,infoPtr);
  ZeroMemory(infoPtr,dwSize);
  infoPtr->dwSize        = dwSize;
  infoPtr->iVersion      = 0;
  infoPtr->fUnicode      = IsWindowUnicode(hwnd);
  infoPtr->uNotifyFormat = sendNotifyFormat(GetParent(hwnd),hwnd,NF_QUERY);
  infoPtr->hwndNotify    = GetParent(hwnd);

  return infoPtr;
}

VOID doneControl(HWND hwnd)
{
  COMCTL32_HEADER *infoPtr = getInfoPtr(hwnd);

  if (infoPtr)
  {
    COMCTL32_Free(infoPtr);
    setInfoPtr(hwnd,NULL);
  }
}

//Default message handler

LRESULT defComCtl32Proc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
  COMCTL32_HEADER *infoPtr;

  switch (Msg)
  {
    case CCM_GETVERSION:
      infoPtr = getInfoPtr(hwnd);
      return infoPtr ? infoPtr->iVersion:0;

    case CCM_SETVERSION:
      infoPtr = getInfoPtr(hwnd);
      if (infoPtr)
      {
        if (checkVersion((INT)wParam))
        {
          INT oldVersion;

          oldVersion = infoPtr->iVersion;
          infoPtr->iVersion = (INT)wParam;
          return oldVersion;
        } else return -1;
      } else return 0;

    case CCM_GETUNICODEFORMAT:
      infoPtr = getInfoPtr(hwnd);
      return infoPtr ? infoPtr->fUnicode:IsWindowUnicode(hwnd);

    case CCM_SETUNICODEFORMAT:
      infoPtr = getInfoPtr(hwnd);
      if (infoPtr)
      {
        BOOL oldFormat;

        oldFormat = infoPtr->fUnicode;
        infoPtr->fUnicode = (INT)wParam;
        return oldFormat;
      } else return IsWindowUnicode(hwnd);

    case WM_NOTIFYFORMAT:
    {
      infoPtr = getInfoPtr(hwnd);

      if (!infoPtr) break;

      if (lParam == NF_REQUERY)
      {
        infoPtr->uNotifyFormat = sendNotifyFormat(GetParent(hwnd),hwnd,NF_QUERY);
        if ((infoPtr->uNotifyFormat != NFR_ANSI) && (infoPtr->uNotifyFormat != NFR_UNICODE))
          infoPtr->uNotifyFormat = IsWindowUnicode(GetParent(hwnd)) ? NFR_UNICODE:NFR_ANSI;
        return infoPtr->uNotifyFormat;
      } else if (lParam == NF_QUERY)
      {
        return infoPtr->uNotifyFormat;
      }
      break;
    }

    case CCM_SETNOTIFYWINDOW:
    {
      infoPtr = getInfoPtr(hwnd);

      if (!infoPtr) break;

      infoPtr->hwndNotify = (HWND)wParam;

      break;
    }
  }

  if (unicode)
    return DefWindowProcW(hwnd,Msg,wParam,lParam);
  else
    return DefWindowProcA(hwnd,Msg,wParam,lParam);
}

LRESULT defComCtl32ProcA(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  return defComCtl32Proc(hwnd,Msg,wParam,lParam,FALSE);
}

LRESULT defComCtl32ProcW(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  return defComCtl32Proc(hwnd,Msg,wParam,lParam,TRUE);
}

//Notifications

BOOL isUnicodeNotify(COMCTL32_HEADER *infoPtr)
{
  if (!infoPtr) return FALSE;

  return infoPtr->uNotifyFormat == NFR_UNICODE;
}

BOOL isUnicodeNotify(HWND hwnd)
{
  COMCTL32_HEADER *infoPtr = getInfoPtr(hwnd);

  return isUnicodeNotify(infoPtr);
}

HWND getNotifyWindow(COMCTL32_HEADER *infoPtr)
{
  if (!infoPtr) return 0;

  return infoPtr->hwndNotify;
}

HWND getNotifyWindow(HWND hwnd)
{
  COMCTL32_HEADER *infoPtr = getInfoPtr(hwnd);

  return getNotifyWindow(infoPtr);
}

LRESULT sendNotify(HWND hwnd,UINT code)
{
  NMHDR nmhdr;

  nmhdr.hwndFrom = hwnd;
  nmhdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
  nmhdr.code     = code;

  return SendMessageA(getNotifyWindow(hwnd),WM_NOTIFY,nmhdr.idFrom,(LPARAM)&nmhdr);
}

LRESULT sendNotify(HWND hwndFrom,HWND hwndTo,UINT code)
{
  NMHDR nmhdr;

  nmhdr.hwndFrom = hwndFrom;
  nmhdr.idFrom   = GetWindowLongA(hwndFrom,GWL_ID);
  nmhdr.code     = code;

  return SendMessageA(hwndTo,WM_NOTIFY,nmhdr.idFrom,(LPARAM)&nmhdr);
}

LRESULT sendNotify(HWND hwnd,UINT code,LPNMHDR nmhdr)
{
  if (!nmhdr) return 0;

  nmhdr->hwndFrom = hwnd;
  nmhdr->idFrom   = GetWindowLongA(hwnd,GWL_ID);
  nmhdr->code     = code;

  return SendMessageA(getNotifyWindow(hwnd),WM_NOTIFY,nmhdr->idFrom,(LPARAM)nmhdr);
}

LRESULT sendNotify(HWND hwndFrom,HWND hwndTo,UINT code,LPNMHDR nmhdr)
{
  if (!nmhdr) return 0;

  nmhdr->hwndFrom = hwndFrom;
  nmhdr->idFrom   = GetWindowLongA(hwndFrom,GWL_ID);
  nmhdr->code     = code;

  return SendMessageA(hwndTo,WM_NOTIFY,nmhdr->idFrom,(LPARAM)nmhdr);
}

LRESULT sendNotifyFormat(HWND hwnd,HWND hwndFrom,LPARAM command)
{
  return SendMessageA(hwnd,WM_NOTIFYFORMAT,hwndFrom,command);
}

LRESULT sendCommand(HWND hwnd,UINT wNotifyCode)
{
  return SendMessageA(getNotifyWindow(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),wNotifyCode),(LPARAM)hwnd);
}

LRESULT sendHScroll(HWND hwnd,UINT wNotifyCode)
{
  return SendMessageA(getNotifyWindow(hwnd),WM_HSCROLL,(WPARAM)wNotifyCode,(LPARAM)hwnd);
}

LRESULT sendVScroll(HWND hwnd,UINT wNotifyCode)
{
  return SendMessageA(getNotifyWindow(hwnd),WM_VSCROLL,(WPARAM)wNotifyCode,(LPARAM)hwnd);
}

//Tooltips

HWND createToolTip(HWND hwnd,UINT flags,BOOL addtool)
{
  HWND hwndToolTip;
  NMTOOLTIPSCREATED nmttc;
  TTTOOLINFOA ti;

  hwndToolTip =
    CreateWindowExA (0, TOOLTIPS_CLASSA, NULL, 0,
                     CW_USEDEFAULT, CW_USEDEFAULT,
                     CW_USEDEFAULT, CW_USEDEFAULT,
                     hwnd, 0, 0, 0);

  if (!hwndToolTip) return 0;

  /* Send NM_TOOLTIPSCREATED notification */
  nmttc.hwndToolTips = hwndToolTip;
  sendNotify(hwnd,NM_TOOLTIPSCREATED,&nmttc.hdr);

  if (addtool)
  {
    ZeroMemory(&ti,sizeof(TTTOOLINFOA));
    ti.cbSize   = sizeof(TTTOOLINFOA);
    ti.uFlags   = flags;
    ti.hwnd     = hwnd;
    ti.uId      = 0;
    ti.lpszText = "";
    SetRectEmpty (&ti.rect);

    SendMessageA(hwndToolTip,TTM_ADDTOOLA,0,(LPARAM)&ti);
  }

  return hwndToolTip;
}

VOID destroyToolTip(HWND hwndToolTip)
{
  if (hwndToolTip) DestroyWindow(hwndToolTip);
}

//stub control

VOID drawStubControl(HWND hwnd,HDC hdc)
{
  RECT rect;
  HBRUSH brush = CreateSolidBrush(RGB(0,0,255));
  HPEN pen = CreatePen(PS_SOLID,0,RGB(255,0,0)),oldPen;
  COLORREF oldColor;

  GetClientRect(hwnd,&rect);
  FillRect(hdc,&rect,brush);
  oldPen = SelectObject(hdc,pen);
  MoveToEx(hdc,0,0,NULL);
  LineTo(hdc,rect.right,rect.bottom);
  MoveToEx(hdc,rect.right,0,NULL);
  LineTo(hdc,0,rect.bottom);
  SelectObject(hdc,oldPen);
  oldColor = SetTextColor(hdc,RGB(255,255,255));
  DrawTextA(hdc,"Unimplemented Control!",-1,&rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
  SetTextColor(hdc,oldColor);

  DeleteObject(brush);
  DeleteObject(pen);
}

//string functions

//compare ANSI with UNICODE string
INT lstrcmpAtoW(CHAR* textA,WCHAR* textW)
{
  INT len,res;
  WCHAR* tmp;

  len = lstrlenA(textA);
  if (len > 0)
  {
    len++;
    tmp = (WCHAR*)COMCTL32_Alloc(len*sizeof(WCHAR));
    lstrcpyAtoW(tmp,textA);
  } else tmp = NULL;

  res = lstrcmpW(tmp,textW);

  if (tmp) COMCTL32_Free(tmp);
  return res;
}

INT lstrcmpAW(WCHAR *textA,BOOL textaunicode,WCHAR *textB,BOOL textbunicode)
{
  if (textaunicode)
  {
    if (textbunicode)
    {
      return lstrcmpW(textA,textB);
    } else
    {
      return lstrcmpAtoW((LPSTR)textB,textA);
    }
  } else
  {
    if (textbunicode)
    {
      return lstrcmpAtoW((LPSTR)textA,textB);
    } else
    {
      return lstrcmpA((LPSTR)textA,(LPSTR)textB);
    }
  }
}

CHAR* lstrstrA(CHAR *text,CHAR *subtext)
{
  return strstr(text,subtext);
}

WCHAR* lstrstrW(WCHAR *text,WCHAR *subtext)
{
  return (WCHAR*)wcswcs((const wchar_t*)text,(wchar_t*)subtext);
}

//NOTE: less information in ASCII subtext
CHAR* lstrstrAtoW(CHAR *text,WCHAR *subtext)
{
  INT len;
  CHAR *tmp,*res;

  len = lstrlenW(subtext);
  if (len > 0)
  {
    len++;
    tmp = (CHAR*)COMCTL32_Alloc(len);
    lstrcpyWtoA(tmp,subtext);
  } else tmp = NULL;

  res = strstr(text,tmp);

  if (tmp) COMCTL32_Free(tmp);
  return res;
}

WCHAR* lstrstrWtoA(WCHAR *text,CHAR *subtext)
{
  INT len;
  WCHAR *tmp,*res;

  len = lstrlenA(subtext);
  if (len > 0)
  {
    len++;
    tmp = (WCHAR*)COMCTL32_Alloc(len*sizeof(WCHAR));
    lstrcpyAtoW(tmp,subtext);
  } else tmp = NULL;

  res = (WCHAR*)wcswcs((const wchar_t*)text,(wchar_t*)tmp);

  if (tmp) COMCTL32_Free(tmp);
  return res;
}

WCHAR* lstrstrAW(WCHAR *text,BOOL textunicode,WCHAR *subtext,BOOL subtextunicode)
{
  if (textunicode)
  {
    if (subtextunicode)
    {
      return lstrstrW(text,subtext);
    } else
    {
      return lstrstrWtoA(text,(LPSTR)subtext);
    }
  } else
  {
    if (subtextunicode)
    {
      return (WCHAR*)lstrstrAtoW((LPSTR)text,subtext);
    } else
    {
      return (WCHAR*)lstrstrA((LPSTR)text,(LPSTR)subtext);
    }
  }
}
