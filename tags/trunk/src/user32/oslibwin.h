/* $Id: oslibwin.h,v 1.50 2001-02-20 15:40:22 sandervl Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBWIN_H__
#define __OSLIBWIN_H__
#include <oslibgdi.h>

#ifndef WIN_INCLUDED
typedef struct _SWP    /* swp */
{
   ULONG   fl;
   LONG    cy;
   LONG    cx;
   LONG    y;
   LONG    x;
   HWND    hwndInsertBehind;
   HWND    hwnd;
   ULONG   ulReserved1;
   ULONG   ulReserved2;
} SWP, *PSWP;
#endif

//SvL: Must be the same as HWND_DESKTOP/OBJECT in pmwin.h!
#define OSLIB_HWND_DESKTOP      1
#define OSLIB_HWND_OBJECT       2

BOOL  OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw = TRUE);
BOOL  OSLibWinSetOwner(HWND hwnd, HWND hwndOwner);


HWND  OSLibWinCreateWindow(HWND hwndParent,ULONG dwWinStyle,
                           char *pszName, HWND Owner, ULONG fBottom,
                           ULONG id, BOOL fTaskList,BOOL fShellPosition, int classStyle);

BOOL  OSLibWinConvertStyle(ULONG dwStyle, ULONG dwExStyle, ULONG *OSWinStyle);
void  OSLibSetWindowStyle(HWND hwnd, ULONG dwStyle, BOOL fTaskList);
DWORD OSLibQueryWindowStyle(HWND hwnd);

#define OSLIB_QWL_USER -4

BOOL  OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value);
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset);
BOOL  OSLibSetWindowID(HWND hwnd, ULONG value);

#define WAOS_WARNING                 0
#define WAOS_NOTE                    1
#define WAOS_ERROR                   2
#define WAOS_CWINALARMS             13     /* count of valid alarms     53304*/

BOOL OSLibWinAlarm(HWND hwndDeskTop,ULONG flStyle);
ULONG OSLibDosBeep(ULONG freg,ULONG dur);
HWND OSLibWinQueryFocus(HWND hwndDeskTop);

#define OSLIB_FID_SYSMENU                0x8002
#define OSLIB_FID_TITLEBAR               0x8003
#define OSLIB_FID_MINMAX                 0x8004
#define OSLIB_FID_MENU                   0x8005
#define OSLIB_FID_VERTSCROLL             0x8006
#define OSLIB_FID_HORZSCROLL             0x8007
#define OSLIB_FID_CLIENT                 0x8008

HWND OSLibWinWindowFromID(HWND hwndParent,ULONG id);
BOOL OSLibWinSetFocus(HWND hwndDeskTop,HWND hwndNewFocus, BOOL activate);
ULONG OSLibGetWindowHeight(HWND hwnd); //for point transformation

extern ULONG ScreenHeight, ScreenWidth;
inline ULONG OSLibQueryScreenHeight(void) { return (ScreenHeight); }
inline ULONG OSLibQueryScreenWidth(void)  { return (ScreenWidth); }

//reserved deleted

#define SVOS_SWAPBUTTON              0
#define SVOS_DBLCLKTIME              1
#define SVOS_CXDBLCLK                2
#define SVOS_CYDBLCLK                3
#define SVOS_CXSIZEBORDER            4
#define SVOS_CYSIZEBORDER            5
#define SVOS_ALARM                   6
#define SVOS_CURSORRATE              9
#define SVOS_FIRSTSCROLLRATE         10
#define SVOS_SCROLLRATE              11
#define SVOS_NUMBEREDLISTS           12
#define SVOS_WARNINGFREQ             13
#define SVOS_NOTEFREQ                14
#define SVOS_ERRORFREQ               15
#define SVOS_WARNINGDURATION         16
#define SVOS_NOTEDURATION            17
#define SVOS_ERRORDURATION           18
#define SVOS_CXSCREEN                20
#define SVOS_CYSCREEN                21
#define SVOS_CXVSCROLL               22
#define SVOS_CYHSCROLL               23
#define SVOS_CYVSCROLLARROW          24
#define SVOS_CXHSCROLLARROW          25
#define SVOS_CXBORDER                26
#define SVOS_CYBORDER                27
#define SVOS_CXDLGFRAME              28
#define SVOS_CYDLGFRAME              29
#define SVOS_CYTITLEBAR              30
#define SVOS_CYVSLIDER               31
#define SVOS_CXHSLIDER               32
#define SVOS_CXMINMAXBUTTON          33
#define SVOS_CYMINMAXBUTTON          34
#define SVOS_CYMENU                  35
#define SVOS_CXFULLSCREEN            36
#define SVOS_CYFULLSCREEN            37
#define SVOS_CXICON                  38
#define SVOS_CYICON                  39
#define SVOS_CXPOINTER               40
#define SVOS_CYPOINTER               41
#define SVOS_DEBUG                   42
#define SVOS_CMOUSEBUTTONS           43
#define SVOS_CPOINTERBUTTONS         43
#define SVOS_POINTERLEVEL            44
#define SVOS_CURSORLEVEL             45
#define SVOS_TRACKRECTLEVEL          46
#define SVOS_CTIMERS                 47
#define SVOS_MOUSEPRESENT            48
#define SVOS_CXBYTEALIGN             49
#define SVOS_CXALIGN                 49
#define SVOS_CYBYTEALIGN             50
#define SVOS_CYALIGN                 50

