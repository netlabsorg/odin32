/* $Id: oslibmenu.cpp,v 1.1 2001-05-11 08:37:24 sandervl Exp $ */
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
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winconst.h>
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibmenu.h"

#define DBG_LOCALLOG	DBG_oslibmenu
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HWND OSLibWinSetMenu(HWND hwndParent, HMENU hMenu)
{
   // Remove current menu from window
   HWND currMenu = WinWindowFromID( (HWND)hwndParent, FID_MENU );
   if (currMenu)
   {
      dprintf(("OSLibWinSetMenu: old menu %x, new menu %x", currMenu, hMenu));
      WinSetOwner (currMenu, HWND_OBJECT);
      WinSetParent(currMenu, HWND_OBJECT, FALSE);
   }

   if (hMenu)
   {
      	if(WinIsWindow(GetThreadHAB(), hMenu) == TRUE) 
	{
         	WinSetOwner (hMenu, hwndParent);
         	WinSetParent(hMenu, hwndParent, FALSE );
         	WinSetWindowUShort(hMenu, QWS_ID, FID_MENU);
     		WinSendMsg(hwndParent, WM_UPDATEFRAME, (MPARAM)FCF_MENU, 0);
     		return hMenu;
      	}
	else {
		dprintf(("OSLibWinSetMenu: %x = invalid menu handle", hMenu));
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
//Returns menu item rectange in screen coordinates
//******************************************************************************
BOOL OSLibGetMenuItemRect(HWND hMenu, int index, LPRECT pRect)
{
  RECTL rectl;
  BOOL  rc;
  ULONG id;

   //First get id from menu index
   id = (ULONG)WinSendMsg(hMenu, MM_ITEMIDFROMPOSITION, MPARAM(index), 0);

   rc = (BOOL)WinSendMsg(hMenu, MM_QUERYITEMRECT, MPARAM(id), (MPARAM)&rectl);
   if(rc == FALSE) {
	dprintf(("OSLibGetMenuItemRect %x %d %d failed!", hMenu, index, id));
	return FALSE;
   }
   WinMapWindowPoints(hMenu, HWND_DESKTOP, (PPOINTL)&rectl, 2);
   pRect->left  = rectl.xLeft;
   pRect->right = rectl.xRight;
   pRect->top   = OSLibQueryScreenHeight() - rectl.yTop;
   pRect->bottom= OSLibQueryScreenHeight() - rectl.yBottom;
   return TRUE;
}
//******************************************************************************
//******************************************************************************

