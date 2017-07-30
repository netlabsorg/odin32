/* $Id: win32wmdiclient.cpp,v 1.41 2002-12-18 12:28:08 sandervl Exp $ */
/*
 * Win32 MDI Client Window Class for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 * Parts based on Wine (windows\mdi.c)
 *
 * Copyright 1994, Bob Amstadt
 *           1995,1996 Alex Korobka
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include "win32wnd.h"
#include "win32wmdiclient.h"
#include <spy.h>
#include "wndmsg.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"

#define DBG_LOCALLOG    DBG_win32wmdiclient
#include "dbglocal.h"


//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::Win32MDIClientWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                : Win32BaseWindow()
{
  Init();
  this->isUnicode = isUnicode;
  memset(&mdiClientInfo, 0, sizeof(mdiClientInfo));
  cbExtra = sizeof(mdiClientInfo);
  pExtra  = (PVOID)&mdiClientInfo;
  CreateWindowExA(lpCreateStructA, classAtom);
}
//******************************************************************************
//******************************************************************************
Win32MDIClientWindow::~Win32MDIClientWindow()
{
}
//******************************************************************************
//******************************************************************************
BOOL Win32MDIClientWindow::isMDIClient()
{
    return TRUE;
}

/*************************************************************************
 *	     SCROLL_SetNCSbState
 *
 * Updates both scrollbars at the same time. Used by MDI CalcChildScroll().
 */
INT SCROLL_SetNCSbState(HWND hwnd, int vMin, int vMax, int vPos,
                        int hMin, int hMax, int hPos)
{
    SCROLLINFO vInfo, hInfo;

    vInfo.cbSize = hInfo.cbSize = sizeof(SCROLLINFO);
    vInfo.nMin   = vMin;
    vInfo.nMax   = vMax;
    vInfo.nPos   = vPos;
    hInfo.nMin   = hMin;
    hInfo.nMax   = hMax;
    hInfo.nPos   = hPos;
    vInfo.fMask  = hInfo.fMask = SIF_RANGE | SIF_POS;

    SetScrollInfo(hwnd,SB_VERT,&vInfo,TRUE);
    SetScrollInfo(hwnd,SB_HORZ,&hInfo,TRUE);
    return 0;
}


/*****************************************************************************
 * Name      : BOOL WIN32API CascadeChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API CascadeChildWindows(HWND x1, DWORD x2)
{
  dprintf(("USER32: CascadeChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}

/*****************************************************************************
 * Name      : BOOL WIN32API TileChildWindows
 * Purpose   : Unknown
 * Parameters: Unknown
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Wed, 1998/06/16 11:55]
 *****************************************************************************/
BOOL WIN32API TileChildWindows(HWND x1, DWORD x2)
{
  dprintf(("USER32: TileChildWindows(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return (FALSE); /* default */
}
//******************************************************************************
//******************************************************************************
