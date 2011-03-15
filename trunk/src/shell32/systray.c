/*
 *  Systray
 *
 *  Copyright 1999 Kai Morich   <kai.morich@bigfoot.de>
 *
 *  Manage the systray window. That it actually appears in the docking
 *  area of KDE or GNOME is delegated to windows/x11drv/wnd.c,
 *  X11DRV_WND_DockWindow.
 *
 *  Modified by ErOs2 for usage with systray_os2.c
 *
 */

#ifndef __WIN32OS2__
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "winnls.h"
#include "shlobj.h"
#include "shellapi.h"
#include "shell32_main.h"
#include "commctrl.h"
#include "debugtools.h"
#include "heap.h"
#include "config.h"
#include "winuser32.h"

DEFAULT_DEBUG_CHANNEL(shell);



typedef struct SystrayItem {
  HWND                  hWnd;
  HWND                  hWndToolTip;
  NOTIFYICONDATAA       notifyIcon;
  struct SystrayItem    *nextTrayItem;
} SystrayItem;

static SystrayItem *systray=NULL;
static int firstSystray=TRUE; /* defer creation of window class until first systray item is created */

BOOL SYSTRAY_ItemInit(SystrayItem *ptrayItem);
void SYSTRAY_ItemTerm(SystrayItem *ptrayItem);
void SYSTRAY_ItemSetMessage(SystrayItem *ptrayItem, ULONG uCallbackMessage);
void SYSTRAY_ItemSetIcon(SystrayItem *ptrayItem, HICON hIcon);
void SYSTRAY_ItemSetTip(SystrayItem *ptrayItem, CHAR* szTip, int modify);


static BOOL SYSTRAY_ItemIsEqual(PNOTIFYICONDATAA pnid1, PNOTIFYICONDATAA pnid2)
{
  if (pnid1->hWnd != pnid2->hWnd) return FALSE;
  if (pnid1->uID  != pnid2->uID)  return FALSE;
  return TRUE;
}


static BOOL SYSTRAY_Add(PNOTIFYICONDATAA pnid)
{
  SystrayItem **ptrayItem = &systray;

  /* Find last element. */
  while( *ptrayItem ) {
    if ( SYSTRAY_ItemIsEqual(pnid, &(*ptrayItem)->notifyIcon) )
      return FALSE;
    ptrayItem = &((*ptrayItem)->nextTrayItem);
  }
  /* Allocate SystrayItem for element and add to end of list. */
  (*ptrayItem) = ( SystrayItem *)malloc( sizeof(SystrayItem) );

  /* Initialize and set data for the tray element. */
  SYSTRAY_ItemInit( (*ptrayItem) );
  (*ptrayItem)->notifyIcon.uID = pnid->uID; /* only needed for callback message */
  (*ptrayItem)->notifyIcon.hWnd = pnid->hWnd; /* only needed for callback message */
  SYSTRAY_ItemSetIcon   (*ptrayItem, (pnid->uFlags&NIF_ICON)   ?GetOS2Icon(pnid->hIcon)  :0);
  SYSTRAY_ItemSetMessage(*ptrayItem, (pnid->uFlags&NIF_MESSAGE)?pnid->uCallbackMessage:0);
  SYSTRAY_ItemSetTip    (*ptrayItem, (pnid->uFlags&NIF_TIP)    ?pnid->szTip           :"", FALSE);

  TRACE("SYSTRAY_Add %p: 0x%08x 0x%08x 0x%08x 0x%08x %s\n",  (*ptrayItem),
        (*ptrayItem)->notifyIcon.hWnd, (*ptrayItem)->notifyIcon.hIcon,
        pnid->uCallbackMessage, (*ptrayItem)->notifyIcon.uCallbackMessage,
        pnid->szTip );
  return TRUE;
}


