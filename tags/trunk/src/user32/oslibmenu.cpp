/* $Id: oslibmenu.cpp,v 1.3 1999-10-25 20:17:18 sandervl Exp $ */
/*
 * Window Menu wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2.h>
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmenu.h"

//******************************************************************************
//******************************************************************************
HWND OSLibWinSetMenu(HWND hwndParent, HMENU hMenu)
{
   // Remove current menu from window
   HWND currMenu = WinWindowFromID( (HWND)hwndParent, FID_MENU );
   if (currMenu)
   {
      WinSetOwner (currMenu, HWND_OBJECT);
      WinSetParent(currMenu, HWND_OBJECT, FALSE);
   }

   if (hMenu)
   {
      if(WinIsWindow(GetThreadHAB(), hMenu) == TRUE) {
         WinSetOwner (hMenu, hwndParent);
         WinSetParent(hMenu, hwndParent, FALSE );
         WinSetWindowUShort(hMenu, QWS_ID, FID_MENU);
     WinSendMsg(hwndParent, WM_UPDATEFRAME, (MPARAM)FCF_MENU, 0);
     return hMenu;
      }
   }
   return 0;
}
//******************************************************************************
//******************************************************************************
int OSLibGetMenuItemCount(HWND hMenu)
{
   return (int)SHORT1FROMMR(WinSendMsg(hMenu, MM_QUERYITEMCOUNT, NULL, NULL));
}
//******************************************************************************
//******************************************************************************
HMENU OSLibWinCreateMenu(PVOID menutemplate)
{
    return (HMENU)WinCreateMenu(HWND_OBJECT, menutemplate);
}
//******************************************************************************
//******************************************************************************
HMENU OSLibWinCreateEmptyMenu()
{
   return WinCreateWindow(HWND_OBJECT, WC_MENU, NULL, MS_ACTIONBAR | 0x0008 | WS_SAVEBITS,
                          0, 0, 0, 0, HWND_OBJECT, HWND_TOP, 0, NULL, NULL);
}
//******************************************************************************
//******************************************************************************
HMENU OSLibWinCreateEmptyPopupMenu()
{
   return WinCreateWindow(HWND_OBJECT, WC_MENU, NULL, WS_CLIPSIBLINGS | WS_SAVEBITS,
                          0, 0, 0, 0, HWND_OBJECT, HWND_TOP, 0, NULL, NULL);
}
//******************************************************************************
//******************************************************************************

