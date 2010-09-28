/* $Id: oslibwin.cpp,v 1.148 2004-03-19 14:42:03 sandervl Exp $ */
/*
 * Window API wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Daniela Engert (dani@ngrt.de)
 * Copyright 2002-2003 Innotek Systemberatung GmbH
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_WINSWITCHLIST
#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <misc.h>
#include <win32type.h>
#include <winconst.h>
#include <winuser32.h>
#include <wprocess.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmsg.h"
#include "oslibgdi.h"
#include "pmwindow.h"
#include "initterm.h"

#define DBG_LOCALLOG    DBG_oslibwin
#include "dbglocal.h"

//Undocumented PM WS_TOPMOST style; similar to WS_EX_TOPMOST in Windows
#define WS_TOPMOST                 0x00200000L

//pmwindow.cpp
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetParent(HWND hwnd, HWND hwndParent, ULONG fRedraw)
{
  if(hwndParent == OSLIB_HWND_DESKTOP)
  {
        hwndParent = HWND_DESKTOP;
  }
  else
  if(hwndParent == OSLIB_HWND_OBJECT) {
        hwndParent = HWND_OBJECT;
  }
  return (WinSetParent(hwnd, hwndParent, fRedraw) == 0);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetOwner(HWND hwnd, HWND hwndOwner)
{
  return WinSetOwner(hwnd, hwndOwner);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinCreateWindow(HWND hwndParent,ULONG dwWinStyle, ULONG dwOSFrameStyle,
                          char *pszName, HWND Owner, ULONG fHWND_BOTTOM,
                          ULONG id, BOOL fTaskList,BOOL fShellPosition,
                          DWORD classStyle, HWND *hwndFrame)
{
    HWND  hwndClient;
    ULONG dwFrameStyle = 0;

    if(pszName && *pszName == 0) {
        pszName = NULL;
    }
    if(hwndParent == OSLIB_HWND_DESKTOP) {
        hwndParent = HWND_DESKTOP;
    }
    if(Owner == OSLIB_HWND_DESKTOP) {
        Owner = HWND_DESKTOP;
    }

    if(classStyle & CS_SAVEBITS_W) dwWinStyle |= WS_SAVEBITS;
    if(classStyle & CS_PARENTDC_W) dwWinStyle |= WS_PARENTCLIP;

    dwWinStyle = dwWinStyle & ~(WS_TABSTOP | WS_GROUP);

    if(fTaskList || hwndParent == HWND_DESKTOP)
    {
        dwFrameStyle |= FCF_NOMOVEWITHOWNER;
    }
    if (fShellPosition) dwFrameStyle |= FCF_SHELLPOSITION;

    FRAMECDATA FCData = {sizeof (FRAMECDATA), 0, 0, 0};
    FCData.flCreateFlags = dwFrameStyle;

    dprintf(("WinCreateWindow %x %s %x task %d shell %d classstyle %x winstyle %x bottom %d", hwndParent, pszName, id, fTaskList, fShellPosition, classStyle, dwWinStyle, fHWND_BOTTOM));
    dprintf(("WinCreateWindow parent %x, owner %x", hwndParent, Owner));

    //Must not use WS_CLIPCHILDREN style with frame window. Transparency won't work otherwise.
    //Eg: dialog parent, groupbox; invalidate part of groupbox -> painting algorithm stops when it finds
    //    a window with WS_CLIPCHILDREN -> result: dialog window won't update groupbox background as groupbox only draws the border
    *hwndFrame = WinCreateWindow(hwndParent,
                           WC_FRAME,
                           pszName, (dwWinStyle & ~WS_CLIPCHILDREN), 0, 0, 0, 0,
                           Owner, (fHWND_BOTTOM) ? HWND_BOTTOM : HWND_TOP,
                           id, (PVOID)&FCData, NULL);

    //We no longer register our own frame window class as there is code in PM
    //that makes assumptions about frame window class names.
    //Instead we subclass the frame window right after creating it.
    if(*hwndFrame) {
        WinSubclassWindow(*hwndFrame, Win32FrameWindowProc);
    }
    if(fOS2Look && *hwndFrame) {
        FCData.flCreateFlags = dwOSFrameStyle;
        WinCreateFrameControls(*hwndFrame, &FCData, NULL);
    }
    if(*hwndFrame == 0) {
        dprintf(("Frame window creation failed!! OS LastError %0x", WinGetLastError(0)));
        return 0;
    }
    hwndClient = WinCreateWindow (*hwndFrame, WIN32_STDCLASS,
                              NULL, dwWinStyle | WS_VISIBLE, 0, 0, 0, 0,
                              *hwndFrame, HWND_TOP, FID_CLIENT, NULL, NULL);
    //@PF For all top-level windows we create invisible vertical scroller
    //to show IBM wheel driver that we need WM_VSCROLL messages
    if (hwndParent == HWND_DESKTOP && *hwndFrame)
       OSLibWinCreateInvisibleScroller(*hwndFrame, SBS_VERT);

    if(hwndClient == 0) {
        dprintf(("Client window creation failed!! OS LastError %0x", WinGetLastError(0)));
    }
    return hwndClient;
}
//******************************************************************************
//Note: Also check OSLibSetWindowStyle when changing this!!
//******************************************************************************
BOOL OSLibWinConvertStyle(ULONG dwStyle, ULONG dwExStyle, ULONG *OSWinStyle,
                          ULONG *OSFrameStyle)
{
  *OSWinStyle   = 0;
  *OSFrameStyle = 0;

  /* Window styles */
  if(dwStyle & WS_DISABLED_W)
        *OSWinStyle |= WS_DISABLED;
  if(dwStyle & WS_CLIPSIBLINGS_W)
        *OSWinStyle |= WS_CLIPSIBLINGS;
  if(dwStyle & WS_CLIPCHILDREN_W)
        *OSWinStyle |= WS_CLIPCHILDREN;

  //Topmost child windows cause painting problems when moved in PM
  if(!(dwStyle & WS_CHILD_W) && dwExStyle & WS_EX_TOPMOST_W)
        *OSWinStyle |= WS_TOPMOST;

  //WS_EX_TOOLWINDOW is incompatible with the OS2Look (titlebar thinner + smaller font)
  if(fOS2Look && !(dwExStyle & WS_EX_TOOLWINDOW_W))
  {
      if((dwStyle & WS_CAPTION_W) == WS_CAPTION_W) {
          *OSFrameStyle = FCF_TITLEBAR;

          if((dwStyle & WS_SYSMENU_W) && !(dwExStyle & WS_EX_TOOLWINDOW_W))
          {
              *OSFrameStyle |= FCF_SYSMENU;
          }
          if(dwStyle & WS_MINIMIZEBOX_W) {
              *OSFrameStyle |= FCF_MINBUTTON;
          }
          if(dwStyle & WS_MAXIMIZEBOX_W) {
              *OSFrameStyle |= FCF_MAXBUTTON;
          }
          if(dwStyle & WS_SYSMENU_W) {
              *OSFrameStyle |= FCF_CLOSEBUTTON;
          }
      }
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinPositionFrameControls(HWND hwndFrame, RECTLOS2 *pRect, DWORD dwStyle,
                                   DWORD dwExStyle, HICON hSysMenuIcon,
                                   BOOL drawCloseButton, BOOL fClassIcon)
{
  SWP  swp[3];
  HWND hwndControl;
  int  i = 0;
  static int minmaxwidth  = 0;
  static int minmaxheight = 0;

  dprintf(("OSLibWinPositionFrameControls %x (%x,%x) %x %d", hwndFrame, dwStyle, dwExStyle, hSysMenuIcon, drawCloseButton));

  //WS_EX_TOOLWINDOW is incompatible with the OS2Look (titlebar thinner + smaller font)
  if(dwExStyle & WS_EX_TOOLWINDOW_W) {
      DebugInt3();
      return FALSE;
  }

  if(minmaxwidth == 0) {
      minmaxwidth  = WinQuerySysValue(HWND_DESKTOP, SV_CXMINMAXBUTTON);
      minmaxheight = WinQuerySysValue(HWND_DESKTOP, SV_CYMINMAXBUTTON);
  }

  if(fOS2Look == OS2_APPEARANCE_SYSMENU) {
      //Note: If no class icon *and* WS_EX_DLGMODALFRAME -> no system menu!!
      //      --> TODO
      hwndControl = WinWindowFromID(hwndFrame, FID_SYSMENU);
      if(hwndControl) {
          swp[i].hwnd = hwndControl;
          swp[i].hwndInsertBehind = HWND_TOP;
          swp[i].x  = pRect->xLeft;
          swp[i].y  = pRect->yBottom;
          if(pRect->yTop - pRect->yBottom > minmaxheight) {
              swp[i].y += pRect->yTop - pRect->yBottom - minmaxheight;
          }
          swp[i].cx = minmaxwidth/2;
          swp[i].cy = minmaxheight;;
          swp[i].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
          dprintf(("FID_SYSMENU (%d,%d)(%d,%d)", swp[i].x, swp[i].y, swp[i].cx, swp[i].cy));
          pRect->xLeft += minmaxwidth/2;
          i++;
      }
  }
  else
  //Note: If no class icon *and* WS_EX_DLGMODALFRAME -> no system menu!!
  if((dwStyle & WS_SYSMENU_W) && !(dwExStyle & WS_EX_TOOLWINDOW_W) &&
     !(!fClassIcon && (dwExStyle & WS_EX_DLGMODALFRAME_W)) && hSysMenuIcon)
  {
      pRect->xLeft += minmaxwidth/2;
  }

  if((dwStyle & WS_CAPTION_W) == WS_CAPTION_W) {
      hwndControl = WinWindowFromID(hwndFrame, FID_TITLEBAR);
      if(hwndControl) {
          swp[i].hwnd = hwndControl;
          swp[i].hwndInsertBehind = HWND_TOP;
          swp[i].x  = pRect->xLeft;
          swp[i].y  = pRect->yBottom;
          if(pRect->yTop - pRect->yBottom > minmaxheight) {
              swp[i].y += pRect->yTop - pRect->yBottom - minmaxheight;
          }
          swp[i].cx = pRect->xRight - pRect->xLeft;
          if((dwStyle & WS_MINIMIZEBOX_W)) {
              swp[i].cx -= minmaxwidth/2;
          }
          if((dwStyle & WS_MAXIMIZEBOX_W)) {
              swp[i].cx -= minmaxwidth/2;
          }
          //there is no close button in warp 3 and sometimes we do not
          //have close button as well
          if((dwStyle & WS_SYSMENU_W) && !fVersionWarp3 && drawCloseButton) {
              swp[i].cx -= minmaxwidth/2;
          }
          swp[i].cy = minmaxheight;
          swp[i].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
          dprintf(("FID_TITLEBAR (%d,%d)(%d,%d)", swp[i].x, swp[i].y, swp[i].cx, swp[i].cy));
          pRect->xLeft += swp[i].cx;
          i++;
      }
      else return FALSE; //no titlebar -> no frame controls
  }
  if((dwStyle & WS_MINIMIZEBOX_W) || (dwStyle & WS_MAXIMIZEBOX_W) || (dwStyle & WS_SYSMENU_W)) {
      hwndControl = WinWindowFromID(hwndFrame, FID_MINMAX);
      if(hwndControl) {
          swp[i].hwnd = hwndControl;
          swp[i].hwndInsertBehind = HWND_TOP;
          swp[i].x  = pRect->xLeft;
          swp[i].y  = pRect->yBottom;
          if(pRect->yTop - pRect->yBottom > minmaxheight) {
              swp[i].y += pRect->yTop - pRect->yBottom - minmaxheight;
          }
          swp[i].cx = 0;
          if((dwStyle & WS_MINIMIZEBOX_W)) {
              swp[i].cx += minmaxwidth/2;
          }
          if((dwStyle & WS_MAXIMIZEBOX_W)) {
              swp[i].cx += minmaxwidth/2;
          }
          //there is no close button in warp 3 and sometimes we do not have
          //close button as well
          if((dwStyle & WS_SYSMENU_W) && !fVersionWarp3 && drawCloseButton) {
              swp[i].cx += minmaxwidth/2;
          }
          //one pixel more to let PM center the controls properly
          swp[i].cy = minmaxheight+1;
          swp[i].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
          dprintf(("FID_MINMAX (%d,%d)(%d,%d)", swp[i].x, swp[i].y, swp[i].cx, swp[i].cy));
          pRect->xLeft += swp[i].cx;
          i++;
      }
  }
  return WinSetMultWindowPos(GetThreadHAB(), swp, i);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowULong(HWND hwnd, ULONG offset, ULONG value)
{
  if(offset == OSLIB_QWL_USER)
    offset = QWL_USER;

  return WinSetWindowULong(hwnd, offset, value);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetMinPosition(HWND hwnd, PSWP pswp, PPOINTL pointl)
{
  return WinGetMinPosition(hwnd, pswp, pointl);
}

//******************************************************************************
//******************************************************************************
ULONG OSLibWinGetWindowULong(HWND hwnd, ULONG offset)
{
  if(offset == OSLIB_QWL_USER)
    offset = QWL_USER;

  return WinQueryWindowULong(hwnd, offset);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinAlarm(HWND hwndDeskTop,ULONG flStyle)
{
    return WinAlarm(hwndDeskTop,flStyle);
}
//******************************************************************************
HWND OSLibWinQueryFocus(HWND hwndDeskTop)
{
    return WinQueryFocus(hwndDeskTop);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinWindowFromID(HWND hwndParent,ULONG id)
{
    return WinWindowFromID(hwndParent,id);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetFocus(HWND hwndDeskTop,HWND hwndNewFocus, BOOL activate)
{
    return WinFocusChange (hwndDeskTop, hwndNewFocus, activate ? 0 : FC_NOSETACTIVE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsChild (HWND hwnd, HWND hwndOf)
{
    return WinIsChild (hwnd, hwndOf);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibGetWindowHeight(HWND hwnd)
{
    RECTL rect;

    return (WinQueryWindowRect(hwnd,&rect)) ? rect.yTop-rect.yBottom:0;
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQuerySysValue(LONG iSysValue)
{
    return WinQuerySysValue(HWND_DESKTOP,iSysValue);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetSysValue(LONG iSysValue, ULONG val)
{
    return WinSetSysValue(HWND_DESKTOP, iSysValue, val);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinQueryDlgItemText(HWND hwndDlg,ULONG idItem,LONG cchBufferMax,char* pchBuffer)
{
    return WinQueryDlgItemText(hwndDlg,idItem,cchBufferMax,pchBuffer);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetDlgItemText(HWND hwndDlg,ULONG idItem,char* pszText)
{
    return WinSetDlgItemText(hwndDlg,idItem,pszText);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryPointerPos(PPOINT pptlPoint)
{
    return WinQueryPointerPos(HWND_DESKTOP,(PPOINTL)pptlPoint);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetPointerPos(int x, int y)
{
    return WinSetPointerPos(HWND_DESKTOP, x, y);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryWindow(HWND hwnd, ULONG lCode)
{
    return WinQueryWindow(hwnd, lCode);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetMultWindowPos(PSWP pswp, ULONG num)
{
    return WinSetMultWindowPos(GetThreadHAB(), pswp, num);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinShowWindow(HWND hwnd, ULONG fl)
{
 BOOL rc = 1;

  if(fl & SWP_SHOW) {
         rc = WinShowWindow(hwnd, TRUE);
  }
  if(rc == 0)
        dprintf(("WinShowWindow %x failed %x", hwnd, WinGetLastError(GetThreadHAB())));
  rc = WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, fl);
  if(rc == 0)
        dprintf(("WinShowWindow %x failed %x", hwnd, WinGetLastError(GetThreadHAB())));
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinDestroyWindow(HWND hwnd)
{
  return WinDestroyWindow(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibWinQueryWindowClientRect(HWND hwndOS2, PRECT pRect)
{
 BOOL     rc;
 RECTLOS2 rectl;

  rc = WinQueryWindowRect(hwndOS2, (PRECTL)&rectl);
  if(rc) {
        pRect->left   = 0;
        pRect->right  = rectl.xRight - rectl.xLeft;
        pRect->top    = 0;
        pRect->bottom = rectl.yTop - rectl.yBottom;
  }
  else  memset(pRect, 0, sizeof(RECT));
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibQueryWindowRectAbsolute (HWND hwndOS2, PRECT pRect)
{
    BOOL     rc;
    RECTLOS2 rectl;

    rc = WinQueryWindowRect (hwndOS2, (RECTL *)&rectl);
    if (rc)
    {
        rc = WinMapWindowPoints (hwndOS2, HWND_DESKTOP, (POINTL *)&rectl, 2);
        if (rc)
        {
            pRect->left   = rectl.xLeft;
            pRect->right  = rectl.xRight;
            pRect->top    = mapScreenY (rectl.yTop);
            pRect->bottom = mapScreenY (rectl.yBottom);
        }
    }
    if (!rc)
    {
        memset(pRect, 0, sizeof(*pRect));
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
#if 0
BOOL OSLibWinQueryWindowRect(Win32BaseWindow *window, PRECT pRect, int RelativeTo)
{
 BOOL     rc;
 RECTLOS2 rectl;

  rc = WinQueryWindowRect(window->getOS2WindowHandle(), (PRECTL)&rectl);
  if(rc) {
        if(RelativeTo == RELATIVE_TO_SCREEN) {
                mapOS2ToWin32RectFrame(window,windowDesktop,&rectl,pRect);
        }
        else    mapOS2ToWin32RectFrame(window,&rectl,pRect);
  }
  else  memset(pRect, 0, sizeof(RECT));
  return rc;
}
#endif
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsIconic(HWND hwnd)
{
 SWP  swp;
 BOOL rc;

  rc = WinQueryWindowPos(hwnd, &swp);
  if(rc == FALSE) {
        dprintf(("OSLibWinIsIconic: WinQueryWindowPos %x failed", hwnd));
        return FALSE;
  }

  if(swp.fl & SWP_MINIMIZE)
        return TRUE;
  else  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetActiveWindow(HWND hwnd)
{
 BOOL rc;

  rc = WinSetActiveWindow(HWND_DESKTOP, hwnd);
  if(rc == FALSE) {
    dprintf(("WinSetActiveWindow %x failure: %x", hwnd, OSLibWinGetLastError()));
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetFocus(HWND hwnd)
{
  return WinSetFocus(HWND_DESKTOP, hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEnableWindow(HWND hwnd, BOOL fEnable)
{
 BOOL rc;
 HWND hwndClient;

  rc = WinEnableWindow(hwnd, fEnable);
  hwndClient = WinWindowFromID(hwnd, FID_CLIENT);
  if(hwndClient) {
        WinEnableWindow(hwndClient, fEnable);
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsWindowEnabled(HWND hwnd)
{
  return WinIsWindowEnabled(hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinIsWindowVisible(HWND hwnd)
{
  return WinIsWindowVisible(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryActiveWindow()
{
  return WinQueryActiveWindow(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQueryWindowTextLength(HWND hwnd)
{
  return WinQueryWindowTextLength(hwnd);
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinQueryWindowText(HWND hwnd, LONG length, LPSTR lpsz)
{
    LONG retLen = WinQueryWindowText(hwnd, length, lpsz);
    OemToCharBuffA(lpsz, lpsz, retLen);
    lpsz[retLen] = '\0';
    return retLen;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetWindowText(HWND hwnd, LPSTR lpsz)
{
    PSZ psz = NULL;
    if(lpsz) {
        psz = (PSZ)_smalloc(strlen(lpsz) + 1);
        CharToOemA(lpsz, psz);
    }
    BOOL rc = WinSetWindowText(hwnd, psz);
    if (psz) {
        _sfree(psz);
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetTitleBarText(HWND hwnd, LPSTR lpsz)
{
    PSZ psz = NULL;
    if(lpsz) {
        psz = (PSZ)_smalloc(strlen(lpsz) + 1);
        CharToOemA(lpsz, psz);
    }
    BOOL rc = WinSetWindowText(WinWindowFromID(hwnd, FID_TITLEBAR), psz);
    if (psz) {
        _sfree(psz);
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinFlashWindow(HWND hwnd, BOOL fFlash)
{
  return WinFlashWindow(hwnd, fFlash);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinWindowFromPoint(HWND hwnd, PVOID ppoint)
{
  return WinWindowFromPoint((hwnd == OSLIB_HWND_DESKTOP) ? HWND_DESKTOP : hwnd, (PPOINTL)ppoint, TRUE);
}

//******************************************************************************
//@PF This is exactly that weird message PM sends when we maximize window from
//icon - this coordinates NEVER surface later and this combination of SWP
//commands is useless, yet it starts the correct reaction of maximiztion from
//icon state
//******************************************************************************
BOOL OSLibWinRestoreWindow(HWND hwnd)
{
 BOOL rc = WinSetWindowPos(hwnd, 0, -32000, 32000, 32000, 32000 , SWP_MAXIMIZE | SWP_RESTORE | SWP_ACTIVATE);
 return (rc);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibWinMinimizeWindow(HWND hwnd)
{
 /* @@PF The reason for this weird minimize algorithm is that we are not fully
    using PM for minimization. I.e. we respect all PM messages yet we do mess
    so much with some messages that minimization is based partly on vodoo.
    That is if you try minimize and deactivate in one call it will fail.
    Here we deactivate yourselves and give focus to next window that is
    on desktop, this func also works with MDI */

    BOOL rc;
    HWND hwndNext;

    rc = WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_MINIMIZE);

    if (rc) {
        rc = WinSetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_DEACTIVATE | SWP_ZORDER);
        if (rc)
        {
          HENUM henum;
          henum = WinBeginEnumWindows(HWND_DESKTOP);
          while ((hwndNext = WinGetNextWindow(henum)) != NULLHANDLE)
          {
           if (WinIsWindowVisible(hwndNext) && WinIsWindowShowing(hwndNext)) break;
          }
          WinEndEnumWindows (henum);
          rc = WinSetWindowPos(hwndNext, HWND_TOP, 0, 0, 0, 0, SWP_ACTIVATE | SWP_ZORDER);
        }
    }

    return (rc);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetBorderSize(HWND hwnd, OSLIBPOINT *pointl)
{
  pointl->x = 0;
  pointl->y = 0;
  return (BOOL) WinSendMsg(hwnd, WM_QUERYBORDERSIZE, MPFROMP( &pointl), 0);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetIcon(HWND hwnd, HANDLE hIcon)
{
    ULONG hIconOS2 = GetOS2Icon(hIcon);
    if(hIconOS2)
       return (BOOL) WinSendMsg(hwnd, WM_SETICON, (MPARAM)hIconOS2, 0);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryWindowPos (HWND hwnd, PSWP pswp)
{
  return WinQueryWindowPos(hwnd, pswp);
}
//******************************************************************************
//******************************************************************************
void OSLibMapSWPtoWINDOWPOS(PSWP pswp, PWINDOWPOS pwpos, PSWP pswpOld,
                            int parentHeight, HWND hwnd)
{
   HWND hWindow            = pswp->hwnd;
   HWND hWndInsertAfter    = pswp->hwndInsertBehind;
   long x                  = pswp->x;
   long y                  = pswp->y;
   long cx                 = pswp->cx;
   long cy                 = pswp->cy;
   UINT fuFlags            = (UINT)pswp->fl;

   HWND   hWinAfter;
   ULONG  flags = 0;

   HWND  hWnd = (hWindow == HWND_DESKTOP) ? HWND_DESKTOP_W: hWindow;

    if (hWndInsertAfter == HWND_TOP)
        hWinAfter = HWND_TOP_W;
    else if (hWndInsertAfter == HWND_BOTTOM)
        hWinAfter = HWND_BOTTOM_W;
    else
        hWinAfter = (HWND) hWndInsertAfter;

    //***********************************
    // convert PM flags to Windows flags
    //***********************************
    if (!(fuFlags & SWP_SIZE))       flags |= SWP_NOSIZE_W;
    if (!(fuFlags & SWP_MOVE))       flags |= SWP_NOMOVE_W;
    if (!(fuFlags & SWP_ZORDER))     flags |= SWP_NOZORDER_W;
    if (  fuFlags & SWP_NOREDRAW)    flags |= SWP_NOREDRAW_W;
    if (!(fuFlags & SWP_ACTIVATE))   flags |= SWP_NOACTIVATE_W;
    if (  fuFlags & SWP_SHOW)        flags |= SWP_SHOWWINDOW_W;
    if (  fuFlags & SWP_HIDE)        flags |= SWP_HIDEWINDOW_W;
    if (  fuFlags & SWP_NOADJUST)    flags |= SWP_NOSENDCHANGING_W;

    if(fuFlags & (SWP_MOVE | SWP_SIZE))
    {
        y  = parentHeight - y - pswp->cy;

        if ((pswp->x == pswpOld->x) && (pswp->y == pswpOld->y))
            flags |= SWP_NOMOVE_W;

        if ((pswp->cx == pswpOld->cx) && (pswp->cy == pswpOld->cy))
            flags |= SWP_NOSIZE_W;

        if (fuFlags & SWP_SIZE)
        {
            if (pswp->cy != pswpOld->cy)
            {
                flags &= ~SWP_NOMOVE_W;
            }
        }
    }

    pswpOld->x  = pswp->x;
    pswpOld->y  = parentHeight-pswp->y-pswp->cy;
    pswpOld->cx = pswp->cx;
    pswpOld->cy = pswp->cy;

    dprintf(("window (%d,%d)(%d,%d)  client (%d,%d)(%d,%d)",
             x,y,cx,cy, pswpOld->x,pswpOld->y,pswpOld->cx,pswpOld->cy));

    pwpos->flags            = (UINT)flags;
    pwpos->cy               = cy;
    pwpos->cx               = cx;
    pwpos->x                = x;
    pwpos->y                = y;
    pwpos->hwndInsertAfter  = hWinAfter;
    pwpos->hwnd             = hWindow;

    dprintf2(("OSLibMapSWPtoWINDOWPOS %x (%d,%d)(%d,%d) -> %x (%d,%d)(%d,%d) parent height %d", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy, flags, x, y, cx, cy, parentHeight));
}
//******************************************************************************
//******************************************************************************
void OSLibMapWINDOWPOStoSWP(struct tagWINDOWPOS *pwpos, PSWP pswp, PSWP pswpOld,
                            int parentHeight, HWND hFrame)
{
 BOOL fCvt = FALSE;

   HWND hWnd            = pwpos->hwnd;
   HWND hWndInsertAfter = pwpos->hwndInsertAfter;
   long x               = pwpos->x;
   long y               = pwpos->y;
   long cx              = pwpos->cx;
   long cy              = pwpos->cy;
   UINT fuFlags         = pwpos->flags;

   HWND  hWinAfter;
   ULONG flags = 0;
   HWND  hWindow = hWnd ? (HWND)hWnd : HWND_DESKTOP;

   if (hWndInsertAfter == HWND_TOPMOST_W)
//      hWinAfter = HWND_TOPMOST;
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_NOTOPMOST_W)
//      hWinAfter = HWND_NOTOPMOST;
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_TOP_W)
      hWinAfter = HWND_TOP;
   else if (hWndInsertAfter == HWND_BOTTOM_W)
      hWinAfter = HWND_BOTTOM;
   else
      hWinAfter = (HWND) hWndInsertAfter;

   if (!(fuFlags & SWP_NOSIZE_W    )) flags |= SWP_SIZE;
   if (!(fuFlags & SWP_NOMOVE_W    )) flags |= SWP_MOVE;
   if (!(fuFlags & SWP_NOZORDER_W  )) flags |= SWP_ZORDER;
   if (  fuFlags & SWP_NOREDRAW_W  )  flags |= SWP_NOREDRAW;
   if (!(fuFlags & SWP_NOACTIVATE_W)) flags |= SWP_ACTIVATE;
   if (  fuFlags & SWP_SHOWWINDOW_W)  flags |= SWP_SHOW;
   //SvL: Must also deactivate the window when hiding it or else focus won't
   //     change. (NOTE: make sure this doesn't cause regressions (01-02-2003)
   if (  fuFlags & SWP_HIDEWINDOW_W) {
       flags |= SWP_HIDE|SWP_DEACTIVATE;
   }

   if (  fuFlags & SWP_NOSENDCHANGING_W) flags |= SWP_NOADJUST;

   if(flags & (SWP_MOVE | SWP_SIZE))
   {
      if((flags & SWP_MOVE) == 0)
      {
         x = pswpOld->x;
         y = pswpOld->y;

         y = parentHeight - y - pswpOld->cy;
     }

      if(flags & SWP_SIZE)
      {
         if (cy != pswpOld->cy)
            flags |= SWP_MOVE;
      }
      else
      {
         cx = pswpOld->cx;
         cy = pswpOld->cy;
      }
      y = parentHeight - y - cy;

      if ((pswpOld->x == x) && (pswpOld->y == y))
         flags &= ~SWP_MOVE;

      if ((pswpOld->cx == cx) && (pswpOld->cy == cy))
         flags &= ~SWP_SIZE;
   }

   pswp->fl               = flags;
   pswp->cy               = cy;
   pswp->cx               = cx;
   pswp->x                = x;
   pswp->y                = y;
   pswp->hwndInsertBehind = hWinAfter;
   pswp->hwnd             = hWindow;
   pswp->ulReserved1      = 0;
   pswp->ulReserved2      = 0;

   dprintf2(("OSLibMapWINDOWPOStoSWP %x (%d,%d)(%d,%d) -> %x (%d,%d)(%d,%d) parent height %d", pwpos->flags, pwpos->x, pwpos->y, pwpos->cx, pwpos->cy, flags, x, y, cx, cy, parentHeight));
}
//******************************************************************************
//******************************************************************************
void OSLibWinSetClientPos(HWND hwnd, int x, int y, int cx, int cy, int parentHeight)
{
 SWP swp;
 BOOL rc;

   swp.hwnd = hwnd;
   swp.hwndInsertBehind = 0;
   swp.x  = x;
   swp.y  = parentHeight - y - cy;
   swp.cx = cx;
   swp.cy = cy;
   swp.fl = SWP_MOVE | SWP_SIZE;

   dprintf(("OSLibWinSetClientPos (%d,%d) (%d,%d) -> (%d,%d) (%d,%d)", x, y, x+cx, y+cy, swp.x, swp.y, swp.x+swp.cx, swp.y+swp.cy));

   rc = WinSetMultWindowPos(GetThreadHAB(), &swp, 1);
   if(rc == FALSE) {
        dprintf(("OSLibWinSetClientPos: WinSetMultWindowPos %x failed %x", hwnd, WinGetLastError(GetThreadHAB())));
   }
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinCalcFrameRect(HWND hwndFrame, RECT *pRect, BOOL fClient)
{
 BOOL rc;

   WinMapWindowPoints(hwndFrame, HWND_DESKTOP, (PPOINTL)pRect, 2);

   rc = WinCalcFrameRect(hwndFrame, (PRECTL)pRect, fClient);
   WinMapWindowPoints(HWND_DESKTOP, hwndFrame, (PPOINTL)pRect, 2);

   return rc;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibGetMinMaxInfo(HWND hwndFrame, MINMAXINFO *pMinMax)
{
 TRACKINFO tinfo;

   memset(&tinfo, 0, sizeof(TRACKINFO));
   WinSendMsg(hwndFrame, WM_QUERYTRACKINFO, (MPARAM)0,(MPARAM)&tinfo);

   pMinMax->ptMinTrackSize.x = tinfo.ptlMinTrackSize.x;
   pMinMax->ptMinTrackSize.y = tinfo.ptlMinTrackSize.y;
   pMinMax->ptMaxTrackSize.x = tinfo.ptlMaxTrackSize.x;
   pMinMax->ptMaxTrackSize.y = tinfo.ptlMaxTrackSize.y;
   return TRUE;
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinBeginEnumWindows(HWND hwnd)
{
   if(hwnd == OSLIB_HWND_DESKTOP)       hwnd = HWND_DESKTOP;
   else
   if(hwnd == OSLIB_HWND_OBJECT)        hwnd = HWND_OBJECT;

   return WinBeginEnumWindows(hwnd);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinGetNextWindow(HWND hwndEnum)
{
   return WinGetNextWindow(hwndEnum);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEndEnumWindows(HWND hwndEnum)
{
   return WinEndEnumWindows(hwndEnum);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinQueryWindowProcess(HWND hwnd, ULONG *pid, ULONG *tid)
{
   BOOL ret;

   ret = WinQueryWindowProcess(hwnd, pid, tid);
   *tid = MAKE_THREADID(*pid, *tid);
   return ret;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinMapWindowPoints (HWND hwndFrom, HWND hwndTo, OSLIBPOINT *pptl, ULONG num)
{
   return WinMapWindowPoints (hwndFrom, hwndTo, (PPOINTL)pptl, num);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryObjectWindow(VOID)
{
  return WinQueryObjectWindow(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinObjectWindowFromID(HWND hwndOwner, ULONG ID)
{
  HWND   hwndNext, hwndFound=0;
  HENUM  henum;

  henum = WinBeginEnumWindows(HWND_OBJECT);
  while ((hwndNext = WinGetNextWindow(henum)) != 0)
  {
    if(WinQueryWindow(hwndNext, QW_OWNER) == hwndOwner &&
       WinQueryWindowUShort(hwndNext, QWS_ID) == ID)
    {
       hwndFound = hwndNext;
       break;
    }
  }
  WinEndEnumWindows(henum);
  return hwndFound;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowID(HWND hwnd, ULONG value)
{
  dprintf(("OSLibSetWindowID hwnd:%x ID:%x", hwnd, value));
  return WinSetWindowULong(hwnd, QWS_ID, value);
}
//******************************************************************************
//******************************************************************************
PVOID OSLibWinSubclassWindow(HWND hwnd,PVOID newWndProc)
{
  return WinSubclassWindow(hwnd,(PFNWP)newWndProc);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowRestoreRect(HWND hwnd, PRECT pRect)
{
 ULONG yHeight = OSLibGetWindowHeight(WinQueryWindow(hwnd, QW_PARENT));

  WinSetWindowUShort(hwnd, QWS_XRESTORE,  (USHORT)pRect->left );
  WinSetWindowUShort(hwnd, QWS_YRESTORE,  (USHORT)(yHeight - pRect->top -
                                                   (pRect->bottom - pRect->top)));
  WinSetWindowUShort(hwnd, QWS_CXRESTORE, (USHORT)(pRect->right - pRect->left));
  WinSetWindowUShort(hwnd, QWS_CYRESTORE, (USHORT)(pRect->bottom - pRect->top));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetWindowMinPos(HWND hwnd, ULONG x, ULONG y)
{
 ULONG yHeight = OSLibGetWindowHeight(WinQueryWindow(hwnd, QW_PARENT));

  WinSetWindowUShort(hwnd, QWS_XMINIMIZE, (USHORT)x );
  WinSetWindowUShort(hwnd, QWS_YMINIMIZE, (USHORT)(yHeight - y -
                    ( 2 * WinQuerySysValue( HWND_DESKTOP, SV_CYSIZEBORDER)) -
                      WinQuerySysValue( HWND_DESKTOP, SV_CYICON)));
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinEnableWindowUpdate(HWND hwndFrame, HWND hwndClient ,BOOL fEnable)
{
  WinEnableWindowUpdate(hwndFrame, fEnable);
  return WinEnableWindowUpdate(hwndClient, fEnable);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinGetLastError()
{
  return WinGetLastError(GetThreadHAB()) & 0xFFFF;
}
//******************************************************************************
//******************************************************************************
void OSLibWinShowTaskList(HWND hwndFrame)
{
  SWBLOCK swblk;
  // the first entry returned is always the window list itself
  if (WinQuerySwitchList(0, &swblk, sizeof(SWBLOCK)) > 0)
     WinSetActiveWindow(HWND_DESKTOP, swblk.aswentry[0].swctl.hwnd);
//      WinShowWindow(swblk.aswentry[0].swctl.hwnd, TRUE);
}
//******************************************************************************
//******************************************************************************
//PF: PM Logic approved by numerous testcases shows this:
//There is no other way to tweak FID_MINMAX without deleting it
//Controls are created with size 0,0, invisible and should be immediately
//positioned. MINMAX control can't function properly without FID_SYSMENU
//control if it is present, so we need to recreate BOTH controls.
//Currently OSLibSetWindowStyle checks for changes and actually do the job
//only when it is needed so no additional messages are generated.
//TO-DO: There MAY BE some regressions here but I haven't seen them
//and yes this code is the only way to do WinCreateFrame controls,
//first delete old then create new - all other variants create new and
//leave old, WinCreateFrameControls can't tweak anything.

void OSLibSetWindowStyle(HWND hwndFrame, HWND hwndClient, ULONG dwStyle,
                         ULONG dwExStyle, ULONG dwOldWindowsStyle)
{
    ULONG dwWinStyle;
    ULONG dwOldWinStyle;

    int checksum, checksum2;
    DWORD dest_tid, dest_pid;

    static int minmaxwidth  = 0;
    static int minmaxheight = 0;


    //PF We can tweak OS/2 controls ONLY from thread that created them
    //in other case heavy PM lockups will follow

    dest_tid = GetWindowThreadProcessId(OS2ToWin32Handle(hwndClient) , &dest_pid );

    if (dest_tid != GetCurrentThreadId())
    {
        dprintf(("OSLibSetWindowStyle: Redirecting Change Frame controls to another thread"));
        WinSendMsg(hwndFrame, WIN32APP_CHNGEFRAMECTRLS, (MPARAM)dwStyle, (MPARAM)dwOldWindowsStyle);
        return;
    }
    if(minmaxwidth == 0) {
        minmaxwidth  = WinQuerySysValue(HWND_DESKTOP, SV_CXMINMAXBUTTON);
        minmaxheight = WinQuerySysValue(HWND_DESKTOP, SV_CYMINMAXBUTTON);
    }

    if (hwndClient)
    {
      //client window:
      dwWinStyle    = WinQueryWindowULong(hwndClient, QWL_STYLE);
      dwOldWinStyle = dwWinStyle;

      if(dwStyle & WS_CLIPCHILDREN_W) {
          dwWinStyle |= WS_CLIPCHILDREN;
      }
      else dwWinStyle &= ~WS_CLIPCHILDREN;

      if(dwWinStyle != dwOldWinStyle) {
           WinSetWindowULong(hwndClient, QWL_STYLE, dwWinStyle);
      }
    }

    //Frame window
    dwWinStyle    = WinQueryWindowULong(hwndFrame, QWL_STYLE);
    dwOldWinStyle = dwWinStyle;

    checksum =  (dwOldWindowsStyle & WS_MINIMIZEBOX_W) + (dwOldWindowsStyle & WS_MAXIMIZEBOX_W) + (dwOldWindowsStyle & WS_SYSMENU_W);
    checksum2 =  (dwStyle & WS_MINIMIZEBOX_W) + (dwStyle & WS_MAXIMIZEBOX_W) + (dwStyle & WS_SYSMENU_W);

    if(dwStyle & WS_DISABLED_W) {
         dwWinStyle |= WS_DISABLED;
    }
    else dwWinStyle &= ~WS_DISABLED;

    if(!(dwStyle & WS_CHILD_W) && dwExStyle & WS_EX_TOPMOST_W) {
         dwWinStyle |= WS_TOPMOST;
    }
    else dwWinStyle &= ~WS_TOPMOST;

    if(dwStyle & WS_CLIPSIBLINGS_W) {
         dwWinStyle |= WS_CLIPSIBLINGS;
    }
    else dwWinStyle &= ~WS_CLIPSIBLINGS;

    if(dwStyle & WS_MINIMIZE_W) {
         dwWinStyle |= WS_MINIMIZED;
    }
    else dwWinStyle &= ~WS_MINIMIZED;

    if(dwStyle & WS_MAXIMIZE_W) {
         dwWinStyle |= WS_MAXIMIZED;
    }
    else
      dwWinStyle &= ~WS_MAXIMIZED;

    //WS_EX_TOOLWINDOW is incompatible with the OS2Look (titlebar thinner + smaller font)
    if(fOS2Look && !(dwExStyle & WS_EX_TOOLWINDOW_W)) {
        ULONG OSFrameStyle = 0;
        SWP rc1,rc2,rc3;
        int totalwidth = 0;

        if((dwStyle & WS_CAPTION_W) == WS_CAPTION_W)
        {
            if(WinWindowFromID(hwndFrame, FID_TITLEBAR) == 0) {
                OSFrameStyle = FCF_TITLEBAR;
            }
            else
              WinQueryWindowPos(WinWindowFromID(hwndFrame, FID_TITLEBAR), &rc1);

            if((dwStyle & WS_SYSMENU_W) && !(dwExStyle & WS_EX_TOOLWINDOW_W))
            {
              if(WinWindowFromID(hwndFrame, FID_SYSMENU) == 0)
                OSFrameStyle |= FCF_SYSMENU;
            }
            WinQueryWindowPos(WinWindowFromID(hwndFrame, FID_SYSMENU), &rc2);
            WinQueryWindowPos(WinWindowFromID(hwndFrame, FID_MINMAX), &rc3);

            if (checksum != checksum2)
            {
              dprintf(("OSLibSetWindowStyle: Min/Max/Close state changed. Creating:"));
              if(dwStyle & WS_MINIMIZEBOX_W)
              {
                  OSFrameStyle |= FCF_MINBUTTON;
                  totalwidth += minmaxwidth/2;
                  dprintf(("min button"));
              }

              if(dwStyle & WS_MAXIMIZEBOX_W)
              {
                  OSFrameStyle |= FCF_MAXBUTTON;
                  totalwidth += minmaxwidth/2;
                  dprintf(("max button"));
              }

              if(dwStyle & WS_SYSMENU_W)
              {
                  OSFrameStyle |= FCF_CLOSEBUTTON;
                  OSFrameStyle |= FCF_SYSMENU;
                  totalwidth += minmaxwidth/2;
                  dprintf(("close button"));
              }
            }
       }
       else
       {
          if (WinWindowFromID(hwndFrame, FID_TITLEBAR))
              WinDestroyWindow(WinWindowFromID(hwndFrame, FID_TITLEBAR));
       }

       if (checksum != checksum2)
       {
         if (WinWindowFromID(hwndFrame, FID_SYSMENU))
             WinDestroyWindow(WinWindowFromID(hwndFrame, FID_SYSMENU));
         if (WinWindowFromID(hwndFrame, FID_MINMAX))
             WinDestroyWindow(WinWindowFromID(hwndFrame, FID_MINMAX));
       }

       if(OSFrameStyle) {
            FRAMECDATA FCData = {sizeof (FRAMECDATA), 0, 0, 0};
            char buffer[255];
            dprintf(("Controls will be created %x",OSFrameStyle));
            FCData.flCreateFlags = OSFrameStyle;

            GetWindowTextA(OS2ToWin32Handle(hwndClient), buffer, sizeof(buffer));
            WinCreateFrameControls(hwndFrame, &FCData, buffer );

            if (totalwidth != rc3.cx)
            {
              rc3.cx =  totalwidth;
              totalwidth = rc3.cx - totalwidth;
              rc1.cx = rc1.cx + totalwidth;
              rc3.x = rc3.x + totalwidth;
            }

            WinSetWindowPos(WinWindowFromID(hwndFrame, FID_MINMAX),0,rc3.x,rc3.y,rc3.cx,rc3.cy, SWP_MOVE | SWP_SIZE | SWP_SHOW);
            WinSetWindowPos(WinWindowFromID(hwndFrame, FID_SYSMENU),0,rc2.x,rc2.y,rc2.cx,rc2.cy, SWP_MOVE | SWP_SIZE | SWP_SHOW);
            WinSetWindowPos(WinWindowFromID(hwndFrame, FID_TITLEBAR),0,rc1.x,rc1.y,rc1.cx,rc1.cy, SWP_MOVE | SWP_SIZE | SWP_SHOW);

            if (WinQueryActiveWindow(HWND_DESKTOP) == hwndFrame)
              WinSendMsg(WinWindowFromID(hwndFrame, FID_TITLEBAR), TBM_SETHILITE, (MPARAM)1, 0);

       }
   } // os2look

   if(dwWinStyle != dwOldWinStyle) {
       dprintf(("Setting new window U long"));
       WinSetWindowULong(hwndFrame, QWL_STYLE, dwWinStyle);
   }

}
//******************************************************************************
//******************************************************************************
BOOL OSLibChangeCloseButtonState(HWND hwndFrame, BOOL State)
{
    dprintf(("OSLibChangeCloseButtonState %x %d", hwndFrame, State));
    return WinEnableMenuItem(WinWindowFromID(hwndFrame, FID_SYSMENU), SC_CLOSE, State);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibQueryWindowStyle(HWND hwnd)
{
    return WinQueryWindowULong(hwnd, QWL_STYLE);
}
//******************************************************************************
//******************************************************************************
void OSLibWinSetVisibleRegionNotify(HWND hwnd, BOOL fNotify)
{
    WinSetVisibleRegionNotify(hwnd, fNotify);
}
//******************************************************************************
//******************************************************************************
HWND OSLibWinQueryCapture()
{
    return WinQueryCapture(HWND_DESKTOP);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetCapture(HWND hwnd)
{
    return WinSetCapture(HWND_DESKTOP, hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinRemoveFromTasklist(HANDLE hTaskList)
{
    dprintf(("OSLibWinRemoveFromTasklist %x", hTaskList));
    return (WinRemoveSwitchEntry(hTaskList)) ? FALSE : TRUE;
}
//******************************************************************************
//******************************************************************************
HANDLE OSLibWinAddToTaskList(HWND hwndFrame, char *title, BOOL fVisible)
{
    SWCNTRL swctrl;
    ULONG   tid;

    swctrl.hwnd          = hwndFrame;
    swctrl.hwndIcon      = 0;
    swctrl.hprog         = 0;
    WinQueryWindowProcess(hwndFrame, (PPID)&swctrl.idProcess, (PTID)&tid);
    swctrl.idSession     = 0;
    swctrl.uchVisibility = (fVisible) ? SWL_VISIBLE : SWL_INVISIBLE;
    swctrl.fbJump        = SWL_JUMPABLE;
    swctrl.bProgType     = PROG_PM;
    if(title) {
        CharToOemBuffA( title, swctrl.szSwtitle, min(strlen(title)+1,MAXNAMEL+4) );
        swctrl.szSwtitle[MAXNAMEL+4-1] = 0;
    }
    else {
        swctrl.szSwtitle[0] = 0;
        swctrl.uchVisibility    = SWL_INVISIBLE;
    }
    HANDLE hTaskList = WinAddSwitchEntry(&swctrl);
    dprintf(("OSLibWinAddToTaskList %s %x", swctrl.szSwtitle, hTaskList));
    return hTaskList;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinChangeTaskList(HANDLE hTaskList, HWND hwndFrame, char *title, BOOL fVisible)
{
    SWCNTRL swctrl;
    ULONG   tid;

    swctrl.hwnd          = hwndFrame;
    swctrl.hwndIcon      = 0;
    swctrl.hprog         = 0;
    WinQueryWindowProcess(hwndFrame, (PPID)&swctrl.idProcess, (PTID)&tid);
    swctrl.idSession     = 0;
    swctrl.uchVisibility = (fVisible) ? SWL_VISIBLE : SWL_INVISIBLE;
    swctrl.fbJump        = SWL_JUMPABLE;
    swctrl.bProgType     = PROG_PM;
    dprintf(("OSLibWinChangeTaskList %x %s swctrl %x size %d", hTaskList, title, &swctrl, sizeof(swctrl)));
    if(title) {
        CharToOemBuffA( title, swctrl.szSwtitle, min(strlen(title)+1,MAXNAMEL+4) );
        swctrl.szSwtitle[MAXNAMEL+4-1] = 0;
    }
    else {
        swctrl.szSwtitle[0] = 0;
        swctrl.uchVisibility    = SWL_INVISIBLE;
    }
    dprintf(("hwnd           %x", swctrl.hwnd));
    dprintf(("hwndIcon       %x", swctrl.hwndIcon));
    dprintf(("hprog          %x", swctrl.hprog));
    dprintf(("idProcess      %x", swctrl.idProcess));
    dprintf(("idSession      %x", swctrl.idSession));
    dprintf(("uchVisibility  %x", swctrl.uchVisibility));
    dprintf(("fbJump         %x", swctrl.fbJump));
    dprintf(("bProgType      %x", swctrl.bProgType));
    dprintf(("szSwtitle      %s", swctrl.szSwtitle));

    return (WinChangeSwitchEntry(hTaskList, &swctrl)) ? FALSE : TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinLockWindowUpdate(HWND hwnd)
{
    return WinLockWindowUpdate(HWND_DESKTOP, (HWND)hwnd);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibGetScreenHeight()
{
    return ScreenHeight;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibGetScreenWidth()
{
    return ScreenWidth;
}
//******************************************************************************
//Returns the maximum position for a window
//Should only be used from toplevel windows
//******************************************************************************
BOOL OSLibWinGetMaxPosition(HWND hwndOS2, RECT *rect)
{
 SWP  swp;

    if(!WinGetMaxPosition(hwndOS2, &swp)) {
        dprintf(("WARNING: WinGetMaxPosition %x returned FALSE", hwndOS2));
        return FALSE;
    }
    rect->left   = swp.x;
    rect->right  = swp.x + swp.cx;
    rect->top    = ScreenHeight - (swp.y + swp.cy);
    rect->bottom = ScreenHeight - swp.y;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinShowPointer(BOOL fShow)
{
    return WinShowPointer(HWND_DESKTOP, fShow);
}
//******************************************************************************
//******************************************************************************
ULONG  OSLibWinQuerySysColor(int index)
{
    return CONVERT_RGB(WinQuerySysColor(HWND_DESKTOP, index, 0));
}
//******************************************************************************
//PF This was added for IBM Wheel Mouse driver - it searches for scrollbars and
//only if they exist sends WM_VSCROLL messages to the window
//******************************************************************************
HWND OSLibWinCreateInvisibleScroller(HWND parentHWND, int direction)
{
    return WinCreateWindow(parentHWND, WC_SCROLLBAR, NULL, direction,
                           0, 0, 0, 0, parentHWND, HWND_BOTTOM, 1, NULL, NULL);
}
//******************************************************************************
//******************************************************************************
void OSLibWinLockVisibleRegions(BOOL fLock)
{
    WinLockVisRegions(HWND_DESKTOP, fLock);
}
//******************************************************************************
//******************************************************************************

/* 'Private' PM property stuff. */
PVOID APIENTRY WinQueryProperty(HWND hwnd, PCSZ pszNameOrAtom);
PVOID APIENTRY WinRemoveProperty(HWND hwnd, PCSZ pszNameOrAtom);
BOOL  APIENTRY WinSetProperty(HWND hwnd, PCSZ pszNameOrAtom, PVOID pvData, ULONG ulFlags);

/**
 * Set Property.
 * @returns Success indicator.
 * @param   hwnd    Window the property is associated with.
 * @param   psz     The property atom or name.
 * @param   pv      Property value.
 * @param   fFlags  Flags. Use 0.
 */
BOOL    OSLibSetProperty(HWND hwnd, const char *psz, void *pv, unsigned fFlags)
{
    USHORT selFS = RestoreOS2FS();
    BOOL fRet = WinSetProperty(hwnd, psz, pv, fFlags);
    SetFS(selFS);
    return fRet;
}

/**
 * Get Property.
 * @returns Property value.
 * @param   hwnd    Window the property is associated with.
 * @param   psz     The property atom or name.
 */
void *  OSLibQueryProperty(HWND hwnd, const char *psz)
{
    USHORT selFS = RestoreOS2FS();
    void *pvRet = WinQueryProperty(hwnd, psz);
    SetFS(selFS);
    return pvRet;
}

/**
 * Remove Property.
 * @returns Property value.
 * @param   hwnd    Window the property is associated with.
 * @param   psz     The property atom or name.
 */
void *  OSLibRemoveProperty(HWND hwnd, const char *psz)
{
    USHORT selFS = RestoreOS2FS();
    void *pvRet = WinRemoveProperty(hwnd, psz);
    SetFS(selFS);
    return pvRet;
}