static BOOL SYSTRAY_Modify(PNOTIFYICONDATAA pnid)
{
  SystrayItem *ptrayItem = systray;

  while ( ptrayItem ) {
    if ( SYSTRAY_ItemIsEqual(pnid, &ptrayItem->notifyIcon) ) {
      if (pnid->uFlags & NIF_ICON)
        SYSTRAY_ItemSetIcon(ptrayItem, GetOS2Icon(pnid->hIcon) );
      if (pnid->uFlags & NIF_MESSAGE)
        SYSTRAY_ItemSetMessage(ptrayItem, pnid->uCallbackMessage);
      if (pnid->uFlags & NIF_TIP)
        SYSTRAY_ItemSetTip(ptrayItem, pnid->szTip, TRUE);

      TRACE("SYSTRAY_Modify %p: 0x%08x %s\n", ptrayItem, ptrayItem->notifyIcon.hWnd, pnid->szTip);
      return TRUE;
    }
    ptrayItem = ptrayItem->nextTrayItem;
  }
  return FALSE; /* not found */
}


static BOOL SYSTRAY_Delete(PNOTIFYICONDATAA pnid)
{
  SystrayItem **ptrayItem = &systray;

  while (*ptrayItem) {
    if (SYSTRAY_ItemIsEqual(pnid, &(*ptrayItem)->notifyIcon)) {
      SystrayItem *next = (*ptrayItem)->nextTrayItem;
      TRACE("%p: 0x%08x %s\n", *ptrayItem, (*ptrayItem)->notifyIcon.hWnd, (*ptrayItem)->notifyIcon.szTip);
      SYSTRAY_ItemTerm(*ptrayItem);

      free(*ptrayItem);
      *ptrayItem = next;

      return TRUE;
    }
    ptrayItem = &((*ptrayItem)->nextTrayItem);
  }

  return FALSE; /* not found */
}

/*************************************************************************
 *
 */
BOOL SYSTRAY_Init(void)
{
  return TRUE;
}

/*************************************************************************
 * Shell_NotifyIconA            [SHELL32.297][SHELL32.296]
 */
BOOL WINAPI Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA pnid )
{
  BOOL flag=FALSE;
  TRACE("enter %d %d %ld\n", pnid->hWnd, pnid->uID, dwMessage);
  switch(dwMessage) {
  case NIM_ADD:
    flag = SYSTRAY_Add(pnid);
    break;
  case NIM_MODIFY:
    flag = SYSTRAY_Modify(pnid);
    break;
  case NIM_DELETE:
    flag = SYSTRAY_Delete(pnid);
    break;
  }
  TRACE("leave %d %d %ld=%d\n", pnid->hWnd, pnid->uID, dwMessage, flag);
  return flag;
}

/*************************************************************************
 * Shell_NotifyIconW            [SHELL32.298]
 */
BOOL WINAPI Shell_NotifyIconW (DWORD dwMessage, PNOTIFYICONDATAW pnid )
{
    BOOL ret;

    PNOTIFYICONDATAA p = HeapAlloc(GetProcessHeap(),0,sizeof(NOTIFYICONDATAA));
    memcpy(p, pnid, sizeof(NOTIFYICONDATAA));
        WideCharToMultiByte( CP_ACP, 0, pnid->szTip, -1, p->szTip, sizeof(p->szTip), NULL, NULL );
        p->szTip[sizeof(p->szTip)-1] = 0;

    ret = Shell_NotifyIconA(dwMessage, p );

    HeapFree(GetProcessHeap(),0,p);
    return ret;
}


#ifdef __WIN32OS2__

BOOL DoWin32PostMessage(HWND w, ULONG m, WPARAM mp1, LPARAM mp2 )
{
    TRACE("DoWin32WinPostMsg: HWND 0x%08x  MSG 0x%08x  ID 0x%08x  MMSG 0x%08x\n", w, m, mp1, mp2);
    if ( w && m )
    {
        return PostMessageA( w, m, mp1, mp2 );
    }
    return FALSE;
}

BOOL DoWin32CharToOem(const char *s, char *t)
{
	return CharToOemA( s , t );
}

#endif