/* The following value enables any greater value to be set by WinSetSysValue. */
/* This is to enable the setting of SV_EXTRAKEYBEEP by applications.          */

#define SVOS_NOTRESERVED             56
#define SVOS_EXTRAKEYBEEP            57

/* The following system value controls whether PM controls the keyboard      */
/* lights for light key keystrokes (else applications will)                  */
#define SVOS_SETLIGHTS               58
#define SVOS_INSERTMODE              59

#define SVOS_MENUROLLDOWNDELAY       64
#define SVOS_MENUROLLUPDELAY         65
#define SVOS_ALTMNEMONIC             66
#define SVOS_TASKLISTMOUSEACCESS     67
#define SVOS_CXICONTEXTWIDTH         68
#define SVOS_CICONTEXTLINES          69
#define SVOS_CHORDTIME               70
#define SVOS_CXCHORD                 71
#define SVOS_CYCHORD                 72
#define SVOS_CXMOTIONSTART           73
#define SVOS_CYMOTIONSTART           74
#define SVOS_BEGINDRAG               75
#define SVOS_ENDDRAG                 76
#define SVOS_SINGLESELECT            77
#define SVOS_OPEN                    78
#define SVOS_CONTEXTMENU             79
#define SVOS_CONTEXTHELP             80
#define SVOS_TEXTEDIT                81
#define SVOS_BEGINSELECT             82
#define SVOS_ENDSELECT               83
#define SVOS_BEGINDRAGKB             84
#define SVOS_ENDDRAGKB               85
#define SVOS_SELECTKB                86
#define SVOS_OPENKB                  87
#define SVOS_CONTEXTMENUKB           88
#define SVOS_CONTEXTHELPKB           89
#define SVOS_TEXTEDITKB              90
#define SVOS_BEGINSELECTKB           91
#define SVOS_ENDSELECTKB             92
#define SVOS_ANIMATION               93
#define SVOS_ANIMATIONSPEED          94
#define SVOS_MONOICONS               95
#define SVOS_KBDALTERED              96
#define SVOS_PRINTSCREEN             97
#define SVOS_LOCKSTARTINPUT          98
#define SVOS_CSYSVALUES              108

LONG OSLibWinQuerySysValue(LONG iSysValue);
ULONG OSLibWinQueryDlgItemText(HWND hwndDlg,ULONG idItem,LONG cchBufferMax,char* pchBuffer);
BOOL OSLibWinSetDlgItemText(HWND hwndDlg,ULONG idItem,char* pszText);
BOOL OSLibWinQueryPointerPos(PPOINT pptlPoint); //pptlPoint == POINTL pointer!

#define SWPOS_SIZE                   0x0001
#define SWPOS_MOVE                   0x0002
#define SWPOS_ZORDER                 0x0004
#define SWPOS_SHOW                   0x0008
#define SWPOS_HIDE                   0x0010
#define SWPOS_NOREDRAW               0x0020
#define SWPOS_NOADJUST               0x0040
#define SWPOS_ACTIVATE               0x0080
#define SWPOS_DEACTIVATE             0x0100
#define SWPOS_EXTSTATECHANGE         0x0200
#define SWPOS_MINIMIZE               0x0400
#define SWPOS_MAXIMIZE               0x0800
#define SWPOS_RESTORE                0x1000
#define SWPOS_FOCUSACTIVATE          0x2000
#define SWPOS_FOCUSDEACTIVATE        0x4000
#define SWPOS_NOAUTOCLOSE            0x8000    /* Valid in PROGDETAILS struct only */

#define HWNDOS_TOP                      (HWND)3
#define HWNDOS_BOTTOM                   (HWND)4

BOOL  OSLibWinSetMultWindowPos(struct _SWP *pswp, ULONG num);
BOOL  OSLibWinShowWindow(HWND hwnd, ULONG fl);

BOOL  OSLibWinDestroyWindow(HWND hwnd);

BOOL  OSLibWinIsIconic(HWND hwnd);
BOOL  OSLibWinIsChild (HWND hwnd, HWND hwndOf);
BOOL  OSLibWinSetActiveWindow(HWND hwnd);
BOOL  OSLibWinSetFocus(HWND hwnd);
BOOL  OSLibWinEnableWindow(HWND hwnd, BOOL fEnable);
BOOL  OSLibWinIsWindowEnabled(HWND hwnd);
BOOL  OSLibWinIsWindowVisible(HWND hwnd);
HWND  OSLibWinQueryActiveWindow();


