/*
 *  Systray
 *
 *  Copyright 1999 Kai Morich   <kai.morich@bigfoot.de>
 *
 *  Manage the systray window. That it actually appears in the docking
 *  area of KDE or GNOME is delegated to windows/x11drv/wnd.c,
 *  X11DRV_WND_DockWindow.
 *
 *  Modified by ErOs2 and Dmitriy Kuminov for usage with systray_os2.c
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
#include "auxthread.h"

#include "systray.h"

DEFAULT_DEBUG_CHANNEL(shell);


struct _SystrayItem {
    HWND                  hWnd;
    HWND                  hWndToolTip;
    NOTIFYICONDATAA       notifyIcon;
    UINT                  uIdx;
    struct _SystrayItem   *nextTrayItem;
};

static SystrayItem *systray = NULL;

BOOL (*SYSTRAY_ItemInit)(SystrayItem *ptrayItem);
void (*SYSTRAY_ItemTerm)(SystrayItem *ptrayItem) = 0;
void (*SYSTRAY_ItemUpdate)(SystrayItem *ptrayItem, ULONG uFlags) = 0;

static BOOL SYSTRAY_ItemIsEqual(PNOTIFYICONDATAA pnid1, PNOTIFYICONDATAA pnid2)
{
    if (pnid1->hWnd != pnid2->hWnd) return FALSE;
    if (pnid1->uID  != pnid2->uID)  return FALSE;
    return TRUE;
}

static BOOL SYSTRAY_Add(PNOTIFYICONDATAA pnid)
{
    SystrayItem *pItem = systray, *pPrevItem = NULL, *ptrayItem;
    ULONG uIdx = 0;

    /* Search for the first free index and also check if already in list */
    while (pItem)
    {
        if (SYSTRAY_ItemIsEqual(pnid, &pItem->notifyIcon))
          return FALSE;

        uIdx = pPrevItem->uIdx + 1;

        if (pPrevItem && pItem->uIdx - pPrevItem->uIdx > 1)
        {
            /* found a hole in the index row, will insert here */
            break;
        }

        pPrevItem = pItem;
        pItem = pItem->nextTrayItem;
    }

    /* check the rest (if any) for duplicates */
    ptrayItem = pItem;
    while (ptrayItem)
    {
        if (SYSTRAY_ItemIsEqual(pnid, &ptrayItem->notifyIcon))
            return FALSE;
        ptrayItem = ptrayItem->nextTrayItem;
    }

    /* Allocate SystrayItem for the new element and insert it */
    ptrayItem = ( SystrayItem *)malloc(sizeof(SystrayItem));
    memset(ptrayItem, 0, sizeof(SystrayItem));
    ptrayItem->nextTrayItem = pItem;

    if (pPrevItem)
    {
        pPrevItem->nextTrayItem = ptrayItem;
        ptrayItem->uIdx = pPrevItem->uIdx + 1;
    }
    else
    {
        systray = ptrayItem;
        ptrayItem->uIdx = 0;
    }

    /* Initialize and set data for the tray element. */
    ptrayItem->uIdx = uIdx;

    ptrayItem->notifyIcon.uID = pnid->uID;
    ptrayItem->notifyIcon.hWnd = pnid->hWnd;

    ptrayItem->notifyIcon.hIcon =
        (pnid->uFlags & NIF_ICON) ? GetOS2Icon(pnid->hIcon) : 0;
    ptrayItem->notifyIcon.uCallbackMessage =
        (pnid->uFlags & NIF_MESSAGE) ? pnid->uCallbackMessage : 0;

    if (pnid->uFlags & NIF_TIP)
        CharToOemA(pnid->szTip, ptrayItem->notifyIcon.szTip);
    else
        ptrayItem->notifyIcon.szTip[0] = '\0';

    /* Implementation specific initialization */
    if (!SYSTRAY_ItemInit(ptrayItem))
    {
        free(ptrayItem);
        if (pPrevItem)
            pPrevItem->nextTrayItem = pItem;
        else
            systray = NULL;
        return FALSE;
    }

    /* Trigger the data update (flags = 0 means it's the first time) */
    SYSTRAY_ItemUpdate(ptrayItem, 0);

    TRACE("SYSTRAY_Add %p: uIdx %u, hWnd 0x%08x, uID %d, hIcon 0x%08x, "
          "uCallbackMessage 0x%08x, szTip [%s]\n",
          ptrayItem, ptrayItem->uIdx, ptrayItem->hWnd, ptrayItem->notifyIcon.uID,
          ptrayItem->notifyIcon.hIcon,ptrayItem->notifyIcon.uCallbackMessage,
          ptrayItem->notifyIcon.szTip);

    return TRUE;
}

