/* $Id: winmenu.cpp,v 1.7 1999-09-24 12:47:51 sandervl Exp $ */

/*
 * Win32 menu API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Parts ported from Wine: (ChangeMenuA/W)
 * Copyright 1993 Martin Ayotte
 * Copyright 1994 Alexandre Julliard
 * Copyright 1997 Morten Welinder
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <odin.h>
#include <odinwrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32wbase.h>
#include "oslibmenu.h"
#include <winresmenu.h>


ODINDEBUGCHANNEL(USER32)

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU,     LoadMenuA,
              HINSTANCE, hinst,
              LPCSTR,    lpszMenu)
{
  Win32MenuRes *winres;
  HMENU hMenu;

  winres = (Win32MenuRes *)FindResourceA(hinst, lpszMenu, RT_MENUA);
  if(winres) {
	hMenu = O32_LoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockOS2Resource());
	delete winres;
	return hMenu;
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, LoadMenuW,
              HINSTANCE, hinst,
              LPCWSTR, lpszMenu)
{
  Win32MenuRes *winres;
  HMENU hMenu;

  winres = (Win32MenuRes *)FindResourceW(hinst, lpszMenu, RT_MENUW);
  if(winres) {
	hMenu = O32_LoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockOS2Resource());
	delete winres;
	return hMenu;
  }
  return 0;
}
//******************************************************************************
//NOTE: menutemplate strings are always in Unicode format
//******************************************************************************
ODINFUNCTION1(HMENU, LoadMenuIndirectA,
              const MENUITEMTEMPLATEHEADER *, menuTemplate)
{
  Win32MenuRes *winres;
  HMENU hMenu;

  winres = new Win32MenuRes((LPVOID)menuTemplate);
  if(winres == NULL)
    return 0;

  hMenu = O32_LoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockOS2Resource());
  delete winres;
  return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HMENU, LoadMenuIndirectW,
              const MENUITEMTEMPLATEHEADER *, menuTemplate)
{
  Win32MenuRes *winres;
  HMENU hMenu;

  winres = new Win32MenuRes((LPVOID)menuTemplate);
  if(winres == NULL)
    return 0;

  hMenu = O32_LoadMenuIndirect((MENUITEMTEMPLATEHEADER *)winres->lockOS2Resource());
  delete winres;
  return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,  DestroyMenu,
              HMENU, hMenu)
{
  return O32_DestroyMenu(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HMENU, GetMenu,
              HWND,  hwnd)
{
  Win32BaseWindow *window;

  window = Win32BaseWindow::GetWindowFromHandle(hwnd);
  if(!window)
  {
    dprintf(("GetMenu, window %x not found", hwnd));
    return 0;
  }

  return window->GetMenu();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,  SetMenu,
              HWND,  hwnd,
              HMENU, hMenu)
{
  Win32BaseWindow *window;

  window = Win32BaseWindow::GetWindowFromHandle(hwnd);
  if(!window)
  {
    dprintf(("SetMenu, window %x not found", hwnd));
    return 0;
  }

  window->SetMenu(hMenu);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(DWORD, GetMenuCheckMarkDimensions)
{
  return O32_GetMenuCheckMarkDimensions();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int,   GetMenuItemCount,
              HMENU, hMenu)
{
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  return OSLibGetMenuItemCount(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT,  GetMenuItemID,
              HMENU, hMenu,
              int,   nPos)
{
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  return O32_GetMenuItemID(hMenu, nPos);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(UINT,  GetMenuState,
              HMENU, hMenu,
              UINT,  arg2,
              UINT,  arg3)
{
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetMenuState(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,   GetMenuStringA,
              HMENU, hMenu,
              UINT,  arg2,
              LPSTR, arg3,
              int,   arg4,
              UINT,  arg5)
{
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetMenuString(hMenu, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,   GetMenuStringW,
              HMENU, hMenu,
              UINT,  idItem,
              LPWSTR,lpsz,
              int,   cchMax,
              UINT,  fuFlags)
{
  char *astring = (char *)malloc(cchMax);
  int   rc;

  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  rc = O32_GetMenuString(hMenu, idItem, astring, cchMax, fuFlags);
  free(astring);
  if(rc)
  {
    dprintf(("USER32: OS2GetMenuStringW %s\n", astring));
             AsciiToUnicode(astring, lpsz);
  }
  else
    lpsz[0] = 0;

  return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, SetMenuItemBitmaps,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              HBITMAP, arg4,
              HBITMAP, arg5)
{
  dprintf(("USER32:  SetMenuItemBitmaps\n"));
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  return O32_SetMenuItemBitmaps(hMenu, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSubMenu,
              HWND, hMenu,
              int, arg2)
{
  if(hMenu == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetSubMenu(hMenu, arg2);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSystemMenu,
              HWND,  hSystemWindow,
              BOOL,  bRevert)
{
  Win32BaseWindow *window;

  window = Win32BaseWindow::GetWindowFromHandle(hSystemWindow);
  if(!window)
  {
    dprintf(("GetSystemMenu, window %x not found", hSystemWindow));
    return 0;
  }

  return O32_GetSystemMenu(window->getOS2FrameWindowHandle(), bRevert);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsMenu,
              HMENU, hMenu)
{
    dprintf(("USER32:  IsMenu\n"));
    return O32_IsMenu(hMenu);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION7(BOOL, TrackPopupMenu,
              HMENU, hMenu,
              UINT, arg2,
              int, arg3,
              int, arg4,
              int, arg5,
              HWND, arg6,
              const RECT *, arg7)
{
    dprintf(("USER32:  TrackPopupMenu\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(hMenu, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(BOOL, TrackPopupMenuEx,
              HMENU, hMenu,
              UINT, flags,
              int, X,
              int, Y,
              HWND, hwnd,
              LPTPMPARAMS, lpPM)
{
 RECT *rect = NULL;

    dprintf(("USER32:  TrackPopupMenuEx, not completely implemented\n"));
    if(lpPM->cbSize != 0)
        rect = &lpPM->rcExclude;

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(hMenu, flags, X, Y, 0, hwnd, rect);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, AppendMenuA,
              HMENU, hMenu,
              UINT, uFlags,
              UINT, ulDNewItem,
              LPCSTR, lpNewItem)
{
 BOOL rc;

    dprintf(("USER32:  OS2AppendMenuA uFlags = %X\n", uFlags));

    if(uFlags & MF_STRING || uFlags == 0)
            dprintf(("USER32:  OS2AppendMenuA %s\n", lpNewItem));

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    rc = O32_AppendMenu(hMenu, uFlags, ulDNewItem, lpNewItem);
    dprintf(("USER32:  AppendMenuA returned %d\n", rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, AppendMenuW,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              LPCWSTR, arg4)
{
    BOOL  rc;
    char *astring = NULL;

    dprintf(("USER32:  OS2AppendMenuW\n"));

    if(arg2 & MF_STRING  && (int)arg4 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg4);
    else
      astring = (char *) arg4;

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    rc = O32_AppendMenu(hMenu, arg2, arg3, astring);
    if(arg2 & MF_STRING  && (int)arg4 >> 16 != 0)
      FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, CheckMenuItem,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    dprintf(("USER32:  OS2CheckMenuItem\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_CheckMenuItem(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreateMenu)
{
    dprintf(("USER32:  CreateMenu\n"));
    return O32_CreateMenu();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreatePopupMenu)
{
    dprintf(("USER32:  CreateMenu\n"));
    return O32_CreatePopupMenu();
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,EnableMenuItem,HMENU,hMenu,
                                  UINT, uIDEnableItem,
                                  UINT, uEnable)
{
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_EnableMenuItem(hMenu,
                              uIDEnableItem,
                              uEnable);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ModifyMenuA,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCSTR, arg5)
{
    dprintf(("USER32:  OS2ModifyMenuA\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_ModifyMenu(hMenu, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ModifyMenuW,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCWSTR, arg5)
{
 BOOL  rc;
 char *astring = NULL;

    dprintf(("USER32: OS2ModifyMenuW %s\n", astring));

    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = O32_ModifyMenu(hMenu, arg2, arg3, arg4, astring);
    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, RemoveMenu,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    dprintf(("USER32:  OS2RemoveMenu\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_RemoveMenu(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, DeleteMenu,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
    dprintf(("USER32:  OS2DeleteMenu\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_DeleteMenu(hMenu, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, HiliteMenuItem,
              HWND, hMenu,
              HMENU, arg2,
              UINT, arg3,
              UINT, arg4)
{
    dprintf(("USER32:  OS2HiliteMenuItem\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_HiliteMenuItem(hMenu, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, InsertMenuA,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCSTR, arg5)
{
    dprintf(("USER32:  OS2InsertMenuA\n"));
    if(hMenu == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_InsertMenu(hMenu, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, InsertMenuW,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3,
              UINT, arg4,
              LPCWSTR, arg5)
{
 BOOL  rc;
 char *astring = NULL;

    dprintf(("USER32:  OS2InsertMenuW %s\n", astring));

    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = O32_InsertMenu(hMenu, arg2, arg3, arg4, astring);
    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetMenuContextHelpId,
              HMENU, hMenu,
              DWORD, dwContextHelpId)
{
  dprintf(("USER32:  OS2SetMenuContextHelpId, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetMenuContextHelpId,
              HMENU, hMenu)
{
  dprintf(("USER32:  OS2GetMenuContextHelpId, not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, CheckMenuRadioItem,
              HMENU, hMenu,
              UINT, idFirst,
              UINT, idLast,
              UINT, idCheck,
              UINT, uFlags)
{
  dprintf(("USER32:  OS2CheckMenuRadioItem, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ChangeMenuA,
              HMENU, hMenu,
              UINT, pos,
              LPCSTR, data,
              UINT, id,
              UINT, flags)
{
    dprintf(("USER32:  ChangeMenuA flags %X\n", flags));

    if (flags & MF_APPEND) return AppendMenuA(hMenu, flags & ~MF_APPEND,
                                             id, data );
    if (flags & MF_DELETE) return DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ModifyMenuA(hMenu, pos, flags & ~MF_CHANGE,
                                             id, data );
    if (flags & MF_REMOVE) return RemoveMenu(hMenu,
                                             flags & MF_BYPOSITION ? pos : id,
                                             flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuA( hMenu, pos, flags, id, data );
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, ChangeMenuW,
              HMENU, hMenu,
              UINT, pos,
              LPCWSTR, data,
              UINT, id,
              UINT, flags)
{
    dprintf(("USER32:  ChangeMenuW flags %X\n", flags));

    if (flags & MF_APPEND) return AppendMenuW(hMenu, flags & ~MF_APPEND,
                                                 id, data );
    if (flags & MF_DELETE) return DeleteMenu(hMenu, pos, flags & ~MF_DELETE);
    if (flags & MF_CHANGE) return ModifyMenuW(hMenu, pos, flags & ~MF_CHANGE,
                                                 id, data );
    if (flags & MF_REMOVE) return RemoveMenu(hMenu,
                                             flags & MF_BYPOSITION ? pos : id,
                                             flags & ~MF_REMOVE );
    /* Default: MF_INSERT */
    return InsertMenuW(hMenu, pos, flags, id, data);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, SetMenuItemInfoA,
              HMENU, hMenu,
              UINT, par1,
              BOOL, par2,
              const MENUITEMINFOA *, lpMenuItemInfo)
{
  dprintf(("USER32:  SetMenuItemInfoA, faked\n"));
  return(TRUE);
}
/*****************************************************************************
 * Function  : SetMenuItemInfoW
 * Purpose   : The SetMenuItemInfo function changes information about a menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetMenuItemInfoW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOW *, lpmmi)
{
  dprintf(("USER32:SetMenuItemInfoW (%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmmi));

  return (SetMenuItemInfoA(hMenu,
                           uItem,
                           fByPosition,
                           (const MENUITEMINFOA *)lpmmi));
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, SetMenuDefaultItem,
              HMENU, hMenu,
              UINT, uItem,
              UINT, fByPos)
{
  dprintf(("USER32:  SetMenuDefaultItem, faked\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, GetMenuItemInfoA,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, aBool,
              MENUITEMINFOA *, lpMenuItemInfo)
{
  dprintf(("USER32:  GetMenuItemInfoA, faked\n"));
  return(TRUE);
}
/*****************************************************************************
 * Function  : GetMenuDefaultItem
 * Purpose   : TheGetMenuDefaultItem function determines the default menu item
 *             on the specified menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the identifier or
 *             position of the menu item.
 *             If the function fails, the return value is - 1.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION3(UINT, GetMenuDefaultItem,
              HMENU, hMenu,
              UINT, fByPos,
              UINT, gmdiFlags)
{
  dprintf(("USER32:GetMenuDefaultItem (%08xh,%u,%08x) not implemented.\n",
         hMenu,
         fByPos,
         gmdiFlags));

  return (-1);
}


/*****************************************************************************
 * Function  : GetMenuItemInfoW
 * Purpose   : The GetMenuItemInfo function retrieves information about a menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, GetMenuItemInfoW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              MENUITEMINFOW *, lpmii)
{
  dprintf(("USER32:GetMenuItemInfoW (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

  return(GetMenuItemInfoA(hMenu,
                          uItem,
                          fByPosition,
                          (MENUITEMINFOA *)lpmii));
}


/*****************************************************************************
 * Function  : GetMenuItemRect
 * Purpose   : The GetMenuItemRect function retrieves the bounding rectangle
 *             for the specified menu item.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, GetMenuItemRect,
              HWND, hWnd,
              HMENU, hMenu,
              UINT, uItem,
              LPRECT, lprcItem)
{
  dprintf(("USER32:GetMenuItemRect (%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         hMenu,
         uItem,
         lprcItem));

  return (FALSE);
}
/*****************************************************************************
 * Function  : InsertMenuItemA
 * Purpose   : The InsertMenuItem function inserts a new menu item at the specified
 *             position in a menu bar or pop-up menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, InsertMenuItemA,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOA*, lpmii)
{
  dprintf(("USER32:InsertMenuItemA (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}


/*****************************************************************************
 * Function  : InsertMenuItemW
 * Purpose   : The InsertMenuItem function inserts a new menu item at the specified
 *             position in a menu bar or pop-up menu.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *             error information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION4(BOOL, InsertMenuItemW,
              HMENU, hMenu,
              UINT, uItem,
              BOOL, fByPosition,
              const MENUITEMINFOW *, lpmii)
{
  dprintf(("USER32:InsertMenuItemW (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}
/*****************************************************************************
 * Function  : MenuItemFromPoint
 * Purpose   : TheMenuItemFromPoint function determines which menu item, if
 *             any, is at the specified location.
 * Parameters:
 * Variables :
 * Result    : Returns the zero-based position of the menu item at the specified
 *             location or -1 if no menu item is at the specified location.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION3(UINT, MenuItemFromPoint,
              HWND, hWnd,
              HMENU, hMenu,
              POINT, ptScreen)
{
  dprintf(("USER32:MenuItemFromPoint (%08xh,%08xh,%u) not implemented.\n",
         hWnd,
         hMenu,
         ptScreen));

  return (-1);
}


/*****************************************************************************
 * Function  :  GetMenuInfo
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB win98/NT5.0
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION2(BOOL, GetMenuInfo,
              HMENU, hMenu,
              LPMENUINFO, lpmi)
{
  dprintf(("USER32: GetMenuInfo(%08xh,%08xh) not implemented.\n",
         hMenu,
         lpmi));

  memset(lpmi,0,sizeof(MENUINFO));
  return 0;
}
#if 0
   POPUPMENU *menu;

    TRACE("(0x%04x %p)\n", hMenu, lpmi);

    if (lpmi && (menu = (POPUPMENU *) USER_HEAP_LIN_ADDR(hMenu)))
    {

   if (lpmi->fMask & MIM_BACKGROUND)
       lpmi->hbrBack = menu->hbrBack;

   if (lpmi->fMask & MIM_HELPID)
       lpmi->dwContextHelpID = menu->dwContextHelpID;

   if (lpmi->fMask & MIM_MAXHEIGHT)
       lpmi->cyMax = menu->cyMax;

   if (lpmi->fMask & MIM_MENUDATA)
       lpmi->dwMenuData = menu->dwMenuData;

   if (lpmi->fMask & MIM_STYLE)
       lpmi->dwStyle = menu->dwStyle;

   return TRUE;
    }
    return FALSE;
}
#endif


/*****************************************************************************
 * Function  :  SetMenuInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * FIXME
 * MIM_APPLYTOSUBMENUS
 * actually use the items to draw the menu
 * Status    : UNTESTED STUB win98/NT5.0
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION2(BOOL, SetMenuInfo,
              HMENU, hMenu,
              LPCMENUINFO, lpmi)
{
  dprintf(("USER32: SetMenuInfo(%08xh,%08xh) not implemented.\n",
         hMenu,
         lpmi));

  return 0;
}
#if 0
    POPUPMENU *menu;

    TRACE("(0x%04x %p)\n", hMenu, lpmi);



    if (lpmi && (lpmi->cbSize==sizeof(MENUINFO)) && (menu=(POPUPMENU*)USER_HEAP_LIN_ADDR(hMenu)))
    {

   if (lpmi->fMask & MIM_BACKGROUND)
       menu->hbrBack = lpmi->hbrBack;

   if (lpmi->fMask & MIM_HELPID)
       menu->dwContextHelpID = lpmi->dwContextHelpID;

   if (lpmi->fMask & MIM_MAXHEIGHT)
       menu->cyMax = lpmi->cyMax;

   if (lpmi->fMask & MIM_MENUDATA)
       menu->dwMenuData = lpmi->dwMenuData;

   if (lpmi->fMask & MIM_STYLE)
       menu->dwStyle = lpmi->dwStyle;

   return TRUE;
    }
    return FALSE;
}
#endif


