/* $Id: winmenu.cpp,v 1.6 1999-09-23 17:36:16 phaller Exp $ */

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
  return (HMENU)FindResourceA(hinst, lpszMenu, RT_MENUA);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, LoadMenuW,
              HINSTANCE, hinst,
              LPCWSTR, lpszMenu)
{
  return (HMENU)FindResourceW(hinst, lpszMenu, RT_MENUW);
}
//******************************************************************************
//TODO: Create PM object?
//NOTE: menutemplate strings are always in Unicode format
//******************************************************************************
ODINFUNCTION1(HMENU, LoadMenuIndirectA,
              const MENUITEMTEMPLATEHEADER *, menuTemplate)
{
  Win32MenuRes *winres;

  winres = new Win32MenuRes((LPVOID)menuTemplate);
  if(winres == NULL)
    return 0;
  else
    return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HMENU, LoadMenuIndirectW,
              const MENUITEMTEMPLATEHEADER *, menuTemplate)
{
  Win32MenuRes *winres;

  winres = new Win32MenuRes((LPVOID)menuTemplate);
  if(winres == NULL)
    return 0;
  else
    return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,  DestroyMenu,
              HMENU, hmenu)
{
  Win32MenuRes *winres;

  if(HIWORD(hmenu) == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  winres = (Win32MenuRes *)hmenu;
  delete winres;
  return TRUE;
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
              HMENU, hmenu)
{
  Win32BaseWindow *window;

  window = Win32BaseWindow::GetWindowFromHandle(hwnd);
  if(!window)
  {
    dprintf(("SetMenu, window %x not found", hwnd));
    return 0;
  }

  window->SetMenu(hmenu);
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
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return OSLibGetMenuItemCount(menu->getOS2Handle());
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT,  GetMenuItemID,
              HMENU, hMenu,
              int,   nPos)
{
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetMenuItemID(menu->getOS2Handle(), nPos);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(UINT,  GetMenuState,
              HMENU, hMenu,
              UINT,  arg2,
              UINT,  arg3)
{
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetMenuState(menu->getOS2Handle(), arg2, arg3);
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
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_GetMenuString(menu->getOS2Handle(), arg2, arg3, arg4, arg5);
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
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;
  char *astring = (char *)malloc(cchMax);
  int   rc;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  rc = O32_GetMenuString(menu->getOS2Handle(), idItem, astring, cchMax, fuFlags);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  SetMenuItemBitmaps\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_SetMenuItemBitmaps(menu->getOS2Handle(), arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSubMenu,
              HWND, hMenu,
              int, arg2)
{
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;
  Win32MenuRes *menuReturned;
  HMENU        hMenuReturned;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
     SetLastError(ERROR_INVALID_PARAMETER);
     return 0;
  }

  hMenuReturned = O32_GetSubMenu(menu->getOS2Handle(), arg2);
  /* @@@PH allocate Win32MenuRes object ! */
  /* @@@PH WARNING, memory leak! */
  menuReturned = new Win32MenuRes(hMenuReturned);
  return ((HMENU)menuReturned);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(HMENU, GetSystemMenu,
              HWND,  hSystemWindow,
              BOOL,  bRevert)
{
  Win32MenuRes *menuReturned;
  HMENU        hMenuReturned;
  Win32BaseWindow *window;

  window = Win32BaseWindow::GetWindowFromHandle(hSystemWindow);
  if(!window)
  {
    dprintf(("GetSystemMenu, window %x not found", hSystemWindow));
    return 0;
  }

  //hMenuReturned = window->GetSystemMenu();
  hMenuReturned =  O32_GetSystemMenu(hSystemWindow, bRevert);

  /* @@@PH allocate Win32MenuRes object ! */
  /* @@@PH WARNING, memory leak! */
  menuReturned = new Win32MenuRes(hMenuReturned);
  return ((HMENU)menuReturned);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, IsMenu,
              HMENU, hMenu)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  IsMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_IsMenu(menu->getOS2Handle());
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  TrackPopupMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(menu->getOS2Handle(), arg2, arg3, arg4, arg5, arg6, arg7);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;
 RECT *rect = NULL;


    dprintf(("USER32:  TrackPopupMenuEx, not completely implemented\n"));
    if(lpPM->cbSize != 0)
        rect = &lpPM->rcExclude;

    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_TrackPopupMenu(menu->getOS2Handle(), flags, X, Y, 0, hwnd, rect);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, AppendMenuA,
              HMENU, hMenu,
              UINT, uFlags,
              UINT, ulDNewItem,
              LPCSTR, lpNewItem)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;
 BOOL rc;

    dprintf(("USER32:  OS2AppendMenuA uFlags = %X\n", uFlags));

    if(uFlags & MF_STRING || uFlags == 0)
            dprintf(("USER32:  OS2AppendMenuA %s\n", lpNewItem));

    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    rc = O32_AppendMenu(menu->getOS2Handle(), uFlags, ulDNewItem, lpNewItem);
    dprintf(("USER32:  OS2AppendMenuA returned %d\n", rc));
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
    Win32MenuRes *menu = (Win32MenuRes *)hMenu;
    BOOL  rc;
    char *astring = NULL;

    dprintf(("USER32:  OS2AppendMenuW\n"));

    if(arg2 & MF_STRING  && (int)arg4 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg4);
    else
      astring = (char *) arg4;

    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    rc = O32_AppendMenu(menu->getOS2Handle(), arg2, arg3, astring);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2CheckMenuItem\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_CheckMenuItem(menu->getOS2Handle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreateMenu)
{
  Win32MenuRes *menu = 0;
  HMENU hMenu;

  hMenu = O32_CreateMenu();
  if(hMenu)
  {
    menu = new Win32MenuRes(hMenu);
    if(menu == NULL)
      return 0;
  }

  return (HMENU)menu;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HMENU, CreatePopupMenu)
{
  Win32MenuRes *menu = 0;
  HMENU hMenu;

  hMenu = O32_CreatePopupMenu();
  if(hMenu)
  {
    menu = new Win32MenuRes(hMenu);
    if(menu == NULL)
      return 0;
  }

  return (HMENU)menu;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL,EnableMenuItem,HMENU,hMenu,
                                  UINT, uIDEnableItem,
                                  UINT, uEnable)
{
  Win32MenuRes *menu = (Win32MenuRes *)hMenu;

  if(menu == NULL || menu->getOS2Handle() == 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }

  return O32_EnableMenuItem(menu->getOS2Handle(),
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2ModifyMenuA\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_ModifyMenu(menu->getOS2Handle(), arg2, arg3, arg4, arg5);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32: OS2ModifyMenuW %s\n", astring));

    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = O32_ModifyMenu(menu->getOS2Handle(), arg2, arg3, arg4, astring);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2RemoveMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_RemoveMenu(menu->getOS2Handle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(BOOL, DeleteMenu,
              HMENU, hMenu,
              UINT, arg2,
              UINT, arg3)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2DeleteMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_DeleteMenu(menu->getOS2Handle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, HiliteMenuItem,
              HWND, hMenu,
              HMENU, arg2,
              UINT, arg3,
              UINT, arg4)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2HiliteMenuItem\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_HiliteMenuItem(menu->getOS2Handle(), arg2, arg3, arg4);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2InsertMenuA\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return O32_InsertMenu(menu->getOS2Handle(), arg2, arg3, arg4, arg5);
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
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2InsertMenuW %s\n", astring));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      astring = UnicodeToAsciiString((LPWSTR)arg5);
    else
      astring = (char *) arg5;

    rc = O32_InsertMenu(menu->getOS2Handle(), arg2, arg3, arg4, astring);
    if(arg3 & MF_STRING  && (int)arg5 >> 16 != 0)
      FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, SetMenuContextHelpId,
              HMENU, hmenu,
              DWORD, dwContextHelpId)
{
  dprintf(("USER32:  OS2SetMenuContextHelpId, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, GetMenuContextHelpId,
              HMENU, hmenu)
{
  dprintf(("USER32:  OS2GetMenuContextHelpId, not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(BOOL, CheckMenuRadioItem,
              HMENU, hmenu,
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
              HMENU, hmenu,
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
              HMENU, hmenu,
              UINT, uItem,
              UINT, fByPos)
{
  dprintf(("USER32:  SetMenuDefaultItem, faked\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL, GetMenuItemInfoA,
              HMENU, hmenu,
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


