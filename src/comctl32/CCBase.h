/* $Id: CCBase.h,v 1.6 2000-03-21 17:30:40 cbratschi Exp $ */
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
  HWND  hwndNotify;    //notify window
} COMCTL32_HEADER;

PVOID initControl(HWND hwnd,DWORD dwSize);
VOID doneControl(HWND hwnd);

LRESULT defComCtl32ProcA(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT defComCtl32ProcW(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);

BOOL isUnicodeNotify(COMCTL32_HEADER *infoPtr);
BOOL isUnicodeNotify(HWND hwnd);

HWND getNotifyWindow(COMCTL32_HEADER *infoPtr);
HWND getNotifyWindow(COMCTL32_HEADER *infoPtr);

LRESULT sendNotify(HWND hwnd,UINT code);
LRESULT sendNotify(HWND hwnd,UINT code,LPNMHDR nmhdr);
LRESULT sendNotifyFormat(HWND hwnd,HWND hwndFrom,LPARAM command);
LRESULT sendCommand(HWND hwnd,UINT wNotifyCode);
LRESULT sendHScroll(HWND hwnd,UINT wNotifyCode);
LRESULT sendVScroll(HWND hwnd,UINT wNotifyCode);

HWND createToolTip(HWND hwnd,UINT flags,BOOL addtool);
VOID destroyToolTip(HWND hwndToolTip);

VOID drawStubControl(HWND hwnd,HDC hdc);

#endif