static BOOL SYSTRAY_Modify(PNOTIFYICONDATAA pnid)
{
    SystrayItem *ptrayItem = systray;

    while (ptrayItem)
    {
        if (SYSTRAY_ItemIsEqual(pnid, &ptrayItem->notifyIcon))
        {
            if (pnid->uFlags & NIF_ICON)
                ptrayItem->notifyIcon.hIcon = GetOS2Icon(pnid->hIcon);
            if (pnid->uFlags & NIF_MESSAGE)
                ptrayItem->notifyIcon.uCallbackMessage = pnid->uCallbackMessage;
            if (pnid->uFlags & NIF_TIP)
                CharToOemA(pnid->szTip, ptrayItem->notifyIcon.szTip);

            SYSTRAY_ItemUpdate(ptrayItem, pnid->uFlags);

            TRACE("SYSTRAY_Modify %p: uIdx %u, hWnd 0x%08x, uID %d, hIcon 0x%08x, "
                  "uCallbackMessage 0x%08x, szTip [%s]\n",
                  ptrayItem, ptrayItem->uIdx, ptrayItem->hWnd, ptrayItem->notifyIcon.uID,
                  ptrayItem->notifyIcon.hIcon,ptrayItem->notifyIcon.uCallbackMessage,
                  ptrayItem->notifyIcon.szTip);

            return TRUE;
        }
        ptrayItem = ptrayItem->nextTrayItem;
    }

    return FALSE; /* not found */
}

static BOOL SYSTRAY_Delete(PNOTIFYICONDATAA pnid)
{
    SystrayItem **ptrayItem = &systray;

    while (*ptrayItem)
    {
        if (SYSTRAY_ItemIsEqual(pnid, &(*ptrayItem)->notifyIcon))
        {
            SystrayItem *next = (*ptrayItem)->nextTrayItem;

            TRACE("SYSTRAY_Delete %p: uIdx %u, hWnd 0x%08x, uID %d\n",
                  *ptrayItem, (*ptrayItem)->uIdx, (*ptrayItem)->notifyIcon.hWnd,
                  (*ptrayItem)->notifyIcon.uID);

            SYSTRAY_ItemTerm(*ptrayItem);

            free(*ptrayItem);

            *ptrayItem = next;

            return TRUE;
        }

        ptrayItem = &((*ptrayItem)->nextTrayItem);
    }

    return FALSE; /* not found */
}

#ifndef __WIN32OS2__
/*************************************************************************
 *
 */
BOOL SYSTRAY_Init(void)
{
    return TRUE;
}
#endif

/*************************************************************************
 *
 */
SystrayItem *SYSTRAY_FindItem(ULONG uIdx)
{
    SystrayItem *ptrayItem = systray;

    while (ptrayItem)
    {
        if (ptrayItem->uIdx == uIdx)
            return ptrayItem;

        ptrayItem = ptrayItem->nextTrayItem;
    }

    return NULL; /* not found */
}

/*************************************************************************
 *
 */
void SYSTRAY_PruneAllItems(void)
{
    SystrayItem *ptrayItem = systray;

    while (ptrayItem)
    {
        SystrayItem *next = ptrayItem->nextTrayItem;

        TRACE("SYSTRAY_PruneAllItems %p: uIdx %u, hWnd 0x%08x, uID %d\n",
              ptrayItem, ptrayItem->uIdx, ptrayItem->notifyIcon.hWnd,
              ptrayItem->notifyIcon.uID);

        SYSTRAY_ItemTerm(ptrayItem);

        free(ptrayItem);

        ptrayItem = next;
    }

    systray = NULL;
}

static PVOID Do_Shell_NotifyIconA(PVOID arg1, PVOID arg2,
                                  PVOID arg3, PVOID arg4)
{
    DWORD dwMessage = (DWORD)arg1;
    PNOTIFYICONDATAA pnid = (PNOTIFYICONDATAA)arg2;

    BOOL flag = FALSE;
    switch(dwMessage)
    {
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
    return (PVOID)flag;
}

/*************************************************************************
 * Shell_NotifyIconA            [SHELL32.297][SHELL32.296]
 */
BOOL WINAPI Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA pnid )
{
    PVOID ret;
    BOOL brc = RunOnAuxThreadAndWait(Do_Shell_NotifyIconA,
                                     (PVOID)dwMessage, (PVOID)pnid,
                                     NULL, NULL, &ret);
    if (brc)
        brc = (BOOL)ret;
    return brc;
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

#endif
