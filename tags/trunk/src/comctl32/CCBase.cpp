/* $Id: CCBase.cpp,v 1.3 2000-03-17 17:13:22 cbratschi Exp $ */
/*
 * COMCTL32 Base Functions and Macros for all Controls
 *
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * parts from WINE code
 */

#include "winbase.h"
#include "comctl32.h"
#include "ccbase.h"

BOOL checkVersion(INT iVersion)
{
  return TRUE; //CB: todo
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

LRESULT sendNotify(HWND hwnd,UINT code,LPNMHDR nmhdr)
{
  if (!nmhdr) return 0;

  nmhdr->hwndFrom = hwnd;
  nmhdr->idFrom   = GetWindowLongA(hwnd,GWL_ID);
  nmhdr->code     = code;

  return SendMessageA(getNotifyWindow(hwnd),WM_NOTIFY,nmhdr->idFrom,(LPARAM)nmhdr);
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

HWND createToolTip(HWND hwnd,UINT flags)
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

  ZeroMemory(&ti,sizeof(TTTOOLINFOA));
  ti.cbSize   = sizeof(TTTOOLINFOA);
  ti.uFlags   = flags;
  ti.hwnd     = hwnd;
  ti.uId      = 0;
  ti.lpszText = "";
  SetRectEmpty (&ti.rect);

  SendMessageA(hwndToolTip,TTM_ADDTOOLA,0,(LPARAM)&ti);

  return hwndToolTip;
}

VOID destroyToolTip(HWND hwndToolTip)
{
  if (hwndToolTip) DestroyWindow(hwndToolTip);
}
