/* $Id: CCBase.h,v 1.10 2000-04-16 18:52:38 cbratschi Exp $ */
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
LRESULT sendNotify(HWND hwndFrom,HWND hwndTo,UINT code);
LRESULT sendNotify(HWND hwnd,UINT code,LPNMHDR nmhdr);
LRESULT sendNotify(HWND hwndFrom,HWND hwndTo,UINT code,LPNMHDR nmhdr);
LRESULT sendNotifyFormat(HWND hwnd,HWND hwndFrom,LPARAM command);
LRESULT sendCommand(HWND hwnd,UINT wNotifyCode);
LRESULT sendHScroll(HWND hwnd,UINT wNotifyCode);
LRESULT sendVScroll(HWND hwnd,UINT wNotifyCode);

HWND createToolTip(HWND hwnd,UINT flags,BOOL addtool);
VOID destroyToolTip(HWND hwndToolTip);

VOID drawStubControl(HWND hwnd,HDC hdc);

#define lstrlenAW(text,unicode) (unicode ? lstrlenW((WCHAR*)text):lstrlenA((CHAR*)text))

#define lstrcpyAW(textA,unicodeA,textB,unicodeB) (unicodeA ? (unicodeB ? lstrcpyW((WCHAR*)textA,(WCHAR*)textB):lstrcpyAtoW((WCHAR*)textA,(CHAR*)textB)):(unicodeB ? lstrcpyWtoA((CHAR*)textA,(WCHAR*)textB):lstrcpyA((CHAR*)textA,(CHAR*)textB)))

INT lstrcmpAtoW(CHAR *textA,WCHAR *textW);
INT lstrcmpAW(WCHAR *textA,BOOL textaunicode,WCHAR *textB,BOOL textbunicode);

//read note in CCBase.cpp!!
INT lstrcmpniA(CHAR *textA,CHAR *textB,INT len);
INT lstrcmpniAtoW(CHAR *textA,WCHAR* textB,INT len);
INT lstrcmpniW(WCHAR *textA,WCHAR *textB,INT len);
INT lstrcmpniAW(WCHAR *textA,BOOL unicodeA,WCHAR *textB,BOOL unicodeB,INT len);

CHAR*  lstrstrA(CHAR *text,CHAR *subtext);
WCHAR* lstrstrW(WCHAR *text,WCHAR *subtext);
CHAR*  lstrstrAtoW(CHAR *text,WCHAR *subtext);
WCHAR* lstrstrWtoA(WCHAR *text,CHAR *subtext);
WCHAR* lstrstrAW(WCHAR *text,BOOL textunicode,WCHAR *subtext,BOOL subtextunicode);

#define TICKDIFF(start,end) ((end > start) ? (end-start):(0xFFFFFFFF-start+end))

#endif
