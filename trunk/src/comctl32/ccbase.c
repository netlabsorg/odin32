/* $Id: ccbase.c,v 1.1 2000-02-22 17:11:38 cbratschi Exp $ */
/*
 * COMCTL32 Base Functions and Macros for all Controls
 *
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
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
  COMCTL32_HEADER *infoPtr = COMCTL32_Alloc(dwSize);

  if (!infoPtr) return NULL;

  setInfoPtr(hwnd,infoPtr);
  infoPtr->dwSize        = dwSize;
  infoPtr->iVersion      = 0;
  infoPtr->fUnicode      = IsWindowUnicode(hwnd);
  infoPtr->uNotifyFormat = sendNotifyFormat(GetParent(hwnd),hwnd,NF_QUERY);

  return infoPtr;
}

VOID doneControl(HWND hwnd)
{
  COMCTL32_HEADER *infoPtr = getInfoPtr(hwnd);

  COMCTL32_Free(infoPtr);
  setInfoPtr(hwnd,NULL);
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

    case WM_NOTIFY:
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

LRESULT sendNotify(HWND hwnd,UINT code)
{
  NMHDR nmhdr;

  nmhdr.hwndFrom = hwnd;
  nmhdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
  nmhdr.code     = code;

  return SendMessageA(hwnd,WM_NOTIFY,nmhdr.idFrom,(LPARAM)&nmhdr);
}

LRESULT sendNotifyFormat(HWND hwnd,HWND hwndFrom,LPARAM command)
{
  return SendMessageA(hwnd,WM_NOTIFYFORMAT,hwndFrom,command);
}
