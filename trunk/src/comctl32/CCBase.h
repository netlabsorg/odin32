/* $Id: CCBase.h,v 1.2 2000-02-23 17:09:40 cbratschi Exp $ */
/*
 * COMCTL32 Base Functions and Macros for all Controls
 *
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 */
#ifndef __CCBASE_H
#define __CCBASE_H

#define getInfoPtr(hwnd) ((COMCTL32_HEADER*)GetWindowLongA(hwnd,0))
#define setInfoPtr(hwnd,infoPtr) (SetWindowLongA(hwnd,0,(DWORD)infoPtr))

typedef struct
{
  DWORD dwSize;        //whole structure size
  INT   iVersion;      //version
  BOOL  fUnicode;      //Unicode flag
  UINT  uNotifyFormat; //notify format
} COMCTL32_HEADER;

PVOID initControl(HWND hwnd,DWORD dwSize);
VOID doneControl(HWND hwnd);

LRESULT defComCtl32ProcA(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT defComCtl32ProcW(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);

LRESULT sendNotify(HWND hwnd,UINT code);
LRESULT sendNotify(HWND hwnd,UINT code,LPNMHDR nmhdr);
LRESULT sendNotifyFormat(HWND hwnd,HWND hwndFrom,LPARAM command);
LRESULT sendCommand(HWND hwnd,UINT wNotifyCode);

HWND createToolTip(HWND hwnd,UINT flags);
VOID destroyToolTip(HWND hwndToolTip);

#endif
