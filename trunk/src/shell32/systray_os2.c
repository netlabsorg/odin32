/*
 *  OS/2 System Tray support. For usage with SysTray/WPS by MadInt
 *  or System Tray widget for XCenter/eCenter.
 *
 *  author: ErOs2
 *  date: 1 May 2004
 *
 *  Notes: what's about HAB at lines 113, 118 ?
 *
 */


#define  INCL_WIN
#include <os2wrap.h>

#include <string.h>

#include <odin.h>
#include <winconst.h>


#define WM_TRAYADDME (WM_USER+1)
#define WM_TRAYDELME (WM_USER+2)
#define WM_TRAYICON  (WM_USER+3)
#define WM_TRAYEXIT  (0xCD20)

#define SZAPP       "SystrayServer"
#define SZTOPIC     "TRAY"


BOOL DoWin32PostMessage(HWND, ULONG, MPARAM, MPARAM);
BOOL DoWin32CharToOem(const char *s, char *t);

static HWND hwndTrayServer = 0;


typedef struct _NOTIFYICONDATAA
{   ULONG    cbSize;
    HWND     hWnd;
    ULONG    uID;
    ULONG    uFlags;
    ULONG    uCallbackMessage;
    HPOINTER hIcon;
    CHAR     szTip[64];
} NOTIFYICONDATAA, *PNOTIFYICONDATAA;


typedef struct SystrayItem {
  HWND                  hWndFrame;
  HWND                  hWndClient;
  NOTIFYICONDATAA       notifyIcon;
  struct SystrayItem    *nextTrayItem;
} SystrayItem;




static MRESULT EXPENTRY SYSTRAY_WndProc(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    SystrayItem *ptrayItem = (SystrayItem *)WinQueryWindowULong(hWnd, QWL_USER);

    switch ( msg )
    {
        case WM_CREATE:
            WinSetWindowULong(hWnd, QWL_USER, (ULONG)mp1);
            ptrayItem = (SystrayItem *)mp1;
            return (MRESULT)FALSE;
        case WM_DDE_INITIATEACK:
            hwndTrayServer = (HWND)mp1;
            if(hwndTrayServer)
                WinPostMsg(hwndTrayServer,WM_TRAYADDME,(MPARAM)hWnd,(MPARAM)NULL);
            return (MRESULT)TRUE;
        case WM_BUTTON1DBLCLK|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_LBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2DBLCLK|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_RBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3DBLCLK|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_MBUTTONDBLCLK_W);
            return (MRESULT)FALSE;
        case WM_BUTTON1UP|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_LBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2UP|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_RBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3UP|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_MBUTTONUP_W);
            return (MRESULT)FALSE;
        case WM_BUTTON1DOWN|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_LBUTTONDOWN_W);
            return (MRESULT)FALSE;
        case WM_BUTTON2DOWN|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_RBUTTONDOWN_W);
            return (MRESULT)FALSE;
        case WM_BUTTON3DOWN|0x2000:
            DoWin32PostMessage(ptrayItem->notifyIcon.hWnd, ptrayItem->notifyIcon.uCallbackMessage,
                            (MPARAM)ptrayItem->notifyIcon.uID, (MPARAM)WM_MBUTTONDOWN_W);
            return (MRESULT)FALSE;
    }
    return WinDefWindowProc( hWnd , msg , mp1 , mp2 );
}

//extern HAB hab;
const char * const systraysupclass = "Odin32SystraySupport";

BOOL SYSTRAY_RegisterClass(void)
{
    //WinRegisterClass( hab, systraysupclass, SYSTRAY_WndProc, 0, sizeof(ULONG) );
    WinRegisterClass(0, systraysupclass, SYSTRAY_WndProc, 0, sizeof(ULONG) );
    return TRUE;
}


BOOL SYSTRAY_ItemInit(SystrayItem *ptrayItem)
{
    ULONG fcf = FCF_TITLEBAR;

    if ( !SYSTRAY_RegisterClass() )
    {
        return FALSE;
    }


    memset( ptrayItem, 0, sizeof(SystrayItem) );
    ptrayItem->hWndFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &fcf, NULL,
                                               "", 0, NULLHANDLE, 0, NULL );
    if ( !ptrayItem->hWndFrame ) {
        return FALSE;
    }
    ptrayItem->hWndClient = WinCreateWindow(ptrayItem->hWndFrame, systraysupclass, "",
                                            0, 0, 0, 0, 0, ptrayItem->hWndFrame,
                                            HWND_TOP, FID_CLIENT, ptrayItem, NULL );
    if ( !ptrayItem->hWndClient ) {
        return FALSE;
    }

    WinDdeInitiate(ptrayItem->hWndClient,SZAPP,SZTOPIC,NULL);
    if (hwndTrayServer)
        WinPostMsg(hwndTrayServer,WM_TRAYICON,(MPARAM)ptrayItem->hWndClient,(MPARAM)NULL);

    return (hwndTrayServer!=NULLHANDLE);
}

void SYSTRAY_ItemTerm(SystrayItem *ptrayItem)
{
    WinDestroyWindow(ptrayItem->hWndFrame);
}


void SYSTRAY_ItemSetMessage(SystrayItem *ptrayItem, ULONG uCallbackMessage)
{
  ptrayItem->notifyIcon.uCallbackMessage = uCallbackMessage;
}


void SYSTRAY_ItemSetIcon(SystrayItem *ptrayItem, HPOINTER hIcon)
{
    WinSendMsg( ptrayItem->hWndFrame, WM_SETICON, MPFROMLONG( hIcon ), MPVOID );
    if (hwndTrayServer)
        WinPostMsg(hwndTrayServer,WM_TRAYICON,(MPARAM)ptrayItem->hWndClient,(MPARAM)NULL);
}


void SYSTRAY_ItemSetTip(SystrayItem *ptrayItem, CHAR* szTip, int modify)
{
	char tmp[ 64 ];
    strncpy(ptrayItem->notifyIcon.szTip, szTip, 64 );
    ptrayItem->notifyIcon.szTip[ 63 ] = 0;
    DoWin32CharToOem( ptrayItem->notifyIcon.szTip, tmp );
    WinSetWindowText( ptrayItem->hWndFrame, tmp );
    if (hwndTrayServer)
        WinPostMsg(hwndTrayServer,WM_TRAYICON,(MPARAM)ptrayItem->hWndClient,(MPARAM)NULL);
}