#define RELATIVE_TO_WINDOW 0
#define RELATIVE_TO_SCREEN 1
BOOL  OSLibWinQueryWindowRect(Win32BaseWindow *window, PRECT pRect, int RelativeTo = RELATIVE_TO_WINDOW);
BOOL  OSLibWinCalcFrameRect(HWND hwnd, RECT *pRect, BOOL fClient);
BOOL  OSLibGetMinMaxInfo(HWND hwndFrame, MINMAXINFO *pMinMax);

#define QWOS_NEXT         0
#define QWOS_PREV         1
#define QWOS_TOP          2
#define QWOS_BOTTOM       3
#define QWOS_OWNER        4
#define QWOS_PARENT       5
#define QWOS_NEXTTOP      6
#define QWOS_PREVTOP      7
#define QWOS_FRAMEOWNER   8

HWND  OSLibWinQueryWindow(HWND hwnd, ULONG lCode);

LONG  OSLibWinQueryWindowTextLength(HWND hwnd);
LONG  OSLibWinQueryWindowText(HWND hwnd, LONG length, LPSTR lpsz);
BOOL  OSLibWinSetWindowText(HWND hwnd, LPSTR lpsz);
BOOL  OSLibWinFlashWindow(HWND hwnd, BOOL fFlash);
HWND  OSLibWinWindowFromPoint(HWND hwnd, PVOID ppoint);
BOOL  OSLibWinMinimizeWindow(HWND hwnd);

BOOL  OSLibWinGetBorderSize(HWND hwnd, OSLIBPOINT *pointl);
BOOL  OSLibWinSetIcon(HWND hwnd, HANDLE hIcon);

BOOL  OSLibWinQueryWindowPos (HWND hwnd, PSWP pswp);
void  OSLibMapSWPtoWINDOWPOS(PSWP pswp, PWINDOWPOS pwpos, PSWP pswpOld, 
                            int parentHeight, int clientOrgX, int clientOrgY, 
                            HWND hwnd);
void  OSLibMapWINDOWPOStoSWP(struct tagWINDOWPOS *pwpos, PSWP pswp, PSWP pswpOld, 
                             int parentHeight, int clientOrgX, int clientOrgY, HWND hFrame);

HWND  OSLibWinBeginEnumWindows(HWND hwnd);
HWND  OSLibWinGetNextWindow(HWND hwndEnum);
HWND  OSLibWinQueryClientWindow(HWND hwndFrame);
BOOL  OSLibWinEndEnumWindows(HWND hwndEnum);
BOOL  OSLibWinQueryWindowProcess(HWND hwnd, ULONG *pid, ULONG *tid);
BOOL  OSLibWinMapWindowPoints (HWND hwndFrom, HWND hwndTo, OSLIBPOINT *pptl, ULONG num);

HWND  OSLibWinObjectWindowFromID(HWND hwndOwner, ULONG ID);

HWND  OSLibWinQueryObjectWindow(VOID);

PVOID OSLibWinSubclassWindow(HWND hwnd,PVOID newWndProc);

BOOL OSLibSetWindowRestoreRect(HWND hwnd, PRECT pRect);
BOOL OSLibSetWindowMinPos(HWND hwnd, ULONG x, ULONG y);

BOOL  OSLibWinGetKeyboardStateTable(unsigned char * PMKeyState);
BOOL  OSLibWinSetKeyboardStateTable(unsigned char * PMKeyState);

#define HTOS_NORMAL                   0
#define HTOS_TRANSPARENT              (-1)
#define HTOS_DISCARD                  (-2)
#define HTOS_ERROR                    (-3)

BOOL  OSLibWinEnableWindowUpdate(HWND hwnd,BOOL fEnable);
ULONG OSLibWinGetLastError();

void  OSLibWinShowTaskList(HWND hwndFrame);

void  OSLibWinSetVisibleRegionNotify(HWND hwnd, BOOL fNotify);

HWND  OSLibWinQueryCapture();
BOOL  OSLibWinSetCapture(HWND hwnd);

BOOL   OSLibWinRemoveFromTasklist(HANDLE hTaskList);
HANDLE OSLibWinAddToTaskList(HWND hwndFrame, char *title, BOOL fVisible);
BOOL   OSLibWinChangeTaskList(HANDLE hTaskList, HWND hwndFrame, char *title, BOOL fVisible);

inline ULONG GetScreenHeight()
{
  return ScreenHeight;
}

inline ULONG GetScreenWidth()
{
  return ScreenWidth;
}

ULONG OSLibGetScreenHeight();
ULONG OSLibGetScreenWidth();

BOOL   OSLibWinLockWindowUpdate(HWND hwnd);
BOOL   OSLibWinGetMaxPosition(HWND hwndOS2, RECT *rect);

#endif //__OSLIBWIN_H__
