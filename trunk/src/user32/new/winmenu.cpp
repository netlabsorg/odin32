/* $Id: winmenu.cpp,v 1.6 1999-09-14 09:56:06 phaller Exp $ */

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
#include <stdlib.h>
#include <string.h>
#include <win32wbase.h>
#include "oslibmenu.h"
#include <winresmenu.h>

//******************************************************************************
//******************************************************************************
HMENU WIN32API LoadMenuA(HINSTANCE hinst, LPCSTR lpszMenu)
{
 HMENU rc;

    rc = (HMENU)FindResourceA(hinst, lpszMenu, RT_MENUA);
    dprintf(("LoadMenuA (%X) returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API LoadMenuW(HINSTANCE hinst, LPCWSTR lpszMenu)
{
 HMENU rc;

    rc = (HMENU)FindResourceW(hinst, lpszMenu, RT_MENUW);
    dprintf(("LoadMenuW (%X) returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Create PM object?
//NOTE: menutemplate strings are always in Unicode format
//******************************************************************************
HMENU WIN32API LoadMenuIndirectA( const MENUITEMTEMPLATEHEADER * menuTemplate)
{
 Win32MenuRes *winres;

    dprintf(("OS2LoadMenuIndirectA\n"));
    winres = new Win32MenuRes((LPVOID)menuTemplate);
    if(winres == NULL) {
        return 0;
    }
    return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API LoadMenuIndirectW(const MENUITEMTEMPLATEHEADER *menuTemplate)
{
 Win32MenuRes *winres;

    dprintf(("OS2LoadMenuIndirectW\n"));
    winres = new Win32MenuRes((LPVOID)menuTemplate);
    if(winres == NULL) {
        return 0;
    }
    return (HMENU)winres;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyMenu(HMENU hmenu)
{
 Win32MenuRes *winres;

    dprintf(("OS2DestroyMenu\n"));
    if(HIWORD(hmenu) == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    winres = (Win32MenuRes *)hmenu;
    delete winres;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API GetMenu( HWND hwnd)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetMenu, window %x not found", hwnd));
        return 0;
    }
    dprintf(("GetMenu %x", hwnd));
    return window->GetMenu();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenu( HWND hwnd, HMENU hmenu)
{
  Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetMenu, window %x not found", hwnd));
        return 0;
    }
    dprintf(("SetMenu %x %x\n", hwnd, hmenu));
    window->SetMenu(hmenu);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMenuCheckMarkDimensions(void)
{
    dprintf(("USER32:  GetMenuCheckMarkDimensions\n"));
    return O32_GetMenuCheckMarkDimensions();
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuItemCount( HMENU hMenu)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetMenuItemCount %x", hMenu));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return OSLibGetMenuItemCount(menu->getOS2Handle());
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMenuItemID( HMENU hMenu, int nPos)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetMenuItemID %x %d\n", hMenu, nPos));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetMenuItemID(menu->getOS2Handle(), nPos);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMenuState(HMENU hMenu, UINT arg2, UINT  arg3)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetMenuState\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetMenuState(menu->getOS2Handle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuStringA( HMENU hMenu, UINT arg2, LPSTR arg3, int arg4, UINT  arg5)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetMenuStringA"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetMenuString(menu->getOS2Handle(), arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetMenuStringW(HMENU hMenu, UINT idItem, LPWSTR lpsz, int cchMax, UINT fuFlags)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;
 char *astring = (char *)malloc(cchMax);
 int   rc;

    dprintf(("USER32:  GetMenuStringW"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    rc = O32_GetMenuString(menu->getOS2Handle(), idItem, astring, cchMax, fuFlags);
    free(astring);
    if(rc) {
                dprintf(("USER32: OS2GetMenuStringW %s\n", astring));
                AsciiToUnicode(astring, lpsz);
    }
    else        lpsz[0] = 0;
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetMenuItemBitmaps( HMENU hMenu, UINT arg2, UINT arg3, HBITMAP arg4, HBITMAP  arg5)
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
HMENU WIN32API GetSubMenu(HWND hMenu, int  arg2)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetSubMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetSubMenu(menu->getOS2Handle(), arg2);
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API GetSystemMenu( HWND hMenu, BOOL  arg2)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  GetSystemMenu\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_GetSystemMenu(menu->getOS2Handle(), arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsMenu( HMENU hMenu)
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
BOOL WIN32API TrackPopupMenu(HMENU hMenu, UINT arg2, int arg3, int arg4, int arg5, HWND arg6, const RECT *  arg7)
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
BOOL WIN32API TrackPopupMenuEx(HMENU hMenu, UINT flags, int X, int Y, HWND hwnd, LPTPMPARAMS lpPM)
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
BOOL WIN32API AppendMenuA(HMENU hMenu, UINT uFlags, UINT ulDNewItem, LPCSTR lpNewItem)
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
BOOL WIN32API AppendMenuW( HMENU hMenu, UINT arg2, UINT arg3, LPCWSTR  arg4)
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
DWORD WIN32API CheckMenuItem( HMENU hMenu, UINT arg2, UINT  arg3)
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
HMENU WIN32API CreateMenu(void)
{
 Win32MenuRes *menu = 0;
 HMENU hMenu;

    hMenu = O32_CreateMenu();
    if(hMenu) {
        menu = new Win32MenuRes(hMenu);
        if(menu == NULL) {
            return 0;
        }
    }
    dprintf(("USER32:  OS2CreateMenu returned %d\n", hMenu));
    return (HMENU)menu;
}
//******************************************************************************
//******************************************************************************
HMENU WIN32API CreatePopupMenu(void)
{
 Win32MenuRes *menu = 0;
 HMENU hMenu;

    dprintf(("USER32:  OS2CreatePopupMenu\n"));
    hMenu = O32_CreatePopupMenu();
    if(hMenu) {
        menu = new Win32MenuRes(hMenu);
        if(menu == NULL) {
            return 0;
        }
    }
    return (HMENU)menu;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableMenuItem( HMENU hMenu, UINT arg2, UINT  arg3)
{
 Win32MenuRes *menu = (Win32MenuRes *)hMenu;

    dprintf(("USER32:  OS2EnableMenuItem\n"));
    if(menu == NULL || menu->getOS2Handle() == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return O32_EnableMenuItem(menu->getOS2Handle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ModifyMenuA( HMENU hMenu, UINT arg2, UINT arg3, UINT arg4, LPCSTR  arg5)
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
BOOL WIN32API ModifyMenuW( HMENU hMenu, UINT arg2, UINT arg3, UINT arg4, LPCWSTR arg5)
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
BOOL WIN32API RemoveMenu( HMENU hMenu, UINT arg2, UINT  arg3)
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
BOOL WIN32API DeleteMenu( HMENU hMenu, UINT arg2, UINT  arg3)
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
BOOL WIN32API HiliteMenuItem( HWND hMenu, HMENU arg2, UINT arg3, UINT  arg4)
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
BOOL WIN32API InsertMenuA( HMENU hMenu, UINT arg2, UINT arg3, UINT arg4, LPCSTR  arg5)
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
BOOL WIN32API InsertMenuW(HMENU hMenu, UINT arg2, UINT arg3, UINT arg4, LPCWSTR arg5)
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
BOOL WIN32API SetMenuContextHelpId(HMENU hmenu, DWORD dwContextHelpId)
{
  dprintf(("USER32:  OS2SetMenuContextHelpId, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetMenuContextHelpId(HMENU hmenu)
{
  dprintf(("USER32:  OS2GetMenuContextHelpId, not implemented\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CheckMenuRadioItem(HMENU hmenu, UINT idFirst, UINT idLast,
                                    UINT idCheck, UINT uFlags)
{
  dprintf(("USER32:  OS2CheckMenuRadioItem, not implemented\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ChangeMenuA(HMENU hMenu, UINT pos, LPCSTR data, UINT id, UINT flags)
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
BOOL WIN32API ChangeMenuW(HMENU hMenu, UINT pos, LPCWSTR data,
                          UINT id, UINT flags )
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
BOOL WIN32API SetMenuItemInfoA(HMENU hmenu, UINT par1, BOOL par2,
                               const MENUITEMINFOA * lpMenuItemInfo)
{
  dprintf(("USER32:  SetMenuItemInfoA, faked\n"));
  return(TRUE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API SetMenuItemInfoW
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

BOOL WIN32API SetMenuItemInfoW(HMENU            hMenu,
                               UINT             uItem,
                               BOOL             fByPosition,
                               const MENUITEMINFOW *lpmmi)
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
BOOL WIN32API SetMenuDefaultItem(HMENU hmenu, UINT uItem, UINT fByPos )
{
  dprintf(("USER32:  SetMenuDefaultItem, faked\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetMenuItemInfoA(HMENU hmenu, UINT uItem, BOOL aBool,
                               MENUITEMINFOA *lpMenuItemInfo )

{
  dprintf(("USER32:  GetMenuItemInfoA, faked\n"));
  return(TRUE);
}
/*****************************************************************************
 * Name      : UINT WIN32API GetMenuDefaultItem
 * Purpose   : The GetMenuDefaultItem function determines the default menu item
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

UINT WIN32API GetMenuDefaultItem(HMENU hMenu,
                                    UINT  fByPos,
                                    UINT  gmdiFlags)
{
  dprintf(("USER32:GetMenuDefaultItem (%08xh,%u,%08x) not implemented.\n",
         hMenu,
         fByPos,
         gmdiFlags));

  return (-1);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetMenuItemInfoW
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

BOOL WIN32API GetMenuItemInfoW(HMENU           hMenu,
                               UINT            uItem,
                               BOOL            fByPosition,
                               MENUITEMINFOW * lpmii)
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
 * Name      : BOOL WIN32API GetMenuItemRect
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

BOOL WIN32API GetMenuItemRect(HWND   hWnd,
                                 HMENU  hMenu,
                                 UINT   uItem,
                                 LPRECT lprcItem)
{
  dprintf(("USER32:GetMenuItemRect (%08xh,%08xh,%08xh,%08x) not implemented.\n",
         hWnd,
         hMenu,
         uItem,
         lprcItem));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API InsertMenuItemA
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

BOOL WIN32API InsertMenuItemA(HMENU           hMenu,
                              UINT            uItem,
                              BOOL            fByPosition,
                              const MENUITEMINFOA* lpmii)
{
  dprintf(("USER32:InsertMenuItemA (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API InsertMenuItemW
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

BOOL WIN32API InsertMenuItemW(HMENU           hMenu,
                              UINT            uItem,
                              BOOL            fByPosition,
                              const MENUITEMINFOW * lpmii)
{
  dprintf(("USER32:InsertMenuItemW (%08xh,%08xh,%u,%08x) not implemented.\n",
         hMenu,
         uItem,
         fByPosition,
         lpmii));

   return (FALSE);
}
/*****************************************************************************
 * Name      : UINT WIN32API MenuItemFromPoint
 * Purpose   : The MenuItemFromPoint function determines which menu item, if
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

UINT WIN32API MenuItemFromPoint(HWND  hWnd,
                                   HMENU hMenu,
                                   POINT ptScreen)
{
  dprintf(("USER32:MenuItemFromPoint (%08xh,%08xh,%u) not implemented.\n",
         hWnd,
         hMenu,
         ptScreen));

  return (-1);
}


/*****************************************************************************
 * Name      : BOOL WIN32API GetMenuInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB win98/NT5.0
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API GetMenuInfo (HMENU hMenu, LPMENUINFO lpmi)
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
 * Name      : BOOL WIN32API SetMenuInfo
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

BOOL WIN32API SetMenuInfo (HMENU hMenu, LPCMENUINFO lpmi)
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

